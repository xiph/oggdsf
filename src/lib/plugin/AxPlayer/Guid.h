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

#ifndef GUID_H
#define GUID_H

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <tchar.h>

class Guid : public GUID
{
public:
    typedef std::basic_string<TCHAR> string;
    typedef std::basic_istringstream<TCHAR> istringstream;
    typedef std::basic_ostringstream<TCHAR> ostringstream;
    typedef std::basic_ostream<TCHAR> ostream;

    Guid()
    {
        Data1 = 0;
        Data2 = 0;
        Data3 = 0;
        Data4[0] = 0;
        Data4[1] = 0;
        Data4[2] = 0;
        Data4[3] = 0;
        Data4[4] = 0;
        Data4[5] = 0;
        Data4[6] = 0;
        Data4[7] = 0;
    }

    Guid(const Guid& other)
    {
        Data1 = other.Data1; 
        Data2 = other.Data2;
        Data3 = other.Data3;
        Data4[0] = other.Data4[0]; 
        Data4[1] = other.Data4[1]; 
        Data4[2] = other.Data4[2]; 
        Data4[3] = other.Data4[3]; 
        Data4[4] = other.Data4[4]; 
        Data4[5] = other.Data4[5]; 
        Data4[6] = other.Data4[6]; 
        Data4[7] = other.Data4[7];
    }

    template <class T>
    T strToNum(const string &inputString, std::ios_base &(*f)(std::ios_base&) = std::dec)
    {
        T t;
        istringstream stringStream(inputString);

        if ((stringStream >> f >> t).fail())
        {
            throw std::runtime_error("Invalid conversion");
        }
        return t;
    }

    Guid (const string& stringGuid)
    {
        assert(stringGuid[0]  == _T('{') && 
               stringGuid[9]  == _T('-') &&
               stringGuid[14] == _T('-') &&
               stringGuid[19] == _T('-') &&
               stringGuid[24] == _T('-') &&
               stringGuid[37] == _T('}'));

        const string data1Str = stringGuid.substr(1, 8);
        Data1 = strToNum<unsigned long>(data1Str, std::hex);

        const string data2Str = stringGuid.substr(10, 4);
        Data2 = strToNum<unsigned short>(data2Str, std::hex);

        const string data3Str = stringGuid.substr(15, 4);
        Data3 = strToNum<unsigned short>(data3Str, std::hex);

        for (int i = 0; i < 2; ++i)
        {
            const string data4Str = stringGuid.substr(20 + i * 2, 2);
            Data4[i] = static_cast<unsigned char>(strToNum<unsigned int>(data4Str, std::hex));
        }

        for (int i = 0; i < 6; ++i)
        {
            const string data4Str = stringGuid.substr(25 + i * 2, 2);
            Data4[i + 2] = static_cast<unsigned char>(strToNum<unsigned int>(data4Str, std::hex));
        }
    }

    operator string () const
    {
        ostringstream os;

        os << _T("{");
        ostream::char_type oldFill = os.fill(_T('0'));

        os << std::hex << std::setw(8) << Data1 << _T("-");
        os << std::hex << std::setw(4) << Data2 << _T("-");
        os << std::hex << std::setw(4) << Data3 << _T("-");

        for (int i = 0; i < 2; ++i)
        {
            os << std::hex << std::setw(2) << static_cast<unsigned short>(Data4[i]);
        }

        os << _T("-");

        for (int i = 0; i < 6; ++i)
        {
            os << std::hex << std::setw(2) << static_cast<unsigned short>(Data4[i + 2]);
        }

        os << _T("}");
        os.fill(oldFill);

        return os.str();
    }

    string ToString() const
    {
        return operator string();
    }

    bool operator < (const Guid& other) const
    {
        return  Data1 < other.Data1 && 
                Data2 < other.Data2 &&
                Data3 < other.Data3 &&
                Data4[0] < other.Data4[0] && 
                Data4[1] < other.Data4[1] && 
                Data4[2] < other.Data4[2] && 
                Data4[3] < other.Data4[3] && 
                Data4[4] < other.Data4[4] && 
                Data4[5] < other.Data4[5] && 
                Data4[6] < other.Data4[6] && 
                Data4[7] < other.Data4[7];
    }
};

#endif // GUID_H
