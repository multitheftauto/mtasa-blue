/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto 
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/defs/CDefs.cpp
 *  PURPOSE:     Definitions base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CScriptDebugging* CDefs::m_pScriptDebugging = nullptr;

#ifdef MTA_CLIENT
void CDefs::Initialize(CScriptDebugging* pScriptDebugging)
{
    m_pScriptDebugging = pScriptDebugging;
}
#else
void CDefs::Initialize(CGame* pGame)
{
    m_pScriptDebugging = pGame->GetScriptDebugging();
}
#endif
