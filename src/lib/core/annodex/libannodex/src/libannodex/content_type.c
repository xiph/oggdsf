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
#include <string.h>

#ifndef WIN32
#include <pwd.h>
#endif

#include <sys/types.h>

#include "anx_snprint.h"

int
anx_content_type_matches (char * content_type, char * content_type_pattern)
{
  char * m_sep, * m_major, * m_minor;
  char * p_sep, * p_major, * p_minor;
  int ret = 0;

  /* Catch the common trivial cases of full wildcards and exact matches */
  if (!strcmp (content_type_pattern , "*/*")) {
    return 1;
  }
  if (!strcmp (content_type, content_type_pattern)) {
    return 1;
  }

  m_major = strdup (content_type);
  m_sep = strchr (m_major, '/');
  *m_sep = '\0';
  m_minor = ++m_sep;

  p_major = strdup (content_type_pattern);
  p_sep = strchr (p_major, '/');
  *p_sep = '\0';
  p_minor = ++p_sep;

  if (!strcmp (p_minor, "*") && !strcmp (p_major, m_major)) ret = 1;
  /* if (!strcmp (p_major, "*") && !strcmp (p_minor, m_minor)) ret = 1;*/

  free (m_major);
  free (p_major);

  return ret;
}

/* /etc/mime.types support */
static char *
scan_mimetypes (char * mimetypes, char * ext)
{
  FILE * f;
#define MAXLINE 256
  char buf[MAXLINE];
  char content_type[MAXLINE], next[MAXLINE];
  int found = 0;
  int n, o;

  f = fopen (mimetypes, "r");
  if (f == NULL) return NULL;

  while (!found && fgets (buf, MAXLINE, f) != NULL) {
    if (sscanf (buf, "%s%n", content_type, &n) > 0 && content_type[0] != '#') {
      while (!found && sscanf (&buf[n], "%s%n", next, &o) > 0) {
	if (!strcasecmp (ext, next)) found = 1;
	n += o;
      }
    }
  }

  fclose (f);

  if (found)
    return anx_strdup (content_type);
  else
    return NULL;
}

/*
 * anx_get_homedir ()
 *
 * Get the user's home directory.
 */
static char *
anx_get_homedir (void)
{
#ifndef _POSIX_SOURCE
  return NULL;
#else

  uid_t uid;
  char * username, * homedir;
  struct passwd * pw;

  if ((homedir = getenv ("HOME")) != NULL) {
    return strdup (homedir);
  }

  /* else ... go hunting for it */
  uid = getuid ();

  username = getenv ("LOGNAME");
  if (!username) username = getenv ("USER");

  if (username) {
    pw = getpwnam (username);
    if (pw && pw->pw_uid == uid) goto gotpw;
  }

  pw = getpwuid (uid);

gotpw:

  if (!pw) {
    return NULL;
  }

  homedir = anx_strdup (pw->pw_dir);

  return homedir;

#endif /* POSIX */
}

char *
anx_content_type_guess_by_filename (char * filename)
{
  char * ext;
#ifndef WIN32
  char * content_type;
  char * homedir;
  char home_types [1024];
#endif

  ext = strrchr (filename, '.');

  if (ext == NULL) return NULL;

  ext++;

  if (!strncasecmp (ext, "cmml", 5)) return strdup ("text/x-cmml");
  if (!strncasecmp (ext, "anx", 4)) return strdup ("application/x-annodex");

#ifndef WIN32
  /* Scan ~/.mime.types first */
  if ((homedir = anx_get_homedir()) != NULL) {
    snprintf (home_types, 1024, "%s/.mime.types", homedir);
    content_type = scan_mimetypes (home_types, ext);
    free (homedir);
    if (content_type != NULL) return content_type;
  }

  /* Scan /etc/mime.types */
  content_type = scan_mimetypes ("/etc/mime.types", ext);
  if (content_type != NULL) return content_type;

  /* Scan others */
  content_type = scan_mimetypes ("/etc/apache/mime.types", ext);
  if (content_type != NULL) return content_type;
  content_type = scan_mimetypes ("/etc/httpd/mime.types", ext);
  if (content_type != NULL) return content_type;
#endif

  return NULL;
}
