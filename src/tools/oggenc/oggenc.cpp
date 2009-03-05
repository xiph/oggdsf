// Copyright (C) 2009 Cristian Adam
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// - Neither the name of Cristian Adam nor the names of contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <iostream> 
#include <sstream>
#include <iomanip>
#include <typeinfo>
#include <map>
 
// Disable any warnings issued by the import directives 
#pragma warning (push, 0)  
#import <quartz.dll> rename_namespace("dshow") 
#import <qedit.dll> rename_namespace("dshow") 
#pragma warning (pop) 

#include <strmif.h> 
#include <vfwmsgs.h> 
#include <uuids.h> 

// Vorbis Encode Filter
const GUID CLSID_VorbisEncoder = {0x5c94fe86, 0xb93b, 0x467f, {0xbf, 0xc3, 0xbd, 0x6c, 0x91, 0x41, 0x6f, 0x9b} };

// Ogg Mux Filter
const GUID CLSID_OggMuxFilter = {0x1f3effe4, 0x0e70, 0x47c7, {0x9c, 0x48, 0x05, 0xeb, 0x99, 0xe2, 0x00, 0x11 } };

// Vorbis Encode Settings
struct __declspec(uuid("a4c6a887-7bd3-4b33-9a57-a3eb10924d3a"))
IVorbisEncodeSettings: IUnknown 
{
    virtual IVorbisEncodeSettings __stdcall getEncoderSettings() = 0;
    
    virtual bool __stdcall setQuality(int inQuality) = 0;
    virtual bool __stdcall setBitrateQualityMode(int inBitrate) = 0;
    virtual bool __stdcall setManaged(int inBitrate, int inMinBitrate, int inMaxBitrate) = 0;
};

// Ogg Mux Progress
struct __declspec(uuid("90d6513c-a665-4b16-aca7-b3d1d4efe58d"))
IOggMuxProgress: IUnknown
{
    virtual LONGLONG __stdcall getProgressTime() = 0;
    virtual LONGLONG __stdcall getBytesWritten() = 0;
};


_COM_SMARTPTR_TYPEDEF(IVorbisEncodeSettings, __uuidof(IVorbisEncodeSettings));
_COM_SMARTPTR_TYPEDEF(IOggMuxProgress, __uuidof(IOggMuxProgress));
_COM_SMARTPTR_TYPEDEF(IFileSinkFilter, __uuidof(IFileSinkFilter));

#pragma comment(lib, "strmiids.lib") 
#pragma comment(lib, "winmm.lib")

const __int64 MILLI_SECONDS = 1000;            // 10 ^ 3 
const __int64 NANO_SECONDS = 1000000000;       // 10 ^ 9 
const __int64 UNITS = NANO_SECONDS / 100;      // 10 ^ 7 

struct ComInitializer 
{ 
    ComInitializer() 
    { 
        ::CoInitialize(0); 
    } 
    ~ComInitializer() 
    { 
        ::CoUninitialize(); 
    } 
} g_comInit; 
 

REFERENCE_TIME GetAudioSourceDuration(const std::wstring& sourceFile);
dshow::IPinPtr FindPin(dshow::IBaseFilterPtr filter, dshow::_PinDirection desiredDirection);
std::string NarrowString(const WCHAR* message);
void ThrowIfError(HRESULT hr, const std::string& reason);
void PrintHeader();
void PrintHelp();
void PrintProgress(LONGLONG done, LONGLONG total, LONGLONG elapsed);
void PrintStatistics(const std::wstring& file, LONGLONG total, LONGLONG duration, LONGLONG size);
bool ParseCommandlineParameters(int argc, wchar_t* argv[]);

struct Options
{
    double quality;
    int bitrate;

    bool isManaged;

    int managedBitrate;
    int managedMinBitrate;
    int managedMaxBitrate;

    std::wstring sourceFile;
    std::wstring oggFile;

    Options() :
        quality(3.0),
        bitrate(0),
        isManaged(false),
        managedBitrate(0),
        managedMinBitrate(0),
        managedMaxBitrate(0)
    {
    }

} g_options;


