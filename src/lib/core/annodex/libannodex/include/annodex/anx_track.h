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

#ifndef __ANX_TRACK_H__
#define __ANX_TRACK_H__

/** \file
 * Functions related to tracks in Annodex media
 */

#include <annodex/anx_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Query the contents of tracks in an annodex
 * \param annodex an ANNODEX* handle
 * \returns a list of AnxTrack *
 */
AnxList *
anx_get_track_list (ANNODEX * annodex);

/**
 * Query the content type of a track in an annodex
 * \param annodex an ANNODEX* handle
 * \param serialno the serialno of the track to query
 */
char * anx_track_get_content_type (ANNODEX * annodex, long serialno);

/**
 * Query the number of header packets of a track in an annodex
 * \param annodex an ANNODEX* handle
 * \param serialno the serialno of the track to query
 */
long anx_track_get_nr_headers (ANNODEX * annodex, long serialno);

/**
 * Query the granule rate of a track in an annodex
 * \param annodex an ANNODEX* handle
 * \param serialno the serialno of the track to query
 * \param granule_rate_n return granule_rate numerator
 * \param granule_rate_d return granule_rate denominator
 * \returns 0 on success, -1 on failure
 */
int anx_track_get_granule_rate (ANNODEX * annodex, long serialno,
				anx_int64_t * granule_rate_n,
				anx_int64_t * granule_rate_d);

/**
 * Convert a time in seconds to a granule position for a track in an annodex
 * \param annodex an ANNODEX* handle
 * \param serialno the serialno of the track to query
 * \param seconds the time to convert
 * \returns the granulepos corresponding to seconds
 */
anx_int64_t anx_track_time_to_granules (ANNODEX * annodex, long serialno,
					double seconds);

/**
 * Convert a granule position to time in seconds for a track in an annodex
 * \param annodex an ANNODEX* handle
 * \param serialno the serialno of the track to query
 * \param granules the granule position to convert
 */
double anx_track_granules_to_time (ANNODEX * annodex, long serialno,
				   anx_int64_t granules);

#ifdef __cplusplus
}
#endif

#endif /* __ANX_TRACK_H__ */
