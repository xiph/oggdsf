/*
   Copyright (C) 2003, 2004 Zentaro Kavanagh
   
   Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
   Organisation (CSIRO) Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of CSIRO Australia nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
//These are the original callbacks... need to provide an interface that does not 
//include the C structs and hopefully no void*
//
//typedef int (*CMMLReadStreamCB)  (CMML *cmml, const CMML_Stream *stream, void * user_data);
//typedef int (*CMMLReadHeadCB)   (CMML *cmml, const CMML_Head *head, void * user_data);
//typedef int (*CMMLReadClipCB) (CMML *cmml, const CMML_Clip *clip, void * user_data);

//STL Include Files
#include <string>
using namespace std;

//Local Include Files
#include "C_CMMLPreamble.h"
#include "C_CMMLRootTag.h"
//#include "C_CMMLError.h"


//typedef int (*CMMLReadStreamCB)  (const C_StreamTag *inStream, void * user_data);
//typedef int (*CMMLReadHeadCB)   (const C_HeadTag *inHead, void * user_data);
//typedef int (*CMMLReadClipCB) (const C_ClipTag *inClip, void * user_data);


class LIBCMMLTAGS_API C_CMMLDoc
{
public:
	//Constructors
	C_CMMLDoc(void);
	virtual ~C_CMMLDoc(void);

	C_CMMLPreamble* preamble();
	C_CMMLRootTag* root();

	void setRoot(C_CMMLRootTag* inRootTag);

	virtual string toString();
	C_CMMLDoc* clone();
	

	//Opening files
	//bool open(string inFilename);
	//bool open(string inFilename, CMMLReadStreamCB inStreamCB, CMMLReadHeadCB inHeadCB, CMMLReadClipCB inClipCB);
	//ISSUE ::: Void pointer on interface...
	//bool open(string inFilename, CMMLReadStreamCB inStreamCB, CMMLReadHeadCB inHeadCB, CMMLReadClipCB inClipCB, void* inUserData);
	//ISSUE ::: Find a way to allow the passing in of a fstream
	
	

	//Close the file. Releases resources but leaves the object capable of handling
	//another file.
	//void close();

	//Load all the callbacks if they were unknown at open
	// or if they have to be changed mid parse.
	//bool setCallbacks(CMMLReadStreamCB inStreamCB, CMMLReadHeadCB inHeadCB, CMMLReadClipCB inClipCB, void* inUserData);

	//Ask the underlying library to read a certain number of bytes
	//void read(unsigned long inNumBytes);

	//The the underlying library to read to the end of the file.
	// This is useful to do initially to read the entire file
	//void readToEnd();

	//Resets the filestream, removes all stored tag data, but DOES NOT
	// remove the file stream or the callbacks
	//void resetFile();

	//Removes the associated file, removes all stored tag data, resets all the
	// callbacks. This brings it back to it's initial state as it was when
	// constructed with no parameters before it was attached to any files or callbacks.

	//Currently just clear all tags by deleting them
	//void clearAll();

	//Accessors
	//Is the file ready to go. ie has a file been attached and opened.
	//bool isStreamReady();

	//NO READ OPERATIONS CAN TAKE PLACE WHILE EITHER OF THE TWO FOLLOWING REPORT TRUE

	//Is it at the end of the file
	//bool isEOF();

	//Is there an error reading the stream.
	//bool isStreamError();

	//Returns a pointer to the last CMML error. Or NULL if no error.
	//These are parsing errors. They ARE NOT the same as the stream errors.
	//They represent badly formatted documents not errors in reading the data.
	//You shouldn't call this unless CMMLError is true.
	//C_CMMLError* lastCMMLError();

	//Returns if there is a CMML error pending.
	//bool isCMMLError();

	//ALL THE FOLLOWING WILL RETURN NULL IF THEY DON"T EXIST OR HAVE NOT BEEN 
	// READ YET.
	
	//Returns a pointer to the list of clips already read. Should never return NULL
	// As each new clip is read it is appended to the end of the clip list.
	// The clip list object can tell you about how many clips have been read
	// and you can access them by number.
	//ISSUE ::: Should they also be able to be retrieved by id ??
	//C_ClipTagList* clipList();

	//Returns a pointer to the most recently read clip or if the entire file is
	// read the last clip in the file
	//C_ClipTag* lastClip();

	//Returns a pointer to the second last clip that waas read or if the entire file
	// has been read, the previous clip in the file.
	//C_ClipTag* previousClip();




protected:
	C_CMMLPreamble* mPreamble;
	C_CMMLRootTag* mRoot;


	//bool mIsStreamReady;
	//bool mIsEOF;
	//bool mIsStreamError;



	//C_CMMLError* mLastError;
	
};
