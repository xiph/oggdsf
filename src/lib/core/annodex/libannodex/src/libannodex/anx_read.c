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

#if ANX_CONFIG_READ

#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "anx_private.h"

#include "xtag.h"

/*#define DEBUG*/

#define CHUNKSIZE 8500

static off_t anx_reader_seek_head (ANNODEX * annodex);

static int
anx_reader_stop_on_head (ANNODEX * annodex, const AnxHead * head,
			 void * user_data)
{
#ifdef DEBUG
  printf ("anx_reader_stop_on_head()\n");
#endif

  return ANX_STOP_OK;
}

static void
anx_reader_slurp_head (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;
  AnxReadHead read_head;

#ifdef DEBUG
  printf ("anx_reader_slurp_head()\n");
#endif

  read_head = reader->read_head;

  reader->read_head = anx_reader_stop_on_head;

  while (!anx_ready (annodex) && (anx_read (annodex, 128) > 0));

  reader->read_head = read_head;
}

int
anx_set_read_stream_callback (ANNODEX * annodex, AnxReadStream read_stream,
			      void * user_data)
{
  AnxReader * reader;

  if (annodex == NULL) return -1;

  reader = &annodex->x.reader;

  reader->read_stream = read_stream;
  reader->read_stream_user_data = user_data;

  return 0;
}

void *
anx_get_read_stream_user_data (ANNODEX * annodex)
{
  AnxReader * reader;

  if (annodex == NULL) return NULL;

  reader = &annodex->x.reader;

  return reader->read_stream_user_data;
}

int
anx_set_read_track_callback (ANNODEX * annodex, AnxReadTrack read_track,
			     void * user_data)
{
  AnxReader * reader;

  if (annodex == NULL) return -1;

  reader = &annodex->x.reader;

  reader->read_track = read_track;
  reader->read_track_user_data = user_data;

  return 0;
}

void *
anx_get_read_track_user_data (ANNODEX * annodex)
{
  AnxReader * reader;

  if (annodex == NULL) return NULL;

  reader = &annodex->x.reader;

  return reader->read_track_user_data;
}

int
anx_set_read_head_callback (ANNODEX * annodex, AnxReadHead read_head,
			    void * user_data)
{
  AnxReader * reader;

  if (annodex == NULL) return -1;

  reader = &annodex->x.reader;

  reader->read_head = read_head;
  reader->read_head_user_data = user_data;

  return 0;
}

void *
anx_get_read_head_user_data (ANNODEX * annodex)
{
  AnxReader * reader;

  if (annodex == NULL) return NULL;

  reader = &annodex->x.reader;

  return reader->read_head_user_data;
}

int
anx_set_read_clip_callback (ANNODEX * annodex, AnxReadClip read_clip,
			    void * user_data)
{
  AnxReader * reader;

  if (annodex == NULL) return -1;

  reader = &annodex->x.reader;

  reader->read_clip = read_clip;
  reader->read_clip_user_data = user_data;

  return 0;
}

void *
anx_get_read_clip_user_data (ANNODEX * annodex)
{
  AnxReader * reader;

  if (annodex == NULL) return NULL;

  reader = &annodex->x.reader;

  return reader->read_clip_user_data;
}

int
anx_set_read_raw_callback (ANNODEX * annodex, AnxReadRaw read_raw,
			   void * user_data)
{
  AnxReader * reader;

  if (annodex == NULL) return -1;

  reader = &annodex->x.reader;

  reader->read_raw = read_raw;
  reader->read_raw_user_data = user_data;

  return 0;
}

void *
anx_get_read_raw_user_data (ANNODEX * annodex)
{
  AnxReader * reader;

  if (annodex == NULL) return NULL;

  reader = &annodex->x.reader;

  return reader->read_raw_user_data;
}

static int
anx_reader_call_stream_callback (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;
  int cb_ret = 0;

  if (reader->read_stream) {
    cb_ret = reader->read_stream (annodex, annodex->timebase, NULL /* utc */,
				  reader->read_stream_user_data);
  }

  return cb_ret;
}

