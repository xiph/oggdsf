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

#include "config.h"
#include "anx_compat.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include <annodex/annodex.h>

#include "common.h"

/*
 * usage ()
 *
 * print usage information.
 */
static void
usage (void)
{
  printf ("\nUsage: anxinfo [options] file\n");
  printf ("Display information about annodexed media.\n\n");
  printf ("Options\n");
  printf ("  -v, --verbose     Display verbose information about file\n\n");
  printf ("  -h, --help        Display this help and exit\n");
  printf ("  --version         Output version information and exit\n\n");
}

static int verbose;
static int nr_clips = 0;
static long raw_size = 0;

static int
printf_time_npt (double seconds)
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
    return printf ("%s%02d:%02d:0%2.3f", sign, hrs, min, sec);
  } else {
    return printf ("%s%02d:%02d:%02.3f", sign, hrs, min, sec);
  }
}

#if 0
static int
read_track (ANNODEX * anx, long serialno, char * id, char * content_type,
		anx_int64_t granule_rate_n, anx_int64_t granule_rate_d,
		int nr_header_packets, void * user_data)
{
  char buf[64];

  /* XXX: id */

  snprintf (buf, 64, "%ld/%ld", (long)granule_rate_n, (long)granule_rate_d);
  printf ("%010ld\t%s\t%20s\t%7d\n",
	  serialno, content_type, buf, nr_header_packets);

  return 0;
}
#endif

static int
read_head (ANNODEX * anx, const AnxHead * head, void * user_data)
{
  return 0;
}

static int
read_clip (ANNODEX * anx, const AnxClip * clip, void * user_data)
{
  nr_clips++;

  return 0;
}

static int
read_raw (ANNODEX * anx, unsigned char * data, long n, long serialno,
	  anx_int64_t granulepos, void * user_data)
{
  raw_size += n;

  return 0;
}

int
main (int argc, char *argv[])
{
  int show_version = 0;
  int show_help = 0;
  int i;

  char * infilename = NULL;

  ANNODEX * anx = NULL;
  long n;
  
  char buf[64];

  AnxList * tracks, * l;
  AnxTrack * s;

  set_progname (argv[0]);

#define OPT(s) (strcmp (argv[i], (s)) == 0)
    
  /* Parse options; modify behaviour according to user-specified options */
  for (i=1; i < argc; i++) {
    if (OPT("--help") || OPT("-h")) {
      show_help = 1;
    } else if (OPT("--version")) {
      show_version = 1;
    } else if (OPT("--verbose") || OPT("-v")) {
      verbose = 1;
      inc_debug_level();
    } else {
      infilename = argv[i];
    }
  }

  if (show_version) {
    printf ("anxinfo version " VERSION "\n");
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

  if (verbose) {
    printf ("# anxinfo " VERSION "\t\t\thttp://www.annodex.net/\n\n");

    if (infilename) {
      printf ("* Filename: %s\n", infilename);
    }

    printf ("\n");
    printf ("%-20s%-10s  %12s %7s %-20s\n",
	    "# id", "Serial", "Granule rate", "+Hdrs", "Content-type");
  }

  anx_set_read_head_callback (anx, read_head, NULL);
  anx_set_read_clip_callback (anx, read_clip, NULL);
  anx_set_read_raw_callback (anx, read_raw, NULL);

  while ((n = anx_read (anx, 1024)) > 0);

  if (n == -1) {
      exit_err ("Failed anx_read: %s\n", anx_strerror (anx));
  }

  tracks = anx_get_track_list (anx);
  for (l = tracks; l; l = l->next) {
    s = (AnxTrack *)l->data;

    snprintf (buf, 64, "%ld/%ld",\
	      (long)s->granule_rate_n, (long)s->granule_rate_d);
    printf ("%-20s%010ld  %12s %7ld %-20s\n",
	    s->id ? s->id : "-- unidentified --",
	    s->serialno, buf, s->nr_header_packets, s->content_type);

  }

  if (verbose) {
    printf ("\n*Clips: %d\n", nr_clips);
    printf ("*Content: %ld bytes\n", raw_size);
    printf ("*Timebase: ");
    printf_time_npt (anx_get_timebase (anx));
    printf ("\n");
    printf ("*Duration: ");
    printf_time_npt (anx_tell_time (anx));
    printf ("\n");
  }

  if (anx_close (anx) != NULL) {
    exit_err ("Failed close of annodex");
  }

  exit (0);

usage_err:
  usage ();
  exit (0);
}
