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

/* cmml.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <expat.h>


/* include files for cmml only */
#include "cmml_private.h"
#include "cmml.h"

/* for debugging purposes
#define DEBUG
*/


#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/* --- CMML parser handling --- */

/*
 * cmml_open: Takes a filename and opens it as read-only. Then checks
 * whether it is an XML file satisfying the CMML specification with
 * the xml directive and the cmml tag. It returns a CMML object.
 * If you're working on an already opened file, use cmml_open_stdio
 * instead.
 */
CMML * 
cmml_open (char *XMLfilename)
{
  FILE * file;
  CMML_Context * context;

  /* open file */
  file = fopen (XMLfilename, "r");

  if (file == NULL) {
    return NULL;
  }

  /* set up context and register the callbacks for expat to parse 
     any xml directive and dtd declarations and 
     any opening and closing tags*/
  context = cmml_context_setup(file);

  /* return context as CMML */
  return (CMML *) context;

}
 

/*
 * cmml_new: Takes FILE pointer to an already-opened CMML
 * document.  Returns a CMML object containing internal initialised
 * data structures, etc, which is used in subsequent operations on the
 * CMML file.
 */
CMML * 
cmml_new (FILE * file)
{
  CMML_Context * context;

  /* set up context and register the callbacks for expat to parse 
     xml directives and dtd declarations if exists and 
     any opening and closing tags*/
#ifdef WIN32
  /* stupid Windows hack: has different memory addresses for
     stdin within dynamically loaded library to application */
  if (_fileno(file)==0) {
	  context = cmml_context_setup(stdin);
  } else {
	  context = cmml_context_setup(file);
  }
#else
  context = cmml_context_setup(file);
#endif

  /* return context as CMML */
  return (CMML *) context;
}

/*
 * cmml_destroy: Close a CMML object. Return the FILE pointer from
 * which the CMML object was derived.
 *
 * Simply free all the memory that was allocated for the context, and
 * also free the parser itself. 
 */
FILE *
cmml_destroy (CMML *cmml)
{
  return cmml_context_destroy((CMML_Context *)cmml);
}

/*
 * cmml_close: Close a CMML object.
 *
 * Simply free all the memory that was allocated for the context, and
 * also free the parser itself.  Return the CMML object if something
 * goes wrong (i.e. system file close error)
 */
CMML *
cmml_close(CMML *cmml)
{
  CMML_Context *ctxt = (CMML_Context *) cmml;
  FILE * tmp;
  if (fclose(ctxt->file) != 0) {
    return cmml;
  }

  tmp=cmml_destroy(cmml);
  return NULL;
}


/*
 * cmml_set_read_callbacks: registration function for callbacks.
 */
int cmml_set_read_callbacks (CMML *cmml,
			     CMMLReadStream read_stream,
			     CMMLReadHead   read_head,
			     CMMLReadClip read_clip,
			     void * user_data)
{
  CMML_Context * context = (CMML_Context *) cmml;
  context->read_stream = read_stream;
  context->read_head   = read_head;
  context->read_clip = read_clip;
  context->user_data   = user_data;
  return 0;
}


/*
 * cmml_read: read n bytes from a file and parse them.
 * Activates registered callbacks when reaching stream, head or clip
 * end tags and blocks until they have completed. Stores parsed info
 * in status. If an error or EOF occurs, returns -1 in which case you
 * can check the error code with cmml_last_error().
 */
long 
cmml_read (CMML *cmml, long n)
{
  CMML_Context * ctxt = (CMML_Context*) cmml;
#define READ_BUF_SIZE      4096
  XML_Char buf[READ_BUF_SIZE]; /* buffer to read the CMML doc into */
  long bytes, remaining = n, nread = 0;
  
  while (remaining > 0) {
    if ((bytes = (long)fread(buf, 1, MIN(remaining, READ_BUF_SIZE),
		       ctxt->file)) == -1) {
      cmml_set_error(cmml, CMML_READ_ERROR);
      return -1;
    }
    
    remaining -= bytes;
    nread += bytes;
    
    /* XML_Parse activates registered expat callbacks, which in turn
       activate the registered libcmml user callbacks when the end
       of a stream,head or clip tag are reached */
    if (XML_Parse(ctxt->parser, buf, bytes, bytes == 0) == XML_STATUS_ERROR) {
      if (cmml_get_last_error(cmml) != NULL) return -1;
      cmml_set_error(cmml, CMML_EXPAT_ERROR);
      return -1;
    }

    if (bytes == 0) {
      if (cmml_get_last_error(cmml) == NULL) {
	cmml_set_error(cmml, CMML_EOF);
      }
      return nread;
    }
  }

#undef READ_BUF_SIZE

  return nread;
}

