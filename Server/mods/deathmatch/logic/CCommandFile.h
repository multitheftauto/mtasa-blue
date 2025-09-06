/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCommandFile.h
 *  PURPOSE:     Command file parser class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "stdio.h"
class CConsole;
class CClient;

class CCommandFile
{
public:
    CCommandFile(const char* szFilename, CConsole& Console, CClient& Client);
    ~CCommandFile();

    bool IsValid() { return m_pFile != NULL; };
    bool Run();

private:
    bool Parse(char* szLine);

    static char*       SkipWhitespace(char* szLine);
    static void        TrimRightWhitespace(char* szLine);
    static inline bool IsWhitespace(char cChar) { return cChar == 32 || cChar == 9 || cChar == 10 || cChar == 13; };

    CConsole& m_Console;
    CClient&  m_Client;

    FILE* m_pFile;
    bool  m_bEcho;
};
