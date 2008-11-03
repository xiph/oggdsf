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

#include "stdafx.h"
#include "SpeexEncodeInputPin.h"


#define ADAPT_FRAME_RATE 1


SpeexEncodeInputPin::SpeexEncodeInputPin(       AbstractTransformFilter* inParentFilter
                                            ,   CCritSec* inFilterLock
                                            ,   AbstractTransformOutputPin* inOutputPin
                                            ,   vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformInputPin(      inParentFilter
                                    ,   inFilterLock
                                    ,   inOutputPin
                                    ,   NAME("SpeexEncodeInputPin")
                                    ,   L"PCM In"
                                    ,   inAcceptableMediaTypes)
	,	mWaveFormat(NULL)
    ,   mBegun(false)
	,	mUptoFrame(0)
{

}

SpeexEncodeInputPin::~SpeexEncodeInputPin(void)
{
	DestroyCodec();
}

HRESULT SpeexEncodeInputPin::TransformData(unsigned char* inBuf, long inNumBytes) 
{
    HRESULT locHR = S_OK;
    vector<StampedOggPacket*> locPackets;
    if (!mBegun) {
        locPackets = mSpeexEncoder.setupCodec(mEncoderSettings);
        
        //What to do about this?

        //if (locPackets.size() != VorbisEncoder::NUM_VORBIS_HEADERS) {
        //    //Is this really what we want to return?
        //    return E_FAIL;
        //}


        locHR = sendPackets(locPackets);
        deletePacketsAndEmptyVector(locPackets);
        if (locHR != S_OK) {
            return locHR;
        }
        mBegun = true;
    }

    unsigned long locNumSamplesPerChannel = bufferBytesToSampleCount(inNumBytes);
    locPackets = mSpeexEncoder.encode((const short* const)inBuf, locNumSamplesPerChannel);

#ifdef ADAPT_FRAME_RATE
	/* skip packet if we are too late ? */
	__int64 curTime = (m_dsTimeStart * mEncoderSettings.sampleRate())/1000;
	if (mUptoFrame-curTime > locNumSamplesPerChannel) 
	{
		return S_OK;
	}
#endif

    locHR = sendPackets(locPackets);
    deletePacketsAndEmptyVector(locPackets);

#ifdef ADAPT_FRAME_RATE
	/* Resend same packet if we are too early */
	if (!FAILED(locHR)) 
	{
		while (curTime-mUptoFrame > locNumSamplesPerChannel) 
		{
		    locPackets = mSpeexEncoder.encode((const short* const)inBuf, locNumSamplesPerChannel);
			locHR = sendPackets(locPackets);
			deletePacketsAndEmptyVector(locPackets);
		}
	}
#endif

	return locHR;

}
bool SpeexEncodeInputPin::ConstructCodec() 
{

	((SpeexEncodeFilter*)mParentFilter)->mSpeexFormatBlock.numChannels = mWaveFormat->nChannels;
	((SpeexEncodeFilter*)mParentFilter)->mSpeexFormatBlock.samplesPerSec = mWaveFormat->nSamplesPerSec;

    mEncoderSettings.setAudioParameters(mWaveFormat->nSamplesPerSec, mWaveFormat->nChannels);

    return true;
}

void SpeexEncodeInputPin::DestroyCodec() 
{

}

HRESULT SpeexEncodeInputPin::EndOfStream()
{
    CAutoLock locLock(mStreamLock);

    //TODO:::!!

    //vector<StampedOggPacket*> locPackets = mSpeexEncoder.flush();

    //HRESULT locHR = sendPackets(locPackets);
    //deletePacketsAndEmptyVector(locPackets);
    return AbstractTransformInputPin::EndOfStream();

}

HRESULT SpeexEncodeInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	
	if (	(inMediaType->subtype == MEDIASUBTYPE_PCM) &&
			(inMediaType->formattype == FORMAT_WaveFormatEx)) {

		mWaveFormat = (WAVEFORMATEX*)inMediaType->pbFormat;
		
	} else {
		//Failed... should never be here !
		throw 0;
	}
	
	ConstructCodec();

	return CBaseInputPin::SetMediaType(inMediaType);

}


void SpeexEncodeInputPin::deletePacketsAndEmptyVector(vector<StampedOggPacket*>& inPackets)
{
    for (size_t i = 0; i < inPackets.size(); i++) {
        delete inPackets[i];
    }
    inPackets.clear();
}

unsigned long SpeexEncodeInputPin::bufferBytesToSampleCount(long inByteCount)
{
    if (mEncoderSettings.numChannels() == 0) {
        return 0;
    }
    //TODO::: Needs a bytes per sample thingy
    const long SIZE_OF_SHORT = sizeof(short);
    return (inByteCount / mEncoderSettings.numChannels()) / SIZE_OF_SHORT;
}

HRESULT SpeexEncodeInputPin::sendPackets(const vector<StampedOggPacket*>& inPackets)
{
  
	LONGLONG locFrameStart;
    LONGLONG locFrameEnd;
    IMediaSample* locSample = NULL;
    BYTE* locBuffer = NULL;

    for (size_t pack = 0; pack < inPackets.size(); pack++) {
        locFrameStart = mUptoFrame;
        locFrameEnd = inPackets[pack]->endTime();
        mUptoFrame = locFrameEnd;

        HRESULT locHR = mOutputPin->GetDeliveryBuffer(     &locSample
                                                        ,   &locFrameStart
                                                        ,   &locFrameEnd
                                                        ,   NULL);
        if (FAILED(locHR)) {
		    //We get here when the application goes into stop mode usually.
		    return locHR;
	    }	

        //TODO::: Should we be checking this return?
        locSample->GetPointer(&locBuffer);

	    if (locSample->GetSize() >= inPackets[pack]->packetSize()) {
		    memcpy((void*)locBuffer, (const void*)inPackets[pack]->packetData(), inPackets[pack]->packetSize());
    		
		    //Set the sample parameters.
		    SetSampleParams(locSample, inPackets[pack]->packetSize(), &locFrameStart, &locFrameEnd);

		    locHR = ((SpeexEncodeOutputPin*)mOutputPin)->mDataQueue->Receive(locSample);
            if (locHR != S_OK) {
                return locHR;
            }

	    } else {
		    throw 0;
	    }
    }

    return S_OK;
}

HRESULT SpeexEncodeInputPin::CompleteConnect (IPin *inReceivePin)
{
    //This data is captured in setmedia type. We set it on the settings class
    // here so that when we are setting options which depend on knowing audio
    // parameters, ie some settings require stereo. This gets it ready
    // before the user can touch it from a properties page

    //Defaults
    SpeexEncodeSettings locSettings;
    mEncoderSettings = locSettings;

    
    mEncoderSettings.setAudioParameters(    ((SpeexEncodeFilter*)mParentFilter)->mSpeexFormatBlock.samplesPerSec
                                                ,((SpeexEncodeFilter*)mParentFilter)->mSpeexFormatBlock.numChannels);

    return AbstractTransformInputPin::CompleteConnect(inReceivePin);

}