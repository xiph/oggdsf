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

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <annodex/anx_import.h>
#include <annodex/annodex.h>

#include <cmml.h>

#define anx_malloc malloc
#define anx_free free 

#define MIN(a,b) ((a)<(b)?(a):(b))

#define anx_strdup(s) ((s)==NULL?NULL:strdup(s))

#define BUFSIZE 1024

/*#define DEBUG*/

typedef struct _AnxCMML_Data AnxCMML_Data;

struct _AnxCMML_Data {
  AnxImportStream import_stream;
  AnxImportHead import_head;
  AnxImportClip import_clip;
  void * import_user_data;

  AnxImportImport import_import;
  void * import_import_user_data;
};

static AnxHead *
anx_head_from_cmml_head (const CMML_Head * head)
{
 AnxHead * h;

  h = anx_malloc (sizeof (AnxHead));

  /* id attribute of header */
  h->head_id = anx_strdup(head->head_id);

  /* language of header */
  h->lang = anx_strdup(head->lang);

  /* directionality of lang */
  h->dir = anx_strdup(head->dir);

  /* profile of header */
  h->profile = anx_strdup(head->profile);

  /* title element of header */
  h->title = anx_strdup(head->title);

  /* id attribute of title element */
  h->title_id = anx_strdup(head->title_id);

  /* language of title */
  h->title_lang = anx_strdup(head->title_lang);

  /* directionality of title_lang */
  h->title_dir = anx_strdup(head->title_dir);

  /* id attribute of base element */
  h->base_id = anx_strdup(head->base_id);

  /* href attribute of base element */
  h->base_href = anx_strdup(head->base_href);

#if 0
  /* id attribute of stream element */
  h->stream_id = anx_strdup(head->stream_id);
#endif

  /* list of meta elements */
  h->meta = anx_list_clone ((AnxList*)head->meta);

  return h;
}

static int
read_stream (CMML * cmml, const CMML_Stream * stream, void * user_data)
{
  AnxCMML_Data * ac = (AnxCMML_Data *)user_data;
  CMML_List * media_node;
  CMML_ImportElement * m;
  char * granulerate;
  char * id;
  char * content_type;
  char * src;
  double timebase = 0.0;
  char * utc = NULL;
  double start_time = 0.0;
  double seek_offset = 0.0;
  double seek_end = 0.0;

  if (ac->import_import) {

    timebase = stream->timebase ? stream->timebase->t.sec : 0.0;
    utc = stream->utc ? stream->utc->tstr : NULL;

    ac->import_stream (timebase, utc, ac->import_user_data);

    for (media_node = stream->import; 
	 media_node; 
	 media_node = media_node->next) {

      m = (CMML_ImportElement *) media_node->data;

      /* ignore granulerate for now, but might be used for raw importers */
      granulerate = anx_strdup(m->granulerate);
      id = anx_strdup (m->id);
      content_type = anx_strdup (m->contenttype);
      src = anx_strdup (m->src);
      start_time = m->start_time ? m->start_time->t.sec : 0.0;
      /* end_time = m->end_time ? m->end_time->t.sec : 0.0; */

#if 0
      seek_offset = m->seek_offset ? m->seek_offset->t.sec : 0.0;
      seek_end = m->seek_end ? m->seek_end->t.sec : -1.0;
#else
      seek_offset = 0.0;
      seek_end = -1.0;
#endif
    
#ifdef DEBUG
      printf ("import_import (%f) %s %s @%f-@%f\n", start_time, src,
	      content_type, seek_offset, seek_end);
#endif
      
      ac->import_import (start_time, src, id, content_type,
			 seek_offset, seek_end, ac->import_import_user_data);
    }
  } else {
#ifdef DEBUG
    printf ("no import_import callback\n");
#endif
  }

  return 0;
}

/* callbacks for head, media, clip elements */
static int
read_head (CMML * cmml, const CMML_Head * head, void * user_data)
{
  AnxCMML_Data * ac = (AnxCMML_Data *)user_data;
  AnxHead * h;

  h = anx_head_from_cmml_head (head);

  if (ac->import_head != NULL) {
    ac->import_head (h, ac->import_user_data);
  }

  anx_free (h);

  return 0;
}

