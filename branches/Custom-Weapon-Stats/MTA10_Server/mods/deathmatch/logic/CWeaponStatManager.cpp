/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponStatManager.h
*  PURPOSE:     source file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/
#include "StdInc.h"

CWeaponStatManager::CWeaponStatManager ( )
{
    CWeaponStat* pWeaponStat = NULL;
    // Make our list nicer by starting at the non-skill weapons so we don't have duplicates of STD weapon skill
    for ( int i = NUM_WeaponInfosOtherSkill + 1; i < NUM_WeaponInfosStdSkill; i++)
    {
        // Storage for Original weapon data ( for resetting )
        pWeaponStat = new CWeaponStat ( (eWeaponType) (WEAPONTYPE_PISTOL + i), WEAPONSKILL_STD );
        m_OriginalWeaponData.push_back ( pWeaponStat );

        // Storage for new weapon data ( for script use )
        pWeaponStat = new CWeaponStat ( (eWeaponType) (WEAPONTYPE_PISTOL + i), WEAPONSKILL_STD );
        m_WeaponData.push_back ( pWeaponStat );
    }

    for ( int skill = 0; skill < 3 ; skill++ )
    {
        for ( int i = 0; i < NUM_WeaponInfosOtherSkill; i++ )
        {
            // Storage for Original weapon data ( for resetting )
            pWeaponStat = new CWeaponStat ( (eWeaponType) (WEAPONTYPE_PISTOL + i), (eWeaponSkill)skill );
            m_OriginalWeaponData.push_back ( pWeaponStat );

            // Storage for new weapon data ( for script use )
            pWeaponStat = new CWeaponStat ( (eWeaponType) (WEAPONTYPE_PISTOL + i), (eWeaponSkill)skill );
            m_WeaponData.push_back ( pWeaponStat );
        }
    }
}

CWeaponStatManager::~CWeaponStatManager ( )
{
    for ( std::list < CWeaponStat* >::iterator iter = m_OriginalWeaponData.begin (); iter != m_OriginalWeaponData.end ();iter++ )
    {
        if ( (*iter ) )
            delete (*iter);
    }

    for ( std::list < CWeaponStat* >::iterator iter = m_WeaponData.begin (); iter != m_WeaponData.end ();iter++ )
    {
        if ( (*iter ) )
            delete (*iter);
    }
}


CWeaponStat* CWeaponStatManager::GetWeaponStats ( eWeaponType type, eWeaponSkill skill )
{
    for ( std::list < CWeaponStat* >::iterator iter = m_OriginalWeaponData.begin (); iter != m_OriginalWeaponData.end ();iter++ )
    {
        if ( (*iter)->GetWeaponSkillLevel() == skill && (*iter)->GetWeaponType () == type )
        {
            return (*iter);
        }
    }
    return NULL;
}
CWeaponStat* CWeaponStatManager::GetOriginalWeaponStats ( eWeaponType type, eWeaponSkill skill )
{
    for ( std::list < CWeaponStat* >::iterator iter = m_OriginalWeaponData.begin (); iter != m_OriginalWeaponData.end ();iter++ )
    {
        if ( (*iter)->GetWeaponSkillLevel() == skill && (*iter)->GetWeaponType () == type )
        {
            return (*iter);
        }
    }
    return NULL;
}