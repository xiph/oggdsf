/* Copyright (C) 2003 CSIRO Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the CSIRO nor the names of its
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

/* cmml_element.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <expat.h>

#include "cmml_snprint.h"

/* include files for cmml only */
#include "cmml_private.h"
#include "cmml.h"


/* --- stand-alone CMML element handling --- */

/*
 * cmml_preamble_new: Create a new CMML_Preamble.
 */
CMML_Preamble * 
cmml_preamble_new (char *encoding, char *id, char *lang, char *dir)
{
  CMML_Preamble * p;
  p = (CMML_Preamble *) cmml_malloc(sizeof(CMML_Preamble));
  p->xml_version = cmml_strdup("1.0");
  if (encoding) {
    p->xml_encoding = cmml_strdup(encoding);
  } else {
    p->xml_encoding = cmml_strdup("UTF-8");
  }
  p->xml_standalone   = 1;
  p->doctype_declared = 1;
  if (lang) {
    p->cmml_lang = cmml_strdup(lang);
  } else {
    p->cmml_lang = NULL;
  }
  if (dir) {
    p->cmml_dir = cmml_strdup(dir);
  } else {
    p->cmml_dir = NULL;
  }
  if (id) {
    p->cmml_id = cmml_strdup(id);
  } else {
    p->cmml_id = NULL;
  }
  p->cmml_xmlns = cmml_strdup("http://www.annodex.net/cmml");
  return p;
}

/*
 * cmml_element_new: initialise memory for a new CMML_Element.
 */
CMML_Element *
cmml_element_new(CMML_Element_Type type)
{
  CMML_Element * elem = cmml_malloc(sizeof(*elem));
  
  switch (type) {
  case CMML_STREAM:
    elem->e.stream = cmml_stream_new();
    break;
  case CMML_HEAD:
    elem->e.head = cmml_head_new();
    break;
  case CMML_CLIP:
    elem->e.clip = cmml_clip_new(NULL, NULL);
    break;
  default:
    cmml_free(elem);
    return NULL;
    break;
  }
  elem->type = type;
  
  return elem;
}

CMML_Stream * 
cmml_stream_new ()
{
  CMML_Stream * s;
  s = (CMML_Stream *) cmml_malloc(sizeof(CMML_Stream));
  s->id = NULL;
  s->timebase = NULL;
  s->utc = NULL;
  s->import = cmml_list_new();
  return s;
}

CMML_Head * 
cmml_head_new    (void)
{
  CMML_Head * h;
  h = (CMML_Head *) cmml_malloc(sizeof(CMML_Head));
  h->head_id = NULL;
  h->lang = NULL;
  h->dir = NULL;
  h->profile = NULL;
  h->title = NULL;
  h->title_id = NULL;
  h->title_lang = NULL;
  h->title_dir = NULL;
  h->base_id = NULL;
  h->base_href = NULL;
  h->meta = cmml_list_new();
  return h;
}

CMML_Clip  * 
cmml_clip_new (CMML_Time *start_time, CMML_Time *end_time)
{
  CMML_Clip * a;
  a = (CMML_Clip *) cmml_malloc(sizeof(CMML_Clip));
  a->clip_id      = NULL;
  a->lang         = NULL;
  a->dir          = NULL;
  a->track        = NULL;
  a->start_time   = cmml_time_clone(start_time);
  a->end_time     = cmml_time_clone(end_time);
  a->meta         = cmml_list_new();
  a->anchor_id    = NULL;
  a->anchor_lang  = NULL;
  a->anchor_dir   = NULL;
  a->anchor_class = NULL;
  a->anchor_href  = NULL;
  a->anchor_text  = NULL;
  a->img_id       = NULL;
  a->img_lang     = NULL;
  a->img_dir      = NULL;
  a->img_src      = NULL;
  a->img_alt      = NULL;
  a->desc_id      = NULL;
  a->desc_lang    = NULL;
  a->desc_dir     = NULL;
  a->desc_text    = NULL;
  return a;
}

CMML_Error * 
cmml_error_new (CMML_Error_Type type)
{
  CMML_Error * e;
  e = (CMML_Error *) cmml_malloc(sizeof(CMML_Error));
  e->type = type;
  e->line = 0;
  e->col = 0;
  return e;
}

/*
 * Cloning functions
 */