static int
anx_reader_read_mapping (ANNODEX * annodex, unsigned char * data, long n)
{
  AnxMapping * mapping = (AnxMapping *)data;
  anx_int64_t tn, td;
  /*char * utc = NULL;*/

  if (n < sizeof (AnxMapping)) {
    /* XXX: header too small! */
    return -1;
  }

  tn = _le_64 (mapping->timebase_numerator);
  td = _le_64 (mapping->timebase_denominator);

  annodex->timebase = (double)tn / (double)td;

  return 0;
}

static int
anx_reader_call_track_callback (ANNODEX * annodex, AnxTrack * track)
{
  AnxReader * reader = &annodex->x.reader;
  int cb_ret = 0;

  if (reader->read_track) {
    cb_ret = reader->read_track (annodex, track->serialno,\
				 track->id, track->content_type,
				 track->granule_rate_n,
				 track->granule_rate_d, 
				 track->nr_header_packets,
				 reader->read_track_user_data);
  }

  return cb_ret;
}

static int
anx_reader_call_track_callbacks (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;
  AnxList * l;
  AnxTrack * track;
  int cb_ret = 0;
 
  for (l = reader->tracks; !cb_ret && l; l = l->next) {
    track = (AnxTrack *)l->data;
    cb_ret = anx_reader_call_track_callback (annodex, track);
  }

  return cb_ret;
}

static int
anx_reader_read_track_bos (ANNODEX * annodex, unsigned char * data, long n,
			   long serialno)
{
  AnxReader * reader = &annodex->x.reader;
  AnxBOS * bos = (AnxBOS *)data;
  AnxList * l;
  AnxTrack * track;
  char * buf;
  AnxParams * params;
  char * content_type = NULL, * id = NULL;
  int cb_ret = ANX_CONTINUE;
  
  if (n < ANX_BOS_SIZE) {
    /* XXX: error */
    return -1;
  }

  /* XXX: don't doubly insert track records; this should be unnecessary */
  for (l = reader->tracks; l; l = l->next) {
    track = (AnxTrack *)l->data;
    if (track->serialno == serialno) return 0;
  }

  buf = (char *)data + ANX_BOS_SIZE;
  params = anx_params_new_parse (buf, ANX_PARAMS_HEADERS);
  content_type = anx_params_get (params, "Content-Type");
  id = anx_params_get (params, "ID");

#ifdef DEBUG
  printf ("Got BOS granule_rate %ld/%ld\t%d headers\t%s\t%s\n",
	  (long)bos->granule_rate_numerator,
	  (long)bos->granule_rate_denominator,
	  bos->nr_header_packets, content_type, id);
#endif

  track = anx_malloc (sizeof (AnxTrack));
  track->serialno = serialno;
  track->id = id;
  track->content_type = content_type;
  track->granule_rate_n = _le_64 (bos->granule_rate_numerator);
  track->granule_rate_d = _le_64 (bos->granule_rate_denominator);
  track->nr_header_packets = _le_32 (bos->nr_header_packets);

  reader->tracks = anx_list_append (reader->tracks, track);

#if 0
  if (reader->read_track) {
    cb_ret = reader->read_track (annodex, serialno, id, content_type,
				 track->granule_rate_n,
				 track->granule_rate_d, 
				 track->nr_header_packets,
				 reader->user_data);
#ifdef DEBUG
  } else {
    printf ("anx_reader_read_sub_bos: no read_track callback\n");
#endif
  }
#endif

  if (annodex->anno_serialno_valid == 0 && content_type &&
      !strncmp (content_type, "text/x-cmml", 15)) {
    annodex->anno_serialno_valid = 1;
    annodex->anno_serialno = serialno;
  }
  
  return cb_ret;
}

