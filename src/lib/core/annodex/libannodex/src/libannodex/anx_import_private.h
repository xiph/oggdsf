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

#ifndef __ANX_IMPORT_PRIVATE_H__
#define __ANX_IMPORT_PRIVATE_H__

/* Media */

/* anx_media */

AnxSource *
anx_media_open (ANNODEX * anx, char * filename,
		char * id, char * content_type,
		double start, double end, int flags,
		AnxImportStream import_stream,
		AnxImportHead import_head,
		AnxImportClip import_clip,
		void * import_user_data,
		AnxImportImport import_import,
		void * import_import_user_data);

AnxSource *
anx_media_openfd (ANNODEX * anx, int fd,
		  char * id, char * content_type,
		  double start, double end, int flags,
		  AnxImportHead import_head,
		  AnxImportClip import_clip,
		  void * import_user_data,
		  AnxImportImport import_import,
		  void * import_import_user_data);

long anx_media_read (AnxSource * source, unsigned char * buf, long n, long bound);
long anx_media_sizeof_next_read (AnxSource * source, long bound);
int anx_media_close (AnxSource * source);

int
anx_media_query_track (AnxSource * source, int track_index,
		       char ** id, char ** content_type,
		       long * nr_header_packets,
		       anx_int64_t * granule_rate_n,
		       anx_int64_t * granule_rate_d);

int anx_media_current_track (AnxSource * source);
anx_int64_t anx_media_tell (AnxSource * source);
double anx_media_tell_time (AnxSource * source);
int anx_media_eos (AnxSource * source);

#endif /* __ANX_IMPORT_PRIVATE_H__ */
