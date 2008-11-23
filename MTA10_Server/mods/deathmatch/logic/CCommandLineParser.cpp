/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCommandLineParser.cpp
*  PURPOSE:     Command line parser class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CCommandLineParser::CCommandLineParser ( void )
{
    m_bMainConfig = false;
    m_szMainConfig [0] = 0;
    m_bHTTPEnabled = true;
}


bool CCommandLineParser::Parse ( int iArgumentCount, char* szArguments [] )
{
    // HTTP is enabld unless otherwize specified
    m_bHTTPEnabled = true;

    // While we're not out of arguments
    int iIndex = 0;
    while ( iIndex < iArgumentCount )
    {
        // Any argument type we know?
        if ( stricmp ( szArguments [iIndex], "--config" ) == 0 )
        {
            // Grab the next argument
            if ( ++iIndex < iArgumentCount )
            {
                m_bMainConfig = true;
                strncpy ( m_szMainConfig, szArguments [iIndex], 256 );
                m_szMainConfig[255] = '\0';
            }
        }

        // We can't disable HTTP because then downloading resource files doesn't work
        /*
        else if ( stricmp ( szArguments [iIndex], "-nohttp" ) == 0 )
        {
            m_bHTTPEnabled = false;
        }
        */

        // Increment the index
        ++iIndex;
    }

    // Success
    return true;
}


bool CCommandLineParser::GetMainConfig ( const char*& pszMainConfig )
{
    pszMainConfig = m_szMainConfig;
    return m_bMainConfig;
}
