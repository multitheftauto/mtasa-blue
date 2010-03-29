/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConsoleCommand.h
*  PURPOSE:     Console command class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONSOLECOMMAND_H
#define __CCONSOLECOMMAND_H

#include "CClient.h"

typedef bool (FCommandHandler) ( class CConsole*, const char*, CClient*, CClient* );

class CConsoleCommand
{
public:
                                    CConsoleCommand             ( FCommandHandler* pHandler, const char* szCommand, bool bRestricted );
    inline                          ~CConsoleCommand            ( void )            { delete [] m_szCommand; };

    bool                            operator ()                 ( class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient );
    inline FCommandHandler*         GetHandler                  ( void )            { return m_pHandler; };
    inline const char*              GetCommand                  ( void )            { return m_szCommand; };
    inline bool                     IsRestricted                ( void )            { return m_bRestricted; };
private:
    FCommandHandler*                m_pHandler;
    char*                           m_szCommand;
    bool                            m_bRestricted;

};

#endif
