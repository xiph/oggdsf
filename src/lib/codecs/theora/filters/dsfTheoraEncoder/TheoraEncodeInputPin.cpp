//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
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

#include "stdafx.h"
#include "theoraencodeinputpin.h"

TheoraEncodeInputPin::TheoraEncodeInputPin(AbstractTransformFilter* inParentFilter, CCritSec* inFilterLock, AbstractTransformOutputPin* inOutputPin, vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformInputPin(inParentFilter, inFilterLock, inOutputPin, NAME("TheoraEncodeInputPin"), L"YV12 In", inAcceptableMediaTypes)
	,	mXOffset(0)
	,	mYOffset(0)
	,	mHeight(0)
	,	mWidth(0)
	,	mUptoFrame(0)
	,	mBegun(false)
	,	mVideoFormat(NULL)
	

{
	//debugLog.open("g:\\logs\\theoencfiltinput.log", ios_base::out);
	mYUV.y = NULL;
	mYUV.u = NULL;
	mYUV.v = NULL;
	
}

TheoraEncodeInputPin::~TheoraEncodeInputPin(void)
{
	//debugLog.close();
	DestroyCodec();
	delete[] mYUV.y;
	delete[] mYUV.u;
	delete[] mYUV.v;

}


HRESULT TheoraEncodeInputPin::deliverData(LONGLONG inStart, LONGLONG inEnd, unsigned char* inBuf, unsigned long inNumBytes) {
	//debugLog <<" deliverData : "<<inStart<<" - "<<inEnd<<"  :: size = "<<inNumBytes<<endl;
	//Get a pointer to a new sample stamped with our time
	IMediaSample* locSample;
	HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, &inStart, &inEnd, NULL);

	if (locHR != S_OK) {
		//We get here when the application goes into stop mode usually.
		return locHR;
	}	
	
	BYTE* locBuffer = NULL;

	
	//Make our pointers set to point to the samples buffer
	locSample->GetPointer(&locBuffer);

	if (locSample->GetSize() >= inNumBytes) {

		memcpy((void*)locBuffer, (const void*)inBuf, inNumBytes);
		
		//Set the sample parameters.
		SetSampleParams(locSample, inNumBytes, &inStart, &inEnd);

		{
			CAutoLock locLock(m_pLock);

			HRESULT locHR = ((TheoraEncodeOutputPin*)mOutputPin)->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
			if (locHR != S_OK) {
				return locHR;	
			} else {
			}
		}
		//debugLog<<"deliverData : SUCCESS"<<endl;
		return S_OK;
	} else {
		//debugLog<<"Buffer too small !!!! FATALITY !"<<endl;
		throw 0;
	}

}

long TheoraEncodeInputPin::encodeYV12ToYV12(unsigned char* inBuf, long inNumBytes) {
	//
	// Now properly pads on one side only... 20/12/04
	//

	//Source Buffer all in one buffer

	//YV12 memory layout for directshow
	//=================================
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//VVVVVVVVVVVVVVVVVVVVVVVV VVVVVVVVVVVVVVVVVVVVVVVV
	//UUUUUUUUUUUUUUUUUUUUUUUU UUUUUUUUUUUUUUUUUUUUUUUU


	//Destination buffer Y, U, V in seperate planes, lowercase are line padding, "." height padding
	//.............................. ..............................
	//.............................. ..............................
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//.............................. ..............................
	//.............................. ..............................

	//.............................. ..............................
	//vvvVVVVVVVVVVVVVVVVVVVVVVVVvvv vvvVVVVVVVVVVVVVVVVVVVVVVVVvvv
	//.............................. ..............................
	
	//.............................. ..............................
	//uuuUUUUUUUUUUUUUUUUUUUUUUUUuuu uuuUUUUUUUUUUUUUUUUUUUUUUUUuuu
	//.............................. ..............................

	//NOTE	: mHeight, mWidth are the actual video sizes and are the sizes of the incoming buffer
	//		  The yuv width, height parameters are a /16 up rounded size of the output buffer to be sent to libtheora


	//Setup the source pointer
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr
	//
	
	//========
	//Y DATA |
	//=========================================================================================================

	//Setup the destination pointer
	char* locDestUptoPtr = mYUV.y;
	//

	////Pad top (which is really bottom of the image) of output Y plane buffer with mYOffset lines of width mYUV.y_width
	////y_width is the out frame width.
	//if (mYOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, mYOffset * mYUV.y_width);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += (mYOffset * mYUV.y_width);							//I'm fairly sure it can be junk... but leave for now !
	//}
	//Source pointer does not advance
	//

	//Add mHeight lines of data of width mWidth plus padding of mXOffset at the start of each line
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		//
		//This branch of the condition does exactly the same as the else branch where mXOffset = 0
		for (long line = 0; line < mHeight; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth);
			locSourceUptoPtr += mWidth;
			locDestUptoPtr += mWidth;
		}
	} else {
		for (long line = 0; line < mHeight; line++) {
			//Pad the start of the line with mXOffset bytes
			memset((void*)locDestUptoPtr, NULL, mXOffset);
			locDestUptoPtr += mXOffset;

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth);
			locSourceUptoPtr += mWidth;
			locDestUptoPtr += mWidth;

			//
			//This is no longer done... we just pad on one side for efficiency
			//
			//Pad the end of the line with mXOffset bytes
			//memset((void*)locDestUptoPtr, NULL, mXOffset);
			//locDestUptoPtr += mXOffset;
		}

	}

	
	//Pad bottom of Y plane buffer with mYOffset lines of width mYUV.y_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, mYOffset * mYUV.y_width);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += (mYOffset * mYUV.y_width);
		//Source pointer does not advance
	}
	


	//========
	//V DATA |
	//=========================================================================================================

	//Set the destination poitner
	locDestUptoPtr = mYUV.v;
	//

	//ASSERT (mYOffset is EVEN)
	//ASSERT (mHeight is EVEN)
	//ASSERT (mWidth is EVEN)
	//

	////Pad top of V plane buffer with mYOffset/2 lines of width mYUV.uv_width
	//if (mYOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	////

	//Add mHeight/2 lines of data of length mWidth/2 plus padded by mXOffset/2 at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);
		}
	} else {
		for (long line = 0; line < mHeight / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);

			//
			//This is no longer done... we just pad on one side for efficiency
			//

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			//locDestUptoPtr += (mXOffset / 2);
			////Source pointer does not advance
		}

	}


	//Pad bottom of V plane buffer with mYOffset / 2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}
	




	//========
	//U DATA |
	//=========================================================================================================

	//Set the destination pointer
	locDestUptoPtr = mYUV.u;
	//

	////Pad top of U plane buffer with mYOffset/2 lines of width mYUV.uv_width
	//if (mYOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	////

	//Add mHeight/2 lines of data of length mWidth/2 plus padded by mXOffset/2 at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);
		}
	} else {
		for (long line = 0; line < mHeight / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);

			//
			//This is no longer done... we just pad on one side for efficiency
			//

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			//locDestUptoPtr += (mXOffset / 2);
			////Source pointer does not advance
		}

	}

	
	//Pad bottom of U plane buffer with mYOffset / 2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}

	//======================================================================================================
	return 0;

}

