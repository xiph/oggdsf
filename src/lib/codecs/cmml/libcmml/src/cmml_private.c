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

/* cmml_private.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include <expat.h>

/* include files for cmml only */
#include "cmml_private.h"
#include "cmml.h"

/* for debugging functions in this file
#define DEBUG
*/

/*
 * cmml_parse_to_head: parse the top bit of the cmml file until it
 * reaches the head tag to get all the timebase etc. information
 */
int
cmml_parse_to_head (CMML_Context * ctxt)
{
#define READ_BUF_SIZE 16
  XML_Char buf[READ_BUF_SIZE];
  long bytes, nread = 0;

  /* if stream tag had not previously been read, read until you've got it by */
  /* checking for timebase as this is a stream attribute that is obligatory */
  /* or until you have a head tag */
  while (ctxt->status->stream == NULL || 
	 ctxt->status->stream->timebase == NULL) {
	  if ((bytes = (long)fread(buf, 1, READ_BUF_SIZE, ctxt->file)) == -1) {
      cmml_set_error((CMML*)ctxt, CMML_READ_ERROR);
      return -1;
    }
    nread += bytes;

    /* XML_Parse activates registered expat callbacks */
    if (XML_Parse (ctxt->parser, buf, bytes, bytes == 0) == XML_STATUS_ERROR) {
      if (cmml_get_last_error((CMML*)ctxt) != NULL) return -1;
      cmml_set_error ((CMML*)ctxt, CMML_EXPAT_ERROR);
      return -1;
    }

    /* end of file reached */
    if (bytes == 0) {
      if (cmml_get_last_error((CMML*)ctxt) == NULL) {
	cmml_set_error ((CMML*)ctxt, CMML_EOF);
      }
      return -1;
    }
  }
  return 0;
  
#undef READ_BUF_SIZE
}

/*
 * characters: handler for PCDATA between start and end tags.  
 */
static void 
characters(void *cmml, const XML_Char *ch, int len)
{
  CMML_Context * context = (CMML_Context*) cmml;
  XML_Char *p;

  /*
   * Do we have enough room in the buffer for the PCDATA offered to
   * us (plus the '\0' that strncat will put on)?  If not, then
   * repeatedly make the buffer bigger until we do have enough room.
   */
  while ((context->cdata->limit - strlen(context->cdata->base)) <= 
	     (unsigned int) len) {
    p = realloc(context->cdata->base, context->cdata->limit * 2);
    if (!p) {
      fprintf(stderr, "libcmml: memory allocation error.");
      exit(1);
    }
    context->cdata->limit *= 2;
    context->cdata->base = p;
  }
  strncat(context->cdata->base, ch, len);
}

/*
 * insert_root_attrs: insert attributes from a "cmml" root element
 * the status context
 */
static void 
insert_root_attrs(CMML_Preamble * docinfo, const XML_Char **attr, CMML_Context *ctxt)
{
  int i;

  if (!docinfo) return;

  for (i = 0; attr[i] != NULL; i += 2) {
    if (cmml_strcmp(attr[i], "id") == 0) {
      docinfo->cmml_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      docinfo->cmml_lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      docinfo->cmml_dir = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "xmlns") == 0) {
      /* this is fix so redefining it is a sloppy error */
      if (cmml_strcmp(attr[i+1], "http://www.annodex.net/cmml") != 0) {
	cmml_set_error((CMML*)ctxt, CMML_XMLNS_REDEFINED);
      }
    }
  }
  /* this is fix so ignore whatever it was set to */
  docinfo->cmml_xmlns = cmml_strdup("http://www.annodex.net/cmml");
}


/*
 * insert_stream_attrs: insert attributes from a "stream" element into
 * a CMML_Element of type CMML_HEAD.
 */
static int
insert_stream_attrs(CMML_Stream * stream, const XML_Char **attr, CMML_Context *ctxt)
{
  int i;

  if (!stream) return 0;

  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting stream attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      stream->id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "timebase") == 0) {
      stream->timebase = cmml_time_new (attr[i+1]);
    } else if (cmml_strcmp(attr[i], "utc") == 0) {
      stream->utc = cmml_utc_new (attr[i+1]);
      if (!stream->utc->t.utc) { /* just ignoring tag */
	cmml_time_free (stream->utc);
	stream->utc = NULL;
	cmml_set_error((CMML*)ctxt, CMML_FORMAT_ERROR);
      }
    }
  }
  /* set default timebase  */
  if (stream->timebase == NULL) {
    stream->timebase = cmml_time_new("0");
  }
  /* check parsed timbase as it is used later in library */
  if (stream->timebase->t.sec < 0.0) {
    cmml_set_error((CMML*)ctxt, CMML_TIME_ERROR);
    return -1; /* stream to be destroyed */
  }
  return 0;
}

/*
 * insert_import_tag: insert attributes from a "import" element into a
 * CMML_Element of type CMML_IMPORT.
 */
