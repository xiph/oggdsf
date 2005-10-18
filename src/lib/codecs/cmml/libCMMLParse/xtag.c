//===========================================================================
//Copyright (C) 2005 Commonwealth Scientific and Industrial Research
//                   Organisation (CSIRO) Australia
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================


#include "stdafx.h"

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "xlist.h"

#undef XTAG_DEBUG

#undef FALSE
#undef TRUE

#define FALSE (0)
#define TRUE (!FALSE)

#undef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))

#undef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))

typedef struct _XTag XTag;
typedef struct _XAttribute XAttribute;
typedef struct _XTagParser XTagParser;

#define XTAG_INTERNAL
#include "xtag.h"

/*
 * struct _XTag is kind of a union ... it normally represents a whole
 * tag (and its children), but it could alternatively represent some
 * PCDATA. Basically, if tag->pcdata is non-NULL, interpret only it and
 * ignore the name, attributes and inner_tags.
 */
struct _XTag {
  char * name;
  char * pcdata;
  XTag * parent;
  XList * attributes;
  XList * children;
  XList * current_child;
};

struct _XAttribute {
  char * name;
  char * value;
};

struct _XTagParser {
  int valid; /* boolean */
  XTag * current_tag;
  char * start;
  char * end;
};

/* Character classes */
#define X_NONE           0
#define X_WHITESPACE  1<<0
#define X_OPENTAG     1<<1
#define X_CLOSETAG    1<<2
#define X_DQUOTE      1<<3
#define X_SQUOTE      1<<4
#define X_EQUAL       1<<5
#define X_SLASH       1<<6

static int
xtag_cin (char c, int char_class)
{
  if (char_class & X_WHITESPACE)
    if (isspace(c)) return TRUE;

  if (char_class & X_OPENTAG)
    if (c == '<') return TRUE;

  if (char_class & X_CLOSETAG)
    if (c == '>') return TRUE;

  if (char_class & X_DQUOTE)
    if (c == '"') return TRUE;

  if (char_class & X_SQUOTE)
    if (c == '\'') return TRUE;

  if (char_class & X_EQUAL)
    if (c == '=') return TRUE;

  if (char_class & X_SLASH)
    if (c == '/') return TRUE;

  return FALSE;
}

static int
xtag_index (XTagParser * parser, int char_class)
{
  char * s;
  int i;

  s = parser->start;

  for (i = 0; s[i] && s != parser->end; i++) {
    if (xtag_cin(s[i], char_class)) return i;
  }

  return -1;
}

static void
xtag_skip_over (XTagParser * parser, int char_class)
{
  char * s;
  int i;

  if (!parser->valid) return;

  s = (char *)parser->start;

  for (i = 0; s[i] && s != parser->end; i++) {
    if (!xtag_cin(s[i], char_class)) {
      parser->start = &s[i];
      return;
    }
  }

  return;
}

static void
xtag_skip_whitespace (XTagParser * parser)
{
  xtag_skip_over (parser, X_WHITESPACE);
}

#if 0
static void
xtag_skip_to (XTagParser * parser, int char_class)
{
  char * s;
  int i;

  if (!parser->valid) return;

  s = (char *)parser->start;

  for (i = 0; s[i] && s != parser->end; i++) {
    if (xtag_cin(s[i], char_class)) {
      parser->start = &s[i];
      return;
    }
  }

  return;  
}
#endif

static char *
xtag_slurp_to (XTagParser * parser, int good_end, int bad_end)
{
  char * s, * ret;
  int xi;

  if (!parser->valid) return NULL;

  s = parser->start;

  xi = xtag_index (parser, good_end | bad_end);

  if (xi > 0 && xtag_cin (s[xi], good_end)) {
    ret = malloc ((xi+1) * sizeof(char));
    strncpy (ret, s, xi);
    ret[xi] = '\0';
    parser->start = &s[xi];
    return ret;
  }

  return NULL;
}

