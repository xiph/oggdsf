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

#ifndef __ANX_PRIVATE_H__
#define __ANX_PRIVATE_H__

#include <config.h>

#define _ANX_PRIVATE

#include <stdlib.h>

typedef struct _ANNODEX ANNODEX;

#include <oggz/oggz.h>

#include <annodex/anx_core.h>
#include <annodex/annodex.h>

#include "anx_compat.h"
#include "anx_mapping.h"
#include "anx_bos.h"
#include "anx_tree.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef _ANX_COMMAND
typedef void * AnxCommand;
#endif

#ifndef _ANX_MEDIA
typedef void AnxImporter;
typedef void AnxSource;
#endif

#include "anx_command.h"
#include "anx_byteswap.h"
#include "anx_import_private.h"

typedef struct _AnxWriterTrack AnxWriterTrack;
typedef struct _AnxWriter AnxWriter;
typedef struct _AnxReader AnxReader;

struct _AnxWriterTrack {
  AnxTrack sub;

  AnxSource * media;
  int media_index;

  int written_bos;
  int written_eos;
};

struct _AnxWriter {
  AnxSource * current_media;
  AnxWriterTrack * current_track;

  int need_mapping;
  int need_bos;
  int need_anno_bos;
  int need_head;
  int secondary_headers_remaining;

  int done_media;
  int done_anno_eos;
  int done_eos;

  AnxTrack anno_track;

  ogg_stream_state mapping_stream;
  ogg_stream_state annotations_stream;

  AnxList * medias;
  AnxList * tracks; /* list of AnxWriterTrack */

  int eos; /* end of stream */

  AnxImportStream import_stream_callback;
  AnxImportHead import_head_callback;
  AnxImportClip import_clip_callback;
  void * import_user_data;

  AnxImportImport import_import_callback;
  void * import_import_user_data;

  AnxMapping mapping;

  char * anno_buf;
  long anno_buf_len, anno_buf_n;

  unsigned char * media_buf;
  long media_buf_len, media_buf_n;
  long media_serialno;
};

struct _AnxReader {
  AnxReadStream read_stream;
  void * read_stream_user_data;

  AnxReadTrack read_track;
  void * read_track_user_data;

  AnxReadHead read_head;
  void * read_head_user_data;

  AnxReadClip read_clip;
  void * read_clip_user_data;

  AnxReadRaw read_raw;
  void * read_raw_user_data;

  AnxList * tracks; /* List of AnxTracks */
  int got_non_bos;
  int delivered_all_init_callbacks;

  int eos; /* end of stream */

  off_t offset; /* offset of current page start */
  off_t offset_head; /* offset of first page after head */
};

struct _ANNODEX {
  int flags;
  int err;
  FILE * file;
  int timing_modifiable;
  int slurpable;

  OGGZ * oggz;

  double timebase;
  anx_int64_t current_granule;
  long current_serialno;

  AnxHead * head;

  AnxTree * commands;
  AnxCommand current_command;
  AnxCommand pending_command;

  int mapping_serialno_valid; /* is mapping_serialno valid? */
  long mapping_serialno; /* serialno of mapping */

  int anno_serialno_valid; /* is anno_serialno valid? */
  long anno_serialno; /* serialno of annotations stream */ 

  union {
    AnxReader reader;
    AnxWriter writer;
  } x;
};

/* Annodex */
ANNODEX * anx_writer_init (ANNODEX * annodex);
ANNODEX * anx_reader_init (ANNODEX * annodex);

int anx_writer_close (ANNODEX * annodex);

double anx_writer_seek_time (ANNODEX * annodex, double seconds, int whence);
double anx_reader_seek_time (ANNODEX * annodex, double seconds, int whence);

int anx_writer_seek_id (ANNODEX * annodex, const char * id);
int anx_reader_seek_id (ANNODEX * annodex, const char * id);

/* Commands */
ANNODEX * anx_commands_init (ANNODEX * annodex);
AnxCommand anx_command_seek (ANNODEX * annodex, double time_offset);

/* Errors */
void anx_set_error (ANNODEX * annodex, int err);

static __inline__ int
_anx_ready (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;
  AnxWriter * writer = &annodex->x.writer;

  if (annodex->flags == ANX_READ) {
    if (!reader->got_non_bos) return 0;
  }

  if (annodex->flags == ANX_WRITE) {
    if (writer->medias == NULL) return 0;
  }

  return 1;
}

#endif /* __ANX_PRIVATE_H__ */
