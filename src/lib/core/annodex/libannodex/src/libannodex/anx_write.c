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

#if ANX_CONFIG_WRITE

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <time.h>

#include "anx_private.h"

/*#define DEBUG*/

#define CMML_CONTENT_TYPE "text/x-cmml"
#define EMPTY_CLIP "<clip/>"

static char * cmml_content_type = CMML_CONTENT_TYPE;

static int
anx_import_stream_default (double timebase, char * utc, void * user_data)
{
  ANNODEX * anx_writer = (ANNODEX *)user_data;

  if (anx_writer->timebase == 0.0) {
    anx_set_timebase (anx_writer, timebase);
  }

  /* XXX: handle utc */

  return 0;
}

static int
anx_import_head_default (AnxHead * head, void * user_data)
{
  ANNODEX * anx_writer = (ANNODEX *)user_data;

  anx_set_head (anx_writer, head);

  return 0;
}

static int
anx_import_clip_default (AnxClip * clip, double time_offset,
			 void * user_data)
{
  ANNODEX * anx_writer = (ANNODEX *)user_data;

#ifdef DEBUG
  printf ("anx_import_clip_default: insert at %f, now %f\n", time_offset,
	  anx_tell_time (anx_writer));
#endif

  anx_insert_clip (anx_writer, time_offset, clip);

  return 0;
}

static int
anx_import_import_default (double start_time, char * filename,
			   char * id, char * content_type,
			   double seek_offset, double seek_end,
			   void * user_data)
{
  ANNODEX * anx_writer = (ANNODEX *)user_data;

#ifdef DEBUG
  printf ("anx_import_import_default: insert %s at %f\n", content_type,
	  start_time);
#endif

  /* XXX: handle start time */
  anx_write_import (anx_writer, filename, id, content_type, seek_offset,
		    seek_end, 0);

  /* XXX: allow recursive imports to set timebase? */

  return 0;
}

static int
anx_import_import_recursive (double at_time, char * filename,
			     char * id, char * content_type,
			     double seek_offset, double seek_end,
			     void * user_data)
{
  ANNODEX * anx_writer = (ANNODEX *)user_data;

#ifdef DEBUG
  printf ("anx_import_import_recursive: insert %s at %f\n", content_type,
	  at_time);
#endif

  /* XXX: handle at_time */
  anx_write_import (anx_writer, filename, id, content_type, seek_offset,
		    seek_end, ANX_ALLOW_RECURSIVE_IMPORT);

  return 0;
}

static AnxWriterTrack *
anx_writer_add_track (ANNODEX * annodex, AnxSource * media, int media_index,
		      char * id, char * content_type, long nr_header_packets,
		      anx_int64_t granule_rate_n, anx_int64_t granule_rate_d)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxWriterTrack * wsub;
  AnxTrack * track;

#ifdef DEBUG
  printf ("anx_writer_add_track: %s\n", content_type);
#endif

  wsub = anx_malloc (sizeof (AnxWriterTrack));
  track = (AnxTrack *)wsub;

  track->serialno = random ();
  track->id = id;
  track->content_type = content_type;
  track->nr_header_packets = nr_header_packets;
  track->granule_rate_n = granule_rate_n;
  track->granule_rate_d = granule_rate_d;

  wsub->media = media;
  wsub->media_index = media_index;

  wsub->written_bos = 0;
  wsub->written_eos = 0;

  writer->secondary_headers_remaining += nr_header_packets;
  writer->tracks = anx_list_append (writer->tracks, wsub);

  return wsub;
}

static AnxWriterTrack *
anx_writer_find_track (ANNODEX * annodex, AnxSource * media, int media_index)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxWriterTrack * wsub;
  AnxList * l;

  for (l = writer->tracks; l; l = l->next) {
    wsub = (AnxWriterTrack *)l->data;

    if (wsub->media == media && wsub->media_index == media_index) {
      return wsub;
    }
  }

  return NULL;
}

