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

#if ANX_CONFIG_WRITE

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <dirent.h>

#define _ANX_PRIVATE
#include <annodex/anx_import.h>

#define _ANX_MEDIA
#include "anx_private.h"
#include "content_type.h"

/* #define DEBUG */

#include "fix_dl.h"

#ifdef WIN32
#include <windef.h>
#define S_ISREG(x) TRUE
#endif
 


static AnxList * media_importers = NULL;

int
anx_importer_register (AnxImporter * importer)
{
  media_importers = anx_list_append (media_importers, importer);
#ifdef DEBUG
  printf ("registered importer for content-type %s\n", importer->content_type);
#endif
  return 0;
}

int
anx_importer_unregister (AnxImporter * importer)
{
  AnxList * l;

  l = anx_list_find (media_importers, importer);
  media_importers = anx_list_remove (media_importers, l);

  return 0;
}

typedef AnxImporter * (*AnxImporterInitFunc) (int i);

static void
anx_register_media_importer_lib (const char * pathname,
				 char * content_type_pattern)
{
  void * dlhandle;
  AnxImporterInitFunc importer_func;
  AnxImporter * importer;
  int i;

#ifdef WIN32 /* only try with .dll files */
    char *extension;
	int len_ext;
	len_ext = (strlen(pathname)>4?strlen(pathname)-4:0);
	extension = (char*)pathname + len_ext;
	if (len_ext<4 || strcmp(extension, ".dll")!=0) return;
#endif

#ifdef DEBUG
  printf ("trying importer lib %s for %s\n", pathname, content_type_pattern);
#endif

  dlhandle = dlopen_F (pathname, RTLD_LAZY);
  if (dlhandle == NULL) {
#ifdef DEBUG
    printf ("\tno dlhandle: %s\n", dlerror_F ());
#endif
  } else {
    importer_func = (AnxImporterInitFunc)
      dlsym_F (dlhandle, "anx_importer_init");

    if (importer_func != NULL) {
      for (i = 0; (importer = importer_func(i)) != NULL; i++) {
	if (anx_content_type_matches (importer->content_type,
				      content_type_pattern))
	  anx_importer_register (importer);
      }
    }
  }

}

static void
anx_register_all_media_importers_dir (char * dirname, char * content_type_pattern)
{
  DIR * dir;
  struct dirent * dirent;
  struct stat statbuf;
#define BUF_LEN 256
  char buf[BUF_LEN];

  dir = opendir (dirname);
  if (!dir) {
    /* fail silently */
    return;
  }

  while ((dirent = readdir (dir)) != NULL) {
    snprintf (buf, BUF_LEN, "%s/%s", dirname, dirent->d_name);
    if (lstat (buf, &statbuf) == -1) {
      perror ("stat");
    } else {
      if (S_ISREG(statbuf.st_mode))
	anx_register_media_importer_lib (buf, content_type_pattern);
    }
  }
}

void
anx_init_importers (char * content_type_pattern)
{
  anx_register_all_media_importers_dir (IMPORTERS_DIR, content_type_pattern);
}

static AnxImporter *
anx_media_find_importer (char * content_type)
{
  AnxList * l;
  AnxImporter * importer = NULL;

  for (l = media_importers; l; l = l->next) {
    importer = (AnxImporter *)l->data;
    if (!strcasecmp (importer->content_type, content_type))
      return importer;
  }

  return NULL;
}

AnxSource *
anx_media_open (ANNODEX * anx, char * filename, char * id,
		char * ocontent_type,
		double start_time, double end_time, int flags,
		AnxImportStream import_stream,
		AnxImportHead import_head,
		AnxImportClip import_clip,
		void * import_user_data,
		AnxImportImport import_import,
		void * import_import_user_data)
{
  AnxSource * m = NULL;
  AnxImporter * importer = NULL;
  int ignore_media;
  int fd;
  AnxImportCallbacks cb;
  char * content_type;

  if (ocontent_type == NULL) {
    content_type = anx_content_type_guess_by_filename (filename);
  } else {
    content_type = ocontent_type;
  }

  if (content_type == NULL) {
    anx_set_error (anx, ANX_ERR_NOIMPORTER);
    goto out;
  }

  importer = anx_media_find_importer (content_type);

  if (importer == NULL) {
    anx_set_error (anx, ANX_ERR_NOIMPORTER);
    goto out;
  }

  memset (&cb, 0, sizeof (AnxImportCallbacks));

  /* Handle IGNORE flags */

  /* XXX: Should there be an IGNORE_STREAM flag ? */
  cb.import_stream = import_stream;

  if ((flags & ANX_IGNORE_ANNO) == 0) {
    cb.import_head = import_head;
    cb.import_clip = import_clip;
  }

  cb.import_user_data = import_user_data;

  ignore_media = (flags & ANX_IGNORE_MEDIA) ? 1 : 0;

  if ((flags & ANX_IGNORE_IMPORT_IMPORT) == 0) {
    cb.import_import = import_import;
    cb.import_import_user_data = import_import_user_data;
  }

  /* Open media file using open() callback, or else with openfd() */

#ifdef DEBUG
  printf ("anx_media_open: attempting to open %s with content-type %s; %s\n",
	  filename, content_type, ignore_media ? "Ignoring media" : "With media");
#endif

  if (importer->open) {
    m = importer->open (filename, id, ignore_media, start_time, end_time, &cb);
  } else if (importer->openfd) {
    fd = open (filename, O_RDONLY);
    if (fd == -1) {
      anx_set_error (anx, ANX_ERR_SYSTEM);
      goto out;
    } else {
      m = importer->openfd (fd, id, ignore_media, start_time, end_time, &cb);
    }
  }

  if (m == NULL) {
    anx_set_error (anx, ANX_ERR_NOTSUPPORTED);
  } else {
    m->importer = importer;
#if 0
    if (m->content_type == NULL)
      m->content_type = importer->content_type;
#endif
  }

 out:
  if (content_type != ocontent_type) anx_free (content_type);

  return m;
}

