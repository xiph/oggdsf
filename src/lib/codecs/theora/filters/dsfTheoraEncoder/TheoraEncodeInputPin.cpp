//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2008 Cristian Adam
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

/* Define to adapt theora frame rate to DirectShow clock */
#define ADAPT_FRAME_RATE 1


TheoraEncodeInputPin::TheoraEncodeInputPin(AbstractTransformFilter* inParentFilter, CCritSec* inFilterLock, AbstractTransformOutputPin* inOutputPin, vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformInputPin(inParentFilter, inFilterLock, inOutputPin, NAME("TheoraEncodeInputPin"), L"Theora In", inAcceptableMediaTypes)
	,	m_xOffset(0)
	,	m_yOffset(0)
	,	m_height(0)
	,	m_width(0)
	,	m_uptoFrame(0)
	,	m_hasBegun(false)
	,	m_numFrames(0)
    ,   m_flipImageVerticaly(false)

{
	//debugLog.open("g:\\logs\\theoencfiltinput.log", ios_base::out);
	m_yuv.y = NULL;
	m_yuv.u = NULL;
	m_yuv.v = NULL;
}

TheoraEncodeInputPin::~TheoraEncodeInputPin(void)
{
	//debugLog.close();
	DestroyCodec();
	delete[] m_yuv.y;
	delete[] m_yuv.u;
	delete[] m_yuv.v;

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

	//NOTE	: m_height, m_width are the actual video sizes and are the sizes of the incoming buffer
	//		  The yuv width, height parameters are a /16 up rounded size of the output buffer to be sent to libtheora


	//Setup the source pointer
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr
	//
	
	//========
	//Y DATA |
	//=========================================================================================================

	//Setup the destination pointer
	unsigned char* locDestUptoPtr = m_yuv.y;
	//

	////Pad top  of output Y plane buffer with m_yOffset lines of width m_yuv.y_width
	////y_width is the out frame width.
	//if (m_yOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, m_yOffset * m_yuv.y_width);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += (m_yOffset * m_yuv.y_width);							//I'm fairly sure it can be junk... but leave for now !
	//}
	//Source pointer does not advance
	//

	//Add m_height lines of data of width m_width plus padding of m_xOffset at the start of each line
	if (m_xOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		//
		//This branch of the condition does exactly the same as the else branch where m_xOffset = 0
		for (unsigned long line = 0; line < m_height; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width);
			locSourceUptoPtr += m_width;
			locDestUptoPtr += m_width;
		}
	} else {
		for (unsigned long line = 0; line < m_height; line++) {
			//Pad the start of the line with m_xOffset bytes
			memset((void*)locDestUptoPtr, NULL, m_xOffset);
			locDestUptoPtr += m_xOffset;

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width);
			locSourceUptoPtr += m_width;
			locDestUptoPtr += m_width;

			//
			//This is no longer done... we just pad on one side for efficiency
			//
			//Pad the end of the line with m_xOffset bytes
			//memset((void*)locDestUptoPtr, NULL, m_xOffset);
			//locDestUptoPtr += m_xOffset;
		}

	}

	
	//Pad bottom of Y plane buffer with m_yOffset lines of width m_yuv.y_width
	if (m_yOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, m_yOffset * m_yuv.y_width);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += (m_yOffset * m_yuv.y_width);
		//Source pointer does not advance
	}
	


	//========
	//V DATA |
	//=========================================================================================================

	//Set the destination poitner
	locDestUptoPtr = m_yuv.v;
	//

	//ASSERT (m_yOffset is EVEN)
	//ASSERT (m_height is EVEN)
	//ASSERT (m_width is EVEN)
	//

	////Pad top of V plane buffer with m_yOffset/2 lines of width m_yuv.uv_width
	//if (m_yOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	////

	//Add m_height/2 lines of data of length m_width/2 plus padded by m_xOffset/2 at each end
	if (m_xOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (unsigned long line = 0; line < m_height / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);
		}
	} else {
		for (unsigned long line = 0; line < m_height / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			locDestUptoPtr += (m_xOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);

			//
			//This is no longer done... we just pad on one side for efficiency
			//

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			//locDestUptoPtr += (m_xOffset / 2);
			////Source pointer does not advance
		}

	}


	//Pad bottom of V plane buffer with m_yOffset / 2 lines of width m_yuv.uv_width
	if (m_yOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
		//Source pointer does not advance
	}
	




	//========
	//U DATA |
	//=========================================================================================================

	//Set the destination pointer
	locDestUptoPtr = m_yuv.u;
	//

	////Pad top of U plane buffer with m_yOffset/2 lines of width m_yuv.uv_width
	//if (m_yOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	////

	//Add m_height/2 lines of data of length m_width/2 plus padded by m_xOffset/2 at each end
	if (m_xOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (unsigned long line = 0; line < m_height / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);
		}
	} else {
		for (unsigned long line = 0; line < m_height / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			locDestUptoPtr += (m_xOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);

			//
			//This is no longer done... we just pad on one side for efficiency
			//

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			//locDestUptoPtr += (m_xOffset / 2);
			////Source pointer does not advance
		}

	}

	
	//Pad bottom of U plane buffer with m_yOffset / 2 lines of width m_yuv.uv_width
	if (m_yOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
		//Source pointer does not advance
	}

	//======================================================================================================
	return 0;

}

