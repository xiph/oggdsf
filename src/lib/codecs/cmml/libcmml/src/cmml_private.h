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

/* cmml_private.h */

#ifndef __CMML_PRIVATE_H__
#define __CMML_PRIVATE_H__

#include <string.h>
#include <expat.h>
#include "config.h"

#ifdef WIN32
#include <io.h>
#define VSNPRINTF(a,b,c,d) _vsnprintf(a,b,c,d)
#define strdup(a) _strdup(a)
#else
#include <unistd.h>
#define VSNPRINTF(a,b,c,d) vsnprintf(a,b,c,d)
#endif

/* 
 * define the type CMML and 
 * indicate to cmml.h that is has been defined 
 */
typedef struct _CMML_Context CMML;
#define _CMML_PRIVATE
#include "cmml.h"

#define cmml_malloc emalloc
#define cmml_free(x) if(x) { free(x); x=0; }

/* -------------------------- */
/* stuff required for parsing */
/* -------------------------- */

/* initial size of base storage area in DataBuf */
#define INIT_DATA_BUF_SIZE 10000

#define cmml_strcmp(X, Y) strcmp((X), (Y))
#define cmml_strdup(s) ((s)==NULL?NULL:strdup(s))

/* -------------------------- */
/* stuff required for seeking */
/* -------------------------- */
typedef enum _CMML_SEEK_STATUS {
  _CMML_SEEK_NONE,
  _CMML_SEEK_TIME,
  _CMML_SEEK_ID,
  _CMML_SEEK_FOUND
} CMML_SEEK_STATUS;

/* ---------------------- TYPES --------------------------------*/

/*
 * CMML_Status: contains current parsing state information such as
 * - a copy of the current stream tag
 * - a copy of the current headtag
 * - a link to the current a tag and its start & end times
 * - an indication which was the last parsed element's type
 * - what's the last encountered parsing error
 * - whether there was a correctly parsed xml processing instruction
 * - whether there was a correctly parsed CMML DTD declaration
 * - when seeking, either the seconds or the id to seek for
 * Call the following functions to get (a copy of) the state info:
 * - cmml_get_last_stream
 * - cmml_get_last_head
 * - cmml_get_last_a
 * - cmml_get_last_error
 */
typedef struct {
  CMML_Preamble *info;
  CMML_Element_Type  type;
  CMML_Stream *stream;
  int          import_cnt;
  CMML_Head   *head;
  CMML_Clip   *clip;
  CMML_Clip   *prev_clip;
  CMML_Error   error;
  CMML_SEEK_STATUS seek_type;
  char  *seek_id;
  double seek_secs;
  CMML_Time * t_start;
  CMML_Time * t_end;
} CMML_Status;

/*
 * DataBuf: buffer for PCDATA (between start and end tags).
 */
typedef struct {
    XML_Char *base;   /* PCDATA array */
    int limit;        /* size of base field (initially INIT_DATA_BUF_SIZE), 
			 but may grow. */
} CMML_DataBuf;


/*
 * Context: the context data that gets passed around to event
 * handlers, etc.
 */
typedef struct {
  FILE * file;                /* currently-open CMML doc */
  XML_Parser parser;          /* the parser object */
  int sloppy_parsing;         /* sloppy parsing turned on/off */
  CMML_Status * status;       /* current parsing status */
  CMML_DataBuf * cdata;       /* holding area for character data (PCDATA) */
  CMMLReadStream read_stream; /* callback for stream tag */
  CMMLReadHead   read_head;   /* callback for head tag */
  CMMLReadClip read_clip; /* callback for clip tag */
  void * user_data;           /* user data for callback */
} CMML_Context;


/* ----------------------  API  --------------------------------*/

/* ------------------------------- *
 * functions to help setup parsing *
 * ------------------------------- */

/* set up the context and destroy it */
CMML_Context * cmml_context_setup(FILE * file);
FILE *         cmml_context_destroy(CMML_Context * context);
int            cmml_parse_to_head (CMML_Context * context);
CMML_ImportElement *
               cmml_import_element_destroy (CMML_ImportElement * import);
CMML_MetaElement *
               cmml_meta_element_destroy (CMML_MetaElement * meta);


/* set the encountered error in the context */
void cmml_set_error(CMML * cmml, CMML_Error_Type error);

/* -------------------------------------- *
 * helper functions for printing tags     *
 * -------------------------------------- */

int cmml_snprintf          (char * str, size_t size, const char * format, ...);
int cmml_snprint_attribute (char * buf, int n, char * attr_name, 
			    char * attr_value);
int cmml_snprint_pcdata    (char * buf, int n, char * data);

/* ------------------------ *
 * generic helper functions *
 * ------------------------ */
/*
 * emalloc: a safer malloc.  This sort of thing needs to be replaced,
 * however, with better memory-management code and error-reporting
 * functions.
 */
void *emalloc(size_t size);

#endif /* __CMML_PRIVATE_H__ */