//-------------------------------------------------------------------------

long TheoraEncodeInputPin::encodeIYUVToYV12(unsigned char* inBuf, long inNumBytes) {
	//Same as YV12 but planes U and V are reversed.

	//Source Buffer all in one buffer

	//IYUV memory layout for directshow
	//=================================
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	
	//UUUUUUUUUUUUUUUUUUUUUUUU UUUUUUUUUUUUUUUUUUUUUUUU
	
	//VVVVVVVVVVVVVVVVVVVVVVVV VVVVVVVVVVVVVVVVVVVVVVVV
	


	//Destination buffer Y, U, V in seperate planes, lowercase are line padding, "." height padding
	//.............................. ..............................
	//.............................. ..............................
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//.............................. ..............................
	//.............................. ..............................

	//.............................. ..............................
	//uuuUUUUUUUUUUUUUUUUUUUUUUUUuuu uuuUUUUUUUUUUUUUUUUUUUUUUUUuuu
	//.............................. ..............................
	
	//.............................. ..............................
	//vvvVVVVVVVVVVVVVVVVVVVVVVVVvvv vvvVVVVVVVVVVVVVVVVVVVVVVVVvvv
	//.............................. ..............................
	


	//NOTE	: mHeight, mWidth are the actual video sizes and are the sizes of the incoming buffer
	//		  The yuv width, height parameters are a /16 up rounded size of the output buffer to be sent to libtheora


	//Setup the source pointer
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr
	//
	
	//========
	//Y DATA |
	//=========================================================================================================

	//Setup the destination pointer
	char* locDestUptoPtr = mYUV.y;
	//

	////Pad top of Y plane buffer with mYOffset lines of width mYUV.y_width
	//if (mYOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, mYOffset * mYUV.y_width);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += (mYOffset * mYUV.y_width);
	//}
	//Source pointer does not advance
	//

	//Add mHeight lines of data of width mWidth plus padding of mXOffset at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth);
			locSourceUptoPtr += mWidth;
			locDestUptoPtr += mWidth;
		}
	} else {
		for (long line = 0; line < mHeight; line++) {
			//Pad the start of the line with mXOffset bytes
			memset((void*)locDestUptoPtr, NULL, mXOffset);
			locDestUptoPtr += mXOffset;

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth);
			locSourceUptoPtr += mWidth;
			locDestUptoPtr += mWidth;

			////Pad the end of the line with mXOffset bytes
			//memset((void*)locDestUptoPtr, NULL, mXOffset);
			//locDestUptoPtr += mXOffset;
		}

	}

	//Pad bottom of Y plane buffer with mYOffset lines of width mYUV.y_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, mYOffset * mYUV.y_width);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += (mYOffset * mYUV.y_width);
		//Source pointer does not advance
	}

	//========
	//U DATA |
	//=========================================================================================================

	//Set the destination pointer
	locDestUptoPtr = mYUV.u;
	//

	////Pad top of U plane buffer with mYOffset/2 lines of width mYUV.uv_width
	//if (mYOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	//

	//Add mHeight/2 lines of data of length mWidth/2 plus padded by mXOffset/2 at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);
		}
	} else {
		for (long line = 0; line < mHeight / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			//locDestUptoPtr += (mXOffset / 2);
			////Source pointer does not advance
		}

	}

	//Pad bottom of U plane buffer with mYOffset / 2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}
	


	//========
	//V DATA |
	//=========================================================================================================

	//Set the destination poitner
	locDestUptoPtr = mYUV.v;
	//

	////Pad top of V plane buffer with mYOffset/2 lines of width mYUV.uv_width
	//if (mYOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	////

	//Add mHeight/2 lines of data of length mWidth/2 plus padded by mXOffset/2 at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);
		}
	} else {
		for (long line = 0; line < mHeight / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			//locDestUptoPtr += (mXOffset / 2);
			////Source pointer does not advance
		}

	}

	//Pad bottom of V plane buffer with mYOffset / 2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}
	





	//======================================================================================================
	return 0;

}
//-------------------------------------------------------------------------

