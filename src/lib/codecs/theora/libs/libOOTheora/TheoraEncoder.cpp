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
#include "theoraencoder.h"

TheoraEncoder::TheoraEncoder(void)
{
}

TheoraEncoder::~TheoraEncoder(void)
{
}


StampedOggPacket** TheoraEncoder::initCodec(theora_info inTheoraInfo) {
	mTheoraInfo = inTheoraInfo;
	theora_encode_init(&mTheoraState,&mTheoraInfo);

	StampedOggPacket** locHeaders = new StampedOggPacket*[3];

	ogg_packet locOldPacket;
	theora_encode_header(&mTheoraState, &locOldPacket);

	locHeaders[0] = oldToNewPacket(&locOldPacket);
	
	theora_comment_init(&mTheoraComment);
	theora_encode_comment(&mTheoraComment, &locOldPacket);

	locHeaders[1] = oldToNewPacket(&locOldPacket);

	theora_encode_tables(&mTheoraState, &locOldPacket);
	
	locHeaders[2] = oldToNewPacket(&locOldPacket);

	return locHeaders;
}



//ogg_packet* TheoraDecoder::simulateOldOggPacket(StampedOggPacket* inPacket) {
//	const unsigned char NOT_USED = 0;
//	ogg_packet* locOldPacket = new ogg_packet;
//	if (mFirstHeader) {
//		locOldPacket->b_o_s = 1;
//		mFirstHeader = false;
//	} else {
//		locOldPacket->b_o_s = NOT_USED;
//	}
//	locOldPacket->e_o_s = NOT_USED;
//	locOldPacket->bytes = inPacket->packetSize();
//	locOldPacket->granulepos = inPacket->endTime();
//	locOldPacket->packet = inPacket->packetData();
//	locOldPacket->packetno = NOT_USED;
//	return locOldPacket;
//}

StampedOggPacket* TheoraEncoder::oldToNewPacket(ogg_packet* inOldPacket) {
	const unsigned char NOT_USED = 0;

	//Need to clone the packet data
	unsigned char* locBuff = new unsigned char[inOldPacket->bytes];
	memcpy((void*)locBuff, (const void*)inOldPacket->packet, inOldPacket->bytes);
	StampedOggPacket* locOggPacket = new StampedOggPacket(locBuff, inOldPacket->bytes, true, NOT_USED, inOldPacket->granulepos, StampedOggPacket::OGG_END_ONLY);
	return locOggPacket;

}
StampedOggPacket* TheoraEncoder::encodeTheora(yuv_buffer* inYUVBuffer) {
	const int NOT_LAST_FRAME = 0;
	const int IS_LAST_FRAME = 1;
	int retVal = 0;

	ogg_packet locOldOggPacket;
	retVal = theora_encode_YUVin(&mTheoraState, inYUVBuffer);
	
	retVal = theora_encode_packetout(&mTheoraState, NOT_LAST_FRAME, &locOldOggPacket);

	return oldToNewPacket(&locOldOggPacket);
	
}
