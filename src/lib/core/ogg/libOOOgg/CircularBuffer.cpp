#include "StdAfx.h"
#include ".\circularbuffer.h"

CircularBuffer::CircularBuffer(unsigned long inBufferSize)
	:	mBufferSize(inBufferSize)
	,	mBuffer(NULL)
	,	mReadPtr(0)
	,	mWritePtr(0)

{
	mBuffer = new unsigned char[inBufferSize];
}

CircularBuffer::~CircularBuffer(void)
{
	delete mBuffer;
}

unsigned long CircularBuffer::read(unsigned char* outData, unsigned long inBytesToRead) {

	unsigned long locBytesToRead =			(inBytesToRead > numBytesAvail())	?	numBytesAvail()
																				:	inBytesToRead;

	unsigned long locEndDistance = mBufferSize - mReadPtr;
	//Where we will be if in relation to the end of the raw buffer if we wrote the bufferout from here.
	//Negative values indicate bytes past the end ofthe buffer.
	signed long locEndOffset = locEndDistance - locBytesToRead;

	if (locEndOffset >= 0) {
		//Within the buffer
		memcpy((void*)outData, (const void*)(mBuffer + mReadPtr), locBytesToRead);
		mReadPtr += locBytesToRead;
	} else {
		//Copy from the end of the raw buffer as much as we can into outdtata
		memcpy((void*)outData, (const void*)(mBuffer + mReadPtr), locEndDistance);

		//Copy from the start of the raw buffer whatever is left
		memcpy((void*)(outData + locEndDistance), (const void*)(mBuffer + mReadPtr + locEndDistance), locBytesToRead - locEndDistance);
		mReadPtr = (mReadPtr + locBytesToRead) % mBufferSize;
	}

	
	
	return locBytesToRead;
}

unsigned long CircularBuffer::spaceLeft() {
	//The write pointer is always treated as being equal to or in front of the read pointer.
	return mBufferSize - numBytesAvail();
}
unsigned long CircularBuffer::numBytesAvail() {
	if (mReadPtr > mWritePtr) {
		//Read pointer is to the right of the Write pointer
		// Since the write pointer is always in front, this means all the data from the read ptr
		// to the end of the buffer, plus everything from the start up to the write pointer is
		// available
		//
		////

		
		return (mBufferSize + mWritePtr - mReadPtr);



	} else {
		//if (mReadPtr <= mWritePtr)
		return mWritePtr - mReadPtr;
	}
}

void CircularBuffer::reset() {
	mWritePtr = 0;
	mReadPtr = 0;
}

unsigned long CircularBuffer::write(const unsigned char* inData, unsigned long inBytesToWrite) {
	unsigned long locBytesToWrite =			(inBytesToWrite >  spaceLeft())		?	spaceLeft()
																				:	inBytesToWrite;

	unsigned long locEndDistance = mBufferSize - mWritePtr;
	//Where we will be, in relation to the end of the raw buffer if we wrote the buffer out from here.
	//Negative values indicate bytes past the end ofthe buffer.
	signed long locEndOffset = locEndDistance - locBytesToWrite;


	if (locEndOffset >= 0) {
		//Within the buffer
		memcpy((void*)(mBuffer + mWritePtr), ((const void*)inData), locBytesToWrite);
		
		mWritePtr += locBytesToWrite;
	} else {
		
		//Copy from the end of the raw buffer as much as we can into outdtata
		memcpy((void*)(mBuffer + mWritePtr), (const void*)inData, locEndDistance);

		//Copy from the start of the raw buffer whatever is left
		memcpy((void*)(mBuffer + mWritePtr + locEndDistance), (const void*)(inData + locEndDistance), locBytesToWrite - locEndDistance);
		
		//Advance the write pointer wrapping voer the end.
		mWritePtr = (mWritePtr + locBytesToWrite) % mBufferSize;
	}

	return locBytesToWrite;

	

}
