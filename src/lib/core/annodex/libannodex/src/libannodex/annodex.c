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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <anx_private.h>

/*#define DEBUG*/

static int
anx_flags_disabled (int flags)
{
 if (flags & ANX_WRITE) {
   if (!ANX_CONFIG_WRITE) return ANX_ERR_DISABLED;
 } else {
   if (!ANX_CONFIG_READ) return ANX_ERR_DISABLED;
 }

 return 0;
}

static ANNODEX *
anx_new_with_file_oggz (int flags, FILE * file, OGGZ * oggz, int slurpable)
{
  ANNODEX * annodex;

  annodex = (ANNODEX *) anx_malloc (sizeof (ANNODEX));

  annodex->flags = flags;
  annodex->err = 0;
  annodex->file = file;
  annodex->timing_modifiable = 1;
  annodex->slurpable = slurpable;

  annodex->oggz = oggz;

  annodex->timebase = 0;
  annodex->current_granule = 0;
  annodex->current_serialno = -1;

  annodex->head = NULL;

  annodex->commands = NULL;
  annodex->current_command = NULL;
  annodex->pending_command = NULL;
    
  annodex->mapping_serialno_valid = 0;
  annodex->anno_serialno_valid = 0;

  switch (flags) {
  case ANX_READ:
    if (ANX_CONFIG_READ) anx_reader_init (annodex);
    break;
  case ANX_WRITE:
    if (ANX_CONFIG_WRITE) anx_writer_init (annodex);
    break;
  default:
    break;
  }

  return annodex;
}

ANNODEX *
anx_new (int flags)
{
  OGGZ * oggz = NULL;
  int oggz_flags;

  if (anx_flags_disabled (flags)) return NULL;

  if (flags & ANX_WRITE) {
    oggz_flags = OGGZ_WRITE|OGGZ_NONSTRICT;
  } else {
    oggz_flags = OGGZ_READ;
  }

  oggz = oggz_new (oggz_flags);

  return anx_new_with_file_oggz (flags, NULL, oggz, 0);
}

ANNODEX *
anx_open (char * filename, int flags)
{
  OGGZ * oggz = NULL;
  int oggz_flags;

  if (anx_flags_disabled (flags)) return NULL;

  if (flags & ANX_WRITE) {
    oggz_flags = OGGZ_WRITE|OGGZ_NONSTRICT;
  } else {
    oggz_flags = OGGZ_READ;
  }

  oggz = oggz_open (filename, oggz_flags);

  return anx_new_with_file_oggz (flags, NULL, oggz, 1);
}

ANNODEX *
anx_open_stdio (FILE * file, int flags)
{
  OGGZ * oggz = NULL;
  int slurpable = 0;
  int oggz_flags;
  int fd;

  if (anx_flags_disabled (flags)) return NULL;

  if (flags & ANX_WRITE) {
    oggz_flags = OGGZ_WRITE|OGGZ_NONSTRICT;
  } else {
    oggz_flags = OGGZ_READ;

    fd = fileno (file);
    if (lseek (fd, 0, SEEK_CUR) != -1) {
      slurpable = 1;
    }
  }

  oggz = oggz_open_stdio (file, oggz_flags);

  return anx_new_with_file_oggz (flags, file, oggz, slurpable);
}

int
anx_flush (ANNODEX * annodex)
{
  if (annodex == NULL) return -1;

  if (annodex->oggz == NULL) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1;
  }

  return oggz_flush (annodex->oggz);
}

ANNODEX *
anx_close (ANNODEX * annodex)
{
  if (annodex == NULL) return NULL;

  if (oggz_close (annodex->oggz) != 0) {
    anx_set_error (annodex, ANX_ERR_SYSTEM);
      return annodex;
  }

  anx_free (annodex);

  return NULL;
}

AnxHead *
anx_set_head (ANNODEX * annodex, AnxHead * head)
{
  if (annodex == NULL) return NULL;

  if (annodex->flags == ANX_READ) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return NULL;
  }

  anx_head_free (annodex->head);

  annodex->head = anx_head_clone (head);

  return head;
}

AnxHead *
anx_get_head (ANNODEX * annodex)
{
  if (annodex == NULL) return NULL;

  if (annodex->flags == ANX_READ && annodex->head == NULL) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return NULL;
  }

  return anx_head_clone (annodex->head);
}

