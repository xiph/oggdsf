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

/* The name we were invoked as */
static char * progname;

/* Default debug level */
#define DEBUG_LEVEL 0

#define MAXLINE 1024

/* Verbosity level for debugging */
static int debug_level = DEBUG_LEVEL;

/*
 * exit_err (fmt)
 *
 * Print a formatted error message and errno information to stderr,
 * then exit with return code 1.
 */
void
exit_err (const char * fmt, ...)
{ 
  va_list ap;
  int errno_save;
  char buf[MAXLINE];
  int n;
  
  errno_save = errno;
  
  va_start (ap, fmt);
  
  snprintf (buf, MAXLINE, "%s: ", progname);
  n = strlen (buf);
  
  vsnprintf (buf+n, MAXLINE-n, fmt, ap);
  n = strlen (buf);
  
  snprintf (buf+n, MAXLINE-n, ": %s\n", strerror (errno_save));
  
  fflush (stdout); /* in case stdout and stderr are the same */
  fputs (buf, stderr);
  fflush (NULL);
  
  va_end (ap);
  exit (1);
}

/*
 * print_err (fmt)
 *
 * Print a formatted error message to stderr.
 */
void
print_err (const char * fmt, ...)
{ 
  va_list ap;
  int errno_save;
  char buf[MAXLINE];
  int n;
  
  errno_save = errno;
  
  va_start (ap, fmt);
  
  snprintf (buf, MAXLINE, "%s: ", progname);
  n = strlen (buf);

  vsnprintf (&buf[n], (size_t)MAXLINE-n, fmt, ap);
  n = strlen (buf);
  
  fflush (stdout); /* in case stdout and stderr are the same */
  fputs (buf, stderr);
  fputc ('\n', stderr);
  fflush (NULL);

  va_end (ap);
}

/*
 * print_debug (level, fmt)
 *
 * Print a formatted debugging message of level 'level' to stderr
 */
void
print_debug (int level, const char * fmt, ...)
{
  va_list ap;
 
  if (level > debug_level) return;
 
  va_start (ap, fmt);
  print_err (fmt);
  va_end (ap);
}

/*
 * set_progname (const char * name)
 *
 * Set the program name string as used by error and debug routines.
 */
int
set_progname (const char * name)
{
  if ((progname = strdup (name)) == NULL) {
    return 1;
  }
  return 0;
}

/*
 * inc_debug_level (void)
 *
 * Set the debug level
 */
int
inc_debug_level (void)
{
  return (++debug_level);
}