//-------------------------------------------------------------------------

long TheoraEncodeInputPin::encodeIYUVToYV12(unsigned char* inBuf, long inNumBytes) {
	//
	// Now properly pads on one side only... 20/12/04
	//

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
	


	//NOTE	: m_height, m_width are the actual video sizes and are the sizes of the incoming buffer
	//		  The yuv width, height parameters are a /16 up rounded size of the output buffer to be sent to libtheora


	//Setup the source pointer
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr
	//
	
	//========
	//Y DATA |
	//=========================================================================================================

	//Setup the destination pointer
	unsigned char* locDestUptoPtr = m_yuv.y;
	//

	////Pad top of Y plane buffer with m_yOffset lines of width m_yuv.y_width
	//if (m_yOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, m_yOffset * m_yuv.y_width);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += (m_yOffset * m_yuv.y_width);
	//}
	//Source pointer does not advance
	//

	//Add m_height lines of data of width m_width plus padding of m_xOffset at each end
	if (m_xOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (unsigned long line = 0; line < m_height; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width);
			locSourceUptoPtr += m_width;
			locDestUptoPtr += m_width;
		}
	} else {
		for (unsigned long line = 0; line < m_height; line++) {
			//Pad the start of the line with m_xOffset bytes
			memset((void*)locDestUptoPtr, NULL, m_xOffset);
			locDestUptoPtr += m_xOffset;

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width);
			locSourceUptoPtr += m_width;
			locDestUptoPtr += m_width;

			////Pad the end of the line with m_xOffset bytes
			//memset((void*)locDestUptoPtr, NULL, m_xOffset);
			//locDestUptoPtr += m_xOffset;
		}

	}

	//Pad bottom of Y plane buffer with m_yOffset lines of width m_yuv.y_width
	if (m_yOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, m_yOffset * m_yuv.y_width);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += (m_yOffset * m_yuv.y_width);
		//Source pointer does not advance
	}

	//========
	//U DATA |
	//=========================================================================================================

	//Set the destination pointer
	locDestUptoPtr = m_yuv.u;
	//

	////Pad top of U plane buffer with m_yOffset/2 lines of width m_yuv.uv_width
	//if (m_yOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	//

	//Add m_height/2 lines of data of length m_width/2 plus padded by m_xOffset/2 at each end
	if (m_xOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (unsigned long line = 0; line < m_height / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);
		}
	} else {
		for (unsigned long line = 0; line < m_height / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			locDestUptoPtr += (m_xOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			//locDestUptoPtr += (m_xOffset / 2);
			////Source pointer does not advance
		}

	}

	//Pad bottom of U plane buffer with m_yOffset / 2 lines of width m_yuv.uv_width
	if (m_yOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
		//Source pointer does not advance
	}
	


	//========
	//V DATA |
	//=========================================================================================================

	//Set the destination poitner
	locDestUptoPtr = m_yuv.v;
	//

	////Pad top of V plane buffer with m_yOffset/2 lines of width m_yuv.uv_width
	//if (m_yOffset != 0) {
	//	memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
	//	locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
	//	//Source pointer does not advance
	//}
	////

	//Add m_height/2 lines of data of length m_width/2 plus padded by m_xOffset/2 at each end
	if (m_xOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (unsigned long line = 0; line < m_height / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);
		}
	} else {
		for (unsigned long line = 0; line < m_height / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			locDestUptoPtr += (m_xOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, m_width / 2);
			locSourceUptoPtr += (m_width / 2);
			locDestUptoPtr += (m_width / 2);

			////Pad the end of the line
			//memset((void*)locDestUptoPtr, NULL, m_xOffset / 2);
			//locDestUptoPtr += (m_xOffset / 2);
			////Source pointer does not advance
		}

	}

	//Pad bottom of V plane buffer with m_yOffset / 2 lines of width m_yuv.uv_width
	if (m_yOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (m_yOffset * m_yuv.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((m_yOffset * m_yuv.uv_width) / 2);
		//Source pointer does not advance
	}
	





	//======================================================================================================
	return 0;

}
//-------------------------------------------------------------------------

