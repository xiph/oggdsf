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

/* anx_tree.c */

typedef struct _AnxTree AnxTree;
typedef struct _AnxNode AnxNode;

#define _ANX_TREE
#include "anx_tree.h"


#define anx_new(T) (T *) anx_malloc (sizeof(T))

struct _AnxTree {
  AnxCmpFunc cmp;
  AnxNode * root;
};

struct _AnxNode {
  AnxNode * parent;
  AnxNode * left;
  AnxNode * right;
  void * data;
};

static AnxNode *
anx_node_new (void * data)
{
  AnxNode * n = anx_new (AnxNode);
  n->parent = n->left = n->right = NULL;
  n->data = data;
  return n;
}

AnxTree *
anx_tree_new (AnxCmpFunc cmp)
{
  AnxTree * t = anx_new (AnxTree);
  t->cmp = cmp;
  t->root = NULL;
  return t;
}

#define ATTACH_LEFT(p,n) \
  (p)->left = (n);       \
  (n)->parent = (p);

#define ATTACH_RIGHT(p,n) \
  (p)->right = (n);       \
  (n)->parent = (p);


static __inline__ AnxTree *
_anx_tree_insert (void * ctx, AnxTree * tree, AnxNode * node)
{
  AnxNode * m;

  if (tree == NULL) return NULL;

  m = tree->root;

  if (m == NULL) {
    tree->root = node;
    return tree;
  }

  while (m != NULL) {
    if (tree->cmp (ctx, node->data, m->data) < 0) {
      if (m->left != NULL) m = m->left;
      else {
	ATTACH_LEFT (m, node);
	m = NULL;
      }
    } else {
      if (m->right != NULL) m = m->right;
      else {
	ATTACH_RIGHT (m, node);
	m = NULL;
      }
    }
  }

  return tree;
}

AnxTree *
anx_tree_insert (void * ctx, AnxTree * tree, void * data)
{
  AnxNode * node = anx_node_new (data);
 
  if (tree == NULL) return NULL;

  return _anx_tree_insert (ctx, tree, node);
}

static __inline__ AnxNode *
_anx_tree_find (void * ctx, AnxTree * tree, void * data)
{
  AnxNode * n;

  n = tree->root;
  while (n) {
    if (n->data == data) return n;
  
    if (tree->cmp (ctx, data, n->data) < 0) {
      n = n->left;
    } else {
      n = n->right;
    }
  }
  
  return NULL;
}

AnxTree *
anx_tree_remove (void * ctx, AnxTree * tree, void * data)
{
  AnxNode * node = NULL;
  AnxNode * l, * r, * p, * lx;
  int was_left = 0;

  if (tree == NULL) return NULL;

  node = _anx_tree_find (ctx, tree, data);
  if (node == NULL) return NULL;

  /* Cut node out of tree */

  if ((p = node->parent)) {
    if (node == p->left) {
      was_left = 1;
      p->left = NULL;
    }
    else
      p->right = NULL;
  }

  if ((l = node->left)) {
    l->parent = NULL;
  }

  if ((r = node->right)) {
    r->parent = NULL;
  }

  if (!p && !l && !r) {
    tree->root = NULL;
  } else if (!p && !l && r) {
    tree->root = r;
  } else if (!p && l && !r) {
    tree->root = l;
  } else if (p && !l && !r) {
    /* no further action */
  } else if (p && l && !r) {
    if (was_left) {
      ATTACH_LEFT (p, l);
    } else {
      ATTACH_RIGHT (p, l);
    }
  } else if (p && !l && r) {
    if (was_left) {
      ATTACH_LEFT (p, r);
    } else {
      ATTACH_RIGHT (p, r);
    }
  } else if (!p && l && r) {
    for (lx = l; lx->right; lx = lx->right);
    ATTACH_RIGHT (lx, r);
    tree->root = l;
  } else if (p && l && r) {
    for (lx = l; lx->right; lx = lx->right);
    ATTACH_RIGHT (lx, r);
    if (was_left) {
      ATTACH_LEFT (p, l);
    } else {
      ATTACH_RIGHT (p, l);
    }
  }

  anx_free (node);

  return tree;
}

static __inline__ AnxNode *
_anx_tree_first (void * ctx, AnxTree * tree)
{
  AnxNode * m;
  for (m = tree->root; m && m->left != NULL; m = m->left);
  return m;
}

static __inline__ AnxNode *
_anx_tree_last (void * ctx, AnxTree * tree)
{
  AnxNode * m;
  for (m = tree->root; m && m->right != NULL; m = m->right);
  return m;
}

static __inline__ AnxNode *
_anx_node_prev (void * ctx, AnxTree * tree, AnxNode * node)
{
  AnxNode * m, * p;
  
  if (node->left == NULL) {
    p = node->parent;
    if (p && (p->right == node)) return p;
    else return NULL;
  }

  for (m = node->left; m->right != NULL; m = m->right);

  return m;
}

static __inline__ AnxNode *
_anx_node_next (void * ctx, AnxTree * tree, AnxNode * node)
{
  AnxNode * m, * p;
  
  if (node->right == NULL) {
    p = node->parent;
    if (p && (p->left == node)) return p;
    else return NULL;
  }

  for (m = node->right; m->left != NULL; m = m->left);

  return m;
}


/* Public functions */

AnxNode *
anx_tree_find (void * ctx, AnxTree * tree, void * data)
{
  if (tree == NULL) return NULL;
  return _anx_tree_find (ctx, tree, data);
}

AnxNode *
anx_tree_first (void * ctx, AnxTree * tree)
{
  if (tree == NULL) return NULL;

  return _anx_tree_first (ctx, tree);
}

AnxNode *
anx_tree_last (void * ctx, AnxTree * tree)
{
  if (tree == NULL) return NULL;

  return _anx_tree_last (ctx, tree);
}

AnxNode *
anx_node_prev (void * ctx, AnxTree * tree, AnxNode * node)
{
  if (tree == NULL || node == NULL) return NULL;

  return _anx_node_prev (ctx, tree, node);
}

AnxNode *
anx_node_next (void * ctx, AnxTree * tree, AnxNode * node)
{
  if (tree == NULL || node == NULL) return NULL;

  return _anx_node_next (ctx, tree, node);
}

void *
anx_node_data (void * ctx, AnxTree * tree, AnxNode * node)
{
  if (tree == NULL || node == NULL) return NULL;

  return node->data;
}

static __inline__ AnxNode *
anx_next_leaf (AnxNode * node)
{
  AnxNode * m = node;

  if (node == NULL) return NULL;

  while (1) {
    if (m->left) m = m->left;
    else if (m->right) m = m->right;
    else break;
  }

  return m;
}

AnxTree *
anx_tree_free (void * ctx, AnxTree * tree)
{
  AnxNode * n, * p;

  if (tree == NULL) return NULL;

  p = tree->root;

  while ((n = anx_next_leaf (p))) {
    if ((p = n->parent)) {
      if (p->left == n) p->left = NULL;
      else p->right = NULL;
    }
    anx_free (n);
  }

  anx_free (tree);

  return NULL;
}
