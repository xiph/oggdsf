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


#include <config.h>

#include <time.h>

#include <limits.h>

#include <stdlib.h>
#define anx_malloc malloc
#define anx_free free 

#include <string.h>

/*#define DEBUG*/

#define MIN(a,b) ((a)<(b)?(a):(b))

#include <oggz/oggz.h>

#include <annodex/anx_import.h>
#include <annodex/annodex.h>

#define INT32_LE_AT(x) _le_32((*(ogg_int32_t *)(x)))
#define INT32_BE_AT(x) _be_32((*(ogg_int32_t *)(x)))
#define INT64_LE_AT(x) _le_64((*(ogg_int64_t *)(x)))

#define VORBIS_CONTENT_TYPE "audio/x-vorbis"
#define SPEEX_CONTENT_TYPE "audio/x-speex"
#define THEORA_CONTENT_TYPE "video/x-theora"
#define XVID_CONTENT_TYPE "video/x-xvid"

#undef  USE_THEORA_PRE_ALPHA_3_FORMAT

#define SUBSECONDS 1000L

typedef struct _AnxOggMedia AnxOggMedia;
typedef struct _AnxOggData AnxOggData;

struct _AnxOggMedia {
  long length;
  unsigned char * data;
  long granulepos;
  AnxSourceTrack * media_track;
};

struct _AnxOggData {
  OGGZ * oggz;

  AnxSource * anx_media;

  char * id;
  int ignore_media;

  int got_non_bos;

  long nr_headers_remaining;

  int need_seek;
  int got_end;

  OggzTable * logicals;

  AnxList * media_packets;
  long current_offset; /* offset into current packet */
};

static AnxImporter anxogg_importer;

static ogg_uint32_t
_le_32 (ogg_uint32_t i)
{
   ogg_uint32_t ret=i;
#ifdef WORDS_BIGENDIAN
   ret =  (i>>24);
   ret += (i>>8) & 0x0000ff00;
   ret += (i<<8) & 0x00ff0000;
   ret += (i<<24);
#endif
   return ret;
}

static ogg_uint32_t
_be_32 (ogg_uint32_t i)
{
   ogg_uint32_t ret=i;
#ifndef WORDS_BIGENDIAN
   ret =  (i>>24);
   ret += (i>>8) & 0x0000ff00;
   ret += (i<<8) & 0x00ff0000;
   ret += (i<<24);
#endif
   return ret;
}

static double
anxogg_seek_update (AnxSource * media)
{
  AnxOggData * aod = (AnxOggData *)media->custom_data;
  long units;
  double offset;

  units = (long)(SUBSECONDS * media->start_time);
  offset = oggz_seek_units (aod->oggz, units, SEEK_SET);

  aod->need_seek = 0;

  if (media->end_time != -1.0 && offset >= media->end_time) {
    aod->got_end = 1;
  }

  return offset;
}

