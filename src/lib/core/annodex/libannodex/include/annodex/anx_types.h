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

/** \file
 * Public structures and datatypes
 */

#ifndef __ANX_TYPES_H__
#define __ANX_TYPES_H__

#include <stdio.h>
#include <annodex/anx_list.h>

/* On GNU systems, anx_int64.h is autogenerated during configure. On
 * non-GNU Win32 systems, we instead include a static definition. */
#if defined(_WIN32) && !defined(__GNUC__)
#  include <annodex/anx_int64_w32.h>
#else
#  include <annodex/anx_int64.h>
#endif

#ifndef _ANX_PRIVATE
/**
 * An ANNODEX handle.
 */
typedef void ANNODEX;
#endif

/** Track */
typedef struct _AnxTrack AnxTrack;

/**
 * A track of data
 */
struct _AnxTrack {
  long serialno;          /*< The serialno of the Ogg logical bitstream */  
  char * id;              /*< The text id of the track */
  char * content_type;    /*< Content type of track contents */
  long nr_header_packets; /*< number of extra header packets */
  anx_int64_t granule_rate_n;  /*< the granule rate numerator */
  anx_int64_t granule_rate_d;  /*< the granule rate denominator */
};


/* Annotation elements */
typedef struct _AnxHead AnxHead;
typedef struct _AnxClip AnxClip;
typedef struct _AnxMetaElement AnxMetaElement;

struct _AnxMetaElement {
  char *id;      /**< id attribute of meta element */
  char *lang;    /**< language code of meta element */
  char *dir;     /**< directionality of lang (ltr/rtl) */
  char *name;    /**< property name of meta element */
  char *content; /**< property value of meta element */
  char *scheme;  /**< scheme name of meta element */
};

struct _AnxHead {
  char *head_id;    /**< id attribute of header */
  char *lang;       /**< language of header */
  char *dir;        /**< directionality of lang */
  char *profile;    /**< profile of header */
  char *title;      /**< title element of header */
  char *title_id;   /**< id attribute of title element */
  char *title_lang; /**< language of title */
  char *title_dir;  /**< directionality of title_lang */
  char *base_id;    /**< id attribute of base element */
  char *base_href;  /**< href attribute of base element */
  AnxList * meta;   /**< list of meta elements */
};

struct _AnxClip {
  char *clip_id;         /**< id attribute of clip */
  char *lang;            /**< language attribute of desc element */
  char *dir;             /**< directionality of lang */
  char *track;           /**< track attribute of clip */
  char *anchor_id;       /**< id attribute of anchor */
  char *anchor_lang;     /**< language of anchor */
  char *anchor_dir;      /**< directionality of lang */
  char *anchor_class;    /**< stylesheet of anchor */
  char *anchor_href;     /**< href out of clip */
  char *anchor_text;     /**< anchor text */
  char *img_id;          /**< id attribute of image */
  char *img_lang;        /**< language of img */
  char *img_dir;         /**< directionality of lang */
  char *img_src;         /**< keyframe image of clip */
  char *img_alt;         /**< alternate text for image */
  char *desc_id;         /**< id attribute of desc element */
  char *desc_lang;       /**< language attribute of desc element */
  char *desc_dir;        /**< directionality of lang */
  char *desc_text;       /**< the description itself */
  AnxList * meta;      /**< list of meta elements */
};

/* Importer Callbacks */

typedef int (*AnxImportStream) (double timebase, char * utc, void * user_data);
typedef int (*AnxImportHead) (AnxHead * head, void * user_data);
typedef int (*AnxImportClip) (AnxClip * clip, double time_offset,
				void * user_data);

typedef int (*AnxImportImport) (double start_time, char * filename,
				char * id, char * content_type,
				double seek_offset, double seek_end,
				void * user_data);

typedef struct _AnxImportCallbacks AnxImportCallbacks;

struct _AnxImportCallbacks {
  AnxImportStream import_stream;
  AnxImportHead import_head;
  AnxImportClip import_clip;
  void * import_user_data;
  AnxImportImport import_import;
  void * import_import_user_data;
};

#endif /* __ANX_TYPES_H__ */