static CMML_List *
insert_import_tag (CMML_List * import_list, const XML_Char **attr, CMML_Context * ctxt)
{
  int i;
  CMML_ImportElement * m;

  if ((m = cmml_malloc(sizeof(*m))) == NULL) return import_list;
  m->id   = NULL;
  m->lang = NULL;
  m->dir  = NULL;
  m->granulerate = NULL;
  m->contenttype = NULL;
  m->src         = NULL;
  m->start_time  = NULL;
  m->end_time    = NULL;
  m->title       = NULL;
  m->param       = NULL;

  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting import attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      m->id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      m->lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      m->dir = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "granulerate") == 0) {
      m->granulerate = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "contenttype") == 0) {
      m->contenttype = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "src") == 0) {
      m->src = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "start") == 0) {
      m->start_time = 
	cmml_time_new_in_sec (attr[i+1],
			      ctxt->status->stream->utc,
			      ctxt->status->stream->timebase->t.sec);
    } else if (cmml_strcmp(attr[i], "end") == 0) {
      m->end_time = 
	cmml_time_new_in_sec (attr[i+1],
			      ctxt->status->stream->utc,
			      ctxt->status->stream->timebase->t.sec);
    } else if (cmml_strcmp(attr[i], "title") == 0) {
      m->title = cmml_strdup(attr[i+1]);
    }
  }
  /* set default start_time  */
  if (m->start_time == NULL) {
    m->start_time = cmml_time_new("0");
  }
  /* check if required src attribute is there */
  if (!m->src) {
    /* ignore import tag if it has no src and flag error */
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    cmml_import_element_destroy(m);
    return import_list;
  }
  /* check parsed start time for correctness */
  if (m->start_time->t.sec < 0.0) {
    cmml_set_error((CMML*)ctxt, CMML_TIME_ERROR);
    cmml_import_element_destroy(m);
    return import_list;
  }
  /* check parsed end time for correctness and delete it if not */
  if ((m->end_time != NULL) && 
      ((m->end_time->t.sec < 0.0) ||
       (m->end_time->t.sec < m->start_time->t.sec))) {
    cmml_set_error((CMML*)ctxt, CMML_FORMAT_ERROR);
    cmml_time_free (m->end_time);
  }
  import_list = cmml_list_append(import_list, m);
  return import_list;
}

/*
 * insert_param_tag: Make a new CMML_ImportElement and add it to the list
 * of CMML_ImportElements contained within the clip or head
 * CMML_Element.
 */
static CMML_List * 
insert_param_tag(CMML_List * import_list, const XML_Char **attr, 
		  int import_cnt, CMML_Context * ctxt)
{
  CMML_List * import_node;
  CMML_ParamElement * p;
  CMML_ImportElement *s;
  int i = 0;

  /* make sure there are import elements to attach the param to */
  if (!import_list || cmml_list_is_empty(import_list)) {
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    return import_list;
  }

  /* create new param_element and insert it */
  if((p = cmml_malloc(sizeof(*p))) == NULL) return import_list;
  p->id = NULL;
  p->value = NULL;
  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting param attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      p->id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "name") == 0) {
      p->name = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "value") == 0) {
      p->value = cmml_strdup(attr[i+1]);
    } 
  }
  /* check if required id and value attributes are there */
  if (!p->name || !p->value) {
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    cmml_free(p->id);
    cmml_free(p->name);
    cmml_free(p->value);
  } else {
    /* locate correct import_node in import_list to attach params to */
    for (i=0, import_node = import_list; 
	 (i<import_cnt) && (import_node!=NULL); 
	 i++, import_node = import_node->next);
    s = (CMML_ImportElement *) import_node->data;
    s->param = cmml_list_append(s->param, p);
  }
  return import_list;
}


/*
 * insert_head_attrs: insert attributes from a "head" element into a
 * CMML_HEAD.
 */
static void 
insert_head_attrs(CMML_Head * head, const XML_Char **attr)
{
  int i;

  if (!head) return;
  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting head attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      head->head_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      head->lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      head->dir = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "profile") == 0) {
      head->profile = cmml_strdup(attr[i+1]);
    }
  }
}

/*
 * insert_title_attrs: insert attributes from a "title" element into a
 * CMML_HEAD.
 */
static void 
insert_title_attrs(CMML_Head * head, const XML_Char **attr)
{
  int i;

  if (!head) return;
  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting title attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      head->title_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      head->title_lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      head->title_dir = cmml_strdup(attr[i+1]);
    }
  }
}

/*
 * insert_base_attrs: insert attributes from a "base" element into a
 * CMML_HEAD.
 */
static void 
insert_base_attrs(CMML_Head * head, const XML_Char **attr, CMML_Context *ctxt)
{
  int i;

  if (!head) return;
  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting base attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      head->base_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "href") == 0) {
      head->base_href = cmml_strdup(attr[i+1]);
    }
  }
  /* check if required href attribute is there */
  if (!head->base_href) {
    /* ignore base tag if it has no href */
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    cmml_free(head->base_id);
  }
}


/*
 * insert_meta_tag: Make a new CMML_MetaElement and add it to the list
 * of CMML_MetaElements contained within the clip or head
 * CMML_Element.
 */
static CMML_List * 
insert_meta_tag(CMML_List * meta_list, const XML_Char **attr, CMML_Context *ctxt)
{
  int i;
  CMML_MetaElement * m;

  if((m = cmml_malloc(sizeof(*m))) == NULL) return NULL;
  m->id = NULL;
  m->lang = NULL;
  m->dir = NULL;
  m->name = NULL;
  m->content = NULL;
  m->scheme = NULL;

  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting meta attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      m->id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      m->lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      m->dir = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "name") == 0) {
      m->name = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "content") == 0) {
      m->content = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "scheme") == 0) {
      m->scheme = cmml_strdup(attr[i+1]);
    }
  }
  /* check if required content attribute is there */
  if (!m->content) {
    /* just ignore meta tag if it has no content */
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    cmml_meta_element_destroy(m);
  } else {
    meta_list = cmml_list_append(meta_list, m);
  }
  return meta_list;
}

