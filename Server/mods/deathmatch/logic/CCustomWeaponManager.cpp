/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCustomWeaponManager.cpp
*  PURPOSE:     Source file for the manager of all CCustomWeapons.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#include "StdInc.h"

CCustomWeaponManager::CCustomWeaponManager (  )
{

}

CCustomWeaponManager::~CCustomWeaponManager ( )
{
    
}

void CCustomWeaponManager::DoPulse ( void )
{
    
}

void CCustomWeaponManager::RemoveFromList ( CCustomWeapon* pWeapon )
{
    m_List.remove ( pWeapon );
}
