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
#include "theoradecoder.h"

TheoraDecoder::TheoraDecoder(void)
: mFirstPacket(true)
, mFirstHeader(true)
, mPacketCount(0)
{
}

TheoraDecoder::~TheoraDecoder(void)
{
}

bool TheoraDecoder::initCodec() {
	theora_comment_init(&mTheoraComment);
	theora_info_init(&mTheoraInfo);
	
	return true;
 }

//This is temporary... get it out of here after testing

yuv_buffer* TheoraDecoder::decodeTheora(StampedOggPacket* inPacket) {		//Accepts packet and deletes it.

	if (mPacketCount < 3) {
		decodeHeader(inPacket);		//Accepts header and deletes it.
		
		return NULL;
	} else {
		if (mFirstPacket) {
			theora_decode_init(&mTheoraState, &mTheoraInfo);
			mFirstPacket = false;
		}
			ogg_packet* locOldPack = simulateOldOggPacket(inPacket);		//Accepts the packet and deletes it.
			theora_decode_packetin(&mTheoraState, locOldPack);
			delete locOldPack->packet;
			delete locOldPack;
			
			int locRetVal = theora_decode_YUVout(&mTheoraState, &mYUVBuffer);
			//Error check
			return &mYUVBuffer;
	}

}

ogg_packet* TheoraDecoder::simulateOldOggPacket(StampedOggPacket* inPacket) {		//inPacket is accepted and deleted.
	const unsigned char NOT_USED = 0;
	ogg_packet* locOldPacket = new ogg_packet;		//Returns this... the caller is responsible for it.
	if (mFirstHeader) {
		locOldPacket->b_o_s = 1;
		mFirstHeader = false;
	} else {
		locOldPacket->b_o_s = NOT_USED;
	}
	locOldPacket->e_o_s = NOT_USED;
	locOldPacket->bytes = inPacket->packetSize();
	locOldPacket->granulepos = inPacket->endTime();
	locOldPacket->packet = inPacket->packetData();
	locOldPacket->packetno = NOT_USED;
	
	//Set this to NULL do it doesn't get deleted by the destructor we are about invoke.
	inPacket->setPacketData(NULL);
	delete inPacket;

	return locOldPacket;		//Gives a poitner to the caller.
}
bool TheoraDecoder::decodeHeader(StampedOggPacket* inHeaderPacket) {		//inHeaderPacket is accepted and deleted.

	ogg_packet* locOldPack = simulateOldOggPacket(inHeaderPacket);		//Accepts packet and deletes it.
	theora_decode_header(&mTheoraInfo, &mTheoraComment, locOldPack);

	delete locOldPack->packet;
	delete locOldPack;
	mPacketCount++;
	return true;
}