static int
xtag_assert_and_pass (XTagParser * parser, int char_class)
{
  char * s;

  if (!parser->valid) return FALSE;

  s = parser->start;

  if (!xtag_cin (s[0], char_class)) {
    parser->valid = FALSE;
    return FALSE;
  }

  parser->start = &s[1];

  return TRUE;
}

static char *
xtag_slurp_quoted (XTagParser * parser)
{
  char * s, * ret;
  int quote = X_DQUOTE; /* quote char to match on */
  int xi;

  if (!parser->valid) return NULL;

  xtag_skip_whitespace (parser);

  s = parser->start;

  if (xtag_cin (s[0], X_SQUOTE)) quote = X_SQUOTE;

  if (!xtag_assert_and_pass (parser, quote)) return NULL;

  s = parser->start;

  for (xi = 0; s[xi]; xi++) {
    if (xtag_cin (s[xi], quote)) {
      if (!(xi > 1 && s[xi-1] == '\\')) break;
    }
  }

  ret = malloc ((xi+1) * sizeof(char));
  strncpy (ret, s, xi);
  ret[xi] = '\0';
  parser->start = &s[xi];

  if (!xtag_assert_and_pass (parser, quote)) return NULL;

  return ret;
}

static XAttribute *
xtag_parse_attribute (XTagParser * parser)
{
  XAttribute * attr;
  char * name, * value;
  char * s;

  if (!parser->valid) return NULL;

  xtag_skip_whitespace (parser);
 
  name = xtag_slurp_to (parser, X_WHITESPACE | X_EQUAL, X_SLASH | X_CLOSETAG);

  if (name == NULL) return NULL;

  xtag_skip_whitespace (parser);
  s = parser->start;

  if (!xtag_assert_and_pass (parser, X_EQUAL)) {
#ifdef XTAG_DEBUG
    printf ("xtag: attr failed EQUAL on <%s>\n", name);
#endif
    goto err_free_name;
  }

  xtag_skip_whitespace (parser);

  value = xtag_slurp_quoted (parser);

  if (value == NULL) {
#ifdef XTAG_DEBUG
    printf ("Got NULL quoted attribute value\n");
#endif
    goto err_free_name;
  }

  attr = malloc (sizeof (*attr));
  attr->name = name;
  attr->value = value;

  return attr;

 err_free_name:
  free (name);

  parser->valid = FALSE;

  return NULL;
}

static XTag *
xtag_parse_tag (XTagParser * parser)
{
  XTag * tag, * inner;
  XAttribute * attr;
  char * name;
  char * pcdata;
  char * s;

  if (!parser->valid) return NULL;

  if ((pcdata = xtag_slurp_to (parser, X_OPENTAG, X_NONE)) != NULL) {
    tag = malloc (sizeof (*tag));
    tag->name = NULL;
    tag->pcdata = pcdata;
    tag->parent = parser->current_tag;
    tag->attributes = NULL;
    tag->children = NULL;
    tag->current_child = NULL;

    return tag;
  }

  s = parser->start;

  /* if this starts a close tag, return NULL and let the parent take it */
  if (xtag_cin (s[0], X_OPENTAG) && xtag_cin (s[1], X_SLASH))
    return NULL;

  if (!xtag_assert_and_pass (parser, X_OPENTAG)) return NULL;

  name = xtag_slurp_to (parser, X_WHITESPACE | X_SLASH | X_CLOSETAG, X_NONE);

  if (name == NULL) return NULL;

#ifdef XTAG_DEBUG
  printf ("<%s ...\n", name);
#endif

  tag = malloc (sizeof (*tag));
  tag->name = name;
  tag->pcdata = NULL;
  tag->parent = parser->current_tag;
  tag->attributes = NULL;
  tag->children = NULL;
  tag->current_child = NULL;

  s = parser->start;

  if (xtag_cin (s[0], X_WHITESPACE)) {
    while ((attr = xtag_parse_attribute (parser)) != NULL) {
      tag->attributes = xlist_append (tag->attributes, attr);
    }
  }

  xtag_skip_whitespace (parser);

  s = parser->start;

  if (xtag_cin (s[0], X_CLOSETAG)) {
    parser->current_tag = tag;

    xtag_assert_and_pass (parser, X_CLOSETAG);

    while ((inner = xtag_parse_tag (parser)) != NULL) {
      tag->children = xlist_append (tag->children, inner);
    }

    xtag_skip_whitespace (parser);

    xtag_assert_and_pass (parser, X_OPENTAG);
    xtag_assert_and_pass (parser, X_SLASH);
    name = xtag_slurp_to (parser, X_WHITESPACE | X_CLOSETAG, X_NONE);
    if (name) {
      if (name && tag->name && strcmp (name, tag->name)) {
#ifdef XTAG_DEBUG
        printf ("got %s expected %s\n", name, tag->name);
#endif
        parser->valid = FALSE;
      }
      free (name);
    }

    xtag_skip_whitespace (parser);
    xtag_assert_and_pass (parser, X_CLOSETAG);

  } else {
    xtag_assert_and_pass (parser, X_SLASH);
    xtag_assert_and_pass (parser, X_CLOSETAG);
  }


  return tag;
}

