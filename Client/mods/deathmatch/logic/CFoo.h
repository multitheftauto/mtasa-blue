/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CFoo.h
 *  PURPOSE:     Debugging class (not used in Release mode)
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

/* This class is purely for debugging things. This will avoid usage of CClientGame. */

#pragma once

class CFoo
{
public:
    #ifdef MTA_DEBUG
    void Init(class CClientGame* pClientGame) { g_pClientGame = pClientGame; };

    void DoPulse();
    void Test(const char* szString);

    class CClientGame* g_pClientGame;
    #else
    void Init(CClientGame* pClientGame){};
    void DoPulse(){};
    void Test(const char* szString){};
    #endif
};
