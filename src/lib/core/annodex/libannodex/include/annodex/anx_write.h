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

#ifndef __ANX_WRITE_H__
#define __ANX_WRITE_H__

/** \file
 * Writer specific functions
 */

/** \defgroup writing Writing Annodex media
 *
 */

/** \defgroup writing_files Writing to files and file descriptors
 * \ingroup writing
 *
 * If you wish to write Annodex media to a file or file descriptor (such
 * as a network socket), it can be directly written as follows:
 *
 * - open an annodex using anx_open() or anx_open_stdio()
 * - import any media using anx_writer_import()
 * - call anx_write() repeatedly until it returns 0 or -1
 * - close the annodex with anx_close()
 *
 * This procedure is illustrated in src/examples/write-clip-file.c:
 *
 * \include write-clip-file.c
 *
 */

#include <annodex/anx_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise system importers matching a given content type pattern.
 * \param content_type_pattern A content type or range of content types
 */
void anx_init_importers (char * content_type_pattern);

/**
 * Query the size of the next page to be written.
 * \returns the size in bytes of the next page to be written
 */
long anx_write_get_next_page_size (ANNODEX * annodex);

/**
 * Import a file into the current ANNODEX* writer.
 * \param annodex An ANNODEX* writer
 * \param filename the file to import
 * \param id the id of the file
 * \param content_type the content-type of the whole import file.
 * \param seek_offset a time in seconds to begin importing data from
 * \param seek_end a time in seconds to finish importing data
 * \param flags writer import flags (RECURSIVE etc.)
 */
int
anx_write_import (ANNODEX * annodex, char * filename,
		  char * id, char * content_type,
		  double seek_offset, double seek_end, int flags);

/**
 * Override the callbacks that importers should call when they
 * wish to handle annotation elements. The default head callback
 * overwrites the head and the default clip callback inserts an
 * clip into the scheduling of the ANNODEX* writer.
 */
int
anx_write_set_anno_callbacks (ANNODEX * annodex,
			      AnxImportHead import_head_callback,
			      AnxImportClip import_clip_callback,
			      void * user_data);

/**
 * Retrieve the user_data associated with attached annotation import callbacks
 * \param annodex an ANNODEX* handle
 * \returns the associated user_data
 */
void *
anx_write_get_anno_user_data (ANNODEX * annodex);

/**
 * Override the Import Import callback, used when an importer wishes to
 * initiate further importing.
 * \param annodex An ANNODEX* writer
 * \param import_import_callbcak The new Import Import callback
 * \param user_data User-defined data to pass to the callback
 */
int
anx_write_set_ii_callback (ANNODEX * annodex,
			   AnxImportImport import_import_callback,
			   void * user_data);

/** 
 * Retrieve the user_data associated with attached recursive import callbacks
 * \param annodex an ANNODEX* handle
 * \returns the associated user_data
 */
void *
anx_write_get_ii_user_data (ANNODEX * annodex);

/**
 * Insert a clip into an ANNODEX* writer
 * \param annodex An ANNODEX* writer
 * \param at_time Time in seconds to schedule the clip
 * \param clip a clip
 */
int
anx_insert_clip (ANNODEX * annodex, double at_time, AnxClip * clip);

double anx_writer_get_end (ANNODEX * annodex);
int anx_writer_set_end (ANNODEX * annodex, double end_time);

/**
 * Write to an annodex opened with anx_open() or anx_open_stdio().
 * \param annodex An ANNODEX* writer
 * \param n count of bytes to write
 * \returns the count of bytes actually written
 */
long anx_write (ANNODEX * annodex, long n);

/**
 * Output data from an annodex into a memory buffer. Usually the annodex
 * would have been created with anx_new().
 * \param annodex an ANNODEX* writer
 * \param buf a memory buffer of size at least n bytes in which to output
 * \param n a count of bytes to output
 * \returns the count of bytes actually output
 */
long anx_write_output (ANNODEX * annodex, unsigned char * buf, long n);

int anx_request_header (ANNODEX * annodex);
int anx_request_media_sync (ANNODEX * annodex);

#ifdef _UNIMPLEMENTED_  
  
/* Scheduled commands to control the Annodex writer */

typedef struct _AnxSchedule AnxSchedule;
typedef struct _AnxImport AnxImport;

typedef enum _AnxSchedType {
  ANX_SCHED_CLIP,
  ANX_SCHED_IMPORT
} AnxSchedType;
				       
struct _AnxImport {
  char * location;     /* URI to media document */
  char * content_type;
  int flags;
};

struct _AnxSchedule {
  double start_time;
  AnxSchedType type;
  union {
    AnxClip * clip;
    AnxImport * import;
  } data;
};

AnxSchedule *
anx_schedule_query (ANNODEX * annodex, double at_time, AnxSchedType type);

int
anx_schedule_remove (ANNODEX * annodex, double at_time, AnxSchedType type);

AnxSchedule *
anx_schedule_first (ANNODEX * annodex);

AnxSchedule *
anx_schedule_next (ANNODEX * annodex, AnxSchedule * schedule);

AnxSchedule *
anx_schedule_prev (ANNODEX * annodex, AnxSchedule * schedule);

#endif /* _UNIMPLEMENTED_ */

#ifdef __cplusplus
}
#endif

#endif /* __ANX_WRITE_H__ */
