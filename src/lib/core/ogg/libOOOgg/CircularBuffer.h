#pragma once
#include "dllstuff.h"
#include "IFIFOBuffer.h"
class LIBOOOGG_API CircularBuffer
	:	public IFIFOBuffer
{
public:
	CircularBuffer(unsigned long inBufferSize);
	~CircularBuffer(void);

	virtual unsigned long read(unsigned char* outData, unsigned long inBytesToRead);
	virtual unsigned long write(const unsigned char* inData, unsigned long inBytesToWrite);

	virtual unsigned long numBytesAvail();
	virtual unsigned long spaceLeft();

	virtual void reset();
protected:
	unsigned long mBufferSize;
	unsigned long mReadPtr;
	unsigned long mWritePtr;

	void bufASSERT(bool inBool) {	if (!inBool) throw 0; };
	unsigned char* mBuffer;
};
