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

/**
 * \file 
 * cmml-parse:
 *
 * A simple example program to show how to parse a CMML instance
 * document and print out the title attribute of the head tag. Error
 * handling is ignored.
 */

/** the reading buffer's size */
#define BUFSIZE 100000


/**
 * read_head: the callback for a head element
 *
 * \param cmml the CMML* handle in use
 * \param head the head element's content represented in a
 *        CMML_Head*
 * \param user_data user defined data
 *
 * \returns 0 on success, 1 on error
 */
static int
read_head (CMML * cmml, const CMML_Head * head, void * user_data) {
  puts(head->title);
  return 0;
}

/**
 * main function of cmml-parse, which opens the CMML file, registers
 * the callbacks, and then steps through the file in chunks of BUFSIZE
 * size, during which the callbacks get activated as the relevant
 * elements get parsed.
 */
int main(int argc, char *argv[])
{
  char *filename = NULL;
  FILE * cmml_file;
  CMML * doc;
  long n = 0;

  if (argc != 2) {
    fprintf (stderr, "Usage: %s <CMMLfile>\n", argv[0]);
    exit (1);
  }
  filename = argv[1];
 
  cmml_file = fopen(filename, "r");
  doc = cmml_new(cmml_file);

  /* alternatively use:
   *  doc = cmml_open(filename);
   */

  cmml_set_read_callbacks (doc, NULL, read_head, NULL, NULL);

  while (((n = cmml_read (doc, BUFSIZE)) > 0));

  cmml_file = cmml_destroy(doc);
  fclose(cmml_file);
  
  /* alternatively use:
   *  cmml_close(doc);
   */

  exit(0);
}