int wmain(int argc, wchar_t* argv[]) 
{ 
    using namespace dshow; 
    using namespace std; 
 
    if (!ParseCommandlineParameters(argc, argv))
    {
        return 1;
    }
  
    try 
    { 
        IGraphBuilderPtr graphBuilder; 
 
        HRESULT hr = graphBuilder.CreateInstance(CLSID_FilterGraph); 
        ThrowIfError(hr, "ERROR: could not create filter graph object");

        IFilterGraphPtr filterGraph = graphBuilder;
  
        // Add a renderer, which will be replaced by the encoder
        IBaseFilterPtr directSound;
        hr = directSound.CreateInstance(CLSID_DSoundRender);
        ThrowIfError(hr, "ERROR: could not create direct sound renderer");

        filterGraph->AddFilter(directSound, L"Direct Sound Renderer");

        // Render the audio source file using intelligent connect
        hr = graphBuilder->raw_RenderFile(const_cast<wchar_t*>(g_options.sourceFile.c_str()), 0); 
        ThrowIfError(hr, "ERROR: could not render the file");

        // Get the audio decoder output pin
        IPinPtr directSoundRendererInput = FindPin(directSound, dshow::PINDIR_INPUT);

        IPinPtr audioRendererPin;
        hr = directSoundRendererInput->raw_ConnectedTo(&audioRendererPin);
        ThrowIfError(hr, "ERROR: could not get audio renderer output pin");

        // Remove the DirectSound renderer
        audioRendererPin->Disconnect();
        filterGraph->RemoveFilter(directSound);

        // Add the Vorbis Encoder
        IBaseFilterPtr vorbisEncoder;
        hr = vorbisEncoder.CreateInstance(CLSID_VorbisEncoder);
        ThrowIfError(hr, "ERROR: could not create vorbis encoder object");

        filterGraph->AddFilter(vorbisEncoder, L"Vorbis Encoder");

        // Connect the audio decoder to the Vorbis Encoder
        IPinPtr vorbisEncoderInput = FindPin(vorbisEncoder, dshow::PINDIR_INPUT);

        hr = filterGraph->raw_ConnectDirect(audioRendererPin, vorbisEncoderInput, 0);
        ThrowIfError(hr, "ERROR: could not connect audio renderer to vorbis encoder");

        // Add the Ogg Mux Filter
        IBaseFilterPtr oggMuxer;
        hr = oggMuxer.CreateInstance(CLSID_OggMuxFilter);
        ThrowIfError(hr, "ERROR: could not create ogg mux object");

        filterGraph->AddFilter(oggMuxer, L"Ogg Muxer");

        // Connect the Vorbis Encoder to Ogg Muxer
        IPinPtr vorbisEncoderOutput = FindPin(vorbisEncoder, dshow::PINDIR_OUTPUT);
        IPinPtr oggMuxerInput = FindPin(oggMuxer, dshow::PINDIR_INPUT);

        hr = filterGraph->raw_ConnectDirect(vorbisEncoderOutput, oggMuxerInput, 0);
        ThrowIfError(hr, "ERROR: could not connect vorbis encoder to ogg muxer");

        // Get the encoder settings object
        IVorbisEncodeSettingsPtr encodeSettings = vorbisEncoder;

        // Set the name of the Ogg file
        IFileSinkFilterPtr oggSource = oggMuxer;

        hr = oggSource->SetFileName(g_options.oggFile.c_str(), 0);
        ThrowIfError(hr, "ERROR: could not set ogg muxer file");

        // Get the progress from the Ogg Muxer
        IOggMuxProgressPtr oggMuxProgress = oggMuxer;
 
        REFERENCE_TIME audioSourceDuration = GetAudioSourceDuration(g_options.sourceFile);

        wcout << L"Encoding \"" << g_options.sourceFile.c_str() << L"\" to" << endl;
        wcout << L"\t \"" << g_options.oggFile.c_str() << L"\"" << endl;

        if (g_options.bitrate != 0)
        {
            wcout << L"WARNING: oggcodecs build version <= 0.81.15562 do not work correctly!" << endl;
            wcout << L"WARNING: at the end of the encoding, the average bitrate does not" << endl;
            wcout << L"WARNING: have the expected value." << endl;

            encodeSettings->setBitrateQualityMode(g_options.bitrate);
        }
        else if (g_options.managedMinBitrate != 0 && g_options.managedMaxBitrate != 0)
        {
            wcout << L"using bitrate management (min " << g_options.managedMinBitrate << L" kbps, ";
            wcout << L"max " << g_options.managedMaxBitrate << L" kbps)" << endl;

            encodeSettings->setManaged(g_options.bitrate, g_options.managedMinBitrate, 
                g_options.managedMaxBitrate);
        }
        else
        {
            wcout << L"at quality " << fixed << setprecision(1) << g_options.quality << endl;

            encodeSettings->setQuality(static_cast<int>(g_options.quality * 10));
        }

        unsigned long startTime = timeGetTime(); 

        // Start the encoding
        IMediaControlPtr control = graphBuilder; 
        control->Run(); 

        LONGLONG destinationFileSize;
        bool isEncoding = true;
        while (isEncoding)
        {
            LONGLONG muxerProgressTime = oggMuxProgress->getProgressTime();
            destinationFileSize = oggMuxProgress->getBytesWritten();
                                
            // Give a margin of 10 nanoseconds
            if (muxerProgressTime >= audioSourceDuration - 10)
            {
                muxerProgressTime = audioSourceDuration;

                control->Stop();
                isEncoding = false;
            }
            else
            {
                // Sleep 50 milliseconds
                Sleep(50);
            }

            unsigned long currentTime = ::timeGetTime();

            PrintProgress(muxerProgressTime, audioSourceDuration, (currentTime - startTime) * 10000);
        } 

        unsigned long endTime = timeGetTime(); 
        PrintStatistics(g_options.oggFile, audioSourceDuration, (endTime - startTime) * 10000, destinationFileSize);
    } 
    catch(const std::runtime_error& err) 
    { 
        wcout << err.what() << endl; 
    } 
    catch(const _com_error& err)
    {
        wcout << L"Error code: 0x" << hex << err.Error() << L"(" << err.ErrorMessage() << L")" << endl;
    }
    catch (...)
    {
        wcout << "Unknown exception!";
    }
     
    return 0; 
} 