static void
anx_writer_init_media (ANNODEX * annodex, AnxSource * media)
{
  AnxWriter * writer = &annodex->x.writer;
  int i, ret=0;
  char * id, * content_type;
  long nr_header_packets;
  anx_int64_t granule_rate, rate_interval;

  if (media != NULL) {
    for (i = 0; ; i++) {
      ret = anx_media_query_track (media, i, &id, &content_type,
				   &nr_header_packets,
				   &granule_rate, &rate_interval);

      if (ret < 0) break;
      
#ifdef DEBUG
      printf ("anx_writer: adding track %d for %s\n", i, content_type);
#endif
      anx_writer_add_track (annodex, media, i, id, content_type,
			    nr_header_packets, granule_rate, rate_interval);
    }

    writer->medias = anx_list_append (writer->medias, media);
  }
}

int
anx_write_import (ANNODEX * annodex, char * filename,
		  char * id, char * content_type,
		  double seek_offset, double seek_end, int flags)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxSource * m;

  AnxImportStream import_stream = NULL;
  AnxImportHead import_head = NULL;
  AnxImportClip import_clip = NULL;
  void * import_user_data = NULL;

  AnxImportImport import_import = NULL;
  void * import_import_user_data = NULL;

  if ((flags & ANX_IGNORE_IMPORT_IMPORT) &&
      (flags & ANX_ALLOW_RECURSIVE_IMPORT)) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1;
  }

  import_stream = writer->import_stream_callback;

  if ((flags & ANX_IGNORE_ANNO) == 0) {
    import_head = writer->import_head_callback;
    import_clip = writer->import_clip_callback;
    import_user_data = writer->import_user_data;
  }

  if ((flags & ANX_IGNORE_IMPORT_IMPORT) == 0) {
    import_import = writer->import_import_callback;
    import_import_user_data = writer->import_import_user_data;
  }

  if (flags & ANX_ALLOW_RECURSIVE_IMPORT) {
    import_import = anx_import_import_recursive;
    import_import_user_data = annodex;
  }

  m = anx_media_open (annodex, filename, id, content_type,
		      seek_offset, seek_end, flags,
		      import_stream, import_head, import_clip,
		      import_user_data,
		      import_import, import_import_user_data);

  if (m == NULL) {
#ifdef DEBUG
    printf ("anx_write_import: anx_media_open failed\n");
#endif
    return -1;
  }

  if (anx_media_eos(m)) {
    anx_media_close (m);
  } else {
    anx_writer_init_media (annodex, m);
  }

  return 0;
}

int
anx_write_set_anno_callbacks (ANNODEX * annodex,
			      AnxImportHead import_head_callback,
			      AnxImportClip import_clip_callback,
			      void * user_data)
{
  AnxWriter * writer;

  if (annodex == NULL) return -1;

  writer = &annodex->x.writer;

  writer->import_head_callback = import_head_callback;
  writer->import_clip_callback = import_clip_callback;
  writer->import_user_data = user_data;

  return 0;
}

void *
anx_write_get_anno_user_data (ANNODEX * annodex)
{
  AnxWriter * writer;

  if (annodex == NULL) return NULL;

  writer = &annodex->x.writer;

  return writer->import_user_data;
}

int
anx_write_set_ii_callback (ANNODEX * annodex,
			   AnxImportImport import_import_callback,
			   void * user_data)
{
  AnxWriter * writer;

  if (annodex == NULL) return -1;

  writer = &annodex->x.writer;

  writer->import_import_callback = import_import_callback;
  writer->import_import_user_data = user_data;

  return 0;
}

void *
anx_write_get_ii_user_data (ANNODEX * annodex)
{
  AnxWriter * writer;

  if (annodex == NULL) return NULL;

  writer = &annodex->x.writer;

  return writer->import_import_user_data;
}

/* Page retrieval */
static int
anx_writer_update_eos (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxSource * media;
  AnxList * l;

  /* If we are at the end of the stream, mark media as done */
  for (l = writer->medias; l; l = l->next) {
    media = (AnxSource *)l->data;
    if (!anx_media_eos (media)) {
#ifdef DEBUG
      printf ("anx_writer_update_eos: found one not at eos\n");
#endif
      break;
    }
  }
  if (l == NULL) { /* if none found which are not at eos */
#ifdef DEBUG
    printf ("anx_writer_update_eos: all at eos\n");
#endif
    writer->done_media = 1;
  }

  return 0;
}

