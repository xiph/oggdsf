/**
   Copyright (C) 2003 Commonwealth Scientific and Industrial Research
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

#include <stdio.h>
#include <string.h>

#include <annodex/annodex.h>

int
main (int argc, char *argv[])
{
  ANNODEX * anx = NULL;
  AnxClip my_clip;
  char * infilename, * outfilename;
  long n;

  if (argc != 3) {
    fprintf (stderr, "Usage: %s infile outfile.anx\n", argv[0]);
    exit (1);
  }

  /* Load all importers */
  anx_init_importers ("*/*");

  infilename = argv[1];
  outfilename = argv[2];

  /* Create an ANNODEX* writer, writing to outfilename */
  anx = anx_open (outfilename, ANX_WRITE);

  /* Import infilename into the writer */
  anx_write_import (anx, infilename, NULL /* id */,
		    NULL /* unknown content-type */,
		    0 /* seek_offset */, -1 /* seek_end */, 0 /* flags */);

  /* Insert a clip starting at time 0 */
  memset (&my_clip, 0, sizeof (AnxClip));
  my_clip.anchor_href = "http://www.annodex.net/";
  my_clip.anchor_text = "Find out about Annodex media";

  anx_insert_clip (anx, 0, &my_clip);

  /* End the clip at time 2.0 seconds */
  anx_insert_clip (anx, 2.0, NULL);

  while ((n = anx_write (anx, 1024)) > 0);

  anx_close (anx);

  exit (0);
}