/*
 * cmml_set_sloppy: sets the sloppy switch for error handling
 */        
void cmml_set_sloppy (CMML *cmml, int value)
{
  CMML_Context * context = (CMML_Context *) cmml;
  context->sloppy_parsing = value;
}

/*
 * cmml_get_preamble: returns a structure containing some of the
 * parsing information received out of a stream.
 */
CMML_Preamble * 
cmml_get_preamble (CMML *cmml)
{
  CMML_Context * context = (CMML_Context *) cmml;
  CMML_Preamble * i;

  /* if cmml document has not been parsed to cmml tag, fix it */
  if (!context || !context->status || !context->status->info) return NULL;

  /* parse to head if cmml tag not yet parsed */
  if (!context->status->info->cmml_xmlns) {
    if (cmml_parse_to_head(context)!=0) return NULL;
  }

  i = cmml_preamble_new(context->status->info->xml_encoding,
	  context->status->info->cmml_id,
	  context->status->info->cmml_lang,
	  context->status->info->cmml_dir);
  if (context->status->info->xml_version != NULL) {
	  i->xml_version    = context->status->info->xml_version;
  }
  i->xml_standalone = context->status->info->xml_standalone;
  i->doctype_declared = context->status->info->doctype_declared;
  return i;
}

/*
 * cmml_get_last_{stream,head,clip}: Return a copy of the current
 * stream, head, last a element.
 */
CMML_Stream * 
cmml_get_last_stream (CMML *cmml)
{
  CMML_Context * context = (CMML_Context *) cmml;
  CMML_Stream * m;

  m = cmml_stream_clone (context->status->stream);
  return m;
}

CMML_Head * cmml_get_last_head (CMML *cmml)
{
  CMML_Context * context = (CMML_Context *) cmml;
  CMML_Head * h;

  h = cmml_head_clone (context->status->head);
  return h;
}

CMML_Clip * 
cmml_get_last_clip (CMML *cmml)
{
  CMML_Context * context = (CMML_Context*) cmml;
  CMML_Clip * a;

  a = cmml_clip_clone (context->status->clip);
  return a;
}

CMML_Clip * 
cmml_get_previous_clip (CMML *cmml)
{
  CMML_Context * context = (CMML_Context *) cmml;
  CMML_Clip * a;

  a = cmml_clip_clone (context->status->prev_clip);
  return a;
}

/*
 * cmml_get_last_error: Return the error status of the most recently
 * executed cmml command.
 */
CMML_Error * 
cmml_get_last_error (CMML * cmml)
{
  CMML_Context * context = (CMML_Context*) cmml;
  /* if sloppy_parsing is switched on, ignore sloppy errors */
  if (context->status->error.type >= CMML_FORMAT_ERROR 
      && context->sloppy_parsing) {
    return NULL;
  }
  if (context->status->error.type == CMML_OK) {
    return NULL;
  } else {
    return &((context->status)->error);
  }
}

void
cmml_error_clear (CMML * cmml)
{
  cmml_set_error (cmml, CMML_OK);
}

int
cmml_set_window (CMML *cmml, CMML_Time *start, CMML_Time *end)
{
  CMML_Context * ctxt = (CMML_Context*) cmml;
  int no_times = 0;

  /* check times and set them in status */
  if (start) {
    if (ctxt->status->t_start) cmml_time_free(ctxt->status->t_start);
    ctxt->status->t_start = cmml_time_clone(start);
    no_times++;
  }
  if (end) {
    if (ctxt->status->t_end) cmml_time_free(ctxt->status->t_end);
    ctxt->status->t_end = cmml_time_clone(end);
    no_times++;
  }
  return no_times;
}


