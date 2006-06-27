#pragma once


#include <ivorbiscodec.h>

class TremorDecoder
{
public:
	TremorDecoder(void);
	~TremorDecoder(void);



	enum eVorbisResult {
		VORBIS_DATA_OK = 0,
		VORBIS_HEADER_OK,
		VORBIS_COMMENT_OK,
		VORBIS_CODEBOOK_OK,
		VORBIS_ERROR_MIN = 64,
		VORBIS_HEADER_BAD,
		VORBIS_COMMENT_BAD,
		VORBIS_CODEBOOK_BAD,
		VORBIS_SYNTH_FAILED,
		VORBIS_BLOCKIN_FAILED
	};

	//bool setDecodeParams(SpeexDecodeSettings inSettings);
	eVorbisResult decodePacket(		const unsigned char* inPacket
								,	unsigned long inPacketSize
								,	short* outSamples
								,	unsigned long inOutputBufferSize
								,	unsigned long* outNumSamples); 

	int numChannels()	{	return mNumChannels;	}
	int sampleRate()	{	return mSampleRate;		}
protected:
	eVorbisResult decodeHeader();
	eVorbisResult decodeComment();
	eVorbisResult decodeCodebook();

	short clip16(int inVal)		{	
		//return (short)((inVal > 32767) ? (32767) : ((inVal < -32768) ? (-32768) : (inVal)));	
		int retVal = inVal;
		retVal -= ((inVal <= 32767) - 1) & (inVal - 32767);
		retVal -= ((inVal >= -32768) -1) & (inVal + 32768);
		return retVal;
	}
	unsigned long mPacketCount;

	int mNumChannels;
	int mSampleRate;
	//int mNumFrames;
	//int mNumExtraHeaders;
	//bool mIsVBR;

	vorbis_info mVorbisInfo;
	vorbis_comment mVorbisComment;
	vorbis_dsp_state mVorbisState;
	vorbis_block mVorbisBlock;

	ogg_packet mWorkPacket;
	ogg_reference mOggRef;
	ogg_buffer mOggBuffer;


};
