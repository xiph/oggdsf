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

/* cmml.h */

#ifndef __CMML_H__
#define __CMML_H__

/**
 * \file 
 *
 * cmml.h includes definitions of the public API and data types
 * required to parse CMML 2.0 files.
 */

#include <stdio.h>
#include <expat.h>

/**
 * A CMML handle.
 */
#ifndef _CMML_PRIVATE
typedef void CMML;
#endif

/* ------------------------- TYPES --------------------------------- */
/* CMML_Time and CMML_List are kept here and not in cmml_private.h   */
/* because the library user may need detailed access to these fields */

/** 
 * CMML_UTC: utc time specification struct similar to the one defined
 * in time.h
 */
typedef struct {
  short   tm_hsec; /**< hundreds of seconds [0-99] */
  short   tm_sec;  /**< seconds after the minute [0-59] */
  short   tm_min;  /**< minutes after the hour [0-59] */
  short   tm_hour; /**< hours since midnight [0-23] */
  short   tm_mday; /**< day of the month [1-31] */
  short   tm_mon;  /**< months since January [1-12] */
  short   tm_year; /**< years including century */

} CMML_UTC;

/** 
 * CMML_Time_Type: enumerates the different time types
 */
typedef enum _CMML_Time_Type {
  CMML_SEC_TIME,   /**< time is seconds */
  CMML_UTC_TIME    /**< time is utc */
} CMML_Time_Type;


/**
 * CMML_Time: a time specification in CMML: either seconds or utc
 * time; keeps the orininal time string for reuse, too
 */
typedef struct {
  char *tstr;          /**< copy of original time string for reuse in printing */
  CMML_Time_Type type; /**< the type of time for selecting the union content */
  /** holds either a utc or a sec time */
  union {
    CMML_UTC * utc;    /**< actual time in utc */
    double sec;        /**< actual time in seconds */
  } t;
} CMML_Time ;


/**
 * CMML_List: A doubly linked list
 */
typedef struct _CMML_List CMML_List;
/**
 * USE THE DEFINED TYPE CMML_List FOR THIS!
 *
 * CMML_List: A doubly linked list
 */
struct _CMML_List {
  CMML_List * prev; /**< previous list member */
  CMML_List * next; /**< next list member */
  void * data;      /**< actual member content */
};

/**
 * CMML_CloneFunc: Signature of a cloning function for CMML_List.
 */
typedef void * (*CMML_CloneFunc) (void * data);

/**
 * CMML_FreeFunc: Signature of a freeing function.
 */
typedef void * (*CMML_FreeFunc) (void * data);

/**
 * CMML_CmpFunc: Signature of a comparison function for CMML_List
 * to compares two scalars, returning:
 *  +ve, s1 > s2
 *    0, s1 == s2
 *  -ve, s1 < s2
 */
typedef int (*CMML_CmpFunc) (void * cmp_ctx, void * s1, void * s2);


/**
 * CMML_Preamble: a struct to keep the general details of
 * a CMML document available
 *
 */
typedef struct {
  char *xml_version;      /**< version attribute of xml proc instr */
  char *xml_encoding;     /**< encoding attribute of xml proc instr */
  int   xml_standalone;   /**< standalone attribute of xml proc instr */
  int   doctype_declared; /**< was doctype declared */
  char *cmml_lang;        /**< lang attribute of cmml tag */
  char *cmml_dir;         /**< dir attribute of cmml tag */
  char *cmml_id;          /**< id attribute of cmml tag */
  char *cmml_xmlns;       /**< xmlns attribute of cmml tag */
} CMML_Preamble;


/**
 * CMML_ParamElement: a struct to keep the details of the param tag in
 * a import element
 */
typedef struct {
  char *id;    /**< id of param element */
  char *name;  /**< property name of param element */
  char *value; /**< property value of param element */
} CMML_ParamElement;

/**
 * CMML_ImportElement: a struct to keep the details of the import tag
 */
