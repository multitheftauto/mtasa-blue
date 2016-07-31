/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWeaponManager.cpp
*  PURPOSE:     Weapon manager class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include "StdInc.h"
using namespace std;

CClientWeaponManager::CClientWeaponManager ( CClientManager * pManager )
{
    m_pManager = pManager;
    m_bDeleteAll = false;
}


CClientWeaponManager::~CClientWeaponManager ( void )
{
    m_bDeleteAll = true;
    list < CClientWeapon * > ::iterator iter = m_Weapons.begin ();
    for ( ; iter != m_Weapons.end () ; iter++ )
    {
        delete *iter;
    }
    m_bDeleteAll = false;
}


void CClientWeaponManager::RemoveFromList ( CClientWeapon * pWeapon )
{
    if ( !m_bDeleteAll )
    {
        m_Weapons.remove ( pWeapon );
    }
}

void CClientWeaponManager::DoPulse ( void )
{    
    list < CClientWeapon * > ::iterator iter = m_Weapons.begin ();
    for ( ; iter != m_Weapons.end () ; iter++ )
    {
        (*iter)->DoPulse ();
    }
}