static CMML_ParamElement *
cmml_param_element_clone (CMML_ParamElement * param)
{
  CMML_ParamElement * new_param;

  if (param == NULL) return NULL;

  new_param = (CMML_ParamElement* ) cmml_malloc (sizeof (CMML_ParamElement));
  /* maybe consider memcpy for speedup and introduce reference counting!
     memcpy (new_param, param, sizeof (*new_param));
     -> NO: may introduce a problem with byte alignment
  */
  new_param->id    = cmml_strdup (param->id);
  new_param->name  = cmml_strdup (param->name);
  new_param->value = cmml_strdup (param->value);
  return new_param;
}

static CMML_ImportElement *
cmml_import_element_clone (CMML_ImportElement * import)
{
  CMML_ImportElement * new_import;
  if (import == NULL) return NULL;
  new_import = (CMML_ImportElement*) cmml_malloc (sizeof (CMML_ImportElement));
  new_import->id          = cmml_strdup (import->id);
  new_import->lang        = cmml_strdup (import->lang);
  new_import->dir         = cmml_strdup (import->dir);
  new_import->granulerate = cmml_strdup (import->granulerate);
  new_import->contenttype = cmml_strdup (import->contenttype);
  new_import->src         = cmml_strdup (import->src);
  new_import->start_time  = cmml_time_clone (import->start_time);
  new_import->end_time    = cmml_time_clone (import->end_time);
  new_import->title       = cmml_strdup (import->title);
  new_import->param       = 
    cmml_list_clone_with (import->param,
			  (CMML_CloneFunc)cmml_param_element_clone);
  return new_import;
}

static CMML_MetaElement *
cmml_meta_element_clone (CMML_MetaElement * meta)
{
  CMML_MetaElement * new_meta;

  if (meta == NULL) return NULL;

  new_meta = (CMML_MetaElement* ) cmml_malloc (sizeof (CMML_MetaElement));
  /* maybe consider memcpy for speedup and introduce reference counting!
     memcpy (new_meta, meta, sizeof (*new_meta));
  */
  new_meta->id      = cmml_strdup (meta->id);
  new_meta->lang    = cmml_strdup (meta->lang);
  new_meta->dir     = cmml_strdup (meta->dir);
  new_meta->name    = cmml_strdup (meta->name);
  new_meta->content = cmml_strdup (meta->content);
  new_meta->scheme  = cmml_strdup (meta->scheme);
  return new_meta;
}

CMML_Preamble * 
cmml_preamble_clone (CMML_Preamble * src)
{
  CMML_Preamble * p;
  if (src == NULL) return NULL;
  p = (CMML_Preamble *) cmml_malloc(sizeof(CMML_Preamble));
  p->xml_version      = cmml_strdup(src->xml_version);
  p->xml_encoding     = cmml_strdup(src->xml_encoding);
  p->xml_standalone   = src->xml_standalone;
  p->doctype_declared = src->doctype_declared;
  p->cmml_lang        = cmml_strdup(src->cmml_lang);
  p->cmml_dir         = cmml_strdup(src->cmml_dir);
  p->cmml_id          = cmml_strdup(src->cmml_id);
  p->cmml_xmlns       = cmml_strdup(src->cmml_xmlns);
  return p;
}

/*
 * cmml_element_clone: create a copy of a CMML_Element.
 */
CMML_Element *
cmml_element_clone(CMML_Element * src)
{
  CMML_Element * elem;
  elem = cmml_element_new(src->type);
  elem->type = src->type;
  switch (elem->type) {
  case CMML_STREAM:
    elem->e.stream = cmml_stream_clone(src->e.stream);
    break;
  case CMML_HEAD:
    elem->e.head = cmml_head_clone(src->e.head);
    break;
  case CMML_CLIP:
    elem->e.clip = cmml_clip_clone(src->e.clip);
    break;
  default:
    cmml_free(elem);
    return NULL;
    break;
  }
  return elem;
}

CMML_Stream *
cmml_stream_clone(CMML_Stream * src)
{
  CMML_Stream * s;
  if (src == NULL) return NULL;
  s = (CMML_Stream *) cmml_malloc (sizeof (CMML_Stream));
  /*
  memcpy (s, src, sizeof (*s));
  */
  s->id       = cmml_strdup(src->id);
  s->timebase = cmml_time_clone(src->timebase);
  if (src->utc != NULL) {
    s->utc      = cmml_time_clone(src->utc);
  } else {
    s->utc = NULL;
  }
  s->import    =
    cmml_list_clone_with (src->import, (CMML_CloneFunc)cmml_import_element_clone);
  return s;
}

