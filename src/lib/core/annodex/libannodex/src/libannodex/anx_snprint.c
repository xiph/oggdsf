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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <annodex/anx_core.h>
#include <annodex/anx_types.h>
#include "anx_snprint.h"

char *
anx_strdup (char * s)
{
  char * ret;
  if (!s) return NULL;
  ret = anx_malloc (strlen (s) + 1);
  return strcpy (ret, s);
}

/*
 * XXX: anx_snprintf implements the non-C99 functionality of returning -1 on
 * potential overrun.
 */
static int
anx_snprintf (char * str, size_t size, const char * format_args, ...)
{
  size_t retval;
  va_list ap;
  
  va_start (ap, format_args);
  retval = vsnprintf (str, size, format_args, ap);
  if (retval > size) retval = -1;
  return retval;
}

static int
anx_snprint_escaped (char * buf, int n, char * text)
{
  int len = 0;
  int n_written;
  char * c;

  for (c = text; *c != '\0'; c++) {
    if (*c == '\"') {
      if ((n_written = anx_snprintf (buf + len, n, "&quot;")) == -1) {
	return -1;
      }
      n -= n_written;
      len += n_written;
    } else if (n > 0) {
      *(buf + len) = *c;
      n--;
      len++;
    } else {
      return -1;
    }
  }

  return len;
}

static int
anx_snprint_attribute (char * buf, int n, char * attr_name, char * attr_value)
{
  int len = 0;
  int n_written;

  if (attr_value == NULL) return 0;

  if ((n_written = anx_snprintf (buf, n, " %s=\"", attr_name)) == -1) {
    return -1;
  }
  n -= n_written;
  len += n_written;

  APPEND_ESCAPED(attr_value);
  APPEND_STRING("\"");

  return len;
}

static int
anx_snprint_time_npt (char * s, int n, char * name, double seconds)
{
  int hrs, min;
  double sec;
  char * sign;

  sign = (seconds < 0.0) ? "-" : "";

  if (seconds < 0.0) seconds = -seconds;

  hrs = (int) (seconds/3600.0);
  min = (int) ((seconds - ((double)hrs * 3600.0)) / 60.0);
  sec = seconds - ((double)hrs * 3600.0)- ((double)min * 60.0);

  /* XXX: %02.3f workaround */
  if (sec < 10.0) {
    return anx_snprintf (s, n, " %s=\"npt:%s%02d:%02d:0%2.3f\"", name,
			 sign, hrs, min, sec);
  } else {
    return anx_snprintf (s, n, " %s=\"npt:%s%02d:%02d:%02.3f\"", name,
			 sign, hrs, min, sec);
  }
}

static int
anx_snprint_time (char * buf, int n, char * name, double seconds)
{
  if (name == NULL) return 0;

  /* XXX: need to support non-npt time formats too */

  return anx_snprint_time_npt (buf, n, name, seconds);
}

int
anx_head_snprint (char *buf, int n, AnxHead * h)
{
    int len = 0;
    int n_written;

    if (h == NULL) {
      return anx_snprintf (buf, n, "<head/>");
    }

    BEGIN_TAG("head");

    APPEND_ATTRIBUTE("id", h->head_id);
    APPEND_ATTRIBUTE("lang", h->lang);
    APPEND_ATTRIBUTE("dir", h->dir);
    APPEND_ATTRIBUTE("profile", h->profile);

    END_TAG();

    if (h->title || h->title_id) {
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

    if (!anx_list_is_empty(h->meta)) {
        AnxList * meta_node;

        for (meta_node = h->meta;
             meta_node != NULL;
             meta_node = meta_node->next) {
            AnxMetaElement * m = (AnxMetaElement *) meta_node->data;

            BEGIN_TAG("meta");
            APPEND_ATTRIBUTE("id", m->id);
	    APPEND_ATTRIBUTE("lang", m->lang);
	    APPEND_ATTRIBUTE("dir", m->dir);
            APPEND_ATTRIBUTE("name", m->name);
            APPEND_ATTRIBUTE("content", m->content);
            APPEND_ATTRIBUTE("scheme", m->scheme);
            END_EMPTY_TAG();
        }
    }
        
    CLOSE_TAG("head");

    return len;
}

int
anx_clip_snprint (char *buf, int n, AnxClip * clip, double start, double end)
{
    int len = 0;
    int n_written;

    if (clip == NULL) {
      return anx_snprintf (buf, n, "<clip/>");
    }

    BEGIN_TAG("clip");
    APPEND_ATTRIBUTE("id", clip->clip_id);
    APPEND_ATTRIBUTE("lang", clip->lang);
    APPEND_ATTRIBUTE("dir", clip->dir);
    APPEND_ATTRIBUTE("track", clip->track);

    if (start >= 0.0) {
      APPEND_TIME_ATTRIBUTE("start", start);
    }

    if (end >= 0.0) {
      APPEND_TIME_ATTRIBUTE("end", end);
    }

    END_TAG();

    if (clip->anchor_id || clip->anchor_lang || clip->anchor_dir ||
	clip->anchor_href || clip->anchor_text) {
      BEGIN_TAG("a");
      APPEND_ATTRIBUTE("id", clip->anchor_id);
      APPEND_ATTRIBUTE("lang", clip->anchor_lang);
      APPEND_ATTRIBUTE("dir", clip->anchor_dir);
      APPEND_ATTRIBUTE("class", clip->anchor_class);
      APPEND_ATTRIBUTE("href", clip->anchor_href);
      END_TAG();

      APPEND_DATA(clip->anchor_text);

      CLOSE_TAG("a");
    }

    if (clip->img_id || clip->img_lang || clip->img_dir || clip->img_src ||
	clip->img_alt) {
      BEGIN_TAG("img");
      APPEND_ATTRIBUTE("id", clip->img_id);
      APPEND_ATTRIBUTE("lang", clip->img_lang);
      APPEND_ATTRIBUTE("dir", clip->img_dir);
      APPEND_ATTRIBUTE("src", clip->img_src);
      APPEND_ATTRIBUTE("alt", clip->img_alt);
      END_EMPTY_TAG();
    }

    if (!anx_list_is_empty(clip->meta)) {
        AnxList * meta_node;

        for (meta_node = clip->meta;
             meta_node != NULL;
             meta_node = meta_node->next) {
            AnxMetaElement * m = (AnxMetaElement *) meta_node->data;

            BEGIN_TAG("meta");
            APPEND_ATTRIBUTE("id", m->id);
	    APPEND_ATTRIBUTE("lang", m->lang);
	    APPEND_ATTRIBUTE("dir", m->dir);
            APPEND_ATTRIBUTE("name", m->name);
            APPEND_ATTRIBUTE("content", m->content);
            APPEND_ATTRIBUTE("scheme", m->scheme);
            END_EMPTY_TAG();
        }
    }

    if (clip->desc_id || clip->desc_lang || clip->desc_dir ||
	clip->desc_text) {
      BEGIN_TAG("desc");
      APPEND_ATTRIBUTE("id", clip->desc_id);
      APPEND_ATTRIBUTE("lang", clip->desc_lang);
      APPEND_ATTRIBUTE("dir", clip->desc_dir);
      END_TAG();
    
      APPEND_DATA(clip->desc_text);
      
      CLOSE_TAG("desc");
    }

    CLOSE_TAG("clip");

    return len;
}
