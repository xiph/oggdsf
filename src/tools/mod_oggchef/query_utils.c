/*
   Copyright (C) 2003-2005 Commonwealth Scientific and Industrial Research
   Organisation (CSIRO) Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of CSIRO Australia nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "query_utils.h"

#include "httpd.h"
#ifdef WIN32
# undef strtoul /* Otherwise Visual Studio .NET 2003 complains */
#endif
#include "apr_strings.h"


/**
 * Determine the relative quality factor of a mime type given the Accept:
 * header of a resource request, following rules of RFC2616 Sec. 14.1
 * @param r The resource request
 * @param content_type The content_type to check
 * @return The relative quality factor
 */
float get_accept_quality (request_rec * r, char * content_type)
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


/**
 * Create a table corresponding to name=value pairs in the query string
 * @param r The resource request
 * @param query The query string
 * @return A newly created table with corresponding name=value keys.
 */
apr_table_t *make_cgi_table (request_rec * r, char * query)
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

