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

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* use ltdl and ldfcn only if not on windows */
#ifdef WIN32
#include <windows.h>

/* on windows just wrap the functions */
const char *
dlerror_F (void)
{
  int errnumber;
  static char errmsg[100];
  if ((errnumber = GetLastError()) != ERROR_SUCCESS) {
    _snprintf(errmsg, 100, "ERROR: LoadLibrary failed on Error number %d\n", errnumber);
    return errmsg;
  } else {
    _snprintf(errmsg, 100, "LoadLibrary success\n");
    return errmsg;
  }
}

#else					      

/* Use ltdl only if native dlfcn is non-existent or broken */
#if (defined(HAVE_DLFCN_H) && !defined(__APPLE__))

/* ANSI C forbids an empty source file */
int fix_dl_dummy (void) { return 0; }

#else

#include <ltdl.h>

static int ltdl_ready = 0;

static void
ensure_ltdl_is_ready (void)
{
  if (ltdl_ready) return;

  if (lt_dlinit() != 0) {
    const char *err = lt_dlerror();
    fprintf(stderr, "ERROR: lt_dlinit() failed: %s\n", err);
    exit(1);
  }
  lt_dlmalloc = malloc;
  lt_dlfree = free;

  ltdl_ready = 1;
}

const char *
dlerror_F (void)
{
  ensure_ltdl_is_ready ();
  return lt_dlerror();
}

void *
dlopen_F (const char * pathname, int flag)
{
  ensure_ltdl_is_ready ();

#ifdef __APPLE__
  /* XXX: On Mac OS X, lt_dlsym() is a bit silly and crashes if you lt_dlopen
   * a .a library.  The real fix for this should go in libltdl, but this'll do
   * for now */
  if (strstr(pathname, ".dylib") == NULL && strstr(pathname, ".so") == NULL) {
    return NULL;
  }
#endif

  return lt_dlopen (pathname);
}

void *
dlsym_F (void * handle, char * symbol)
{
  ensure_ltdl_is_ready ();
  return lt_dlsym (handle, symbol);
}

int
dlclose_F (void * handle)
{
  ensure_ltdl_is_ready ();
  return lt_dlclose (handle);
}

#endif

#endif /* WIN32 */
