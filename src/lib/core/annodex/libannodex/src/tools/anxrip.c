/*
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

/** \file
 * anxrip.c -- Utility to extract data from Annodex media
 */

#include "config.h"
#include "anx_compat.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include <annodex/annodex.h>

#include "common.h"

/** Preamble to prefix CMML output */
#define CMML_PREAMBLE \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
"<!DOCTYPE cmml SYSTEM \"cmml.dtd\">\n"

/**
 * Print usage information.
 */
static void
usage (void)
{
  printf ("\nUsage: anxrip [options] file\n");
  printf ("Extract (rip) data from annodexed media.\n\n");
  printf ("CMML options\n");
  printf ("  -c, --cmml        Extract annotations as a valid CMML file (default)\n");
  printf ("  --head            Extract only the CMML head element\n");
  printf ("  -g, --granulerate Include information about granulerates\n");

  printf ("\nData track options\n");
#if 0
  printf ("  -s serialno, --serialno serialno\n");
  printf ("                    Extract data track with specified serial number from annodexed media\n");
  printf ("  -t content-type, --type content-type\n");
  printf ("                    Extract data track of specified content-type from annodexed media\n");
#endif
  printf ("  -r, --raw         Extract data tracks as raw output\n");
#if 0
  printf ("  -a, --annodex     Extract data tracks as annodexed media\n");
  printf ("  -x, --xiph        Extract data tracks as Ogg files (Xiph.Org)\n\n");
#endif
  printf ("\nOutput options\n");
  printf ("  -o filename, --output filename\n");
  printf ("                    Specify output filename (default is stdout)\n");

  printf ("\nMiscellaneous options\n");
  printf ("  -h, --help        Display this help and exit\n");
  printf ("  --version         Output version information and exit\n\n");
}

static FILE * outfile;
static char buf[102400];

static int print_granulerate = 0;

static int
fprintf_time_npt (double seconds)
{
  int hrs, min;
  double sec;
  char * sign;

  sign = (seconds < 0.0) ? "-" : "";

  if (seconds < 0.0) seconds = -seconds;

  hrs = (int) (seconds/3600.0);
  min = (int) ((seconds - ((double)hrs * 3600.0)) / 60.0);
  sec = seconds - ((double)hrs * 3600.0)- ((double)min * 60.0);

  /* XXX: %02.3f workaround */
  if (sec < 10.0) {
    return fprintf (outfile, "%s%02d:%02d:0%2.3f", sign, hrs, min, sec);
  } else {
    return fprintf (outfile, "%s%02d:%02d:%02.3f", sign, hrs, min, sec);
  }
}

#if 0
static int
read_stream (ANNODEX * anx, double timebase, char * utc, void * user_data)
{
  fprintf (outfile, "<stream timebase=\"");
  fprintf_time_npt (timebase);
  fprintf (outfile, "\">\n"); /* XXX: and UTC */

  return 0;
}

static int
read_track (ANNODEX * anx, long serialno, char * id, char * content_type,
	    anx_int64_t granule_rate_n, anx_int64_t granule_rate_d,
	    int nr_header_packets, void * user_data)
{
  if (!(content_type && !strncmp (content_type, "text/x-cmml", 12))) {
    fprintf (outfile, "<import");
    if (id) fprintf (outfile, " id=\"%s\"", id);
    if (content_type) fprintf (outfile, " content-type=\"%s\"", content_type);
    if (print_granulerate && granule_rate_n != 0 && granule_rate_d != 0)
      fprintf (outfile, " granulerate=\"%ld/%ld\"",
	       (long)granule_rate_n, (long)granule_rate_d);
    fprintf (outfile, "/>\n");
  }
  
  return 0;
}
#endif

static int
read_head (ANNODEX * anx, const AnxHead * head, void * user_data)
{
  anx_head_snprint (buf, 102400 , (AnxHead *)head);
  fputs (buf, outfile);
  fputc ('\n', outfile);
  
  return 0;
}

static int
read_clip (ANNODEX * anx, const AnxClip * clip, void * user_data)
{
  double t;

  t = anx_tell_time (anx);
  /*  fprintf (outfile, "%.3f (%ld)\t", t, anx_tell (anx));*/

  anx_clip_snprint (buf, 102400, (AnxClip *)clip, t, -1.0);
  fputs (buf, outfile);
  fputc ('\n', outfile);
  return 0;
}

