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
#include "StreamHeaders.h"
#include <libOOOgg/IOggCallback.h>
#include <libOOOgg/IStampedOggPacketSink.h>
#include <libOOOgg/OggPacketiser.h>

#include <libOOOgg/OggPage.h>
#include <fstream>
using namespace std;


class OggDemuxSourceFilter;
class OggDemuxSourcePin;
class StreamHeaders;

class OGG_DEMUX_API OggStream
	:	public IOggCallback
	,	protected IStampedOggPacketSink

{
public:
	OggStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek);
	virtual ~OggStream(void);

	//more hacks
	virtual void setLastEndGranPos(__int64 inGranPos);
	unsigned long serialNo();
	void setSerialNo(unsigned long inSerialNo);
	//virtual bool IdentifyCodec(OggPacket* inOggPacket) = 0;
	
	virtual bool InitCodec(StampedOggPacket* inOggPacket) = 0;
	virtual BYTE* getFormatBlock() = 0;
	virtual unsigned long getFormatBlockSize() = 0;

	virtual GUID getMajorTypeGUID() = 0;
	virtual GUID getFormatGUID() = 0;
	virtual GUID getSubtypeGUID() = 0;
	virtual wstring getPinName() = 0;
	virtual bool createFormatBlock() = 0;

	virtual LONGLONG getCurrentPos() = 0;

	virtual unsigned long getNumBuffers() = 0;
	virtual unsigned long getBufferSize() = 0;

	virtual unsigned long numCodecHeaders();
	void setSendExcess(bool inSendExcess);

	virtual CMediaType* createMediaType(GUID inMajorType, GUID inSubType, GUID inFormatType, unsigned long inFormatSize, BYTE* inFormatBlock);

	virtual bool dispatchPacket(StampedOggPacket* inPacket);
	virtual bool deliverCodecHeaders();

	//IOggcalback Interface
	virtual bool acceptOggPage(OggPage* inOggPage);   //Beware if you override this further

	virtual OggDemuxSourcePin* getPin();
	virtual bool OggStream::AddPin();

	void setAllowDispatch(bool inAllowDispatch);
	bool streamReady();
	
	
	void flush();
	void flush(unsigned short inNumPacketsToIgnore);
protected:
	virtual bool acceptStampedOggPacket(StampedOggPacket* inPacket);
	
	//virtual bool processPacket(StampedOggPacket* inPacket);
	virtual bool processHeaderPacket(StampedOggPacket* inPacket);
	virtual bool processDataPacket(StampedOggPacket* inPacket);
	//virtual bool processExcessPacket(StampedOggPacket* inPacket);

	OggPacketiser mPacketiser;
	//StampedOggPacket* mPartialPacket;
	StreamHeaders* mCodecHeaders;
	vector<StampedOggPacket*> mExcessPackets;
	unsigned long mSerialNo;
	signed long mNumHeadersNeeded;
	bool mStreamReady;
	bool mFirstRun;
	bool mSendExcess;
	bool mAllowSeek;
	bool mAllowDispatch;

	__int64 mLastEndGranulePos;
	__int64 mLastStartGranulePos;

	OggDemuxSourcePin* mSourcePin;
	OggDemuxSourceFilter* mOwningFilter;
	CCritSec* mStreamLock;

	//fstream debugLog;
};
