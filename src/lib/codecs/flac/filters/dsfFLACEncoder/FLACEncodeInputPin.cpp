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
#include "FLACencodeinputpin.h"

FLACEncodeInputPin::FLACEncodeInputPin(     AbstractTransformFilter* inParentFilter
                                        ,   CCritSec* inFilterLock
                                        ,   AbstractTransformOutputPin* inOutputPin
                                        ,   vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformInputPin(      inParentFilter
                                    ,   inFilterLock
                                    ,   inOutputPin
                                    ,   NAME("FLACEncodeInputPin")
                                    ,   L"PCM In"
                                    ,   inAcceptableMediaTypes)
	,	mBegun(false)
	,	mWaveFormat(NULL)
	
	
{

}

FLACEncodeInputPin::~FLACEncodeInputPin(void)
{
	DestroyCodec();
}

//PURE VIRTUALS
HRESULT FLACEncodeInputPin::TransformData(unsigned char* inBuf, long inNumBytes) 
{

    HRESULT locHR = S_OK;
    if (mBegun == false) {
        //FLACEncoderSettings locSettings;
        //locSettings.setAudioParameters(         ((FLACEncodeFilter*)mParentFilter)->mFLACFormatBlock.numChannels
        //                                    ,   ((FLACEncodeFilter*)mParentFilter)->mFLACFormatBlock.sampleRate 
        //                                    ,   ((FLACEncodeFilter*)mParentFilter)->mFLACFormatBlock.numBitsPerSample);
        //locSettings.setEncodingLevel(5);
        const vector<StampedOggPacket*>& locHeaderPackets = mFLACEncoder.setupCodec(mFLACEncoderSettings);
        if (locHeaderPackets.size() == 0) {
            return E_FAIL;
        }
        locHR = deliverPackets(locHeaderPackets);
        if (locHR != S_OK) {
            return locHR;
        }
        mBegun = true;
    }

    //TODO::: Handle other bit depths
    const short* const loc16BitBuffer = (const short* const)inBuf;
    const vector<StampedOggPacket*>& locDataPackets = mFLACEncoder.encode16Bit(loc16BitBuffer, inNumBytes/2);
    return deliverPackets(locDataPackets);
}
bool FLACEncodeInputPin::ConstructCodec() 
{

    //TODO::: This is redundant?

	FLACEncodeFilter* locParentFilter = (FLACEncodeFilter*)mParentFilter;	//View only don't delete.
	locParentFilter->mFLACFormatBlock.numBitsPerSample = mWaveFormat->wBitsPerSample;
	locParentFilter->mFLACFormatBlock.numChannels = mWaveFormat->nChannels;
	locParentFilter->mFLACFormatBlock.samplesPerSec = mWaveFormat->nSamplesPerSec;

    mWaveFormat = NULL;

	return true;
}
void FLACEncodeInputPin::DestroyCodec()
{

}

HRESULT FLACEncodeInputPin::deliverPackets(const vector<StampedOggPacket*>& inPackets)
{
    HRESULT locHR = S_OK;
    for (size_t i = 0; i < inPackets.size(); i++) {
        IMediaSample* locSample = NULL;
        BYTE* locBuffer = NULL;
        LONGLONG locStartTime = inPackets[i]->startTime();
        LONGLONG locEndTime = inPackets[i]->endTime();

        locHR = mOutputPin->GetDeliveryBuffer(&locSample, NULL, NULL, NULL);
        if (locHR != S_OK) {
            return locHR;
        }
        
        locHR = locSample->GetPointer(&locBuffer);
        if (locHR != S_OK) {
            return locHR;
        }
  
        memcpy((void*)locBuffer, (const void*)inPackets[i]->packetData(), inPackets[i]->packetSize());
        SetSampleParams(locSample, inPackets[i]->packetSize(), &locStartTime, &locEndTime);

        locHR = ((FLACEncodeOutputPin*)(mOutputPin))->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
        if (locHR != S_OK) {
            return locHR;
        }
    }
    return S_OK;
}

STDMETHODIMP FLACEncodeInputPin::EndOfStream(void) 
{
	//Catch the end of stream so we can send a finish signal.
    CAutoLock locLock(mStreamLock);
    deliverPackets(mFLACEncoder.flush());
	return AbstractTransformInputPin::EndOfStream();		//Call the base class.
}

HRESULT FLACEncodeInputPin::SetMediaType(const CMediaType* inMediaType) 
{

	if (	(inMediaType->subtype == MEDIASUBTYPE_PCM) &&
			(inMediaType->formattype == FORMAT_WaveFormatEx)) {

		mWaveFormat = (WAVEFORMATEX*)inMediaType->pbFormat;
		
	} else {
		//Failed... should never be here !
		throw 0;
	}
	//This is here and not the constructor because we need audio params from the
	// input pin to construct properly.	
	ConstructCodec();

	return CBaseInputPin::SetMediaType(inMediaType);
}

HRESULT FLACEncodeInputPin::CompleteConnect (IPin *inReceivePin)
{
    //This data is captured in setmedia type. We set it on the settings class
    // here so that when we are setting options which depend on knowing audio
    // parameters, ie some settings require stereo. This gets it ready
    // before the user can touch it from a properties page

    //Defaults
    FLACEncoderSettings locSettings;
    mFLACEncoderSettings = locSettings;

    mFLACEncoderSettings.setAudioParameters(    ((FLACEncodeFilter*)mParentFilter)->mFLACFormatBlock.numChannels
                                            ,   ((FLACEncodeFilter*)mParentFilter)->mFLACFormatBlock.samplesPerSec 
                                            ,   ((FLACEncodeFilter*)mParentFilter)->mFLACFormatBlock.numBitsPerSample);

    return AbstractTransformInputPin::CompleteConnect(inReceivePin);

}