typedef struct {
  char *id;               /**< id of import tag */
  char *lang;             /**< language code of import tag */
  char *dir;              /**< directionality of lang (ltr/rtl) */
  char *granulerate;      /**< base temporal resolution in Hz */
  char *contenttype;      /**< content type of the import bitstream */
  char *src;              /**< URI to import document */
  CMML_Time * start_time; /**< insertion time in annodex bitstream */
  CMML_Time * end_time;   /**< end time of this logical  bitstream */
  char *title;            /**< comment on the import bitstream */
  CMML_List * param;      /**< list of optional further nam-value
			     metadata for the import bitstreams */
} CMML_ImportElement;

/** 
 * CMML_Stream: an optional stream element containing information on
 * the set of import bitstreams which are being annotated by the CMML
 * data.
 * 
 */
typedef struct {
  char *id;             /**< id attribute of stream element */
  CMML_Time * timebase; /**< timebase attribute of stream element */
  CMML_Time * utc;      /**< utc attribute of stream element */
  CMML_List * import;   /**< list of import elements */
} CMML_Stream;


/**
 * CMML_MetaElement: a struct to keep the details of the meta tag in
 * either a head or an clip
 */
typedef struct {
  char *id;      /**< id attribute of meta element */
  char *lang;    /**< language code of meta element */
  char *dir;     /**< directionality of lang (ltr/rtl) */
  char *name;    /**< property name of meta element */
  char *content; /**< property value of meta element */
  char *scheme;  /**< scheme name of meta element */
} CMML_MetaElement;

/**
 * CMML_Head: a head element as presented in the CMML document.
 */
typedef struct {
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
  CMML_List * meta; /**< list of meta elements */
} CMML_Head;


/**
 * CMML_Clip: an clip element as presented in the CMML document.
 */
typedef struct {
  char *clip_id;         /**< id attribute of clip */
  char *lang;            /**< language attribute of desc element */
  char *dir;             /**< directionality of lang */
  char *track;           /**< track attribute of clip */
  CMML_Time *start_time; /**< start time of this clip */
  CMML_Time *end_time;   /**< end time of this clip */
  CMML_List * meta;      /**< list of meta elements */
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
} CMML_Clip;

/**
 * CMML_Element_Type: indication of what the CMML_Element actually
 * represents.
 */
typedef enum _CMML_Element_Type {
  CMML_NONE,   /**< no element */
  CMML_CMML,   /**< cmml element */
  CMML_STREAM, /**< stream element */
  CMML_IMPORT, /**< import element */
  CMML_HEAD,   /**< head element */
  CMML_CLIP    /**< clip element */
} CMML_Element_Type;


/**
 * CMML_Element: wrapper type for any of the three elements which can
 * occur in a CMML document (and are stored in CMML_Status).  Memory
 * freed by cmml_destroy_element()
 */
typedef struct {
  CMML_Element_Type type; /**< type of element to select in union */
  /** holds either the stream, head or clip tag */
  union {
    CMML_Stream * stream; /**< the stream tag */
    CMML_Head   * head;   /**< the head tag */
    CMML_Clip   * clip;   /**< the clip tag */
  } e;
} CMML_Element;


/**
 * CMML_Error_Type: indication of the type of error that occured.
 * There are three classes of errors:
 * - general processing feedback (CMML_OK, CMML_EOF)
 * - fatal processing feedback (these errors render the CMML file useless)
 * - tolerable processing errors (these lead to ignoring certain tags)
 * The user may select to 
 */