static long
anx_packet_init (ANNODEX * annodex, unsigned char * buf, long n,
		 long serialno, anx_int64_t granulepos, int bos, int eos,
		 int flush)
{
  AnxWriter * writer = &annodex->x.writer;
  ogg_packet op;
  int ret;

#ifdef DEBUG
  printf ("anx_packet_init: length %ld\tserialno %010ld\tbos %d\n",
	  n, serialno, bos);
#endif

#if 1
  if (anx_media_eos (writer->current_media)) {
    writer->done_media = 1;
  }
#else
  anx_writer_update_eos (annodex);
#endif

  annodex->current_granule = granulepos;
  annodex->current_serialno = serialno;

  op.packet = buf;
  op.bytes = n;
  op.b_o_s = bos;
  op.e_o_s = eos;
  op.granulepos = (ogg_int64_t) granulepos;
  op.packetno = -1;

#ifdef DEBUG
  printf ("anx_packet_init: feeding oggz!\n");
#endif

  ret = oggz_write_feed (annodex->oggz, &op, serialno, flush, NULL);
#ifdef DEBUG
  if (ret < 0) {
    printf ("oggz_write_feed returned %d\n", ret);
  }
#endif

  return 0;
}

static long
anx_init_mapping_packet (ANNODEX * annodex)
{
  unsigned char * mapping;
  char * b;
  anx_int64_t tn, td;

  mapping = anx_malloc (ANX_MAPPING_SIZE);
  memset (mapping, 0, ANX_MAPPING_SIZE);
  b = (char *)mapping;

  strncpy (b, ANX_MAPPING_IDENTIFIER, 8);
  b += 8;

  *(ogg_int16_t *)b = _le_16 (ANX_MAPPING_VERSION_MAJOR);
  b += 2;

  *(ogg_int16_t *)b = _le_16 (ANX_MAPPING_VERSION_MINOR);
  b += 2;

  tn = (anx_int64_t) (annodex->timebase * 1000);
  td = 1000UL;

  *(ogg_int64_t *)b = _le_64 (tn);
  b += 8;

  *(ogg_int64_t *)b = _le_64 (td);
  b += 8;

#ifdef DEBUG
  printf ("anx_init_mapping_packet: timebase %lld/%lld\n[sizeof(AnxMapping) = %d\n",
	  mapping->timebase_numerator, mapping->timebase_denominator,
	  sizeof (*mapping));
#endif

  return anx_packet_init (annodex, mapping, ANX_MAPPING_SIZE,
			  annodex->mapping_serialno,
			  0 /* granulepos */, 1 /* bos */, 0 /* eos */,
			  OGGZ_FLUSH_AFTER);
}

static long
anx_init_track_bos_packet (ANNODEX * annodex, AnxTrack * track)
{
  unsigned char * buf;
  char * b;
  AnxParams * params = NULL;
  size_t params_size, buf_size;

  if (track == NULL) return -1;

  params = anx_params_append (params, "Content-Type", track->content_type);
  params = anx_params_append (params, "ID", track->id);

  /* XXX: Non-C99 kludge */
#if 0
  params_size = anx_params_snprint (NULL, 0, params, ANX_PARAMS_HEADERS) + 1;

  buf_size = ANX_BOS_SIZE + params_size;
#else
  buf_size = 1024;
#endif

  buf = anx_malloc (buf_size);
  b = (char *)buf;

  strncpy ((char *)b, ANX_BOS_IDENTIFIER, 8);
  b += 8;

  *(ogg_int64_t *)b = _le_64 (track->granule_rate_n);
  b += 8;

  *(ogg_int64_t *)b = _le_64 (track->granule_rate_d);
  b += 8;

  *(ogg_int32_t *)b = _le_32 (track->nr_header_packets);
  b += 4;

  b = (char *)buf + ANX_BOS_SIZE;

  /* XXX: Non-C99 kludge */
#if 0
  anx_params_snprint (b, params_size, params, ANX_PARAMS_HEADERS);
#else
  params_size = anx_params_snprint (b, 1024 - ANX_BOS_SIZE, params, ANX_PARAMS_HEADERS);
  buf_size = ANX_BOS_SIZE + params_size;
#endif

  return anx_packet_init (annodex, buf, buf_size, track->serialno,
			  0 /* granulepos */, 1 /* bos */, 0 /* eos */,
			  OGGZ_FLUSH_AFTER);
}

static long
anx_init_wsub_bos_packet (ANNODEX * annodex, AnxWriterTrack * wsub)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxTrack * track = (AnxTrack *)wsub;

  if (wsub == NULL) return -1;

  wsub->written_bos = 1;

  writer->current_track = wsub;
  /*writer->serialno = track->serialno;*/

  return anx_init_track_bos_packet (annodex, track);
}