static AnxHead *
anx_head_from_xtag (XTag * tag)
{
  AnxHead * head;
  AnxMetaElement * meta;
  XTag * child;

  if (tag == NULL) return NULL;

  head = anx_malloc (sizeof(AnxHead));

  head->head_id = xtag_get_attribute (tag, "id");
  head->lang = xtag_get_attribute (tag, "lang");
  head->dir = xtag_get_attribute (tag, "dir");
  head->profile = xtag_get_attribute (tag, "profile");

  child = xtag_first_child (tag, "title");
  head->title = xtag_get_pcdata (child);
  head->title_id = xtag_get_attribute (child, "id");
  head->title_lang = xtag_get_attribute (child, "lang");
  head->title_dir = xtag_get_attribute (child, "dir");

  child = xtag_first_child (tag, "base");
  head->base_id = xtag_get_attribute (child, "id");
  head->base_href = xtag_get_attribute (child, "href");

  head->meta = anx_list_new ();

  for (child = xtag_first_child (tag, "meta"); child;
       child = xtag_next_child (tag, "meta")) {
    meta = anx_malloc (sizeof (AnxMetaElement));
    meta->id = xtag_get_attribute (child, "id");
    meta->lang = xtag_get_attribute (child, "lang");
    meta->dir = xtag_get_attribute (child, "dir");
    meta->name = xtag_get_attribute (child, "name");
    meta->content = xtag_get_attribute (child, "content");
    meta->scheme = xtag_get_attribute (child, "scheme");

    head->meta = anx_list_append (head->meta, meta);
  }

  return head;
}

static AnxClip *
anx_clip_from_xtag (XTag * tag)
{
  AnxClip * clip;
  AnxMetaElement * meta;
  XTag * child;

  clip = anx_malloc (sizeof(AnxClip));

  clip->clip_id = xtag_get_attribute (tag, "id");
  clip->lang = xtag_get_attribute (tag, "lang");
  clip->dir = xtag_get_attribute (tag, "dir");
  clip->track = xtag_get_attribute (tag, "track");

  child = xtag_first_child (tag, "a");
  clip->anchor_id = xtag_get_attribute (child, "id");
  clip->anchor_lang = xtag_get_attribute (child, "lang");
  clip->anchor_dir = xtag_get_attribute (child, "dir");
  clip->anchor_class = xtag_get_attribute (child, "class");
  clip->anchor_href = xtag_get_attribute (child, "href");
  clip->anchor_text = xtag_get_pcdata (child);

  child = xtag_first_child (tag, "img");
  clip->img_id = xtag_get_attribute (child, "id");
  clip->img_lang = xtag_get_attribute (child, "lang");
  clip->img_dir = xtag_get_attribute (child, "dir");
  clip->img_src = xtag_get_attribute (child, "src");
  clip->img_alt = xtag_get_attribute (child, "alt");

  clip->meta = anx_list_new ();

  for (child = xtag_first_child (tag, "meta"); child;
       child = xtag_next_child (tag, "meta")) {
    meta = anx_malloc (sizeof (AnxMetaElement));
    meta->id = xtag_get_attribute (child, "id");
    meta->lang = xtag_get_attribute (child, "lang");
    meta->dir = xtag_get_attribute (child, "dir");
    meta->name = xtag_get_attribute (child, "name");
    meta->content = xtag_get_attribute (child, "content");
    meta->scheme = xtag_get_attribute (child, "scheme");

    clip->meta = anx_list_append (clip->meta, meta);
  }

  child = xtag_first_child (tag, "desc");
  clip->desc_id = xtag_get_attribute (child, "id");
  clip->desc_lang = xtag_get_attribute (child, "lang");
  clip->desc_dir = xtag_get_attribute (child, "dir");
  clip->desc_text = xtag_get_pcdata (child);

  return clip;
}