CMML_Head *
cmml_head_clone(CMML_Head * head)
{
  CMML_Head * new_head;

  if (head == NULL) return NULL;
  new_head = (CMML_Head *) cmml_malloc (sizeof (CMML_Head));
  /*
  memcpy (new_head, head, sizeof (*new_head));
  */
  new_head->head_id    = cmml_strdup(head->head_id);
  new_head->lang       = cmml_strdup(head->lang);
  new_head->dir        = cmml_strdup(head->dir);
  new_head->profile    = cmml_strdup(head->profile);
  new_head->title      = cmml_strdup(head->title);
  new_head->title_id   = cmml_strdup(head->title_id);
  new_head->title_lang = cmml_strdup(head->title_lang);
  new_head->title_dir  = cmml_strdup(head->title_dir);
  new_head->base_id    = cmml_strdup(head->base_id);
  new_head->base_href  = cmml_strdup(head->base_href);
  new_head->meta =
    cmml_list_clone_with (head->meta, (CMML_CloneFunc)cmml_meta_element_clone);
  return new_head;

}

CMML_Clip  * 
cmml_clip_clone (CMML_Clip * src)
{
  CMML_Clip * a;
  if (src == NULL) return NULL;
  a = (CMML_Clip *) cmml_malloc (sizeof (CMML_Clip));
  /*
  memcpy (a, src, sizeof (*a));
  */
  a->clip_id    = cmml_strdup(src->clip_id);
  a->lang       = cmml_strdup(src->lang);
  a->dir        = cmml_strdup(src->dir);
  a->track      = cmml_strdup(src->track);
  a->start_time = cmml_time_clone(src->start_time);
  a->end_time   = cmml_time_clone(src->end_time);
  a->meta =
    cmml_list_clone_with (src->meta, (CMML_CloneFunc)cmml_meta_element_clone);
  a->anchor_id       = cmml_strdup(src->anchor_id);
  a->anchor_lang     = cmml_strdup(src->anchor_lang);
  a->anchor_dir      = cmml_strdup(src->anchor_dir);
  a->anchor_class    = cmml_strdup(src->anchor_class);
  a->anchor_href     = cmml_strdup(src->anchor_href);
  a->anchor_text     = cmml_strdup(src->anchor_text);
  a->img_id       = cmml_strdup(src->img_id);
  a->img_lang     = cmml_strdup(src->img_lang);
  a->img_dir      = cmml_strdup(src->img_dir);
  a->img_src      = cmml_strdup(src->img_src);
  a->img_alt      = cmml_strdup(src->img_alt);
  a->desc_id      = cmml_strdup(src->desc_id);
  a->desc_lang    = cmml_strdup(src->desc_lang);
  a->desc_dir     = cmml_strdup(src->desc_dir);
  a->desc_text    = cmml_strdup(src->desc_text);
  return a;
}

/*
 * Destroying functions
 */
static CMML_ParamElement *
cmml_param_element_destroy (CMML_ParamElement * param)
{
  if (param == NULL) return NULL;
  cmml_free (param->id);
  cmml_free (param->name);
  cmml_free (param->value);
  return NULL;
}

CMML_ImportElement *
cmml_import_element_destroy (CMML_ImportElement * import)
{
  if (import == NULL) return NULL;
  cmml_free (import->id);
  cmml_free (import->lang);
  cmml_free (import->dir);
  cmml_free (import->granulerate);
  cmml_free (import->contenttype);
  cmml_free (import->src);
  cmml_free (import->start_time);
  cmml_free (import->end_time);
  cmml_free (import->title);
  cmml_list_free_with(import->param,
		      (CMML_FreeFunc) cmml_param_element_destroy);
  cmml_free (import);
  return NULL;
}

CMML_MetaElement *
cmml_meta_element_destroy (CMML_MetaElement * meta)
{
  if (meta == NULL) return NULL;
  cmml_free (meta->id);
  cmml_free (meta->lang);
  cmml_free (meta->dir);
  cmml_free (meta->name);
  cmml_free (meta->content);
  cmml_free (meta->scheme);
  cmml_free (meta);
  return NULL;
}

