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


#ifndef __FIX_DL_H__
#define __FIX_DL_H__


/* use ltdl and dlfc only if not on windows */
#ifdef WIN32

#include <windows.h>
/* on windows just wrap the functions */
const char * dlerror_F (void);

#define dlopen_F(a,b) LoadLibrary(a)
#define dlsym_F(a,b) GetProcAddress(a,b)
#define dlclose_F(a) FreeLibrary(a)

#else

/* If we have a working dlfcn, simply wrap that with new names. Otherwise,
 * link in new wrapper functions (implemented with libltdl).
 */

#if defined(HAVE_DLFCN_H) && !defined(__APPLE__)

#include <dlfcn.h>
#define dlerror_F dlerror
#define dlopen_F dlopen
#define dlsym_F dlsym
#define dlclose_F dlclose

#else

/* Mac OS X doesn't define RTLD_LAZY or RTLD_NOW, which is required to call
 * the dlopen_F wrapper function.  Fix that.
 */
#define RTLD_LAZY 0
#define RTLD_NOW 1

const char * dlerror_F (void);
void * dlopen_F (const char * pathname, int flag);
void * dlsym_F (void * handle, char * symbol);
int dlclose_F (void * handle);

#endif

#endif /* WIN32 */

#endif /* __FIX_DL_H__ */
