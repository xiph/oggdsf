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

//This class will be a cache of a single media file.
//It will only allow a single chunk of data to be cached...
// ie you can't cache bytes 0-1000 and 2000-3000...
// only consecutive blocks for now.
//
//Data can be read randomly... but only written sequentially.
//Will act as a buffer so that data read off the network can be put straight
// into the file and then read as needed.

#include <string>
#include <fstream>
using namespace std;
class OGG_DEMUX_API SingleMediaFileCache
{
public:
	SingleMediaFileCache(void);
	~SingleMediaFileCache(void);

	bool open(string inFileName);
	void close();
	bool write(const unsigned char* inBuff, unsigned long inBuffSize);
	unsigned long read(unsigned char* outBuff, unsigned long inBuffSize);
	bool readSeek(unsigned long inSeekPos);
	unsigned long totalBytes();
	unsigned long bytesAvail();

protected:
	fstream mLocalFile;

	unsigned long mBytesWritten;
	unsigned long mReadPtr;
	
	//fstream debugLog;
	bool mIsComplete;
};
