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
    m_bIP = false;
    m_bPort = false;
    m_bMaxPlayers = false;

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
        if ( stricmp ( szArguments [ iIndex ], "--config" ) == 0 )
        {
            // Grab the next argument
            if ( ++iIndex < iArgumentCount )
            {
                m_bMainConfig = true;
                m_strMainConfig = szArguments [iIndex];
            }
        }
        else if ( stricmp ( szArguments [ iIndex ], "--ip" ) == 0 )
        {
            if ( ++iIndex < iArgumentCount )
            {
                m_bIP = true;
                m_strIP = szArguments [ iIndex ];
            }
        }
        else if ( stricmp ( szArguments [ iIndex ], "--port" ) == 0 )
        {
            if ( ++iIndex < iArgumentCount )
            {
                m_bPort = true;
                m_usPort = (unsigned short) atoi ( szArguments [ iIndex ] );
            }
        }
        else if ( stricmp ( szArguments [ iIndex ], "--maxplayers" ) == 0 )
        {
            if ( ++iIndex < iArgumentCount )
            {
                m_bMaxPlayers = true;
                m_uiMaxPlayers = (unsigned int) atoi ( szArguments [ iIndex ] );
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
