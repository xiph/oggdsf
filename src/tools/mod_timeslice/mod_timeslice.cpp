// ===========================================================================
// Copyright (C) 2004-2005  Zentaro Kavanagh
// Copyright (C) 2004-2005  Commonwealth Scientific and Industrial Research
//                          Organisation (CSIRO) Australia
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// - Neither the name of Zentaro Kavanagh, CSIRO Australia nor the names of
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ===========================================================================

#include "anx_time.h"
#include "apr_stdcall.h"

#include "httpd.h"
#ifdef WIN32
# undef strtoul /* Otherwise Visual Studio .NET 2003 complains */
#endif
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "apr_strings.h"

#include <stdio.h>
#include <string.h>

#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>
#include <libOOOggSeek/AutoAnxSeekTable.h>

#include <iostream>
#include <fstream>



/* Note: This file's pretty ugly at the moment, it's basically a merge of the
   mod_annodex_ap20 source code and AnxCutter.  Don't worry, it will be tidied
   up :)
 */

#define DEBUG

#define ANX_MIME_TYPE "application/x-annodex"
#define CMML_MIME_TYPE "text/x-cmml"

#define CMML_PREAMBLE \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \

#define MOD_ANNODEX_TYPE_UNKNOWN -1

#define MOD_ANNODEX_TYPE_ANX 0
#define MOD_ANNODEX_TYPE_CMML 1

#define MEDIA_BUF_LEN 8192


/**
 * Create a table corresponding to name=value pairs in the query string
 * @param r The resource request
 * @param query The query string
 * @return A newly created table with corresponding name=value keys.
 */
static apr_table_t *
make_cgi_table (request_rec * r, char * query)
{
  apr_table_t * t;
  char * key, * val, * end;

  t = apr_table_make (r->pool, 3);

  if (!query) return t;

  key = query;

  do {
    val = strchr (key, '=');
    end = strchr (key, '&');

    if (end) {
      if (val) {
        if (val < end) {
          *val++ = '\0';
        } else {
          val = NULL;
        }
      }
      *end++ = '\0';
    } else {
      if (val) *val++ = '\0';
    }

    /*ap_rprintf (r, "%s = %s\n", key, val);*/
    apr_table_set (t, key, val);

    key = end;

  } while (end != NULL);

  return t;
}

/**
 * Determine the relative quality factor of a mime type given the Accept:
 * header of a resource request, following rules of RFC2616 Sec. 14.1
 * @param r The resource request
 * @param content_type The content_type to check
 * @return The relative quality factor
 */
static float
get_accept_quality (request_rec * r, char * content_type)
{
  char * a, * accept, *next, * last, * pnext, * plast;
  float q = 0.0, type_q = 0.0, all_q = 0.0;
  char * m_sep, * m_major;
  apr_size_t m_major_len;

  a = (char *)apr_table_get (r->headers_in, (const char *)"Accept");

  /* If there was no Accept: header, accept all types equally */
  if (a == NULL) return 1.0;

  /* Form a 'major/*' mime type range for later comparison */
  m_sep = strchr (content_type, '/');
  m_major_len = (apr_size_t)(m_sep - content_type);
  m_major = apr_pstrndup (r->pool, content_type, m_major_len + 2);
  *(m_major+m_major_len+1) = '*';
  *(m_major+m_major_len+2) = '\0';

  /* Copy the Accept line for tokenization */
  accept = apr_pstrdup (r->pool, a);

  apr_collapse_spaces (accept, accept);

  next = apr_strtok (accept, ",", &last);
  while (next) {
    pnext = apr_strtok (next, ";", &plast);

    if (!strcmp (pnext, content_type)) {
      while (pnext) {
	pnext = apr_strtok (NULL, ";", &plast);
	if (pnext && sscanf (pnext, "q=%f", &q) == 1) {
	  return q;
	}
      }
      return 1.0;
    } else if (!strcmp (pnext, "*/*")) {
      while (pnext) {
	pnext = apr_strtok (NULL, ";", &plast);
	if (pnext && sscanf (pnext, "q=%f", &q) == 1) {
	  all_q = q;
	}
      }
      all_q = 1.0;
    } else if (!strcmp (pnext, m_major)) {
      while (pnext) {
	pnext = apr_strtok (NULL, ";", &plast);
	if (pnext && sscanf (pnext, "q=%f", &q) == 1) {
	  type_q = q;
	}
      }
      type_q = 1.0;
    }
    next = apr_strtok (NULL, ",", &last);
  }

  if (q > 0.0) return q;
  else if (type_q > 0.0) return type_q;
  else return all_q;
}


