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
#include <string.h>

#include <annodex/anx_core.h>
#include <annodex/annodex.h>

#include "common.h"

#define ANX_CONTENT_TYPE "application/x-annodex"

/*
 * usage ()
 *
 * print usage information.
 */
static void
usage (void)
{
  printf ("\nUsage: anxed [options] file\n");
  printf ("Re-edit annodexed media.\n\n");
  printf ("Options\n");
  printf ("  -o, --output      Specify output filename (default is stdout)\n\n");
  printf ("  -s, --seek-offset Specify time offset to seek input to\n");
  printf ("  -e, --seek-end    Specify time offset to end input at\n\n");
  printf ("  -h, --help        Display this help and exit\n");
  printf ("  --version         Output version information and exit\n\n");
}

int
main (int argc, char *argv[])
{
  int show_version = 0;
  int show_help = 0;
  int i;

  ANNODEX * anx = NULL;
  long n = 1024;
  
  char * infilename = NULL;
  /*  FILE * infile = stdin;*/

  double seek_offset = 0.0, seek_end = -1.0;

  char * outfilename = NULL;

#define BUF_LEN 1024
  char outfilename_buf[BUF_LEN];

  int ret;

  set_progname (argv[0]);

#define OPT(s) (strcmp (argv[i], (s)) == 0)
    
  /* Parse options; modify behaviour according to user-specified options */
  for (i=1; i < argc; i++) {
    if (OPT("--help") || OPT("-h")) {
      show_help = 1;
    } else if (OPT("--version")) {
      show_version = 1;
    } else if (OPT("--verbose") || OPT("-v")) {
      inc_debug_level();
    } else if (OPT("--seek-end") || OPT("-e")) {
      i++; if (i >= argc) goto usage_err;
      seek_end = anx_parse_time (argv[i]);
    } else if (OPT("--seek-offset") || OPT("-s")) {
      i++; if (i >= argc) goto usage_err;
      seek_offset = anx_parse_time (argv[i]);
    } else if (OPT("--output") || OPT("-o")) {
      i++; if (i >= argc) goto usage_err;
      outfilename = argv[i];
    } else {
      infilename = argv[i];
    }
  }

  if (show_version) {
    printf ("%s version " VERSION "\n", argv[0]);
  }

  if (show_version || show_help) {
    printf ("# anxed Copyright (C) 2003 CSIRO Australia www.csiro.au / www.annodex.net\n");
  }

  if (show_help) {
    usage ();
  }

  if (show_version || show_help) {
    exit (0);
  }

  if (infilename == NULL) {
    goto usage_err;
  }

  if (outfilename == NULL) {
    snprintf (outfilename_buf, BUF_LEN, "%s.%d", infilename, getpid ());
    outfilename = outfilename_buf;
  }

  anx = anx_open (outfilename, ANX_WRITE);

  if (anx == NULL) {
    fprintf (stderr, "Failed creation of new annodex\n");
    goto cleanup_err;
  }

  anx_init_importers (ANX_CONTENT_TYPE);

#ifdef DEBUG
    printf ("anxed: anx_writer_import %s @npt=%f-%f\n", infilename, seek_offset, seek_end);
#endif

  ret = anx_write_import (anx, infilename, NULL, ANX_CONTENT_TYPE,\
			  seek_offset, seek_end, 0);

  if (ret == -1) {
    fprintf (stderr, "Failed import of original file.\n");
    goto cleanup_err;
  }

  while ((n = anx_write (anx, 1024)) > 0);

  if (anx_close (anx) != NULL) {
    fprintf (stderr, "Failed close of annodex\n");
    goto cleanup_err;
  }

  /* Success */

  if (outfilename == outfilename_buf) {
    if (rename (outfilename, infilename) == -1) {
      exit_err ("Error renaming %s as %s", outfilename, infilename);
    }
  }

  exit (0);

 cleanup_err:
  unlink (outfilename);
  exit (1);

 usage_err:
  usage ();
  exit (0);
}