/*
 * insert_clip_attrs: insert attributes from an "a" element into a
 * CMML_Element of type CMML_CLIP.
 */
static int 
insert_clip_attrs(CMML_Clip * a, const XML_Char **attr, CMML_Context * ctxt)
{
  int i;

  if (!a) return -1;
  if (!ctxt->status->stream) {
    /* the document has no stream tag before this clip tag */
    ctxt->status->stream = cmml_stream_new();
    ctxt->status->stream->timebase = cmml_time_new("0");
  }
  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting clip attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      a->clip_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      a->lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      a->dir = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "track") == 0) {
      a->track = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "start") == 0) {
      a->start_time = 
	cmml_time_new_in_sec (attr[i+1], 
			      ctxt->status->stream->utc,
			      ctxt->status->stream->timebase->t.sec);
    } else if (cmml_strcmp(attr[i], "end") == 0) {
      a->end_time = 
	cmml_time_new_in_sec (attr[i+1],
			      ctxt->status->stream->utc,
			      ctxt->status->stream->timebase->t.sec);
    }
  }
  /* set default track  */
  if (a->track == NULL) {
    a->track = cmml_strdup("default");
  }
  /* check for required start attribute */
  if (!a->start_time || a->start_time->t.sec < 0) {
    cmml_set_error((CMML*)ctxt, CMML_TIME_ERROR);
    return -1;
  }
  /* check parsed end time for correctness and delete it if not */
  if ((a->end_time != NULL) && 
      ((a->end_time->t.sec < 0.0) ||
       (a->end_time->t.sec < a->start_time->t.sec))) {
    cmml_set_error((CMML*)ctxt, CMML_FORMAT_ERROR);
    cmml_time_free (a->end_time);
  }
  /* check if new clip comes after timebase and after previous clip */
  if (a->start_time->t.sec < ctxt->status->stream->timebase->t.sec ||
      (ctxt->status->clip && 
       (a->start_time->t.sec < ctxt->status->clip->start_time->t.sec))) {
      cmml_set_error((CMML*)ctxt, CMML_NONSEQUENTIAL_CLIP);
#ifdef DEBUG
      fprintf(stderr, 
	      "nonsequential: clip=%f(%s), timebase=%f, prev_clip=%f\n",
	      a->start_time->t.sec, a->start_time->tstr,
	      ctxt->status->stream->timebase->t.sec,
	      ctxt->status->prev_clip->start_time->t.sec);
#endif
      return -1;
  }
  return 0;
}

/* 
 * insert_anchor_attrs: add the anchor attribute and values to the
 * clip structure.
 */
static int 
insert_anchor_attrs(CMML_Clip * clip, const XML_Char **attr, CMML_Context *ctxt)
{
  int i;

  if (!clip) return -1;
  /* there can only be one anchor tag attached to clip */
  if (clip->anchor_href || clip->anchor_text) {
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    return -1;
  }

  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting anchor attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      clip->anchor_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      clip->anchor_lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      clip->anchor_dir = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "class") == 0) {
      clip->anchor_class = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "href") == 0) {
      clip->anchor_href = cmml_strdup(attr[i+1]);
    }
  }
  /* check for required href attribute */
  if (clip->anchor_href == NULL) {
    /* just ignore anchor tag if it has no href */
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    cmml_free(clip->anchor_id);
    cmml_free(clip->anchor_lang);
    cmml_free(clip->anchor_dir);
    cmml_free(clip->anchor_class);
    return -1;
  }
  return 0;
}

/* 
 * insert_img_attrs: add the img attribute and values to the clip
 * structure.
 */
static int
insert_img_attrs(CMML_Clip * clip, const XML_Char **attr, CMML_Context *ctxt)
{
  int i;

  if (!clip) return -1;
  /* there can only be one image tag attached to clip */
  if (clip->img_src) {
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    return -1;
  }

  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting img attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      clip->img_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      clip->img_lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      clip->img_dir = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "src") == 0) {
      clip->img_src = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "alt") == 0) {
      clip->img_alt = cmml_strdup(attr[i+1]);
    }
  }
  /* check for required src attribute */
  if (clip->img_src == NULL) {
    /* just ignore clip tag if it has no src */
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    cmml_free(clip->img_id);
    cmml_free(clip->img_lang);
    cmml_free(clip->img_dir);
    cmml_free(clip->img_alt);
    return -1;
  }
  return 0;
}

/* 
 * insert_desc_attrs: add the desc attribute and values to the clip
 * structure.
 */
static int
insert_desc_attrs(CMML_Clip * clip, const XML_Char **attr, CMML_Context* ctxt)
{
  int i;

  if (!clip) return -1;
  /* there can only be one desc tag attached to clip */
  if (clip->desc_text) {
    cmml_set_error((CMML*)ctxt, CMML_TAG_IGNORED);
    return -1;
  }
  
  for (i = 0; attr[i] != NULL; i += 2) {
#ifdef DEBUG
    fprintf(stderr, "Inserting desc attribute %s=%s\n", attr[i], attr[i+1]);
#endif
    if (cmml_strcmp(attr[i], "id") == 0) {
      clip->desc_id = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "lang") == 0) {
      clip->desc_lang = cmml_strdup(attr[i+1]);
    } else if (cmml_strcmp(attr[i], "dir") == 0) {
      clip->desc_dir = cmml_strdup(attr[i+1]);
    }
  }
  return 0;
}

