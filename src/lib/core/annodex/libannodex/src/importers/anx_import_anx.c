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



#include <time.h>
#include <string.h>
#include <limits.h>

#include <stdlib.h>
#define anx_malloc malloc
#define anx_free free 

#define MIN(a,b) ((a)<(b)?(a):(b))

#include <annodex/anx_import.h>
#include <annodex/annodex.h>

/*#define DEBUG*/

typedef struct _AnxAnxSource AnxAnxSource;
typedef struct _AnxAnxTrack AnxAnxTrack;
typedef struct _AnxAnxData AnxAnxData;

struct _AnxAnxSource {
  long length;
  unsigned char * data;
  long granulepos;
  AnxSourceTrack * media_track;
};

struct _AnxAnxTrack {
  long serialno;
  AnxSourceTrack * media_track;
};

struct _AnxAnxData {
  ANNODEX * anx_reader;
  AnxSource * anx_media;

  int ignore_media;

  AnxImportStream import_stream;
  AnxImportHead import_head;
  AnxImportClip import_clip;
  void * import_user_data;

  int need_seek;
  int got_end;
  long nr_headers_remaining;

  AnxList * tracks;

  AnxList * media_packets;
  long current_offset; /* offset into current packet */
};

static AnxImporter anxanx_importer;

static AnxAnxTrack *
add_track (AnxAnxData * aad, long serialno, AnxSourceTrack * m_track)
{
  AnxAnxTrack * track;

  track = (AnxAnxTrack *) anx_malloc (sizeof (AnxAnxTrack));
  if (track == NULL) return NULL;

  track->serialno = serialno;
  track->media_track = m_track;

  aad->tracks = anx_list_append (aad->tracks, track);

  aad->nr_headers_remaining += m_track->nr_header_packets;

  return track;
}

static AnxAnxTrack *
find_track (AnxAnxData * aad, long serialno)
{
  AnxList * l;
  AnxAnxTrack * track;

  if (aad == NULL) return NULL;

  for (l = aad->tracks; l; l = l->next) {
    track = (AnxAnxTrack *)l->data;

    if (track->serialno == serialno) return track;
  }

  return NULL;
}

static double
anxanx_seek_update (AnxSource * media)
{ 
  AnxAnxData * aad = (AnxAnxData *)media->custom_data;
  double offset;

  offset = anx_seek_time (aad->anx_reader, media->start_time, ANX_SEEK_SET);

  aad->need_seek = 0;
  
  if (media->end_time != -1.0 && offset >= media->end_time) {
    aad->got_end = 1;
  }

  return offset;
}

static int
read_stream (ANNODEX * anx, double timebase, char * utc, void * user_data)
{
  AnxAnxData * aad = (AnxAnxData *)user_data;
  AnxSource * media = aad->anx_media;

  if (aad->import_stream != NULL) {
    timebase += media->start_time;

    aad->import_stream (timebase, utc, aad->import_user_data);
  }

  return 0;
}

static int
read_track (ANNODEX * anx, long serialno, char * id, char * content_type,
		anx_int64_t granule_rate_n, anx_int64_t granule_rate_d,
		int nr_header_packets, void * user_data)
{
  AnxAnxData * aad = (AnxAnxData *)user_data;
  AnxSource * media = aad->anx_media;
  AnxSourceTrack * track = NULL;

#ifdef DEBUG
  printf ("anxanx: read track (%ld) %s\n", serialno, content_type);
#endif

  if (!strncmp (content_type, "text/x-cmml", 12)) return 0;

  track = (AnxSourceTrack *) anx_malloc (sizeof (AnxSourceTrack));
  if (track == NULL) return -1;

  track->id = id;
  track->content_type = content_type;
  track->nr_header_packets = nr_header_packets;
  track->granule_rate_n = granule_rate_n;
  track->granule_rate_d = granule_rate_d;

  if (track->granule_rate_n != 0 && track->granule_rate_d != 0) {
    if (media->start_time != 0.0) {
      track->start_granule =
	media->start_time * track->granule_rate_n /
	track->granule_rate_d;
    }
    
    if (media->end_time != -1.0) {
      track->end_granule =
	media->end_time * track->granule_rate_n /
	track->granule_rate_d;
    }
  }

  if (add_track (aad, serialno, track) == NULL) return -1;

  media->tracks = anx_list_append (media->tracks, track);

  return 0;
}