long TheoraEncodeInputPin::encodeRGB24toYV12(unsigned char* inBuf, long inNumBytes) {
	//Blue Green Red Blue Green Red.
	unsigned long locNumPixels = (inNumBytes/3);
	unsigned char* locAYUVBuf = new unsigned char[locNumPixels<<2];   //4 bytes per pixel

	/*




	Conversion from RGB to YUV is defined by starting with the following:

	L = Kr * R + Kb * B + (1 – Kr – Kb) * G

	The YUV values are then obtained as follows:

	Y =                 floor(2^(M-8) * (219*(L–Z)/S + 16) + 0.5)
	U = clip3(0, 2^M-1, floor(2^(M-8) * (112*(B-L) / ((1-Kb)*S) + 128) + 0.5))
	V = clip3(0, 2^M-1, floor(2^(M-8) * (112*(R-L) / ((1-Kr)*S) + 128) + 0.5))

	where Z = 16
	S = 219
	M = 8 bits per sample.

	==>
	Y =					floor(L + 0.5)
	U =					(112*(B-L) / ((1-Kb)*S) + 128)		


	Kr'			= Kr * 65536
	Kb'			= Kb * 65536
	G_FACTOR	= (1 - Kr - Kb) * 65536

	L'			= (Kr' * R)  +  (Kb' * B) + (G_FACTOR * G)
				= 65536 * ( (Kr * R) + (Kb * B) + ((1 - Kr - Kb) * G) )
				= 65536 * L

	Y			= round( 219 * (L-Z)/S + 16 )
				= round ( L-Z + 16 )
				= round( L )

	Y'			= L'
				= 65536 * L
				
	Y			= L' >> 16

	U_FACTOR	= ( 1 - Kb) * S
	U_FACTOR'	= 12716213
	
				= 65536 * U_FACTOR

	V_FACTOR'	= 10061022
	
	B'			= 65536 * B
	R'			= 65536 * R

	_U_			= round( 112 * (B-L) / ( (1-Kb)*S ) + 128 )
				= round( (112 * (B-L) / U_FACTOR) + 128 )
				= (112 * (B' - L') / U_FACTOR') + 128
				= (112 * 65536 * (B - L) / (U_FACTOR * 65536)) + 128
				= (112 * (B - L) / U_FACTOR) + 128

	Hence integerisation scaling cancels
	==>
	_U_			= (112 * (B' - L') / U_FACTOR') + 128
	
	_V_			= (112 * (R' - L') / V_FACTOR') + 128
		
	*/

	/*
	Kr = 0.299
	Kb = 0.114


	*/

	//Scaled by factor of 65536 to integerise.
	const int KR = 19596;
	const int KB = 7472;
	
	const int ROUNDER = 32768;

	const int G_FACTOR = 38470;
	const int U_FACTOR = 12716213;
	const int V_FACTOR = 10061022;

	int locL = 0;
	int locB = 0;
	int locR = 0;

	//unsigned char* locSourcePtr = inBuf;
	unsigned char* locDestPtr = locAYUVBuf;

    //SOURCE: Blue Green Red Blue Green Red.
	//DEST: v u y a

	unsigned char* locSourceEnds = inBuf + (locNumPixels * 3);

	//Upside down... Upside down !
	//for (unsigned char* locSourcePtr = inBuf; locSourcePtr < locSourceEnds; locSourcePtr += 3) {
	//	locB = locSourcePtr[0];					//Blue
	//	locL = KB * (locB);						//Blue
	//	
	//	locL += G_FACTOR * (locSourcePtr[1]);	//Green

	//	locR = locSourcePtr[2];					//Red
	//	locL += KR * (locR);					//Red

	//	
	//	*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locR<<16) - locL)) / V_FACTOR) + 128);			//V for Victor
	//	*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locB<<16) - locL)) / U_FACTOR) + 128);			//U for ugly
	//	*(locDestPtr++) = CLIP3(0, 255, locL >> 16);												//Y for yellow
	//	*(locDestPtr++) = 255;																		//A for alpha
	//}
	

	unsigned char* locColSourcePtr = NULL;
	unsigned char* locColEndPtr = NULL;
	unsigned long locLineLength = mWidth * 3;
	unsigned long col = 0;
	for (unsigned char* locSourcePtr = locSourceEnds - locLineLength; locSourcePtr >= inBuf; locSourcePtr -= locLineLength) {
		//
		//for(unsigned char* locColSourcePtr = locSourcePtr, int i = 0; i < mWidth; i++, locColSourcePtr +=4) {
		//
		locColSourcePtr = locSourcePtr;
		locColEndPtr = locColSourcePtr + locLineLength;
		while (locColSourcePtr < locColEndPtr) {
			locB = locColSourcePtr[0];					//Blue
			locL = KB * (locB);							//Blue
		
			locL += G_FACTOR * (locColSourcePtr[1]);	//Green

			locR = locColSourcePtr[2];					//Red
			locL += KR * (locR);						//Red

		
			*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locR<<16) - locL)) / V_FACTOR) + 128);			//V for Victor
			*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locB<<16) - locL)) / U_FACTOR) + 128);			//U for ugly
			*(locDestPtr++) = CLIP3(0, 255, locL >> 16);												//Y for yellow
			*(locDestPtr++) = 255;																		//A for alpha

			//debugCount++;		
			locColSourcePtr+=3;

		}


	}


	//Still need to pass through to the AYUV conversion.
	encodeAYUVtoYV12(locAYUVBuf, locNumPixels<<2);
	delete[] locAYUVBuf;
	locAYUVBuf = NULL;

	return 0;
}



