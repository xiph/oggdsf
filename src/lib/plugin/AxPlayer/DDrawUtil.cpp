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

#include "StdAfx.h"
#include "DDrawUtil.h"

#include <ddraw.h>

std::string DDrawUtil::GetSurfaceDescFlags(unsigned long flags )
{
    typedef std::map<unsigned long, std::string> SurfaceDescFlagsToStringMap;
    static SurfaceDescFlagsToStringMap surfaceDescFlagsToStringMap;

#define ADD_FLAG(flag) \
    surfaceDescFlagsToStringMap[flag] = #flag; 

    if (surfaceDescFlagsToStringMap.empty())
    {
        ADD_FLAG(DDSD_ALL);
        ADD_FLAG(DDSD_ALPHABITDEPTH);
        ADD_FLAG(DDSD_BACKBUFFERCOUNT);
        ADD_FLAG(DDSD_CAPS);
        ADD_FLAG(DDSD_CKDESTBLT);
        ADD_FLAG(DDSD_CKDESTOVERLAY);
        ADD_FLAG(DDSD_CKSRCBLT);
        ADD_FLAG(DDSD_CKSRCOVERLAY);
        ADD_FLAG(DDSD_HEIGHT);
        ADD_FLAG(DDSD_LINEARSIZE);
        ADD_FLAG(DDSD_MIPMAPCOUNT);
        ADD_FLAG(DDSD_PITCH);
        ADD_FLAG(DDSD_PIXELFORMAT);
        ADD_FLAG(DDSD_REFRESHRATE);
        ADD_FLAG(DDSD_WIDTH);
        ADD_FLAG(DDSD_ZBUFFERBITDEPTH);
    }
#undef ADD_FLAG

    std::string result;

    SurfaceDescFlagsToStringMap::const_iterator it = surfaceDescFlagsToStringMap.begin();
    for (; it != surfaceDescFlagsToStringMap.end(); ++it)
    {
        if ((it->first & flags) == it->first)
        {
            result += it->second;
            result += " ";
        }
    }

    return result;
}

std::string DDrawUtil::GetPixelFormatFlags(unsigned long flags )
{
    typedef std::map<unsigned long, std::string> PixelFormatFlagsToStringMap;
    static PixelFormatFlagsToStringMap pixelFormatFlagsToStringMap;

#define ADD_FLAG(flag) \
    pixelFormatFlagsToStringMap[flag] = #flag; 

    if (pixelFormatFlagsToStringMap.empty())
    {
        ADD_FLAG(DDPF_ALPHA);
        ADD_FLAG(DDPF_ALPHAPIXELS);
        ADD_FLAG(DDPF_ALPHAPREMULT);
        ADD_FLAG(DDPF_BUMPDUDV);
//        ADD_FLAG(DDPF_BUMPHEIGHT);
        ADD_FLAG(DDPF_COMPRESSED);
//        ADD_FLAG(DDPF_D3DFORMAT);
        ADD_FLAG(DDPF_FOURCC);
        ADD_FLAG(DDPF_LUMINANCE);
//        ADD_FLAG(DDPF_LUMINANCEPIXELS);
//        ADD_FLAG(DDPF_NOVEL_TEXTURE_FORMAT);
        ADD_FLAG(DDPF_PALETTEINDEXED1);
        ADD_FLAG(DDPF_PALETTEINDEXED2);
        ADD_FLAG(DDPF_PALETTEINDEXED4);
        ADD_FLAG(DDPF_PALETTEINDEXED8);
        ADD_FLAG(DDPF_PALETTEINDEXEDTO8);
        ADD_FLAG(DDPF_RGB);
        ADD_FLAG(DDPF_RGBTOYUV);
        ADD_FLAG(DDPF_STENCILBUFFER);
        ADD_FLAG(DDPF_YUV);
        ADD_FLAG(DDPF_ZBUFFER);
        ADD_FLAG(DDPF_ZPIXELS);
    }
#undef ADD_FLAG

    std::string result;

    PixelFormatFlagsToStringMap::const_iterator it = pixelFormatFlagsToStringMap.begin();
    for (; it != pixelFormatFlagsToStringMap.end(); ++it)
    {
        if ((it->first & flags) == it->first)
        {
            result += it->second;
            result += " ";
        }
    }

    return result;
}
