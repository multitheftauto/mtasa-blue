/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsoleLogger.cpp
*  PURPOSE:     Console Logging functionality
*  DEVELOPERS:  Derek Abdine <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

#define MAX_STRING_LENGTH 2048
template<> CConsoleLogger * CSingleton< CConsoleLogger >::m_pSingleton = NULL;

// Used for cleaning sensitive info from logs
struct {
    uint uiNumBlanks;
    const char* szDelim;
    const char* szText;
} g_WordsToCheck [] = {
               { 2, "", "login " },
               { 2, "", "register " },
               { 2, "", "addaccount " },
               { 2, "", "chgpass " },
               { 2, "", "chgmypass " },
               { 1, "'", "password" },
            };

CConsoleLogger::CConsoleLogger ( )
{
    // Create file name
    m_strFilename = CalcMTASAPath ( MTA_CONSOLE_LOG_PATH );

    // Cycle if over size (100KB, 5 backup files)
    CycleFile( m_strFilename, 100, 5 );

    // get the file stream
    File.open ( m_strFilename.c_str(), ios::app );
}


CConsoleLogger::~CConsoleLogger ( )
{
    File.close();
}


void CConsoleLogger::WriteLine ( const string& strInLine )
{
    SString strLine = strInLine;
    CleanLine( strLine );
    // Output to log
    File                << "["
                        << GetLocalTimeString ( true )
                        << "] "
                        << strLine
                        << endl; 
}


void CConsoleLogger::LinePrintf ( const char* szFormat, ... )
{
    char szBuffer [MAX_STRING_LENGTH];

    // Convert it to a string
    va_list marker;
    va_start ( marker, szFormat );
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Replace possible LF
    for ( int i = 0; i != MAX_STRING_LENGTH; i++ )
    {
        if ( szBuffer[i] == 10 )
        {
            szBuffer[i] = ' ';
        }
    }

    // Send to output function
    WriteLine ( szBuffer );
}


//
// Clean line of sensitive info
//
void CConsoleLogger::CleanLine( SString& strLine )
{
    const char* szBlankText = "";
    for ( uint i = 0 ; i < NUMELMS( g_WordsToCheck ) ; i++ )
    {
        int uiNumBlanks = g_WordsToCheck[i].uiNumBlanks;
        const char* szDelim = g_WordsToCheck[i].szDelim;
        const char* szText = g_WordsToCheck[i].szText;
        int iPos = strLine.ToLower().find( szText );
        if ( iPos != std::string::npos )
        {
            iPos += strlen( szText );
            if ( szDelim[0] )
                iPos = strLine.find_first_of( szDelim, iPos );
            if ( iPos != std::string::npos )
            {
                while ( uiNumBlanks-- )
                {
                    iPos = ReplaceNextWord( strLine, iPos, szBlankText );
                    // Also remove trailing space if present
                    if ( strLine.SubStr( iPos, 1 ) == " " )
                        strLine = strLine.SubStr( 0, iPos ) + strLine.SubStr( iPos + 1 );
                }
            }
        }      
    }
}

//
// Replace next word in string with something
//
int CConsoleLogger::ReplaceNextWord( SString& strLine, int iPos, const char* szBlankText )
{
    int iStart = strLine.find_first_not_of( "': ", iPos );
    if ( iStart != std::string::npos )
    {
        int iEnd = strLine.find_first_of( "' ", iStart );
        if ( iEnd == std::string::npos )
            iEnd = strLine.length();
        strLine = strLine.SubStr( 0, iStart ) + szBlankText + strLine.SubStr( iEnd );
        iPos = iStart + strlen( szBlankText );
    }
    return iPos;
}
