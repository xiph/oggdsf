/*
   Copyright (C) 2003 Zentaro Kavanagh
   
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
#pragma once

//STL Include Files
#include <string>
using namespace std;

//Local Include Files
#include "C_CMMLTag.h"
#include "C_HumReadCMMLTag.h"
#include "C_StreamTag.h"
#include "C_HeadTag.h"
#include "C_ClipTagList.h"

class LIBCMMLTAGS_API C_CMMLRootTag :
	//Derived Classes
	public C_HumReadCMMLTag
{
public:
	//Constructors
	C_CMMLRootTag(void);
	virtual ~C_CMMLRootTag(void);

	//Accessors
	
	C_StreamTag* stream();
	C_HeadTag* head();
	C_ClipTagList* clipList();

	//Mutators
	void setStream(C_StreamTag* inStreamTag);
	void setHead(C_HeadTag* inHeadTag);
	void C_CMMLRootTag::setClipList(C_ClipTagList* inClipList);

	//Other
	virtual string toString();
	virtual C_CMMLTag* genericClone();
	C_CMMLRootTag* clone();

protected:
	//Property Data
	
	C_StreamTag* mStream;
	C_HeadTag* mHead;
	C_ClipTagList* mClipList;

	//Protected Helper Methods
	virtual void privateClone(C_CMMLTag* outTag);

};
