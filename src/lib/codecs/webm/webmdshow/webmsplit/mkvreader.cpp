// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <strmif.h>
#include "mkvreader.hpp"
#include <cassert>
#if 0
#include <limits>
#include <vfwmsgs.h>
#endif

namespace WebmSplit
{

MkvReader::MkvReader()
{
}


MkvReader::~MkvReader()
{
}


void MkvReader::SetSource(IAsyncReader* pSource)
{
    m_pSource = pSource;
}


bool MkvReader::IsOpen() const
{
    return m_pSource;
}


HRESULT MkvReader::MkvRead(
    LONGLONG start,
    LONG len,
    BYTE* ptr)
{
    //TODO: use aligned read so we can timeout the read
    return m_pSource->SyncRead(start, len, ptr);
}


HRESULT MkvReader::MkvLength(
    LONGLONG* pTotal,
    LONGLONG* pAvailable)
{
    HRESULT hr = m_pSource->Length(pTotal, pAvailable);
#if 0
    if (hr == VFW_S_ESTIMATED)
    {
        *pAvailable = *pTotal = std::numeric_limits<LONGLONG>::max();
    }
#endif
    return hr;
}


} //end namespace WebmSplit