static int
anx_read_packet (OGGZ * oggz, ogg_packet * op, long serialno, void * user_data)
{
  ANNODEX * annodex = (ANNODEX *)user_data;
  AnxReader * reader = &annodex->x.reader;

  int cb_ret = ANX_CONTINUE;

  XTag * tag;
  char * tag_name;
  AnxClip * clip;
  AnxHead * head;

  annodex->current_serialno = serialno;
  annodex->current_granule = op->granulepos;

#ifdef DEBUG
  {
    unsigned char * c = op->packet;
    printf ("read_packet [%ld] (@%ld): %c%c%c%c ...\n", serialno,
	    (long)op->granulepos, c[0], c[1], c[2], c[3]);
  }
#endif

  if (op->b_o_s) {
#ifdef DEBUG
    printf ("\tread_packet: b_o_s\n");
#endif
    if (op->bytes >= 8) {
      if (!strncmp ((char *)op->packet, ANX_MAPPING_IDENTIFIER, 8)) {
	/* ANNODEX mapping */
	annodex->mapping_serialno_valid = 1;
	annodex->mapping_serialno = serialno;
	anx_reader_read_mapping (annodex, op->packet, op->bytes);
      } else if (!strncmp ((char *)op->packet, ANX_BOS_IDENTIFIER, 8)) {
	/* AnxData BOS */
	cb_ret = anx_reader_read_track_bos (annodex, op->packet, op->bytes,
						serialno);
      } else {
	/* Unknown BOS */
      }
    }
    return cb_ret;

  } else {
    if (reader->got_non_bos == 0) {
      oggz_set_data_start (oggz, oggz_tell (oggz));

      /* If not slurping, ie. this read is under user control, ie. the user
       * has had an opportunity to set track callbacks etc., then,
       * if the track callbacks etc. haven't been called yet, call them.
       */
      if (reader->read_head != anx_reader_stop_on_head) {
	if (!reader->delivered_all_init_callbacks) {
	  anx_reader_call_stream_callback (annodex);
	  anx_reader_call_track_callbacks (annodex);
	  reader->delivered_all_init_callbacks = 1;
	}
      }

      reader->got_non_bos = 1;
    }
  }

  if (annodex->mapping_serialno_valid == 0) {
#ifdef DEBUG
    printf ("\tread_packet: mapping serialno invalid -- breaking out\n");
#endif
    return 0;
  }

  if (annodex->anno_serialno_valid == 0) {
#ifdef DEBUG
    printf ("\tread_packet: anno serialno invalid -- breaking out\n");
#endif
    return 0;
  }

  if (serialno == annodex->anno_serialno) {
      
#ifdef DEBUG
    printf ("anx_read_packet: delivering clip (%ld) length %ld\n",
	    (long)annodex->current_granule, op->bytes);
#endif

    if ((tag = xtag_new_parse ((char *)op->packet, op->bytes)) != NULL) {
#ifdef DEBUG
      printf ("anx_read_packet: parsed ok ... \n");
#endif
      if ((tag_name = xtag_get_name (tag)) != NULL) {
#ifdef DEBUG
	printf ("anx_read_packet: got name ...\n");
#endif
	if (!strncasecmp ("clip", tag_name, 5)) {
#ifdef DEBUG
          printf ("anx_read_packet: is <clip>\n");
#endif
	  if (reader->read_clip) {
	    clip = anx_clip_from_xtag (tag);
	    cb_ret =
	      reader->read_clip (annodex, clip, reader->read_clip_user_data);
	  }
	} else if (!strncasecmp ("head", tag_name, 5)) {
#ifdef DEBUG
          printf ("anx_read_packet: is <head>\n");
#endif
	  reader->offset_head = reader->offset;

	  head = anx_head_from_xtag (tag);

	  if (reader->read_head) {
	    cb_ret = reader->read_head (annodex, head,
					reader->read_head_user_data);
	  }

          anx_set_head (annodex, head);
	}
      }
      xtag_free (tag);
    }
  } else if (serialno != annodex->mapping_serialno) {
#ifdef DEBUG
    printf ("\tread_packet: delivering media (%ld)\n",
	    (long)annodex->current_granule);
#endif
    if (reader->read_raw) {
      cb_ret = reader->read_raw (annodex, op->packet, op->bytes,
				 serialno, op->granulepos,
				 reader->read_raw_user_data);
    }
  }

  return cb_ret;
}

