/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CConsoleLogger.cpp
 *  PURPOSE:     Console Logging functionality
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using namespace std;

#define MAX_STRING_LENGTH 2048
template <>
CConsoleLogger* CSingleton<CConsoleLogger>::m_pSingleton = NULL;

CConsoleLogger::CConsoleLogger()
{
    // Create file name
    m_strFilename = CalcMTASAPath(MTA_CONSOLE_LOG_PATH);

    // Cycle if over size (100KB, 5 backup files)
    CycleFile(m_strFilename, 100, 5);

    // get the file stream
    File.open(m_strFilename.c_str(), ios::app);
}

CConsoleLogger::~CConsoleLogger()
{
    File.close();
}

void CConsoleLogger::WriteLine(const string& strInLine)
{
    SString strLine = strInLine;
    CEntryHistory::CleanLine(strLine);
    // Output to log
    File << "[" << GetLocalTimeString(true) << "] " << strLine << endl;
}

void CConsoleLogger::LinePrintf(const char* szFormat, ...)
{
    char szBuffer[MAX_STRING_LENGTH];

    // Convert it to a string
    va_list marker;
    va_start(marker, szFormat);
    VSNPRINTF(szBuffer, MAX_STRING_LENGTH, szFormat, marker);
    va_end(marker);

    // Replace possible LF
    for (int i = 0; i != MAX_STRING_LENGTH; i++)
    {
        if (szBuffer[i] == 10)
        {
            szBuffer[i] = ' ';
        }
    }

    // Send to output function
    WriteLine(szBuffer);
}