static long
anx_init_head_packet (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;
  long input_n;

  input_n = (long)anx_head_snprint (NULL, 0, annodex->head);

  if (input_n > writer->anno_buf_len) {
    writer->anno_buf = realloc (writer->anno_buf, input_n);
    writer->anno_buf_len = input_n;
  }

  input_n =
    (long)anx_head_snprint (writer->anno_buf, writer->anno_buf_len,
			    annodex->head);

  return anx_packet_init (annodex, (unsigned char *)writer->anno_buf, input_n,
			  annodex->anno_serialno, 0 /* granulepos */,
			  0 /* bos */, 0 /* eos */,
			  OGGZ_FLUSH_AFTER);
}

static long
anx_init_clip_packet (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxTrack * track = &writer->anno_track;
  AnxCommand command;
  AnxCommandData data;
  double command_time;
  anx_int64_t granulepos;
  long input_n = 0;

  command = annodex->pending_command;
  if (command == NULL) return -1;

  data = anx_command_get_data (annodex, command);
  command_time = anx_command_get_time_offset (annodex, command);
  granulepos = (anx_int64_t)(command_time * track->granule_rate_n /
			     track->granule_rate_d);

  if (data.clip != NULL) {

    input_n = (long)anx_clip_snprint (NULL, 0, data.clip, -1.0, -1.0);

    if (input_n > writer->anno_buf_len) {
      writer->anno_buf = realloc (writer->anno_buf, input_n);
      writer->anno_buf_len = input_n;
    }

    input_n =
      (long)anx_clip_snprint (writer->anno_buf, writer->anno_buf_len,
			      data.clip, -1.0, -1.0);
  }

  return anx_packet_init (annodex, (unsigned char *)writer->anno_buf, input_n,
			  annodex->anno_serialno, granulepos,
			  0 /* bos */, 0 /* eos */,
			  OGGZ_FLUSH_AFTER);
}

static long
anx_init_clear_clip_packet (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxTrack * track = &writer->anno_track;
  AnxCommand command;
  double command_time;
  anx_int64_t granulepos;
  long input_n;

  command = annodex->pending_command;
  if (command == NULL) return -1;

  command_time = anx_command_get_time_offset (annodex, command);
  granulepos = (anx_int64_t)(command_time * track->granule_rate_n /
			     track->granule_rate_d);

  input_n = (long) strlen (EMPTY_CLIP);

  if (input_n > writer->anno_buf_len) {
    writer->anno_buf = realloc (writer->anno_buf, input_n);
    writer->anno_buf_len = input_n;
  }

  input_n = snprintf (writer->anno_buf, writer->anno_buf_len, EMPTY_CLIP);

  return anx_packet_init (annodex, (unsigned char *)writer->anno_buf, input_n,
			  annodex->anno_serialno, granulepos,
			  0 /* bos */, 0 /* eos */,
			  OGGZ_FLUSH_AFTER);
}

static long
anx_writer_fill_media (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxSource * media, * soonest_media = NULL;
  AnxList * l;
  AnxWriterTrack * wsub;
  double media_time, min_time = DBL_MAX;
  int media_index;
  long bound;
  long read_len;
  long input_n;

  if (writer->media_buf_n != -1) {
#ifdef DEBUG
    printf ("anx_writer_fill_media: media_buf_n != -1\n");
#endif
    return -1;
  }

  if (writer->need_bos) {
#ifdef DEBUG
    printf ("anx_writer_fill_media: need_bos\n");
#endif
    return 0;
  }

  bound = -1;

  /* update the current_media */
  for (l = writer->medias; l; l = l->next) {
#ifdef DEBUG
    int i = 0;
#endif

    media = (AnxSource *)l->data;
    media_time = anx_media_tell_time (media);
#ifdef DEBUG
    printf ("anx_write: got media [%d] %p time %f\n", i++, media, media_time);
#endif
    if (media_time != -1 && media_time < min_time) {
      soonest_media = media;
      min_time = media_time;
    }
  }

  if (soonest_media == NULL) return -1;

  writer->current_media = soonest_media;

  /* Read data from the current media, which in turn should set its
   * current track appropriately */

  read_len = anx_media_sizeof_next_read (writer->current_media, bound);

  if (read_len > writer->media_buf_len) {
#ifdef DEBUG
    printf ("realloc media buf (%p) from %ld bytes to %ld bytes\n",
	    writer->media_buf, writer->media_buf_len, read_len);
#endif
    writer->media_buf = realloc (writer->media_buf, read_len);
    writer->media_buf_len = read_len;
  }

  input_n = anx_media_read (writer->current_media, writer->media_buf,
			    read_len, bound);

  writer->media_buf_n = input_n;

  /* Now grab the track details from the media */
  media_index = anx_media_current_track (soonest_media);

#ifdef DEBUG
  printf ("anx_write: media_index %d (soonest %p)\n", media_index,
	  soonest_media);
#endif
  wsub = anx_writer_find_track (annodex, soonest_media, media_index);

  writer->current_track = wsub;
  writer->media_serialno = wsub->sub.serialno;

  anx_writer_update_eos (annodex);

  return 0;
}

