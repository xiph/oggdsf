#pragma once
#include "dllstuff.h"
class LIBOOOGG_API IFIFOBuffer {
public:
	virtual unsigned long read(unsigned char* outData, unsigned long inBytesToRead) = 0;
	virtual unsigned long write(const unsigned char* inData, unsigned long inBytesToWrite) = 0;

	virtual  unsigned long numBytesAvail() = 0;
	virtual unsigned long spaceLeft() = 0;

	virtual void reset() = 0;
};