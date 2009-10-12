#ifndef UTIL_H
#define UTIL_H

#include <shlobj.h>
#include <fstream>
#include "pugixml/pugixml.hpp"

namespace util
{
    inline void ConfigureLog(HANDLE hModule)
    {
        using namespace std;
        using namespace pugi;

        // Obtain the module name
        wstring moduleFileName;
        moduleFileName.resize(MAX_PATH);

        int chars = ::GetModuleFileName(static_cast<HMODULE>(hModule), &*moduleFileName.begin(), moduleFileName.size());
        moduleFileName.resize(chars);

        size_t lastBackslash = moduleFileName.rfind(L'\\') + 1;
        size_t lastDot = moduleFileName.rfind(L'.');

        wstring moduleName = moduleFileName.substr(lastBackslash, lastDot - lastBackslash);

        // Obtain the user data configuration directory
        wstring configLocation;
        configLocation.resize(MAX_PATH);
        ::SHGetSpecialFolderPath(0, &*configLocation.begin(), CSIDL_APPDATA, false);
        configLocation.resize(wcslen(configLocation.c_str()));

        configLocation += L"\\Xiph.org\\oggcodecs";

        // Open the settings xml and read the log configuration
        wstring xmlFileName = configLocation;
        xmlFileName += L"\\settings.xml";

        ifstream xmlConfigStream;
        xmlConfigStream.open(xmlFileName.c_str());

        xml_document doc;
        doc.load(xmlConfigStream);

        stringstream queryString;
        queryString << "/Configuration/Module[@Name=\"" << CW2A(moduleName.c_str()) << "\"]/Log";

        xpath_query query(queryString.str().c_str());

        string levelString = doc.select_single_node(query).node().attribute("Level").value();

        unsigned short level = logNONE;
        if (!levelString.empty())
        {
            istringstream is;
            is.str(levelString);

            is >> level;
        }

        Log::ReportingLevel() = static_cast<LogLevel>(level);

        if (level != logNONE)
        {
            wstring logFileName = configLocation;
            logFileName += L"\\";
            logFileName += moduleName + L".log";
            
            Log::Stream(logFileName);
        }
    }
}

#endif // UTIL_H