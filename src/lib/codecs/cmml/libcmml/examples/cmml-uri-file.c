/** Copyright (C) 2003 CSIRO Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the CSIRO nor the names of its
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

#include <stdio.h>

#include <cmml.h>
#include <string.h>

/**
 * \file 
 * cmml-uri-file-open:
 *
 * A simple example program to show how to open a CMML file given
 * through a file: uri and print out the desc tags of the clip
 * tag. Error handling is ignored.
 */

/** the reading buffer's size */
#define BUFSIZE 100000

/** a struct for holding basic URI parts */
typedef struct {
  char *scheme;    /**< the URI scheme to work with, here file: */
  char *authority; /**< the scheme's namespace authority, here not used */
  char *path;      /**< the path parts including filename */
  char *querystr;  /**< the query string, here not used XXX */
  char *fragstr;   /**< the fragment offset */
} URI;
 

/**
 * parse_file_uri: a rough function to parse a uri string into its
 * components
 *
 * \param uri_string the given uri
 *
 * \returns a URI structure containing the different parsed components
 */
static URI *
parse_file_uri (const char *uri_string)
{
  const char *location;
  const char *locbegin;
  int length;
  URI *result;
  locbegin = uri_string;
  result = (URI*) calloc(sizeof(URI), sizeof(char));

  /* ignore file:// and authority parts to get path */
  location = strstr (locbegin, "://");
  locbegin = location+3;
  length = strlen(locbegin);
  location = strchr(locbegin, '#'); /* XXX: ignore queries for the moment */
  if (location != NULL) length = location - locbegin;
  result->path = (char*) calloc (length+1, sizeof(char));
  result->path = strncpy(result->path, locbegin, length);
  result->path[length] = '\0';

  if (location != NULL) { 
    /* fragment given */
    length = strlen(location);
    result->fragstr = NULL;
    result->fragstr = (char*) calloc (length, sizeof(char));
    result->fragstr = strncpy(result->fragstr, location+1, length);
  } else {
    result->fragstr = NULL;
  }
  return result;
}

/**
 * read_clip: the callback for a clip element
 *
 * \param cmml the CMML* handle in use
 * \param clip the clip element's content represented in a
 *        CMML_Clip*
 * \param user_data user defined data
 *
 * \returns 0 on success, 1 on error
 */
static int
read_clip (CMML * cmml, const CMML_Clip * clip, void * user_data) {
  puts(clip->desc_text);
  return 0;
}

/**
 * main function of cmml-uri-file-open, which opens the CMML file
 * given through a file uri, checks any offsets, registers the
 * callbacks, and then steps through the file in chunks of BUFSIZE
 * size, during which the callbacks get activated as the relevant
 * elements get parsed. 
 *
 * Fragments may be given to the uri in one of two forms:
 * - #name - name is the value of an clip's id tag
 * - #clock:YYYYMMDDThhmmss.ffZ - this is a utc time; npt or smpte also possible
 */
int main(int argc, char *argv[])
{
  char *uri_string = NULL;
  URI * uri;
  CMML * doc;
  long n = 0;

  if (argc < 2) {
    fprintf (stderr, "Usage: %s <file://filename#fragment>\n", argv[0]);
    exit (1);
  }
  uri_string = argv[1];
 
  uri = parse_file_uri(uri_string);

  doc = cmml_open(uri->path);

  /* if fragment given, forward to that */
  if (uri->fragstr != NULL) cmml_skip_to_offset(doc, uri->fragstr);

  cmml_set_read_callbacks (doc, NULL, NULL, read_clip, NULL);

  while (((n = cmml_read (doc, BUFSIZE)) > 0));
  
  cmml_close(doc);

  exit(0);
}
