#include "StdAfx.h"
#include "abstractvideoencodeinputpin.h"

AbstractVideoEncodeInputPin::AbstractVideoEncodeInputPin(AbstractVideoEncodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractVideoEncodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName)
	:	CBaseInputPin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName),
		mOutputPin(inOutputPin),
		mUptoFrame(0),
		mBegun(false),
		mParentFilter(inParentFilter)
	
{
	//debugLog.open("C:\\temp\\aaein.log", ios_base::out);
	//ConstructCodec();
	
}

AbstractVideoEncodeInputPin::~AbstractVideoEncodeInputPin(void)
{
	//debugLog.close();
	//DestroyCodec();
}


void AbstractVideoEncodeInputPin::ResetFrameCount() {
	mUptoFrame = 0;
	
}
bool AbstractVideoEncodeInputPin::SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime) 
{
	outMediaSample->SetTime(inStartTime, inEndTime);
	outMediaSample->SetMediaTime(NULL, NULL);
	outMediaSample->SetActualDataLength(inDataSize);
	outMediaSample->SetPreroll(FALSE);
	outMediaSample->SetDiscontinuity(FALSE);
	outMediaSample->SetSyncPoint(TRUE);
	return true;
}


STDMETHODIMP AbstractVideoEncodeInputPin::Receive(IMediaSample* inSample) {

	//debugLog <<endl<< "Received sample..."<<endl;
	HRESULT locHR;
	BYTE* locBuff = NULL;
	locHR = inSample->GetPointer(&locBuff);

	if (FAILED(locHR)) {
		//debugLog << "Failed to get pointer... bailing out"<<endl;
		return locHR;
	} else {
		
		long locResult = encodeData(locBuff, inSample->GetActualDataLength());
		if (locResult >= 0) {
			//debugLog << "Encode Data returns 0... OK"<<endl;
			return S_OK;
		} else {
			//debugLog<< "Encode Data returns "<<locResult<<" FAILURE"<<endl;
			return S_FALSE;
		}
	}

	//debugLog<<"Receive falls through... returning OK"<<endl;
	return S_OK;
}

HRESULT AbstractVideoEncodeInputPin::CheckMediaType(const CMediaType *inMediaType) {
	//FIX::: Clean this up !
	
	if	( (inMediaType->majortype == MEDIATYPE_Video) &&
			(inMediaType->subtype == MEDIASUBTYPE_YV12) &&
			(inMediaType->formattype == FORMAT_VideoInfo)
		)
	{
		return S_OK;
	} else {
		return S_FALSE;
	}
}

STDMETHODIMP AbstractVideoEncodeInputPin::EndOfStream(void) {
	return mParentFilter->mOutputPin->DeliverEndOfStream();
}

STDMETHODIMP AbstractVideoEncodeInputPin::BeginFlush() {
	CAutoLock locLock(m_pLock);
	CBaseInputPin::BeginFlush();
	return mParentFilter->mOutputPin->DeliverBeginFlush();
}
STDMETHODIMP AbstractVideoEncodeInputPin::EndFlush() {
	CAutoLock locLock(m_pLock);
	CBaseInputPin::EndFlush();
	return mParentFilter->mOutputPin->DeliverEndFlush();

}

STDMETHODIMP AbstractVideoEncodeInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
	return mParentFilter->mOutputPin->DeliverNewSegment(tStart, tStop, dRate);
}

HRESULT AbstractVideoEncodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	//FIX:::Error checking
	//RESOLVED::: Bit better.

	if (inMediaType->subtype == MEDIASUBTYPE_YV12) {
		mVideoFormat = (VIDEOINFOHEADER*)inMediaType->pbFormat;
		//mParentFilter->mAudioFormat = AbstractAudioDecodeFilter::VORBIS;
	} else {
		//Failed... should never be here !
		throw 0;
	}
	return CBaseInputPin::SetMediaType(inMediaType);
}

HRESULT AbstractVideoEncodeInputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}

	switch (inPosition) {
		case 0:

			outMediaType->SetType(&MEDIATYPE_Video);
			outMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
			//Don't set the format data here. That's up to our output pin/
			return S_OK;			
		default:
			return VFW_S_NO_MORE_ITEMS;
	}
}