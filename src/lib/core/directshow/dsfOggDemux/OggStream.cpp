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
#include "oggstream.h"

OggStream::OggStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek)
	:	mCodecHeaders(NULL)
	,	mOwningFilter(inOwningFilter)
	,	mAllowSeek(inAllowSeek)
	,	mSerialNo(0)
	,	mSourcePin(NULL)
	,	mNumHeadersNeeded(0)
	,	mStreamReady(false)
	//,	mPartialPacket(NULL)
	,	mFirstRun(true)
	,	mSendExcess(true)
	,	mLastEndGranulePos(0)
	,	mLastStartGranulePos(0)
	,	mStreamLock(NULL)
	,	mAllowDispatch(false)
{
	
	//Need to do something here !
	mSerialNo = inBOSPage->header()->StreamSerialNo();
	//string locLogName = "G:\\logs\\oggstream" + StringHelper::numToString(mSerialNo) + ".log";
	//debugLog.open(locLogName.c_str(), ios_base::out);
	mStreamLock = new CCritSec;
	//This may need to be moved to derived class
	//Yep, Sure did !
	//InitCodec(inBOSPage->getPacket(0));

	//Set the packetisers callback to our acceptstampedoggpacket function.
	mPacketiser.setPacketSink(this);

}

OggStream::~OggStream(void)
{
	//debugLog<<"Destructor..."<<endl;
	//debugLog.close();
	delete mSourcePin;


	delete mCodecHeaders;
	//delete mPartialPacket;
	delete mStreamLock;
	//Don't try to delete owning filter !!
}

bool OggStream::streamReady() {
	return mStreamReady;

}

unsigned long OggStream::serialNo() {
	return mSerialNo;
}
void OggStream::setAllowDispatch(bool inAllowDispatch) {
	mAllowDispatch = inAllowDispatch;
}
bool OggStream::acceptStampedOggPacket(StampedOggPacket* inPacket) {
    if (!mStreamReady) {
		//Streams not ready, still headers.
		//Stream is ready when all header packets are seen.
		// Any data packets the are on the same page as the headers are excess packets. ???? Maybe not any more.
		processHeaderPacket(inPacket);

		if(mNumHeadersNeeded <= 0) {
			//Make sure we create the pin if we got everything already
			AddPin();
			//Add pin will set streamready to true if it was all good.
		}

		//This branch we keep the incoming packet so don't delete
	} else {
		//Data packets...
		if (mAllowDispatch) {
			if (mFirstRun) {
				mFirstRun = false;
				//debugLog<<"Delviering codec headers..."<<endl;
				//Deliver the header data
				deliverCodecHeaders();
			}		
			
			processDataPacket(inPacket);
			delete inPacket;
		} else {
			delete inPacket;
			return false;
		}
	}
	
	return true;
}


//ANX::: Need to override here to ensure the anxdata header isn't passed through.
bool OggStream::processHeaderPacket(StampedOggPacket* inPacket) {
	//FIX::: Return values

	//We don't delete the packet... the codecheader list will delete when it's done.
	//StampedOggPacket* locPacket = processPacket(inPacket);
	if (inPacket != NULL) {
		//We got a comlpete packet
		//debugLog<<"Adding codec header..."<<endl;
		mCodecHeaders->addPacket(inPacket);
		mNumHeadersNeeded--;
		//debugLog<<"Headers still needed = "<<mNumHeadersNeeded<<endl;
	}
	return true;
}

bool OggStream::processDataPacket(StampedOggPacket* inPacket) {
	//We must delete the packet we get back when we have finished
	//StampedOggPacket* locPacket = processPacket(inPacket);
	if (inPacket != NULL) {
		dispatchPacket(inPacket);
		//delete locPacket;
		inPacket = NULL;
	}

	return true;
}


OggDemuxSourcePin* OggStream::getPin() {
	return mSourcePin;
}


CMediaType* OggStream::createMediaType(GUID inMajorType, GUID inSubType, GUID inFormatType, unsigned long inFormatSize, BYTE* inFormatBlock) {
	AM_MEDIA_TYPE locAMMediaType;
	locAMMediaType.majortype = inMajorType;

	locAMMediaType.subtype = inSubType;
	locAMMediaType.formattype = inFormatType;
	locAMMediaType.cbFormat = inFormatSize; //0;//sizeof(sSpeexFormatBlock);
	locAMMediaType.pbFormat = inFormatBlock; //(BYTE*)locSpeexFormatInfo;
	locAMMediaType.pUnk = NULL;
	
			
	//LEAK CHECK::: Does this get deleted ?
	return new CMediaType(locAMMediaType);
}

