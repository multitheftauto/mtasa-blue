/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CPlayer;

//
// Bridge between the game and sim system
//
class CSimControl
{
public:
    static void Startup();
    static void Shutdown();
    static void DoPulse();
    static void EnableSimSystem(bool bEnable, bool bApplyNow = true);
    static bool IsSimSystemEnabled();
    static void AddSimPlayer(CPlayer* pPlayer);
    static void RemoveSimPlayer(CPlayer* pPlayer);
    static void UpdateSimPlayer(CPlayer* pPlayer);
};