anx_int64_t
anx_tell (ANNODEX * annodex)
{
  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1L;
  }

  return annodex->current_granule;
}

double
anx_tell_time (ANNODEX * annodex)
{
  anx_int64_t offset;

  if (annodex == NULL) return -1.0;

  if (!_anx_ready (annodex)) {
#ifdef DEBUG
    printf ("anx_tell_time: NOT READY\n");
#endif
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1.0;
  }

  offset = anx_tell (annodex);
#ifdef DEBUG
  printf ("anx_tell_time: offset %lld\n", offset);
#endif

  if (offset == -1) return -1.0;

  return anx_track_granules_to_time (annodex, annodex->current_serialno,
				     offset);
}

#ifdef DEPRECATED
long
anx_seek (ANNODEX * annodex, long packet_offset, int whence)
{
  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ENOTREADY);
    return -1L;
  }

  if (annodex->flags == ANX_READ) {
    return anx_reader_seek (annodex, packet_offset, whence);
  }

  if (annodex->flags == ANX_WRITE) {
    return anx_writer_seek (annodex, packet_offset, whence);
  }

  anx_set_error (annodex, ANX_EINVALID);
  return -1;
}
#endif

double
anx_seek_time (ANNODEX * annodex, double seconds, int whence)
{
  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1.0;
  }

  if (annodex->flags == ANX_READ) {
    if (ANX_CONFIG_READ)
      return anx_reader_seek_time (annodex, seconds, whence);
  }

  if (annodex->flags == ANX_WRITE) {
    if (ANX_CONFIG_WRITE)
      return anx_writer_seek_time (annodex, seconds, whence);
  }

  anx_set_error (annodex, ANX_ERR_INVALID);
  return -1;
}

int
anx_seek_id (ANNODEX * annodex, const char * id)
{
  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1L;
  }

  if (annodex->flags == ANX_READ) {
    if (ANX_CONFIG_READ) return anx_reader_seek_id (annodex, id);
  }

  if (annodex->flags == ANX_WRITE) {
    if (ANX_CONFIG_WRITE) return anx_writer_seek_id (annodex, id);
  }

  anx_set_error (annodex, ANX_ERR_INVALID);
  return -1;
}

int
anx_eos (ANNODEX * annodex)
{
  if (annodex->flags == ANX_READ) {
    return annodex->x.reader.eos;
  } else {
    return annodex->x.writer.eos;
  }
}

double
anx_get_timebase (ANNODEX * annodex)
{
  if (annodex == NULL) return -1;

  if (!_anx_ready (annodex)) {
#ifdef DEBUG
    printf ("anx_get_timebase: NOT READY, but %f\n", annodex->timebase);
#endif
    anx_set_error (annodex, ANX_ERR_NOTREADY);
    return -1;
  }

  return annodex->timebase;
}

double
anx_set_timebase (ANNODEX * annodex, double seconds)
{
  if (annodex == NULL) return -1;

  if (annodex->flags == ANX_READ) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1.0;
  }

  /* XXX: Should refuse if started writing already, ie. too late!
   * checking anx_ready is not quite right */
  if (_anx_ready (annodex)) {
    anx_set_error (annodex, ANX_ERR_INVALID);
    return -1.0;
  }

#ifdef DEBUG
  printf ("anx_set_timebase: %f\n", seconds);
#endif

  annodex->timebase = seconds;

  return annodex->timebase;
}

int
anx_ready (ANNODEX * annodex)
{
  return _anx_ready (annodex);
}

void
anx_set_error (ANNODEX * annodex, int err)
{
  if (annodex) annodex->err = err;
}

int
anx_last_error (ANNODEX * annodex)
{
  if (annodex) return annodex->err;
  else return 0;
}

const char *
anx_strerror (ANNODEX * annodex)
{
  if (annodex == NULL) return "No annodex";

  switch (annodex->err) {
  case ANX_ERR_OK: return "OK";
  case ANX_ERR_SYSTEM: return "System error";
  case ANX_ERR_INVALID: return "Invalid operation";
  case ANX_ERR_NOTSUPPORTED: return "Operation not supported";
  case ANX_ERR_NOTREADY: return "Not ready";
  case ANX_ERR_NOTIMPLEMENTED: return "Not implemented";
  case ANX_ERR_NOIMPORTER: return "No importer";
  default: return "Unknown error";
  }
}