unsigned long OggStream::numCodecHeaders() {
	//TODO::: Check for null.
	//debugLog<<"Num codec headers = "<<mCodecHeaders->numPackets()<<endl;
	return mCodecHeaders->numPackets();
}
void OggStream::flush() {
	CAutoLock locLock(mStreamLock);
	//delete mPartialPacket;
	//TODO::: Tell the packetiser to flush.
	//mPartialPacket = NULL;
	mPacketiser.reset();
}

void OggStream::flush(unsigned short inNumPacketsToIgnore) {
	CAutoLock locLock(mStreamLock);
	//delete mPartialPacket;
	//TODO::: Tell the packetiser to flush.
	//mPartialPacket = NULL;
	//debugLog<<"Flush and ignore "<<inNumPacketsToIgnore<<endl;
	mPacketiser.reset();
	mPacketiser.setNumIgnorePackets(inNumPacketsToIgnore);
}
//ANX::: Need to override here to create anx pins
bool OggStream::AddPin() {
	createFormatBlock();
	CMediaType* locMediaType = createMediaType(	getMajorTypeGUID(),
												getSubtypeGUID(), 
												getFormatGUID(), 
												getFormatBlockSize(), 
												getFormatBlock());

	//LEAK CHECK::: Where does this get deleted ?
	OggDemuxSourcePin* locSourcePin = new OggDemuxSourcePin(	NAME("Ogg Source Pin"), 
																mOwningFilter, 
																mOwningFilter->m_pLock, 
																mCodecHeaders, 
																locMediaType, 
																getPinName(),
																mAllowSeek,
																getNumBuffers(),
																getBufferSize()
															);
	mStreamReady = true;
	mSourcePin = locSourcePin;
	
	return true;
	
}

void OggStream::setLastEndGranPos(__int64 inGranPos) {
	//osDebug<<"*************************** ERROR ERROR ERROR **********************"<<endl;
	//debugLog<<"*************************** ERROR ERROR ERROR **********************"<<endl;
	mLastEndGranulePos = inGranPos;
}
bool OggStream::acceptOggPage(OggPage* inOggPage) {		//Gives away page.
	
	//debugLog<<"Accepting ogg page..."<<endl;


	//TODO::: Is this even necessary any more ?????

	//Chaining hack for icecast.
	if ( (!mAllowSeek) && (inOggPage->header()->isBOS() )) {
		//debugLog<<"ice case hack"<<endl;
		//A BOS page can only be sent here if it's a chain... otherwise
		// it would have already been stripped by the demux if it was at the
		// start of the file.
		//debugLog<<"Detected chain... setting seek timebase to -1"<<endl;
		mOwningFilter->mSeekTimeBase = -1;		

	}

	//FIX::: Add proper error checking.

	//debugLog<<"acceptOggPage : "<<endl<<inOggPage->header()->toString()<<endl<<endl;;
	//StampedOggPacket* locPacket = NULL;
	////osDebug<<"New page sets start gran to "<<mLastEndGranulePos<<endl;
	mLastStartGranulePos = mLastEndGranulePos;
	mLastEndGranulePos = inOggPage->header()->GranulePos();

	//Put the page through the packetiser.
	return mPacketiser.acceptOggPage(inOggPage);

}

void OggStream::setSendExcess(bool inSendExcess) {
	mSendExcess = inSendExcess;
}

bool OggStream::deliverCodecHeaders() {
	StampedOggPacket* locPacket = NULL;
	for (unsigned long i = 0; i < mCodecHeaders->numPackets(); i++) {
		locPacket = mCodecHeaders->getPacket(i);

		dispatchPacket(locPacket);
	}
	return true;
}

//ANX::: Maybe also needs override. ??
bool OggStream::dispatchPacket(StampedOggPacket* inPacket) { //We don't own this packet.
	//debugLog<<"Ogg Stream : Packet stamps = "<<inPacket->startTime()<<" - "<<inPacket->endTime()<<endl;
	return mSourcePin->deliverOggPacket(inPacket);		
}

void OggStream::setSerialNo(unsigned long inSerialNo) {
	mSerialNo = inSerialNo;
}

