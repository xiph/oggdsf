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
#include "stdafx.h"
#include ".\singlemediafilecache.h"

SingleMediaFileCache::SingleMediaFileCache(void)
	:	mBytesWritten(0)
	,	mIsComplete(false)
	,	mReadPtr(0)
{
	//debugLog.open("G:\\logs\\mediacache.log", ios_base::out);
}

SingleMediaFileCache::~SingleMediaFileCache(void)
{
	//debugLog.close();
}

bool SingleMediaFileCache::open(string inFileName) {
	mBytesWritten = 0;
	//debugLog<<"Opening "<<inFileName<<endl;
	mLocalFile.open(inFileName.c_str(), ios_base::in|ios_base::out|ios_base::binary|ios_base::trunc);

	if (mLocalFile.is_open()) {
		//debugLog<<"File open...."<<endl;
	} else {
		//debugLog<<"File open FAILED"<<endl;
	}
	return mLocalFile.is_open();
}
void SingleMediaFileCache::close() {
	mLocalFile.close();
	
}
bool SingleMediaFileCache::write(const unsigned char* inBuff, unsigned long inBuffSize) {
	//debugLog<<"Writeing "<<inBuffSize<<endl;
	//debugLog<<"Read Ptr = "<<mLocalFile.tellg();
	//debugLog<<"Write Ptr = "<<mLocalFile.tellp();
	mLocalFile.seekp(0, ios_base::end);
	if (inBuffSize != 0) {
		mLocalFile.write((const char*)inBuff, inBuffSize);
		mBytesWritten += inBuffSize;
	}

	if (mLocalFile.fail()) {
		//debugLog<<"*** Write put into FAIL"<<endl;
	}
	return !(mLocalFile.fail());
}
unsigned long SingleMediaFileCache::read(unsigned char* outBuff, unsigned long inBuffSize) {
	//debugLog<<"Read request for "<<inBuffSize<<"  got ";
	//debugLog<<"Read Ptr = "<<mLocalFile.tellg();
	//debugLog<<"Write Ptr = "<<mLocalFile.tellp();
	mLocalFile.seekg(mReadPtr);
	unsigned long locBytesAvail = bytesAvail();
	if (locBytesAvail >= inBuffSize) {
        mLocalFile.read((char*)outBuff, inBuffSize);
        //debugLog<<mLocalFile.gcount()<<endl;
		mReadPtr+=mLocalFile.gcount();
		return mLocalFile.gcount();
	} else if (locBytesAvail > 0) {
		mLocalFile.read((char*)outBuff, locBytesAvail);
		//debugLog<<locBytesAvail<<endl;
		mReadPtr += locBytesAvail;
		return locBytesAvail;
	} else {
		//debugLog << "NOTHING"<<endl;
		return 0;
	}
	
}
bool SingleMediaFileCache::readSeek(unsigned long inSeekPos) {
	if (inSeekPos < mBytesWritten) {
		//debugLog<<"Seeking to "<<inSeekPos<<endl;
		mReadPtr = inSeekPos;
		return true;
	} else {
		return false;
	}
}

unsigned long SingleMediaFileCache::totalBytes() {
	return mBytesWritten;
}
unsigned long SingleMediaFileCache::bytesAvail() {
	if (mLocalFile.fail()) {
		//debugLog<<"bytesAvail : File is in fail state"<<endl;
	}
	//debugLog<<"bytesAvail : Byteswritten = "<<mBytesWritten<<endl;
	//if ((!mLocalFile.fail()) && (mBytesWritten > 0)) {
	if (mBytesWritten > 0) {
		//debugLog<<"bytes Avail = "<<mBytesWritten - mReadPtr<<endl;
		return mBytesWritten - mReadPtr;
	} else {
		return 0;
	}
}
