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

#include <stdlib.h>

#include "stdafx.h"
#include "xlist.h"

static XList *
xlist_node_new (void * data)
{
  XList * l;

  l = (XList *) malloc (sizeof (XList));
  l->prev = l->next = NULL;
  l->data = data;

  return l;
}

XList *
xlist_new (void)
{
  return NULL;
}

XList *
xlist_clone (XList * list)
{
  XList * l, * new_list;

  if (list == NULL) return NULL;
  new_list = xlist_new ();

  for (l = list; l; l = l->next) {
    new_list = xlist_append (new_list, l->data);
  }

  return new_list;
}

XList *
xlist_clone_with (XList * list, XCloneFunc clone)
{
  XList * l, * new_list;
  void * new_data;

  if (list == NULL) return NULL;
  if (clone == NULL) return xlist_clone (list);

  new_list = xlist_new ();

  for (l = list; l; l = l->next) {
    new_data = clone (l->data);
    new_list = xlist_append (new_list, new_data);
  }

  return new_list;
}


XList *
xlist_tail (XList * list)
{
  XList * l;
  for (l = list; l; l = l->next)
    if (l->next == NULL) return l;
  return NULL;
}

XList *
xlist_prepend (XList * list, void * data)
{
  XList * l = xlist_node_new (data);

  if (list == NULL) return l;

  l->next = list;
  list->prev = l;

  return l;
}

XList *
xlist_append (XList * list, void * data)
{
  XList * l = xlist_node_new (data);
  XList * last;

  if (list == NULL) return l;

  last = xlist_tail (list);
  if (last) last->next = l;
  l->prev = last; 
  return list;
}

XList *
xlist_add_before (XList * list, void * data, XList * node)
{
  XList * l, * p;

  if (list == NULL) return xlist_node_new (data);
  if (node == NULL) return xlist_append (list, data);
  if (node == list) return xlist_prepend (list, data);

  l = xlist_node_new (data);
  p = node->prev;

  l->prev = p;
  l->next = node;
  if (p) p->next = l;
  node->prev = l;
  
  return list;
}

XList *
xlist_add_after (XList * list, void * data, XList * node)
{
  XList * l, * n;

  if (node == NULL) return xlist_prepend (list, data);

  l = xlist_node_new (data);
  n = node->next;

  l->prev = node;
  l->next = n;
  if (n) n->prev = l;
  node->next = l;

  return list;
}

XList *
xlist_find (XList * list, void * data)
{
  XList * l;

  for (l = list; l; l = l->next)
    if (l->data == data) return l;

  return NULL;
}

XList *
xlist_remove (XList * list, XList * node)
{
  if (node == NULL) return list;

  if (node->prev) node->prev->next = node->next;
  if (node->next) node->next->prev = node->prev;

  if (node == list) return list->next;
  else return list;
}

int
xlist_length (XList * list)
{
  XList * l;
  int c = 0;

  for (l = list; l; l = l->next)
    c++;

  return c;
}

int
xlist_is_empty (XList * list)
{
  return (list == NULL);
}

int
xlist_is_singleton (XList * list)
{
  if (list == NULL) return 0;
  if (list->next == NULL) return 1;
  else return 0;
}

/*
 * xlist_free_with (list, free_func)
 *
 * Step through list 'list', freeing each node using free_func(), and
 * also free the list structure itself.
 */
XList *
xlist_free_with (XList * list, XFreeFunc free_func)
{
  XList * l, * ln;

  for (l = list; l; l = ln) {
    ln = l->next;
    free_func (l->data);
    free (l);
  }

  return NULL;
}

/*
 * xlist_free (list)
 *
 * Free the list structure 'list', but not its nodes.
 */
XList *
xlist_free (XList * list)
{
  XList * l, * ln;

  for (l = list; l; l = ln) {
    ln = l->next;
    free (l);
  }

  return NULL;
}

