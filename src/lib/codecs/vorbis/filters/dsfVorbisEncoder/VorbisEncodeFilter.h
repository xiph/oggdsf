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

#pragma once

//Local Includes
#include "vorbisencoderdllstuff.h"
#include "IVorbisEncodeSettings.h"
#include "PropsVorbisEncoder.h"

//External Includes
#include "AbstractTransformFilter.h"

//Forward Declarations
struct VORBISFORMAT;
class VorbisEncodeInputPin;
class VorbisEncodeOutputPin;

class VorbisEncodeFilter
	:	public AbstractTransformFilter
	,	public IVorbisEncodeSettings
    ,	public ISpecifyPropertyPages
{
public:
	//Friend Classes
	friend class VorbisEncodeInputPin;
	friend class VorbisEncodeOutputPin;

	//Constructors
	VorbisEncodeFilter(void);
	virtual ~VorbisEncodeFilter(void);

	//COM Initialisation
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	//COM Creator function
	static CUnknown* WINAPI VorbisEncodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

    /// ISpecifyPropertyPages::GetPages Implementation
	STDMETHODIMP GetPages(CAUUID* outPropPages);

	/// Returns the encoder setting for vorbis
	virtual STDMETHODIMP_(VorbisEncodeSettings) getEncoderSettings();
	
    /// Sets the quality
    virtual STDMETHODIMP_(bool) setQuality(int inQuality);

    /// Sets the bitrate via quality mode
    virtual STDMETHODIMP_(bool) setBitrateQualityMode(int inBitrate);

    /// Sets the  managed bitrate constraints
    virtual STDMETHODIMP_(bool) setManaged(int inBitrate, int inMinBitrate, int inMaxBitrate);

protected:
	//Implementation of pure virtuals from AbstractTransformFilter
	virtual bool ConstructPins();

	//Member data
	VORBISFORMAT mVorbisFormatBlock;
};