void
cmml_preamble_destroy(CMML_Preamble * info)
{
  if (info == NULL) return;
  cmml_free(info->xml_version);
  cmml_free(info->xml_encoding);
  cmml_free(info->cmml_lang);
  cmml_free(info->cmml_dir);
  cmml_free(info->cmml_id);
  cmml_free(info->cmml_xmlns);
  cmml_free(info);
  return;
}

/*
 * cmml_element_destroy: interface described in cmml.h
 */
void 
cmml_element_destroy (CMML_Element * elem)
{
  if (!elem) return;
  switch (elem->type) {
  case CMML_STREAM:
    cmml_stream_destroy(elem->e.stream);
    break;
  case CMML_HEAD:
    cmml_head_destroy(elem->e.head);
    break;
  case CMML_CLIP:
    cmml_clip_destroy(elem->e.clip);
    break;
  default:
    break;
  }
  cmml_free(elem);
}

/*
 * cmml_stream_destroy: free all memory associated with a CMML_Stream
 */
void 
cmml_stream_destroy(CMML_Stream * s)
{
  if (s == NULL) return;
  cmml_free(s->id);
  cmml_time_free(s->timebase);
  cmml_time_free(s->utc);
  cmml_list_free_with(s->import, (CMML_FreeFunc) cmml_import_element_destroy);
  cmml_free(s);
  return;
}

/*
 * cmml_head_destroy: free all memory associated with a CMML_Head
 */
void 
cmml_head_destroy(CMML_Head * h)
{
  if (h == NULL) return;
  cmml_free(h->head_id);
  cmml_free(h->lang);
  cmml_free(h->dir);
  cmml_free(h->profile);
  cmml_free(h->title);
  cmml_free(h->title_id);
  cmml_free(h->title_lang);
  cmml_free(h->title_dir);
  cmml_free(h->base_id);
  cmml_free(h->base_href);
  cmml_list_free_with(h->meta, (CMML_FreeFunc) cmml_meta_element_destroy);
  cmml_free(h);
  return;
}

/*
 * cmml_clip_destroy: free all memory associated with a CMML_Clip
 */
void 
cmml_clip_destroy(CMML_Clip * a)
{
  if (a == NULL) return;
  cmml_free(a->clip_id);
  cmml_free(a->track);
  cmml_free(a->lang);
  cmml_free(a->dir);
  cmml_time_free(a->start_time);
  cmml_time_free(a->end_time);
  cmml_list_free_with(a->meta, (CMML_FreeFunc) cmml_meta_element_destroy);
  cmml_free(a->anchor_id);
  cmml_free(a->anchor_lang);
  cmml_free(a->anchor_dir);
  cmml_free(a->anchor_class);
  cmml_free(a->anchor_href);
  cmml_free(a->anchor_text);
  cmml_free(a->img_id);
  cmml_free(a->img_lang);
  cmml_free(a->img_dir);
  cmml_free(a->img_src);
  cmml_free(a->img_alt);
  cmml_free(a->desc_id);
  cmml_free(a->desc_lang);
  cmml_free(a->desc_dir);
  cmml_free(a->desc_text);
  cmml_free(a);
  return;
}

/*
 * cmml_error_destroy: free all memory associated with a CMML_Clip
 */
void
cmml_error_destroy(CMML_Error *error)
{
  if (!error) return;
  cmml_free(error);
  return;
}

/* return number of characters written */
static
int cmml_param_list_snprint (char *buf, int *_len, int *_n, int *_n_written, 
			    CMML_List * param)
{
  int len = *_len;
  int n = *_n;
  int n_written = *_n_written;
  
  if (!cmml_list_is_empty(param)) {
    CMML_List * param_node;
      
    for (param_node = param;
	 param_node != NULL;
	 param_node = param_node->next) {
      CMML_ParamElement * m = (CMML_ParamElement *) param_node->data;
	
      BEGIN_TAG("param");
      APPEND_ATTRIBUTE("id", m->id);
      APPEND_ATTRIBUTE("name", m->name);
      APPEND_ATTRIBUTE("value", m->value);
      END_EMPTY_TAG();
      NEWLINE;
    }
  }
  *_len = len;
  *_n = n;
  *_n_written = n_written;
  return 0;
}