long TheoraEncodeInputPin::encodeRGB32toYV12(unsigned char* inBuf, long inNumBytes) {
	//Blue Green Red Alpha Blue Green Red Alpha
	//debugLog<<"EncodeRGB32 To YV12 :"<<endl;

	unsigned long locNumPixels = (inNumBytes/4);
	
	//debugLog<<"EncodeRGB32 To YV12 : Num pixels = "<<locNumPixels<<endl;
	//debugLog<<"EncodeRGB32 To YV12 : Num BYtes = "<<inNumBytes<<endl;
	unsigned char* locAYUVBuf = new unsigned char[inNumBytes];   //4 bytes per pixel

	//debugLog<<"EncodeRGB32 To YV12 :"<<endl;

	//Scaled by factor of 65536 to integerise.
	const int KR = 19596;
	const int KB = 7472;
	
	const int ROUNDER = 32768;

	const int G_FACTOR = 38470;
	const int U_FACTOR = 12716213;
	const int V_FACTOR = 10061022;

	int locL = 0;
	int locB = 0;
	int locR = 0;

	//unsigned char* locSourcePtr = inBuf;
	unsigned char* locDestPtr = locAYUVBuf;

    //SOURCE: Blue Green Red Blue Green Red.
	//DEST: v u y a

	unsigned char* locSourceEnds = inBuf + (inNumBytes);
	//debugLog<<"EncodeRGB32 To YV12 : Source Starts = "<<(int)inBuf<<endl;
	//debugLog<<"EncodeRGB32 To YV12 : Source Ends = "<<(int)locSourceEnds<<endl;

	//Debugging only... all refs to debugCount remove later
	//unsigned long debugCount = 0;
	//

	//Upside down !!
	//for (unsigned char* locSourcePtr = inBuf; locSourcePtr < locSourceEnds; locSourcePtr += 4) {
	//	locB = locSourcePtr[0];					//Blue
	//	locL = KB * (locB);						//Blue
	//	
	//	locL += G_FACTOR * (locSourcePtr[1]);	//Green

	//	locR = locSourcePtr[2];					//Red
	//	locL += KR * (locR);					//Red

	//	
	//	*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locR<<16) - locL)) / V_FACTOR) + 128);			//V for Victor
	//	*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locB<<16) - locL)) / U_FACTOR) + 128);			//U for ugly
	//	*(locDestPtr++) = CLIP3(0, 255, locL >> 16);												//Y for yellow
	//	*(locDestPtr++) = locSourcePtr[3];																		//A for alpha

	//	debugCount++;
	//}
	unsigned char* locColSourcePtr = NULL;
	unsigned char* locColEndPtr = NULL;
	unsigned long locLineLength = mWidth * 4;
	unsigned long col = 0;
	for (unsigned char* locSourcePtr = locSourceEnds - locLineLength; locSourcePtr >= inBuf; locSourcePtr -= locLineLength) {
		//
		//for(unsigned char* locColSourcePtr = locSourcePtr, int i = 0; i < mWidth; i++, locColSourcePtr +=4) {
		//
		locColSourcePtr = locSourcePtr;
		locColEndPtr = locColSourcePtr + locLineLength;
		while (locColSourcePtr < locColEndPtr) {
			locB = locColSourcePtr[0];					//Blue
			locL = KB * (locB);							//Blue
		
			locL += G_FACTOR * (locColSourcePtr[1]);	//Green

			locR = locColSourcePtr[2];					//Red
			locL += KR * (locR);						//Red

		
			*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locR<<16) - locL)) / V_FACTOR) + 128);			//V for Victor
			*(locDestPtr++) = CLIP3(0, 255, ((112 * ( (locB<<16) - locL)) / U_FACTOR) + 128);			//U for ugly
			*(locDestPtr++) = CLIP3(0, 255, locL >> 16);												//Y for yellow
			*(locDestPtr++) = locColSourcePtr[3];														//A for alpha

			//debugCount++;		
			locColSourcePtr+=4;

		}


	}

	//debugLog<<"EncodeRGB32 To YV12 : debugCount = "<<debugCount<<endl;

	//ASSERT(debugCount == locNumPixels);
	
	ASSERT(locDestPtr == (locAYUVBuf + inNumBytes));

	//debugLog<<"EncodeRGB32 To YV12 : Calling AYUV to YV12 conversion"<<endl;
	//Still need to pass through to the AYUV conversion.

	encodeAYUVtoYV12(locAYUVBuf, inNumBytes);
	delete[] locAYUVBuf;
	locAYUVBuf = NULL;

	return 0;
}




