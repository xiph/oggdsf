//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
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

#pragma once
#include "VorbisComments.h"
class LIBVORBISCOMMENT_API StreamCommentInfo
{
public:
	StreamCommentInfo(void);
	~StreamCommentInfo(void);

	enum eCodecIDs {
		NO_CODEC = 0,
		VORBIS = 1,
		THEORA = 2

	};
	VorbisComments* comments();
	void setComments(VorbisComments* inComments);

	LOOG_INT64 pageStart();
	void setPageStart(LOOG_INT64 inPageStart);

	unsigned short codecID();
	void setCodecID(unsigned short inCodecID);

	bool isDirty();
	void setIsDirty(bool inIsDirty);

	unsigned long majorStreamNo();
	void setMajorStreamNo(unsigned long inMajorStreamNo);

	unsigned long minorStreamNo();
	void setMinorStreamNo(unsigned long inMinorStreamNo);

protected:
	VorbisComments* mComments;
	LOOG_INT64 mPageStart;
	unsigned short mCodecID;

	bool mIsDirty;

	unsigned long mMajorStreamNo;
	unsigned long mMinorStreamNo;
};