/*
 * callback for expat to parse the <?xml processing instruction
 */
static void
cmml_xml_decl_parse (void *userData,
		     const XML_Char *version,
		     const XML_Char *encoding,
		     int             standalone)
{
  CMML_Context * context = (CMML_Context *) userData;
  /* parse and validate something like
     <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
  */
  context->status->info->xml_version  = cmml_strdup(version);
  context->status->info->xml_encoding = cmml_strdup(encoding);
  if (standalone < 0) {
    /* set to "yes" by default */
	context->status->info->xml_standalone = 1;
  } else {
	context->status->info->xml_standalone = standalone;
  }
}

/*
 * callback for expat to parse the DOCTYPE cmml declaration
 */
static void
cmml_doctype_parse (void *userData,
		    const XML_Char *doctypeName,
		    const XML_Char *sysid,
		    const XML_Char *pubid,
		    int            has_internal_subset)
{
  CMML_Context * context = (CMML_Context *) userData;
  /* parse and validate something like
     <!DOCTYPE cmml SYSTEM "cmml.dtd">
  */
  if (cmml_strcmp(doctypeName, "cmml") == 0 &&
      cmml_strcmp(sysid, "cmml.dtd") == 0) {
    /* found cmml doctype declaration -> store in status */
    /* XXX: anything else to be done? */
    context->status->info->doctype_declared = 1;
  }
}


/*
 * cmml_start: handler (called by Expat) when encountering a start tag.
 *
 * If the tag name is "stream", "head"  or "a", then make a new
 * CMML_Element, add attributes to it, and put it into the status struct.
 *
 * If the tag name is "param" then the CMML_Element at the tail of the
 * list can be import only.
 *
 * If the tag name is "title", "stream", or "base", then assert that
 * the currently processed CMML_Element is a head, and add
 * attributes to that head.
 *
 * If the tag name is "desc" then assert that the currently processed 
 * CMML_Element is an clip, and add attributes to that clip.
 *
 * If the tag name is "meta" then the CMML_Element at the tail of the
 * list can be either a head or an clip.
 *
 * The above arrangement takes advantage of the "shallowness" of the
 * CMML DTD, that is, whenever "title", "stream", "base", "desc",
 * "param" or "meta" is encountered, then its parent element is
 * exactly one level above it.
 */
