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

#ifndef WIN32
#include <unistd.h>
#endif

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
  printf ("Usage: anxenc -t content-type file [output options]\n");
  printf ("Encapsulate data into annodexed media.\n\n");
  printf ("Input options\n");
  printf ("  -t content-type, --type content-type\n");
  printf ("                    Specify content-type of input file\n\n");
  printf ("Output options\n");
  printf ("  -o, --output      Specify output filename (default is stdout)\n\n");
  printf ("General options\n");
  printf ("  -h, --help        Display this help and exit\n");
  printf ("  --version         Output version information and exit\n\n");
  printf ("Please report bugs to <audio@cmis.csiro.au>.\n");
}

int
main (int argc, char *argv[])
{
  int show_version = 0;
  int show_help = 0;
  int i;

  ANNODEX * anx = NULL;
  unsigned char buf [1024];
  long n = 1024, n_written;

  char * infilename = NULL;
  /*  FILE * infile = stdin;*/

  char * outfilename = NULL;
  FILE * outfile = stdout;

#define BUF_LEN 1024
  char outfilename_buf[BUF_LEN];

  char * content_type = NULL;

  int ret;

  set_progname (argv[0]);

  anx_init_importers ("*/*");

  anx = anx_new (ANX_WRITE);
  if (anx == NULL) {
    exit_err ("Failed creation of new annodex\n");
  }

#define OPT(s) (strcmp (argv[i], (s)) == 0)
    
  /* Parse options; modify behaviour according to user-specified options */
  for (i=1; i < argc; i++) {
    if (OPT("--help") || OPT("-h")) {
      show_help = 1;
    } else if (OPT("--version")) {
      show_version = 1;
    } else if (OPT("--verbose") || OPT("-v")) {
      inc_debug_level();
    } else if (OPT("--output") || OPT("-o")) {
      i++; if (i >= argc) goto usage_err;
      outfilename = argv[i];
    } else if (OPT("--type") || OPT("-t")) {
      i++; if (i >= argc) goto usage_err;
      content_type = argv[i];
    } else {
      infilename = argv[i];
      ret = anx_write_import (anx, infilename, NULL, content_type,
			      0.0, -1.0, 0);
      if (ret == -1) {
	fprintf (stderr, "anxenc: Error importing %s: %s\n", infilename,
		 anx_strerror (anx));
      }
      content_type = NULL;
    }
  }

  if (show_version) {
    printf ("anxenc version " VERSION "\n");
  }

  if (show_version || show_help) {
    printf ("# anxenc, Copyright (C) 2003 CSIRO Australia www.csiro.au ; www.annodex.net\n");
  }

  if (show_help) {
    usage ();
  }

  if (show_version || show_help) {
    exit (0);
  }

  if (!anx_ready (anx)) {
    fprintf (stderr, "Not ready: %s\n", anx_strerror (anx));
    exit (1);
  }

  if (outfilename == NULL) {
    snprintf (outfilename_buf, BUF_LEN, "%s.anx", infilename);
    outfilename = outfilename_buf;
  }

  if ((outfile = fopen (outfilename, "wb")) == NULL) {
    exit_err ("error opening %s for writing", outfilename);
  }

  while (n > 0) {
    n = anx_write_output (anx, buf, 1024);
    if (n == -1) {
      exit_err ("Failed anx_writer_output: %s\n", anx_strerror (anx));
    }

    if ((n_written = fwrite (buf, 1, (size_t)n, outfile)) < n) {
      if (ferror(outfile) != 0)
	print_debug (2, "Error writing output file\n");
      else
	print_debug (2, "Error: short write; wrote %ld bytes\n", n_written);
    }
  }

  if (anx_close (anx) != NULL) {
    exit_err ("Failed close of annodex %s\n", infilename);
  }

  if (fclose (outfile) == EOF) {
    exit_err ("Error closing output file %s\n", outfilename);
  }

  exit (0);

usage_err:
  usage ();
  exit (0);
}