typedef enum _CMML_Error_Type {
  /* general processing feedback */
  CMML_OK,           /**< everything went fine and EOF was not encountered */
  CMML_EOF,          /**< EOF, but everything went fine */
  /* fatal errors */
  CMML_READ_ERROR,   /**< Reading the raw CMML doc into a buffer */
  CMML_TIME_ERROR,   /**< Error in formatting of time in a required tag */
  CMML_MALLOC_ERROR, /**< Memory allocation error */
  CMML_EXPAT_ERROR,  /**< Expat reported an error */
  CMML_PARSE_ERROR,  /**< General parsing error */
  CMML_NO_CMML_TAG,  /**< No cmml tag was found */
  CMML_NO_HEAD_TAG,  /**< No head tag was found */
  CMML_STREAM_NOT_FIRST,  /**< there is a head or a tag before the stream */
  CMML_HEAD_AFTER_CLIP,   /**< there is a clip tag before the head tag */
  CMML_DUPLICATE_STREAM,  /**< there are two stream tags */
  CMML_DUPLICATE_HEAD,    /**< there are two head tags */
  /* sloppy errors */
  CMML_FORMAT_ERROR,      /**< error in the formatting of an implied attribute */
  CMML_UNKNOWN_TAG,       /**< skipping an unknown tag */
  CMML_TAG_IGNORED,       /**< skipping a faulty tag */
  CMML_XMLNS_REDEFINED,   /**< xmlns in cmml tag is wrongly redefined */
  CMML_NONSEQUENTIAL_CLIP /**< clip with start time in the "past" skipped */
} CMML_Error_Type;

/**
 * CMML_Error: contains the error type plus the line and col numbers
 * where they occured.
 */
typedef struct {
  CMML_Error_Type type; /**< holds the error code */
  long line;            /**< line in which error occured */
  long col;             /**< column in which error occured */
} CMML_Error;

/**
 * CMMLReadStream: Signature for a callback called when the CMML
 * stream element is parsed
 *
 * \param cmml the CMML* handle in use
 * \param stream the stream element's content represented in a
 *        CMML_Stream*
 * \param user_data user defined data
 *
 * \returns 0 on success, 1 on error
 */
typedef int (*CMMLReadStream)  (CMML *cmml, const CMML_Stream *stream,
			       void * user_data);
/**
 * CMMLReadHead: Signature for a callback called when the CMML head
 * element is parsed
 *
 * \param cmml the CMML* handle in use
 * \param head the head element's content represented in a
 *        CMML_Head*
 * \param user_data user defined data
 *
 * \returns 0 on success, 1 on error
 */
typedef int (*CMMLReadHead)   (CMML *cmml, const CMML_Head *head,
			       void * user_data);

/**
 * CMMLReadClip: Signature for a callback called when a CMML clip
 * element is parsed
 *
 * \param cmml the CMML* handle in use
 * \param clip the clip element's content represented in a
 *        CMML_Clip*
 * \param user_data user defined data
 *
 * \returns 0 on success, 1 on error
 */
typedef int (*CMMLReadClip) (CMML *cmml, const CMML_Clip *clip,
			       void * user_data);


/* ----------------------  API  --------------------------------*/

/* --------------------------------- *
 * CMML parsing: functions in cmml.c *
 * --------------------------------- */

/**
 * cmml_open: Takes a filename and opens it as read-only. Then checks
 * whether it is an XML file satisfying the CMML specification with
 * the xml directive and the cmml tag. It returns a CMML object.  If
 * you're working on an already opened FILE*, use cmml_open_stdio instead.
 *
 * \param XMLfilename a CMML file to open
 *
 * \returns a CMML* handle
 */
CMML * cmml_open (char *XMLfilename);

/**
 * cmml_attach_parser: Takes a FILE pointer to an already-opened CMML
 * document.  Returns a CMML object containing internal initialised
 * data structures, etc, which is used in subsequent operations on the
 * CMML file.  This function must be called exactly once, after the
 * file descriptor has been opened.
 *
 * \param file an open FILE pointer
 *
 * \returns a CMML* handle
 */
CMML * cmml_new (FILE * file);

/**
 * cmml_detach_parser: Close a CMML object.  Return the FILE pointer
 * from which the CMML object was derived.
 *
 * \param cmml a CMML* handle
 *
 * \returns the open FILE pointer
 */
FILE * cmml_destroy (CMML * cmml);

/**
 * cmml_close: Close a CMML object. 
 *
 * \param cmml a CMML* handle
 *
 * \returns NULL on success or the unchanged CMML object (e.g. in case
 *          of a system error on close())
 */