static void 
cmml_start(void *cmml, const XML_Char *name, const XML_Char **attrs)
{
  CMML_Context * context = (CMML_Context*) cmml;
  CMML_Stream * stream;
  CMML_Head   * head;
  CMML_Clip * clip;

  /* if already in error state, just return */
  if (cmml_get_last_error(cmml) != NULL) return;
  /* ignore all start tags in the middle of a sloppy non-sequential clip */
  if (context->status->error.type == CMML_NONSEQUENTIAL_CLIP) {
    return;
  }

  /* ----- the "cmml" tag ----- */
  if (cmml_strcmp(name, "cmml") == 0) {
    /* has to be the very first tag */
    if (context->status->type != CMML_NONE) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      return;
    }
    context->status->type = CMML_CMML;
    insert_root_attrs(context->status->info, attrs, cmml);
    return;

  /* ----- the "stream" tag ---- */
  } else if (cmml_strcmp(name, "stream") == 0) {
    /* has to be first after cmml */
    if (context->status->type != CMML_CMML) {
      cmml_set_error(cmml, CMML_NO_CMML_TAG);
      context->status->type = CMML_STREAM;
      return;
    }
    context->status->type = CMML_STREAM;
    if (context->status->head || context->status->clip) {
      cmml_set_error(cmml, CMML_STREAM_NOT_FIRST);
      return;
    }
    /* there can only be one stream tag */
    if (context->status->stream != NULL) {
      cmml_set_error(cmml, CMML_DUPLICATE_STREAM);
      return;
    }
    /* parse stream attributes */
    stream = cmml_stream_new();
    if (insert_stream_attrs(stream, attrs, cmml) < 0) {
      cmml_stream_destroy(stream);
      return;
    }
    /* save stream tag */
    context->status->stream = stream;
    /* if in the middle of seeking for a time offset,
       and seektime is before timebase, stop seeking and report error */
    if (context->status->seek_type == _CMML_SEEK_TIME) {
      if (context->status->seek_secs <= 
	  context->status->stream->timebase->t.sec) {
	context->status->seek_secs = _CMML_SEEK_FOUND;
	context->status->seek_secs = -1.0;
      }
    }
    return;

  } else if (cmml_strcmp(name, "import") == 0) {
    if (context->status->type != CMML_STREAM || 
	context->status->stream == NULL) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      return;
    } else {
      context->status->type = CMML_IMPORT;
      context->status->stream->import = 
	insert_import_tag(context->status->stream->import, attrs, context);
      context->status->import_cnt++;
    }
    return;

  } else if (cmml_strcmp(name, "param") == 0) {
    if (context->status->type != CMML_IMPORT ||
	context->status->stream == NULL ||
	context->status->import_cnt < 0) {
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      return;
    } else {
      context->status->stream->import =
	insert_param_tag(context->status->stream->import, attrs, 
			 context->status->import_cnt, context);
    }
    return;

    
  /* ----- the "head" tag ----- */
  } else if (cmml_strcmp(name, "head") == 0) {
    /* has to be after cmml tag */
    if (context->status->type != CMML_CMML) {
      cmml_set_error(cmml, CMML_NO_CMML_TAG);
      context->status->type = CMML_HEAD;
      return;
    }
    /* if there has been no stream tag, fix it up */
    if (!context->status->stream) {
      context->status->stream = cmml_stream_new();
      context->status->stream->timebase = cmml_time_new("0");
    }
    context->status->type = CMML_HEAD;
    /* there musn't be a head tag after a clip tag */
    if (context->status->clip) {
      cmml_set_error(cmml, CMML_HEAD_AFTER_CLIP);
      return;
    }
    /* there can only be one head tag */
    if (context->status->head != NULL) {
      cmml_set_error(cmml, CMML_DUPLICATE_HEAD);
      return;
    }
    /* parse head tag attributes */
    head = cmml_head_new();
    insert_head_attrs(head, attrs);
    /* save head tag */
    context->status->head = head;
    return;

  } else if (cmml_strcmp(name, "title") == 0 ) {
    if (context->status->type != CMML_HEAD || context->status->head == NULL) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      XML_SetCharacterDataHandler(context->parser, NULL);
      return;
    } 
    /* there can only be one title tag */
    if (context->status->head->title != NULL) {
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      XML_SetCharacterDataHandler(context->parser, NULL);
      return;
    }
    /* start collecting PCDATA */
    insert_title_attrs(context->status->head, attrs);
    XML_SetCharacterDataHandler(context->parser, characters);
    return;

	
  } else if (cmml_strcmp(name, "base") == 0) {
    if (context->status->type != CMML_HEAD || context->status->head == NULL) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      return;
    }
    /* there can only be one base tag */
    if (context->status->head->base_href != NULL) {
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      return;
    }
    insert_base_attrs(context->status->head, attrs, cmml);
    return;

  } else if (cmml_strcmp(name, "meta") == 0) {
    if (context->status->type != CMML_HEAD &&
	context->status->type != CMML_CLIP) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      return;
    }
    if (context->status->type == CMML_HEAD) {
      if (context->status->head == NULL) {
	cmml_set_error(cmml, CMML_PARSE_ERROR);
	return;
      }
      context->status->head->meta =
	insert_meta_tag(context->status->head->meta, attrs, cmml);
    } else { /* CMML_CLIP */
      if (context->status->clip == NULL) {
	cmml_set_error(cmml, CMML_PARSE_ERROR);
	return;
      }
      context->status->clip->meta =
	insert_meta_tag(context->status->clip->meta, attrs, cmml);
    }
    return;

  /* ----- the "clip" tag ----- */
  } else if (cmml_strcmp(name, "clip") == 0) {
    /* has to be after cmml and head */
    if (context->status->type != CMML_CMML) {
      cmml_set_error(cmml, CMML_NO_CMML_TAG);
      context->status->type = CMML_CLIP;
      return;
    }
    context->status->type = CMML_CLIP;
    if (!context->status->head) {
      cmml_set_error(cmml, CMML_NO_HEAD_TAG);
      return;
    }
    /* try creating current clip */
    clip = cmml_clip_new(NULL, NULL);
    if (insert_clip_attrs(clip, attrs, cmml) < 0) {
      cmml_clip_destroy(clip);
      return;
    }
    /* copy context->status->clip to context->status->prev_clip */
    if (context->status->prev_clip != NULL) {
      cmml_clip_destroy(context->status->prev_clip);
    }
    context->status->prev_clip = context->status->clip;
    context->status->clip = clip;
    /* if seeking, check seek id or time and set result accordingly */
    if (context->status->seek_type == _CMML_SEEK_TIME) {
      if (context->status->seek_secs <= clip->start_time->t.sec) {
	context->status->seek_type = _CMML_SEEK_FOUND;
      }
    }
    if (context->status->seek_type == _CMML_SEEK_ID) {
      if (cmml_strcmp(context->status->seek_id, clip->clip_id) == 0) {
	context->status->seek_type = _CMML_SEEK_FOUND;
      }
    } 
    return;

  } else if (cmml_strcmp(name, "a") == 0) {
    if (context->status->type != CMML_CLIP || 
	context->status->clip == NULL) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      XML_SetCharacterDataHandler(context->parser, NULL);
      return;
    }
    if (insert_anchor_attrs(context->status->clip, attrs, cmml) == 0) {
      XML_SetCharacterDataHandler(context->parser, characters);
    }
    return;

  } else if (cmml_strcmp(name, "img") == 0) {
    if (context->status->type != CMML_CLIP || 
	context->status->clip == NULL) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      return;
    }
    insert_img_attrs(context->status->clip, attrs, cmml);
    return;

  } else if (cmml_strcmp(name, "desc") == 0) {
    if (context->status->type != CMML_CLIP || 
	context->status->clip == NULL) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
      XML_SetCharacterDataHandler(context->parser, NULL);
      return;
    }
    if (insert_desc_attrs(context->status->clip, attrs, cmml) == 0) {
      XML_SetCharacterDataHandler(context->parser, characters);
    }
    return;

  } else {
    cmml_set_error(cmml, CMML_UNKNOWN_TAG);
    return;
  }
}

/* 
 * clip_in_interval: helper function for checking whether the regarded
 * clip in the context is within the set time interval for the cmml stream
 */
