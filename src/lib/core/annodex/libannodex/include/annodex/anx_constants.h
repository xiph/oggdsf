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

/** \file
 * Named constants used by the libannodex C API
 */

#ifndef __ANX_CONSTANTS_H__
#define __ANX_CONSTANTS_H__

/* Flags to anx_open */
#define ANX_READ 00
#define ANX_WRITE 01

/* Returns from read callbacks etc. */
#define ANX_CONTINUE 0
#define ANX_STOP_OK 1
#define ANX_STOP_ERR -1

/* Flags to anx_writer_import */
#define ANX_IGNORE_ANNO            0x1
#define ANX_IGNORE_MEDIA           0x2
#define ANX_IGNORE_IMPORT_IMPORT   0x4
#define ANX_ALLOW_RECURSIVE_IMPORT 0x8

/* Whence argument to anx_seek */
#define ANX_SEEK_SET 0
#define ANX_SEEK_CUR 1
#define ANX_SEEK_END 2
#define ANX_SEEK_HEAD 3
#define ANX_SEEK_PREV 4
#define ANX_SEEK_NEXT 5

/* Error codes */
enum AnxError {
  /** No error */
  ANX_ERR_OK = 0,

  /** System specific error; check errno for details */
  ANX_ERR_SYSTEM = -1,

  /** The requested operation is not suitable for this ANNODEX */
  ANX_ERR_INVALID = -2,

  /** Not supported */
  ANX_ERR_NOTSUPPORTED = -3,

  /** Functionality disabled at build time */
  ANX_ERR_DISABLED = -4,

  ANX_ERR_NOTREADY = -10,
  ANX_ERR_NOSEEK = -11,
  ANX_ERR_EOF = -12,
  ANX_ERR_NOIMPORTER = -13,
  ANX_ERR_NOTIMPLEMENTED = -99
};

#endif /* __ANX_CONSTANTS_H__ */