CMML * cmml_close (CMML * cmml);

/**
 * cmml_set_read_callbacks: registration function for callbacks.
 *
 * \param cmml a CMML* handle
 * \param read_stream a CMMLReadStream callback function for the
 *        stream tag
 * \param read_head a CMMLReadHead callback function for the head
 *        tag
 * \param read_clip a CMMLReadClip callback function for the a
 *        tag
 * \param user_data a pointer to a user defined object being passed to
 *        the callbacks
 *
 * \returns 0 on success, -1 on failure
 */
int cmml_set_read_callbacks (CMML *cmml,
			     CMMLReadStream read_stream,
			     CMMLReadHead read_head,
			     CMMLReadClip read_clip,
			     void * user_data);

/**
 * cmml_read: read n bytes from a file descriptor and parse them.
 * Activates registered callbacks when reaching stream, head or clip
 * end tags and blocks until they have completed. Stores parsed info
 * in status. If an error or EOF occurs, returns -1 in which case you
 * can check the error code with cmml_last_error().
 *
 * \param cmml a CMML* handle
 * \param n number of bytes to read
 *
 * \returns the actual number of bytes read
 */
long cmml_read (CMML *cmml, long n); /* file */

/**
 * cmml_set_sloppy: Sets the parsing and error indication to ignore
 * sloppy errors, which are defined below the CMML_FORMAT_ERROR in the
 * CMML_ErrorType struct
 *
 * \param cmml a CMML* handle
 * \param value a bool value to set the sloppy handling
 *
 */
void cmml_set_sloppy (CMML *cmml, int value);

/**
 * cmml_get_preamble: returns a structure containing some of the
 * parsing informtion received out of a stream.
 *
 * \param cmml a CMML* handle
 *
 * \returns 1 if CMML tag specificed, 0 otherwise
 */
CMML_Preamble * cmml_get_preamble (CMML *cmml);

/**
 * cmml_get_last_stream: Return a copy of the current stream element.
 *
 * \param cmml a CMML* handle
 *
 * \returns a CMML_Stream* structure with the stream element details;
 *          NULL if none existed
 */
CMML_Stream * cmml_get_last_stream (CMML *cmml);

/**
 * cmml_get_last_head: Return a copy of the current head element.
 *
 * \param cmml a CMML* handle
 *
 * \returns a CMML_Head* structure with the head element details;
 *          NULL if none existed
 */
CMML_Head   * cmml_get_last_head   (CMML *cmml);

/**
 * cmml_get_last_clip: Return a copy of the current clip element.
 *
 * \param cmml a CMML* handle
 *
 * \returns a CMML_Clip* structure with the clip element details;
 *          NULL if none existed
 */
CMML_Clip * cmml_get_last_clip (CMML *cmml);

/**
 * cmml_get_previous_clip: Return a copy of the previous clip
 * element.
 *
 * \param cmml a CMML* handle
 *
 * \returns a CMML_Clip* structure with the clip element details;
 *          NULL if none existed
 */
CMML_Clip * cmml_get_previous_clip (CMML *cmml);

/**
 * cmml_get_last_error: Return the error status of the most recently
 * executed cmml command.
 *
 * \param cmml a CMML* handle
 *
 * \returns a CMML_Error* structure with the error details;
 *          NULL if none happened
 */
CMML_Error * cmml_get_last_error (CMML * cmml);


/**
 * cmml_error_clear: Clear the last error.
 *
 * \param cmml a CMML* handle
 */
void         cmml_error_clear (CMML * cmml);

/**
 * cmml_set_window: sets the active temporal interval of a cmml stream;
 * all clips outside that interval get ignored
 *
 * \param cmml a CMML* handle
 * \param start the start time of the interval given as CMML_Time* structure
 * \param end the end time of the interval given as CMML_Time* structure
 *
 * \returns 2 if both times could be set, 1 for one only, and 0 if none
 */
int cmml_set_window (CMML * cmml, CMML_Time * start, CMML_Time * end);

