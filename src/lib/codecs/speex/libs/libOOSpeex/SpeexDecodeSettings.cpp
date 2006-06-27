#include "StdAfx.h"
#include "SpeexDecodeSettings.h"

SpeexDecodeSettings::SpeexDecodeSettings(void)
	:	mForceSampleRate(0)
	,	mForceChannels(SPEEX_CHANNEL_LEAVE_ALONE)
	,	mForceBand(SPEEX_BAND_LEAVE_ALONE)
	,	mPerceptualEnhancement(true)
{
}

SpeexDecodeSettings::~SpeexDecodeSettings(void)
{
}

bool SpeexDecodeSettings::set(		int inSampleRate
								,	eChannelForce inChannels
								,	eBandForce inBand
								,	bool inPercEnh)
{
	if ((inSampleRate >= 0) && (inChannels == SPEEX_BAND_LEAVE_ALONE)) {
		mForceSampleRate = inSampleRate;
	}

	mForceChannels = inChannels;
	mForceBand = inBand;
	mPerceptualEnhancement = inPercEnh;

	return true;

}