/* return number of characters written */
static
int cmml_import_list_snprint (char *buf, int *_len, int *_n, int *_n_written, 
			     CMML_List * import)
{
  int len = *_len;
  int n = *_n;
  int n_written = *_n_written;
  
  if (!cmml_list_is_empty(import)) {
    CMML_List * import_node;
      
    for (import_node = import;
	 import_node != NULL;
	 import_node = import_node->next) {
      CMML_ImportElement * m = (CMML_ImportElement *) import_node->data;
	
      BEGIN_TAG("import");
      APPEND_ATTRIBUTE("id", m->id);
      APPEND_ATTRIBUTE("lang", m->lang);
      APPEND_ATTRIBUTE("dir", m->dir);
      APPEND_ATTRIBUTE("granulerate", m->granulerate);
      APPEND_ATTRIBUTE("contenttype", m->contenttype);
      APPEND_ATTRIBUTE("src", m->src);
      if (m->start_time != NULL) {
	APPEND_ATTRIBUTE("start", m->start_time->tstr);
      }
      if (m->end_time != NULL) {
	APPEND_ATTRIBUTE("end", m->end_time->tstr);
      }
      APPEND_ATTRIBUTE("title", m->title);
      if(!cmml_list_is_empty(m->param)) {
	END_TAG();
	NEWLINE;
	if (cmml_param_list_snprint(buf, &len, &n, &n_written, m->param) < 0)
	  return -1;
	CLOSE_TAG("import");
      } else {
	END_EMPTY_TAG();
      }
      NEWLINE;
    }
  }
  *_len = len;
  *_n = n;
  *_n_written = n_written;
  return 0;
}

/* return number of characters written */
static
int cmml_meta_list_snprint (char *buf, int *_len, int *_n, int *_n_written, 
			    CMML_List * meta)
{
  int len = *_len;
  int n = *_n;
  int n_written = *_n_written;
  
  if (!cmml_list_is_empty(meta)) {
    CMML_List * meta_node;
      
    for (meta_node = meta;
	 meta_node != NULL;
	 meta_node = meta_node->next) {
      CMML_MetaElement * m = (CMML_MetaElement *) meta_node->data;
	
      BEGIN_TAG("meta");
      APPEND_ATTRIBUTE("id", m->id);
      APPEND_ATTRIBUTE("lang", m->lang);
      APPEND_ATTRIBUTE("dir", m->dir);
      APPEND_ATTRIBUTE("name", m->name);
      APPEND_ATTRIBUTE("content", m->content);
      APPEND_ATTRIBUTE("scheme", m->scheme);
      END_EMPTY_TAG();
      NEWLINE;
    }
  }
  *_len = len;
  *_n = n;
  *_n_written = n_written;
  return 0;
}

int
cmml_preamble_snprint(char *buf, int n, CMML_Preamble *pre)
{
  int len = 0;
  int n_written = 0;

  if (!pre) return 0;

  /* write xml processing instruction */
  if ((n_written = cmml_snprintf(buf + len, n, "<?xml")) == -1) {
    return -1;
  }
  n -= n_written;
  len += n_written;
  APPEND_ATTRIBUTE("version",    pre->xml_version);
  APPEND_ATTRIBUTE("encoding",   pre->xml_encoding);
  if (pre->xml_standalone == 0) {
    APPEND_ATTRIBUTE("standalone", "no");
  } else if (pre->xml_standalone == 1) {
    APPEND_ATTRIBUTE("standalone", "yes");
  }
  if ((n_written = cmml_snprintf(buf + len, n, "?>\n")) == -1) {
    return -1;
  }
  n -= n_written;
  len += n_written;

  /* write doctype declaration */
  if (pre->doctype_declared) {
    if ((n_written = 
	 cmml_snprintf(buf + len, n, "<!DOCTYPE cmml SYSTEM \"cmml.dtd\">\n\n")) == -1) {
      return -1;
    }
    n -= n_written;
    len += n_written;
  }
  
  /* write cmml tag */
  BEGIN_TAG("cmml");
  APPEND_ATTRIBUTE("id", pre->cmml_id);
  APPEND_ATTRIBUTE("lang", pre->cmml_lang);
  APPEND_ATTRIBUTE("dir", pre->cmml_dir);
  /* to print or not to print?
  APPEND_ATTRIBUTE("xmlns", pre->cmml_xmlns);
  */
  END_TAG();
  NEWLINE;

  return len;
}

