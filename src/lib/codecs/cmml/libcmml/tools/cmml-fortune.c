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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <cmml.h>

/**
 * DEFAULT_DURATION: default duration of created CMML file
 */
#define DEFAULT_DURATION "00:30"

/**
 * DEFAULT_ENCODING: default encoding format of created CMML file
 */
#define DEFAULT_ENCODING "UTF-8"

/**
 * DEFAULT_SHORT_COMMAND: default command to create short descriptions
 */
#define DEFAULT_SHORT_COMMAND "fortune -s -n 80"

/**
 * DEFAULT_LONG_COMMAND: default command to create long descriptions
 */
#define DEFAULT_LONG_COMMAND "fortune -l"

/**
 * BUFSIZE: default buffer size for printing
 */
#define BUFSIZE 10000

/**
 * \file
 * cmml-fortune:
 * generates a valid CMML document with random content.
 *
\verbatim
Usage: cmml-fortune [options]
Generate a random CMML file using an external program
to provide text (fortune cookies by default).
 
Possible options:
  -d timespec, --duration timespec
                 Specify the duration of the generated CMML file
                 ('00:30' by default)
  -e encoding, --encoding encoding
                 Specify the encoding of the generated CMML file
                 ('UTF-8' by default)
 
  -o filename, --output filename
                 Specify the output filename. The file is written
                 to standard output by default.
 
  -s command,  --short-command command
                 Specify the command to use to generate short text
                 for the title and anchors ('fortune -s -n 80' by default)
  -l command,  --long-command command
                 Specify the command to use to generate long text
                 for descriptions ('fortune -l' by default)
  -h, --help     Display this help and exit
  -v, --version  Display version information and exit
\endverbatim
 *
 */

/**
 * PrintUsage: prints out help on how to use this program
 *
 * \param prog the program's name
 */
static void
PrintUsage (char * prog) {
  fprintf (stderr, "Usage: %s [options]\n", prog);
  fprintf (stderr, "Generate a random CMML file using an external program\n");
  fprintf (stderr, "to provide text (fortune cookies by default).\n\n");
  fprintf (stderr, "Possible options:\n");

#ifdef HAVE_GETOPT_LONG
  fprintf (stderr, "  -d timespec, --duration timespec\n");
  fprintf (stderr, "                 Specify the duration of the generated CMML file\n");
  fprintf (stderr, "                 ('" DEFAULT_DURATION "' by default)\n");
  fprintf (stderr, "  -e encoding, --encoding encoding\n");
  fprintf (stderr, "                 Specify the encoding of the generated CMML file\n");
  fprintf (stderr, "                 ('" DEFAULT_ENCODING "' by default)\n\n");
  fprintf (stderr, "  -o filename, --output filename\n");
  fprintf (stderr, "                 Specify the output filename. The file is written\n");
  fprintf (stderr, "                 to standard output by default.\n\n");
  fprintf (stderr, "  -s command,  --short-command command\n");
  fprintf (stderr, "                 Specify the command to use to generate short text\n");
  fprintf (stderr, "                 for the title and anchors ('"
	                             DEFAULT_SHORT_COMMAND "' by default)\n");
  fprintf (stderr, "  -l command,  --long-command command\n");
  fprintf (stderr, "                 Specify the command to use to generate long text\n");
  fprintf (stderr, "                 for descriptions ('"
	                             DEFAULT_LONG_COMMAND "' by default)\n\n");
  fprintf (stderr, "  -h, --help     Display this help and exit\n");
  fprintf (stderr, "  -v, --version  Display version information and exit\n");
#else
  fprintf (stderr, "  -d timespec\n");
  fprintf (stderr, "                 Specify the duration of the generated CMML file\n");
  fprintf (stderr, "                 ('" DEFAULT_DURATION "' by default)\n");
  fprintf (stderr, "  -e encoding\n");
  fprintf (stderr, "                 Specify the encoding of the generated CMML file\n");
  fprintf (stderr, "                 ('" DEFAULT_ENCODING "' by default)\n\n");
  fprintf (stderr, "  -o filename\n");
  fprintf (stderr, "                 Specify the output filename. The file is written\n");
  fprintf (stderr, "                 to standard output by default.\n\n");
  fprintf (stderr, "  -s command\n");
  fprintf (stderr, "                 Specify the command to use to generate short text\n");
  fprintf (stderr, "                 for the title and anchors ('"
	                             DEFAULT_SHORT_COMMAND "' by default)\n");
  fprintf (stderr, "  -l command\n");
  fprintf (stderr, "                 Specify the command to use to generate long text\n");
  fprintf (stderr, "                 for descriptions ('"
	                             DEFAULT_LONG_COMMAND "' by default)\n\n");
  fprintf (stderr, "  -h             Display this help and exit\n");
  fprintf (stderr, "  -v             Display version information and exit\n");
#endif
  fprintf(stderr, "\nPlease report bugs to <libcmml-devel@cmis.csiro.au>.\n");
  exit(1);
}

