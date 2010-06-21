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
#include "flacencoderdllstuff.h"
#include "IFLACEncodeSettings.h"
#include "PropsFLACEncoder.h"

//External Includes
#include "AbstractTransformFilter.h"

//Forward Declarations
struct FLACFORMAT;
class FLACEncodeInputPin;
class FLACEncodeOutputPin;

class FLACEncodeFilter
	//Base Classes
	:	public AbstractTransformFilter
	,	public IFLACEncodeSettings
    ,	public ISpecifyPropertyPages
{
public:
	//Friend Classes
	friend class FLACEncodeInputPin;
	friend class FLACEncodeOutputPin;

	//Constructors
	FLACEncodeFilter(void);
	virtual ~FLACEncodeFilter(void);

	//COM Creator function
	static CUnknown* WINAPI FLACEncodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	//COM Initialisation
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

    /// ISpecifyPropertyPages::GetPages Implementation
	STDMETHODIMP GetPages(CAUUID* outPropPages);


    //IFLACEncodeSettings Implementation

    virtual STDMETHODIMP_(bool) canModifySettings();

    virtual STDMETHODIMP_(bool) setEncodingLevel(unsigned long inLevel);
    virtual STDMETHODIMP_(bool) setLPCOrder(unsigned long inLPCOrder);
    virtual STDMETHODIMP_(bool) setBlockSize(unsigned long inBlockSize);
    virtual STDMETHODIMP_(bool) useMidSideCoding(bool inUseMidSideCoding); //Only for 2 channels
    virtual STDMETHODIMP_(bool) useAdaptiveMidSideCoding(bool inUseAdaptiveMidSideCoding); //Only for 2 channels, overrides midside, is faster
    virtual STDMETHODIMP_(bool) useExhaustiveModelSearch(bool inUseExhaustiveModelSearch);
    virtual STDMETHODIMP_(bool) setRicePartitionOrder(unsigned long inMin, unsigned long inMax);

    virtual STDMETHODIMP_(long) encoderLevel();
    virtual STDMETHODIMP_(unsigned long) LPCOrder();
    virtual STDMETHODIMP_(unsigned long) blockSize();
    virtual STDMETHODIMP_(unsigned long) riceMin();
    virtual STDMETHODIMP_(unsigned long) riceMax();
    virtual STDMETHODIMP_(bool) isUsingMidSideCoding();
    virtual STDMETHODIMP_(bool) isUsingAdaptiveMidSideCoding();
    virtual STDMETHODIMP_(bool) isUsingExhaustiveModel();
protected:
	//Implementation from AbstractTransformFilter
	virtual bool ConstructPins();

	FLACFORMAT mFLACFormatBlock;
	
};