/*
 * cmml_element_snprint: Given a buffer buf of size n, write a CMML
 * representation of a CMML_Element into it.  Return the number of
 * characters actually written to the buffer (including the
 * terminating '\0').  If buf is too small, return -1.
 *
 * cmml_{stream,head,clip}_snprint work similarly but operate only on
 * elements of the specified type.
 *
 * cmml_error_snprint prints a string description of the CMML_Error
 */

int 
cmml_element_snprint(char *buf, int n, CMML_Element *elem)
{
  int n_written = 0;

  if (!elem) return 0;
  /* Deduce the type of the CMML_Element and call the appropriate */
  switch (elem->type) {
  case CMML_STREAM:
    n_written = cmml_stream_snprint(buf, n, (CMML_Stream*) elem->e.stream);
    break;
  case CMML_HEAD:
    n_written = cmml_head_snprint(buf, n, (CMML_Head*) elem->e.head);
    break;
  case CMML_CLIP:
    n_written = cmml_clip_snprint(buf, n, (CMML_Clip*) elem->e.clip);
    break;
  default:
    break;
  }

  return n_written;
}

int 
cmml_stream_snprint(char *buf, int n, CMML_Stream * m)
{
  int len = 0;
  int n_written;
  
  if (m == NULL) return 0;
  
  BEGIN_TAG("stream");
  APPEND_ATTRIBUTE("id", m->id);
  if (m->timebase) {
    APPEND_ATTRIBUTE("timebase", m->timebase->tstr);
  }
  if (m->utc) {
    APPEND_ATTRIBUTE("utc", m->utc->tstr);
  }
  END_TAG();

  if (cmml_import_list_snprint(buf, &len, &n, &n_written, m->import) < 0)
    return -1;
  CLOSE_TAG("stream");
  
  return len;
}

int 
cmml_stream_pretty_snprint(char *buf, int n, CMML_Stream * m)
{
  int len = 0;
  int n_written;
  
  if (m == NULL) return 0;
  
  BEGIN_TAG("stream");
  APPEND_ATTRIBUTE("id", m->id);
  if (m->timebase) {
    APPEND_ATTRIBUTE("timebase", m->timebase->tstr);
  }
  if (m->utc) {
    APPEND_ATTRIBUTE("utc", m->utc->tstr);
  }
  END_TAG();
  NEWLINE;

  if (cmml_import_list_snprint(buf, &len, &n, &n_written, m->import) < 0)
    return -1;
  CLOSE_TAG("stream");
  NEWLINE;

  return len;
}

int 
cmml_head_snprint(char *buf, int n, CMML_Head * h)
{
    int len = 0;
    int n_written;

    if (h == NULL) return 0;

    BEGIN_TAG("head");
    APPEND_ATTRIBUTE("id", h->head_id);
    APPEND_ATTRIBUTE("lang", h->lang);
    APPEND_ATTRIBUTE("dir", h->dir);
    APPEND_ATTRIBUTE("profile", h->profile);
    END_TAG();

    if (h->title || h->title_id || h->title_lang || h->title_dir) {
        BEGIN_TAG("title");
        APPEND_ATTRIBUTE("id", h->title_id);
        APPEND_ATTRIBUTE("lang", h->title_lang);
        APPEND_ATTRIBUTE("dir", h->title_dir);
        END_TAG();
        APPEND_DATA(h->title);
        CLOSE_TAG("title");
    }

    if (h->base_id || h->base_href) {
        BEGIN_TAG("base");
        APPEND_ATTRIBUTE("id", h->base_id);
        APPEND_ATTRIBUTE("href", h->base_href);
        END_EMPTY_TAG();
    }

    if (cmml_meta_list_snprint(buf, &len, &n, &n_written, h->meta) < 0)
      return -1;
    CLOSE_TAG("head");
    
    return len;
}

