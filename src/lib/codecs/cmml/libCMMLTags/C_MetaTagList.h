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
#include <libCMMLTags/C_MappedTagList.h>
#include <libCMMLTags/C_MetaTag.h>

class LIBCMMLTAGS_API C_MetaTagList
	: public C_MappedTagList
{
public:
	C_MetaTagList(void);
	virtual ~C_MetaTagList(void);

	/// Empties the list.
	void emptyList();

	/// Adds a tag to the list. You give away your pointer.
	void addTag(C_MetaTag* inTag);

	/// Adds a tag to this list by specifying name and content.
	void addTag(wstring inName, wstring inContent);

	/// Gets the indexed tag from the list. You can modify but not delete the pointer.
	C_MetaTag* getTag(unsigned long inTagNo);

	/// Returns a pointer to the tag with this name. You can modify but not delete.
	C_MetaTag* getTag(wstring inName);

	/// Gets the matching content from the tag with given name.
	wstring getContent(wstring inName);
	
	/// Returns an xml representation of this tag.
	virtual wstring toString();

	/// Performs a deep copy and returns a pointer you can keep.
	C_MetaTagList* clone();
protected:
	//Protected Helper Methods
	virtual void privateClone(C_MappedTagList* outTagList);

};
