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

#ifndef __ANX_READ_H__
#define __ANX_READ_H__

/** \file
 *
 * libannodex provides a convenient callback based framework for reading
 * Annodex media. After opening an annodex for reading, you can attach
 * various callbacks relevant to the parts of the file you are interested
 * in, including the stream header, track headers, head element, clips
 * and media data. Then, as bytes are read, libannodex will call your
 * callbacks as appropriate.
 *
 * For tutorial information and examples, refer to the following:
 *
 * - \link reading_files Reading from files and file descriptors \endlink
 * - \link reading_memory Reading from memory buffers \endlink
 * - \link reading_callbacks Advanced management of AnxRead* callbacks \endlink
 */

#include <annodex/anx_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Signature for a callback called when the 'Annodex' stream header is parsed
 * \param annodex the ANNODEX* handle in use
 * \param timebase the timebase of the annodex
 * \param utc the UTC timebase of the annodex
 * \param user_data A generic pointer you have provided earlier
 * \returns 
 * - ANX_CONTINUE on success, and to inform anx_read*() functions to
 *   continue on to the next packet
 * - ANX_STOP_OK on success, to inform anx_read*() functions to return
 *   without further processing
 * - ANX_STOP_ERR on error, to inform anx_read*() functions to return
 *   without further processing
 */
typedef int (*AnxReadStream) (ANNODEX * annodex, double timebase,
			      char * utc, void * user_data);

/**
 * Signature for a callback called each time an 'AnxData' track header is
 * parsed.
 * \param annodex the ANNODEX* handle in use
 * \param serialno the track serial number
 * \param id the id attribute of the track
 * \param content_type the content type of the track
 * \param granule_rate_n the numerator of the granule rate
 * \param granule_rate_d the denominator of the granule rate
 * \param nr_header_packets the number of header packets of the track
 * \param user_data A generic pointer you have provided earlier
 * \returns 
 * - ANX_CONTINUE on success, and to inform anx_read*() functions to
 *   continue on to the next packet
 * - ANX_STOP_OK on success, to inform anx_read*() functions to return
 *   without further processing
 * - ANX_STOP_ERR on error, to inform anx_read*() functions to return
 *   without further processing
 */
typedef int (*AnxReadTrack) (ANNODEX * annodex, long serialno,
			     char * id, char * content_type,
			     anx_int64_t granule_rate_n,
			     anx_int64_t granule_rate_d,
			     int nr_header_packets,
			     void * user_data);
/**
 * Signature for a callback called when the head element is parsed
 * \param annodex the ANNODEX* handle in use
 * \param head the head element
 * \param user_data A generic pointer you have provided earlier
 * \returns 
 * - ANX_CONTINUE on success, and to inform anx_read*() functions to
 *   continue on to the next packet
 * - ANX_STOP_OK on success, to inform anx_read*() functions to return
 *   without further processing
 * - ANX_STOP_ERR on error, to inform anx_read*() functions to return
 *   without further processing
 */
typedef int (*AnxReadHead) (ANNODEX * annodex, const AnxHead * head,
			    void * user_data);

/**
 * Signature for a callback called each time a clip element is parsed
 * \param annodex the ANNODEX* handle in use
 * \param clip the clip element
 * \param user_data A generic pointer you have provided earlier
 * \returns 
 * - ANX_CONTINUE on success, and to inform anx_read*() functions to
 *   continue on to the next packet
 * - ANX_STOP_OK on success, to inform anx_read*() functions to return
 *   without further processing
 * - ANX_STOP_ERR on error, to inform anx_read*() functions to return
 *   without further processing
 */
typedef int (*AnxReadClip) (ANNODEX * annodex, const AnxClip * clip,
			    void * user_data);

/**
 * Signature for a callback called each time a raw data packet is found
 * \param annodex the ANNODEX* handle in use
 * \param data a pointer to the data read from the packet
 * \param n the length in bytes of the data
 * \param serialno the track serial number of the packet
 * \param granulepos the granule position of the packet
 * \param user_data A generic pointer you have provided earlier
 * \returns 
 * - ANX_CONTINUE on success, and to inform anx_read*() functions to
 *   continue on to the next packet
 * - ANX_STOP_OK on success, to inform anx_read*() functions to return
 *   without further processing
 * - ANX_STOP_ERR on error, to inform anx_read*() functions to return
 *   without further processing
 */
typedef int (*AnxReadRaw) (ANNODEX * annodex, unsigned char * data, long n,
			   long serialno, anx_int64_t granulepos,
			   void * user_data);

