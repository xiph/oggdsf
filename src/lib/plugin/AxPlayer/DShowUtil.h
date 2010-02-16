//===========================================================================
// Copyright (C) 2010 Cristian Adam
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
//- Neither the name of Cristian Adam nor the names of contributors
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
//==============================================================

#ifndef DSHOWUTIL_H
#define DSHOWUTIL_H

class RunningObjectTable
{
public:
	RunningObjectTable(); 
	virtual ~RunningObjectTable();

	void Add(const CComPtr<IFilterGraph2>& filterGraph);
	void Remove(); 

private:
	DWORD m_rotID;
};

class DShowUtil
{
public:
	static CComPtr<IPin> FindPin(const CComPtr<IBaseFilter>& filter, 
							PIN_DIRECTION desiredDirection, 
							unsigned int pinNumber = 0);

	static CComPtr<IBaseFilter> AddFilterFromCLSID(const CComPtr<IFilterGraph2>& graphBuilder, 
									const GUID filterClsid, 
									const wchar_t* filterName);

	static void RemoveFilters(const CComPtr<IFilterGraph2>& graphBuilder);

	static void SaveGraph(const CComPtr<IFilterGraph2>& graphBuilder, const CString& path);
	static void LoadGraph(const CComPtr<IFilterGraph2>& m_graphBuilder, const CString& path);
    static CString DShowUtil::GetEventCodeString(long eventCode);
};

#endif // DSHOWUTIL_H