static int
clip_in_interval(CMML_Context * ctxt) 
{
  int in_interval = 1; /* true by default */

  /* if no clip tag or clip's time is corrupted, return false */
  if (!ctxt->status->clip || !ctxt->status->clip ||
      ctxt->status->clip->start_time->t.sec < 0) {
    return 0;
  }

  /* check the start time */
  if (ctxt->status->t_start) {
    in_interval = 0;
    /* convert from utc to sec if necessary */
    if (ctxt->status->t_start->type == CMML_UTC_TIME) {
      ctxt->status->t_start = 
	cmml_time_utc_to_sec (ctxt->status->t_start,
			      ctxt->status->stream->utc,
			      ctxt->status->stream->timebase->t.sec);
      if (ctxt->status->t_start->t.sec < 0) {
	cmml_time_free(ctxt->status->t_start);
	ctxt->status->t_start = NULL;
	in_interval = 1;
      }
    }
    /* compare the time of the clip */
    if (ctxt->status->t_start &&
	ctxt->status->t_start->t.sec <= 
	ctxt->status->clip->start_time->t.sec) {
      in_interval = 1;
    }
  }

  /* check the end time */
  if (ctxt->status->t_end) {
    /* convert from utc to sec if necessary */
    if (ctxt->status->t_end->type == CMML_UTC_TIME) {
      ctxt->status->t_end = 
	cmml_time_utc_to_sec (ctxt->status->t_end,
			      ctxt->status->stream->utc,
			      ctxt->status->stream->timebase->t.sec);
      if (ctxt->status->t_end->t.sec < 0) {
	cmml_time_free(ctxt->status->t_end);
	ctxt->status->t_end = NULL;
      }
    }
    /* compare the time of the clip */
    if (ctxt->status->t_end) {
      if (ctxt->status->t_end->t.sec >=
	  ctxt->status->clip->start_time->t.sec) {
	in_interval = 1;
      } else {
	in_interval = 0;
      }
    }
  }
  return in_interval;
}

/* 
 * call_clip_callback: helper function for correcting an clip's start
 * and and end times if there is a time interval set for the stream
 * and then calling the user defined clip callback function
 */
static void
call_clip_callback(CMML_Context * ctxt, CMML_Clip * clip) 
{
  /* if clip's start_time before interval, set it to start */
  if (clip && ctxt->status->t_start &&
      clip->start_time->t.sec < ctxt->status->t_start->t.sec) {
    cmml_time_free (clip->start_time);
    clip->start_time = cmml_time_clone(ctxt->status->t_start);
  }

  /* if clip's end_time beyond interval -> drop it */
  if (clip && ctxt->status->t_end && clip->end_time &&
      clip->end_time->t.sec >= ctxt->status->t_end->t.sec) {
    cmml_time_free (clip->end_time);
    clip->end_time = NULL;
  }

  ctxt->read_clip ((CMML*)ctxt, clip, ctxt->user_data);
  return;
}

/*
 * cmml_end: handler (called by expat) when encountering an end tag.
 *
 * If the tag name is "title" or "desc" the PCDATA handling has
 * to be finalised and the characters stored in the respective fields.
 *
 * If the tag name is "stream", "head" or "a", then user callbacks
 * for these tags have to be activated.
 */