static AnxClip *
anx_clip_from_cmml_clip (const CMML_Clip * clip)
{
  AnxClip * a;

  a = anx_malloc (sizeof (AnxClip));

  /* copy clip data across */
  a->clip_id = anx_strdup (clip->clip_id);
  a->lang = anx_strdup (clip->lang);
  a->dir = anx_strdup (clip->dir);
  a->track = anx_strdup (clip->track);
  a->anchor_id = anx_strdup (clip->anchor_id);
  a->anchor_lang = anx_strdup (clip->anchor_lang);
  a->anchor_dir = anx_strdup (clip->anchor_dir);
  a->anchor_class = anx_strdup (clip->anchor_class);
  a->anchor_href = anx_strdup (clip->anchor_href);
  a->anchor_text = anx_strdup (clip->anchor_text);
  a->img_id = anx_strdup (clip->img_id);
  a->img_lang = anx_strdup (clip->img_lang);
  a->img_dir = anx_strdup (clip->img_dir);
  a->img_src = anx_strdup (clip->img_src);
  a->img_alt = anx_strdup (clip->img_alt);
  a->desc_id = anx_strdup (clip->desc_id);
  a->desc_lang = anx_strdup (clip->desc_lang);
  a->desc_dir = anx_strdup (clip->desc_dir);
  a->desc_text = anx_strdup (clip->desc_text);

  a->meta =
    anx_list_clone_with ((AnxList *)clip->meta,
			 (AnxCloneFunc)anx_meta_element_clone);

  return a;
}

static int
read_clip (CMML * cmml, const CMML_Clip * clip, void * user_data)
{
  AnxCMML_Data * ac = (AnxCMML_Data *)user_data;
  AnxClip * a;

  if (ac->import_clip) {
    a = anx_clip_from_cmml_clip (clip);

#ifdef DEBUG
    {
      char buf[1024];
      double start, end;

      start = clip->start_time ? clip->start_time->t.sec : -1.0;
      end = clip->end_time ? clip->end_time->t.sec : -1.0;

      anx_clip_snprint (buf, 1024, a, start, end);
      printf ("anxcmml::read_clip: %s\n", buf);
    }
#endif

    if (clip->start_time != NULL) {
      ac->import_clip (a, clip->start_time->t.sec,
			 ac->import_user_data);
    }
    if (clip->end_time != NULL) {
      ac->import_clip (NULL, clip->end_time->t.sec,
			 ac->import_user_data);
    }

    anx_free (a);
  }

  return 0;
}

static AnxSource *
anxcmml_open (const char * path, const char * id, int ignore_media,
	      double start_time, double end_time,
	      AnxImportCallbacks * import_callbacks)
{
  AnxSource * m;
  AnxCMML_Data * ac;

  CMML * doc;
  CMML_Error * err;

  long n;

#ifdef DEBUG
  char buf[BUFSIZE];
#endif

  /* try open file for parsing and setup cmml parsing */
  if ((doc = cmml_open ((char *)path)) == NULL) {
    return NULL;
  }

#ifdef DEBUG
  fprintf (stderr, "anxcmml: opened %s\n", path);
#endif

  ac = (AnxCMML_Data *) anx_malloc (sizeof (AnxCMML_Data));

  ac->import_stream = import_callbacks->import_stream;
  ac->import_head = import_callbacks->import_head;
  ac->import_clip = import_callbacks->import_clip;
  ac->import_user_data = import_callbacks->import_user_data;

  ac->import_import = import_callbacks->import_import;
  ac->import_import_user_data = import_callbacks->import_import_user_data;

  /* parse cmml file and register commands */
  cmml_set_read_callbacks (doc, read_stream, read_head, read_clip, ac);

  /* handle start/end times */

  while ((n = cmml_read (doc, BUFSIZE)) > 0) {
    /* if error reading, print and exit */
    if ((err = cmml_get_last_error(doc)) && err->type != CMML_EOF) {
#ifdef DEBUG
      cmml_error_snprint(buf, BUFSIZE, err, doc);
      fprintf (stderr, "Failed to parse %s: %s\n", path, buf);
#endif
    }
  }

  if (n == -1) {
    if ((err = cmml_get_last_error(doc)) && err->type != CMML_EOF) {
#ifdef DEBUG
      cmml_error_snprint(buf, BUFSIZE, err, doc);
      fprintf (stderr, "Failed to parse %s: %s\n", path, buf);
#endif
    }
  }

  cmml_close(doc);

  m = (AnxSource *) anx_malloc (sizeof (AnxSource));
  m->tracks = NULL;
  m->current_track = NULL;
  m->eos = 0;
  m->start_time = 0;
  m->end_time = 0;
  m->custom_data = NULL;

  return m;
}

static int
anxcmml_close (AnxSource * media)
{
  anx_free (media);
  return 0;
}

static struct _AnxImporter anx_cmml_importer = {
  (AnxImporterOpenFunc)anxcmml_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxcmml_close,
  (AnxImporterReadFunc)NULL,
  (AnxImporterSizeofNextReadFunc)NULL,
  "text/cmml"
};

static struct _AnxImporter anx_x_cmml_importer = {
  (AnxImporterOpenFunc)anxcmml_open,
  (AnxImporterOpenFDFunc)NULL,
  (AnxImporterCloseFunc)anxcmml_close,
  (AnxImporterReadFunc)NULL,
  (AnxImporterSizeofNextReadFunc)NULL,
  "text/x-cmml"
};

AnxImporter *
anx_importer_init (int i)
{
  if (i == 0)
    return &anx_cmml_importer;
  else if (i == 1)
    return &anx_x_cmml_importer;
  else
    return NULL;
}