static ogg_int64_t
anx_reader_time_to_units (ANNODEX * annodex, double seconds)
{
  return (ogg_int64_t)(1000L * seconds);
}

static double
anx_reader_units_to_time (ANNODEX * annodex, ogg_int64_t units)
{
  return ((double)units / 1000.0);
}

/* An OGGZ metric for seeking on Ogg framing */
static ogg_int64_t
anx_reader_metric (OGGZ * oggz, long serialno, ogg_int64_t granulepos,
		   void * user_data)
{
  ANNODEX * annodex = (ANNODEX *)user_data;
  int err;
  anx_int64_t granule_rate_n, granule_rate_d;
  double seconds;
  ogg_int64_t units;

  err =
    anx_track_get_granule_rate (annodex, serialno,
				&granule_rate_n, &granule_rate_d);
  if (err == -1) return -1L;

  seconds =
    (double)granulepos * (double)granule_rate_d / (double)granule_rate_n;

  units = anx_reader_time_to_units (annodex, seconds);

  return units;
}

ANNODEX *
anx_reader_init (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;

  /* Set all callbacks and user data etc. to NULL / 0 */
  memset (reader, 0, sizeof (AnxReader));

  reader->tracks = NULL;
  reader->got_non_bos = 0;
  reader->delivered_all_init_callbacks = 0;

  reader->eos = 0;

  reader->offset = 0;
  reader->offset_head = 0;

  oggz_set_read_callback (annodex->oggz, -1, anx_read_packet, annodex);
  oggz_set_metric (annodex->oggz, -1, anx_reader_metric, annodex);

#ifdef DEBUG
  printf ("anx_reader_init: annodex->slurpable? %s\n",
	  annodex->slurpable ? "YES" : "NO");
#endif

  if (annodex->slurpable) {
    anx_reader_slurp_head (annodex);
    anx_reader_seek_head (annodex);
    reader->got_non_bos = 0;
  }

  return annodex;
}

long
anx_read (ANNODEX * annodex, long n)
{
  if (annodex == NULL || annodex->oggz == NULL) return -1;

  return oggz_read (annodex->oggz, n);
}

/* generic */
long
anx_read_input (ANNODEX * annodex, unsigned char * buf, long n)
{
  if (annodex == NULL || annodex->oggz == NULL) return -1;

  return oggz_read_input (annodex->oggz, buf, n);
}

double
anx_reader_seek_time (ANNODEX * annodex, double seconds, int whence)
{
  ogg_int64_t units, units_at;
  int err;

  if (annodex == NULL || annodex->oggz == NULL) return -1;

#ifdef ANX_CONFIG_READ
  switch (whence) {
  case ANX_SEEK_HEAD:
    if (seconds == 0.0) {
      err = anx_reader_seek_head (annodex);
      if (err == -1) return -1.0;
      else return 0.0; /* XXX: timebase? */
    } else {
      whence = ANX_SEEK_SET;
    }
    /* fall through */
  default:

    units = anx_reader_time_to_units (annodex, seconds);
    units_at = oggz_seek_units (annodex->oggz, units, whence);
    if (units_at == -1) return -1.0;
    
    return anx_reader_units_to_time (annodex, units);
    break;
  }
#else
  return ANX_ERR_DISABLED;
#endif
}

static off_t
anx_reader_seek_head (ANNODEX * annodex)
{
  /* seek to the very beginning, to allow delivery of the head packet */
  return oggz_seek (annodex->oggz, 0, SEEK_SET);
}

int
anx_reader_seek_id (ANNODEX * annodex, const char * id)
{
  anx_set_error (annodex, ANX_ERR_NOTIMPLEMENTED);
  return -1;
}