XTag *
xtag_free (XTag * xtag)
{
  XList * l;
  XAttribute * attr;
  XTag * child;

  if (xtag == NULL) return NULL;

  if (xtag->name) free (xtag->name);
  if (xtag->pcdata) free (xtag->pcdata);

  for (l = xtag->attributes; l; l = l->next) {
    if ((attr = (XAttribute *)l->data) != NULL) {
      if (attr->name) free (attr->name);
      if (attr->value) free (attr->value);
      free (attr);
    }
  }
  xlist_free (xtag->attributes);

  for (l = xtag->children; l; l = l->next) {
    child = (XTag *)l->data;
    xtag_free (child);
  }
  xlist_free (xtag->children);

  free (xtag);

  return NULL;
}

XTag *
xtag_new_parse (const char * s, int n, int* ErrorOffset)
{
  XTagParser parser;
  XTag * tag, * ttag, * wrapper;

  parser.valid = TRUE;
  parser.current_tag = NULL;
  parser.start = (char *)s;

  if (n == -1)
    parser.end = NULL;
  else if (n == 0)
    return NULL;
  else
    parser.end = (char *)&s[n];

  tag = xtag_parse_tag (&parser);

  if (!parser.valid) {
    xtag_free (tag);
	*ErrorOffset = (int)(parser.start - s);	// DLB. 20/9/05
    return NULL;
  }

  if ((ttag = xtag_parse_tag (&parser)) != NULL) {

    if (!parser.valid) {
      xtag_free (ttag);
	  *ErrorOffset = (int)(parser.start - s);	// DLB. 20/9/05
      return tag;
    }

    wrapper = malloc (sizeof (XTag));
    wrapper->name = NULL;
    wrapper->pcdata = NULL;
    wrapper->parent = NULL;
    wrapper->attributes = NULL;
    wrapper->children = NULL;
    wrapper->current_child = NULL;

    wrapper->children = xlist_append (wrapper->children, tag);
    wrapper->children = xlist_append (wrapper->children, ttag);

    while ((ttag = xtag_parse_tag (&parser)) != NULL) {

      if (!parser.valid) {
        xtag_free (ttag);
		*ErrorOffset = (int)(parser.start - s);	// DLB. 20/9/05
        return wrapper;
      }

      wrapper->children = xlist_append (wrapper->children, ttag);
    }

	*ErrorOffset = (int)(parser.start - s);	// DLB. 20/9/05
    return wrapper;
  }

  *ErrorOffset = (int)(parser.start - s);	// DLB. 20/9/05
  return tag;
}

char *
xtag_get_name (XTag * xtag)
{
  return xtag ? xtag->name : NULL;
}

char *
xtag_get_pcdata (XTag * xtag)
{
  XList * l;
  XTag * child;

  if (xtag == NULL) return NULL;

  for (l = xtag->children; l; l = l->next) {
    child = (XTag *)l->data;
    if (child->pcdata != NULL) {
      return child->pcdata;
    }
  }

  return NULL;
}

