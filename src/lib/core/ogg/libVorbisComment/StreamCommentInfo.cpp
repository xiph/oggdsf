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

#include "StdAfx.h"
#include ".\streamcommentinfo.h"

StreamCommentInfo::StreamCommentInfo(void)
	:	mIsDirty(false)
	,	mPageStart(0)
	,	mCodecID(NO_CODEC)
	,	mComments(NULL)
{
}

StreamCommentInfo::~StreamCommentInfo(void)
{
	delete mComments;
}


VorbisComments* StreamCommentInfo::comments() {
	return mComments;
}
void StreamCommentInfo::setComments(VorbisComments* inComments) {
	mComments = inComments;
}
__int64 StreamCommentInfo::pageStart() {
	return mPageStart;
	
}
void StreamCommentInfo::setPageStart(__int64 inPageStart) {
	mPageStart = inPageStart;
}
unsigned short StreamCommentInfo::codecID() {
	return mCodecID;
}
void StreamCommentInfo::setCodecID(unsigned short inCodecID) {
	mCodecID = inCodecID;
}

bool StreamCommentInfo::isDirty() {
	return mIsDirty;
}
void StreamCommentInfo::setIsDirty(bool inIsDirty) {
	mIsDirty = inIsDirty;
}

unsigned long StreamCommentInfo::majorStreamNo() {
	return mMajorStreamNo;
}
void StreamCommentInfo::setMajorStreamNo(unsigned long inMajorStreamNo) {
	mMajorStreamNo = inMajorStreamNo;
}

unsigned long StreamCommentInfo::minorStreamNo() {
	return mMinorStreamNo;
}
void StreamCommentInfo::setMinorStreamNo(unsigned long inMinorStreamNo) {
	mMinorStreamNo = inMinorStreamNo;
}