/**
 * cmml_skip_to_secs: seeks forward thru clip tags to required time
 * offset (no backwards seeking: streaming!).
 *
 * \param cmml a CMML* handle
 * \param seconds the time offset to seek to
 *
 * \returns the actual time offset seeked to or -1 on error
 */
double cmml_skip_to_secs (CMML *cmml, double seconds);

/**
 * cmml_skip_to_utc: seeks forward thru clip tags to required time
 * offset (no backwards seeking: streaming!).
 *
 * \param cmml a CMML* handle
 * \param utc the time offset to seek to
 *
 * \returns the actual time offset seeked to in seconds or -1 on error
 */
double cmml_skip_to_utc  (CMML *cmml, const char * utc);

/**
 * cmml_skip_to_id: seeks forward thru clip tags to required id (no
 * backwards seeking: streaming!).
 *
 * \param cmml a CMML* handle
 * \param id the name of the clip tag to seek to
 *
 * \returns the actual time offset seeked to in seconds or -1 on error
 */
double cmml_skip_to_id   (CMML *cmml, const char * id);

/** 
 * cmml_skip_to_offset: seek forward thru clip tags and time to
 * required fragment offset (no backwards seeking: streaming!).
 *
 * \param cmml a CMML* handl
 * \param offset the offset given as id or time spec
 *
 * \returns the actual time offset seeked to in seconds or -1 on error 
 */
double cmml_skip_to_offset (CMML *cmml, const char * offset);


/* ---------------------------------------------------- *
 * CMML element handling: cmml_element.c cmml_snprint.c *
 * ---------------------------------------------------- */

/**
 * cmml_preamble_new: Create a new CMML_Preamble.
 *
 * \param encoding the character encoding to be used; UTF-8 if NULL
 * \param id the id attribute of the cmml tag; not used if NULL
 * \param lang the lang attribute of the cmml tag; not used if NULL
 * \param dir the dir attribute of the cmml tag; not used if NULL
 *
 * \returns a CMML preamble
 */
CMML_Preamble * cmml_preamble_new (char *encoding, char *id, char *lang, char *dir);

/**
 * cmml_element_new: Create a new CMML_Element.
 *
 * \param type the element type to create (head, stream, or clip)
 *
 * \returns a element of the requested type
 */
CMML_Element * cmml_element_new (CMML_Element_Type type);

/**
 * cmml_stream_new: Create a new CMML_Stream.
 *
 * \returns a stream element
 */
CMML_Stream  * cmml_stream_new   (void);

/**
 * cmml_head_new: Create a new CMML_Head.
 *
 * \returns a head element
 */
CMML_Head    * cmml_head_new    (void);

/**
 * cmml_clip_new: Create a new CMML_Anchr.
 *
 * \param start_time the start time at which to create an clip
 * \param end_time the end time at which to end the clip
 *
 * \returns a clip element
 */
CMML_Clip  * cmml_clip_new  (CMML_Time *start_time, CMML_Time *end_time);

/**
 * cmml_error_new: Create a new CMML_Error.
 *
 * \param type the type of error to create as from the given list
 *
 * \returns an error type
 */
CMML_Error   * cmml_error_new   (CMML_Error_Type type);

/**
 * cmml_preamble_clone: Create a copy of a CMML_Preamble.
 *
 * \param src the original preamble
 *
 * \returns a copy of a CMML preamble
 */
CMML_Preamble * cmml_preamble_clone (CMML_Preamble * src);

/**
 * cmml_element_clone: Create a copy of an element.
 *
 * \param src the original element
 *
 * \returns a copy of the element
 */
CMML_Element * cmml_element_clone (CMML_Element * src);

/**
 * cmml_stream_clone: Create a copy of a stream element.
 *
 * \param src the original stream element
 *
 * \returns a copy of the stream element
 */
CMML_Stream  * cmml_stream_clone  (CMML_Stream  * src);

/**
 * cmml_head_clone: Create a copy of a head element.
 *
 * \param src the original head element
 *
 * \returns a copy of the head element
 */
CMML_Head    * cmml_head_clone    (CMML_Head    * src);

