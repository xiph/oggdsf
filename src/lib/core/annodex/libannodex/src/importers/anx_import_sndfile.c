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

#include <time.h>
#include <string.h>

#include <stdlib.h>
#define anx_malloc malloc
#define anx_free free 

/*#define DEBUG*/

#define MIN(a,b) ((a)<(b)?(a):(b))

#include <annodex/anx_import.h>

#include <sndfile.h>

#define WAV_HEADER_LEN 44

typedef struct _AnxSndfileData AnxSndfileData;

struct _AnxSndfileData {
  SNDFILE * sndfile;
  SF_INFO * sf_info;
  long total_remaining;
  long framesperpacket; /* sf frames per anx packet */
  long frame_remaining; /* bytes remaining in current frame */
  unsigned char header[WAV_HEADER_LEN];
  int header_remaining;
};

static AnxImporter anxsf_importer;

static unsigned int
le_int(unsigned int i)
{
   unsigned int ret=i;
#ifdef WORDS_BIGENDIAN
   ret =  i>>24;
   ret += (i>>8)&0x0000ff00;
   ret += (i<<8)&0x00ff0000;
   ret += (i<<24);
#endif
   return ret;
}

static unsigned short
le_short(unsigned short s)
{
   unsigned short ret=s;
#ifdef WORDS_BIGENDIAN
   ret =  s>>8;
   ret += s<<8;
#endif
   return ret;
}


static void
anxsf_generate_wav_header (AnxSndfileData * sfd, int rate, int channels,
                           int frames)
{
  unsigned char * h = sfd->header;
  int itmp;
  short stmp;

#define MWRITE(data, n) \
  memcpy (h, data, n); h += n;

  MWRITE ("RIFF", 4);

  /* yay magic numbers */
  itmp = le_int(2 * channels * frames);
  MWRITE(&itmp, 4);

  MWRITE ("WAVEfmt ", 8);

  itmp = le_int(16);
  MWRITE(&itmp, 4);

  stmp = le_short(1);
  MWRITE(&stmp, 2);

  stmp = le_short(channels);
  MWRITE(&stmp, 2);

  itmp = le_int(rate);
  MWRITE(&itmp, 4);

  itmp = le_int(rate*channels*2);
  MWRITE(&itmp, 4);

  stmp = le_short(2*channels);
  MWRITE(&stmp, 2);

  stmp = le_short(16);
  MWRITE(&stmp, 2);

  MWRITE ("data", 4);

  itmp = le_int(frames * channels * 2);
  MWRITE(&itmp, 4);
}

static int primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23 };

static long
calc_framesperpacket (long n)
{
  int i, nprimes;

  nprimes = sizeof(primes) / sizeof(primes[0]);

  for (i = 0; n > 1000 && i < nprimes; i++) {
    while ( n > 1000 && (n % primes[i]) == 0) {
      n /= primes[i];
    }
  }

  return n;
}


static AnxSource *
anxsf_open (const char * path, const char * id, int ignore_media,
	    double start_time, double end_time,
	    AnxImportCallbacks * unused)
{
  AnxSource * m;
  AnxSourceTrack * track;
  AnxSndfileData * sfd;
  SF_INFO * sf_info;
  long fpp;

  if (ignore_media) return NULL;

  m = (AnxSource *) anx_malloc (sizeof (AnxSource));
  track = (AnxSourceTrack *) anx_malloc (sizeof (AnxSourceTrack));

  m->importer = &anxsf_importer;
  m->current_track = track;
  m->tracks = anx_list_append (NULL, track);

  m->eos = 0;
  m->start_time = start_time;
  m->end_time = end_time;

  sfd = anx_malloc (sizeof (AnxSndfileData));
  m->custom_data = sfd;

  sf_info = anx_malloc (sizeof (SF_INFO));
  sfd->sf_info = sf_info;

  sfd->sndfile = sf_open (path, SFM_READ, sf_info);

  fpp = calc_framesperpacket (sf_info->samplerate);

  sfd->framesperpacket = fpp;
  sfd->frame_remaining = fpp;
  sfd->header_remaining = WAV_HEADER_LEN;

  anxsf_generate_wav_header (sfd, sf_info->samplerate, sf_info->channels,
			     sf_info->frames);

  track->id = (char *)id; /* XXX: dup? */
  track->content_type = "audio/x-wav";

  track->granule_rate_n = sf_info->samplerate;
  track->granule_rate_d = 1;

  track->current_granule = 0;
  track->eos = 0;

  if (end_time != -1.0) {
    track->end_granule = end_time * sf_info->samplerate;
  } else {
    track->end_granule = sf_info->frames;
  }

  if (start_time != 0.0) {
    track->start_granule = start_time * sf_info->samplerate;

    track->current_granule =
      sf_seek (sfd->sndfile, track->start_granule, SEEK_SET);
  
    if (track->current_granule >= track->end_granule) track->eos = 1;
    else track->eos = 0;
  } else {
    track->start_granule = 0;
  }

#ifdef DEBUG
  fprintf (stderr, "anxsf_open: %lld - %lld\n", track->start_granule,
	   track->end_granule);
#endif

  sfd->total_remaining = track->end_granule - track->start_granule;

  track->nr_header_packets = 1;

  return m;
}