long TheoraEncodeInputPin::encodeRGB24toYV12(unsigned char* inBuf, long inNumBytes) 
{
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

    //Blue Green Red Blue Green Red.
    unsigned long numPixels = inNumBytes / 3;

    std::vector<unsigned char> ayuvBuf;
    ayuvBuf.resize(numPixels * 4);


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
    unsigned char* pDest = &*ayuvBuf.begin();

    //SOURCE: Blue Green Red Blue Green Red.
    //DEST: v u y a

    unsigned char* pSourceEnds = inBuf + inNumBytes;

    unsigned char* pSource = 0;
    unsigned char* pEnd = 0;

    long stride = 0;

    if (m_flipImageVerticaly)
    {
        stride = m_width * 3;
        pSource = inBuf;
        pEnd = pSourceEnds;
    }
    else
    {
        // Negative stride
        stride = 0 - m_width * 3;
        pSource = pSourceEnds - std::abs(stride);
        pEnd = inBuf - std::abs(stride);
    }

    for (; pSource != pEnd; pSource += stride) 
    {
        unsigned char* pColSource = pSource;
        unsigned char* pColEnd = pColSource + std::abs(stride);

        while (pColSource < pColEnd) 
        {
            locB = pColSource[0];					//Blue
            locL = KB * (locB);						//Blue

            locL += G_FACTOR * (pColSource[1]);	    //Green

            locR = pColSource[2];					//Red
            locL += KR * (locR);					//Red

            *(pDest++) = CLIP3(0, 255, ((112 * ( (locR<<16) - locL)) / V_FACTOR) + 128);	//V for Victor
            *(pDest++) = CLIP3(0, 255, ((112 * ( (locB<<16) - locL)) / U_FACTOR) + 128);	//U for ugly
            *(pDest++) = CLIP3(0, 255, locL >> 16);											//Y for yellow
            *(pDest++) = 255;														        //A for alpha

            pColSource += 3;
        }
    }

    encodeAYUVtoYV12(&*ayuvBuf.begin(), ayuvBuf.size());

	return 0;
}



