#pragma once
#include "cmmldecoderdllstuff.h"
#include <string>
#include <fstream>
#include "libWinCMMLParse.h"
#include "CMMLParser.h"
#include "libCMMLTags.h"
using namespace std;
class CMMLDecodeFilter
	:	public CTransformFilter
{
public:
	CMMLDecodeFilter(void);
	virtual ~CMMLDecodeFilter(void);
	//COM Creator Function
	DECLARE_IUNKNOWN
	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	//Implement the Tranform filter interface
	HRESULT CMMLDecodeFilter::CheckInputType(const CMediaType* inInputMediaType);
	HRESULT CMMLDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType);
	HRESULT CMMLDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest);
	HRESULT CMMLDecodeFilter::GetMediaType(int inPosition, CMediaType* outMediaType);
	HRESULT CMMLDecodeFilter::Transform(IMediaSample* inSample, IMediaSample* outSample);

protected:
	wstring toWStr(string inString);
	CMMLParser* mCMMLParser;
	bool mSeenHead;
	C_HeadTag* mHeadTag;

	fstream debugLog;
	

};