/*
 *  *** ARCHIVAL FROM HERE DOWN -- need to re-implement clip seeking ***
 */

#if 0
static long
anx_reader_seek_next_clip (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;
  off_t offset_orig, offset_at, offset_next;
  long packet_at = -1;
  ogg_page * og;

#ifdef DEBUG
  printf ("anx_reader_seek_next_clip() ...\n");
#endif

#if 0
  offset_at = anx_reader_tell_raw (annodex);
  if (offset_at == -1) return -1;

  offset_orig = offset_at;
#else
  offset_orig = reader->offset;
#endif

  og = &reader->current_page;

  while (1) {
    offset_next = anx_reader_get_next_start_page (annodex, og);

    if (offset_next < 0) {
      goto notfound;
    }

    if (offset_next == 0) {
      goto notfound;
    }

    offset_at = offset_next;
    packet_at = ogg_page_granulepos (og);

    if (ogg_page_serialno (og) == annodex->anno_serialno) {
      if (offset_next != offset_orig) break;
    }
  }

#ifdef DEBUG
  printf ("FOUND (%ld)\n", packet_at);
#endif

  offset_at = anx_reader_reset (annodex, offset_at, packet_at);
  if (offset_at == -1) return -1;

  return annodex->current_granule;

 notfound:
#ifdef DEBUG
  printf ("NOT FOUND\n");
#endif

  anx_reader_reset (annodex, offset_orig, -1);

  return -1;
}

static long
anx_reader_seek_prev_clip (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;
  off_t offset_orig, offset_start, offset_at, offset_next;
  long packet_at = -1, packet_next;
  long serialno;
  ogg_page * og;

#ifdef DEBUG
  printf ("anx_reader_seek_prev_clip() ...\n");
#endif

#if 0
  offset_at = anx_reader_tell_raw (annodex);
  if (offset_at == -1) return -1;

  offset_orig = offset_at;
#else
  offset_orig = reader->offset;
#endif

  og = &reader->current_page;

  offset_start = reader->offset;

  while (1) {

    offset_next = anx_reader_get_prev_start_page (annodex, og, &packet_next,
						  &serialno);

    if (offset_next < 0) {
      goto notfound;
    }

    if (offset_next == 0) {
      goto notfound;
    }

    offset_at = offset_next;
    /* packet_at = ogg_page_granulepos (og); */
    packet_at = packet_next;

#ifdef DEBUG
    printf ("prev_clip: checking @%ld (%ld) [%s]\n",
	    offset_at, packet_at,
	    (serialno == annodex->anno_serialno) ? "anno" : "media");
#endif
    
    if (serialno == annodex->anno_serialno) {
      if (offset_next != offset_orig) break;
    }

    offset_start = offset_next;

    if (offset_start < 0) offset_start = 0;
    
    offset_at = anx_reader_seek_raw (annodex, offset_start, SEEK_SET);
    if (offset_at == -1) goto notfound;
  }

#ifdef DEBUG
  printf ("FOUND (%ld)\n", packet_at);
#endif

  offset_at = anx_reader_reset (annodex, offset_at, packet_at);
  if (offset_at == -1) return -1;

  return annodex->current_granule;

 notfound:
#ifdef DEBUG
  printf ("NOT FOUND\n");
#endif

  anx_reader_reset (annodex, offset_orig, -1);

  return -1;
}

