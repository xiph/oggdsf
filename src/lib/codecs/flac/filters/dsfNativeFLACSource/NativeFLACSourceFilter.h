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
#pragma once

#include <FLAC++/decoder.h>
#include <FLAC/seekable_stream_decoder.h>
#include <FLAC/stream_decoder.h>

//Forward Declarations
class NativeFLACSourcePin;

class NativeFLACSourceFilter: public CBaseFilter, public IFileSourceFilter, public IAMFilterMiscFlags,
                              public IMediaSeeking, public CAMThread, protected FLAC::Decoder::Stream
{
public:
    //Friends
    friend class NativeFLACSourcePin;

    //Constants
    enum eThreadCommands 
    {
        THREAD_EXIT = 0,
        THREAD_PAUSE = 1,
        THREAD_RUN = 2
    };

    static const unsigned long BUFF_SIZE = 8192;
    
    static const wchar_t* NAME;
    static const AMOVIESETUP_MEDIATYPE m_mediaTypes;
    static const AMOVIESETUP_PIN m_pinReg;
    static const AMOVIESETUP_FILTER m_filterReg;

#ifdef WINCE    
    // returns setup data for filter registration
    LPAMOVIESETUP_FILTER GetSetupData();
#endif

    //COM Stuff
    DECLARE_IUNKNOWN
    HRESULT __stdcall NonDelegatingQueryInterface(REFIID riid, void **ppv);
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

    NativeFLACSourceFilter();
    virtual ~NativeFLACSourceFilter();

    //IBaseFilter Pure virtuals
    virtual int GetPinCount();
    virtual CBasePin* GetPin(int inPinNo);

    //IAMFilterMiscFlags Interface
    ULONG __stdcall GetMiscFlags();

    //IFileSource Interface
    virtual HRESULT __stdcall GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);
    virtual HRESULT __stdcall Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

    //Streaming Methods
    HRESULT __stdcall Run(REFERENCE_TIME tStart);
    HRESULT __stdcall Pause();
    HRESULT __stdcall Stop();

    //CAMThread
    virtual DWORD ThreadProc();

    //FLAC virtuals
    virtual ::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte outBuffer[], size_t* outNumBytes);
    virtual ::FLAC__StreamDecoderSeekStatus seek_callback(FLAC__uint64 inSeekPos);
    virtual ::FLAC__StreamDecoderTellStatus tell_callback(FLAC__uint64* outTellPos);
    virtual ::FLAC__StreamDecoderLengthStatus length_callback(FLAC__uint64* outLength);
    virtual ::FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* outFrame,const FLAC__int32 *const outBuffer[]);
    virtual void metadata_callback(const FLAC__StreamMetadata* inMetaData);
    virtual void error_callback(FLAC__StreamDecoderErrorStatus inStatus);
    virtual bool eof_callback();

    //IMediaSeeking Interface
    HRESULT __stdcall GetCapabilities(DWORD *pCapabilities);
    HRESULT __stdcall CheckCapabilities(DWORD *pCapabilities);
    HRESULT __stdcall IsFormatSupported(const GUID *pFormat);
    HRESULT __stdcall QueryPreferredFormat(GUID *pFormat);
    HRESULT __stdcall SetTimeFormat(const GUID *pFormat);
    HRESULT __stdcall GetTimeFormat(GUID *pFormat);
    HRESULT __stdcall GetDuration(LONGLONG *pDuration);
    HRESULT __stdcall GetStopPosition(LONGLONG *pStop);
    HRESULT __stdcall GetCurrentPosition(LONGLONG *pCurrent);
    HRESULT __stdcall ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat);
    HRESULT __stdcall SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags);
    HRESULT __stdcall GetPositions(LONGLONG *pCurrent, LONGLONG *pStop);
    HRESULT __stdcall GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest);
    HRESULT __stdcall SetRate(double dRate);
    HRESULT __stdcall GetRate(double *dRate);
    HRESULT __stdcall GetPreroll(LONGLONG *pllPreroll);
    HRESULT __stdcall IsUsingTimeFormat(const GUID *pFormat);

#if defined (_DEBUG) && defined (WINCE)
    ULONG __stdcall NonDelegatingRelease()
    {
        if (m_cRef == 1) 
        {
            ASSERT(m_pGraph == NULL);
        }
        return CUnknown::NonDelegatingRelease();
    }
#endif

protected:
    //Helper Methods
    HRESULT DataProcessLoop();

    __int64 SeekFile(__int64 distance, unsigned long moveMethod);
    __int64 SeekFile(LARGE_INTEGER distance, unsigned long moveMethod);

    std::wstring VorbisCommentToString(const FLAC__StreamMetadata_VorbisComment_Entry& comment);

    //Pin Class
    NativeFLACSourcePin* m_flacSourcePin;

    //Source File Members
    std::wstring m_fileName;
    HANDLE m_inputFileHandle;
    __int64 m_fileSize;
    unsigned long m_extraBeginDataLength;

    //State Variables
    bool m_begun;
    bool m_justSeeked;
    __int64 m_seekRequest;
    bool m_wasEof;
    unsigned long m_upTo;

    //Stream info data.
    unsigned short m_numChannels;
    unsigned long m_frameSize;
    unsigned long m_sampleRate;
    unsigned short m_bitsPerSample;
    unsigned short m_significantBitsPerSample;
    __int64 m_totalNumSamples;

    typedef std::multimap<std::wstring, std::wstring> VorbisCommentMap;
    VorbisCommentMap m_vorbisCommentsMap;
};
