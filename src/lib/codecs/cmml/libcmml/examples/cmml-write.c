/* Copyright (C) 2003 CSIRO Australia

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
#include <string.h>

#include <cmml.h>

/**
 * \file 
 * cmml-write:
 *
 * A simple example program to show how to create a CMML instance
 * document. Error handling is ignored.
 */

/** the reading buffer's size */
#define BUFSIZE 100000

/**
 * main function of cmml-write, which writes a CMML file.
 */
int main(int argc, char *argv[])
{
  CMML_Preamble * pre = NULL;
  CMML_Head * head = NULL;
  CMML_Clip * clip = NULL;
  CMML_Time * start = NULL, * end = NULL;
  char buf[BUFSIZE];

  /* write preamble */
  pre = cmml_preamble_new("UTF-8", NULL, NULL, NULL);
  cmml_preamble_snprint (buf, BUFSIZE, pre);
  puts(buf);
  cmml_preamble_destroy(pre);

  /* write head */
  head = cmml_head_new();
  head->title = strdup("This is a test file");
  cmml_head_snprint (buf, BUFSIZE, head);
  puts(buf);
  cmml_head_destroy(head);

  /* write a clip for the interval 0.6 to 3.5 seconds */
  cmml_npt_snprint (buf, BUFSIZE, 0.6);
  start = cmml_time_new (buf);

  cmml_npt_snprint (buf, BUFSIZE, 3.5);
  end = cmml_time_new (buf);

  clip = cmml_clip_new(start, end);
  clip->anchor_href = strdup("http://www.annodex.net/");
  clip->anchor_text = strdup("hyperlink to annodex.net");
  clip->desc_text = strdup("This is a clip");
  cmml_clip_snprint (buf, BUFSIZE, clip);
  puts (buf);
  cmml_clip_destroy(clip);

  printf ("</cmml>\n");

  exit(0);
}