static int
read_raw (ANNODEX * anx, unsigned char * data, long n, long serialno,
	    anx_int64_t granulepos, void * user_data)
{
  fwrite (data, 1, n, outfile);
  return 0;
}


static int
print_tracks (ANNODEX * anx)
{
  AnxList * l;
  AnxTrack * s;

  fprintf (outfile, "<stream timebase=\"");
  fprintf_time_npt (anx_get_timebase (anx));
  fprintf (outfile, "\">\n"); /* XXX: and UTC */

  for (l = anx_get_track_list (anx); l; l = l->next) {
    s = (AnxTrack *)l->data;

    if (!(s->content_type && !strncmp (s->content_type, "text/x-cmml", 14))) {
      fprintf (outfile, "<import");
      if (s->id) fprintf (outfile, " id=\"%s\"", s->id);
      if (s->content_type) fprintf (outfile, " contenttype=\"%s\"",
				    s->content_type);
      if (print_granulerate && s->granule_rate_n != 0 &&
	  s->granule_rate_d != 0)
	fprintf (outfile, " granulerate=\"%ld/%ld\"",
		 (long)s->granule_rate_n, (long)s->granule_rate_d);
      fprintf (outfile, "/>\n");
    }
  }

  fprintf (outfile, "</stream>\n");

  return 0;
}

/**
 * Program entry
 */
int
main (int argc, char *argv[])
{
  int show_version = 0;
  int show_help = 0;
  int i;

  int rip_raw = 0;
  int head_only = 0;

  char * infilename = NULL;
  char * outfilename = NULL;

  ANNODEX * anx = NULL;
  long n;

  set_progname (argv[0]);

#define OPT(s) (strcmp (argv[i], (s)) == 0)
    
  /* Parse options; modify behaviour according to user-specified options */
  for (i=1; i < argc; i++) {
    if (OPT("--help") || OPT("-h")) {
      show_help = 1;
    } else if (OPT("--version")) {
      show_version = 1;
    } else if (OPT("--raw") || OPT("-r")) {
      rip_raw = 1;
      head_only = 0;
    } else if (OPT("--cmml") || OPT("-c")) {
      rip_raw = 0;
      head_only = 0;
    } else if (OPT("--head")) {
      rip_raw = 0;
      head_only = 1;
    } else if (OPT("--granulerate") || OPT("-g")) {
      print_granulerate = 1;
    } else if (OPT("--output") || OPT("-o")) {
      i++; if (i >= argc) goto usage_err;
      outfilename = argv[i];
    } else {
      infilename = argv[i];
    }
  }

  if (show_version) {
    printf ("anxrip version " VERSION "\n");
  }

  if (show_version || show_help) {
    printf ("# anxrip, Copyright (C) 2003 CSIRO Australia www.csiro.au ; www.annodex.net\n");
  }

  if (show_help) {
    usage ();
  }

  if (show_version || show_help) {
    exit (0);
  }

  if (infilename == NULL) {
    goto usage_err;
  } else {
    if (strcmp (infilename, "-") == 0) {
      anx = anx_open_stdio (stdin, ANX_READ);
    } else {
      anx = anx_open (infilename, ANX_READ);
    }
  }

  if (anx == NULL) {
    exit_err ("Error opening annodex %s", infilename);
  }

  if (outfilename == NULL) {
    outfile = stdout;
  } else {
    if ((outfile = fopen (outfilename, "w")) == NULL) {
      exit_err ("error opening %s for writing", outfilename);
    }
  }

  if (rip_raw) {
    anx_set_read_raw_callback (anx, read_raw, NULL);
  } else if (head_only) {
    anx_set_read_head_callback (anx, read_head, NULL);
  } else {
    anx_set_read_head_callback (anx, read_head, NULL);
    anx_set_read_clip_callback (anx, read_clip, NULL);
  }

  if (!rip_raw) {
    fprintf (outfile, CMML_PREAMBLE);
    fprintf (outfile, "<cmml>\n");

    print_tracks (anx);
  }

  while ((n = anx_read (anx, 1024)) > 0);

  if (n == -1) {
      exit_err ("Failed anx_read: %s\n", anx_strerror (anx));
  }

  if (!rip_raw) {
    fprintf (outfile, "</cmml>\n");
  }

  if (anx_close (anx) != NULL) {
    exit_err ("Failed close of annodex");
  }

  exit (0);

usage_err:
  usage ();
  exit (0);
}