int 
cmml_head_pretty_snprint(char *buf, int n, CMML_Head * h)
{
  int len = 0;
  int n_written;
  
  if (h == NULL) return 0;

  BEGIN_TAG("head");
  APPEND_ATTRIBUTE("id", h->head_id);
  APPEND_ATTRIBUTE("lang", h->lang);
  APPEND_ATTRIBUTE("dir", h->dir);
  APPEND_ATTRIBUTE("profile", h->profile);
  END_TAG();
  NEWLINE;
    
  if (h->title || h->title_id || h->title_lang || h->title_dir) {
    BEGIN_TAG("title");
    APPEND_ATTRIBUTE("id", h->title_id);
    APPEND_ATTRIBUTE("lang", h->title_lang);
    APPEND_ATTRIBUTE("dir", h->title_dir);
    END_TAG();
    APPEND_DATA(h->title);
    CLOSE_TAG("title");
    NEWLINE;
  }

  if (h->base_id || h->base_href) {
    BEGIN_TAG("base");
    APPEND_ATTRIBUTE("id", h->base_id);
    APPEND_ATTRIBUTE("href", h->base_href);
    END_EMPTY_TAG();
    NEWLINE;
  }

  if (cmml_meta_list_snprint(buf, &len, &n, &n_written, h->meta) < 0)
    return -1;
  CLOSE_TAG("head");
  NEWLINE;

  return len;
}

int cmml_clip_snprint(char *buf, int n, CMML_Clip * a)
{
  int len = 0;
  int n_written;
  
  if (a == NULL) return 0;
  
  BEGIN_TAG("clip");
  APPEND_ATTRIBUTE("id", a->clip_id);
  APPEND_ATTRIBUTE("lang", a->lang);
  APPEND_ATTRIBUTE("dir", a->dir);
  APPEND_ATTRIBUTE("track", a->track);
  if (a->start_time) APPEND_ATTRIBUTE("start", a->start_time->tstr);
  if (a->end_time)   APPEND_ATTRIBUTE("end", a->end_time->tstr);
  END_TAG();

  /* print meta tags */
  if (cmml_meta_list_snprint(buf, &len, &n, &n_written, a->meta) < 0)
    return -1;

  /* print anchor tag */
  if (a->anchor_id || a->anchor_text || a->anchor_href) {
    BEGIN_TAG("a");
    APPEND_ATTRIBUTE("id",    a->anchor_id);
    APPEND_ATTRIBUTE("lang",  a->anchor_lang);
    APPEND_ATTRIBUTE("dir",   a->anchor_dir);
    APPEND_ATTRIBUTE("class", a->anchor_class);
    APPEND_ATTRIBUTE("href",  a->anchor_href);
    END_TAG();
    APPEND_DATA(a->anchor_text);
    CLOSE_TAG("a");
  }

  /* print image tag */
  if (a->img_id || a->img_src || a->img_alt) {
    BEGIN_TAG("img");
    APPEND_ATTRIBUTE("id",   a->img_id);
    APPEND_ATTRIBUTE("lang", a->img_lang);
    APPEND_ATTRIBUTE("dir",  a->img_dir);
    APPEND_ATTRIBUTE("src",  a->img_src);
    APPEND_ATTRIBUTE("alt",  a->img_alt);
    END_EMPTY_TAG();
  }

  /* print desc tag */
  if (a->desc_id || a->desc_text) {
    BEGIN_TAG("desc");
    APPEND_ATTRIBUTE("id",   a->desc_id);
    APPEND_ATTRIBUTE("lang", a->desc_lang);
    APPEND_ATTRIBUTE("dir",  a->desc_dir);
    END_TAG();
    APPEND_DATA(a->desc_text);
    CLOSE_TAG("desc");
  }

  CLOSE_TAG("clip");
  return len;
}

