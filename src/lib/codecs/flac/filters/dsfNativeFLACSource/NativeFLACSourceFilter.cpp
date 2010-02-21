//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//Copyright (C) 2008, 2009 Cristian Adam
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
#include "NativeFLACSourceFilter.h"
#include "NativeFLACSourcePin.h"
#include "dsfNativeFLACSource.h"
#include <sstream>
#include "common/util.h"

CFactoryTemplate g_Templates[] = 
{
    { 
        NativeFLACSourceFilter::NAME,           // Name
        &CLSID_NativeFLACSourceFilter,          // CLSID
        NativeFLACSourceFilter::CreateInstance, // Method to create an instance of MyComponent
        NULL,                                   // Initialization function
        &NativeFLACSourceFilter::m_filterReg    // Set-up information (for filters)
    }
};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

const wchar_t* NativeFLACSourceFilter::NAME = L"Xiph.Org Native FLAC Decoder";

const AMOVIESETUP_MEDIATYPE NativeFLACSourceFilter::m_mediaTypes = 
{
    &MEDIATYPE_Audio,
    &MEDIASUBTYPE_PCM
};

const AMOVIESETUP_PIN NativeFLACSourceFilter::m_pinReg = 
{
    L"PCM Output",                      //Name (obsoleted)
    FALSE,                              //Renders from this pin ?? Not sure about this.
    TRUE,                               //Is an output pin
    FALSE,                              //Cannot have zero instances of this pin
    FALSE,                              //Cannot have more than one instance of this pin
    &CLSID_NULL,                        //Connects to filter (obsoleted)
    NULL,                               //Connects to pin (obsoleted)
    1,                                  //Only support one media type
    &m_mediaTypes                       //Pointer to media type (Audio/PCM)
};

const AMOVIESETUP_FILTER NativeFLACSourceFilter::m_filterReg = 
{
    &CLSID_NativeFLACSourceFilter,          // Filter CLSID.
    NAME,                                   // Filter name.
    MERIT_NORMAL,                           // Merit.
    1,                                      // Number of pin types.
    &m_pinReg                               // Pointer to pin information.
};


#ifdef WINCE
LPAMOVIESETUP_FILTER NativeFLACSourceFilter::GetSetupData()
{
    return (LPAMOVIESETUP_FILTER)(&m_filterReg);	
}
#endif

