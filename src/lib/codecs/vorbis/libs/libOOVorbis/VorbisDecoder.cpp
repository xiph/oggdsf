//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#include "VorbisDecoder.h"

VorbisDecoder::VorbisDecoder(void)
	:	mPacketCount(0)
	,	mNumChannels(0)
	,	mSampleRate(0)
{
	vorbis_info_init(&mVorbisInfo);
	vorbis_comment_init(&mVorbisComment);

}

VorbisDecoder::~VorbisDecoder(void)
{
}

VorbisDecoder::eVorbisResult VorbisDecoder::decodeHeader()
{
	int locRet = vorbis_synthesis_headerin(&mVorbisInfo, &mVorbisComment, &mWorkPacket);
	if (locRet < 0) {
		//Error
		return VORBIS_HEADER_BAD;
	}

	return VORBIS_HEADER_OK;
}

VorbisDecoder::eVorbisResult VorbisDecoder::decodeComment()
{
	int locRet = vorbis_synthesis_headerin(&mVorbisInfo, &mVorbisComment, &mWorkPacket);
	if (locRet < 0) {
		//Error
		return VORBIS_COMMENT_BAD;
	}

	return VORBIS_COMMENT_OK;
}
VorbisDecoder::eVorbisResult VorbisDecoder::decodeCodebook()
{
	int locRet = vorbis_synthesis_headerin(&mVorbisInfo, &mVorbisComment, &mWorkPacket);
	if (locRet < 0) {
		//Error
		return VORBIS_CODEBOOK_BAD;
	}

	locRet = vorbis_synthesis_init(&mVorbisState, &mVorbisInfo);

	//TODO::: What return codes?

	locRet = vorbis_block_init(&mVorbisState, &mVorbisBlock);

	mNumChannels = mVorbisInfo.channels;
	mSampleRate = mVorbisInfo.rate;

	return VORBIS_CODEBOOK_OK;
}
VorbisDecoder::eVorbisResult VorbisDecoder::decodePacket(		const unsigned char* inPacket
											,	unsigned long inPacketSize
											,	short* outSamples
											,	unsigned long inOutputBufferSize
											,	unsigned long* outNumSamples)
{
	mWorkPacket.b_o_s = 0;
	mWorkPacket.bytes = inPacketSize;
	mWorkPacket.e_o_s = 0;
	mWorkPacket.granulepos = 0;
	mWorkPacket.packet = (unsigned char*)inPacket;		//Naughty!
	mWorkPacket.packetno = mPacketCount;

	*outNumSamples = 0;

	if (mPacketCount == 0) {
		mPacketCount++;
		mWorkPacket.b_o_s = 1;
		return decodeHeader();
	} else if (mPacketCount == 1) {
		//Comment
		mPacketCount++;
		return decodeComment();
	} else if (mPacketCount == 2) {
		//Codebooks
		mPacketCount++;
		return decodeCodebook();
	} else {
		mPacketCount++;

		int locRet = vorbis_synthesis(&mVorbisBlock, &mWorkPacket);

		if (locRet != 0) {
			//Error
			return VORBIS_SYNTH_FAILED;
		}

		locRet = vorbis_synthesis_blockin(&mVorbisState, &mVorbisBlock);

		if (locRet != 0) {
			//Error
			return VORBIS_BLOCKIN_FAILED;
		}

		float** locPCM;
		int locNumSamples;
		int locTemp = 0;
		short* locOutBuffer;
		
		while ((locNumSamples = vorbis_synthesis_pcmout(&mVorbisState, &locPCM)) > 0) {
			if (locNumSamples * mNumChannels * sizeof(short) > inOutputBufferSize) {
				//TODO::: Buffer overflow
			} else {
				
				for (int chan= 0; chan < mNumChannels; chan++) {
					//Interleave offset
					locOutBuffer = outSamples + chan;
					//Pointer into one channel of pcm
					float* locOneChannel = locPCM[chan];
					for (int i = 0; i < locNumSamples; i++) {
						locTemp = (int)(locOneChannel[i] * 32767.0f);
						*locOutBuffer = clip16(locTemp);

						//Jump forward numChannels in the buffer
						locOutBuffer += mNumChannels;
					}
				}

				vorbis_synthesis_read(&mVorbisState, locNumSamples);
				*outNumSamples = locNumSamples;
			}

		}

		return VORBIS_DATA_OK;


		
	}

}