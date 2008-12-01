//===========================================================================
//Copyright (C) 2004-2006 Zentaro Kavanagh
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
#include ".\flacpushdecoder.h"

FLACPushDecoder::FLACPushDecoder(void)
	:	mInPacket(NULL)
	,	mOutPacket(NULL)
	,	mNumChannels(0)
	,	mFrameSize(0)
	,	mSampleRate(0)
	,	mBegun(false)
	,	mGotMetaData(false)
{

}

FLACPushDecoder::~FLACPushDecoder(void)
{
	delete mInPacket;
	delete mOutPacket;
}

void FLACPushDecoder::initCodec() {
	init();
}
void FLACPushDecoder::flushCodec() {
	flush();
}

bool FLACPushDecoder::acceptMetadata(OggPacket* inPacket) {
	delete mInPacket;
	mInPacket = inPacket;
	bool locMetaOK = process_until_end_of_metadata();
	delete mInPacket;
	mInPacket = NULL;
	delete mOutPacket;
	mOutPacket = NULL;
	return locMetaOK;
}
StampedOggPacket* FLACPushDecoder::decodeFLAC(OggPacket* inPacket) {
	//Basically puts the incoming packet into the member variable.
	//Calls process_single() and the read call back is fired.
	//The read callback feeds in the packet we just saved.
	//The write callback fires.
	//The write callback sets the outpacket into a member variable.
	//We return the member variable.
	delete mInPacket;
	mInPacket = inPacket;
	if(process_single()) {
		return mOutPacket;
	} else {
		delete mInPacket;
		mInPacket = NULL;
		delete mOutPacket;
		mOutPacket = NULL;
		return NULL;
	}

}
//FLAC Callbacks
::FLAC__StreamDecoderReadStatus FLACPushDecoder::read_callback(FLAC__byte outBuffer[], size_t* outNumBytes) 
{
	//If we have a packet waiting...
	if (mInPacket != NULL) {
		//Copy it onto the buffer.
		memcpy((void*)outBuffer, (const void*)mInPacket->packetData(), mInPacket->packetSize());

		//Tell the decoder how big it is.
		*outNumBytes = mInPacket->packetSize();
		
		//Delete the packet.
		delete mInPacket;
		mInPacket = NULL;
	
		return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;

	} else {
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	}

}
::FLAC__StreamDecoderWriteStatus FLACPushDecoder::write_callback(const ::FLAC__Frame* inFrame, const FLAC__int32* const inBuffer[]) 
{

	if (! mBegun) {
	
		mBegun = true;
		
		mNumChannels = inFrame->header.channels;
		mFrameSize = mNumChannels * SIZE_16_BITS;
		mSampleRate = inFrame->header.sample_rate;
		
	}
	unsigned long locNumFrames = inFrame->header.blocksize;
	unsigned long locActualSize = locNumFrames * mFrameSize;
	//unsigned long locTotalFrameCount = locNumFrames * mNumChannels;

	//BUG::: There's a bug here. Implicitly assumes 2 channels.
	unsigned char* locBuff = new unsigned char[locActualSize];


	signed short* locShortBuffer = (signed short*)locBuff;		//Don't delete this.
	
	signed short tempInt = 0;
	int tempLong = 0;
	//float tempFloat = 0;
	
	//FIX:::Move the clipping to the abstract function
	//Make sure our sample buffer is big enough

	//Modified for FLAC int32 not float

		
	//Must interleave and convert sample size.
	for(unsigned long i = 0; i < locNumFrames; i++) {
		for (unsigned long j = 0; j < mNumChannels; j++) {

			tempLong = inBuffer[j][i];
			tempInt = (signed short)(tempLong);

		
			*locShortBuffer = tempInt;
			locShortBuffer++;
		}
	}
	delete mOutPacket;
	mOutPacket = new StampedOggPacket(locBuff, locActualSize, false, false, 0, locNumFrames, StampedOggPacket::OGG_END_ONLY);
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

}
void FLACPushDecoder::metadata_callback(const ::FLAC__StreamMetadata* inMetadata) 
{

}
void FLACPushDecoder::error_callback(::FLAC__StreamDecoderErrorStatus inStatus) 
{

}
