/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCommandLineParser.h
*  PURPOSE:     Command line parser class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOMMANDLINEPARSER_H
#define __CCOMMANDLINEPARSER_H

#include "../Config.h"

class CCommandLineParser
{
public:
                                CCommandLineParser              ( void );

    bool                        Parse                           ( int iArgumentCount, char* szArguments [] );

    bool                        GetMainConfig                   ( const char*& szMainConfig )  { szMainConfig = m_strMainConfig.c_str (); return m_bMainConfig; }
    bool                        GetIP                           ( const char*& szIP )          { szIP = m_strIP.c_str (); return m_bIP; }
    bool                        GetPort                         ( unsigned short& usPort )     { usPort = m_usPort; return m_bPort; }
    bool                        GetMaxPlayers                   ( unsigned int& uiMaxPlayers ) { uiMaxPlayers = m_uiMaxPlayers; return m_bMaxPlayers; }
    bool                        IsHTTPEnabled                   ( void )        { return m_bHTTPEnabled; };

private:
    bool                        m_bMainConfig;
    bool                        m_bIP;
    bool                        m_bPort;
    bool                        m_bMaxPlayers;

    std::string                 m_strMainConfig;
    std::string                 m_strIP;
    unsigned short              m_usPort;
    unsigned int                m_uiMaxPlayers;
    bool                        m_bHTTPEnabled;
};

#endif