REFERENCE_TIME GetAudioSourceDuration(const std::wstring& sourceFile)
{
    dshow::IMediaDetPtr mediaDet;
    mediaDet.CreateInstance(__uuidof(dshow::MediaDet));

    mediaDet->Filename = sourceFile.c_str();

    long streams = mediaDet->OutputStreams;

    mediaDet->CurrentStream = streams - 1;

    double lengthInSeconds = mediaDet->StreamLength;

    return static_cast<REFERENCE_TIME>(lengthInSeconds * UNITS);
}


dshow::IPinPtr FindPin(dshow::IBaseFilterPtr filter, dshow::_PinDirection desiredDirection)
{
    dshow::IEnumPinsPtr enumerator;
    filter->EnumPins(&enumerator);

    dshow::IPinPtr pin;

    unsigned long fetched = 0;
    while (enumerator->Next(1, &pin, &fetched) == S_OK)
    {
        dshow::_PinDirection direction;
        pin->QueryDirection(&direction);

        if (direction == desiredDirection)
        {
            return pin;
        }
        pin = 0;
    }

    return 0;
}

std::string NarrowString(const WCHAR* message)
{
    std::string result;

    int chars = ::WideCharToMultiByte(CP_ACP, 0, message, -1, 0, 0, 0, 0);
    result.resize(chars);

    ::WideCharToMultiByte(CP_ACP, 0, message, -1, &*result.begin(), chars, 0, 0);

    return result;
}


void ThrowIfError(HRESULT hr, const std::string& reason)
{
    if (FAILED(hr))
    {
        std::ostringstream os;

        os << reason.c_str() << ". Error Code: 0x" << std::hex << hr 
            << " (" << NarrowString(_com_error(hr).ErrorMessage()).c_str() << ")";

        throw std::runtime_error(os.str());
    }
}

void PrintHeader()
{
    using namespace std;

    wcout << "DirectShow OggEnc 0.1" << endl;
    wcout << "(c) 2009 Cristian Adam" << endl;
    wcout << endl;
}

