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

/* cmml_list.c */

#include "cmml_private.h"
#include "cmml.h"

static CMML_List *
cmml_list_node_new (void * data)
{
  CMML_List * l;

  l = (CMML_List *) cmml_malloc (sizeof (CMML_List));
  l->prev = l->next = NULL;
  l->data = data;

  return l;
}

CMML_List *
cmml_list_new (void)
{
  return NULL;
}

CMML_List *
cmml_list_clone (CMML_List * list)
{
  CMML_List * l, * new_list;

  if (list == NULL) return NULL;
  new_list = cmml_list_new ();

  for (l = list; l; l = l->next) {
    new_list = cmml_list_append (new_list, l->data);
  }

  return new_list;
}

CMML_List *
cmml_list_clone_with (CMML_List * list, CMML_CloneFunc clone)
{
  CMML_List * l, * new_list;
  void * new_data;

  if (list == NULL) return NULL;
  if (clone == NULL) return cmml_list_clone (list);

  new_list = cmml_list_new ();

  for (l = list; l; l = l->next) {
    new_data = clone (l->data);
    new_list = cmml_list_append (new_list, new_data);
  }

  return new_list;
}


CMML_List *
cmml_list_tail (CMML_List * list)
{
  CMML_List * l;

  for (l = list; l; l = l->next)
    if (l->next == NULL) return l;
  return NULL;
}

CMML_List *
cmml_list_prepend (CMML_List * list, void * data)
{
  CMML_List * l = cmml_list_node_new (data);
  if (list == NULL) return l;
  l->next = list;
  list->prev = l;
  return l;
}

CMML_List *
cmml_list_append (CMML_List * list, void * data)
{
  CMML_List * l = cmml_list_node_new (data);
  CMML_List * last;

  if (list == NULL) return l;

  last = cmml_list_tail (list);
  if (last) last->next = l;
  l->prev = last; 
  return list;
}

CMML_List *
cmml_list_add_before (CMML_List * list, void * data, CMML_List * node)
{
  CMML_List * l, * p;

  if (list == NULL) return cmml_list_node_new (data);
  if (node == NULL) return cmml_list_append (list, data);
  if (node == list) return cmml_list_prepend (list, data);

  l = cmml_list_node_new (data);
  p = node->prev;

  l->prev = p;
  l->next = node;
  if (p) p->next = l;
  node->prev = l;
  
  return list;
}

CMML_List *
cmml_list_add_after (CMML_List * list, void * data, CMML_List * node)
{
  CMML_List * l, * n;

  if (node == NULL) return cmml_list_prepend (list, data);

  l = cmml_list_node_new (data);
  n = node->next;

  l->prev = node;
  l->next = n;
  if (n) n->prev = l;
  node->next = l;

  return list;
}

CMML_List *
cmml_list_find (CMML_List * list, void * data)
{
  CMML_List * l;

  for (l = list; l; l = l->next)
    if (l->data == data) return l;

  return NULL;
}

CMML_List *
cmml_list_remove (CMML_List * list, CMML_List * node)
{
  if (node == NULL) return list;

  if (node->prev) node->prev->next = node->next;
  if (node->next) node->next->prev = node->prev;

  if (node == list) return list->next;
  else return list;
}

int
cmml_list_length (CMML_List * list)
{
  CMML_List * l;
  int c = 0;

  for (l = list; l; l = l->next)
    c++;

  return c;
}

int
cmml_list_is_empty (CMML_List * list)
{
  return (list == NULL);
}

int
cmml_list_is_singleton (CMML_List * list)
{
  if (list == NULL) return 0;
  if (list->next == NULL) return 1;
  else return 0;
}

/*
 * cmml_list_free_with (list)
 *
 * Step through list 'list', freeing each node using ss_free(), and
 * also free the list structure itself.
 */
CMML_List *
cmml_list_free_with (CMML_List * list, CMML_FreeFunc free_func)
{
  CMML_List * l, * ln;

  for (l = list; l; l = ln) {
    ln = l->next;
    free_func (l->data);
    cmml_free (l);
  }

  return NULL;
}

/*
 * cmml_list_free (list)
 *
 * Free the list structure 'list', but not its nodes.
 */
CMML_List *
cmml_list_free (CMML_List * list)
{
  CMML_List * l, * ln;

  for (l = list; l; l = ln) {
    ln = l->next;
    cmml_free (l);
  }

  return NULL;
}