static long
anx_init_media_packet (ANNODEX * annodex, int flush)
{
  AnxWriter * writer = &annodex->x.writer;
  long input_n = writer->media_buf_n;

  if (input_n == -1) return -1;

  writer->media_buf_n = -1;

  return anx_packet_init (annodex, writer->media_buf, input_n,
			  writer->media_serialno,
			  anx_media_tell (writer->current_media),
			  0 /* bos */, 0 /* eos */, flush);
}

static long
anx_init_eos_packet (ANNODEX * annodex, long serialno, anx_int64_t granulepos)
{
#ifdef DEBUG
  printf ("anx_write: init eos for (%ld)\n", serialno);
#endif
  return anx_packet_init (annodex, NULL, 0, serialno, granulepos,
			  0 /* bos */, 1 /* eos */, 0 /* flush */);
}

static long
anx_init_clip_eos_packet (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxTrack * track = &writer->anno_track;
  anx_int64_t granulepos;

  granulepos = (anx_int64_t)(anx_tell_time (annodex) *
			     track->granule_rate_n /
			     track->granule_rate_d);

  return anx_init_eos_packet (annodex, annodex->anno_serialno, granulepos);
}


static long
anx_writer_sync (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;
  AnxList * l;
  AnxWriterTrack * wsub;
  AnxTrack * track;
  /*  double time_offset;*/
  double command_offset, current_offset;
  AnxCommandType command_type;
  int made_packet = 0;
  anx_int64_t granulepos;

  if (writer->tracks == NULL) {
#ifdef DEBUG
    printf ("anx_writer_sync: writer->tracks == NULL\n");
#endif
    return -1;
  }

  /* First check if we have ended the current packet; if so create
   * a new one.
   */

  if (writer->done_media == 0) {
    /* Find the next packet and init it ... */
    
    if (anx_writer_fill_media (annodex) == -1) {
#ifdef DEBUG
      printf ("anx_writer_sync: fill_media returned -1\n");
#endif
    }

    if (writer->need_mapping) {
      anx_init_mapping_packet (annodex);
      writer->need_mapping = 0;
      made_packet = 1;
    } else if (writer->need_anno_bos) {
      /*anx_init_anno_bos_packet (annodex);*/
      anx_init_track_bos_packet (annodex, &writer->anno_track);
      writer->need_anno_bos = 0;
      made_packet = 1;
    } else if (writer->need_bos) {
      for (l = writer->tracks; l; l = l->next) {
	wsub = (AnxWriterTrack *)l->data;
	if (!wsub->written_bos) {
	  anx_init_wsub_bos_packet (annodex, wsub);
	  made_packet = 1;
	  break;
	}
      }

      if (l == NULL) {
	anx_init_eos_packet (annodex, annodex->mapping_serialno, 0);
	writer->need_bos = 0;
      }
    } else if (writer->need_head) {
      anx_init_head_packet (annodex);
      writer->need_head = 0;
      made_packet = 1;
    } else if (writer->secondary_headers_remaining > 0) {
      anx_init_media_packet (annodex, OGGZ_FLUSH_AFTER);
      writer->secondary_headers_remaining--;
      made_packet = 1;
    } else if (annodex->pending_command) {

      command_offset =
	anx_command_get_time_offset (annodex, annodex->pending_command);

      current_offset = anx_tell_time (annodex);

      /* While current offset >= pending offset and we haven't made a packet
       * yet, handle next command */

#ifdef DEBUG
      printf ("\nanx_write: [%f] next command at %f\n", current_offset,
	      command_offset);
#endif
      
      while (!made_packet &&
	     ((current_offset > command_offset) ||
	      (current_offset == 0.0 && command_offset == 0.0))) {

	annodex->current_command = annodex->pending_command;

#ifdef DEBUG
	printf ("\t[%f] writing %p\n", current_offset,
		annodex->pending_command);
#endif

	command_type =
	  anx_command_get_type (annodex, annodex->pending_command);

	switch (command_type) {
	case ANX_CMD_NOOP:
	  break;
	case ANX_CMD_INSERT_CLIP:
	  anx_init_clip_packet (annodex);
	  made_packet = 1;
	  break;
	case ANX_CMD_CLEAR_CLIP:
	  anx_init_clear_clip_packet (annodex);
	  made_packet = 1;
	  break;
	case ANX_CMD_LOAD_MEDIA:
	  break;
	case ANX_CMD_UNLOAD_MEDIA:
	  break;
	default:
	  break;
	}

#ifdef DEBUG
	printf ("\t[%f] pending_command before: %p\n",
		current_offset, annodex->pending_command);
#endif
	
	annodex->pending_command =
	  anx_command_next (annodex, annodex->pending_command);

#ifdef DEBUG
	printf ("\t[%f] pending_command after : %p\n",
		current_offset, annodex->pending_command);
#endif

	if (made_packet || annodex->pending_command == NULL) break;

	command_offset =
	  anx_command_get_time_offset (annodex, annodex->pending_command);

	current_offset = anx_tell_time (annodex);

#ifdef DEBUG
	printf ("anx_write: [%f] next command at %f\n", current_offset,
		command_offset);
#endif
      }
    }

    /* Create a media packet */
    if (made_packet == 0) {
      anx_init_media_packet (annodex, 0 /* flush */);
#ifdef DEBUG
      printf (".[%ld]", (long)anx_tell (annodex));
#endif
    }

  } else if (writer->done_media) {
#ifdef DEBUG
    printf ("anx_writer_sync: writer->done_media\n");
#endif
    if (!writer->done_anno_eos) {
      anx_init_clip_eos_packet (annodex);
      writer->done_anno_eos = 1;
      made_packet = 1;
    } else {
      for (l = writer->tracks; l; l = l->next) {
	wsub = (AnxWriterTrack *)l->data;
	if (!wsub->written_eos) {
	  track = (AnxTrack *)wsub;
#ifdef DEBUG
	  printf ("anx_writer_sync: writing eos for %s\n", 
		  track->content_type);
#endif
	  granulepos = (anx_int64_t) anx_tell_time (annodex) * 
	    track->granule_rate_n / track->granule_rate_d;
	  anx_init_eos_packet (annodex, track->serialno, granulepos);
	  wsub->written_eos = 1;
	  made_packet = 1;
	  break;
	}
      }
      
      if (l == NULL) {
#ifdef DEBUG
	printf ("anx_writer_sync: setting writer->eos\n");
#endif
	writer->eos = 1;
      }
    }
  } else {
#ifdef DEBUG
    printf ("anx_writer_sync: HUH?\n");
#endif    
  }

  return writer->eos;
}

