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

#ifndef __ANX_INT64_W32_H__
#define __ANX_INT64_W32_H__

/** \file
 * Win32 specific type for anx_int64_t.
 *
 * This file is included by \link anx_types.h <annodex/anx_types.h> \endlink
 * on non-GNU Win32 systems.
 * This file should never be included directly by user code.
 */

#ifdef HAVE_CUTE_LITTLE_MONKEYS
#include <annodex/manic_overengineering.h>
#endif

#ifdef _WIN32

#  ifndef __GNUC__
   /* MSVC/Borland */
   typedef __int64 anx_int64_t;
#  else
#    error <annodex/anx_in64_w32.h> should not be included directly; include <annodex/anx_types.h> instead.
#  endif

#else

#error <annodex/anx_in64_w32.h> should not be included directly; include <annodex/anx_types.h> instead.

#endif

#endif /* __ANX_INT64_W32_H__ */