static int
read_head (ANNODEX * anx, const AnxHead * head, void * user_data)
{
  AnxAnxData * aad = (AnxAnxData *)user_data;

  if (aad->import_head != NULL) {
    aad->import_head ((AnxHead *)head, aad->import_user_data);
  }

  return 0;
}

static int
read_clip (ANNODEX * anx, const AnxClip * clip, void * user_data)
{
  AnxAnxData * aad = (AnxAnxData *)user_data;
  /*AnxSource * media = aad->anx_media;*/
  double t;

  if (aad->import_clip != NULL) {
    t = anx_tell_time (anx);
#ifdef DEBUG
    printf ("anxanx: importing clip at %f\n", t);
#endif
    aad->import_clip ((AnxClip *)clip, t, aad->import_user_data);
  }

  return 0;
}

static int
read_raw (ANNODEX * anx, unsigned char * data, long n,
	  long serialno, anx_int64_t current_granule, void * user_data)
{
  AnxAnxData * aad = (AnxAnxData *)user_data;
  AnxAnxSource * aam;
  AnxSource * m = aad->anx_media;
  AnxAnxTrack * track;
  AnxSourceTrack * media_track;

  track = find_track (aad, serialno);
  if (track == NULL) {
#ifdef DEBUG
    printf ("anxanx: track %ld not found\n", serialno);
#endif
    return -1;
  }
  media_track = track->media_track;

  if ((aad->nr_headers_remaining == 0) && !aad->got_end && 
      (m->end_time != -1) && (current_granule != -1) &&
      (current_granule >= media_track->end_granule)) {
    aad->got_end = 1;
  }

  if (aad->got_end) {
#ifdef DEBUG
    printf ("anxanx: skipping (got_end)\n");
#endif
    return 0;
  }

  aam = anx_malloc (sizeof (AnxAnxSource));
  aam->length = n;
  aam->data = anx_malloc (n);
  aam->granulepos = anx_tell (anx);
  aam->media_track = media_track;

#if 0
  if (aad->nr_headers_remaining == 0) {
    aam->granulepos -= media_track->start_granule;
  }
#endif

#ifdef DEBUG
  printf ("anxanx: stored granulepos %ld\n", aam->granulepos);
#endif

  memcpy (aam->data, data, n);

  aad->media_packets = anx_list_append (aad->media_packets, aam);

  if (aad->nr_headers_remaining > 0) {
    aad->nr_headers_remaining--;

    if (aad->nr_headers_remaining == 0) {
      aad->need_seek = 1;
      return ANX_STOP_OK;
    }
  }

  return 0;
}

static AnxSource *
anxanx_open (const char * path, const char * id, int ignore_media,
	     double start_time, double end_time,
	     AnxImportCallbacks * import_callbacks)
{
  AnxSource * m;
  AnxAnxData * aad;
  ANNODEX * anxr;
  long n;

  m = (AnxSource *) anx_malloc (sizeof (AnxSource));

  m->importer = &anxanx_importer;
  m->tracks = NULL;
  m->current_track = NULL;

  m->eos = 0;
  m->start_time = start_time;
  m->end_time = end_time;

  aad = anx_malloc (sizeof (AnxAnxData));
  m->custom_data = aad;

  if ((anxr = anx_open ((char *)path, ANX_READ)) == NULL)
    return NULL;

  aad->anx_reader = anxr;
  aad->anx_media = m;

  aad->ignore_media = ignore_media;

  aad->import_stream = import_callbacks->import_stream;
  aad->import_head = import_callbacks->import_head;
  aad->import_clip = import_callbacks->import_clip;
  aad->import_user_data = import_callbacks->import_user_data;

  aad->need_seek = 0;
  aad->got_end = 0;
  aad->nr_headers_remaining = 0;

  aad->tracks = NULL;

  aad->media_packets = NULL;
  aad->current_offset = 0;

  anx_set_read_stream_callback (anxr, read_stream, aad);
  anx_set_read_track_callback (anxr, read_track, aad);
  anx_set_read_head_callback (anxr, read_head, aad);
  anx_set_read_clip_callback (anxr, read_clip, aad);

  if (ignore_media) {
    while ((n = anx_read (anxr, 1024)) > 0);
  } else {
    anx_set_read_raw_callback (anxr, read_raw, aad);
    while (!anx_ready (anxr) && (n = anx_read (anxr, 1024)) > 0);
  }


  return m;
}

