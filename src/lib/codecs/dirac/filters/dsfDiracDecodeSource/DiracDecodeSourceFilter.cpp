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
#include ".\DiracDecodeSourceFilter.h"

CFactoryTemplate g_Templates[] = 
{
    { 
		L"DiracDecodeSourceFilter",						// Name
	    &CLSID_DiracDecodeSourceFilter,            // CLSID
	    DiracDecodeSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 


CUnknown* WINAPI DiracDecodeSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	DiracDecodeSourceFilter *pNewObject = new DiracDecodeSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

DiracDecodeSourceFilter::DiracDecodeSourceFilter(void)
	:	CBaseFilter(NAME("DiracDecodeSourceFilter"), NULL, m_pLock, CLSID_DiracDecodeSourceFilter)
	,	mDecoder(NULL)
{
	mDiracSourcePin = new DiracDecodeSourcePin(this, m_pLock);
}

DiracDecodeSourceFilter::~DiracDecodeSourceFilter(void)
{
	delete mDiracSourcePin;
	mDiracSourcePin = NULL;
}

//BaseFilter Interface
int DiracDecodeSourceFilter::GetPinCount() {
	return 1;
}
CBasePin* DiracDecodeSourceFilter::GetPin(int inPinNo) {

	if (inPinNo == 0) {
		return mDiracSourcePin;
	} else {
		return NULL;
	}
}

//IAMFilterMiscFlags Interface
ULONG DiracDecodeSourceFilter::GetMiscFlags(void) {
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

	//IFileSource Interface
STDMETHODIMP DiracDecodeSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) {
	//Return the filename and mediatype of the raw data

	 
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;
	
	return S_OK;
}

//ANX::: Seek table will need modifying to handle this.
STDMETHODIMP DiracDecodeSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	//Strip the extension...
	//size_t locDotPos = mFileName.find_last_of('.');
	//if (locDotPos != ios_base::npos) {
	//	mHDRFileName = mFileName.substr(0, locDotPos);
	//	mHDRFileName += ".hdr";
	//} else {
	//	return S_FALSE;
	//}

	mInputFile.open(StringHelper::toNarrowStr(mFileName), ios_base::in | ios_base::binary);

	if (!mInputFile.is_open()) {
		return S_FALSE;
	}


	
	return S_OK;
}

STDMETHODIMP DiracDecodeSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}


//IMEdiaStreaming
STDMETHODIMP DiracDecodeSourceFilter::Run(REFERENCE_TIME tStart) {
	const REFERENCE_TIME A_LONG_TIME = UNITS * 1000;
	CAutoLock locLock(m_pLock);
	//debugLog<<"Run  :  time = "<<tStart<<endl;
	//DeliverNewSegment(tStart, tStart + A_LONG_TIME, 1.0);
	return CBaseFilter::Run(tStart);
	

}
STDMETHODIMP DiracDecodeSourceFilter::Pause(void) {
	CAutoLock locLock(m_pLock);
	//debugLog << "** Pause called **"<<endl;
	if (m_State == State_Stopped) {
		//debugLog << "Was in stopped state... starting thread"<<endl;
		if (ThreadExists() == FALSE) {
			Create();
		}
		CallWorker(THREAD_RUN);
	}
	//debugLog<<"Was NOT is stopped state, not doing much at all..."<<endl;
	
	HRESULT locHR = CBaseFilter::Pause();
	
	return locHR;
	
}
STDMETHODIMP DiracDecodeSourceFilter::Stop(void) {
	CAutoLock locLock(m_pLock);
	//debugLog<<"** Stop Called ** "<<endl;
	CallWorker(THREAD_EXIT);
	Close();
	//DeliverBeginFlush();
	//DeliverEndFlush();
	return CBaseFilter::Stop();
}

