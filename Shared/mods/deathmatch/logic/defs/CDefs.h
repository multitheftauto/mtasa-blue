/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/defs/CDefs.h
 *  PURPOSE:     Definitions base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#ifdef MTA_CLIENT
    #include "../CScriptDebugging.h"
#else
    //#include "CScriptDebugging.h"
#endif
class CDefs
{
public:
    #ifdef MTA_CLIENT
        static void Initialize(CScriptDebugging* pScriptDebugging);
    #else
        static void Initialize(class CGame* pGame);
    #endif

protected:
    static CScriptDebugging*          m_pScriptDebugging;
};