static int
anx_write_hungry (OGGZ * oggz, int empty, void * user_data)
{
  ANNODEX * annodex = (ANNODEX *) user_data;

#ifdef DEBUG
  printf ("anx_write_hungry: oggz wants some!\n");
#endif

  return anx_writer_sync (annodex);
}

ANNODEX *
anx_writer_init (ANNODEX * annodex)
{
  AnxWriter * writer = &annodex->x.writer;

#ifdef DEBUG
  printf ("anx_writer_init: %p\n", (void *)annodex);
#endif

  oggz_write_set_hungry_callback (annodex->oggz, anx_write_hungry, 1, annodex);

  writer->current_media = NULL;
  writer->current_track = NULL;

  writer->need_mapping = 1;
  writer->need_bos = 1;
  writer->need_anno_bos = 1;
  writer->need_head = 1;
  writer->secondary_headers_remaining = 0;

  writer->done_media = 0;
  writer->done_anno_eos = 0;
  writer->done_eos = 0;

  /* Initialise the commands list */
  annodex->commands = NULL;
  anx_commands_init (annodex);

  /* Initialise the stream serial numbers */
  srandom (time(NULL));

  annodex->mapping_serialno = random ();
  annodex->mapping_serialno_valid = 1;

  annodex->anno_serialno = random ();
  annodex->anno_serialno_valid = 1;

  writer->medias = NULL;
  writer->tracks = NULL;

  writer->anno_track.serialno = annodex->anno_serialno;
  writer->anno_track.id = NULL;
  writer->anno_track.content_type = cmml_content_type;
  writer->anno_track.nr_header_packets = 1;
  writer->anno_track.granule_rate_n = 1000;
  writer->anno_track.granule_rate_d = 1;

  writer->eos = 0; /* end of stream */

  writer->import_stream_callback = anx_import_stream_default;
  writer->import_head_callback = anx_import_head_default;
  writer->import_clip_callback = anx_import_clip_default;
  writer->import_user_data = annodex;

  writer->import_import_callback = anx_import_import_default;
  writer->import_import_user_data = annodex;

  writer->anno_buf = anx_malloc (102400);
  writer->anno_buf_len = 102400;

  writer->media_buf = anx_malloc (102400);
  writer->media_buf_len = 102400;
  writer->media_buf_n = -1;

  writer->media_serialno = -1;

  return annodex;
}

