//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
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
#include "oggseektable.h"
#include "dllstuff.h"
#include "IOggCallback.h"
#include "OggDataBuffer.h"
#include "FLACMath.h"
#include "math.h"
#include <fstream>
using namespace std;
class LIBOOOGGSEEK_API AutoOggSeekTable 
	:	public OggSeekTable
	,	public IOggCallback
{
public:
	AutoOggSeekTable(string inFileName);
	~AutoOggSeekTable(void);

	static const __int64 DS_UNITS = 10000000;
	static const LINT_MAX = 4294967295;

	bool buildTable();

	//IOggCallback interface
	bool acceptOggPage(OggPage* inOggPage);

	__int64 fileDuration();

protected:
	unsigned long mFilePos;
	unsigned long mPacketCount;
	unsigned long mSampleRate;
	unsigned long mNumHeaders;

	unsigned long mSerialNoToTrack;
	unsigned long mGranulePosShift;
	bool mLastIsSeekable;
	bool isTheora;
	__int64 mLastSeekTime;
	__int64 mFileDuration;
	fstream mFile;
	string mFileName;
	OggDataBuffer mOggDemux;

};