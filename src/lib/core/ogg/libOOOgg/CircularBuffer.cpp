#include "StdAfx.h"
#include ".\circularbuffer.h"

CircularBuffer::CircularBuffer(unsigned long inBufferSize)
	:	mBufferSize(inBufferSize)
	,	mBuffer(NULL)
	,	mReadPtr(0)
	,	mWritePtr(0)

{
	mBuffer = new unsigned char[inBufferSize + 1];
}

CircularBuffer::~CircularBuffer(void)
{
	delete mBuffer;
}

unsigned long CircularBuffer::read(unsigned char* outData, unsigned long inBytesToRead) {

	if (inBytesToRead >  spaceLeft()) {
		return 0;
	}
    	
	unsigned long locBytesToRead =	inBytesToRead;
		
		//	(inBytesToRead <= numBytesAvail())	?	inBytesToRead
		//											:	numBytesAvail();
	//locBytesToRead = the lower of numBytesAvail() and inBytesToRead
	bufASSERT(locBytesToRead <= inBytesToRead);
	bufASSERT(locBytesToRead <= numBytesAvail());
	
	unsigned long locEndDistance = (mBufferSize + 1 - mWritePtr);
	bufASSERT(locEndDistance <= mBufferSize);

	//Where we will be if in relation to the end of the raw buffer if we wrote the bufferout from here.
	//Negative values indicate bytes past the end ofthe buffer.
	//Positive values indicate bytes before the end of buffer.
	
	
	if (locEndDistance >= locBytesToRead) {
		//Within the buffer
		bufASSERT(mReadPtr < mBufferSize);
		
		memcpy((void*)outData, (const void*)(mBuffer + mReadPtr), locBytesToRead);
	} else {
		//Copy from the end of the raw buffer as much as we can into outdtata
		memcpy((void*)outData, (const void*)(mBuffer + mReadPtr), locEndDistance);

		//Copy from the start of the raw buffer whatever is left
		memcpy((void*)(outData + locEndDistance), (const void*)(mBuffer), locBytesToRead - locEndDistance);
	}
	mReadPtr = (mReadPtr + locBytesToRead) % (mBufferSize + 1);

	return locBytesToRead;
}

unsigned long CircularBuffer::write(const unsigned char* inData, unsigned long inBytesToWrite) {
	if (inBytesToWrite >  spaceLeft()) {
		return 0;
	}

	unsigned long locBytesToWrite =	inBytesToWrite;
		
		//		(inBytesToWrite >  spaceLeft())		?	spaceLeft()
		//										:	inBytesToWrite;

	bufASSERT(locBytesToWrite <= spaceLeft());
	bufASSERT(locBytesToWrite <= inBytesToWrite);
	bufASSERT(locBytesToWrite <= mBufferSize);
	bufASSERT(mWritePtr <= mBufferSize);

	unsigned long locEndDistance = (mBufferSize + 1 - mWritePtr);

	bufASSERT(locEndDistance <= mBufferSize + 1);
	//Where we will be, in relation to the end of the raw buffer if we wrote the buffer out from here.
	//Negative values indicate bytes past the end ofthe buffer.
	//signed long locEndOffset = locEndDistance - locBytesToWrite;


	if (locEndDistance >= locBytesToWrite) {
		//Within the buffer
		memcpy((void*)(mBuffer + mWritePtr), ((const void*)inData), locBytesToWrite);
		
		
	} else {
		
		//Copy from the end of the raw buffer as much as we can into outdtata
		memcpy((void*)(mBuffer + mWritePtr), (const void*)inData, locEndDistance);

		//Copy from the start of the raw buffer whatever is left
		memcpy((void*)(mBuffer), (const void*)(inData + locEndDistance), locBytesToWrite - locEndDistance);
		
		//Advance the write pointer wrapping voer the end.
		
	}
	mWritePtr = (mWritePtr + locBytesToWrite) % (mBufferSize + 1);

	return locBytesToWrite;

	

}

unsigned long CircularBuffer::spaceLeft() {
	bufASSERT(mReadPtr <= mBufferSize);
	bufASSERT(mWritePtr <= mBufferSize);

	//The write pointer is always treated as being equal to or in front of the read pointer.
	//return mBufferSize - numBytesAvail() - 1;
	if (mReadPtr > mWritePtr) {
		//Read pointer is to the right of the Write pointer
		// Since the write pointer is always in front, this means all the data from the read ptr
		// to the end of the buffer, plus everything from the start up to the write pointer is
		// available
		//
		////

		bufASSERT(mReadPtr > mWritePtr);
		return  (mReadPtr - mWritePtr - 1);
	} else {
		bufASSERT(mReadPtr <= mWritePtr);
		return mBufferSize + mReadPtr - mWritePtr ;
	}
}
unsigned long CircularBuffer::numBytesAvail() {
	bufASSERT(mReadPtr <= mBufferSize);
	bufASSERT(mWritePtr <= mBufferSize);

	if (mReadPtr > mWritePtr) {
		//Read pointer is to the right of the Write pointer
		// Since the write pointer is always in front, this means all the data from the read ptr
		// to the end of the buffer, plus everything from the start up to the write pointer is
		// available
		//
		////

		bufASSERT(mReadPtr > mWritePtr);

		//Here
		return  (mBufferSize + 1 + mWritePtr - mReadPtr);
	} else {
		//if (mReadPtr <= mWritePtr)
		return mWritePtr - mReadPtr;
	}
}

void CircularBuffer::reset() {
	mWritePtr = 0;
	mReadPtr = 0;
}