AnxSource *
anx_media_openfd (ANNODEX * anx, int fd, char * id, char * content_type,
		  double start_time, double end_time, int flags,
		  AnxImportHead import_head,
		  AnxImportClip import_clip,
		  void * import_user_data,
		  AnxImportImport import_import,
		  void * import_import_user_data)
{
  AnxImporter * importer = NULL;
  AnxSource * m = NULL;
  int ignore_media;
  AnxImportCallbacks cb;

  if (content_type == NULL) {
    anx_set_error (anx, ANX_ERR_NOIMPORTER);
    return NULL;
  }

  if (media_importers) {
    importer = anx_media_find_importer (content_type);
  }

  if (importer == NULL) {
    anx_set_error (anx, ANX_ERR_NOIMPORTER);
    return NULL;
  }

  if (importer->openfd == NULL) return NULL;

  /* Handle IGNORE flags */

  if ((flags & ANX_IGNORE_ANNO) == 0) {
    cb.import_head = import_head;
    cb.import_clip = import_clip;
    cb.import_user_data = import_user_data;
  }

  ignore_media = (flags & ANX_IGNORE_MEDIA) ? 1 : 0;

  if ((flags & ANX_IGNORE_IMPORT_IMPORT) == 0) {
    cb.import_import = import_import;
    cb.import_import_user_data = import_import_user_data;
  }

  /* Open media using openfd() callback */

  if (importer->openfd) {
    m = importer->openfd (fd, id, ignore_media, start_time, end_time, &cb);
  }

  if (m == NULL) {
    anx_set_error (anx, ANX_ERR_NOTSUPPORTED);
  } else {
    m->importer = importer;
#if 0
    if (m->content_type == NULL)
      m->content_type = importer->content_type;
#endif
  }

  return m;
}

long
anx_media_read (AnxSource * media, unsigned char * buf, long n, long bound)
{
  AnxImporter * importer;

  importer = media->importer;
  return importer->read (media, buf, n, bound);
}

long
anx_media_sizeof_next_read (AnxSource * media, long bound)
{
  AnxImporter * importer;

  importer = media->importer;
  return importer->sizeof_next_read (media, bound);
}

int
anx_media_close (AnxSource * media)
{
  AnxImporter * importer;

  importer = media->importer;

  return importer->close (media);
}

int
anx_media_query_track(AnxSource * media, int track_index,
		      char ** id, char ** content_type,
		      long * nr_header_packets,
		      anx_int64_t * granule_rate_n,
		      anx_int64_t * granule_rate_d)
{
  AnxList * l;
  AnxSourceTrack * track;
  int i = 0;

  for (l = media->tracks; l; l = l->next) {
    if (track_index == i) {
      track = (AnxSourceTrack *)l->data;

      *id = track->id;
      *content_type = track->content_type;
      *nr_header_packets = track->nr_header_packets;
      *granule_rate_n = track->granule_rate_n;
      *granule_rate_d = track->granule_rate_d;
      return 0;
    }
    i++;
  }

  return -1;
}

static int
anx_media_ensure_current_track (AnxSource * media)
{
  if (media == NULL || media->tracks == NULL) return -1;

  if (media->current_track == NULL) {
    media->current_track = (AnxSourceTrack *)media->tracks->data;
  }

  return 0;
}

int
anx_media_current_track (AnxSource * media)
{
  AnxList * l;
  AnxSourceTrack * track;
  int i = 0;

  if (anx_media_ensure_current_track (media) == -1) return -1;

  for (l = media->tracks; l; l = l->next) {
    track = (AnxSourceTrack *)l->data;
    if (track == media->current_track) {
#ifdef DEBUG
    printf ("anx_media_current_track: [%d] (%s)\n",
	    i, track->content_type);
#endif
      return i;
    }
    i++;
  }

  return 0;
}

anx_int64_t
anx_media_tell (AnxSource * media)
{
  if (media == NULL) return -1L;

  if (anx_media_ensure_current_track (media) == -1) return -1L;

  return media->current_track->current_granule;
}

double
anx_media_tell_time (AnxSource * media)
{
  if (media == NULL) return -1.0;

  if (anx_media_ensure_current_track (media) == -1) return -1.0;

  return ((double)media->current_track->current_granule *
	  (double)media->current_track->granule_rate_d /
	  (double)media->current_track->granule_rate_n);
}

int
anx_media_eos (AnxSource * media)
{
  if (media == NULL) return 0;
  return media->eos;
}

#else /* ANX_CONFIG_WRITE */

#include <annodex/anx_import.h>
#include <annodex/anx_constants.h>

int
anx_register_media_importer (AnxImporter * importer)
{
  return ANX_ERR_DISABLED;
}

int
anx_unregister_media_importer (AnxImporter * importer)
{
  return ANX_ERR_DISABLED;
}

void
anx_init_importers (char * content_type_pattern)
{
  return;
}

#endif
