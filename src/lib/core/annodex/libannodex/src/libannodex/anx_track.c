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

#include <stdlib.h>
#include <string.h>

#include "anx_private.h"
#include "anx_snprint.h"

/*#define DEBUG*/

static AnxTrack *
anx_track_clone (AnxTrack * track)
{
  AnxTrack * s;

  s = anx_malloc (sizeof (struct _AnxTrack));
  s->serialno = track->serialno;
  s->id = anx_strdup (track->id);
  s->content_type = anx_strdup (track->content_type);
  s->nr_header_packets = track->nr_header_packets;
  s->granule_rate_n = track->granule_rate_n;
  s->granule_rate_d = track->granule_rate_d;

  return s;
}

AnxList *
anx_get_track_list (ANNODEX * annodex)
{
  AnxReader * reader = &annodex->x.reader;
  AnxWriter * writer = &annodex->x.writer;
  AnxList * l;

  if (annodex->flags == ANX_READ) {
    l = reader->tracks;
  } else {
    l = writer->tracks;
  }

  return anx_list_clone_with (l, (AnxCloneFunc)anx_track_clone);
}

static AnxTrack *
anx_get_track (ANNODEX * annodex, long serialno)
{
  AnxReader * reader = &annodex->x.reader;
  AnxWriter * writer = &annodex->x.writer;
  AnxTrack * track;
  AnxList * l;

  if (annodex->flags == ANX_READ) {
    l = reader->tracks;
  } else {
    if (serialno == annodex->anno_serialno) {
      return &writer->anno_track;
    }

    l = writer->tracks;
  }

  for (; l; l = l->next) {
    track = (AnxTrack *)l->data;
    if (track->serialno == serialno)
      return track;
  }

  return NULL;
}

int
anx_track_get_granule_rate (ANNODEX * annodex, long serialno,
			    anx_int64_t * granule_rate_n,
			    anx_int64_t * granule_rate_d)
{
  AnxTrack * track;

  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1;
  }

  if (serialno == annodex->mapping_serialno) {
    *granule_rate_n = 0;
    *granule_rate_d = 1;
    return 0;
  }

  track = anx_get_track (annodex, serialno);

  if (track == NULL) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1;
  }

  *granule_rate_n = track->granule_rate_n;
  *granule_rate_d = track->granule_rate_d;

  return 0;
}

long
anx_track_get_nr_headers (ANNODEX * annodex, long serialno)
{
  AnxTrack * track;

  if (annodex == NULL) return -1;

  track = anx_get_track (annodex, serialno);

  if (track == NULL) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1L;
  }

  return track->nr_header_packets;
}

char *
anx_track_get_content_type (ANNODEX * annodex, long serialno)
{
  AnxTrack * track;

  if (annodex == NULL) return NULL;

  track = anx_get_track (annodex, serialno);

  if (track == NULL || track->content_type == NULL) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return NULL;
  }

  return strdup (track->content_type);
}

static __inline__ anx_int64_t
_anx_time_to_granules_0 (ANNODEX * annodex, long serialno, double seconds)
{
  AnxTrack * track;

  track = anx_get_track (annodex, serialno);

  if (track == NULL) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1L;
  }

  return (anx_int64_t) (seconds * track->granule_rate_n / track->granule_rate_d);
}

static __inline__ anx_int64_t
_anx_time_to_granules (ANNODEX * annodex, long serialno, double seconds)
{
  return _anx_time_to_granules_0 (annodex, serialno,
				  (seconds - annodex->timebase));
}

anx_int64_t
anx_track_time_to_granules (ANNODEX * annodex, long serialno, double seconds)
{
  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1;
  }

  return _anx_time_to_granules (annodex, serialno, seconds);
}

static __inline__ double
_anx_granules_to_time (ANNODEX * annodex, long serialno, anx_int64_t granules)
{
  AnxTrack * track;
  double seconds;

  /* XXX: is this all ok for interval != 1 ? */

  track = anx_get_track (annodex, serialno);

  if (track == NULL) {
#ifdef DEBUG
    printf ("_anx_granules_to_time: could not find track (%ld)\n",
	    serialno);
#endif
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1.0;
  }

  seconds =
    (double)(granules * track->granule_rate_d) /
    (double)track->granule_rate_n;

  /*  return seconds + annodex->timebase;*/
  return seconds;
}

double
anx_track_granules_to_time (ANNODEX * annodex, long serialno,
			    anx_int64_t granules)
{
  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1.0;
  }

  return _anx_granules_to_time (annodex, serialno, granules);
}

