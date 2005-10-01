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
#include "oggdllstuff.h"
#include <winsock.h>
#include "HTTPSocket.h"
#include "SingleMediaFileCache.h"
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

class OGG_DEMUX_API HTTPFileSource
	:	public IFilterDataSource
	,	public CAMThread
	,	protected HTTPSocket	
{
public:
	HTTPFileSource(void);
	virtual ~HTTPFileSource(void);

	//Thread commands
	static const int THREAD_RUN = 0;
	static const int THREAD_EXIT = 1;

	//IFilterDataSource Interface
	virtual unsigned long seek(unsigned long inPos);
	virtual void close() ;
	virtual bool open(string inSourceLocation);
	virtual void clear();
	virtual bool isEOF();
	virtual bool isError();
	virtual unsigned long read(char* outBuffer, unsigned long inNumBytes);
	virtual string shouldRetryAt();

	//CAMThread pure virtuals
	DWORD HTTPFileSource::ThreadProc();

protected:
	void unChunk(unsigned char* inBuff, unsigned long inNumBytes);
	unsigned short getHTTPResponseCode(string inHTTPResponse);
	bool startThread();
	void DataProcessLoop();

	SingleMediaFileCache mFileCache;

	bool mIsChunked;
	unsigned long mChunkRemains;

	bool mIsFirstChunk;
	string mRetryAt;

	fstream debugLog;
	fstream fileDump;
	fstream rawDump;

	unsigned char* mInterBuff;
	unsigned long mNumLeftovers;
	static	const unsigned long RECV_BUFF_SIZE = 1024;

	CCritSec* mBufferLock;
};
