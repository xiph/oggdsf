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

#include <string.h>
#include <annodex/anx_types.h>
#include <annodex/anx_core.h>

#include "anx_snprint.h"

AnxMetaElement *
anx_meta_element_clone (AnxMetaElement * meta)
{
  AnxMetaElement * new_meta;

  if (meta == NULL) return NULL;

  new_meta = (AnxMetaElement* ) anx_malloc (sizeof (AnxMetaElement));
  new_meta->id = anx_strdup (meta->id);
  new_meta->lang = anx_strdup (meta->lang);
  new_meta->dir = anx_strdup (meta->dir);
  new_meta->name = anx_strdup (meta->name);
  new_meta->content = anx_strdup (meta->content);
  new_meta->scheme = anx_strdup (meta->scheme);

  return new_meta;
}

AnxMetaElement *
anx_meta_element_free (AnxMetaElement * meta)
{
  if (meta == NULL) return NULL;

  anx_free (meta->id);
  anx_free (meta->lang);
  anx_free (meta->dir);
  anx_free (meta->name);
  anx_free (meta->content);
  anx_free (meta->scheme);

  anx_free (meta);

  return NULL;
}

AnxHead *
anx_head_clone (AnxHead * head)
{
  AnxHead * new_head;

  new_head = (AnxHead *) anx_malloc (sizeof (AnxHead));

  new_head->head_id = anx_strdup (head->head_id);
  new_head->lang = anx_strdup (head->lang);
  new_head->dir = anx_strdup (head->dir);
  new_head->profile = anx_strdup (head->profile);
  new_head->title = anx_strdup (head->title);
  new_head->title_id = anx_strdup (head->title_id);
  new_head->title_lang = anx_strdup(head->title_lang);
  new_head->title_dir = anx_strdup (head->title_dir);
  new_head->base_id = anx_strdup (head->base_id);
  new_head->base_href = anx_strdup (head->base_href);

  new_head->meta =
    anx_list_clone_with (head->meta, (AnxCloneFunc)anx_meta_element_clone);

  return new_head;
}

AnxHead *
anx_head_free (AnxHead * head)
{
  if (head == NULL) return NULL;
  
  anx_free (head->head_id);
  anx_free (head->lang);
  anx_free (head->dir);
  anx_free (head->profile);
  anx_free (head->title);
  anx_free (head->title_id);
  anx_free (head->title_lang);
  anx_free (head->title_dir);
  anx_free (head->base_id);
  anx_free (head->base_href);

  anx_list_free_with (head->meta, (AnxFreeFunc)anx_meta_element_free);

  anx_free (head);

  return NULL;
}

AnxClip *
anx_clip_clone (AnxClip * clip)
{
  AnxClip * new_clip;

  new_clip = (AnxClip *) anx_malloc (sizeof (AnxClip));

  new_clip->clip_id = anx_strdup (clip->clip_id);
  new_clip->lang = anx_strdup (clip->lang);
  new_clip->dir = anx_strdup (clip->dir);
  new_clip->track = anx_strdup (clip->track);

  new_clip->anchor_id = anx_strdup (clip->anchor_id);
  new_clip->anchor_lang = anx_strdup (clip->anchor_lang);
  new_clip->anchor_dir = anx_strdup (clip->anchor_dir);
  new_clip->anchor_class = anx_strdup (clip->anchor_class);
  new_clip->anchor_href = anx_strdup (clip->anchor_href);
  new_clip->anchor_text = anx_strdup (clip->anchor_text);

  new_clip->img_id = anx_strdup (clip->img_id);
  new_clip->img_lang = anx_strdup (clip->img_lang);
  new_clip->img_dir = anx_strdup (clip->img_dir);
  new_clip->img_src = anx_strdup (clip->img_src);
  new_clip->img_alt = anx_strdup (clip->img_alt);

  new_clip->desc_id = anx_strdup (clip->desc_id);
  new_clip->desc_lang = anx_strdup (clip->desc_lang);
  new_clip->desc_dir = anx_strdup (clip->desc_dir);
  new_clip->desc_text = anx_strdup (clip->desc_text);

  new_clip->meta =
    anx_list_clone_with (clip->meta, (AnxCloneFunc)anx_meta_element_clone);

  return new_clip;
}

AnxClip *
anx_clip_free (AnxClip * clip)
{
  if (clip == NULL) return NULL;

  anx_free (clip->clip_id);
  anx_free (clip->lang);
  anx_free (clip->dir);
  anx_free (clip->track);
  anx_free (clip->anchor_id);
  anx_free (clip->anchor_lang);
  anx_free (clip->anchor_dir);
  anx_free (clip->anchor_class);
  anx_free (clip->anchor_href);
  anx_free (clip->anchor_text);
  anx_free (clip->img_id);
  anx_free (clip->img_lang);
  anx_free (clip->img_dir);
  anx_free (clip->img_src);
  anx_free (clip->img_alt);
  anx_free (clip->desc_id);
  anx_free (clip->desc_lang);
  anx_free (clip->desc_dir);
  anx_free (clip->desc_text);

  anx_list_free_with (clip->meta, (AnxFreeFunc)anx_meta_element_free);

  anx_free (clip);

  return NULL;
}
