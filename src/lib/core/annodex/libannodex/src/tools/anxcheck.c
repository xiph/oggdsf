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
  printf ("\nUsage: anxcheck [options] file\n");
  printf ("Check annodexed media.\n\n");
  printf ("Options\n");
  printf ("  -v, --verbose     Display verbose information about file\n\n");
  printf ("  -h, --help        Display this help and exit\n");
  printf ("  --version         Output version information and exit\n\n");
}

static int verbose;
static double prev_time = 0;

static int
read_track (ANNODEX * anx, long serialno, char * id, char * content_type,
		anx_int64_t granule_rate_n, anx_int64_t granule_rate_d,
		int nr_header_packets, void * user_data)
{
  /* actually store and recalc granule_rate stuff */
  return 0;
}

static int
check_time (ANNODEX * anx)
{
  double at_time = anx_tell (anx);

  if (at_time < prev_time) {
    return -1;
  }

  prev_time = at_time;

  return 0;
}

static int
read_head (ANNODEX * anx, const AnxHead * head, void * user_data)
{
  return check_time (anx);
}

static int
read_clip (ANNODEX * anx, const AnxClip * clip, void * user_data)
{
  return check_time (anx);
}

static int
read_raw (ANNODEX * anx, unsigned char * data, long n, long serialno,
	    anx_int64_t granulepos, void * user_data)
{
  return check_time (anx);
}

#if 0
static int
check_file (char * filename)
{
  return 0;
}
#endif

int
main (int argc, char *argv[])
{
  int show_version = 0;
  int show_help = 0;
  int i;

  char * filename = NULL;

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
    } else if (OPT("--verbose") || OPT("-v")) {
      verbose = 1;
      inc_debug_level();
    } else {
      filename = argv[i];
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

  if (filename == NULL) {
    goto usage_err;
  } else {
    if (strcmp (filename, "-") == 0) {
      anx = anx_open_stdio (stdin, ANX_READ);
    } else {
      anx = anx_open (filename, ANX_READ);
    }
  }

  if (anx == NULL) {
    exit_err ("Error opening annodex %s", filename);
  }

  printf ("# anxcheck " VERSION "\t\t\thttp://www.annodex.net/\n\n");

  if (filename) {
    printf ("* Filename: %s\n", filename);
  }

  anx_set_read_track_callback (anx, read_track, NULL);
  anx_set_read_head_callback (anx, read_head, NULL);
  anx_set_read_clip_callback (anx, read_clip, NULL);
  anx_set_read_raw_callback (anx, read_raw, NULL);

  while ((n = anx_read (anx, 1024)) > 0);

  if (n == -1) {
      exit_err ("Failed anx_read: %s\n", anx_strerror (anx));
  }

  if (anx_close (anx) != NULL) {
    exit_err ("Failed close of annodex");
  }

  exit (0);

usage_err:
  usage ();
  exit (0);
}