enum eDemuxState {
	SEEN_NOTHING,
	SEEN_ANNODEX_BOS,
	SEEN_ANNODEX_EOS,
	SEEN_ALL_CODEC_HEADERS,
	INVALID = 100
};

eDemuxState demuxState;

typedef pair<unsigned long, unsigned long> tSerial_HeadCountPair;

unsigned long bytePos;

bool gotAllHeaders;

vector<tSerial_HeadCountPair> theStreams;

unsigned long annodexSerialNo;

using namespace std;

unsigned long headerCount(OggPacket* inPacket)
{
  const unsigned short NUM_SEC_HEADERS_OFFSET = 24;

  return iLE_Math::charArrToULong(inPacket->packetData() +
                                  NUM_SEC_HEADERS_OFFSET);

}

request_rec *theRequest;

bool writePageToOutputFile(OggPage* inOggPage) {
  // outputFile.write((char*)inOggPage->createRawPageData(), inOggPage->pageSize());
  ap_rwrite((char*)inOggPage->createRawPageData(),
            inOggPage->pageSize(),
	    theRequest);
  return true;
}

//This will be called by the callback
bool pageCB(OggPage* inOggPage, void *)
{

	bool allEmpty = true;

	switch (demuxState) {

		case SEEN_NOTHING:
			if (		(inOggPage->numPackets() == 1)
					&&	(inOggPage->header()->isBOS())
					&&	(strncmp((char*)inOggPage->getPacket(0)->packetData(), "Annodex\0", 8) == 0)) {
				
				//Advance the state
				demuxState = SEEN_ANNODEX_BOS;

				//Remember the annodex streams serial no
				annodexSerialNo = inOggPage->header()->StreamSerialNo();

				//Write out the page.
				writePageToOutputFile(inOggPage);
			} else {

				demuxState = INVALID;
			}

			break;
		case SEEN_ANNODEX_BOS:
			if (		(inOggPage->numPackets() == 1)
					&&	(inOggPage->header()->isBOS())
					&&	(strncmp((char*)inOggPage->getPacket(0)->packetData(), "AnxData\0", 8) == 0)) {
				

				//Create an association of serial no and num headers
				tSerial_HeadCountPair locMap;
				locMap.first = inOggPage->header()->StreamSerialNo();
				locMap.second = headerCount(inOggPage->getPacket(0));
				
				//Add the association to the list
				theStreams.push_back(locMap);

				//Write the page out to the output file.
				writePageToOutputFile(inOggPage);
			} else if (			(inOggPage->header()->isEOS())
							&&	(inOggPage->header()->StreamSerialNo() == annodexSerialNo)) {

				//It's the Annodex EOS.
				demuxState = SEEN_ANNODEX_EOS;
				writePageToOutputFile(inOggPage);
			} else {
				demuxState = INVALID;
			}
			break;
		case SEEN_ANNODEX_EOS:
			for (unsigned int i = 0; i < theStreams.size(); i++) {
				if (theStreams[i].first == inOggPage->header()->StreamSerialNo()) {
					if (theStreams[i].second >= 1) {
						theStreams[i].second--;
						writePageToOutputFile(inOggPage);
					} else {
						demuxState = INVALID;
					}
				}
			}

			
			for (unsigned int i = 0; i < theStreams.size(); i++) {
				if (theStreams[i].second != 0) {
					allEmpty = false;
				}
			}

			if (allEmpty) {

				demuxState = SEEN_ALL_CODEC_HEADERS;
			}
			break;
		case SEEN_ALL_CODEC_HEADERS:
			break;
		case INVALID:
			break;
		default:
			break;
	}


	if (demuxState == INVALID) {
	}
	// delete inOggPage;

	return true;
}