/* 
 * cmml_skip_to_secs: seek forward thru clip tags to
 * required time offset (no backwards seeking: streaming!).
 * Seeking presumes the clips are ordered in time.
 * Clips out of band are ignored.
 * return clip time found or -1 on error
 */
double 
cmml_skip_to_secs (CMML *cmml, double seconds)
{
  CMML_Context * ctxt = (CMML_Context*) cmml;

#define READ_BUF_SIZE      64
  XML_Char buf[READ_BUF_SIZE]; /* buffer to read the CMML doc into */
  long bytes, nread = 0;

  if (seconds < 0.0) {
    return -1.0;
  }

  /* make sure to have the stream tag parsed to check about position */
  if (cmml_parse_to_head (ctxt)!=0) {
    return -1.0;
  }

  /* if seek time is in past, return error */
  if (ctxt->status->stream != NULL && ctxt->status->stream->timebase != NULL) {
    if (seconds <  ctxt->status->stream->timebase->t.sec) {
      /* stream starts beyond that time */
      return -1;
    }
    if (ctxt->status->clip && 
	seconds < ctxt->status->clip->start_time->t.sec) {
      /* already past that time */
      return 0;
    }
  }

  /* start seeking */
  ctxt->status->seek_type = _CMML_SEEK_TIME;
  ctxt->status->seek_secs = seconds;
  
#ifdef DEBUG
  fprintf(stderr, "seeking to %f seconds\n", ctxt->status->seek_secs);
#endif
  
  /* go through clip tags seeking correct position */
  /* The first clip tag that has a larger start time will return a
     _CMML_SEEK_FOUND and stop the parsing; otherwise it parses
     through to the end */
  while (ctxt->status->seek_type != _CMML_SEEK_FOUND) {
    if ((bytes = (long)fread(buf, 1, READ_BUF_SIZE, ctxt->file)) == -1) {
      cmml_set_error(cmml, CMML_READ_ERROR);
      goto return_error;
    }
    nread += bytes;
    
    /* XML_Parse activates registered expat callbacks, where on parsing
       the correct clip_id, the seek flag gets changed */
    if (XML_Parse(ctxt->parser, buf, bytes, bytes == 0) == XML_STATUS_ERROR) {
      if (cmml_get_last_error(cmml) != NULL) return -1;
      cmml_set_error(cmml, CMML_EXPAT_ERROR);
      goto return_error;
    }
    
    /* end of file reached */
    if (bytes == 0) {
      if (cmml_get_last_error(cmml) == NULL) {
	cmml_set_error(cmml, CMML_EOF);
      }
      goto return_error;
    }
  }
#undef READ_BUF_SIZE
  if (ctxt->status->seek_type == _CMML_SEEK_FOUND && 
      ctxt->status->seek_secs < 0.0) {
    ctxt->status->seek_type = _CMML_SEEK_NONE;
    return ctxt->status->stream->timebase->t.sec;
  }
  
  ctxt->status->seek_type = _CMML_SEEK_NONE;
  ctxt->status->seek_secs = -1;
  return ctxt->status->clip->start_time->t.sec;
  
 return_error:
  ctxt->status->seek_type = _CMML_SEEK_NONE;
  ctxt->status->seek_secs = -1;
  return -1;
}

/*
 * cmml_skip_to_utc: seek for a utc time offset
 * return clip time found or -1 on error
 */
