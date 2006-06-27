#pragma once



class SpeexDecodeSettings
{
public:

	enum eChannelForce {
		SPEEX_CHANNEL_LEAVE_ALONE = 0,
		SPEEX_CHANNEL_FORCE_MONO,
		SPEEX_CHANNEL_FORCE_STEREO

	};

	enum eBandForce {
		SPEEX_BAND_LEAVE_ALONE = 0,
		SPEEX_BAND_FORCE_NARROWBAND,
		SPEEX_BAND_FORCE_WIDEBAND,
		SPEEX_BAND_FORCE_ULTRAWIDEBAND
	};

	friend class SpeexDecoder;
	SpeexDecodeSettings(void);
	~SpeexDecodeSettings(void);

	bool set(int inSampleRate, eChannelForce inChannels, eBandForce inBand, bool inPercEnh);
private:
	int mForceSampleRate;
	eChannelForce mForceChannels;
	eBandForce mForceBand;
	bool mPerceptualEnhancement;

};
