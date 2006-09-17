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
#include "Vorbisdecodeoutputpin.h"

VorbisDecodeOutputPin::VorbisDecodeOutputPin(		VorbisDecodeFilter* inParentFilter
												,	CCritSec* inFilterLock
												,	vector<CMediaType*> inAcceptableMediaTypes)

	:	AbstractTransformOutputPin(		inParentFilter
									,	inFilterLock
									,	NAME("VorbisDecodeOutputPin")
									,	L"PCM Out"
									,	65536
									,	20
									,	inAcceptableMediaTypes)

{

		
}
VorbisDecodeOutputPin::~VorbisDecodeOutputPin(void)
{
	
	
}

STDMETHODIMP VorbisDecodeOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT VorbisDecodeOutputPin::CreateAndFillFormatBuffer(CMediaType* outMediaType, int inPosition)
{
    //WFE::: Extensible format here
    if (inPosition == 0) {
        //This is not required... it's done in FillMediaType in the base class
		//outMediaType->SetType(&MEDIATYPE_Audio);
		//outMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
		//outMediaType->SetFormatType(&FORMAT_WaveFormatEx);
		//outMediaType->SetTemporalCompression(FALSE);
		//outMediaType->SetSampleSize(0);

		WAVEFORMATEXTENSIBLE* locFormatEx = (WAVEFORMATEXTENSIBLE*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEXTENSIBLE));
        
		locFormatEx->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

		locFormatEx->Format.nChannels = (WORD)((VorbisDecodeFilter*)m_pFilter)->mVorbisFormatInfo->numChannels;
		locFormatEx->Format.nSamplesPerSec =  ((VorbisDecodeFilter*)m_pFilter)->mVorbisFormatInfo->samplesPerSec;
        locFormatEx->Format.wBitsPerSample = 16;

        
        locFormatEx->Samples.wValidBitsPerSample = 16;

        if (locFormatEx->Format.nChannels <= 6)
        {
            //THis is the incorrect order for vorbis, but we have to reorder it manually on output.

            switch (locFormatEx->Format.nChannels)
            {
                case 1:
                    locFormatEx->dwChannelMask =        SPEAKER_FRONT_LEFT;
                    break;
                case 2:
                    locFormatEx->dwChannelMask =        SPEAKER_FRONT_LEFT
                                        |   SPEAKER_FRONT_RIGHT;
                    break;
                case 3:
                    locFormatEx->dwChannelMask =        SPEAKER_FRONT_LEFT
                                        |   SPEAKER_FRONT_RIGHT
                                        |   SPEAKER_FRONT_CENTER;
                    break;
                case 4:
                    locFormatEx->dwChannelMask =        SPEAKER_FRONT_LEFT
                                        |   SPEAKER_FRONT_RIGHT
                                        |   SPEAKER_BACK_LEFT
                                        |   SPEAKER_BACK_RIGHT;
                    break;
                case 5:
                   locFormatEx->dwChannelMask =        SPEAKER_FRONT_LEFT
                                        |   SPEAKER_FRONT_RIGHT
                                        |   SPEAKER_FRONT_CENTER
                                        |   SPEAKER_BACK_LEFT
                                        |   SPEAKER_BACK_RIGHT;
                    break;

                case 6:
                     locFormatEx->dwChannelMask =        SPEAKER_FRONT_LEFT
                                        |   SPEAKER_FRONT_RIGHT
                                        |   SPEAKER_FRONT_CENTER
                                        |   SPEAKER_LOW_FREQUENCY
                                        |   SPEAKER_BACK_LEFT
                                        |   SPEAKER_BACK_RIGHT
                                        ;
                     break;

                default:
                    locFormatEx->dwChannelMask = 0;
                    break;


            }

           

            //Assume channels ordered 012345

            //For 1 channel
            //      Do nothing

            //For 2 channels
            //      Do nothing

            //For 3 channels
            //      Swap channels 1 and 2 (the right and centre)

            //For 4 channels
            //      Do nothing

            //For 5 channels
            //      Swap channels 1 and 2 (the right and centre)

            //For 6 channels
            //      Change from FL   FC   FR   RL   RR   LFE
            //      to          FL   FR   FC   LFE  RL   RR

  
        } else {
            //More than 6 channels it's up to the output device to figure it out.
            locFormatEx->dwChannelMask = 0;
        }

		locFormatEx->Format.nBlockAlign = (WORD)((locFormatEx->Format.nChannels) * (locFormatEx->Format.wBitsPerSample >> 3));
		locFormatEx->Format.nAvgBytesPerSec = ((locFormatEx->Format.nChannels) * (locFormatEx->Format.wBitsPerSample >> 3)) * locFormatEx->Format.nSamplesPerSec;
		locFormatEx->Format.cbSize = 22; //sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        locFormatEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;


        return S_OK;
    } else if (inPosition == 1) {

		WAVEFORMATEX* locWaveFormat = (WAVEFORMATEX*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
		//TODO::: Check for null ?

		locWaveFormat->wFormatTag = WAVE_FORMAT_PCM;
		locWaveFormat->nChannels = ((VorbisDecodeFilter*)m_pFilter)->mVorbisFormatInfo->numChannels;
		locWaveFormat->nSamplesPerSec =  ((VorbisDecodeFilter*)m_pFilter)->mVorbisFormatInfo->samplesPerSec;
		locWaveFormat->wBitsPerSample = 16;
		locWaveFormat->nBlockAlign = (locWaveFormat->nChannels) * (locWaveFormat->wBitsPerSample >> 3);
		locWaveFormat->nAvgBytesPerSec = ((locWaveFormat->nChannels) * (locWaveFormat->wBitsPerSample >> 3)) * locWaveFormat->nSamplesPerSec;
		locWaveFormat->cbSize = 0;

 
		return S_OK;
	} else {
        return S_FALSE;
	}
}