long TheoraEncodeInputPin::encodeAYUVtoYV12(unsigned char* inBuf, long inNumBytes) {

	//Victor Ugly Yellow Alpha --fonts are fuzzy late at night-- (Yellow is not colour yellow)

	//AYUV is VUYA VUYA VUYA VUYA 4:4:4 sampling
	// Twice the vertical and horizontal sampling of YV12 in chrominance

	//Strategy : Process two lines and 2 cols at a time averaging 4 U and V around the position where a
	// YV12 chroma sample will be... leave luminance samples... ignore alpha samples

	//debugLog<<"Encode AYUV To YV12 :"<<endl;
	
	const int PIXEL_BYTE_SIZE = 4;
	ASSERT (mHeight % 2 == 0);
	ASSERT (mWidth % 2 == 0);

	unsigned char* locSourceUptoPtr = inBuf;						//View only... don't delete locUptoPtr
	unsigned char* locSourceNextLine = locSourceUptoPtr + (mWidth * PIXEL_BYTE_SIZE);	//View only don't delete
	
	char* locYUpto = mYUV.y;
	char* locUUpto = mYUV.u;
	char* locVUpto = mYUV.v;
	//Pointer to the same pixel on next line
	char* locDestNextLine = locYUpto + (mWidth);				//View only... don't delete

	int temp = 0;

	//Process 2 lines at a time
	for (int line = 0; line < mHeight; line += 2) {
		//debugLog<<"Encode AYUV To YV12 : ++ Line = "<<line<<endl;
		
		ASSERT (locSourceUptoPtr == (inBuf + (line * (mWidth * PIXEL_BYTE_SIZE))));
		ASSERT (locSourceNextLine == locSourceUptoPtr + (mWidth * PIXEL_BYTE_SIZE));
		ASSERT (locYUpto == (mYUV.y + (line * mWidth)));
		ASSERT (locDestNextLine == locYUpto + (mWidth));
		

		//Columns also done 2 at a time
		for (int col = 0; col < mWidth; col += 2) {
			//debugLog<<"Encode AYUV To YV12 : ++++++ Col = "<<col<<endl;


			//						v	u	y	a	v	u	y	a
			//SourceUpto			^
			//NextUpto				^

			//====================
			//V for Victor samples
			//====================
			temp =	*(locSourceUptoPtr++);			//Current pixel

			//						v	u	y	a	v	u	y	a
			//SourceUpto				^
			//NextUpto				^

			//This is three as we already advanced one and are pointing to a u not a v
			temp += *(locSourceUptoPtr + 3);		//Pixel to right
			temp += *(locSourceNextLine++);			//Pixel below

			//						v	u	y	a	v	u	y	a
			//SourceUpto				^
			//NextUpto					^

			temp += *(locSourceNextLine + 3);			//Pixel below right
			temp >>= 2;								//Divide by 4 to average.
			*(locVUpto++) = (unsigned char)temp;

			//						v	u	y	a	v	u	y	a
			//SourceUpto				^
			//NextUpto					^

			//====================
			//U for Ugly samples
			//====================
			temp =	*(locSourceUptoPtr++);			//Current pixel

			//						v	u	y	a	v	u	y	a
			//SourceUpto					^
			//NextUpto					^

			temp += *(locSourceUptoPtr + 3);			//Pixel to right
			temp += *(locSourceNextLine++);			//Pixel below


			//						v	u	y	a	v	u	y	a
			//SourceUpto					^
			//NextUpto						^

			temp += *(locSourceNextLine + 3);			//Pixel below right
			temp >>= 2;								//Divide by 4 to average.
			*(locUUpto++) = (unsigned char)temp;

			//						v	u	y	a	v	u	y	a
			//SourceUpto					^
			//NextUpto						^

			//====================
			//Y for Yellow samples.
			//====================

			*(locYUpto++) = *(locSourceUptoPtr++);
			*(locDestNextLine++) = *(locSourceNextLine++);

			//						v	u	y	a	v	u	y	a
			//SourceUpto						^
			//NextUpto							^

			//Ignore the alpha channel
			//--
			//locSourceUptoPtr++;			//Optimised away... merged into increment below
			//locSourceNextLine++;			// "	"	"	"	"	"	"	"	"	"	"

			//						v	u	y	a	v	u	y	a
			//SourceUpto							^
			//NextUpto								^


			//--
			//Source and next pointer have advanced four bytes so far.
			//Added 2 Y for yellows (one from each line) and one each of U and V, ignore an A on each line
			//--

			//Current line extra Y for yellows.
			locSourceUptoPtr += 3;					//Skip the A and U and V samples
			*(locYUpto++) = *(locSourceUptoPtr);	//get the Y for yellow sample		
			locSourceUptoPtr += 2;					//Advance 1 for the Y for yellow and Skip the A sample.

			//						v	u	y	a	v	u	y	a
			//SourceUpto											^
			//NextUpto								^


			//Next line extra Y for yellows.
			locSourceNextLine += 3;							//Skip the A and U and V samples
			*(locDestNextLine++) = *(locSourceNextLine);	//get the Y for yellow sample		
			locSourceNextLine += 2;							//Advance 1 for the Y for yellow and Skip the A sample.

			//						v	u	y	a	v	u	y	a
			//SourceUpto											^
			//NextUpto												^

			//--
			//In total source and next pointer advanced 8 bytes on each line, and we got 4 Y for yellows (2 each line)
			// and one each U, V, ignored 4 A's (2 each line)
			//--


		}
		//Overall this loop will advance :
		//Sourceupto    by		= 8 bytes * (mWidth in pixels / 2 pixels at a time) * 2 lines
		//						= 8 * mWidth
		//						= 2 lines of 4 byte pixels of pixel width mWidth
		//and the same for sourcenextline
		//--
		//At the end of this loop sourceupto points to the end of the current line (start of next line)
		// and nextupto points to the end of the next line
		//
		//On the next iteration we want the sourceupto to point 2 lines down from where it did on the previous one
		// This is the same place that the sourcenextline points at the end of the iteration.
		//--

		//Ensures source will point to lines 2, 4, 6 etc.
		locSourceUptoPtr = locSourceNextLine;			
		//Ensures nextlinesource is one line ahead of the main source.
		locSourceNextLine += (mWidth * PIXEL_BYTE_SIZE);

		//Ensures the current destination buffer skips a line ie points to line 2, 4, 6 etc
		locYUpto = locDestNextLine;
		//Ensures the nextlinedest skips a
		locDestNextLine += (mWidth);
	}
	return 0;
}
long TheoraEncodeInputPin::encodeYUY2ToYV12(unsigned char* inBuf, long inNumBytes) {
	
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr

	//YUY2 is Y0 U0 Y1 V0 Y2 U1 Y3 V1
	// it has twice as much sampling height as YV12 so downsample it.

	char* locYUpto = mYUV.y;
	char* locUUpto = mYUV.u;
	char* locVUpto = mYUV.v;

	

	//After downsampling... from each block of 8, we get 4 y samples and 1 each of u and v


	for (int i = 0; i < mHeight / 2; i++) {
		//TO DO: Draw memory layouts.

		//***Part of the average method... store the pointer to the last of the previous line
		//locLastUUpto = locUUpto;
		//locLastVUpto = locVUpto;
		//***

		for (int j = 0; j < mWidth / 2; j++) {
			*(locYUpto++) = *(locSourceUptoPtr++);
			*(locUUpto++) = *(locSourceUptoPtr++);
			*(locYUpto++) = *(locSourceUptoPtr++);
			*(locVUpto++) = *(locSourceUptoPtr++);
		}

		
		//***Drop line method
		for (int j = 0; j < mWidth / 2; j++) {
			//Ignore the second line
			*(locYUpto++) = *(locSourceUptoPtr++);
			locSourceUptoPtr++;
			*(locYUpto++) = *(locSourceUptoPtr++);
			locSourceUptoPtr++;
		}
		//***

		//*** PArt of the Alternate method to average...
		//for (int j = 0; j < mWidth / 2; j++) {
		//	//Ignore the second line
		//	*(locYUpto++) = *(locSourceUptoPtr++);
		//	*(locLastUUpto++) = ((short)(*locLastUUpto) + ((short)(*locUUpto))) / 2;
		//	
		//	*(locYUpto++) = *(locSourceUptoPtr++);
		//	*(locLastVUpto++) = ((short)(*locLastVUpto) + ((short)(*locVUpto))) / 2;
		//	
		//}
		//***



	}
	return 0;
}

