/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CAntiCheatModule.cpp
*  PURPOSE:     Anti-cheat module class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CANTICHEATMODULE_H
#define __CANTICHEATMODULE_H

class CAntiCheatModule
{
public:
                        CAntiCheatModule        ( class CAntiCheat& AntiCheat );
    virtual             ~CAntiCheatModule       ( void );

    // Implement this function in the anticheat module class. This should 
    // return true if the checks were successful, and false if the player
    // is a cheater.
    virtual bool        PerformChecks           ( void ) = 0;

private:
    class CAntiCheat&   m_AntiCheat;
};

#endif