long
anx_write_output (ANNODEX * annodex, unsigned char * buf, long n)
{
  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1L;
  }

  return oggz_write_output (annodex->oggz, buf, n);
}

long
anx_write (ANNODEX * annodex, long n)
{
  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1L;
  }

  return oggz_write (annodex->oggz, n);
}

long
anx_get_next_page_size (ANNODEX * annodex)
{
  if (annodex == NULL) return -1L;

  return oggz_write_get_next_page_size (annodex->oggz);
}

double
anx_writer_seek_time (ANNODEX * annodex, double seconds, int whence)
{
#if 1
  anx_set_error (annodex, ANX_ERR_NOTIMPLEMENTED);
  return -1;
#else
  long new_offset;

  switch (whence) {
  case ANX_SEEK_SET:
  case ANX_SEEK_CUR: 
  case ANX_SEEK_END:
    /* handled below by importer */
    break;
  case ANX_SEEK_PREV:
  case ANX_SEEK_NEXT:
    anx_set_error (annodex, ANX_ENOTIMPLEMENTED);
    return -1;
    break;
  case ANX_SEEK_HEAD:
  default:
    anx_set_error (annodex, ANX_EINVALID);
    return -1;
    break;
  }

  /* Seek in media */
  new_offset = anx_media_seek (annodex, granule_offset, whence);

  /* Seek in command list */
  anx_command_seek (annodex,
		    anx_track_granules_to_time (annodex, new_offset, NULL));
  
  annodex->current_granule = new_offset;

  return new_offset;
#endif
}

int
anx_writer_seek_id (ANNODEX * annodex, const char * id)
{
  anx_set_error (annodex, ANX_ERR_NOTIMPLEMENTED);
  return -1;
}

#else /* ANX_CONFIG_WRITE */

#include <annodex/anx_types.h>
#include <annodex/anx_constants.h>

int
anx_write_import (ANNODEX * annodex, char * filename,
		   char * id, char * content_type,
		   double seek_offset, double seek_end, int flags)
{
  return ANX_ERR_DISABLED;
}

int
anx_write_set_anno_callbacks (ANNODEX * annodex,
			       AnxImportHead import_head_callback,
			       AnxImportClip import_clip_callback,
			       void * user_data)
{
  return ANX_ERR_DISABLED;
}

int
anx_write_set_ii_callback (ANNODEX * annodex,
			    AnxImportImport import_import_callback,
			    void * user_data)
{
  return ANX_ERR_DISABLED;
}

long
anx_write_output (ANNODEX * annodex, unsigned char * buf, long n)
{
  return ANX_ERR_DISABLED;
}

long
anx_write (ANNODEX * annodex, long n)
{
  return ANX_ERR_DISABLED;
}

long
anx_write_get_next_page_size (ANNODEX * annodex)
{
  return ANX_ERR_DISABLED;
}
  
#endif /* ANX_CONFIG_WRITE */