void PrintHelp()
{
    using namespace std;

    PrintHeader();

    wcout << "Usage: oggenc [options] input.wav [...]" << endl;
    wcout << endl;
    wcout << "OPTIONS" << endl;
    wcout << " General" << endl;
    wcout << " -h, --help           Print this help text" << endl; 
    wcout << " -b, --bitrate        Choose a nominal bitrate to encode at. Attempt" << endl;
    wcout << "                      to encode at a bitrate averaging this. Takes an" << endl;
    wcout << "                      argument in kbps. By default, this produces a VBR" << endl;
    wcout << "                      encoding, equivalent to using -q or --quality." << endl;
    wcout << "                      See the --managed option to use a managed bitrate" << endl;
    wcout << "                      targetting the selected bitrate." << endl; 
    wcout << " --managed            Enable the bitrate management engine. This will allow" << endl;
    wcout << "                      much greater control over the precise bitrate(s) used," << endl;
    wcout << "                      but encoding will be much slower. Don't use it unless" << endl;
    wcout << "                      you have a strong need for detailed control over" << endl;
    wcout << "                      bitrate, such as for streaming." << endl;
    wcout << " -m, --min-bitrate    Specify a minimum bitrate (in kbps). Useful for" << endl;
    wcout << "                      encoding for a fixed-size channel. Using this will" << endl;
    wcout << "                      automatically enable managed bitrate mode (see" << endl;
    wcout << "                      --managed)." << endl;
    wcout << " -M, --max-bitrate    Specify a maximum bitrate in kbps. Useful for" << endl;
    wcout << "                      streaming applications. Using this will automatically" << endl;
    wcout << "                      enable managed bitrate mode (see --managed)." << endl; 
    wcout << " -q, --quality        Specify quality, between -1.0 (very low) and 10.0 (very" << endl;
    wcout << "                      high), instead of specifying a particular bitrate." << endl;
    wcout << "                      This is the normal mode of operation." << endl;
    wcout << "                      The default quality level is 3.0" << endl;
    wcout << " Naming" << endl;
    wcout << " -o, --output fn      Write encoded file to fn" << endl;
    wcout << endl;
    wcout << "INPUT FILES:" << endl;
    wcout << "DirectShow OggEnc input files must currently be PCM WAV, MP3, or WMA files." <<endl;
    wcout << "Files may be mono or stereo (or more channels) and any sample rate." << endl;
}

// MmSSs
std::wstring ToString1(REFERENCE_TIME time) 
{     
    long milliseconds = static_cast<long>(time / (UNITS / MILLI_SECONDS)); 

    using namespace std;

    unsigned long minutes = milliseconds / 1000 % 3600 / 60;
    unsigned long seconds = milliseconds / 1000 % 60;

    std::wostringstream wos;

    wos << setw(2) << minutes << L"m" << setw(2);
    wos << setfill(L'0') << seconds << L"s";
    wos << setfill(L' ');

    return wos.str();
} 

// Mm SS,Ss
std::wstring ToString2(REFERENCE_TIME time) 
{     
    long milliseconds = static_cast<long>(time / (UNITS / MILLI_SECONDS)); 

    using namespace std;

    unsigned long minutes = milliseconds / 1000 % 3600 / 60;
    double seconds = milliseconds / 1000 % 60;
    seconds += static_cast<double>(milliseconds / 100 % 10) / 10.0;

    std::wostringstream wos;

    wos << setw(2) << minutes << L"m " << setw(2);
    wos << setfill(L'0') << setw(3) << fixed << setprecision(1) << seconds << "s";
    wos << setfill(L' ');

    return wos.str();
} 

void PrintProgress(LONGLONG done, LONGLONG total, LONGLONG elapsed)
{
    static char* spinner="|/-\\";
    static int spinpoint = 0; 

    if (done == 0)
    {
        return;
    }

    double percent = static_cast<double>(done) / total * 100.0f;
    LONGLONG remain_time = static_cast<LONGLONG>(elapsed / (static_cast<double>(done) / total) - elapsed);

    using namespace std;

    wcout << setfill(L' ');
    wcout << L"\r";
    wcout << L"\tEncoding [" << setw(5) << fixed << setprecision(1) << percent << L"%] ";
    wcout << L"[" << ToString1(remain_time) << L" remaining] ";
    wcout << L" " << spinner[spinpoint++%4];
}

void PrintStatistics(const std::wstring& file, LONGLONG total, LONGLONG duration, LONGLONG size)
{
    using namespace std;

    wcout << endl;
    wcout << endl;
    wcout << L"Done encoding file \"" << file << L"\"";
    wcout << endl;
    wcout << endl;
    wcout << L"\tFile length:  " << ToString2(total) << endl;
    wcout << L"\tElapsed time: " << ToString2(duration) << endl;
    wcout << L"\tRate:         " << setw(8) << setprecision(4) << 
        static_cast<double>(total) / duration << endl;

    unsigned long durationSeconds = static_cast<unsigned long>(total / (UNITS / MILLI_SECONDS) / 1000);
    wcout << L"\tAverage bitrate: " << setprecision(1) << 
        static_cast<double>(size * 8 / 1000) / durationSeconds << L" kb/s" << endl;
}


