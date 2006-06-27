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