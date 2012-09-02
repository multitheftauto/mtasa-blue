/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConsoleClient.cpp
*  PURPOSE:     Console connected client class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CConsoleClient::CConsoleClient ( CConsole* pConsole ) : CElement ( pConsole->GetMapManager ()->GetRootElement (), NULL )
{
    m_iType = CElement::CONSOLE;
    SetTypeName ( "console" );
    m_strNick = "Console";
    m_pConsole = pConsole;
    m_pAccount->SetName ( "Console" );
    m_pAccount->Register ( "" );
}

