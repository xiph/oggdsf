//===========================================================================
//Copyright (C) 2004-2006 Zentaro Kavanagh
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
#include "StdAfx.h"
#include "FLACEncoder.h"

FLACEncoder::FLACEncoder(void)
    :   mHandledHeaders(true)
    ,   mUptoTime(0)
{
   // mFLACSampleBuffer = new FLAC__int32[
}

FLACEncoder::~FLACEncoder(void)
{
}

bool FLACEncoder::setupCodec(FLACEncoderSettings inSettings)
{
    mUptoTime = 0;
    mSettings = inSettings;
	set_channels(inSettings.numChannels());
	set_sample_rate(inSettings.sampleRate());
	set_bits_per_sample(inSettings.bitsPerSample());
    set_blocksize(inSettings.blockSize());
    set_max_lpc_order(inSettings.LPCOrder());
    set_min_residual_partition_order(inSettings.riceMin());
    set_max_residual_partition_order(inSettings.riceMax());
    set_loose_mid_side_stereo(inSettings.isUsingAdaptiveMidSideCoding());
    set_do_mid_side_stereo(inSettings.isUsingMidSideCoding());
    set_do_exhaustive_model_search(inSettings.isUsingExhaustiveModel());
    
    return (init() == FLAC__STREAM_ENCODER_OK);
}
const vector<StampedOggPacket*>& FLACEncoder::encode16bit(const short* const inBuffer, unsigned long inNumSamples)
{
	FLAC__int32* locFLACBuff = NULL;


    unsigned long locNumSamplesPerChannel = inNumSamples / mSettings.numChannels();

    locFLACBuff = new FLAC__int32[inNumSamples];

    //Expand out to 32 bits
    for (unsigned long sample = 0; sample < inNumSamples; sample++) {
        locFLACBuff[sample] = inBuffer[sample];
    }


    clearStoredPackets();
    //TODO::: Can't really fail... but do what with this?
    bool locRetVal = process_interleaved(locFLACBuff, locNumSamplesPerChannel);
	delete[] locFLACBuff;

    return mPackets;
}

void FLACEncoder::clearStoredPackets()
{
    for (size_t i = 0; i < mPackets.size(); i++) {
        delete mPackets[i];
    }
    mPackets.clear();
}

const vector<StampedOggPacket*>& FLACEncoder::flush()
{
    clearStoredPackets();
    finish();
    return mPackets;
}



::FLAC__StreamEncoderWriteStatus FLACEncoder::write_callback(        const FLAC__byte inBuffer[]
                                                                ,   unsigned inNumBytes
                                                                ,   unsigned inNumSamples
                                                                ,   unsigned inCurrentFrame)
{
    unsigned char* locBuf = new unsigned char[inNumBytes];
	memcpy((void*)locBuf, (const void*) inBuffer, inNumBytes);

    LOOG_INT64 locStartTime = mUptoTime;
    LOOG_INT64 locEndTime = locStartTime + inNumSamples;
    mUptoTime = locEndTime;
    
    StampedOggPacket* locPacket = new StampedOggPacket(locBuf, inNumBytes, false, false, locStartTime, locEndTime, StampedOggPacket::OGG_BOTH);

	if (!mHandledHeaders) {
	    FLACHeaderTweaker::eFLACAcceptHeaderResult locResult = mHeaderTweaker.acceptHeader(locPacket);

	    if (locResult == FLACHeaderTweaker::LAST_HEADER_ACCEPTED) {
		    for (unsigned long i = 0; i < mHeaderTweaker.numNewHeaders(); i++) {
                mPackets.push_back(mHeaderTweaker.getHeader(i)->cloneStamped());
            }
            mHandledHeaders = true;
            return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
		} else if (locResult == FLACHeaderTweaker::HEADER_ACCEPTED) {
			return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
		} else {
			return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
		}
    } else {
        mPackets.push_back(locPacket);
        return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
    }
}
void FLACEncoder::metadata_callback(const ::FLAC__StreamMetadata *metadata)
{
    //Ignore
}