CUnknown* WINAPI NativeFLACSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
    util::ConfigureLogSettings();

    NativeFLACSourceFilter *pNewObject = new (std::nothrow) NativeFLACSourceFilter();
    if (pNewObject == NULL) 
    {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

NativeFLACSourceFilter::NativeFLACSourceFilter() :  
CBaseFilter(NAME, NULL, m_pLock, CLSID_NativeFLACSourceFilter),
m_numChannels(0),
m_sampleRate(0),
m_bitsPerSample(0),
m_significantBitsPerSample(0),
m_begun(false),
m_upTo(0),
m_justSeeked(true),
m_seekRequest(0),
m_totalNumSamples(0),
m_wasEof(false),
m_inputFileHandle(INVALID_HANDLE_VALUE)
{
    m_pLock = new CCritSec;
    m_flacSourcePin = new NativeFLACSourcePin(this, m_pLock);

    LOG(logINFO) << L"NativeFLACSourceFilter object created!" << std::endl;
}

NativeFLACSourceFilter::~NativeFLACSourceFilter(void)
{
    ::CloseHandle(m_inputFileHandle);

    delete m_flacSourcePin;
    m_flacSourcePin = NULL;
}

//BaseFilter Interface
int NativeFLACSourceFilter::GetPinCount() 
{
    return 1;
}

CBasePin* NativeFLACSourceFilter::GetPin(int inPinNo) 
{
    if (inPinNo == 0) 
    {
        return m_flacSourcePin;
    } 
    else 
    {
        return NULL;
    }
}

//IAMFilterMiscFlags Interface
ULONG NativeFLACSourceFilter::GetMiscFlags() 
{
    return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

//IFileSource Interface
HRESULT __stdcall NativeFLACSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) 
{
    CheckPointer(outFileName, E_POINTER);
    *outFileName = NULL;

    if (!m_fileName.empty()) 
    {
        unsigned int size  = sizeof(WCHAR) * (m_fileName.size() + 1);

        *outFileName = (LPOLESTR) CoTaskMemAlloc(size);
        if (*outFileName != NULL) 
        {
            CopyMemory(*outFileName, m_fileName.c_str(), size);
        }
    }
    
    return S_OK;
}

__int64 NativeFLACSourceFilter::SeekFile(__int64 distance, unsigned long moveMethod)
{
    LARGE_INTEGER current;

    current.QuadPart = distance;
    current.LowPart = ::SetFilePointer(m_inputFileHandle, current.LowPart, &current.HighPart, moveMethod);

    if (current.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    {
        current.QuadPart = -1;
    }

    return current.QuadPart;
}

__int64 NativeFLACSourceFilter::SeekFile(LARGE_INTEGER distance, unsigned long moveMethod)
{
    return SeekFile(distance.QuadPart, moveMethod);
}

HRESULT __stdcall NativeFLACSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) 
{
    //Initialize the file here and setup the stream
    CAutoLock locLock(m_pLock);
    m_fileName = inFileName;

    LOG(logINFO) << L"NativeFLACSourceFilter::Load(" << m_fileName << L")";

    if (m_inputFileHandle != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_inputFileHandle);
    }

    m_inputFileHandle = ::CreateFile(m_fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, 
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (m_inputFileHandle == INVALID_HANDLE_VALUE)
    {
        LOG(logERROR) << L"CreateFile(" << m_fileName << L") failed! GetLastError: 0x"
            << std::hex << ::GetLastError();

        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    //CT> Added header check (for FLAC files with ID3 v1/2 tags in them)
    //    We'll look in the first 128kb of the file
    unsigned long locStart = 0;
    bool headerFound = false;
    for (int j = 0; !headerFound && j < 128; j++)   
    {
        unsigned char locTempBuf[1024] = {0};

        unsigned long readBytes = 0;
        ::ReadFile(m_inputFileHandle, &locTempBuf, sizeof(locTempBuf), &readBytes, 0);

        unsigned char* locPtr = locTempBuf;
        for (int i = 0; i < 1023; i++) 
        {
            if (locPtr[i]=='f' && locPtr[i+1]=='L' && locPtr[i+2]=='a' && locPtr[i+3]=='C')         
            {
                headerFound = true;
                locStart = i + (j * 1024);
                break;
            }
        }
    }

    if (!headerFound) 
    {
        LOG(logERROR) << "No FLAC Header Found!";
        return E_FAIL;
    }

    LARGE_INTEGER fileSize;
    fileSize.LowPart = ::GetFileSize(m_inputFileHandle, (unsigned long*)&fileSize.HighPart);
    m_fileSize = fileSize.QuadPart;
    
    m_extraBeginDataLength = locStart;

    set_md5_checking(false);
    set_metadata_ignore_all();
    set_metadata_respond(FLAC__METADATA_TYPE_STREAMINFO);
    set_metadata_respond(FLAC__METADATA_TYPE_VORBIS_COMMENT);

    //TODO::: Need to handle the case where the number of samples is zero by making it non-seekable.

    SeekFile(locStart, FILE_BEGIN);

    init();
    bool locResult = process_until_end_of_metadata();

    if (!locResult)
    {
        LOG(logERROR) << L"process_until_end_of_metadata returned false!";
    }

    return locResult ? S_OK : E_FAIL;
}

HRESULT __stdcall NativeFLACSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    LOG(logDEBUG) << L"NonDelegatingQueryInterface: " << riid;

    if (riid == IID_IFileSourceFilter) 
    {
        return GetInterface((IFileSourceFilter*)this, ppv);
    }

    return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}


//IMediaStreaming
HRESULT __stdcall NativeFLACSourceFilter::Run(REFERENCE_TIME tStart) 
{
    CAutoLock locLock(m_pLock);
    LOG(logINFO) << "Run: " << ReferenceTime(tStart);

    return CBaseFilter::Run(tStart);
}

HRESULT __stdcall NativeFLACSourceFilter::Pause() 
{
    CAutoLock locLock(m_pLock);
    LOG(logINFO) << "Pause";

    if (m_State == State_Stopped) 
    {
        if (ThreadExists() == FALSE) 
        {
            Create();
        }
        CallWorker(THREAD_RUN);
    }

    HRESULT locHR = CBaseFilter::Pause();
    return locHR;
    
}
HRESULT __stdcall NativeFLACSourceFilter::Stop() 
{
    CAutoLock locLock(m_pLock);
    LOG(logINFO) << "Stop";

    CallWorker(THREAD_EXIT);
    Close();

    m_seekRequest = 0;
    m_upTo = 0;

    m_flacSourcePin->DeliverBeginFlush();
    m_flacSourcePin->DeliverEndFlush();

    SeekFile(0, FILE_BEGIN);

    return CBaseFilter::Stop();
}

HRESULT NativeFLACSourceFilter::DataProcessLoop() 
{
    while (true) 
    {
        DWORD locCommand = 0;
        if (CheckRequest(&locCommand) == TRUE) 
        {
            return S_OK;
        }

        if (m_justSeeked) 
        {
            m_upTo = 0;
            m_justSeeked = false;

            LOG(logINFO) << "DataProcessLoop: seek_absolute(" << m_seekRequest << ")";
            if (!seek_absolute(m_seekRequest))
            {
                //ERROR???

                LOG(logERROR) << "DataProcessLoop: seek_absolute failed!";
                flush();
                break;
            }
        }
        
        if (!process_single())
        {
            //ERROR???

            LOG(logERROR) << "DataProcessLoop: process_single failed!";
            flush();
        }

        if (m_wasEof) 
        {
            break;
        }
    }

    SeekFile(0, FILE_BEGIN);
    m_wasEof = false;
    m_flacSourcePin->DeliverEndOfStream();

    return S_OK;
}

//CAMThread Stuff
DWORD NativeFLACSourceFilter::ThreadProc(void) 
{
    while(true) 
    {
        DWORD locThreadCommand = GetRequest();
        switch(locThreadCommand) 
        {
            case THREAD_EXIT:
                Reply(S_OK);
                return S_OK;

            case THREAD_RUN:
                Reply(S_OK);
                DataProcessLoop();
                break;
            //OTHER CASES?
        }
    }
    return S_OK;
}


::FLAC__StreamDecoderReadStatus NativeFLACSourceFilter::read_callback(FLAC__byte outBuffer[], size_t* outNumBytes) 
{
    unsigned long bytesRead = 0;
    BOOL result = ::ReadFile(m_inputFileHandle, outBuffer, BUFF_SIZE, &bytesRead, 0);
    *outNumBytes = bytesRead;

    if (!result && ::GetLastError() == ERROR_HANDLE_EOF || eof_callback())
    {
        m_wasEof = true;
    }
    
    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

::FLAC__StreamDecoderSeekStatus NativeFLACSourceFilter::seek_callback(FLAC__uint64 inSeekPos) 
{
    LOG(logDEBUG) << "seek_callback: pos " << inSeekPos;

    if (SeekFile(inSeekPos, FILE_BEGIN) < 0)
    {
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    }

    return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

::FLAC__StreamDecoderTellStatus NativeFLACSourceFilter::tell_callback(FLAC__uint64* outTellPos) 
{
    __int64 currentPosition = SeekFile(0, FILE_CURRENT);
    if (currentPosition < 0)
    {
        return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
    }

    *outTellPos = currentPosition;

    return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

::FLAC__StreamDecoderLengthStatus NativeFLACSourceFilter::length_callback(FLAC__uint64* outLength) 
{
    *outLength = m_fileSize - m_extraBeginDataLength;
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

::FLAC__StreamDecoderWriteStatus NativeFLACSourceFilter::write_callback(const FLAC__Frame* inFrame,const FLAC__int32 *const inBuffer[]) 
{
    if (!m_begun) 
    {
        m_begun = true; 
    
        m_numChannels = static_cast<unsigned short>(inFrame->header.channels);
        m_frameSize = m_numChannels * (m_bitsPerSample >> 3);

        m_sampleRate = inFrame->header.sample_rate;
    }

    unsigned long numFrames = inFrame->header.blocksize;
    unsigned long bufferSize = numFrames * m_frameSize;
    unsigned long totalFrameCount = numFrames * m_numChannels;

    // TODO::: It's not clear whether in the 32 bit flac sample, the significant bits are always rightmost (
    //      ie justified into the least significant bits. They seem to be for nbits = 16. But it's unclear
    //      whether eg a 20 bit sample is 0000 0000 1111 1111 1111 1111 1111 0000 so it's still a valid
    //      24 bit sample, or whether it would require a 4 bit left shift to be a true 24 bit sample.

    //      For now, working on the basis that it is truly right shifted.


    //It could actually be a single buffer for the class.????
    unsigned char* buffer = new unsigned char[bufferSize];          //Gives to the deliverdata method
    unsigned long leftShift = m_bitsPerSample - m_significantBitsPerSample;

    if (m_bitsPerSample == 8) 
    {
        unsigned char* locByteBuffer = (unsigned char*)buffer;

        if (leftShift == 0) 
        {
            for(unsigned long i = 0; i < numFrames; i++) 
            {
                for (unsigned long j = 0; j < m_numChannels; j++) 
                {
                    *(locByteBuffer++) = (unsigned char)(inFrame + 128);
                }
            }
        } 
        else 
        {
            for(unsigned long i = 0; i < numFrames; i++) 
            {
                for (unsigned long j = 0; j < m_numChannels; j++) 
                {
                    *(locByteBuffer++) = (unsigned char)(inFrame + 128) << leftShift;
                }
            }
        }
    } 
    else if (m_bitsPerSample == 16) 
    {
        signed short* locShortBuffer = (signed short*)buffer;      //Don't delete this.
        if (leftShift == 0) 
        {
            for(unsigned long i = 0; i < numFrames; i++) 
            {
                for (unsigned long j = 0; j < m_numChannels; j++) 
                {
                    *(locShortBuffer++) = (signed short)inBuffer[j][i];
                    //tempLong = inBuffer[j][i];

                    ////FIX::: Why on earth are you dividing by 2 ? It does not make sense !
                    ////tempInt = (signed short)(tempLong/2);
                    //tempInt = (signed short)(tempLong);
                
                    //*locShortBuffer = tempInt;
                    //locShortBuffer++;
                }
            }
        } 
        else 
        {
            for(unsigned long i = 0; i < numFrames; i++) 
            {
                for (unsigned long j = 0; j < m_numChannels; j++) 
                {
                    *(locShortBuffer++) = (signed short)inBuffer[j][i] << leftShift;
                }
            }
        }
    } 
    else if (m_bitsPerSample == 32) 
    {
        signed long* locLongBuffer = (signed long*)buffer;
        if (leftShift == 8) 
        {
            //Special case for 24 bit, let the shift be hard coded.
            for(unsigned long i = 0; i < numFrames; i++) 
            {
                for (unsigned long j = 0; j < m_numChannels; j++) 
                {
                    *(locLongBuffer++) = inBuffer[j][i] << 8;
                }
            }
        } 
        else if (leftShift == 0) 
        {
            //Real 32 bit data
            for(unsigned long i = 0; i < numFrames; i++) 
            {
                for (unsigned long j = 0; j < m_numChannels; j++) 
                {
                    *(locLongBuffer++) = inBuffer[j][i];
                }
            }
        } 
        else 
        {    
            for(unsigned long i = 0; i < numFrames; i++) 
            {
                for (unsigned long j = 0; j < m_numChannels; j++) 
                {
                    *(locLongBuffer++) = inBuffer[j][i] << leftShift;
                }
            }
        }
    }
    
    __int64 start = m_upTo * UNITS / m_sampleRate;
    __int64 stop = (m_upTo + numFrames) * UNITS / m_sampleRate;

    m_flacSourcePin->DeliverData(buffer, bufferSize, start, stop);
    m_upTo += numFrames;

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

std::wstring NativeFLACSourceFilter::VorbisCommentToString( const FLAC__StreamMetadata_VorbisComment_Entry& comment )
{
    std::wstring stringComment;

    // Transform it from UTF-8 in wide chars
    int chars = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(comment.entry), comment.length, 0, 0);
    stringComment.resize(chars);

    ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(comment.entry), comment.length, &*stringComment.begin(), chars);

    return stringComment;
}

void NativeFLACSourceFilter::metadata_callback(const FLAC__StreamMetadata* metadata) 
{
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
        m_numChannels = metadata->data.stream_info.channels;
        m_sampleRate = metadata->data.stream_info.sample_rate;
        m_significantBitsPerSample = metadata->data.stream_info.bits_per_sample;

        m_bitsPerSample = (m_significantBitsPerSample + 7) & 0xfff8;

        if (m_bitsPerSample == 24) 
        {
            m_bitsPerSample = 32;
        }

        m_totalNumSamples = metadata->data.stream_info.total_samples;

        LOG(logINFO) << L"NumChannels: " << m_numChannels;
        LOG(logINFO) << L"SampleRate: " << m_sampleRate;
        LOG(logINFO) << L"SignificantBitsPerSample: " << m_significantBitsPerSample;
        LOG(logINFO) << L"BitsPerSample: " << m_bitsPerSample;
        LOG(logINFO) << L"TotalNumSamples: " << m_totalNumSamples;
    }
    else if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
    {
        LOG(logINFO) << L"Vorbis Comment: " << metadata->data.vorbis_comment.num_comments;
        LOG(logINFO) << L"Vendor String: " << VorbisCommentToString(metadata->data.vorbis_comment.vendor_string);

        for (int i = 0; i < metadata->data.vorbis_comment.num_comments; ++i)
        {
            std::wstring comment = VorbisCommentToString(metadata->data.vorbis_comment.comments[i]);
            LOG(logINFO) << comment;

            int pos = comment.find_first_of(L'=');
            
            std::wstring key = comment.substr(0, pos);
            std::wstring value = comment.substr(pos + 1);

            m_vorbisCommentsMap.insert(std::make_pair(key, value));
        }
    }
}

void NativeFLACSourceFilter::error_callback(FLAC__StreamDecoderErrorStatus inStatus) 
{
    LOG(logERROR) << "error_callback: status - " << inStatus;
}

bool NativeFLACSourceFilter::eof_callback() 
{
    __int64 currentPosition = SeekFile(0, FILE_CURRENT);

    bool isEof = false;
    if (currentPosition == m_fileSize)
    {
        isEof = true;
    }

    return isEof;
}

HRESULT __stdcall NativeFLACSourceFilter::GetCapabilities(DWORD* inCapabilities) 
{
    *inCapabilities = AM_SEEKING_CanSeekAbsolute
                    | AM_SEEKING_CanSeekForwards
                    | AM_SEEKING_CanSeekBackwards
                    | AM_SEEKING_CanGetStopPos
                    | AM_SEEKING_CanGetDuration
                    | AM_SEEKING_CanGetCurrentPos;

    LOG(logDEBUG) << "IMediaSeeking::GetCapabilities([out] " << std::hex << *inCapabilities << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::CheckCapabilities(DWORD *pCapabilities) 
{
    HRESULT result = S_OK;

    DWORD dwActual;
    GetCapabilities(&dwActual);
    if (*pCapabilities & (~dwActual))
    {
        result = S_FALSE;
    }

    LOG(logDEBUG) << "IMediaSeeking::CheckCapabilities([out]  " << *pCapabilities << ") -> 0x" << std::hex << result;

    return result;
}

HRESULT __stdcall NativeFLACSourceFilter::IsFormatSupported(const GUID *pFormat) 
{
    HRESULT result = S_FALSE;

    if (*pFormat == TIME_FORMAT_MEDIA_TIME) 
    {
        result = S_OK;
    } 

    LOG(logDEBUG) << "IMediaSeeking::IsFormatSupported([in] " << ToString(*pFormat) << ") -> 0x" << std::hex << result;

    return result;
}

HRESULT __stdcall NativeFLACSourceFilter::QueryPreferredFormat(GUID *pFormat) 
{
    *pFormat = TIME_FORMAT_MEDIA_TIME;

    LOG(logDEBUG) << "IMediaSeeking::QueryPreferredFormat([out] " << ToString(*pFormat) << ") -> 0x" << std::hex << S_OK; 

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::SetTimeFormat(const GUID *pFormat) 
{
    LOG(logDEBUG) << "IMediaSeeking::SetTimeFormat([in] " << ToString(pFormat) << ") -> 0x" << std::hex << E_NOTIMPL; 

    return E_NOTIMPL;
}

HRESULT __stdcall NativeFLACSourceFilter::GetTimeFormat( GUID *pFormat) 
{
    *pFormat = TIME_FORMAT_MEDIA_TIME;

    LOG(logDEBUG) << "IMediaSeeking::GetTimeFormat([out] " << ToString(*pFormat) << ") -> 0x" << std::hex << S_OK; 

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::GetDuration(LONGLONG *pDuration) 
{
    *pDuration = m_totalNumSamples * UNITS / m_sampleRate;

    LOG(logDEBUG) << "IMediaSeeking::GetDuration([out] " << ToString(*pDuration) << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::GetStopPosition(LONGLONG *pStop) 
{
    *pStop = m_totalNumSamples * UNITS / m_sampleRate;

    LOG(logDEBUG) << "IMediaSeeking::GetStopPosition([out] " << ToString(*pStop) << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::GetCurrentPosition(LONGLONG *pCurrent)
{
    *pCurrent = m_upTo * UNITS / m_sampleRate;

    LOG(logDEBUG) << "IMediaSeeking::GetCurrentPosition([out] " << ToString(*pCurrent) << ") -> 0x" << std::hex << S_OK;
    
    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat)
{
    LOG(logDEBUG) << "IMediaSeeking::ConvertTimeFormat([out] " << ToString(pTarget) 
        << ", [in] " << ToString(pTargetFormat) << ", [in] " << ToString(Source)
        << ", [in] " << ToString(pSourceFormat) << ") -> 0x" << std::hex << E_NOTIMPL;

    return E_NOTIMPL;
}

HRESULT __stdcall NativeFLACSourceFilter::SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags)
{
    unsigned __int64 locSampleToSeek = (*pCurrent) * m_sampleRate/ UNITS;

    LOG(logDEBUG) << "IMediaSeeking::SetPositions([in, out] " << ToString(pCurrent) << ", [in] " << dwCurrentFlags
        << ", [in, out] " << ToString(pStop) << ", [in] " << dwStopFlags << ") -> 0x" << std::hex << S_OK;

    m_flacSourcePin->DeliverBeginFlush();
    m_flacSourcePin->DeliverEndFlush();

    m_seekRequest = locSampleToSeek;
    m_justSeeked = true;


    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop)
{
    *pCurrent = m_upTo * UNITS / m_sampleRate;
    *pStop = m_totalNumSamples * UNITS / m_sampleRate;

    LOG(logDEBUG) << "IMediaSeeking::GetPositions([out] " << ToString(*pCurrent) << ", [out] " << ToString(*pStop)
        << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest)
{
    *pEarliest = 0;
    *pLatest = m_totalNumSamples * UNITS / m_sampleRate;

    LOG(logDEBUG) << "IMediaSeeking::GetAvailable([out] " << ToString(*pEarliest) << ", [out] " << ToString(*pLatest)
        << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::SetRate(double dRate)
{
    HRESULT result = VFW_E_UNSUPPORTED_AUDIO;

    if (dRate == 1.00f)
    {
        result = S_OK;
    }
    else if (dRate <= 0.00f)
    {
        result = E_INVALIDARG;
    }

    LOG(logDEBUG) << "IMediaSeeking::SetRate([in] " << std::setprecision(3) << std::showpoint
        << dRate << ") -> 0x" << std::hex << result;

    return result;
}

HRESULT __stdcall NativeFLACSourceFilter::GetRate(double *dRate)
{
    *dRate = 1.0;

    LOG(logDEBUG) << "IMediaSeeking::GetRate([out] " << std::setprecision(3) << std::showpoint
        << *dRate << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::GetPreroll(LONGLONG *pllPreroll)
{
    *pllPreroll = 0;

    LOG(logDEBUG) << "IMediaSeeking::GetPreroll([out] " << ToString(*pllPreroll) << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

HRESULT __stdcall NativeFLACSourceFilter::IsUsingTimeFormat(const GUID *pFormat)
{
    HRESULT result = S_FALSE;

    if (*pFormat == TIME_FORMAT_MEDIA_TIME) 
    {
        result = S_OK;
    }

    LOG(logDEBUG) << "IMediaSeeking::IsUsingTimeFormat([in] " << ToString(*pFormat) << ") -> 0x" << std::hex << result;

    return result;
}


