#include "stdafx.h"
#include "oggdemuxoutputpin.h"

OggDemuxOutputPin::OggDemuxOutputPin(void)
{
}

OggDemuxOutputPin::~OggDemuxOutputPin(void)
{
}

HRESULT DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {
	//PURE VIRTUAL - We must implement this method
	//This is called by the input pin if it wishes to specify the allocator properties.
	//We *should* honour it's request but we don't have to. The output pin gets the final say as it
	//is the one that has to fit it's data in there. If the input pin is not happy with it it should not connect !

	//WHAT WE MUST DO:
	//a)	Change any properties we see fit.
	//b)	Call SetProperties on the IMemAllcoator interface we were given
	//c)	Commit the allocator
	//==========================================================================================

	//Create the structures for setproperties to use
	ALLOCATOR_PROPERTIES* locReqAlloc = new ALLOCATOR_PROPERTIES;
	ALLOCATOR_PROPERTIES* locActualAlloc = new ALLOCATOR_PROPERTIES;





}
HRESULT CheckMediaType(const CMediaType *pmt) {

}