#if 0
static long
anxsf_seek_update (AnxSource * media)
{
  AnxSndfileData *  sfd = (AnxSndfileData *)media->custom_data;
  long offset;

  offset = sf_seek (sfd->sndfile, sfd->seek_offset, SEEK_SET);
  
  if (offset >= media->end_granule) media->eos = 1;
  else media->eos = 0;

  return offset;
}

static long
anxsf_seek (AnxSource * media, long offset)
{
  AnxSndfileData *  sfd = (AnxSndfileData *)media->custom_data;

  sfd->seek_offset = offset;

#if 0
    /*offset = sf_seek (sfd->sndfile, offset * sfd->framesperpacket, SEEK_SET);*/
    offset = sf_seek (sfd->sndfile, offset, SEEK_SET);

    if (offset >= media->end_granule) media->eos = 1;
    else media->eos = 0;
#else

  if (sfd->header_remaining == 0) {
    anxsf_seek_update (media);
  }
#endif

  /*return offset/sfd->framesperpacket;*/
  return offset;
}
#endif

static long
anxsf_read (AnxSource * media, char * buf, long n, long bound)
{
  AnxSndfileData * sfd = (AnxSndfileData *)media->custom_data;
  AnxSourceTrack * track = media->current_track;
  long header_n = 0;
  long frames_n, frames_to_read, i;
  short * s;

  if (sfd->sf_info == NULL) {
#ifdef DEBUG
    fprintf (stderr, "anxsf_read: NULL sf_info\n");
#endif
    return -1;
  }

  if (sfd->header_remaining > 0) {
    header_n = MIN (n, sfd->header_remaining);
    memcpy (buf, &sfd->header[WAV_HEADER_LEN - sfd->header_remaining],
	    header_n);
    buf += header_n;
    sfd->header_remaining -= header_n;
    n = header_n;
  } else {

    frames_n = n / (sfd->sf_info->channels *  sizeof (short));

    frames_to_read = MIN (frames_n, sfd->total_remaining);
    frames_to_read = MIN (frames_to_read, sfd->frame_remaining);
    if (bound != -1) {
      frames_to_read = MIN (bound * sfd->framesperpacket, frames_to_read);
    }

    frames_to_read =
      sf_readf_short (sfd->sndfile, (short *)buf, frames_to_read);

    s = (short *)buf;
    for (i = 0; i < frames_to_read * sfd->sf_info->channels; i++) {
      *s = le_short (*s);
      s++;
    }

    if (frames_to_read == 0) {
      track->eos = 1;
      media->eos = 1;
    }

    sfd->total_remaining -= frames_to_read;
    sfd->frame_remaining -= frames_to_read;

    if (sfd->frame_remaining <= 0) {
      sfd->frame_remaining = sfd->framesperpacket;
      track->current_granule += sfd->framesperpacket;
      if (track->current_granule >= track->end_granule) track->eos = 1;
    }

    n = frames_to_read * (sfd->sf_info->channels * sizeof (short));
  }

  return n;
}

static long
anxsf_sizeof_next_read (AnxSource * media, long bound)
{
  AnxSndfileData * sfd = (AnxSndfileData *)media->custom_data;
  long frames_to_read;

  if (sfd->sf_info == NULL) {
    return -1;
  }

  if (sfd->header_remaining > 0) {
    return sfd->header_remaining;
  } else {
    frames_to_read =
      sfd->frame_remaining * (sfd->sf_info->channels * sizeof (short));

    if (bound == -1) {
      /*return sfd->header_remaining + frames_to_read;*/
      return frames_to_read;
    } else {
      return
	/*sfd->header_remaining + */
	MIN (bound * sfd->framesperpacket, frames_to_read);
    }
  }
}

static int
anxsf_close (AnxSource * media)
{
  AnxSndfileData *  sfd = (AnxSndfileData *)media->custom_data;

  sf_close (sfd->sndfile);

  anx_free (sfd->sf_info);

  anx_free (media);

  return 0;
}

static struct _AnxImporter anxsf_importer_wav = {
  (AnxImporterOpenFunc)anxsf_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxsf_close,
  (AnxImporterReadFunc)anxsf_read,
  (AnxImporterSizeofNextReadFunc)anxsf_sizeof_next_read,
  "audio/x-wav"
};

static struct _AnxImporter anxsf_importer_aiff = {
  (AnxImporterOpenFunc)anxsf_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxsf_close,
  (AnxImporterReadFunc)anxsf_read,
  (AnxImporterSizeofNextReadFunc)anxsf_sizeof_next_read,
  "audio/x-aiff"
};

static struct _AnxImporter anxsf_importer_au = {
  (AnxImporterOpenFunc)anxsf_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxsf_close,
  (AnxImporterReadFunc)anxsf_read,
  (AnxImporterSizeofNextReadFunc)anxsf_sizeof_next_read,
  "audio/x-au"
};

AnxImporter *
anx_importer_init (int i)
{
  switch (i) {
  case 0:
    return &anxsf_importer_wav;
    break;
  case 1:
    return &anxsf_importer_aiff;
    break;
  case 2:
    return &anxsf_importer_au;
    break;
  default:
    return NULL;
    break;
  }
}