static void 
cmml_end(void *cmml, const XML_Char *name)
{
  CMML_Context * context = (CMML_Context*) cmml;
    
  /* if already in error state, just return */
  if (cmml_get_last_error(cmml) != NULL) return;

  /* ----- the "cmml" tag ----- */
  if (cmml_strcmp(name, "cmml") == 0) {
    if (context->status->type != CMML_CMML) {
      /* ignore cmml tag as there was no cmml start tag */
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      return;
    }
    /* end of document encountered */
    context->status->type = CMML_NONE;
    return;

  /* ----- the "stream" tag ----- */
  } else if (cmml_strcmp(name, "stream") == 0) {
    if (context->status->type != CMML_STREAM) {
      /* ignore stream tag as there was no stream start tag */
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      return;
    }
    /* disallow param tags and reset counter */
    context->status->import_cnt = -1;
    /* end of stream tag encountered -> throw user callback */
    if (context->read_stream != NULL) {
      context->read_stream ((CMML*)cmml, context->status->stream, context->user_data);
    }
    context->status->type = CMML_CMML;
    return;

  } else if (cmml_strcmp(name, "import") == 0) {
    if (context->status->type != CMML_IMPORT) {
      /* ignore import tag as there was no import start tag */
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      return;
    }
    /* end of import tag encountered; go back to stream */
    context->status->type = CMML_STREAM;
    return;

  } else if (cmml_strcmp(name, "param") == 0) {
    /* empty tag: no action */
    return;

  /* ----- the "head" tag ----- */
  } else if (cmml_strcmp(name, "head") == 0) {
    if (context->status->type != CMML_HEAD) {
      /* ignore it as there was no head start tag */
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      return;
    }
    /* end of head tag encountered -> throw user callback */
    if (context->read_head != NULL) {
      context->read_head ((CMML*)cmml, context->status->head, 
			  context->user_data);
    }
    context->status->type = CMML_CMML;
    return;

  } else if (cmml_strcmp(name, "title") == 0 ) {
    if (context->status->type != CMML_HEAD) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
    } else {
      context->status->head->title = cmml_strdup(context->cdata->base);
    }
    *(context->cdata->base) ='\0';
    XML_SetCharacterDataHandler(context->parser, NULL);
    return;
  
  } else if (cmml_strcmp(name, "base") == 0) {    
    /* empty tag: no action */
    return;
  } else if (cmml_strcmp(name, "meta") == 0) {
    /* empty tag: no action */
    return;

  /* ----- the "clip" tag ----- */
  } else if (cmml_strcmp(name, "clip") == 0) {
    /* ignore if this is a sloppy non-sequential clip */
    if (context->status->error.type == CMML_NONSEQUENTIAL_CLIP) {
      context->status->error.type = CMML_NONE;
      return;
    }
    if (context->status->type != CMML_CLIP) {
      /* ignore it as there was no clip start tag */
      cmml_set_error(cmml, CMML_TAG_IGNORED);
      return;
    }
    /* end of a tag encountered -> throw user callback */
    if (context->read_clip != NULL) {
      /* if already in error state, just call it */
      if (cmml_get_last_error(cmml) != NULL) {
	context->read_clip ((CMML*)cmml, NULL, context->user_data);
      }
      /* activate callback if clip is within set time interval */
      if (clip_in_interval(cmml)) {
	/* check if it's the first clip in the interval and then
	   also call the callback for the previous one */
	if (context->status->t_start &&
	    context->status->prev_clip &&
	    (context->status->t_start->t.sec > 
	     context->status->prev_clip->start_time->t.sec) &&
	    (context->status->t_start->t.sec <
	     context->status->prev_clip->start_time->t.sec)) {
	  call_clip_callback(cmml, context->status->prev_clip);
	}
	/* call this clip's callback */
	call_clip_callback(cmml, context->status->clip);
      }
    }
    context->status->type = CMML_CMML;
    return;

  } else if (cmml_strcmp(name, "a") == 0) {
    /* ignore if this is a sloppy non-sequential clip */
    if (context->status->error.type == CMML_NONSEQUENTIAL_CLIP) {
      return;
    }
    if (context->status->type != CMML_CLIP) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
    } else {
      context->status->clip->anchor_text = cmml_strdup(context->cdata->base);
    }
    *(context->cdata->base) ='\0';
    XML_SetCharacterDataHandler(context->parser, NULL);
    return;

  } else if (cmml_strcmp(name, "img") == 0) {    
    /* empty tag: no action */
    return;

  } else if (cmml_strcmp(name, "desc") == 0) {
    /* ignore if this is a sloppy non-sequential clip */
    if (context->status->error.type == CMML_NONSEQUENTIAL_CLIP) {
      return;
    }
    if (context->status->type != CMML_CLIP) {
      cmml_set_error(cmml, CMML_PARSE_ERROR);
    } else {
      context->status->clip->desc_text = cmml_strdup(context->cdata->base);
    }
    *(context->cdata->base) ='\0';
    XML_SetCharacterDataHandler(context->parser, NULL);
    return;
  }
}

/* 
 * cmml_context_setup: Set up the context and initialise it.  Set up
 * the start and end tag handlers.  Associate the context with a new
 * parser, and return the parser.
 */
CMML_Context * cmml_context_setup (FILE * file)
{
  /* allocate context */
  CMML_Context * context;
  context = cmml_malloc(sizeof(CMML_Context));

  /* fill context fields */
  context->file = file;
  /* param NULL = use character encoding of document encoding decl */
  context->parser = XML_ParserCreate(NULL); 
  XML_SetUserData(context->parser, context);
  context->sloppy_parsing = 0;
  context->cdata = cmml_malloc(sizeof(CMML_DataBuf));
  context->cdata->base = cmml_malloc(INIT_DATA_BUF_SIZE);
  *(context->cdata->base) = '\0';
  context->cdata->limit = INIT_DATA_BUF_SIZE;
  context->read_stream = NULL;
  context->read_head   = NULL;
  context->read_clip = NULL;
  context->user_data = NULL;
  context->status = cmml_malloc(sizeof(CMML_Status));
  context->status->info = cmml_malloc(sizeof(CMML_Preamble));
  context->status->info->xml_version      = NULL;
  context->status->info->xml_encoding     = NULL;
  context->status->info->xml_standalone   = -2;
  context->status->info->doctype_declared = 0;
  context->status->info->cmml_lang        = NULL;
  context->status->info->cmml_dir         = NULL;
  context->status->info->cmml_id          = NULL;
  context->status->info->cmml_xmlns       = NULL;
  context->status->type = CMML_NONE;
  context->status->stream = NULL;
  context->status->import_cnt = -1;
  context->status->head   = NULL;
  context->status->clip = NULL;
  context->status->prev_clip = NULL;
  context->status->error.type = CMML_OK;
  context->status->error.line = 0;
  context->status->error.col = 0;
  context->status->seek_type = _CMML_SEEK_NONE;
  context->status->seek_id = NULL;
  context->status->seek_secs = -1.0;
  context->status->t_start = NULL;
  context->status->t_end = NULL;

    
  /* setup callback for xml directive tag to check validity */
  XML_SetXmlDeclHandler(context->parser, cmml_xml_decl_parse);

  /* setup callback for doctype declaration */
  XML_SetStartDoctypeDeclHandler(context->parser, cmml_doctype_parse);
  
  /* setup parser for start/end tag handling to call the
     cmml_start and cmml_end function */
  XML_SetElementHandler(context->parser, cmml_start, cmml_end);

  return context;
}