static long
anxanx_read (AnxSource * media, char * buf, long n, long bound)
{
  AnxAnxData * aad = (AnxAnxData *)media->custom_data;
  AnxAnxSource * aam;
  long bytes_to_read;

  if (aad->ignore_media) return -1;

  if ((aad->media_packets == NULL) && (aad->got_end)) {
    media->eos = 1;
    return 0;
  }

 do_read:

  while ((aad->media_packets == NULL) &&
	 (anx_read (aad->anx_reader, 1024)) > 0);

  if (aad->need_seek && aad->nr_headers_remaining == 0) {
    anxanx_seek_update (media);
    goto do_read;
  }

  if (aad->media_packets == NULL) {
    media->eos = 1;
    return 0;
  }

  aam = (AnxAnxSource *)aad->media_packets->data;
  bytes_to_read = MIN (n, aam->length - aad->current_offset);

  memcpy (buf, &aam->data[aad->current_offset], bytes_to_read);

  aad->current_offset += bytes_to_read;

  media->current_track = aam->media_track;

  if (aam->granulepos != -1) {
    media->current_track->current_granule = aam->granulepos;
  }

  /* If that's finished this media packet, advance to the next one */
  if (aad->current_offset >= aam->length) {
    aad->media_packets =
      anx_list_remove (aad->media_packets, aad->media_packets);

    anx_free (aam->data);
    anx_free (aam);

    aad->current_offset = 0;
  }

  /*  if (anx_eos(aad->anx_reader)) media->eos = 1;*/

  return bytes_to_read;
}

static long
anxanx_sizeof_next_read (AnxSource * media, long bound)
{
  AnxAnxData * aad = (AnxAnxData *)media->custom_data;
  AnxAnxSource * aam;
  long bytes_to_read;

  if (aad->ignore_media) return -1;

  if ((aad->media_packets == NULL) && (aad->got_end)) {
    media->eos = 1;
    return 0;
  }

  while ((aad->media_packets == NULL) &&
	 (anx_read (aad->anx_reader, 1024)) > 0);

  if (aad->media_packets == NULL) {
    media->eos = 1;
    return 0;
  }

  aam = (AnxAnxSource *)aad->media_packets->data;
  bytes_to_read = aam->length - aad->current_offset;

  return bytes_to_read;
}

static int
anxanx_close (AnxSource * media)
{
  AnxAnxData * aad = (AnxAnxData *)media->custom_data;

  anx_close (aad->anx_reader);

  anx_free (media);

  return 0;
}

static struct _AnxImporter anx_anx_importer = {
  (AnxImporterOpenFunc)anxanx_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxanx_close,
  (AnxImporterReadFunc)anxanx_read,
  (AnxImporterSizeofNextReadFunc)anxanx_sizeof_next_read,
  "application/annodex"
};

static struct _AnxImporter anx_x_anx_importer = {
  (AnxImporterOpenFunc)anxanx_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxanx_close,
  (AnxImporterReadFunc)anxanx_read,
  (AnxImporterSizeofNextReadFunc)anxanx_sizeof_next_read,
  "application/x-annodex"
};

AnxImporter *
anx_importer_init (int i)
{
  if (i == 0)
    return &anx_anx_importer;
  else if (i == 1)
    return &anx_x_anx_importer;
  else
    return NULL;
}
