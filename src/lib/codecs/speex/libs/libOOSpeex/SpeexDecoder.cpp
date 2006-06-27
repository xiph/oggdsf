#include "StdAfx.h"
#include "SpeexDecoder.h"

SpeexDecoder::SpeexDecoder(void)
	:	mPacketCount(0)
	,	mFrameSize(0)
	,	mNumChannels(0)
	,	mSampleRate(0)
	,	mNumFrames(0)
	,	mNumExtraHeaders(0)
	,	mIsVBR(false)
	,	mSpeexState(NULL)
	//,	mStereoState(NULL)
{
	//mStereoState = SPEEX_STEREO_STATE_INIT;
	mStereoState.balance = 1.0;
	mStereoState.e_ratio = 0.5;
	mStereoState.smooth_left = 1.0;
	mStereoState.smooth_right = 1.0;
}

SpeexDecoder::~SpeexDecoder(void)
{
}

bool SpeexDecoder::setDecodeParams(SpeexDecodeSettings inSettings)
{
	if (mPacketCount == 0) {
		mDecoderSettings = inSettings;
		return true;
	}
	return false;
}

SpeexDecoder::eSpeexResult SpeexDecoder::decodePacket(		const unsigned char* inPacket
														,	unsigned long inPacketSize
														,	short* outSamples
														,	unsigned long inOutputBufferSize)
{
	if (mPacketCount == 0) {
		mPacketCount++;
		return decodeHeader(inPacket, inPacketSize);
	} else if (mPacketCount == 1) {
		//Comment
		mPacketCount++;
		return SPEEX_COMMENT_OK;
	} else if (mPacketCount < 2+mNumExtraHeaders) {
		//Ignore
		mPacketCount++;
		return SPEEX_EXTRA_HEADER_OK;
	} else {
		mPacketCount++;

		speex_bits_read_from(&mSpeexBits, (char*)inPacket, inPacketSize);
		
		for (int frame = 0; frame < mNumFrames; frame++) {
			int locRet = speex_decode_int(mSpeexState, &mSpeexBits, outSamples);

			if (locRet == -1) {
				break;
			} else if (locRet == -2) {
				//Corrupted
				return SPEEX_CORRUPTED_BITSTREAM;
			}

			if (speex_bits_remaining(&mSpeexBits) < 0) {
				//Corrupted
				return SPEEX_CORRUPTED_UNDERFLOW;
			}


			if (mNumChannels == 2) {
				speex_decode_stereo_int(outSamples, mFrameSize, &mStereoState);
			}
		}
		return SPEEX_DATA_OK;

		
	}
}

SpeexDecoder::eSpeexResult SpeexDecoder::decodeHeader(const unsigned char* inPacket, unsigned long inPacketSize)
{

	SpeexHeader* locSpeexHeader = NULL;
	int locModeID = 0;
	const SpeexMode* locMode = NULL;
	void* locState = NULL;
	SpeexCallback locCallback;

	locSpeexHeader = speex_packet_to_header((char*)inPacket, inPacketSize);

	if (locSpeexHeader == NULL) {
		//Can't read header
		return SPEEX_BAD_HEADER;
	}

	//Check modes?
	locModeID = locSpeexHeader->mode;

	locMode = speex_lib_get_mode(locModeID);

	if (locSpeexHeader->speex_version_id > 1) {
		//Invalid version
		return SPEEX_INVALID_SPEEX_VERSION;
	}

	//TODO::: Other bitstream version checks

	locState = speex_decoder_init(locMode);

	if (locState == NULL) {
		//Init failed
		return SPEEX_INITIALISATION_FAILED;
	}

	speex_decoder_ctl(locState, SPEEX_SET_ENH, &mDecoderSettings.mPerceptualEnhancement);
	speex_decoder_ctl(locState, SPEEX_GET_FRAME_SIZE, &mFrameSize);


	if (mDecoderSettings.mForceChannels == SpeexDecodeSettings::SPEEX_CHANNEL_FORCE_STEREO) {
		locCallback.callback_id = SPEEX_INBAND_STEREO;
		locCallback.func = speex_std_stereo_request_handler;
		locCallback.data = &mStereoState;
		speex_decoder_ctl(locState, SPEEX_SET_HANDLER, &locCallback);
	}

	//TODO::: Apply rate forces
	mSampleRate = locSpeexHeader->rate;

	speex_decoder_ctl(locState, SPEEX_SET_SAMPLING_RATE, &mSampleRate);

	mNumFrames = locSpeexHeader->frames_per_packet;

	if (mDecoderSettings.mForceChannels == SpeexDecodeSettings::SPEEX_CHANNEL_LEAVE_ALONE) {
		mNumChannels = locSpeexHeader->nb_channels;
	}

	mIsVBR = (locSpeexHeader->vbr != 0);

	mNumExtraHeaders = locSpeexHeader->extra_headers;

	free(locSpeexHeader);
	mSpeexState = locState;

	speex_bits_init(&mSpeexBits);

	return SPEEX_HEADER_OK;


}