long TheoraEncodeInputPin::encodeRGB32toYV12(unsigned char* inBuf, long inNumBytes) 
{
	std::vector<unsigned char> ayuvBuf;
    ayuvBuf.resize(inNumBytes);

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
	unsigned char* pDest = &*ayuvBuf.begin();

    //SOURCE: Blue Green Red Blue Green Red.
	//DEST: v u y a

	unsigned char* pSourceEnds = inBuf + inNumBytes;

    unsigned char* pSource = 0;
    unsigned char* pEnd = 0;

    long stride = 0;

    if (m_flipImageVerticaly)
    {
        stride = m_width * 4;
        pSource = inBuf;
        pEnd = pSourceEnds;
    }
    else
    {
        // Negative stride
	    stride = 0 - m_width * 4;
        pSource = pSourceEnds - std::abs(stride);
        pEnd = inBuf - std::abs(stride);
    }

    for (; pSource != pEnd; pSource += stride) 
    {
        unsigned char* pColSource = pSource;
        unsigned char* pColEnd = pColSource + std::abs(stride);

        while (pColSource < pColEnd) 
        {
            locB = pColSource[0];					//Blue
            locL = KB * (locB);						//Blue

            locL += G_FACTOR * (pColSource[1]);	    //Green

            locR = pColSource[2];					//Red
            locL += KR * (locR);					//Red

            *(pDest++) = CLIP3(0, 255, ((112 * ( (locR<<16) - locL)) / V_FACTOR) + 128);	//V for Victor
            *(pDest++) = CLIP3(0, 255, ((112 * ( (locB<<16) - locL)) / U_FACTOR) + 128);	//U for ugly
            *(pDest++) = CLIP3(0, 255, locL >> 16);											//Y for yellow
            *(pDest++) = pColSource[3];														//A for alpha

            pColSource += 4;
        }
    }
	
	encodeAYUVtoYV12(&*ayuvBuf.begin(), ayuvBuf.size());
	
	return 0;
}



