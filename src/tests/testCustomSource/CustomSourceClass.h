//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#pragma once

#include <fstream>
#include <string>

using namespace std;
#include "IFilterDataSource.h"

//This class is just a copy of FilterFileSource class that the demux uses, It's just an example
//	of the most basic source of data. It's intended only for random access files.
//The custom source interface on the filter as at Mar 2006, has no mechanism to allow
//	other sorts of data, like forward only streams. It's just an experiment for now.
//It's most useful if you have some media files in your application you don't want the
//  user to be able to touch. In this case you can decrypt/unpack/read directly from the
//  applications data and feed into the demux.
class CustomSourceClass
	:	public IFilterDataSource
{
public:
	CustomSourceClass(void);
	virtual ~CustomSourceClass(void);

	//IFilterDataSource Interface
	virtual unsigned long seek(unsigned long inPos);
	virtual void close();
	virtual bool open(string inSourceLocation, unsigned long inStartByte = 0);
	virtual void clear();
	virtual bool isEOF();
	virtual bool isError()								{	return false;	}
	virtual unsigned long read(char* outBuffer, unsigned long inNumBytes);
	virtual string shouldRetryAt()						{		return "";		}
	//

protected:
	fstream mSourceFile;
};