int cmml_clip_pretty_snprint(char *buf, int n, CMML_Clip * a)
{
  int len = 0;
  int n_written;
  
  if (a == NULL) return 0;
  
  BEGIN_TAG("clip");
  APPEND_ATTRIBUTE("id", a->clip_id);
  APPEND_ATTRIBUTE("lang", a->lang);
  APPEND_ATTRIBUTE("dir", a->dir);
  APPEND_ATTRIBUTE("track", a->track);
  if (a->start_time) APPEND_ATTRIBUTE("start", a->start_time->tstr);
  if (a->end_time)   APPEND_ATTRIBUTE("end", a->end_time->tstr);
  END_TAG();
  NEWLINE;

  /* print meta list */  
  if (cmml_meta_list_snprint(buf, &len, &n, &n_written, a->meta) < 0)
    return -1;

  /* print anchor tag */
  if (a->anchor_id || a->anchor_text || a->anchor_href) {
    BEGIN_TAG("a");
    APPEND_ATTRIBUTE("id", a->anchor_id);
    APPEND_ATTRIBUTE("lang", a->anchor_lang);
    APPEND_ATTRIBUTE("dir", a->anchor_dir);
    APPEND_ATTRIBUTE("class", a->anchor_class);
    APPEND_ATTRIBUTE("href", a->anchor_href);
    END_TAG();
    APPEND_DATA(a->anchor_text);
    CLOSE_TAG("a");
    NEWLINE;
  }

  /* print image tag */
  if (a->img_id || a->img_src || a->img_alt) {
    BEGIN_TAG("img");
    APPEND_ATTRIBUTE("id", a->img_id);
    APPEND_ATTRIBUTE("lang", a->img_lang);
    APPEND_ATTRIBUTE("dir", a->img_dir);
    APPEND_ATTRIBUTE("src", a->img_src);
    APPEND_ATTRIBUTE("alt", a->img_alt);
    END_EMPTY_TAG();
    NEWLINE;
  }
  
  /* print desc tag */
  if (a->desc_id || a->desc_text) {
    BEGIN_TAG("desc");
    APPEND_ATTRIBUTE("id", a->desc_id);
    APPEND_ATTRIBUTE("lang", a->desc_lang);
    APPEND_ATTRIBUTE("dir", a->desc_dir);
    END_TAG();
    APPEND_DATA(a->desc_text);
    CLOSE_TAG("desc");
    NEWLINE;
  }
  
  CLOSE_TAG("clip");
  NEWLINE;
  return len;

}

int 
cmml_error_snprint   (char *buf, int n, CMML_Error *error, CMML *cmml) 
{
  int len = 0;
  int n_written;
  
  if (error == NULL) {
    return 0;
  }

  switch (error->type) {
  case CMML_OK:
    REPORT (error, "No error");
    break;
  case CMML_EOF:
    REPORT (error, "End of file reached");
    break;
  case CMML_READ_ERROR:
    REPORT (error, "Error reading CMML doc");
    break;
  case CMML_TIME_ERROR:
    REPORT (error, "Error formatting time");
    break;
  case CMML_MALLOC_ERROR:
    REPORT (error, "Error memory allocation");
    break;
  case CMML_EXPAT_ERROR:
    REPORT (error, "Expat error parsing CMML file");

    n_written = cmml_snprintf(buf + len, n, 
			      XML_ErrorString(XML_GetErrorCode(((CMML_Context*)cmml)->parser)));
    if (n_written == -1) {
      return -1;
    }
    n -= n_written;
    len += n_written;
    break;
  case CMML_PARSE_ERROR:
    REPORT (error, "Error parsing CMML file");
    break;
  case CMML_NO_CMML_TAG:
    REPORT (error, "cmml tag missing");
    break;
  case CMML_NO_HEAD_TAG:
    REPORT (error, "head tag missing");
    break;
  case CMML_STREAM_NOT_FIRST:
    REPORT (error, "head or clip tag before stream tag not allowed");
    break;
  case CMML_HEAD_AFTER_CLIP:
    REPORT (error, "head tag after clip tag not allowed");
    break;
  case CMML_DUPLICATE_STREAM:
    REPORT (error, "Multiple stream tags not allowed");
    break;
  case CMML_DUPLICATE_HEAD:
    REPORT (error, "Multiple head tags not allowed");
    break;

  /* sloppy errors from now on */
  case CMML_FORMAT_ERROR:
    REPORT (error, "Skipped implied attribute of wrong format");
    break;
  case CMML_UNKNOWN_TAG:
    REPORT (error, "Skipped unknown tag");
    break;
  case CMML_TAG_IGNORED:
    REPORT (error, "Skipped doubly defined tag or tag with missing required attribute");
    break;
  case CMML_XMLNS_REDEFINED:
    REPORT (error, "Skipped xmlns in cmml tag wrongly redefined");
    break;
  case CMML_NONSEQUENTIAL_CLIP:
    REPORT (error, "Skipped clip out of temporal order");
    break;
  default:
    REPORT (error, "Unknown error");
    break;
  }
  
  return len;
}