long TheoraEncodeInputPin::encodeAYUVtoYV12(unsigned char* inBuf, long inNumBytes) {

	//TODO::: This doesn't appear to do offsets.

	//Victor Ugly Yellow Alpha --fonts are fuzzy late at night-- (Yellow is not colour yellow)

	//AYUV is VUYA VUYA VUYA VUYA 4:4:4 sampling
	// Twice the vertical and horizontal sampling of YV12 in chrominance

	//Strategy : Process two lines and 2 cols at a time averaging 4 U and V around the position where a
	// YV12 chroma sample will be... leave luminance samples... ignore alpha samples

	//debugLog<<"Encode AYUV To YV12 :"<<endl;
	
	const int PIXEL_BYTE_SIZE = 4;
	ASSERT (m_height % 2 == 0);
	ASSERT (m_width % 2 == 0);

	unsigned char* locSourceUptoPtr = inBuf;						//View only... don't delete locUptoPtr
	unsigned char* locSourceNextLine = locSourceUptoPtr + (m_width * PIXEL_BYTE_SIZE);	//View only don't delete
	
	unsigned char* locYUpto = m_yuv.y;
	unsigned char* locUUpto = m_yuv.u;
	unsigned char* locVUpto = m_yuv.v;
	//Pointer to the same pixel on next line
	unsigned char* locDestNextLine = locYUpto + (m_yuv.y_stride);				//View only... don't delete

	int temp = 0;

	//Process 2 lines at a time
	for (unsigned long line = 0; line < m_height; line += 2) {
		//debugLog<<"Encode AYUV To YV12 : ++ Line = "<<line<<endl;
		
		//Ensures the current destination buffer skips a line ie points to line 2, 4, 6 etc
		locYUpto = (m_yuv.y + (line * m_yuv.y_stride));
		//Ensures the nextlinedest skips a
		locDestNextLine = locYUpto + (m_yuv.y_stride);

		//locUUpto = (m_yuv.u + ((line/2) * m_yuv.uv_stride));
		//locVUpto = (m_yuv.v + ((line/2) * m_yuv.uv_stride));

		ASSERT (locSourceUptoPtr == (inBuf + (line * (m_width * PIXEL_BYTE_SIZE))));
		ASSERT (locSourceNextLine == locSourceUptoPtr + (m_width * PIXEL_BYTE_SIZE));
		ASSERT (locYUpto == (m_yuv.y + (line * m_yuv.y_stride)));
		ASSERT (locDestNextLine == locYUpto + (m_yuv.y_stride));
		

		//Pad out the start of the line if needed
		if (m_xOffset != 0) {
			memset((void*)locYUpto, 0, m_xOffset);
			memset((void*)locDestNextLine, 0, m_xOffset);
			memset((void*)locUUpto, 0, m_xOffset/2);
			memset((void*)locVUpto, 0, m_xOffset/2);
			locYUpto += m_xOffset;
			locDestNextLine += m_xOffset;
			locUUpto += (m_xOffset/2);
			locVUpto += (m_xOffset/2);
		}
		//Columns also done 2 at a time
		for (unsigned long col = 0; col < m_width; col += 2) {
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
		//Sourceupto    by		= 8 bytes * (m_width in pixels / 2 pixels at a time) * 2 lines
		//						= 8 * m_width
		//						= 2 lines of 4 byte pixels of pixel width m_width
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
		locSourceNextLine += (m_width * PIXEL_BYTE_SIZE);



	}
	return 0;
}
long TheoraEncodeInputPin::encodeYUY2ToYV12(unsigned char* inBuf, long inNumBytes) {

	//TODO::: This doesn't do offsets properly.
	
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr

	//YUY2 is Y0 U0 Y1 V0 Y2 U1 Y3 V1
	// it has twice as much sampling height as YV12 so downsample it.

	unsigned char* locYUpto = m_yuv.y;
	unsigned char* locUUpto = m_yuv.u;
	unsigned char* locVUpto = m_yuv.v;

	

	//After downsampling... from each block of 8, we get 4 y samples and 1 each of u and v


	for (unsigned long i = 0; i < m_height / 2; i++) {
		//TO DO: Draw memory layouts.

		//***Part of the average method... store the pointer to the last of the previous line
		//locLastUUpto = locUUpto;
		//locLastVUpto = locVUpto;
		//***

		for (unsigned long j = 0; j < m_width / 2; j++) {
			*(locYUpto++) = *(locSourceUptoPtr++);
			*(locUUpto++) = *(locSourceUptoPtr++);
			*(locYUpto++) = *(locSourceUptoPtr++);
			*(locVUpto++) = *(locSourceUptoPtr++);
		}

		
		//***Drop line method
		for (unsigned long j = 0; j < m_width / 2; j++) {
			//Ignore the second line
			*(locYUpto++) = *(locSourceUptoPtr++);
			locSourceUptoPtr++;
			*(locYUpto++) = *(locSourceUptoPtr++);
			locSourceUptoPtr++;
		}
		//***

		//*** PArt of the Alternate method to average...
		//for (int j = 0; j < m_width / 2; j++) {
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

	unsigned char* locYUpto = m_yuv.y;
	unsigned char* locUUpto = m_yuv.u;
	unsigned char* locVUpto = m_yuv.v;

	

	//After downsampling... from each block of 8, we get 4 y samples and 1 each of u and v


	for (unsigned long i = 0; i < m_height / 2; i++) {
		//TO DO: Draw memory layouts.

	

		for (unsigned long j = 0; j < m_width / 2; j++) {
			*(locYUpto++) = *(locSourceUptoPtr++);				//Y for Yellow
			*(locVUpto++) = *(locSourceUptoPtr++);				//V for victor
			
			*(locYUpto++) = *(locSourceUptoPtr++);				//Y for Yellow
			*(locUUpto++) = *(locSourceUptoPtr++);				//U for Ugly
		}

		
		//***Drop line method
		for (unsigned long j = 0; j < m_width / 2; j++) {
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

	unsigned char* locYUpto = m_yuv.y;
	unsigned char* locUUpto = m_yuv.u;
	unsigned char* locVUpto = m_yuv.v;

	

	//After downsampling... from each block of 8, we get 4 y samples and 1 each of u and v


	for (unsigned long i = 0; i < m_height / 2; i++) {
		//TO DO: Draw memory layouts.

	
		for (unsigned long j = 0; j < m_width / 2; j++) {
			*(locUUpto++) = *(locSourceUptoPtr++);			//U for Ugly
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for Yellow
			
			*(locVUpto++) = *(locSourceUptoPtr++);			//V for Victor
			*(locYUpto++) = *(locSourceUptoPtr++);			//Y for Yellow
			
		}

		
		//***Drop line method
		for (unsigned long j = 0; j < m_width / 2; j++) {
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
	LONGLONG locFrameStart = m_uptoFrame;
	LONGLONG locFrameEnd = 0;
	HRESULT locHR = S_OK;
	if (!m_hasBegun) {
		//debugLog<<"encodeData : First time"<<endl;
		m_hasBegun = true;
		
		StampedOggPacket** locHeaders;
		locHeaders = m_theoraEncoder.initCodec(m_theoraInfo);

		for (int i = 0; i < 3; i++) {
			locHR = deliverData(0,0,locHeaders[i]->packetData(), locHeaders[i]->packetSize());
			delete locHeaders[i];
			if (locHR != S_OK) {
				return locHR;
			}
		}
	}


	//This big if block merely takes the buffer and copies it into the yuv_buffer structure which
	// is used below to encode.
	if (m_pinInputType.subtype == MEDIASUBTYPE_YUY2) {
		//debugLog<<"About to encode YUY2 to YV12"<<endl;
		encodeYUY2ToYV12(inBuf, inNumBytes);
	} else if (m_pinInputType.subtype == MEDIASUBTYPE_AYUV) {
		encodeAYUVtoYV12(inBuf, inNumBytes);

	} else if (m_pinInputType.subtype == MEDIASUBTYPE_RGB32) {
		encodeRGB32toYV12(inBuf, inNumBytes);

	} else if (m_pinInputType.subtype == MEDIASUBTYPE_RGB24) {
		encodeRGB24toYV12(inBuf, inNumBytes);


	} else if (m_pinInputType.subtype == MEDIASUBTYPE_YV12) {
		//Should be more specifc.
		//debugLog<<"About to encode YV12 to YV12"<<endl;
		encodeYV12ToYV12(inBuf, inNumBytes);
	} else if (m_pinInputType.subtype == MEDIASUBTYPE_UYVY) {
		
		
		encodeUYVYToYV12(inBuf, inNumBytes);

	} else if (m_pinInputType.subtype == MEDIASUBTYPE_YVYU) {
		
		
		encodeYVYUToYV12(inBuf, inNumBytes);
	} else if (m_pinInputType.subtype == MEDIASUBTYPE_IYUV) {
		
		
		encodeIYUVToYV12(inBuf, inNumBytes);

		
	} else {

		//FATAL ERROR
		throw 0;
	}
	
#ifdef ADAPT_FRAME_RATE
	__int64 frameTime = (1000 * m_theoraInfo.fps_denominator) / m_theoraInfo.fps_numerator;
	__int64 curTheoraTime = (m_numFrames * 1000 * m_theoraInfo.fps_denominator) / m_theoraInfo.fps_numerator;

	// Skip a frame if we are too late
	if (curTheoraTime - m_dsTimeStart > frameTime) 
	{
		//debugLog<<" too late: "<< curTheoraTime << "," << timeStart << ":" << frameTime <<endl;
		return S_OK;
	}
#endif


	StampedOggPacket* locPacket = m_theoraEncoder.encodeTheora(&m_yuv);
	if (locPacket == NULL) {
		//debugLog<<"Encode returns NULL"<<endl;
		return S_FALSE;
	}
	locFrameEnd		= m_uptoFrame 
					= locPacket->endTime();
	//debugLog<<"Delivering..."<<endl;

	//We still own the packet after this, we have to delete it.
	locHR = deliverData(locFrameStart, locFrameEnd, locPacket->packetData(), locPacket->packetSize());
	delete locPacket;


#ifdef ADAPT_FRAME_RATE
	if (!FAILED(locHR)) 
	{
		++m_numFrames;
		locHR = encodeMoreFrames();
	}
#endif

	return locHR;
}

HRESULT TheoraEncodeInputPin::encodeMoreFrames()
{
	HRESULT locHR = S_OK;

	__int64 frameTime = (1000 * m_theoraInfo.fps_denominator) / m_theoraInfo.fps_numerator;
	__int64 curTheoraTime = (m_numFrames * 1000 * m_theoraInfo.fps_denominator) / m_theoraInfo.fps_numerator;

	/* Resend previous packet, if we are too fast */
	while (m_dsTimeStart - curTheoraTime > frameTime) 
	{
		LONGLONG locFrameStart = m_uptoFrame;
		LONGLONG locFrameEnd = 0;

		StampedOggPacket* locPacket = m_theoraEncoder.encodeTheora(&m_yuv);
		if (locPacket == NULL) 
		{
			//debugLog<<"Encode returns NULL"<<endl;
			return S_FALSE;
		}
		locFrameEnd	= m_uptoFrame = locPacket->endTime();
		//debugLog<<"Delivering..."<<endl;

		//We still own the packet after this, we have to delete it.
		locHR = deliverData(locFrameStart, locFrameEnd, locPacket->packetData(), locPacket->packetSize());
		delete locPacket;
		if (FAILED(locHR)) 
		{
			break;
		}

		++m_numFrames;
		curTheoraTime = (m_numFrames * 1000 * m_theoraInfo.fps_denominator) / m_theoraInfo.fps_numerator;
	}

	return locHR;
}

bool TheoraEncodeInputPin::ConstructCodec() 
{
	//debugLog<<"Contructing codec..."<<endl;
	theora_info_init(&m_theoraInfo);
	
	//Round up to multiple of 16 for theora

	//---------------------------------------------------------------------------------------------------------------
	//m_theoraInfo values
	//==================
	//width, height					-	/16 up rounded values, size of the outer frame
	//frame_width, frame_height		-	size of the inner picture region
	//offset_x						-	Distance at bottom left from frame to picture  <= width - frame_width
	//offset_y						-	Distance at bottom left from frame to picture  <= height - frame_height

	//m_yuv values - for YV12 format
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
	//m_width						-	raw video source values... equal to frame_width
	//m_height						-	raw video source values... equal to frame_height
	//m_xOffset						-	x offset
	//m_yOffset						-	y offset
	//---------------------------------------------------------------------------------------------------------------

	//Width of the outer frame
	m_theoraInfo.width			=	m_yuv.y_width
								=	m_yuv.y_stride
								=	(((m_width + 15)>>4)<<4);

	//Width of the inner picture
	m_theoraInfo.frame_width	=	m_width;
								

	//YUV U&V data
	m_yuv.uv_width				=	m_yuv.uv_stride
								=	m_yuv.y_width/2;

	
	//

	//Height data of outer frame
	m_theoraInfo.height			=	m_yuv.y_height
								=	(((m_height + 15)>>4)<<4);

	//Height of the inner picture
	m_theoraInfo.frame_height	=	m_height;

	m_yuv.uv_height				=	m_yuv.y_height/2;

	
	//

	//Set offset values... no longer centred... all the offset is at the bottom left of the image (ie very start of memory image)
	//Difference between the outframe dimensions and the inner picture dimensions
	m_theoraInfo.offset_x		=	m_xOffset
								=	(m_theoraInfo.width - m_theoraInfo.frame_width);

	m_theoraInfo.offset_y		=	m_yOffset
								=	(m_height - m_theoraInfo.frame_height);
	
	
	unsigned long locYBuffSize = m_yuv.y_height * m_yuv.y_stride;
	m_yuv.y				=	new unsigned char[locYBuffSize];
	m_yuv.u				=	new unsigned char[locYBuffSize/4];
	m_yuv.v				=	new unsigned char[locYBuffSize/4];

	//End YV12 specifics
	//

	//HACK:::Bit of a hack to convert dshow nanos to a fps num/denom.
	//Now we multiply the numerator and denom by 1000, this gives us 3 d.p. of precision for framerate.
	unsigned long locNum = (((double)10000000 * 1000) / ((double)m_averageTimePerFrame)) + (double)0.5;

	//debugLog<<"FPS = "<<locNum<<endl;
	m_theoraInfo.fps_numerator = locNum;
	m_theoraInfo.fps_denominator = 1000;
	
	m_theoraInfo.aspect_numerator=0;
	m_theoraInfo.aspect_denominator=0;
	
	m_theoraInfo.colorspace=OC_CS_UNSPECIFIED;		//YV12
	m_theoraInfo.target_bitrate=400000; //m_videoFormat->dwBitRate;

	//Hard code for now
	m_theoraInfo.quality=30; //video_q;

	m_theoraInfo.dropframes_p=0;
	m_theoraInfo.quick_p=1;
	m_theoraInfo.keyframe_auto_p=1;
	m_theoraInfo.keyframe_frequency=64;   //If you change this... change the logged value below
	m_theoraInfo.keyframe_frequency_force=64;  //ditto
	m_theoraInfo.keyframe_data_target_bitrate=m_theoraInfo.target_bitrate*1.5;
	m_theoraInfo.keyframe_auto_threshold=80;
	m_theoraInfo.keyframe_mindistance=8;
	m_theoraInfo.noise_sensitivity=1; 
    m_theoraInfo.pixelformat = OC_PF_420;

	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.frameRateNumerator = m_theoraInfo.fps_numerator;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.frameRateDenominator = m_theoraInfo.fps_denominator;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.maxKeyframeInterval = 6;   //log2(keyframe_freq) from above
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.pictureHeight = m_height;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.pictureWidth = m_width;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.colourSpace = OC_CS_UNSPECIFIED;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.outerFrameHeight = m_theoraInfo.height;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.outerFrameWidth = m_theoraInfo.width;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.xOffset = (unsigned char)m_xOffset;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.yOffset = (unsigned char)m_yOffset;
    ((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.pixelFormat = m_theoraInfo.pixelformat; 

	//TODO ::: DO something about aspect ratios
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.aspectDenominator = 0;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.aspectNumerator = 0;

	return true;

}
void TheoraEncodeInputPin::DestroyCodec() 
{
	//fish_sound_delete(mFishSound);
	//mFishSound = NULL;
}



HRESULT TheoraEncodeInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	DbgLog((LOG_TRACE, 1, L"%s", __FUNCTION__));

	if  (	inMediaType->subtype == MEDIASUBTYPE_YV12 || 
			inMediaType->subtype == MEDIASUBTYPE_IYUV ||
			inMediaType->subtype == MEDIASUBTYPE_YUY2 ||
			inMediaType->subtype == MEDIASUBTYPE_UYVY ||
			inMediaType->subtype == MEDIASUBTYPE_YVYU ||
			inMediaType->subtype == MEDIASUBTYPE_AYUV ||
			inMediaType->subtype == MEDIASUBTYPE_RGB32 ||
			inMediaType->subtype == MEDIASUBTYPE_RGB24
	) 
	{
		if (inMediaType->formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2* videoFormat = (VIDEOINFOHEADER2*)inMediaType->pbFormat;

			m_averageTimePerFrame = videoFormat->AvgTimePerFrame;
			m_width = videoFormat->bmiHeader.biWidth;
            m_height = std::abs(videoFormat->bmiHeader.biHeight);
		}
		else if (inMediaType->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER* videoFormat = (VIDEOINFOHEADER*)inMediaType->pbFormat;

			m_averageTimePerFrame = videoFormat->AvgTimePerFrame;
			m_width = videoFormat->bmiHeader.biWidth;
            m_height = std::abs(videoFormat->bmiHeader.biHeight);
		}

		if (m_averageTimePerFrame == 0)
		{
			// Theora is constant framerate, reject a variable framerate connection.
			return E_FAIL;
		}

		m_pinInputType = *inMediaType;
	} 
	else 
	{
		//Failed... should never be here !
		throw 0;
	}
	ConstructCodec();
	
	return CBaseInputPin::SetMediaType(inMediaType);
}

theora_info* TheoraEncodeInputPin::theoraInfo() 
{
	return &m_theoraInfo;
}

bool TheoraEncodeInputPin::GetFlipImageVerticaly() const
{
    return m_flipImageVerticaly;
}

void TheoraEncodeInputPin::SetFlipImageVerticaly( bool val )
{
    m_flipImageVerticaly = val;
}

StampedOggPacket** TheoraEncodeInputPin::GetCodecHeaders()
{
    return m_theoraEncoder.initCodec(m_theoraInfo);
}