long TheoraEncodeInputPin::encodeYVYUToYV12(unsigned char* inBuf, long inNumBytes) {
	//YVYU	-	Yellow Victor Yellow Ugly
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr

	//YUY2 is Y0 U0 Y1 V0 Y2 U1 Y3 V1
	//YVYU is Y0 V0 Y1 U0 Y2 V1 Y3 U1
	// it has twice as much sampling height as YV12 so downsample it.

	char* locYUpto = mYUV.y;
	char* locUUpto = mYUV.u;
	char* locVUpto = mYUV.v;

	

	//After downsampling... from each block of 8, we get 4 y samples and 1 each of u and v


	for (int i = 0; i < mHeight / 2; i++) {
		//TO DO: Draw memory layouts.

	

		for (int j = 0; j < mWidth / 2; j++) {
			*(locYUpto++) = *(locSourceUptoPtr++);				//Y for Yellow
			*(locVUpto++) = *(locSourceUptoPtr++);				//V for victor
			
			*(locYUpto++) = *(locSourceUptoPtr++);				//Y for Yellow
			*(locUUpto++) = *(locSourceUptoPtr++);				//U for Ugly
		}

		
		//***Drop line method
		for (int j = 0; j < mWidth / 2; j++) {
			//Ignore the second line
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for Yellow
			locSourceUptoPtr++;								//V for victor
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for yellow
			locSourceUptoPtr++;								//U for ugly.
		}

	}
	return 0;
}


