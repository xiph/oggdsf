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

/*
 * Consider the following diagram of commands; command types are
 * 1, 2, 3, ... 8 and time (temporal scheduling) flows downwards.
 *
 *     1       2       3       4       5       6       7       8
 *     |       |       |       |       |       |       |       |
 *     |       A       |       |       |       |       |       |
 *     |       |`-----------------------------.|       |       |
 *     |       |       |       |       |       B       |       |
 *     |       |       |       |,-------------'|       |       |
 *     |       |       |       C       |       |       |       |
 *     |       |       |       |`-----------------------------.|
 *     |       |       |       |       |       |       |       D
 *     |       |       |,-------------------------------------'|
 *     |       |       E       |       |       |       |       |
 *     |       |,-----'|       |       |       |       |       |
 *     |       F       |       |       |       |       |       |
 *     |       |`---------------------.|       |       |       |
 *     |       |       |       |       G       |       |       |
 *     |       |,---------------------'|       |       |       |
 *     |       H       |       |       |       |       |       |
 *     |       |`-------------------------------------.|       |
 *     |       |       |       |       |       |       I       |
 *     |,---------------------------------------------'|       |
 *     J       |       |       |       |       |       |       |
 *     |`-----.|       |       |       |       |       |       |
 *     |       K       |       |       |       |       |       |
 *     |       |`-------------.|       |       |       |       |
 *     |       |       |       L       |       |       |       |
 *     |       |       |       |       |       |       |       |
 *     |       |       |       |       |       |       |       |
 *
 * AnxCommands are stored in temporally sorted order (A, B, C, ... L)
 * in a sorted binary tree (AnxTree). Additionally, each node of this
 * tree contains prev and next pointers to the previous and next
 * commands of the same type, forming a doubly linked list for each
 * type, here (J), (A, F, H, K), (E), (C, L), (G), (B), (I), (D).
 *
 *     |       |       |       |       |       |       |       |
 *     |       |       |       L       |       |       |       |
 *     |       |       |       |`-----.|       |       |       |
 *     |       N,---------------------'M       |       O       |
 *     |       |`-------------------------------------'|`-----.|
 *     |       |       |       |       |       |       |       P
 *     |       |       |       |       |       |       |       |
 *
*/

#include "config.h"
#include "anx_compat.h"

#define _ANX_COMMAND

typedef struct _AnxCommand * AnxCommand;

#include "anx_private.h"

/*#define DEBUG*/

struct _AnxCommand {
  AnxCommand type_prev;
  AnxCommand type_next;
  double time_offset;
  AnxCommandType type;
  AnxCommandData data;
};

static AnxCommand
anx_command_new (ANNODEX * annodex, AnxCommandType type,
		 AnxCommandData data)
{
  AnxCommand c;

  c = (AnxCommand) anx_malloc (sizeof (*c));
  c->type = type;
  c->data = data;
  c->time_offset = -1.0;
  c->type_prev = NULL;
  c->type_next = NULL;

  return c;
}

/*
 * AnxCmpFunc to compare nodes for tree sorting
 */
static int
anx_command_cmp (ANNODEX * annodex, AnxCommand c1, AnxCommand c2)
{
  if (c1->time_offset > c2->time_offset) return 1;
  else if (c1->time_offset < c2->time_offset) return -1;
  else return 0;
}

ANNODEX *
anx_commands_init (ANNODEX * annodex)
{
  if (annodex == NULL) return NULL;

  if (annodex->commands != NULL) return annodex;

  annodex->commands = anx_tree_new ((AnxCmpFunc)anx_command_cmp);
  annodex->pending_command = NULL;

  return annodex;
}

int
anx_print_commands (ANNODEX * annodex)
{
  char buf[1024];
  AnxTree * cs;
  AnxNode * n;
  AnxCommand c;
  AnxCommandType type;
  AnxCommandData data;
  double time_offset;

  cs = annodex->commands;

  for (n = anx_tree_first (annodex, cs); n;
       n = anx_node_next (annodex, cs, n)) {

    c = (AnxCommand) anx_node_data (annodex, cs, n);;
    type = anx_command_get_type (annodex, c);
    time_offset = anx_command_get_time_offset (annodex, c);
    data = anx_command_get_data (annodex, c);

    if (time_offset != -1.0) {
      printf ("Command at %f:\n", time_offset);
    } else {
      printf ("Command at unknown time!!\n");
    }
    
    switch (type) {
    case ANX_CMD_NOOP:
      printf ("NOOP\n");
      break;
    case ANX_CMD_INSERT_CLIP:
      anx_clip_snprint (buf, 1024, data.clip, -1.0, -1.0);
      puts (buf);
      break;
    case ANX_CMD_CLEAR_CLIP:
      printf ("CLEAR CLIP\n");
      break;
    case ANX_CMD_LOAD_MEDIA:
      printf ("LOAD MEDIA\n");
      break;
    case ANX_CMD_UNLOAD_MEDIA:
      printf ("UNLOAD MEDIA\n");
      break;
    default:
      printf ("Unknown command!\n");
      break;
    }

    printf ("\n");
  }

  return 0;
}

