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

#include "StdAfx.h"
#include "stampedoggpacket.h"

StampedOggPacket::StampedOggPacket(void)
	//:	OggPacket()
	:	mStampType(eStampType::NONE)
	,	mStartTime(0)
	,	mEndTime(0)

{
}

StampedOggPacket::~StampedOggPacket(void)
{
}

StampedOggPacket::StampedOggPacket(unsigned char* inPackData, unsigned long inPacketSize, bool inIsTruncated, bool inIsContinuation, __int64 inStartTime = 0, __int64 inEndTime = 0, unsigned short inStampType = 0)
	:	OggPacket(inPackData, inPacketSize, inIsTruncated, inIsContinuation)
	,	mStartTime(inStartTime)
	,	mEndTime(inEndTime)
	,	mStampType(inStampType)
	
	
{
	//mStampType =inStampType;
}

void StampedOggPacket::merge(StampedOggPacket* inMorePacket) {

	//Make a new buffer the size of both data segs together
	unsigned char* locBuff = new unsigned char[mPacketSize + inMorePacket->mPacketSize];
	//Copy this packets data to the start
	memcpy((void*)locBuff, (const void*)mPacketData, mPacketSize);
	//Copy the next packets data after it
	memcpy((void*)(locBuff + mPacketSize), (const void*)inMorePacket->mPacketData, inMorePacket->mPacketSize);
	//Delete our original packet data
	delete[] mPacketData;
	//Now make our data be the combined data
	mPacketData = locBuff;
	//Make the size the sum of both packets
	mPacketSize += inMorePacket->mPacketSize;

	//Copy time stamping

	
	//Don't copy start stamp, keep the current packets start stamp.
	//mStartTime = inMorePacket->startTime();
	//
	mEndTime = inMorePacket->endTime();
	mStampType = inMorePacket->mStampType;

	//---::: Changed, uses two flags no.
	//If the next part of the packet isn't complete then this packet is not complete.
	//mIsComplete = inMorePacket->mIsComplete;

	//The new packet is truncated only if the incoming packet is
	mIsTruncated = inMorePacket->isTruncated();

	//This is not a continuation... a continuation is a packet that does not start at the start of the real packet.
	mIsContinuation = false;
}

OggPacket* StampedOggPacket::clone() {
	//Make a new buffer for packet data
	unsigned char* locBuff = new unsigned char[mPacketSize];

	//Copy the packet data into the new buffer
	memcpy((void*)locBuff, (const void*)mPacketData, mPacketSize);

	//Create the new packet
	StampedOggPacket* retPack = new StampedOggPacket(locBuff, mPacketSize, mIsTruncated, mIsContinuation, mStartTime, mEndTime, mStampType);
	return retPack;
}
__int64 StampedOggPacket::startTime() {
	return mStartTime;
}
__int64 StampedOggPacket::endTime() {
	return mEndTime;
}

void StampedOggPacket::setStartTime(__int64 inStartTime) {
	mStartTime = inStartTime;
}
void StampedOggPacket::setEndTime(__int64 inEndTime) {
	mEndTime = inEndTime;
}

void StampedOggPacket::setTimeStamp(__int64 inStartTime, __int64 inEndTime, StampedOggPacket::eStampType inStampType) {
	mStartTime = inStartTime;
	mEndTime = inEndTime;
	mStampType = inStampType;
}
