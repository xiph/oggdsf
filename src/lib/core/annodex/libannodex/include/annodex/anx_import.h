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

#ifndef __ANX_IMPORT_H__
#define __ANX_IMPORT_H__

#include <annodex/anx_types.h>
#include <annodex/anx_core.h>

/** \file
 * Specification of AnxImporter
 *
 * - When read requires starting a new frame or GOP, check the need_sync flag.
 *   If this is set, output a sync frame.
 *
 * \note Always check not to seek before start_packet and not to seek or read 
 *       past end_packet.
 *
 * - seek should return the actual position after seeking (as the backend may
 *   not be able to seek exactly to the requested position).
 *
 * Backends must implement the following function:
 *
 * - AnxImporter * anx_importer_init (int i);
 *
 * Importers are accessed by index i using values from 0 upwards. For out of
 * range indexes, this function must return NULL.
 *
 * No other symbols in a backend should be visible (ie. declare all other
 * globals and functions as static).
 *
 */

/**
 * An AnxImporter implements generic functions for retrieving data
 * from sources of a particular content type.
 */
typedef struct _AnxImporter AnxImporter;

/**
 * An AnxSourceTrack contains one track of data
 */
typedef struct _AnxSourceTrack AnxSourceTrack;

/**
 * An AnxSource contains an instance of an active source object, which may
 * in turn contain several tracks.
 */
typedef struct _AnxSource AnxSource;

/**
 * Signature of a function for opening a source object by filename.
 * \param path the path to the source file
 * \param id the id of this
 * \param ignore_raw a flag to indicate that the importer should ignore
 *        any raw source read requests, ie. just deliver clips
 * \param start_time the start time to initially seek to
 * \param end_time a time bound to end on
 * \param import_callbacks callbacks to call when further importing or
 *        anchor inserting is required.
 * \returns a new AnxSource * handle
 */
typedef AnxSource* (*AnxImporterOpenFunc) (const char * path, const char * id,
					   int ignore_raw,
					   double start_time, double end_time,
					   AnxImportCallbacks * import_callbacks);

/**
 * Signature of a function for opening a source object attached to an open
 * file descriptor.
 * \param fd an open file descriptor
 * \param id the id of this
 * \param ignore_raw a flag to indicate that the importer should ignore
 *        any raw source read requests, ie. just deliver clips
 * \param start_time the start time to initially seek to
 * \param end_time a time bound to end on
 * \param import_callbacks callbacks to call when further importing or
 *        anchor inserting is required.
 * \returns a new AnxSource * handle
 */
typedef AnxSource * (*AnxImporterOpenFDFunc) (int fd, const char * id,
					      int ignore_raw,
					      double start_time, double end_time,
					      AnxImportCallbacks * import_callbacks);

/**
 * Signature of a function for reading bytes from a source object.
 * \param source an AnxSource * handle
 * \param buf a buffer to read data into
 * \param n a maximum number of bytes to read
 * \param bound a maximum granulecount to read
 */
typedef long (*AnxImporterReadFunc) (AnxSource * source, unsigned char * buf,
				  long n, long bound);

/**
 * Signature of a function to return the preferred next read size.
 * \param source an AnxSource * handle
 * \param bound a maximum granulecount to read
 */
typedef long (*AnxImporterSizeofNextReadFunc) (AnxSource * source, long bound);

/**
 * Signature of a function to close a source object
 * \param source an AnxSource* handle
 */
typedef int (*AnxImporterCloseFunc) (AnxSource * source);

struct _AnxImporter {
  AnxImporterOpenFunc open;
  AnxImporterOpenFDFunc openfd;
  AnxImporterCloseFunc close;
  AnxImporterReadFunc read;
  AnxImporterSizeofNextReadFunc sizeof_next_read;
  char * content_type;
};

struct _AnxSourceTrack {
  char * id;
  char * content_type;
  long nr_header_packets;
  anx_int64_t granule_rate_n;
  anx_int64_t granule_rate_d;
  anx_int64_t start_granule;
  anx_int64_t end_granule;
  anx_int64_t current_granule;
  int eos;
};

struct _AnxSource {
  AnxImporter * importer;
  AnxList * tracks;
  AnxSourceTrack * current_track; /* points within tracks,
				     managed by importer */
  
  int eos;
  double start_time;
  double end_time;
  void * custom_data;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register an AnxImporter object with libannodex
 * \param importer the importer
 * \returns 0 on success, -1 on failure
 */
int anx_importer_register (AnxImporter * importer);

/**
 * Unregister an AnxImporter previously registered with libannodex
 * \param importer the importer
 * \returns 0 on success, -1 on failure
 */
int anx_importer_unregister (AnxImporter * importer);

#ifdef __cplusplus
}
#endif

#endif /* __ANX_IMPORT_H__ */