char *
xtag_get_attribute (XTag * xtag, char * attribute)
{
  XList * l;
  XAttribute * attr;

  if (xtag == NULL) return NULL;

  for (l = xtag->attributes; l; l = l->next) {
    if ((attr = (XAttribute *)l->data) != NULL) {
      if (attr->name && attribute && !strcmp (attr->name, attribute))
        return attr->value;
    }
  }

  return NULL;
}

XTag *
xtag_first_child (XTag * xtag, char * name)
{
  XList * l;
  XTag * child;

  if (xtag == NULL) return NULL;

  if ((l = xtag->children) == NULL) return NULL;

  if (name == NULL) {
    xtag->current_child = l;
    return (XTag *)l->data;
  }

  for (; l; l = l->next) {
    child = (XTag *)l->data;

    if (child->name && name && !strcmp(child->name, name)) {
      xtag->current_child = l;
      return child;
    }
  }

  xtag->current_child = NULL;

  return NULL;
}

XTag *
xtag_next_child (XTag * xtag, char * name)
{
  XList * l;
  XTag * child;

  if (xtag == NULL) return NULL;

  if ((l = xtag->current_child) == NULL)
    return xtag_first_child (xtag, name);

  if ((l = l->next) == NULL)
    return NULL;

  if (name == NULL) {
    xtag->current_child = l;
    return (XTag *)l->data;
  }

  for (; l; l = l->next) {
    child = (XTag *)l->data;

    if (child->name && name && !strcmp(child->name, name)) {
      xtag->current_child = l;
      return child;
    }
  }

  xtag->current_child = NULL;

  return NULL;
}

/*
 * This snprints function takes a variable list of char *, the last of
 * which must be NULL, and prints each in turn to buf.
 * Returns C99-style total length that would have been written, even if
 * this is larger than n.
 */
static int
xtag_snprints (char * buf, int n, ...)
{
  va_list ap;
  char * s;
  int len, to_copy, total = 0;

  va_start (ap, n);
  
  for (s = va_arg (ap, char *); s; s = va_arg (ap, char *)) {
    len = (int) strlen (s);

    if ((to_copy = MIN (n, len)) > 0) {
      memcpy (buf, s, to_copy);
      buf += to_copy;
      n -= to_copy;
    }

    total += len;
  }

  va_end (ap);

  return total;
}

int
xtag_snprint (char * buf, int n, XTag * xtag)
{
  int nn, written = 0;
  XList * l;
  XAttribute * attr;
  XTag * child;

#define FORWARD(N) \
  buf += MIN (n, N); \
  n = MAX (n-N, 0);  \
  written += N;

  if (xtag == NULL) {
    if (n > 0) buf[0] = '\0';
    return 0;
  }

  if (xtag->pcdata) {
    nn = xtag_snprints (buf, n, xtag->pcdata, NULL);
    FORWARD(nn);

    return written;
  }

  if (xtag->name) {
    nn = xtag_snprints (buf, n, "<", xtag->name, NULL);
    FORWARD(nn);

    for (l = xtag->attributes; l; l = l->next) {
      attr = (XAttribute *)l->data;
      
      nn = xtag_snprints (buf, n, " ", attr->name, "=\"", attr->value, "\"",
                          NULL);
      FORWARD(nn);
    }
    
    if (xtag->children == NULL) {
      nn = xtag_snprints (buf, n, "/>", NULL);
      FORWARD(nn);

      return written;
    }
    
    nn = xtag_snprints (buf, n, ">", NULL);
    FORWARD(nn);
  }

  for (l = xtag->children; l; l = l->next) {
    child = (XTag *)l->data;

    nn = xtag_snprint (buf, n, child);
    FORWARD(nn);
  }

  if (xtag->name) {
    nn = xtag_snprints (buf, n, "</", xtag->name, ">", NULL);
    FORWARD(nn);
  }

  return written;
}

