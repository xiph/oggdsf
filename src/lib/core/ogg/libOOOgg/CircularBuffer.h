#pragma once
#include "dllstuff.h"
#include "IFIFOBuffer.h"


//Empty Buffer
//==============
//
//		<--------------- Buffer Size -------------------->
//      
//		0123456789 123456789 123456789 123456789 123456789*
//      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//      R
//		W
//
//
//		0123456789 123456789 123456789 123456789 123456789*
//      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//									R
//									W
//
//When R = W Buffer is empty
//
//		when R = W:	available bytes = 0
//		when R = W: space left = buffer size
////

//Full Buffer
//===========
//
//
//
//		<--------------- Buffer Size -------------------->
//      
//		0123456789 123456789 123456789 123456789 123456789*
//      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                               R
//								W
//
//
//		<--------------- Buffer Size -------------------->
//      
//		0123456789 123456789 123456789 123456789 123456789*
//      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//      R
//								                          W
//
//Buffer is full when R = (W + 1) MOD (bufferSize + 1)
//
//		when	R = (W + 1) MOD (bufferSize + 1):		available bytes = buffer size
//		when	R = (W + 1) MOD (bufferSize + 1):		space left  = 0
//
//
//	
//
//////

//Partial Buffers
//===============
//
//Case 1
//======
//
//
//		<--------------- Buffer Size -------------------->
//      
//		0123456789 123456789 123456789 123456789 123456789*
//      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//    								           R
//											    W
//
//
//		when W > R:		available bytes								=	W - R
//		when W > R:		space left = buffer size - available bytes	=	buffer size + R - W
//
//
//Case 2
//======
//
//
//		<--------------- Buffer Size -------------------->
//				  1			2		  3			4
//		0123456789 123456789 123456789 123456789 123456789*
//      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                                                      R
//		W
//
//		when R > W:			available bytes									=	buffer size + 1 - R + W
//		when R > W:			space left = buffer size - available bytes		=	R - W - 1
//		
//
//
//
//

class LIBOOOGG_API CircularBuffer
	:	public IFIFOBuffer
{
public:
	CircularBuffer(unsigned long inBufferSize);
	~CircularBuffer(void);

	//IFIFOBuffer Implementation
	virtual unsigned long read(unsigned char* outData, unsigned long inBytesToRead);
	virtual unsigned long write(const unsigned char* inData, unsigned long inBytesToWrite);

	virtual unsigned long numBytesAvail();
	virtual unsigned long spaceLeft();

	virtual void reset();
	//
protected:
	unsigned long mBufferSize;
	unsigned long mReadPtr;
	unsigned long mWritePtr;

	void bufASSERT(bool inBool) {	if (!inBool) throw 0; };
	unsigned char* mBuffer;

private:
	CircularBuffer& operator=(const CircularBuffer& other);  /* Don't assign me */
	CircularBuffer(const CircularBuffer& other); /* Don't copy me */
};