long TheoraEncodeInputPin::encodeUYVYToYV12(unsigned char* inBuf, long inNumBytes) {
	//UYVY :: U0 Y0 V0 Y1   - Ugly Yellow Victor Yello
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr

	//UYVY is U0 Y0 V0 Y1 U0 Y2 V0 Y3
	//YUY2 is Y0 U0 Y1 V0 Y2 U1 Y3 V1
	// it has twice as much sampling height as YV12 so downsample it.

	char* locYUpto = mYUV.y;
	char* locUUpto = mYUV.u;
	char* locVUpto = mYUV.v;

	

	//After downsampling... from each block of 8, we get 4 y samples and 1 each of u and v


	for (int i = 0; i < mHeight / 2; i++) {
		//TO DO: Draw memory layouts.

	
		for (int j = 0; j < mWidth / 2; j++) {
			*(locUUpto++) = *(locSourceUptoPtr++);			//U for Ugly
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for Yellow
			
			*(locVUpto++) = *(locSourceUptoPtr++);			//V for Victor
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for Yellow
			
		}

		
		//***Drop line method
		for (int j = 0; j < mWidth / 2; j++) {
			//Ignore the second line
			
			locSourceUptoPtr++;								//U for ugly
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for yellow

			locSourceUptoPtr++;								//V for victor
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for yellow
			
		}


	}
	return 0;
}