/* 
 * cmml_context_destroy: Delete the context and free all the memory
 * that was allocated for it. Also free the parser itself.  Return the
 * file descriptor that the context referred to.
 */
FILE * cmml_context_destroy(CMML_Context *context)
{
  FILE * file = context->file;

  /* free memory areas */

  /* parser */
  XML_ParserFree(context->parser);

  /* status */
  cmml_preamble_destroy (context->status->info);
  cmml_stream_destroy (context->status->stream);
  cmml_head_destroy   (context->status->head);
  cmml_clip_destroy (context->status->clip);
  cmml_clip_destroy (context->status->prev_clip);
  cmml_free(context->status->seek_id);
  cmml_time_free(context->status->t_start);
  cmml_time_free(context->status->t_end);
  cmml_free(context->status);

  /* character data area */
  cmml_free(context->cdata->base);
  cmml_free(context->cdata);

  /* context itself */
  cmml_free(context);

  return file;
}


/*
 * cmml_set_error: set the error condition of the last operation.
 */
void 
cmml_set_error(CMML * cmml, CMML_Error_Type error)
{
  CMML_Context * ctxt = (CMML_Context*) cmml;
  ctxt->status->error.type = error;
  ctxt->status->error.line = XML_GetCurrentLineNumber(ctxt->parser);
  ctxt->status->error.col = XML_GetCurrentColumnNumber(ctxt->parser);
}


/*
 * emalloc: a safer malloc.  This sort of thing needs to be replaced,
 * however, with better memory-management code and error-reporting
 * functions.
 */
void *
emalloc(size_t size)
{
    void *p;
    
    p = malloc(size);
    if (!p) {
        fprintf(stderr, "libcmml: memory allocation error.");
        exit(1);
    }
    
    return p;
}

/*
 * cmml_snprintf implements the non-C99 functionality of returning -1 on
 * potential overrun.
 */
int
cmml_snprintf (char * str, size_t size, const char * format, ...)
{
  int retval;
  va_list ap;
  
  va_start (ap, format);
  retval = VSNPRINTF (str, size, format, ap);
  va_end(ap);

  if ((unsigned int) retval > size) retval = -1;
  return retval;
}

/*
 * escape_chars: a helper function to escape the characters &, <, >,
 * ', " from a string and return a pointer to a string with the
 * replacement codes &amp; &lt; &gt; &apos; &quot;
 * flag=0    - escape pcdata (" not included)
 * otherwise - escape attribute (includes ")
 */
static char *
escape_chars (char *instr, int flag)
{
  char * retstr;
  int len;
  int i,j;
  char c;

  /* calculate strlen of new string */
  len = (int) (strlen (instr));
  for (i=0,c=instr[0]; c!='\0'; c=instr[++i]) {
    switch (c) {
    case ('&'):
      len +=4;
      break;
    /* case ('\''): didin't seem to bother attribute or pcdata values */
    case ('"'):
      if (flag) len += 5;
      break;
    /* case ('>'): didn't seem to bother attribute or pcdata values */
    case ('<'):
      len += 3;
      break;
    }
  }

  /* copy string changing characters */
  retstr = cmml_malloc (len*sizeof(char)+1);
  j=0;
  for (i=0,c=instr[0]; c!='\0'; c=instr[++i]) {
    switch (c) {
    case ('&'):
      retstr[j++] = '&';
      retstr[j++] = 'a';
      retstr[j++] = 'm';
      retstr[j++] = 'p';
      retstr[j++] = ';';
      break;
    case ('"'):
      if (flag) {
	retstr[j++] = '&';
	retstr[j++] = 'q';
	retstr[j++] = 'u';
	retstr[j++] = 'o';
	retstr[j++] = 't';
	retstr[j++] = ';';
      } else {
	retstr[j++] = '"';
      }
      break;
/* didn't bother expat :)
    case ('\''):
      retstr[j++] = '&';
      retstr[j++] = 'a';
      retstr[j++] = 'p';
      retstr[j++] = 'o';
      retstr[j++] = 's';
      retstr[j++] = ';';
      break;
*/
    case ('<'):
      retstr[j++] = '&';
      retstr[j++] = 'l';
      retstr[j++] = 't';
      retstr[j++] = ';';
      break;
/* didin't bother expat :)
    case ('>'):
      retstr[j++] = '&';
      retstr[j++] = 'g';
      retstr[j++] = 't';
      retstr[j++] = ';';
      break;
*/
    default:
      retstr[j++] = instr[i];
    }
  }
  retstr[j]='\0';
#ifdef DEBUG
  fprintf(stderr, "orig str %s\nrest str %s\n", instr, retstr);
#endif
  return retstr;
}

int
cmml_snprint_attribute (char * buf, int n, char * attr_name, char * attr_value)
{
  int n_written;
  char * unesc;

  if (attr_name == NULL || attr_value == NULL) return 0;

  /* escape quotes, amp, lt, gt in attr_value */
  unesc = escape_chars(attr_value,1);

  n_written = cmml_snprintf (buf, n, " %s=\"%s\"", attr_name, unesc);
  cmml_free(unesc);

  return n_written;
}

int
cmml_snprint_pcdata (char * buf, int n, char * data)
{
  int n_written;
  char * unesc;

  if (data == NULL) return 0;

  /* escape &, <, > in data */
  unesc = escape_chars(data,0);

  n_written = cmml_snprintf (buf, n, "%s", unesc);
  cmml_free(unesc);

  return n_written;
}