long
anx_reader_seek (ANNODEX * annodex, long granule_offset, int whence)
{
  off_t offset_at;

  switch (whence) {
  case ANX_SEEK_SET:
    return anx_reader_seek_set (annodex, granule_offset);
    break;
  case ANX_SEEK_CUR: 
    granule_offset += annodex->current_granule;
    return anx_reader_seek_set (annodex, granule_offset);
    break;
  case ANX_SEEK_END:
    return anx_reader_seek_end (annodex, granule_offset);
    break;
  case ANX_SEEK_HEAD:
    if (granule_offset == 0)
      return anx_reader_seek_head (annodex);
    else
      return anx_reader_seek_set (annodex, granule_offset);
    break;
  case ANX_SEEK_PREV:
    offset_at = anx_reader_seek_prev_clip (annodex);
    if (offset_at != -1 && granule_offset != 0) {
      offset_at = anx_reader_seek_set (annodex, offset_at + granule_offset);
    }
    return offset_at;
    break;
  case ANX_SEEK_NEXT:
    offset_at = anx_reader_seek_next_clip (annodex);
    if (offset_at != -1 && granule_offset != 0) {
      offset_at = anx_reader_seek_set (annodex, offset_at + granule_offset);
    }
    return offset_at;
    break;
  default:
    anx_set_error (annodex, ANX_EINVALID);
    return -1;
    break;
  }

}

struct seek_id_data {
  char * id;
  int got_clip;
  int id_match;
};

static int
anx_reader_cmp_clip_id (ANNODEX * annodex, const AnxClip * clip,
			void * user_data)
{
  struct seek_id_data * sid = (struct seek_id_data *)user_data;

  sid->got_clip = 1;

  if (clip->clip_id != NULL && !strcasecmp (clip->clip_id, sid->id)) {
    sid->id_match = 1;
  } else {
    sid->id_match = 0;
  }

  return ANX_STOP_OK;
}

int
anx_reader_seek_id (ANNODEX * annodex, const char * id)
{
  AnxReader * reader = &annodex->x.reader;
  struct seek_id_data sid;
  void * user_data;
  AnxReadClip read_clip;
  off_t offset_orig, offset_at;
  long packet_at = -1;

  read_clip = reader->read_clip;
  user_data = reader->user_data;

  reader->read_clip = anx_reader_cmp_clip_id;
  reader->user_data = &sid;

  sid.id = (char *) id;
  sid.got_clip = 0;
  sid.id_match = 0;
 
  offset_orig = reader->offset;

  /* start from the beginning */
  offset_at = anx_reader_seek_set (annodex, 0);

  packet_at = 0;

  while (packet_at != -1 && sid.id_match == 0) {
    sid.got_clip = 0;
    packet_at = anx_reader_seek_next_clip (annodex);
    if (packet_at != -1) {
      while (sid.got_clip == 0 && anx_read (annodex, 128) > 0);
      if (sid.id_match) anx_reader_seek_prev_clip (annodex);
    }
  }

  if (sid.id_match == 0) {
    anx_reader_reset (annodex, offset_orig, -1);
  }

  reader->read_clip = read_clip;
  reader->user_data = user_data;

  return 0;
}
#endif /* 0 */

#else /* ANX_CONFIG_READ */

#include <annodex/anx_types.h>
#include <annodex/anx_read.h>
#include <annodex/anx_constants.h>

int
anx_set_read_stream_callback (ANNODEX * annodex, AnxReadStream read_stream)
{
  return ANX_ERR_DISABLED;
}

int
anx_set_read_track_callback (ANNODEX * annodex, AnxReadTrack read_track)
{
  return ANX_ERR_DISABLED;
}

int
anx_set_read_head_callback (ANNODEX * annodex, AnxReadHead read_head)
{
  return ANX_ERR_DISABLED;
}

int
anx_set_read_clip_callback (ANNODEX * annodex, AnxReadClip read_clip)
{
  return ANX_ERR_DISABLED;
}

int
anx_set_read_media_callback (ANNODEX * annodex, AnxReadMedia read_media)
{
  return ANX_ERR_DISABLED;
}

int
anx_set_user_data (ANNODEX * annodex, void * user_data)
{
  return ANX_ERR_DISABLED;
}

long
anx_read (ANNODEX * annodex, long n)
{
  return ANX_ERR_DISABLED;
}

long
anx_reader_input (ANNODEX * annodex, unsigned char * buf, long n)
{
  return ANX_ERR_DISABLED;
}

#endif