double cmml_skip_to_utc (CMML *cmml, const char *utcstr)
{
  CMML_Context * ctxt = (CMML_Context*) cmml;
  CMML_UTC * utc;
  double difference;
#ifdef DEBUG
  char bufutc[100];
#endif

  utc = cmml_utc_parse(utcstr);
  if (utc == NULL) {
    fprintf(stderr, "Error parsing utc attribute.\n");
    cmml_set_error(cmml, CMML_TIME_ERROR);
    return -1.0;
  }
#ifdef DEBUG
  cmml_utc_pretty_snprint (bufutc, 100, utc);
  fprintf(stderr, "Seeking %s\n", bufutc);
#endif

  /* if necessary, parse stream tag to get utc timebase */
  if (cmml_parse_to_head(ctxt)!=0) {
    return -1;
  }

  /* if utc does not exist in stream, seeking not possible */
  if (ctxt->status->stream->utc == NULL) {
    fprintf(stderr, "No utc attribute in stream tag -> seeking by utc not possible.\n");
    cmml_set_error(cmml, CMML_TIME_ERROR);
    return -1.0;
  }

  difference = cmml_utc_diff(utc, ctxt->status->stream->utc->t.utc);
#ifdef DEBUG
  fprintf(stderr, "difference between: %s and %s\n", utcstr, ctxt->status->stream->utc->tstr);
  fprintf(stderr, "time difference seeking: %f and offset %f\n", difference,
	  ctxt->status->stream->timebase->t.sec);
#endif
  if (difference < 0) {
    /* utc time is before start of file's utc time */
    return -1.0;
  }
  /* calculate corresponding offset in seconds into the file */
  difference += ctxt->status->stream->timebase->t.sec;
#ifdef DEBUG
  fprintf(stderr, "time difference seeking: %f\n", difference);
#endif

  return cmml_skip_to_secs (cmml, difference);
}

/* 
 * cmml_skip_to_id: seek forward thru clip tags to
 * required id offset (no backwards seeking: streaming!).
 * return clip time found or -1 on error
 */
double cmml_skip_to_id (CMML *cmml, const char * id)
{
  CMML_Context * ctxt = (CMML_Context*) cmml;
#define READ_BUF_SIZE      64
  XML_Char buf[READ_BUF_SIZE]; /* buffer to read the CMML doc into */
  long bytes, nread = 0;

  /* check if last read clip already matches */
  if (ctxt->status->clip &&
      cmml_strcmp(id, ctxt->status->clip->clip_id) == 0) {
    return ctxt->status->clip->start_time->t.sec;
  }
 
  /* set seek flag */
  ctxt->status->seek_type = _CMML_SEEK_ID;
  ctxt->status->seek_id = cmml_strdup(id);

  while (ctxt->status->seek_type != _CMML_SEEK_FOUND) {
    if ((bytes = (long)fread(buf, 1, READ_BUF_SIZE, ctxt->file)) == -1) {
	  cmml_set_error(cmml, CMML_READ_ERROR);
      goto return_error;
    }
    nread += bytes;

    /* XML_Parse activates registered expat callbacks, where on parsing
       the correct clip_id, the seek flag gets changed */
    if (XML_Parse(ctxt->parser, buf, bytes, bytes == 0) == XML_STATUS_ERROR) {
      if (cmml_get_last_error(cmml) != NULL) return -1;
      cmml_set_error(cmml, CMML_EXPAT_ERROR);
      goto return_error;
    }

    /* end of file reached */
    if (bytes == 0) {
      if (cmml_get_last_error(cmml) == NULL) {
	cmml_set_error(cmml, CMML_EOF);
      }
      goto return_error;
    }
  }
#undef READ_BUF_SIZE

  ctxt->status->seek_type = _CMML_SEEK_NONE;
  cmml_free(ctxt->status->seek_id);
  return ctxt->status->clip->start_time->t.sec;

 return_error:
  ctxt->status->seek_type = _CMML_SEEK_NONE;
  cmml_free(ctxt->status->seek_id);
  return -1;
}

/* 
 * cmml_skip_to_offset: seek forward thru clip tags to
 * required fragment offset (no backwards seeking: streaming!).
 * return clip time found or -1 on error
 */
double cmml_skip_to_offset (CMML *cmml, const char * offset)
{
  CMML_Time * secspec;
  char *retval;

  if ((retval = strstr(offset,":")) == NULL) {
    /* no colon found -> is an clip id name */
    return cmml_skip_to_id (cmml, offset);
  } else if ((strncmp(offset,"clock",5))==0) {
    /* clock keyword found -> is a utc time */
    return cmml_skip_to_utc(cmml, offset+6);
  } else {
    /* some other time specification */
    secspec = cmml_time_new(offset);
    return cmml_skip_to_secs(cmml, secspec->t.sec);
  }
  return -1.0;

}
