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
using namespace std;

//Local Include Files
#include <libCMMLTags/C_HumReadCMMLTag.h>
#include <libCMMLTags/C_MetaTagList.h>
#include <libCMMLTags/C_AnchorTag.h>
#include <libCMMLTags/C_ImageTag.h>
#include <libCMMLTags/C_DescTag.h>


class LIBCMMLTAGS_API C_ClipTag :
	//Derived Classes
	public C_HumReadCMMLTag
{
public:
	//Constructors
	C_ClipTag(void);
	virtual ~C_ClipTag(void);

	/// Returns the name of the track this clip belongs to.
	wstring track();

	/// Returns a pointer to the internal metatag list. Don't delete.
	C_MetaTagList* metaList();

	/// Returns a pointer to the internal anchor element. Don't delete.
	C_AnchorTag* anchor();

	/// Returns a pointer to the internal image element. Don't delete.
	C_ImageTag* image();

	/// Returns a pointer to the internal desc element. Don't delete.
	C_DescTag* desc();

	/// Returns the start time for this clip.
	wstring start();

	/// Returns the end time for this clip. May be "".
	wstring end();


	/// Set the track this clip belongs to.
	void setTrack(wstring inTrack);

	/// Set the anchor tag for this clip. You give away your pointer.
	void setAnchor(C_AnchorTag* inAnchor);

	/// Set the image tag for this clip. You give away your pointer.
	void setImage(C_ImageTag* inImage);

	/// Sets the desc tag for this clip. You give away your pointer.
	void setDesc(C_DescTag* inDesc);

	/// Sets the start time for this clip.
	void setStart(wstring inStart);

	/// Sets the end time for this clip. May be "".
	void setEnd(wstring inEnd);

	/// Converts this tag to an xml string.
	virtual wstring toString();

	/// Performs a deep copy returning a pointer you can keep.
	C_ClipTag* clone();

	/// Performs a deep copy returning a pointer to the base class you can keep.
	virtual C_CMMLTag* genericClone();

protected:
	/// Internal deep copy mechanism
	virtual void privateClone(C_CMMLTag* outTag);


	//Property Data
	wstring mTrack;
	C_MetaTagList* mMetaList;
	C_AnchorTag* mAnchor;
	C_ImageTag* mImage;
	C_DescTag* mDesc;

	wstring mStart;
	wstring mEnd;
};
