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
#include "libVorbisComment.h"
#include <string>
#include <vector>

using namespace std;
#include "SingleVorbisComment.h"
#include <libOOOgg/OggPacket.h>
#include <libilliCore/iLE_Math.h>

class LIBVORBISCOMMENT_API VorbisComments
{
public:
	VorbisComments(void);
	~VorbisComments(void);

	string vendorString();
	bool setVendorString(string inVendorString);

	unsigned long numUserComments();
	SingleVorbisComment* getUserComment(unsigned long inIndex);
	
	vector<SingleVorbisComment*> getCommentsByKey(string inKey);

	bool addComment(SingleVorbisComment* inComment);
	bool addComment(string inKey, string inValue);

	bool parseOggPacket(OggPacket* inPacket, unsigned long inStartOffset);
	OggPacket* toOggPacket(unsigned char* inPrefixBuff, unsigned long inPrefixBuffSize);
	string toString();

	unsigned long size();
protected:
	string mVendorString;
	vector<SingleVorbisComment*> mCommentList;
};
