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
#pragma once

//STL Include Files
#include <string>
#include <vector>
using namespace std;

//Local Include Files
#include <libCMMLTags/C_MappedTag.h>

class LIBCMMLTAGS_API C_MappedTagList
{
public:
	C_MappedTagList(void);
	virtual ~C_MappedTagList(void);

	//What to do about this ? **** They are now protected dervied classes implement themselves
	//void addTag(wstring inName, wstring inContent);
	
	//void removeTag ???
	unsigned long numTags();

	wstring getContent(wstring mName);

	//Maybe not !
	//void addTag(wstring inName, wstring inContent);

	virtual wstring toString() = 0;
	


protected:
	vector<C_MappedTag*> mTagList;

	C_MappedTag* getTag(unsigned long inTagNo);
	C_MappedTag* getTag(wstring mName);
	void addTag(C_MappedTag* inTag);
	void emptyList();


	//Protected Helper Methods
	virtual void privateClone(C_MappedTagList* outTagList);


};