/**
 * get_fortune: generates random text using the given command
 *
 * \param command the command to use to generate text
 *
 * \returns the generated text
 */
static char *
get_fortune (char * command)
{
  FILE * p;
  static char buf[BUFSIZE];
  int n;

  p = popen (command, "r");
  if (p == NULL) return 0;

  n = fread (buf, 1, BUFSIZE, p);
  if (n <= 0) return 0;

  /* ensure the text is null terminated, and strip
   * newlines off the end of fortunes if present */
  if (buf[n-1] == '\n')
    buf[n-1] = '\0';
  else
    buf[n] = '\0';

  return strdup (buf);
}

/**
 * main function of cmml-fortune, which opens the CMML file, seeks to
 * any given offsets, registers the callbacks, and then steps through
 * the file in chunks of BUFSIZE size, during which the callbacks get
 * activated as the relevant elements get parsed.
 */
int main(int argc, char *argv[])
{
  int i;
  CMML_Time * dur = NULL;
  CMML_Preamble * pre = NULL;
  CMML_Head * head = NULL;
  CMML_Clip * clip = NULL;
  CMML_Time * start = NULL, * end = NULL;
  double s;

  char * outfilename = NULL;
  FILE * outfile = stdout;

  char * encoding = DEFAULT_ENCODING;
  char * short_command = DEFAULT_SHORT_COMMAND;
  char * long_command = DEFAULT_LONG_COMMAND;
  char buf[BUFSIZE];

  while (1) {
    char * optstring = "hd:e:o:s:l:";

#ifdef HAVE_GETOPT_LONG
    static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"version",no_argument,0, 'v'},
      {"duration", required_argument, 0, 'd'},
      {"encoding", required_argument, 0, 'e'},
      {"output", required_argument, 0, 'o'},
      {"short-command", required_argument, 0, 's'},
      {"long-command", required_argument, 0, 'l'},
      {0,0,0,0}
    };
    i = getopt_long(argc, argv, optstring, long_options, NULL);
#else
    i = getopt(argc, argv, optstring);
#endif

    if (i == -1) break;
    if (i == ':') PrintUsage (argv[0]);

    switch (i) {
    case 'h': /* help */
      PrintUsage (argv[0]);
      break;
    case 'v': /* version */
      fprintf(stdout, "cmml-fortune version " VERSION "\n");
      fprintf(stdout, "# cmml-fortune, Copyright (C) 2003 CSIRO Australia www.csiro.au ; www.annodex.net\n");
      exit (0);
      break;
    case 'd': /* duration */
      dur = cmml_time_new (optarg);
      if (dur == NULL) {
	printf ("Unable to parse duration %s\n", optarg);
      }
      break;
    case 'e': /* encoding */
      encoding = optarg;
      break;
    case 'o': /* output */
      outfilename = optarg;
      break;
    case 's': /* short command */
      short_command = optarg;
      break;
    case 'l': /* long command */
      long_command = optarg;
      break;
    default:
      break;
    }
  }

  /* set duration of file */
  if (dur == NULL)
    dur = cmml_time_new (DEFAULT_DURATION);

  /* open output file */
  if (outfilename != NULL) {
    if ((outfile = fopen (outfilename, "wb")) == NULL) {
      fprintf (stderr, "Error opening %s for writing\n", outfilename);
      exit (1);
    }
  }

  /* write preamble */
  pre = cmml_preamble_new(encoding, NULL, NULL, NULL);
  cmml_preamble_snprint (buf, BUFSIZE, pre);
  fputs (buf, outfile);
  cmml_preamble_destroy(pre);

  /* write head */
  head = cmml_head_new();
  head->title = get_fortune (short_command);
  cmml_head_pretty_snprint (buf, BUFSIZE, head);
  fputs (buf, outfile);
  cmml_head_destroy(head);

  /* write clips */
  for (s = 0.0; s < dur->t.sec; s += 5.0) {
    cmml_npt_snprint (buf, BUFSIZE, s);
    start = cmml_time_new (buf);

    cmml_npt_snprint (buf, BUFSIZE, s+4);
    end = cmml_time_new (buf);

    clip = cmml_clip_new(start, end);
    clip->anchor_href = strdup("http://www.annodex.net/");
    clip->anchor_text = get_fortune(short_command);
    clip->desc_text = get_fortune (long_command);
    cmml_clip_pretty_snprint (buf, BUFSIZE, clip);
    fputs (buf, outfile);
    cmml_clip_destroy(clip);
  }

  fprintf (outfile, "</cmml>\n");

  return 0;
}
