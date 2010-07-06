/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCommandFile.cpp
*  PURPOSE:     Command file parser class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CCommandFile::CCommandFile ( const char* szFilename,
                             CConsole& Console,
                             CClient& Client ) :
m_Console ( Console ),
m_Client ( Client )

{
    // Load the given file
    m_pFile = fopen ( szFilename, "r" );
    m_bEcho = true;
}


CCommandFile::~CCommandFile ( void )
{
    // Unload it again
    if ( m_pFile )
    {
        fclose ( m_pFile );
    }
}


bool CCommandFile::Run ( void )
{
    // Got a file?
    if ( m_pFile )
    {
        // Read lines
        char szBuffer [2048];
        szBuffer [2047] = 0;
        while ( fgets ( szBuffer, 2047, m_pFile ) )
        {
            // Parse it. Don't continue on error.
            if ( !Parse ( szBuffer ) )
            {
                return false;
            }
        }

        // Success
        return true;
    }

    // Nope
    return false;
}


bool CCommandFile::Parse ( char* szLine )
{
    // Skip whitespace first, then trim off the right whitespace
    szLine = SkipWhitespace ( szLine );
    TrimRightWhitespace ( szLine );

    // More than 1 character left?
    if ( szLine [0] != 0 )
    {
        // Got a lua comment comming? --, skip it
        if ( szLine [0] == '-' && szLine [1] == '-' )
        {
            return true;
        } 

        // Is it @echo?
        if ( strncmp ( szLine, "@echo", 5 ) == 0 )
        {
            // Skip whitespace again from the end of @echo
            const char* szEchoMode = SkipWhitespace ( szLine + 5 );

            // Off or on?
            if ( stricmp ( szEchoMode, "off" ) == 0 )
                m_bEcho = false;
            else if ( stricmp ( szEchoMode, "on" ) == 0 )
                m_bEcho = true;
            else
            {
                CLogger::ErrorPrintf ( "Unknown @echo mode. Valid modes are 'on' and 'off'.\n" );
                return false;
            }

            // Done
            return true;
        }

        // Echo the command if asked to
        if ( m_bEcho )
        {
            CLogger::LogPrintf ( "%s\n", szLine );
        }

        // Run the command
        m_Console.HandleInput ( szLine, &m_Client, &m_Client );
        return true;
    }

    // Empty line always works
    return true;
}


char* CCommandFile::SkipWhitespace ( char* szLine )
{
    // While we're not at the end
    while ( *szLine != 0 )
    {
        // Not a space, tab or line feed? Return this position
        if ( !IsWhitespace ( *szLine ) )
        {
            return szLine;
        }

        // Next character
        ++szLine;
    }

    // Return the current position
    return szLine;
}


void CCommandFile::TrimRightWhitespace ( char* szLine )
{
    // Go to the end
    char* szOriginalLine = szLine;
    while ( *szLine )
    {
        ++szLine;
    }

    // Go back trimming whitespace
    while ( ( szLine >= szOriginalLine ) &&
            ( *szLine == 0 || IsWhitespace ( *szLine ) ) )
    {
        *szLine = 0;
        --szLine;
    }
}