/**
 * Set the function to call each time an 'Annodex' stream header is parsed
 * \param annodex an ANNODEX* handle
 * \param read_stream the callback to call
 * \param user_data Arbitrary data you wish to pass to your callback
 * \returns 0 on success, -1 on failure
 */
int
anx_set_read_stream_callback (ANNODEX * annodex, AnxReadStream read_stream,
			      void * user_data);

/**
 * Retrieve the user_data associated with an attached AnxReadStream callback
 * \param annodex an ANNODEX* handle
 * \returns the associated user_data
 */
void *
anx_get_read_stream_user_data (ANNODEX * annodex);

/**
 * Set the function to call each time an 'AnxData' track header is parsed
 * \param annodex an ANNODEX* handle
 * \param read_track the callback to call
 * \param user_data Arbitrary data you wish to pass to your callback
 * \returns 0 on success, -1 on failure
 */
int
anx_set_read_track_callback (ANNODEX * annodex, AnxReadTrack read_track,
			     void * user_data);

/**
 * Retrieve the user_data associated with an attached AnxReadTrack callback
 * \param annodex an ANNODEX* handle
 * \returns the associated user_data
 */
void *
anx_get_read_track_user_data (ANNODEX * annodex);

/**
 * Set the function to call when the head element is read
 * \param annodex an ANNODEX* handle
 * \param read_head the callback to call
 * \param user_data Arbitrary data you wish to pass to your callback
 * \returns 0 on success, -1 on failure
 */
int
anx_set_read_head_callback (ANNODEX * annodex, AnxReadHead read_head,
			    void * user_data);

/**
 * Retrieve the user_data associated with an attached AnxReadHead callback
 * \param annodex an ANNODEX* handle
 * \returns the associated user_data
 */
void *
anx_get_read_head_user_data (ANNODEX * annodex);

/**
 * Set the function to call each time a clip is parsed
 * \param annodex an ANNODEX* handle
 * \param read_clip the callback to call
 * \param user_data Arbitrary data you wish to pass to your callback
 * \returns 0 on success
 */
int
anx_set_read_clip_callback (ANNODEX * annodex, AnxReadClip read_clip,
			    void * user_data);

/**
 * Retrieve the user_data associated with an attached AnxReadClip callback
 * \param annodex an ANNODEX* handle
 * \returns the associated user_data
 */
void *
anx_get_read_clip_user_data (ANNODEX * annodex);

/**
 * Set the function to call each time a raw data packet is read
 * \param annodex an ANNODEX* handle
 * \param read_raw the callback to call
 * \param user_data Arbitrary data you wish to pass to your callback
 * \returns 0 on success, -1 on failure
 */
int
anx_set_read_raw_callback (ANNODEX * annodex, AnxReadRaw read_raw,
			   void * user_data);

/**
 * Retrieve the user_data associated with an attached AnxReadRaw callback
 * \param annodex an ANNODEX* handle
 * \returns the associated user_data
 */
void *
anx_get_read_raw_user_data (ANNODEX * annodex);

/**
 * Associate arbitrary data with read callbacks.
 * \param annodex an ANNODEX* handle
 * \param user_data a pointer to a user defined object to be passed to
 *        each of the read callbacks.
 * \returns 0 on success, -1 on failure
 */
int anx_set_user_data (ANNODEX * annodex, void * user_data);

/**
 * Read from an annodex opened with anx_open() or anx_open_stdio().
 * The annodex is automatically parsed and relevant read callbacks are
 * called, stopping  when the required number of bytes have been read, or
 * earlier if any of the read callbacks returned other than ANX_CONTINUE.
 * \param annodex an ANNODEX* handle
 * \param n count of bytes to read
 * \returns the count of bytes actually read
 */
long anx_read (ANNODEX * annodex, long n);

/**
 * Input data from a memory buffer into an annodex. Usually the annodex
 * would have been created with anx_new().
 * The annodex is automatically parsed and relevant read callbacks are
 * called, stopping  when the required number of bytes have been read, or
 * earlier if any of the read callbacks returned other than ANX_CONTINUE.
 * \param annodex an ANNODEX* handle
 * \param buf a memory buffer containing data to input
 * \param n count of bytes to input
 * \returns the count of bytes actually input
 */
long anx_read_input (ANNODEX * annodex, unsigned char * buf, long n);

#ifdef __cplusplus
}
#endif

#endif /* __ANX_READ_H__ */