static int
read_packet (OGGZ * oggz, ogg_packet * op, long serialno, void * user_data)
{
  unsigned char * header = op->packet;
  AnxOggData * aod = (AnxOggData *)user_data;
  AnxSource * m = aod->anx_media;
  AnxSourceTrack * track = NULL;
  AnxOggMedia * aam;

  if (op->b_o_s) {
    track = (AnxSourceTrack *) anx_malloc (sizeof (AnxSourceTrack));
    memset (track, 0, sizeof(AnxSourceTrack));

    if (!strncmp ((char *)&header[1], "vorbis", 6)) {
      track->content_type = VORBIS_CONTENT_TYPE;
      track->granule_rate_n = (anx_int64_t) INT32_LE_AT(&header[12]);
      track->granule_rate_d = 1;
      track->nr_header_packets = 3;
    } else if (!strncmp ((char *)&op->packet[0], "Speex   ", 8)) {
      track->content_type = SPEEX_CONTENT_TYPE;
      track->granule_rate_n = (ogg_int64_t) INT32_LE_AT(&header[36]);
      track->granule_rate_d = 1;
      track->nr_header_packets = 2 + (ogg_int64_t) INT32_LE_AT(&header[68]);
    } else if (!strncmp ((char *)&op->packet[1], "theora", 6)) {
      char keyframe_granule_shift = 0;
      track->content_type = THEORA_CONTENT_TYPE;
#if USE_THEORA_PRE_ALPHA_3_FORMAT
      /* old header format, used by Theora alpha 2 and earlier */
      keyframe_granule_shift =  (header[36] & 0xf8) >> 3;
#else
      keyframe_granule_shift =  (header[40] & 0x03) /* 00000011 */ << 3;
      keyframe_granule_shift |= (header[41] & 0xe0) /* 11100000 */ >> 5;
#endif
      track->granule_rate_n = INT32_BE_AT(&header[22]) *
	(1 << keyframe_granule_shift) / 1000000;
      track->granule_rate_d = INT32_BE_AT(&header[26]) / 1000000;
      track->nr_header_packets = 3;
    } else if (!strncmp ((char *)&op->packet[1], "video", 5) && 
	       !strncmp ((char *)&op->packet[9], "XVID", 4)) {
      track->content_type = XVID_CONTENT_TYPE;
      track->granule_rate_n = (ogg_int64_t) 25;
      track->granule_rate_d = 1;
      track->nr_header_packets = 2;
    } else {
      anx_free (track);
      return -1;
    }

    aod->nr_headers_remaining += track->nr_header_packets;

    oggz_table_insert (aod->logicals, serialno, track);

#ifdef DEBUG
    printf ("Added track for (%ld): %ld/%ld, +%d headers\n", serialno,
	    (long)track->granule_rate_n, (long)track->granule_rate_d,
	    (int)track->nr_header_packets);
#endif

    /* XXX: fix this for theora */
    if (track->granule_rate_n != 0 && track->granule_rate_d != 0) {
      if (m->start_time != 0.0) {
	track->start_granule =
	  m->start_time * track->granule_rate_n /
	  track->granule_rate_d;
      } else {
	track->start_granule = 0;
      }

      if (m->end_time != -1.0) {
	track->end_granule =
	  m->end_time * track->granule_rate_n /
	  track->granule_rate_d;
      }
    }

#ifdef DEBUG
    printf ("anxogg: adding track for %s\n", track->content_type);
#endif
    m->tracks = anx_list_append (m->tracks, track);

  } else {
    if (aod->got_non_bos == 0) {
      oggz_set_data_start (oggz, oggz_tell (oggz));
    }
    aod->got_non_bos = 1;

    track = (AnxSourceTrack *) oggz_table_lookup (aod->logicals, serialno);
  }

  if (!track) return -1;

  if (aod->nr_headers_remaining == 0 && !aod->got_end && 
      (m->end_time != -1.0) && (op->granulepos != -1) &&
      (op->granulepos >= track->end_granule)) {
    aod->got_end = 1;
  }

  if (!aod->ignore_media && !aod->got_end) {
    aam = anx_malloc (sizeof (AnxOggMedia));
    aam->length = op->bytes;
    aam->data = anx_malloc (op->bytes);
    aam->granulepos = op->granulepos;
    aam->media_track = track;

    if (aod->nr_headers_remaining == 0 && aam->granulepos != -1) {
      aam->granulepos -= track->start_granule;
    }

    memcpy (aam->data, op->packet, op->bytes);
    
    aod->media_packets = anx_list_append (aod->media_packets, aam);

    if (aod->nr_headers_remaining > 0) {
      aod->nr_headers_remaining--;

      if (aod->nr_headers_remaining == 0 && m->start_time != 0.0) {
	aod->need_seek = 1;
	return 1;
      }
    }
  }

  return 0;
}

static AnxSource *
anxogg_open (const char * path, const char * id, int ignore_media,
	     double start_time, double end_time,
	     AnxImportCallbacks * import_callbacks)
{
  AnxSource * m;
  AnxOggData * aod;
  OGGZ * oggz;
  long n;

  m = (AnxSource *) anx_malloc (sizeof (AnxSource));

  m->importer = &anxogg_importer;

  m->eos = 0;
  m->start_time = start_time;
  m->end_time = end_time;

  m->tracks = NULL;
  m->current_track = NULL;

  /* PADDING::: Fix here */
  aod = anx_malloc (sizeof (AnxOggData));

#ifdef DEBUG
  printf ("anx_ogg: open %s\n", path);
#endif

  if ((oggz = oggz_open ((char *)path, OGGZ_READ | OGGZ_AUTO)) == NULL)
    return NULL;

  aod->oggz = oggz;
  aod->anx_media = m;

  aod->id = (char *)id;

  aod->ignore_media = ignore_media;

  aod->media_packets = NULL;
  aod->current_offset = 0;

  aod->got_non_bos = 0;
  aod->need_seek = 0;
  aod->got_end = 0;
  aod->nr_headers_remaining = 0;

  aod->logicals = oggz_table_new ();

  if (ignore_media) {
    oggz_set_read_callback (oggz, -1, read_packet, aod);
    while ((n = oggz_read (oggz, 1024)) > 0) {
    }
  } else {
    oggz_set_read_callback (oggz, -1, read_packet, aod);
    while ((!(aod->got_non_bos && aod->nr_headers_remaining == 0)) &&
	   (n = oggz_read (oggz, 1024)) > 0) {
    }
  }

  m->custom_data = aod;

  return m;
}

