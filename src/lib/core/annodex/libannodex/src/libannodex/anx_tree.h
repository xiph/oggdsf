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

#include <annodex/anx_core.h>

#ifndef _ANX_TREE
typedef void AnxTree;
typedef void AnxNode;
#endif

/* An AnxCmpFunc compares two scalars, returning:
 *  +ve, s1 > s2
 *    0, s1 == s2
 *  -ve, s1 < s2
 */
typedef int (*AnxCmpFunc) (void * cmp_ctx, void * s1, void * s2);

AnxTree *
anx_tree_new (AnxCmpFunc cmp);

AnxTree *
anx_tree_insert (void * ctx, AnxTree * tree, void * data);

AnxTree *
anx_tree_remove (void * ctx, AnxTree * tree, void * data);

AnxNode *
anx_tree_find (void * ctx, AnxTree * tree, void * data);

AnxNode *
anx_tree_first (void * ctx, AnxTree * tree);

AnxNode *
anx_tree_last (void * ctx, AnxTree * tree);

AnxNode *
anx_node_prev (void * ctx, AnxTree * tree, AnxNode * node);

AnxNode *
anx_node_next (void * ctx, AnxTree * tree, AnxNode * node);

void *
anx_node_data (void * ctx, AnxTree * tree, AnxNode * node);

AnxTree *
anx_tree_free (void * ctx, AnxTree * tree);