//PURE VIRTUALS
long TheoraEncodeInputPin::TransformData(unsigned char* inBuf, long inNumBytes) {

	//TODO::: Break this function up a bit !!

	//Time stamps are granule pos not directshow times
	//debugLog<<"Encode data"<<endl;
	LONGLONG locFrameStart = mUptoFrame;
	LONGLONG locFrameEnd = 0;
	HRESULT locHR = S_OK;
	if (!mBegun) {
		//debugLog<<"encodeData : First time"<<endl;
		mBegun = true;
		
		StampedOggPacket** locHeaders;
		locHeaders = mTheoraEncoder.initCodec(mTheoraInfo);

		for (int i = 0; i < 3; i++) {
			locHR = deliverData(0,0,locHeaders[i]->packetData(), locHeaders[i]->packetSize());
			if (locHR != S_OK) {
				return locHR;
			}
		}
	}

	if (mPinInputType.subtype == MEDIASUBTYPE_YUY2) {
		//debugLog<<"About to encode YUY2 to YV12"<<endl;
		encodeYUY2ToYV12(inBuf, inNumBytes);
	} else if (mPinInputType.subtype == MEDIASUBTYPE_AYUV) {
		encodeAYUVtoYV12(inBuf, inNumBytes);

	} else if (mPinInputType.subtype == MEDIASUBTYPE_RGB32) {
		encodeRGB32toYV12(inBuf, inNumBytes);

	} else if (mPinInputType.subtype == MEDIASUBTYPE_RGB24) {
		encodeRGB24toYV12(inBuf, inNumBytes);


	} else if (mPinInputType.subtype == MEDIASUBTYPE_YV12) {
		//Should be more specifc.
		//debugLog<<"About to encode YV12 to YV12"<<endl;
		encodeYV12ToYV12(inBuf, inNumBytes);
	} else if (mPinInputType.subtype == MEDIASUBTYPE_UYVY) {
		
		
		encodeUYVYToYV12(inBuf, inNumBytes);

	} else if (mPinInputType.subtype == MEDIASUBTYPE_YVYU) {
		
		
		encodeYVYUToYV12(inBuf, inNumBytes);
	} else if (mPinInputType.subtype == MEDIASUBTYPE_IYUV) {
		
		
		encodeIYUVToYV12(inBuf, inNumBytes);

		
	} else {

		//FATAL ERROR
		throw 0;
	}
	

	StampedOggPacket* locPacket = mTheoraEncoder.encodeTheora(&mYUV);
	if (locPacket == NULL) {
		//debugLog<<"Encode returns NULL"<<endl;
		return S_FALSE;
	}
	locFrameEnd		= mUptoFrame 
					= locPacket->endTime();
	//debugLog<<"Delivering..."<<endl;
	return deliverData(locFrameStart, locFrameEnd, locPacket->packetData(), locPacket->packetSize());

}
bool TheoraEncodeInputPin::ConstructCodec() {

	//debugLog<<"Contructing codec..."<<endl;
	theora_info_init(&mTheoraInfo);
	
	//Round up to multiple of 16 for theora

	//---------------------------------------------------------------------------------------------------------------
	//mTheoraInfo values
	//==================
	//width, height					-	/16 up rounded values, size of the outer frame
	//frame_width, frame_height		-	size of the inner picture region
	//offset_x						-	Distance at bottom left from frame to picture  <= width - frame_width
	//offset_y						-	Distance at bottom left from frame to picture  <= height - frame_height

	//mYUV values - for YV12 format
	//=============================
	//y_stride						-	Equal to the /16 up rounded wdith values
	//y_width						-	Equal to the /16 up rounded wdith values
	//uv_stride						-	Equal to *half* the /16 up rounded width values
	//uv_width						-	Equal to *half* the /16 up rounded wdith values
	//y_height						-	Equal to the /16 up rounded height value
	//uv_height						-	Equal to *half* the /16 up rounded height value
	//y								-	Buffer of size y_stride*y_height (/16 up rounded values)
	//u,v							-	Buffers each *quarter* the size of the y buffer (/16 up rounded values)

	//Member data
	//===========
	//mWidth						-	raw video source values... equal to frame_width
	//mHeight						-	raw video source values... equal to frame_height
	//mXOffset						-	x offset
	//mYOffset						-	y offset
	//---------------------------------------------------------------------------------------------------------------

	//Width of the outer frame
	mTheoraInfo.width			=	mYUV.y_width
								=	mYUV.y_stride
								=	(((mVideoFormat->bmiHeader.biWidth + 15)>>4)<<4);

	//Width of the inner picture
	mTheoraInfo.frame_width		=	mWidth
								=	mVideoFormat->bmiHeader.biWidth;

	//YUV U&V data
	mYUV.uv_width				=	mYUV.uv_stride
								=	mYUV.y_width/2;

	
	//

	//Height data of outer frame
	mTheoraInfo.height			=	mYUV.y_height
								=	(((mVideoFormat->bmiHeader.biHeight + 15)>>4)<<4);

	//Height of the inner picture
	mTheoraInfo.frame_height	=	mHeight
								=	mVideoFormat->bmiHeader.biHeight;

	mYUV.uv_height				=	mYUV.y_height/2;

	
	//

	//Set offset values... no longer centred... all the offset is at the bottom left of the image (ie very start of memory image)
	//Difference between the outframe dimensions and the inner picture dimensions
	mTheoraInfo.offset_x		=	mXOffset
								=	(mWidth - mTheoraInfo.frame_width);

	mTheoraInfo.offset_y		=	mYOffset
								=	(mHeight - mTheoraInfo.frame_height);
	
	
	unsigned long locYBuffSize = mYUV.y_height * mYUV.y_stride;
	mYUV.y				=	new char[locYBuffSize];
	mYUV.u				=	new char[locYBuffSize/4];
	mYUV.v				=	new char[locYBuffSize/4];

	//End YV12 specifics
	//

	//HACK:::Bit of a hack to convert dshow nanos to a fps num/denom.
	//Now we multiply the numerator and denom by 1000, this gives us 3 d.p. of precision for framerate.
	unsigned long locNum = (((double)10000000 * 1000) / ((double)mVideoFormat->AvgTimePerFrame)) + (double)0.5;

	//debugLog<<"FPS = "<<locNum<<endl;
	mTheoraInfo.fps_numerator = locNum;
	mTheoraInfo.fps_denominator = 1000;
	
	mTheoraInfo.aspect_numerator=0;
	mTheoraInfo.aspect_denominator=0;
	
	mTheoraInfo.colorspace=OC_CS_UNSPECIFIED;		//YV12
	mTheoraInfo.target_bitrate=400000; //mVideoFormat->dwBitRate;

	//Hard code for now
	mTheoraInfo.quality=30; //video_q;

	mTheoraInfo.dropframes_p=0;
	mTheoraInfo.quick_p=1;
	mTheoraInfo.keyframe_auto_p=1;
	mTheoraInfo.keyframe_frequency=64;   //If you change this... change the logged value below
	mTheoraInfo.keyframe_frequency_force=64;  //ditto
	mTheoraInfo.keyframe_data_target_bitrate=mTheoraInfo.target_bitrate*1.5;
	mTheoraInfo.keyframe_auto_threshold=80;
	mTheoraInfo.keyframe_mindistance=8;
	mTheoraInfo.noise_sensitivity=1; 

	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.frameRateNumerator = mTheoraInfo.fps_numerator;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.frameRateDenominator = mTheoraInfo.fps_denominator;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.maxKeyframeInterval = 6;   //log2(keyframe_freq) from above
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.pictureHeight = mHeight;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.pictureWidth = mWidth;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.colourSpace = OC_CS_UNSPECIFIED;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.outerFrameHeight = mTheoraInfo.height;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.outerFrameWidth = mTheoraInfo.width;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.xOffset = mXOffset;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.yOffset = mYOffset;

	//TODO ::: DO something about aspect ratios
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.aspectDenominator = 0;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.aspectNumerator = 0;

	return true;

}
void TheoraEncodeInputPin::DestroyCodec() {
	//fish_sound_delete(mFishSound);
	//mFishSound = NULL;
}



HRESULT TheoraEncodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	//AbstractVideoEncodeInputPin::SetMediaType(inMediaType);

	if  (	inMediaType->subtype == MEDIASUBTYPE_YV12 || 
			inMediaType->subtype == MEDIASUBTYPE_IYUV ||
			inMediaType->subtype == MEDIASUBTYPE_YUY2 ||
			inMediaType->subtype == MEDIASUBTYPE_UYVY ||
			inMediaType->subtype == MEDIASUBTYPE_YVYU ||
			inMediaType->subtype == MEDIASUBTYPE_AYUV ||
			inMediaType->subtype == MEDIASUBTYPE_RGB32 ||
			inMediaType->subtype == MEDIASUBTYPE_RGB24
	) {
		mVideoFormat = (VIDEOINFOHEADER*)inMediaType->pbFormat;
		mPinInputType = *inMediaType;
		//mParentFilter->mAudioFormat = AbstractAudioDecodeFilter::VORBIS;
	} else {
		//Failed... should never be here !
		throw 0;
	}
	ConstructCodec();
	return CBaseInputPin::SetMediaType(inMediaType);



	return S_OK;
	
}

theora_info* TheoraEncodeInputPin::theoraInfo() {
	return &mTheoraInfo;
}