static int
ma_anxenc (request_rec * r, char * filename, char * content_type,
	   apr_table_t * cgi_table)
{
  char * val;
  double seek_offset = 0.0;

  theRequest = r;

  /* put the requested time into seek_offset */
  val = (char *)apr_table_get (cgi_table, "t");

  if (!val)
  {
	fstream inputFile;
	inputFile.open(filename, ios_base::in | ios_base::binary);
	for (;;)
	{
	  const unsigned short BUFF_SIZE = 8092;
	  char* locBuff = new char[BUFF_SIZE];

	  inputFile.read(locBuff, BUFF_SIZE);
	  unsigned long locBytesRead = inputFile.gcount();
	  if (locBytesRead == 0) break;
	  ap_rwrite (locBuff, locBytesRead, r);
	}
	inputFile.close();

	return 0;
  }

  seek_offset = anx_parse_time (val);

  /* begin AnxCutter code */
  demuxState = SEEN_NOTHING;
  bytePos = 0;
  gotAllHeaders = false;
  annodexSerialNo = 0;

  OggDataBuffer testOggBuff;
  testOggBuff.registerStaticCallback(&pageCB, NULL);
	
  /* rip out the headers from the file */

  fstream inputFile;
		
  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                "ma_anxenc t=%s (%ld)\n", val, seek_offset);

  inputFile.open(filename, ios_base::in | ios_base::binary);

  char* locBuff = new char[MEDIA_BUF_LEN];
  while (demuxState < SEEN_ALL_CODEC_HEADERS) {
    inputFile.read(locBuff, MEDIA_BUF_LEN);
    unsigned long locBytesRead = inputFile.gcount();
	if (locBytesRead > 0)
	{
	  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                "feeding %ld bytes to testOggBuff", locBytesRead);
	}
    testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
  }
  inputFile.close();

  // Build a seek table for the file
  AutoAnxSeekTable *locSeekTable = new AutoAnxSeekTable(filename);
  locSeekTable->buildTable();
	
  // Seek to the user's requested start time
  LOOG_UINT64 locStartTime = (LOOG_UINT64) seek_offset * 10000000;
  OggSeekTable::tSeekPair locSeekResult = locSeekTable->getStartPos(locStartTime);
	
  // Stream-copy everything from the requested timepoint onward to the output file
  inputFile.open(filename, ios_base::in | ios_base::binary);
  inputFile.seekg(locSeekResult.second);

  for (;;) {
    inputFile.read(locBuff, MEDIA_BUF_LEN);
    unsigned long locBytesRead = inputFile.gcount();
    if (locBytesRead == 0) break;
    //outputFile.write(locBuff, locBytesRead);
    ap_rwrite (locBuff, locBytesRead, r);
  }
  inputFile.close();

  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                "Served request");
}


extern "C" {

/* The annodex content handler */
static int AP_MODULE_ENTRY_POINT annodex_handler(request_rec *r)
{
  apr_uri_t * uri;
  char * filename;
  apr_table_t * cgi_table;

	   ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                  "Got here 2");

  uri = &(r->parsed_uri);

  /* usually filename is request filename */
  filename = r->filename;

	   ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                  "filename is %s", filename);

  cgi_table = make_cgi_table (r, uri->query);

  ma_anxenc (r, r->filename, ANX_MIME_TYPE, cgi_table);

  return OK;
}

static void AP_MODULE_ENTRY_POINT annodex_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(AP_HOOK_HANDLER_FUNCTION(annodex_handler),
		            NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA timeslice_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    AP_REGISTER_HOOK_FUNCTION(annodex_register_hooks)  /* register hooks */
};

} /* extern "C" */
