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

    bool                        GetMainConfig                   ( const char*& pszMainConfig );
    bool                        IsHTTPEnabled                   ( void )        { return m_bHTTPEnabled; };

private:
    bool                        m_bMainConfig;
    char                        m_szMainConfig [256];
    bool                        m_bHTTPEnabled;
};

#endif
