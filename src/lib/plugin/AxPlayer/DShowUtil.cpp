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

#include "stdafx.h"
#include <dshow.h>
#include "DShowUtil.h"
#include "Guid.h"
#include <map>

#include <vfwmsgs.h>

RunningObjectTable::RunningObjectTable() :
m_rotID(0)
{
}


RunningObjectTable::~RunningObjectTable()
{
	Remove();
}

void RunningObjectTable::Add(const CComPtr<IFilterGraph2>& filterGraph)
{
	if (m_rotID)
	{
		LOG(logERROR) << "Filter Graph already added to Running Object Table, ID is not null!";
		return;
	}

	CComPtr<IMoniker> moniker;
	CComPtr<IRunningObjectTable> rot;

	CHECK_HR(::GetRunningObjectTable(0, &rot));

	CString monikerName;
	monikerName.Format(L"FilterGraph %08p pid %08x", (DWORD_PTR)filterGraph.p, ::GetCurrentProcessId());

	CHECK_HR(::CreateItemMoniker(L"!", monikerName, &moniker));

	CHECK_HR(rot->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, filterGraph, moniker, &m_rotID));
}

void RunningObjectTable::Remove()
{
	CComPtr<IRunningObjectTable> rot;

	CHECK_HR(::GetRunningObjectTable(0, &rot));

	if (m_rotID != 0)
	{
		CHECK_HR(rot->Revoke(m_rotID));
		m_rotID = 0;
	}
}

CComPtr<IPin> DShowUtil::FindPin(const CComPtr<IBaseFilter>& filter, PIN_DIRECTION desiredDirection, 
								   unsigned int pinNumber /*= 0*/)
{
	CComPtr<IEnumPins> enumerator;
	CHECK_HR(filter->EnumPins(&enumerator));

	CComPtr<IPin> pin;

	unsigned int pinCounter = 0;

	unsigned long fetched = 0;
	while (enumerator->Next(1, &pin, &fetched) == S_OK)
	{
		PIN_DIRECTION direction;
		CHECK_HR(pin->QueryDirection(&direction));

		if (direction == desiredDirection)
		{
			if (pinCounter++ != pinNumber)
			{
				pin = 0;
				continue;
			}

			return pin;
		}
		pin = 0;
	}

	return 0;
}

CComPtr<IBaseFilter> DShowUtil::AddFilterFromCLSID(const CComPtr<IFilterGraph2>& graphBuilder, 
												   const GUID filterClsid, const wchar_t* filterName)
{
	CComPtr<IBaseFilter> filter;
	HRESULT hr = filter.CoCreateInstance(filterClsid);
	if (FAILED(hr))
	{
		LOG(logERROR) << "Failed to create filter instance: " << filterClsid 
			<< ", error: 0x" << std::hex << hr;

		AtlThrow(hr);
	}

	hr = graphBuilder->AddFilter(filter, filterName);
	if (FAILED(hr))
	{
		LOG(logERROR) << "Failed to add filter " << filterName << " to graph, error: 0x" << std::hex << hr;
		AtlThrow(hr);
	}

	return filter;
}

void DShowUtil::RemoveFilters(const CComPtr<IFilterGraph2>& graphBuilder)
{
	CComPtr<IEnumFilters> enumFilters;
	CHECK_HR(graphBuilder->EnumFilters(&enumFilters));

	CComPtr<IBaseFilter> filter;
	ULONG fetched = 0;
	while (enumFilters->Next(1, &filter, &fetched) == S_OK)
	{
		CHECK_HR(graphBuilder->RemoveFilter(filter));
		filter = 0;
	}
}

void DShowUtil::SaveGraph(const CComPtr<IFilterGraph2>& graphBuilder, const CString& path)
{
	try
	{
		const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 

		CComPtr<IStorage> storage;
		CHECK_HR(StgCreateDocfile(path,
			STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
			0, &storage));

		CComPtr<IStream> stream;
		CHECK_HR(storage->CreateStream(wszStreamName,
			STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
			0, 0, &stream));

		CComPtr<IPersistStream> persist;
		CHECK_HR(graphBuilder.QueryInterface(&persist));

		CHECK_HR(persist->Save(stream, TRUE));

		stream = 0;
		persist = 0;

		CHECK_HR(storage->Commit(STGC_DEFAULT));
	}
	catch (const CAtlException& except)
	{
		except.m_hr;
	}
}