static long
anxogg_read (AnxSource * media, char * buf, long n, long bound)
{
  AnxOggData * aod = (AnxOggData *)media->custom_data;
  AnxOggMedia * aam;
  long bytes_to_read;

  if (aod->ignore_media) return -1;

 do_read:

  while ((aod->media_packets == NULL) && (oggz_read (aod->oggz, 1024)) > 0);

  if (aod->need_seek && aod->nr_headers_remaining == 0) {
    anxogg_seek_update (media);
    goto do_read;
  }

  if (aod->media_packets == NULL) {
    media->eos = 1;
    return 0;
  }

  aam = (AnxOggMedia *)aod->media_packets->data;
  bytes_to_read = MIN (n, aam->length - aod->current_offset);

  memcpy (buf, &aam->data[aod->current_offset], bytes_to_read);

  aod->current_offset += bytes_to_read;

  media->current_track = aam->media_track;

#ifdef DEBUG
  printf ("anxogg: reading from stream %s\n",
	  media->current_track->content_type);
#endif

  if (aam->granulepos != -1) {
    media->current_track->current_granule = aam->granulepos;
  }

  /* If that's finished this media packet, advance to the next one */
  if (aod->current_offset >= aam->length) {
    aod->media_packets =
      anx_list_remove (aod->media_packets, aod->media_packets);

    anx_free (aam->data);
    anx_free (aam);

    aod->current_offset = 0;
  }

  return bytes_to_read;
}

static long
anxogg_sizeof_next_read (AnxSource * media, long bound)
{
  AnxOggData * aod = (AnxOggData *)media->custom_data;
  AnxOggMedia * aam;
  long bytes_to_read;

  if (aod->ignore_media) return -1;

  while ((aod->media_packets == NULL) && (oggz_read (aod->oggz, 1024)) > 0);

  if (aod->media_packets == NULL) {
    media->eos = 1;
    return 0;
  }

  aam = (AnxOggMedia *)aod->media_packets->data;
  bytes_to_read = aam->length - aod->current_offset;

  return bytes_to_read;
}

static int
anxogg_close (AnxSource * media)
{
  AnxOggData * aod = (AnxOggData *)media->custom_data;

  oggz_close (aod->oggz);

  anx_free (media);

  return 0;
}

static struct _AnxImporter anx_vorbis_importer = {
  (AnxImporterOpenFunc)anxogg_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxogg_close,
  (AnxImporterReadFunc)anxogg_read,
  (AnxImporterSizeofNextReadFunc)anxogg_sizeof_next_read,
  VORBIS_CONTENT_TYPE
};

static struct _AnxImporter anx_speex_importer = {
  (AnxImporterOpenFunc)anxogg_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxogg_close,
  (AnxImporterReadFunc)anxogg_read,
  (AnxImporterSizeofNextReadFunc)anxogg_sizeof_next_read,
  SPEEX_CONTENT_TYPE
};

static struct _AnxImporter anx_theora_importer = {
  (AnxImporterOpenFunc)anxogg_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxogg_close,
  (AnxImporterReadFunc)anxogg_read,
  (AnxImporterSizeofNextReadFunc)anxogg_sizeof_next_read,
  THEORA_CONTENT_TYPE
};

static struct _AnxImporter anx_xvid_importer = {
  (AnxImporterOpenFunc)anxogg_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxogg_close,
  (AnxImporterReadFunc)anxogg_read,
  (AnxImporterSizeofNextReadFunc)anxogg_sizeof_next_read,
  XVID_CONTENT_TYPE
};

AnxImporter *
anx_importer_init (int i)
{
  switch (i) {
  case 0:
    return &anx_vorbis_importer;
  case 1:
    return &anx_speex_importer;
  case 2:
    return &anx_theora_importer;
  case 3:
    return &anx_xvid_importer;
  default:
    return NULL;
  }
}