/* 
 * fix up the links for a type type
 */
static ANNODEX *
anx_commands_fix_type (ANNODEX * annodex, AnxCommandType type)
{
  AnxTree * cs;
  AnxNode * n;
  AnxCommand c, cp = NULL;
  

  if (annodex == NULL) return NULL;
  if (annodex->commands == NULL) return annodex;

  cs = annodex->commands;

  for (n = anx_tree_first (annodex, cs); n;
       n = anx_node_next (annodex, cs, n)) {
    c = (AnxCommand)anx_node_data (annodex, cs, n);
    if (c->type == type) {
      if (cp) cp->type_next = c;
      c->type_prev = cp;
      cp = c;
    }
  }
  if (cp) cp->type_next = NULL;

  return annodex;
}

AnxCommand
anx_command_seek (ANNODEX * annodex, double current_offset)
{
  AnxTree * cs;
  AnxNode * n;
  double c_offset;
  AnxCommand command;

  if (annodex == NULL) return NULL;

  cs = annodex->commands;

  annodex->pending_command = NULL;

  for (n = anx_tree_first (annodex, cs); n;
       n = anx_node_next (annodex, cs, n)) {
    command = (AnxCommand) anx_node_data (annodex, cs, n);
    c_offset = anx_command_get_time_offset (annodex, command);
#ifdef DEBUG
    printf ("command_seek: [%f]  %f ?\n", current_offset, c_offset);
#endif
    if (current_offset <= c_offset) {
      annodex->pending_command = command;
      break;
    }
  }

  return annodex->pending_command;
}

#if 0 /* XXX: crack */
static AnxCommand
anx_command_seek_after (ANNODEX * annodex, double current_offset,
			AnxCommand acommand)
{
  AnxTree * cs;
  AnxNode * n;
  double c_offset;
  AnxCommand command;

  if (annodex == NULL) return NULL;

  if (acommand == NULL) return anx_command_seek (annodex, current_offset);

  cs = annodex->commands;

  annodex->pending_command = NULL;

  n = anx_tree_find (annodex, cs, acommand);

  for (n = anx_node_next (annodex, cs, n); n;
       n = anx_node_next (annodex, cs, n)) {
    command = (AnxCommand) anx_node_data (annodex, cs, n);
    c_offset = anx_command_get_time_offset (annodex, command);
    if (current_offset <= c_offset) {
      annodex->pending_command = command;
      break;
    }
  }

  return annodex->pending_command;
}
#endif

static AnxCommand
anx_command_insert (ANNODEX * annodex, AnxCommand command, double time_offset)
{
  double current_offset, pending_offset;

#ifdef DEBUG
  printf ("anx_command_insert: time_offset = %f\n", time_offset);
#endif

  command->time_offset = time_offset;
  
  if (annodex->commands == NULL) {
    anx_commands_init (annodex);
  }

  anx_tree_insert (annodex, annodex->commands, command);
  anx_commands_fix_type (annodex, command->type);

  current_offset = anx_track_granules_to_time (annodex, annodex->anno_serialno,
					       anx_tell (annodex));
  pending_offset =
    anx_command_get_time_offset (annodex, annodex->pending_command);

#ifdef DEBUG
  printf ("anx_command_insert: pending_offset = %f\n", pending_offset);
#endif

  if ((pending_offset == -1) ||
      (current_offset <= time_offset && time_offset < pending_offset)) {
#if 0
    anx_command_seek_after (annodex, current_offset,
			    annodex->current_command);
#else
    annodex->pending_command = command;
#endif
  }

  return command;
}