void DShowUtil::LoadGraph(const CComPtr<IFilterGraph2>& m_graphBuilder, const CString& path)
{
	try
	{
		CComPtr<IStorage> storage;
		if (StgIsStorageFile(path) != S_OK) 
		{
			LOG(logERROR) << "\"" << path << "\" is not a valid storage file!";
			return;
		}

		CHECK_HR(StgOpenStorage(path, 0, STGM_TRANSACTED | STGM_READ | STGM_SHARE_DENY_WRITE, 0, 0, &storage));

		CComPtr<IPersistStream> persistStream;
		CHECK_HR(m_graphBuilder.QueryInterface(&persistStream));

		CComPtr<IStream> stream;
		CHECK_HR(storage->OpenStream(L"ActiveMovieGraph", 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &stream));
		
		CHECK_HR(persistStream->Load(stream));
	}
	catch (const CAtlException& except)
	{
		except.m_hr;
	}
}

CString DShowUtil::GetEventCodeString(long eventCode)
{
    typedef std::map<int, CString> EventCodeToString;
    static EventCodeToString eventCodesMap;

    if (eventCodesMap.empty())
    {
#define ADD_TO_MAP(x) eventCodesMap[x] = #x;

        ADD_TO_MAP(EC_COMPLETE);
        ADD_TO_MAP(EC_USERABORT);
        ADD_TO_MAP(EC_ERRORABORT);
        ADD_TO_MAP(EC_TIME);
        ADD_TO_MAP(EC_REPAINT);
        ADD_TO_MAP(EC_STREAM_ERROR_STOPPED);
        ADD_TO_MAP(EC_STREAM_ERROR_STILLPLAYING);
        ADD_TO_MAP(EC_ERROR_STILLPLAYING);
        ADD_TO_MAP(EC_PALETTE_CHANGED);
        ADD_TO_MAP(EC_VIDEO_SIZE_CHANGED);
        ADD_TO_MAP(EC_QUALITY_CHANGE);
        ADD_TO_MAP(EC_SHUTTING_DOWN);
        ADD_TO_MAP(EC_CLOCK_CHANGED);
        ADD_TO_MAP(EC_PAUSED);
        ADD_TO_MAP(EC_OPENING_FILE);
        ADD_TO_MAP(EC_BUFFERING_DATA);
        ADD_TO_MAP(EC_FULLSCREEN_LOST);
        ADD_TO_MAP(EC_ACTIVATE);
        ADD_TO_MAP(EC_NEED_RESTART);
        ADD_TO_MAP(EC_WINDOW_DESTROYED);
        ADD_TO_MAP(EC_DISPLAY_CHANGED);
        ADD_TO_MAP(EC_STARVATION);
        ADD_TO_MAP(EC_OLE_EVENT);
        ADD_TO_MAP(EC_NOTIFY_WINDOW);
        ADD_TO_MAP(EC_STREAM_CONTROL_STOPPED);
        ADD_TO_MAP(EC_STREAM_CONTROL_STARTED);
        ADD_TO_MAP(EC_END_OF_SEGMENT);
        ADD_TO_MAP(EC_SEGMENT_STARTED);
        ADD_TO_MAP(EC_LENGTH_CHANGED);
        ADD_TO_MAP(EC_DEVICE_LOST);
        ADD_TO_MAP(EC_SAMPLE_NEEDED);
        ADD_TO_MAP(EC_PROCESSING_LATENCY);
        ADD_TO_MAP(EC_SAMPLE_LATENCY);
        ADD_TO_MAP(EC_SCRUB_TIME);
        ADD_TO_MAP(EC_STEP_COMPLETE);
        ADD_TO_MAP(EC_TIMECODE_AVAILABLE);
        ADD_TO_MAP(EC_EXTDEVICE_MODE_CHANGE);
        ADD_TO_MAP(EC_STATE_CHANGE);
        ADD_TO_MAP(EC_GRAPH_CHANGED);
        ADD_TO_MAP(EC_CLOCK_UNSET);
        ADD_TO_MAP(EC_VMR_RENDERDEVICE_SET);
        ADD_TO_MAP(EC_VMR_SURFACE_FLIPPED);
        ADD_TO_MAP(EC_VMR_RECONNECTION_FAILED);
        ADD_TO_MAP(EC_PREPROCESS_COMPLETE);
        ADD_TO_MAP(EC_CODECAPI_EVENT);
        
#undef ADD_TO_MAP
    }

    EventCodeToString::const_iterator it = eventCodesMap.find(eventCode);
    CString eventCodeString;

    if (it != eventCodesMap.end())
    {
        eventCodeString.Format(L"%s (0x%x)", it->second, it->first);
    }
    else
    {
        eventCodeString.Format(L"0x%x", eventCode);
    }

    return eventCodeString;
}
