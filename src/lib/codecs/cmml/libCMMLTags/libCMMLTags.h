/*
   Copyright (C) 2003, 2004 Zentaro Kavanagh
   
   Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
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

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBCMMLTAGS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBCMMLTAGS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBCMMLTAGS_EXPORTS
#define LIBCMMLTAGS_API __declspec(dllexport)
#else
#define LIBCMMLTAGS_API __declspec(dllimport)
#endif


#include "config.h"

// These classes are exported from the cpp_lib_cmml.dll
#include "C_AnchorTag.h"
#include "C_BaseTag.h"
#include "C_ClipTag.h"
#include "C_ClipTagList.h"
#include "C_CMMLDoc.h"
#include "C_CMMLPreamble.h"
#include "C_CMMLRootTag.h"
#include "C_CMMLTag.h"
#include "C_DescTag.h"
#include "C_HeadTag.h"
#include "C_HumReadCMMLTag.h"
#include "C_ImageTag.h"
#include "C_ImportTag.h"
#include "C_ImportTagList.h"
#include "C_Int64.h"				//Remove ??
#include "C_MappedTag.h"
#include "C_MappedTagList.h"
#include "C_MetaTag.h"
#include "C_MetaTagList.h"
#include "C_ParamTag.h"
#include "C_ParamTagList.h"
#include "C_StreamTag.h"
#include "C_TagList.h"
#include "C_TextFieldTag.h"
#include "C_TitleTag.h"

