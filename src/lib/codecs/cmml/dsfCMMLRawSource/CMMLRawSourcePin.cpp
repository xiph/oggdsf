#include "stdafx.h"
#include ".\cmmlrawsourcepin.h"

CMMLRawSourcePin::CMMLRawSourcePin(CMMLRawSourceFilter* inParentFilter, CCritSec* inFilterLock)
	:	CBaseOutputPin(NAME("CMML Raw Source Pin"), inParentFilter, inFilterLock, &mFilterHR, L"CMML Source")
	,	mDataQueue(NULL)

{
	mCMMLFormatBlock.granuleDenominator = 1;
	mCMMLFormatBlock.granuleNumerator = 1000;

	debugLog.open("G:\\logs\\cmmlrawsource.log", ios_base::out);
}

CMMLRawSourcePin::~CMMLRawSourcePin(void)
{
	debugLog.close();
}

STDMETHODIMP CMMLRawSourcePin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	
	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT CMMLRawSourcePin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	
	mDataQueue->NewSegment(tStart, tStop, dRate);

	return S_OK;
}
HRESULT CMMLRawSourcePin::DeliverEndOfStream(void)
{
	
	mDataQueue->EOS();
    return S_OK;
}

HRESULT CMMLRawSourcePin::DeliverEndFlush(void)
{
	mDataQueue->EndFlush();
    return S_OK;
}

HRESULT CMMLRawSourcePin::DeliverBeginFlush(void)
{
	
	mDataQueue->BeginFlush();
    return S_OK;
}

HRESULT CMMLRawSourcePin::CompleteConnect (IPin *inReceivePin)
{
	mFilterHR = S_OK;
	//Set the delegate for seeking
	//((BasicSeekable*)(inReceivePin))->SetDelegate(this);
	//This may cause issue if pins are disconnected and reconnected
	//DELETE in DEStructor
	mDataQueue = new COutputQueue (inReceivePin, &mFilterHR, FALSE, TRUE,1,TRUE, NUM_BUFFERS);
	if (FAILED(mFilterHR)) {
		mFilterHR = mFilterHR;
	}
	
	return CBaseOutputPin::CompleteConnect(inReceivePin);
}

HRESULT CMMLRawSourcePin::BreakConnect(void) {
	delete mDataQueue;
	mDataQueue = NULL;
	return CBaseOutputPin::BreakConnect();
}

	//CSourceStream virtuals
HRESULT CMMLRawSourcePin::GetMediaType(int inPosition, CMediaType* outMediaType) {
	//Put it in from the info we got in the constructor.
	//NOTE::: May have missed some fields ????
	//NOTE::: May want to check for null pointers
	//outMediaType->SetFormat(mMediaType->Format(), mMediaType->FormatLength());
	if (inPosition == 0) {
		CMediaType locMediaType;

		locMediaType.majortype = MEDIATYPE_Text;
		locMediaType.subtype = MEDIASUBTYPE_CMML;
		locMediaType.formattype = FORMAT_CMML;

		BYTE* locFB = locMediaType.AllocFormatBuffer(sizeof(sCMMLFormatBlock)); //0;//sizeof(sSpeexFormatBlock);

		//locMediaType.pbFormat = (BYTE*)&mCMMLFormatBlock; //(BYTE*)locSpeexFormatInfo;
		memcpy((void*)locFB, (const void*)&mCMMLFormatBlock, sizeof(sCMMLFormatBlock));
		locMediaType.pUnk = NULL;
		*outMediaType = locMediaType;
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}
HRESULT CMMLRawSourcePin::CheckMediaType(const CMediaType* inMediaType) {
	if ((inMediaType->majortype == MEDIATYPE_Text) && (inMediaType->subtype == MEDIASUBTYPE_CMML) && (inMediaType->formattype == FORMAT_CMML)) {
		return S_OK;
	} else {
		return E_FAIL;
	}
}
HRESULT CMMLRawSourcePin::DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest) {

	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;


	locReqAlloc.cbAlign = 1;
	locReqAlloc.cbBuffer = BUFFER_SIZE;
	locReqAlloc.cbPrefix = 0;
	locReqAlloc.cBuffers = NUM_BUFFERS;

	locHR = inoutAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (locHR != S_OK) {
		return locHR;
	}
	
	locHR = inoutAllocator->Commit();

	return locHR;

}


HRESULT CMMLRawSourcePin::deliverTag(C_CMMLTag* inTag) {

	IMediaSample* locSample = NULL;
	REFERENCE_TIME locStart = 0;
	REFERENCE_TIME locStop = 0;

	HRESULT	locHR = GetDeliveryBuffer(&locSample, NULL, NULL, NULL);


	if (locHR != S_OK) {
		//debugLog<<"Failure... No buffer"<<endl;
		return locHR;
	}

	BYTE* locOutBuffer = NULL;
	locHR = locSample->GetPointer(&locOutBuffer);

	if (locHR != S_OK) {
		//debugLog<<"Failure... No buffer"<<endl;
		return locHR;
	}

	wstring locStr = inTag->toString();

	
	//TODO::: For now, this narrowfies the string... to ascii, instead of sending
	// 2 byte chars.

	string locNarrowStr = StringHelper::toNarrowStr(locStr);

	memcpy((void*)locOutBuffer, (const void*)locNarrowStr.c_str(), locNarrowStr.size());

	if (inTag->tagType() == C_CMMLTag::CLIP) {
		C_ClipTag* locClip = (C_ClipTag*)inTag;
		locStart = StringHelper::stringToNum(StringHelper::toNarrowStr(locClip->start())) * 1000ULL;
		
		//TODO::: Do something better for handling of end times !!!!!!!!!!!!!!!!!!!!!!

		locStop = StringHelper::stringToNum(StringHelper::toNarrowStr(locClip->start())) * 1000ULL;
		

	}

	debugLog<<"Tag time = "<<locStart<<" - "<<locStop<<endl;
	debugLog<<locNarrowStr<<endl<<endl;
	locSample->SetActualDataLength(locNarrowStr.size());
	locSample->SetTime(&locStart, &locStop);
	locSample->SetMediaTime(NULL, NULL);
	locSample->SetDiscontinuity(FALSE);
	locSample->SetSyncPoint(TRUE);

	locHR = Deliver(locSample);

	int x = locSample->Release();
	return locHR;


}
