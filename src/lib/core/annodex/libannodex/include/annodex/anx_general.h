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

#ifndef __ANX_GENERAL_H__
#define __ANX_GENERAL_H__

/** \file
 * General functions related to Annodex media
 */

#include <annodex/anx_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Time parsing */
double anx_parse_time (const char * str);

/**
 * Retrieve the error code of the most recent error on an annodex
 * \param annodex an ANNODEX* handle
 * \returns the error code of the most recent error
 */
int anx_last_error (ANNODEX * annodex);

/**
 * Retrieve a printable error string corresponding to the most recent
 * error on annodex
 * \param annodex an ANNODEX* handle
 * \returns an error string
 */
const char * anx_strerror (ANNODEX * annodex);

/**
 * Open a file containing Annodex media
 * \param filename path to the file
 * \param flags ANX_READ or ANX_WRITE
 * \returns an ANNODEX* handle
 */
ANNODEX * anx_open (char * filename, int flags);

/**
 * Attach to an existing file descriptor
 * \param fd an open file descriptor
 * \param flags ANX_READ or ANX_WRITE
 * \returns an ANNODEX* handle
 */
ANNODEX * anx_open_stdio (FILE * file, int flags);

/**
 * Create a managed Annodex handle
 * This is an alternative interface for non-file annodexes
 * \param flags ANX_READ or ANX_WRITE
 * \returns an ANNODEX* handle
 */
ANNODEX * anx_new (int flags);

/**
 * Flush any unwritten data associated with an annodex
 * \param annodex An ANNODEX* handle
 * \returns 0 on success, -1 on failure
 */
int anx_flush (ANNODEX * annodex);

/**
 * Close an annodex.
 * \param annodex an ANNODEX* handle
 * \returns NULL on success; returns the unchanged
 * annodex on failure (eg. due to system close() failing)
 */
ANNODEX * anx_close (ANNODEX * annodex);

/**
 * Forcefully close an annodex without flushing any file descriptors
 * \param annodex an ANNODEX* handle
 * \returns 0 on success, -1 on failure
 */
int anx_destroy (ANNODEX * annodex);

/**
 * Query if an annodex is ready.
 * \param annodex an ANNODEX* handle
 * \returns 0 if annodex is not ready, 1 if it is
 */
int anx_ready (ANNODEX * annodex);

/**
 * Query if an annodex has reached its 'End Of Stream' markers
 * \param annodex an ANNODEX* handle
 * \returns 0 if annodex is not at eos, 1 if it is
 */
int anx_eos (ANNODEX * annodex);

/**
 * Set the head element of the Annodex
 * \param annodex an ANNODEX* handle
 * \param head a complete AnxHead structure to set
 * \returns the head if successful, NULL on failure
 * \note libannodex makes a copy of the head; it can be safely
 * freed after returning from this call.
 */
AnxHead * anx_set_head (ANNODEX * annodex, AnxHead * head);

/**
 * Get a copy of the head element of an annodex
 * \param annodex an ANNODEX* handle
 * \return a copy of the head element
 */
AnxHead * anx_get_head (ANNODEX * annodex);

#if 1
/**
 * Query the current byte offset of an annodex
 * \param annodex an ANNODEX* handle
 * \returns the current byte offset
 * XXX: Deprecated?
 */
anx_int64_t anx_tell (ANNODEX * annodex);
#endif

#if 0 /* Deprecated */
long anx_seek (ANNODEX * annodex, long packets, int whence);
#endif

/**
 * Seek to an anchor identified by id
 * \param annodex an ANNODEX* handle
 * \param id The id of the anchor to seek to
 */
int anx_seek_id (ANNODEX * annodex, const char * id);

/**
 * Query the timebase of an annodex
 * \param annodex an ANNODEX* handle
 * \returns the timebase of the annodex
 */
double anx_get_timebase (ANNODEX * annodex);

/**
 * \param annodex an ANNODEX* handle
 * \param seconds the new timebase
 */
double anx_set_timebase (ANNODEX * annodex, double seconds);

/**
 * Query the current offset of an annodex expressed as time in seconds
 * \param annodex an ANNODEX* handle
 * \returns the current time offset
 */
double anx_tell_time (ANNODEX * annodex);

/**
 * Seek to a time point in an annodex
 * \param annodex an ANNODEX* handle
 * \param seconds the time to seek to
 * \param whence whence parameter (defined in anx_constants.h)
 */
double anx_seek_time (ANNODEX * annodex, double seconds, int whence);


/**
 * Print an AnxHead structure to a memory buffer. The head is serialized
 * to XML conformant with the anxhead DTD.
 * \param buf a buffer to print into
 * \param n the maximum number of characters to print
 * \param h the head to print
 * \returns the number of characters printed, -1 on potential overrun
 * \bug This should follow C99 semantics
 */
int anx_head_snprint (char *buf, int n, AnxHead * h);

/**
 * Print an AnxAnchor structure to a memory buffer. The head is serialized
 * to XML conformant with the anxa DTD.
 * \param buf a buffer to print into
 * \param n the maximum number of characters to print
 * \param a the anchor to print
 * \param start the value of the 'start' attribute, as a time in seconds.
 *        If start is negative, the 'start' attribute is not printed.
 * \returns the number of characters printed, -1 on potential overrun
 * \bug This should follow C99 semantics
 */
int anx_clip_snprint (char * buf, int n, AnxClip * a,
		      double start, double end);

/**
 * Free an AnxHead structure
 * \param head the structure to free
 * \returns NULL on success
 */
AnxHead * anx_head_free (AnxHead * head);

/**
 * Free an AnxClip structure
 * \param clip the structure to free
 * \returns NULL on success
 */
AnxClip * anx_clip_free (AnxClip * clip);
	
/**
 * Clone an AnxHead structure
 * \param head the head to clone
 * \returns a new head structure
 * \note All components of the original head are cloned, including all
 * strings and its list of meta tags.
 */
AnxHead * anx_head_clone (AnxHead * head);

/**
 * Clone an AnxClip structure
 * \param clip the clip to clone
 * \returns a new clip structure
 * \note All components of the original clip are cloned, including all
 * strings and its list of meta tags.
 */
AnxClip * anx_clip_clone (AnxClip * clip);

/**
 * Clone an AnxMetaElement structure
 * \param meta the meta tag structure to clone
 * \returns a new meta tag structure
 */
AnxMetaElement * anx_meta_element_clone (AnxMetaElement * meta);

#ifdef __cplusplus
}
#endif

#endif /* __ANX_GENERAL_H__ */
