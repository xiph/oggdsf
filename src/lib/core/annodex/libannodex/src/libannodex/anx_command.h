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

#ifndef __ANX_COMMAND_H__
#define __ANX_COMMAND_H__

/* Commands */

typedef void AnxMediaChange;

typedef union _AnxCommandData AnxCommandData;

union _AnxCommandData {
  AnxMediaChange * mediachange;
  AnxClip * clip;
};

typedef enum _AnxCommandType {
  ANX_CMD_NOOP,
  ANX_CMD_INSERT_CLIP,
  ANX_CMD_CLEAR_CLIP,
  ANX_CMD_LOAD_MEDIA,
  ANX_CMD_UNLOAD_MEDIA
} AnxCommandType;


/* debug */
int anx_print_commands (ANNODEX * annodex);

AnxCommandData
anx_command_get_data (ANNODEX * annodex, AnxCommand command);

AnxCommandData
anx_command_set_data (ANNODEX * annodex, AnxCommand command,
		      AnxCommandData data);

double
anx_command_get_time_offset (ANNODEX * annodex, AnxCommand command);

long
anx_command_set_time_offset (ANNODEX * annodex, AnxCommand command,
			     double time_offset);

AnxCommandType
anx_command_get_type (ANNODEX * annodex, AnxCommand command);

AnxCommandType
anx_command_set_type (ANNODEX * annodex, AnxCommand command,
		      AnxCommandType type);

AnxCommand
anx_command_remove (ANNODEX * annodex, AnxCommand command);

AnxCommand
anx_command_first (ANNODEX * annodex);

AnxCommand
anx_command_next (ANNODEX * annodex, AnxCommand command);

AnxCommand
anx_command_prev (ANNODEX * annodex, AnxCommand command);

#endif /* __ANX_COMMAND_H__ */