static AnxCommand
anx_command_add (ANNODEX * annodex, AnxCommandType type, double time_offset,
		 AnxCommandData data)
{
  AnxCommand c;
  AnxCommandData new_data;

#ifdef DEBUG
  printf ("anx_command_add: type = %d, time = %f\n", (int)type, time_offset);
#endif

  switch (type) {
  case ANX_CMD_NOOP:
    new_data.clip = NULL;
    break;
  case ANX_CMD_INSERT_CLIP:
    new_data.clip = anx_clip_clone (data.clip);
    break;
  case ANX_CMD_CLEAR_CLIP:
    new_data.clip = NULL;
    break;
  case ANX_CMD_LOAD_MEDIA:
    new_data.mediachange = NULL; /* XXX */
    break;
  case ANX_CMD_UNLOAD_MEDIA:
    new_data.mediachange = NULL;
    break;
  default:
    new_data.clip = NULL;
    break;
  }

  c = anx_command_new (annodex, type, new_data);
  anx_command_insert (annodex, c, time_offset);

  return (AnxCommand)c;
}

int
anx_insert_clip (ANNODEX * annodex, double at_time, AnxClip * clip)
{
  AnxCommandData data;

  data.clip = clip;

#ifdef DEBUG
  printf ("anx_insert_clip (%p) @%f\n", (void *)clip, at_time);
#endif

  if (clip == NULL) {
    anx_command_add (annodex, ANX_CMD_CLEAR_CLIP, at_time, data);
  } else {
    anx_command_add (annodex, ANX_CMD_INSERT_CLIP, at_time, data);
  }

  return 0;
}

AnxCommandData
anx_command_get_data (ANNODEX * annodex, AnxCommand command)
{
  AnxCommandData dummy;

  if (command == NULL) {
    dummy.clip = NULL;
    return dummy;
  }
  return command->data;
}

AnxCommandData
anx_command_set_data (ANNODEX * annodex, AnxCommand command,
		      AnxCommandData data)
{
  AnxCommand c = (AnxCommand)command;
  AnxCommandData old, dummy;

  if (command == NULL) {
    dummy.clip = NULL;
    return dummy;
  }

  old = c->data;
  c->data = data;
  return old;
}

double
anx_command_get_time_offset (ANNODEX * annodex, AnxCommand command)
{
  if (command == NULL) return -1.0;
  return command->time_offset;
}

long
anx_command_set_time_offset (ANNODEX * annodex, AnxCommand command,
			     double time_offset)
{
  long old;

  if (command == NULL) return -1;

  old = (long) command->time_offset;

  anx_tree_remove (annodex, annodex->commands, command);
  command->time_offset = time_offset;
  anx_tree_insert (annodex, annodex->commands, command);
  anx_commands_fix_type (annodex, command->type);

  return old;
}

AnxCommandType
anx_command_get_type (ANNODEX * annodex, AnxCommand command)
{
  if (command == NULL) return (AnxCommandType)-1;
  return command->type;
}

AnxCommandType
anx_command_set_type (ANNODEX * annodex, AnxCommand command,
		      AnxCommandType type)
{
  AnxCommandType old;

  if (command == NULL) return (AnxCommandType)-1;

  old = command->type;
  command->type = type;

  anx_commands_fix_type (annodex, old);
  anx_commands_fix_type (annodex, type);

  return old;
}

AnxCommand
anx_command_remove (ANNODEX * annodex, AnxCommand command)
{
  AnxCommandType type;
  double current_offset;

  if (command == NULL) return (AnxCommand)NULL;

  type = command->type;

  anx_tree_remove (annodex, annodex->commands, command);
  anx_commands_fix_type (annodex, type);

  current_offset = anx_track_granules_to_time (annodex, annodex->anno_serialno,
					       anx_tell (annodex));
  anx_command_seek (annodex, current_offset);

  return NULL;
}

AnxCommand
anx_command_next (ANNODEX * annodex, AnxCommand command)
{
  AnxNode * m, * n;

  if (annodex == NULL) return NULL;

  m = anx_tree_find (annodex, annodex->commands, command);
  n = anx_node_next (annodex, annodex->commands, m);
  return (AnxCommand) anx_node_data (annodex, annodex->commands, n);
}

AnxCommand
anx_command_prev (ANNODEX * annodex, AnxCommand command)
{
  AnxNode * m, * n;

  if (annodex == NULL) return NULL;

  m = anx_tree_find (annodex, annodex->commands, command);
  n = anx_node_prev (annodex, annodex->commands, m);
  return (AnxCommand) anx_node_data (annodex, annodex->commands, n);
}
