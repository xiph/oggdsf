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

struct my_data {
  char * filename;
  long interesting_serialno;
  int interesting_raw_packets;
  int done;
};

static int
read_stream (ANNODEX * anx, double timebase, char * utc, void * user_data)
{
  struct my_data * happy = (struct my_data *) user_data;

  printf ("Welcome to %s! The timebase is %f\n", happy->filename, timebase);
  return ANX_CONTINUE;
}

static int
read_track (ANNODEX * anx, long serialno, char * id, char * content_type,
	    anx_int64_t granule_rate_n, anx_int64_t granule_rate_d,
	    int nr_header_packets, void * user_data)
{
  struct my_data * happy = (struct my_data *) user_data;

  /* Ignore the annotations track, we don't find it interesting! */
  if (!strncmp (content_type, "text/x-cmml", 12)) return ANX_CONTINUE;

  printf ("Our first track has content-type %s and granule rate %ld/%ld.\n",
	  content_type, (long)granule_rate_n, (long)granule_rate_d);

  printf ("We will remember it by its serial number %ld "
	  "and mark it with crosses.\n", serialno);
  happy->interesting_serialno = serialno;

  /* We don't care about any other tracks! */
  anx_set_read_track_callback (anx, NULL, NULL);

  return ANX_CONTINUE;
}

static int
read_raw (ANNODEX * annodex, unsigned char * buf, long n,
	    long serialno, anx_int64_t granulepos, void * user_data)
{
  struct my_data * happy = (struct my_data *) user_data;

  if (happy->done) {
    putchar ('!');
  } else if (serialno == happy->interesting_serialno) {
    happy->interesting_raw_packets++;
    putchar ('+');
  } else {
    putchar ('.');
  }

  return ANX_CONTINUE;
}

static int
read_clip3 (ANNODEX * anx, const AnxClip * clip, void * user_data)
{
  struct my_data * happy = (struct my_data *) user_data;

  printf ("\nAnd the third clip links to %s\n", clip->anchor_href);

  happy->done = 1;

  printf ("This completes our whirlwind tour of the first three clips!\n");
  return ANX_STOP_OK;
}

static int
read_clip2 (ANNODEX * anx, const AnxClip * clip, void * user_data)
{
  printf ("\nThe second clip links to %s\n", clip->anchor_href);
  anx_set_read_clip_callback (anx, read_clip3, user_data);
  return ANX_CONTINUE;
}

static int
read_clip1 (ANNODEX * anx, const AnxClip * clip, void * user_data)
{
  printf ("\nThe first clip links to %s\n", clip->anchor_href);
  anx_set_read_clip_callback (anx, read_clip2, user_data);
  return ANX_CONTINUE;
}

int
main (int argc, char *argv[])
{
  ANNODEX * anx = NULL;
  struct my_data me;
  long n;

  if (argc != 2) {
    fprintf (stderr, "Usage: %s file.anx\n", argv[0]);
    exit (1);
  }

  me.filename = argv[1];
  me.interesting_serialno = -1;
  me.interesting_raw_packets = 0;
  me.done = 0;

  anx = anx_open (me.filename, ANX_READ);

  anx_set_read_stream_callback (anx, read_stream, &me);
  anx_set_read_track_callback (anx, read_track, &me);
  anx_set_read_raw_callback (anx, read_raw, &me);
  anx_set_read_clip_callback (anx, read_clip1, &me);

  while (!me.done && (n = anx_read (anx, 1024)) > 0);

  printf ("%d packets from the first track (serialno %ld) were received\n",
	  me.interesting_raw_packets, me.interesting_serialno);
  printf ("before the third clip.\n");

  anx_close (anx);

  exit (0);
}
