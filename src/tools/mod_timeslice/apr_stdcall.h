// ===========================================================================
// Copyright (C) 2004-2005  Commonwealth Scientific and Industrial Research
//                          Organisation (CSIRO) Australia
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// - Neither the name of Zentaro Kavanagh, CSIRO Australia nor the names of
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ===========================================================================

#pragma once


/* Apache expects its modules to use a __cdecl calling convention on Windows
   but we link to many libraries which are __stdcall, so it's more convenient
   for us to generate code defaulting to __stdcall.  This means, however, that
   we need to declare any entry points into the module as __cdecl.  Thus, prefix
   add the AP_MODULE_ENTRY_POINT macro below to any functions which Apache
   is responsible for calling.
 */

#ifdef WIN32
# define AP_MODULE_ENTRY_POINT __cdecl
#else
# define AP_MODULE_ENTRY_POINT
#endif

/* We also need to override the function typedefs in the Apache header files,
   because the included Apache headers will think our functions are __stdcall,
   rather than __cdecl, which is what they actually are.  So, we need to typecast
   the function prototype into fooling the Apache headers that we're __stdcall.
   Ouch.  Use the macros below to do this.
 */
   
#ifdef WIN32
# define AP_HOOK_HANDLER_FUNCTION(x) (ap_HOOK_handler_t (__stdcall *)) x
#else
# define AP_HOOK_HANDLER_FUNCTION(x) x
#endif

#ifdef WIN32
# define AP_REGISTER_HOOK_FUNCTION(x) (void (__stdcall *)(apr_pool_t *)) x
#else
# define AP_REGISTER_HOOK_FUNCTION(x) x
#endif

/* One final note: remember to extern "C" any functions that are entry points
   into the module, and also to append AP_MODULE_DECLARE_DATA after your
   module declaration!  (See http://httpd.apache.org/docs-2.0/mod/mod_so.html
   for more information about the latter.)
 */

