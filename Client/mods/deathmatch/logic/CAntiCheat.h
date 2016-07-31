/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CAntiCheat.h
*  PURPOSE:     Anti-cheat handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CANTICHEAT_H
#define __CANTICHEAT_H

#include "CAntiCheatModule.h"
#include <list>

class CAntiCheat
{
public:
                                CAntiCheat              ( void );
                                ~CAntiCheat             ( void );

    void                        AddModule               ( CAntiCheatModule& Module );
    void                        RemoveModule            ( CAntiCheatModule& Module );
    void                        ClearModules            ( void );

    bool                        PerformChecks           ( void );

private:
    std::list < CAntiCheatModule* >             m_Entries;
    std::list < CAntiCheatModule* > ::iterator  m_LastEntry;
};

#endif