/**
 * cmml_clip_clone: Create a copy of a clip element.
 *
 * \param src the original clip element
 *
 * \returns a copy of the clip element
 */
CMML_Clip  * cmml_clip_clone  (CMML_Clip  * src);

/**
 * cmml_preamble_destroy: Free all memory associated with a
 * CMML_Preamble.
 *
 * \param preamble the preamble to free
 */
void cmml_preamble_destroy (CMML_Preamble *preamble);

/**
 * cmml_element_destroy: Free all memory associated with a
 * CMML_Element.
 *
 * \param element the element to free
 */
void cmml_element_destroy (CMML_Element *element);

/**
 * cmml_stream_destroy: Free a stream element.
 *
 * \param stream the stream element to free
 */
void cmml_stream_destroy  (CMML_Stream  *stream);

/**
 * cmml_head_destroy: Free a head element.
 *
 * \param head the head element to free
 */
void cmml_head_destroy    (CMML_Head    *head);

/**
 * cmml_clip_destroy: Free a clip element.
 *
 * \param clip the clip element to free
 */
void cmml_clip_destroy  (CMML_Clip  *clip);

/**
 * cmml_error_destroy: Free an error streucture.
 *
 * \param error the error structure to free
 */
void cmml_error_destroy   (CMML_Error   *error);

/**
 * cmml_preamble_snprint: Prints the preamble as given in the
 * cmml structure into the buffer buf of size n.
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param pre the preamble to print
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_preamble_snprint(char *buf, int n, CMML_Preamble *pre);

/**
 * cmml_element_snprint: Given a buffer buf of size n, write a CMML
 * representation of a CMML_Element into it. 
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param elem the element to print
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 *
 */
int cmml_element_snprint        (char *buf, int n, CMML_Element *elem);

/**
 * cmml_stream_snprint: Prints a stream element into a buffer.
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param stream the stream element structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_stream_snprint         (char *buf, int n, CMML_Stream  *stream);

/**
 * cmml_stream_pretty_snprint: Prints a stream element into a buffer.
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param stream the stream element structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_stream_pretty_snprint  (char *buf, int n, CMML_Stream  *stream);

/**
 * cmml_head_snprint: Prints a head element into a buffer.
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param head the head element structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_head_snprint           (char *buf, int n, CMML_Head    *head);

/**
 * cmml_head_pretty_snprint: Prints a head element into a buffer.
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param head the head element structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_head_pretty_snprint    (char *buf, int n, CMML_Head    *head);

/**
 * cmml_clip_snprint: Prints a clip element into a buffer.
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param clip the clip element structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_clip_snprint         (char *buf, int n, CMML_Clip  *clip);

/**
 * cmml_clip_pretty_snprint: Prints a clip element into a buffer.
 *
 * \param buf the buffer to print into
 * \param n the size of the buffer buf
 * \param clip the clip element structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_clip_pretty_snprint  (char *buf, int n, CMML_Clip  *clip);

/**
 * cmml_error_snprint: Prints a string description of the CMML_Error.
 *
 * \param buf  the buffer to print into
 * \param n the size of the buffer buf
 * \param error the error structure
 * \param cmml the cmml status structure (required for expat errors)
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int cmml_error_snprint (char *buf, int n, CMML_Error *error, CMML *cmml);


/* ----------------------------------------------- *
 * helper functions for list handling: cmml_list.c *
 * ----------------------------------------------- */

/**
 * cmml_list_new: Create a new list.
 *
 * \returns a new list
 */
CMML_List * cmml_list_new        (void);

/**
 * cmml_list_clone: Copy a list using the default cloning function.
 *
 * \param list the list to clone
 *
 * \returns a newly cloned list
 */
CMML_List * cmml_list_clone      (CMML_List * list);

/**
 * cmml_list_clone_with: Clone a list using a custom clone function.
 *
 * \param list the list to clone
 * \param clone the function to use to clone a list item
 *
 * \returns a newly cloned list
 */