struct Any
{
    void* pointer;
    std::string pointer_type;

    template <typename T>
    Any(T* value)
    {
        pointer = value;
        pointer_type = typeid(T).name();
    }

    template <typename T>
    Any& operator = (const T& t)
    {
        if (typeid(T).name() != pointer_type)
        {
            throw std::runtime_error("different types");
        }

        *reinterpret_cast<T*>(pointer) = t;

        return *this;
    }
};

void StringToAny(Any* any, const std::wstring& str)
{
    std::wistringstream wis(str);

    if (any->pointer_type == typeid(int).name())
    {
        int value;
        wis >> value;
        *any = value;
    }
    else if (any->pointer_type == typeid(double).name())
    {
        double value;
        wis >> value;
        *any = value;
    }
    else if (any->pointer_type == typeid(bool).name())
    {
        bool value;
        wis >> value;
        *any = value;
    }
    else if (any->pointer_type == typeid(std::wstring).name())
    {
        *any = str;
    }
}

bool ParseCommandlineParameters(int argc, wchar_t* argv[])
{
    using namespace std;

    map<wstring, Any*> paramters;

    paramters.insert(std::make_pair(L"-b", new Any(&g_options.bitrate)));
    paramters.insert(std::make_pair(L"--bitrate", new Any(&g_options.bitrate)));
    paramters.insert(std::make_pair(L"-q", new Any(&g_options.quality)));
    paramters.insert(std::make_pair(L"--quality", new Any(&g_options.quality)));
    paramters.insert(std::make_pair(L"--managed", new Any(&g_options.isManaged)));
    paramters.insert(std::make_pair(L"-m", new Any(&g_options.managedMinBitrate)));
    paramters.insert(std::make_pair(L"--min-bitrate", new Any(&g_options.managedMinBitrate)));
    paramters.insert(std::make_pair(L"-M", new Any(&g_options.managedMaxBitrate)));
    paramters.insert(std::make_pair(L"--max-bitrate", new Any(&g_options.managedMaxBitrate)));
    paramters.insert(std::make_pair(L"-o", new Any(&g_options.oggFile)));
    paramters.insert(std::make_pair(L"--output", new Any(&g_options.oggFile)));

    for (int i = 1; i < argc; ++i)
    {
        if (paramters.find(argv[i]) != paramters.end())
        {
            if (i + 1 < argc)
            {
                StringToAny(paramters[argv[i]], argv[i + 1]);
                ++i;
            }
        }
        else if (argv[i] == wstring(L"-h") || argv[i] == wstring(L"--help"))
        {
            PrintHelp();
            return false;
        }
        else if (wstring(argv[i]).rfind(L".ogg") != wstring::npos ||
                 wstring(argv[i]).rfind(L".oga") != wstring::npos)
        {
            g_options.oggFile = argv[i];
        }
        else
        {
            g_options.sourceFile = argv[i];
        }
    }

    if (g_options.sourceFile.empty())
    {
        PrintHeader();
        wcout << L"ERROR: No input files specified. Use -h for help." << endl;
        return false;
    }

    if (g_options.quality < -1.0 || g_options.quality > 10.0)
    {
        PrintHeader();
        wcout << L"ERROR: accepted quality interval is -1.0 to 10.0";
        return false;
    }

    if (g_options.managedMinBitrate != 0 && g_options.managedMaxBitrate != 0)
    {
        if (g_options.managedMinBitrate >= g_options.managedMaxBitrate)
        {
            wcout << L"ERROR: min bitrate should be less than max bitrate";
            return false;
        }
        else if (g_options.managedMinBitrate > 500 || g_options.managedMaxBitrate > 500)
        {
            wcout << L"WARNINING: managed bitrates greater than 500 kbps might not work" << endl;
            wcout << endl;
        }
        else if (g_options.managedMinBitrate < 50 || g_options.managedMaxBitrate < 50)
        {
            wcout << L"WARNINING: managed bitrates less than 50 kbps might not work" << endl;
            wcout << endl;
        }
    }

    if (g_options.oggFile.empty())
    {
        wstring::size_type position = g_options.sourceFile.rfind(L'.');
        g_options.oggFile = g_options.sourceFile.substr(0, position);
        g_options.oggFile += L".ogg";
    }

    for (map<wstring, Any*>::iterator it = paramters.begin(); it != paramters.end(); ++it)
    {
        delete it->second;
    }

    return true;
}   