HRESULT DiracDecodeSourceFilter::DataProcessLoop() {

    do 
    {
        /* parse the input data */
        state = dirac_parse(decoder);
        
        switch (state)
        {
        case STATE_BUFFER:
            /*
            * parser is out of data. Read data from input stream and pass it
            * on to the parser
            */
            bytes = fread (buffer, 1, sizeof(buffer), ifp);
            if (bytes)
                dirac_buffer (decoder, buffer, buffer + bytes);
            break;

        case STATE_SEQUENCE:
            {
            /*
            * Start of sequence detected. Allocate for the frame buffers and
            * pass this buffer to the parser
            */
            unsigned char *buf[3];

            if (verbose)
            {
                fprintf (stderr, "SEQUENCE : width=%d height=%d chroma=%s chroma_width=%d chroma_height=%d num_frames=%d frame_rate=%d, interlace=%s topfieldfirst=%s\n", 
                decoder->seq_params.width,
                decoder->seq_params.height,
                chroma2string(decoder->seq_params.chroma),
                decoder->seq_params.chroma_width,
                decoder->seq_params.chroma_height,
                decoder->seq_params.num_frames,
                decoder->seq_params.frame_rate,
                decoder->seq_params.interlace ? "yes" : "no",
                decoder->seq_params.interlace ? "yes" : "no");
            }

            FreeFrameBuffer(decoder);

            buf[0] = buf[1] = buf[2] = 0;

            buf[0] = (unsigned char *)malloc (decoder->seq_params.width * decoder->seq_params.height);
            if (decoder->seq_params.chroma != Yonly)
            {
                buf[1] = (unsigned char *)malloc (decoder->seq_params.chroma_width * decoder->seq_params.chroma_height);
                buf[2] = (unsigned char *)malloc (decoder->seq_params.chroma_width * decoder->seq_params.chroma_height);
            }
            dirac_set_buf (decoder, buf, NULL);

            /* write the header file */
            WritePicHeader(decoder, fphdr);
            }
            break;

        case STATE_SEQUENCE_END:
            /*
            * End of Sequence detected. Free the frame buffers
            */
            if (verbose)
                fprintf (stderr, "SEQUENCE_END\n");
            
            FreeFrameBuffer(decoder);
            break;
        
        case STATE_PICTURE_START:
            /*
            * Start of frame detected. If decoder is too slow and frame can be
            * skipped, inform the parser to skip decoding the frame
            */
            num_frames++;
            if (verbose)
            {
                fprintf (stderr, "PICTURE_START : frame_type=%s frame_num=%d\n",
                    ftype2string(decoder->frame_params.ftype),
                    decoder->frame_params.fnum);
            }
            /* Just for testing skip every L2_frame */
            if (skip && decoder->frame_params.ftype == L2_frame)
            {
                if (verbose)
                    fprintf (stderr, "              : Skipping frame\n");

                dirac_skip (decoder, 1);
            }
            else
                dirac_skip (decoder, 0);
            break;

        case STATE_PICTURE_AVAIL:
            if (verbose)
            {
                fprintf (stderr, "PICTURE_AVAIL : frame_type=%s frame_num=%d\n",
                    ftype2string(decoder->frame_params.ftype),
                    decoder->frame_params.fnum);
            }
            /* picture available for display */
            WritePicData(decoder, fpdata);
            break;

        case STATE_INVALID:
            /* Invalid state. Stop all processing */
            fprintf (stderr, "Error processing file %s\n", iname);
            break;

        default:
            continue;
        }
    } while (bytes > 0 && state != STATE_INVALID);





	return S_OK;
}

//CAMThread Stuff
DWORD DiracDecodeSourceFilter::ThreadProc(void) {
	//debugLog << "Thread Proc Called..."<<endl;
	while(true) {
		DWORD locThreadCommand = GetRequest();
		//debugLog << "Command = "<<locThreadCommand<<endl;
		switch(locThreadCommand) {
			case THREAD_EXIT:
				//debugLog << "EXIT ** "<<endl;
				Reply(S_OK);
				return S_OK;

			//case THREAD_PAUSE:
			//	// we are paused already
			//	Reply(S_OK);
			//	break;

			case THREAD_RUN:
				//debugLog << "RUN ** "<<endl;
				Reply(S_OK);
				DataProcessLoop();
				break;
		}
	
	
	}
	return S_OK;
}