CMML_List * cmml_list_clone_with (CMML_List * list, CMML_CloneFunc clone);

/**
 * cmml_list_tail: Returns the tail element of a list.
 *
 * \param list the list
 *
 * \returns the tail element
 */
CMML_List * cmml_list_tail       (CMML_List * list);

/**
 * cmml_list_prepend: Prepend a new node to a list containing given data.
 *
 * \param list the list
 * \param data the data element of the newly created node
 *
 * \returns the new list head
 */
CMML_List * cmml_list_prepend    (CMML_List * list, void * data);

/**
 * cmml_list_append: Append a new node to a list containing given data.
 *
 * \param list the list
 * \param data the data element of the newly created node
 *
 * \returns the head of the list
 */
CMML_List * cmml_list_append     (CMML_List * list, void * data);

/**
 * cmml_list_add_before: Add a new node containing given data before a
 * given node
 *
 * \param list the list
 * \param data the data element of the newly created node
 * \param node the node before which to add the newly created node
 *
 * \returns the head of the list (which may have changed)
 */
CMML_List * cmml_list_add_before (CMML_List * list, void * data, 
				  CMML_List * node);
/**
 * cmml_list_add_after: Add a new node containing given data after a
 * given node.
 *
 * \param list the list
 * \param data the data element of the newly created node
 * \param node the node after which to add the newly created node
 *
 * \returns the head of the list
 */
CMML_List * cmml_list_add_after  (CMML_List * list, void * data, 
				  CMML_List * node);

/**
 * cmml_list_find: Find the first node containing given data in a list.
 *
 * \param list the list
 * \param data the data element to find
 *
 * \returns the first node containing given data, or NULL if it is not found
 */
CMML_List * cmml_list_find       (CMML_List * list, void * data);

/**
 * cmml_list_remove: Remove a node from a list.
 *
 * \param list the list
 * \param node the node to remove
 *
 * \returns the head of the list (which may have changed)
 */
CMML_List * cmml_list_remove     (CMML_List * list, CMML_List * node);

/**
 * cmml_list_length: Query the number of items in a list.
 *
 * \param list the list
 *
 * \returns the number of nodes in the list
 */
int cmml_list_length             (CMML_List * list);

/**
 * cmml_list_is_empty: Query if a list is empty, ie. contains no items.
 *
 * \param list the list
 *
 * \returns 1 if the list is empty, 0 otherwise
 */
int cmml_list_is_empty           (CMML_List * list);

/**
 * cmml_list_is_singleton: Query if the list is singleton,
 * ie. contains exactly one item
 *
 * \param list the list
 *
 * \returns 1 if the list is singleton, 0 otherwise
 */
int cmml_list_is_singleton       (CMML_List * list);

/**
 * cmml_list_free_with: Free a list, using a given function to free
 * each data element
 *
 * \param list the list
 * \param free_func a function to free each data element
 *
 * \returns NULL on success
 */
CMML_List * cmml_list_free_with   (CMML_List * list, 
				   CMML_FreeFunc free_func);

/**
 * cmml_list_free: Free a list, using anx_free() to free each data
 * element.
 *
 * \param list the list
 *
 * \returns NULL on success else the original list.
 */
CMML_List * cmml_list_free       (CMML_List * list);


/* ----------------------------------------------- *
 * helper functions for time handling: cmml_time.c *
 * ----------------------------------------------- */

/**
 * cmml_time_new: Creates a time struct from a string that contains a
 * name-value time spec pair.
 *
 * \param s name-value time specification string
 *
 * \returns a time struct with appropriate time type (utc or seconds).
 */
CMML_Time * cmml_time_new  (const char * s);

/**
 * cmml_sec_new: Creates a time struct only from name-value sec specs.
 *
 * \param s name-value time specification string
 *
 * \returns a time struct.
 */
CMML_Time * cmml_sec_new   (const char * s);

/**
 * cmml_utc_new: Creates a time struct only from name-value utc specs.
 *
 * \param s name-value time specification string
 *
 * \returns a time struct.
 */
