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

/* cmml_snprint.h */

/* macros for writing clip and import tags */

#define BEGIN_TAG(name) \
  { \
    if ((n_written = cmml_snprintf(buf + len, n, "<%s", (name))) == -1) {       \
      return -1;                                                               \
    }                                                                          \
    n -= n_written;                                                            \
    len += n_written; \
  }

#define END_TAG() \
  { \
    if ((n_written = cmml_snprintf(buf + len, n, ">")) == -1) {                 \
      return -1;                                                               \
    }                                                                          \
    n -= n_written;                                                            \
    len += n_written; \
  }

#define END_EMPTY_TAG() \
  { \
    if ((n_written = cmml_snprintf(buf + len, n, "/>")) == -1) {                \
      return -1;                                                               \
    }                                                                          \
    n -= n_written;                                                            \
    len += n_written; \
  }

#define APPEND_ATTRIBUTE(name,value) \
  { \
    if ((n_written = cmml_snprint_attribute (buf + len, n, (name), (value))) == -1) { \
      return -1;                                                               \
    }                                                                          \
    n -= n_written;                                                            \
    len += n_written; \
  }

#define APPEND_DATA(data) \
  { \
    if ((data)) {                                                              \
      if ((n_written = cmml_snprint_pcdata(buf + len, n, (data))) == -1) {      \
        return -1;                                                             \
      }                                                                        \
      n -= n_written;                                                          \
      len += n_written;                                                        \
    } \
  }

#define CLOSE_TAG(name) \
  { \
    if ((n_written = cmml_snprintf(buf + len, n, "</%s>", (name))) == -1) {     \
      return -1;                                                               \
    }                                                                           \
    n -= n_written;                                                            \
    len += n_written; \
  }

#define NEWLINE \
  { \
    if ((n_written = cmml_snprintf(buf + len, n, "\n")) == -1) { \
      return -1;                                                \
    }                                                           \
    n -= n_written;                                             \
    len += n_written; \
  }

/* macros for error and warning reporting */

#define REPORT(err,str) \
  n_written = cmml_snprintf(buf + len, n, "%d:%d: %s",          \
                            (err)->line, (err)->col, (str));    \
  if (n_written == -1) {                                        \
    return -1;                                                  \
  }                                                             \
  n -= n_written;                                               \
  len += n_written;

#define WARN(err,str) \
  n_written = cmml_snprintf(buf + len, n, "%d:%d: Warning: %s", \
                            (err)->line, (err)->col, (str));    \
  if (n_written == -1) {                                        \
    return -1;                                                  \
  }                                                             \
  n -= n_written;                                               \
  len += n_written;