CMML_Time * cmml_utc_new   (const char * s);

/**
 * cmml_time_interval_new: Handles start and end times for clip
 * tags, where we will only store sec offsets.
 *
 * \param s the time construct; examples: npt:40-79 or
 * smpte-25:00:20:20-00:21:30,00:40:21
 * \param t_start the start time returned 
 * \param t_end the end time (if any) returned
 *
 * \returns creates two time constructs if the string was a time
 * range, otherwise just a t_start time, and returns the number of
 * times created or -1 otherwise.
 */
int cmml_time_interval_new (const char * s, 
			    CMML_Time ** t_start, CMML_Time ** t_end);

/**
 * cmml_time_new_in_sec: For parsing/converting start and end time
 * specs in clips into a seconds representation, which is the only
 * one libcmml stores internally for clips.
 *
 * \param s the string that contains the time spec
 * \param ref the reference utc time if given in the stream tag
 * \param base the timebase in seconds as given in the stream tag
 *
 * \returns a time structure in seconds
 */
CMML_Time *cmml_time_new_in_sec (const char * s, CMML_Time * ref, double base);

/**
 * cmml_time_utc_to_sec: Convert a time from utc to seconds with
 * reference to a timebase
 *
 * \param t the given time struct
 * \param ref the reference utc time if given in the stream tag
 * \param base the timebase in seconds as given in the stream tag
 *
 * \returns a time structure in seconds
 */
CMML_Time *cmml_time_utc_to_sec (CMML_Time * t, CMML_Time * ref, double base);

/**
 * cmml_time_free: Free time struct
 *
 * \param t the given time struct
 */
void        cmml_time_free (CMML_Time *t);

/**
 * cmml_time_clone: Copying times 
 *
 * \param t the given time struct
 *
 * \returns the cloned time struct
 */
CMML_Time * cmml_time_clone   (CMML_Time *t);

/**
 * cmml_sec_parse: for parsing sec specs with "npt:", "smpte:" etc.
 *
 * \param s the string containing the name-value time specification
 *
 * \returns the parsed seconds as a double
 */
double cmml_sec_parse (const char *s);

/**
 * cmml_utc_parse: for parsing utc times with "clock:" 
 *
 * \param s the string containing only the utc string as
 * YYYYMMDDTHHmmss.hhZ 
 *
 * \returns a utc time structure
 */
CMML_UTC * cmml_utc_parse (const char * s);

/**
 * cmml_utc_clone: for copying utc times
 *
 * \param t the src utc time structure
 *
 * \returns the copied utc time structure
 */
CMML_UTC * cmml_utc_clone (CMML_UTC *t);

/**
 * cmml_utc_diff: for calculating the difference between two utc times
 * as t2-t1 in seconds
 *
 * \param t2 the more recent utc time
 * \param t1 the less recent utc time
 *
 * \returns the difference between the two utc times in seconds
 */
double     cmml_utc_diff  (CMML_UTC *t2, CMML_UTC *t1);

/**
 * cmml_npt_snprint: prints npt time to a string
 *
 * \param buf buffer to print string to
 * \param n the size of the buffer buf
 * \param seconds the npt time in seconds as a double
 *
 * \returns number of characters written to the buffer (including
 * the terminating \\0) or a  negative value on failure.
 */
int        cmml_npt_snprint (char * buf, int n, double seconds);

/**
 * cmml_utc_snprint: for printing utc times
 *
 * \param buf  the buffer to print into
 * \param n the size of the buffer buf
 * \param t the utc time structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int        cmml_utc_snprint (char *buf, int n, CMML_UTC *t);

/**
 * cmml_utc_pretty_snprint: for printing utc times
 *
 * \param buf  the buffer to print into
 * \param n the size of the buffer buf
 * \param t the utc time structure
 *
 * \returns the number of characters written to the buffer (including
 * the terminating \\0) or -1 if buffer too small.
 */
int        cmml_utc_pretty_snprint (char *buf, int n, CMML_UTC *t);



#endif /* __CMML_H__ */
