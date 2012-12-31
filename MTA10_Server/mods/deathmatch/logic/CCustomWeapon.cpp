/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCustomWeapon.cpp
*  PURPOSE:     Source file for custom weapons.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#include "StdInc.h"

CCustomWeapon::CCustomWeapon ( CElement* pParent, CXMLNode* pNode, CObjectManager * pObjectManager, CCustomWeaponManager * pWeaponManager, eWeaponType weaponType ) : CObject ( pParent, pNode, pObjectManager, false )
{
    // Init
    m_iType = CElement::WEAPON;
    SetTypeName ( "weapon" );

    m_Type = weaponType;
    m_State = WEAPONSTATE_READY;

    // Setup weapon config.
    m_weaponConfig.bDisableWeaponModel = false;
    m_weaponConfig.bShootIfTargetBlocked = false;
    m_weaponConfig.bShootIfTargetOutOfRange = false;
    m_weaponConfig.flags.bShootThroughStuff = false;
    m_weaponConfig.flags.bIgnoreSomeObjectsForCamera = false;
    m_weaponConfig.flags.bSeeThroughStuff = false;
    m_weaponConfig.flags.bCheckDummies = true;
    m_weaponConfig.flags.bCheckObjects = true;
    m_weaponConfig.flags.bCheckPeds = true;
    m_weaponConfig.flags.bCheckVehicles = true;
    m_weaponConfig.flags.bCheckBuildings = true;
    m_weaponConfig.flags.bCheckCarTires = true;

    m_pWeaponStat = new CWeaponStat ( m_Type, WEAPONSKILL_STD );

    m_itargetWheel = MAX_WHEELS + 1;
    m_nAmmoInClip = m_pWeaponStat->GetMaximumClipAmmo();
    m_ucCounter = 0;
    m_nAmmoTotal = 9999;

    m_pOwner = NULL;

    ResetWeaponTarget ( );

    m_pWeaponManager = pWeaponManager;

    // Add us to the manager's list
    m_pWeaponManager->AddToList ( this );
}

CCustomWeapon::~CCustomWeapon ( )
{
    m_pWeaponManager->RemoveFromList ( this );
}

void CCustomWeapon::SetWeaponTarget ( CElement * pTarget, int subTarget )
{
    m_pTarget = pTarget; 
    m_targetType = TARGET_TYPE_ENTITY; 
    if ( IS_PED ( pTarget ) )
    {
        if ( subTarget == 255 )
            m_targetBone = eBone::BONE_PELVIS;
        else
            m_targetBone = (eBone)subTarget; 
    }
    if ( IS_PLAYER ( pTarget ) )
    {
        m_itargetWheel = subTarget; 
    }
}

void CCustomWeapon::SetWeaponTarget ( CVector vecTarget )
{
    m_vecTarget = vecTarget; 
    m_targetType = TARGET_TYPE_VECTOR;
}

void CCustomWeapon::ResetWeaponTarget ( void )
{
    m_pTarget = NULL;
    m_vecTarget = CVector ( 0, 0, 0 );
    m_targetType = TARGET_TYPE_FIXED;
    m_targetBone = BONE_PELVIS;
}

bool CCustomWeapon::SetFlags ( eWeaponFlags flag, bool bData )
{
    switch ( flag )
    {
        case WEAPONFLAGS_SHOOT_IF_OUT_OF_RANGE:
            m_weaponConfig.bShootIfTargetOutOfRange = bData;
        return true;
        case WEAPONFLAGS_SHOOT_IF_TARGET_BOCKED:
            m_weaponConfig.bShootIfTargetBlocked = bData;
        return true;
        case WEAPONFLAGS_DISABLE_MODEL:
            m_weaponConfig.bDisableWeaponModel = bData;
            /*if ( m_weaponConfig.bDisableWeaponModel )
            {
                this->SetVisible ( false );
            }
            else
            {
                //this->SetVisible( true );
            }*/
        return true;
        case WEAPONFLAGS_INSTANT_RELOAD:
            m_weaponConfig.bInstantReload = bData;
        return true;
    }
    return false;
}

bool CCustomWeapon::SetFlags ( const SLineOfSightFlags flags )
{
    m_weaponConfig.flags = flags;
    return true;
}

bool CCustomWeapon::GetFlags ( eWeaponFlags flag, bool &bData )
{

    switch ( flag )
    {
        case WEAPONFLAGS_SHOOT_IF_OUT_OF_RANGE:
            bData = m_weaponConfig.bShootIfTargetOutOfRange;
        return true;
        case WEAPONFLAGS_SHOOT_IF_TARGET_BOCKED:
            bData = m_weaponConfig.bShootIfTargetBlocked;
        return true;
        case WEAPONFLAGS_DISABLE_MODEL:
            bData = m_weaponConfig.bDisableWeaponModel;
        return true;
        case WEAPONFLAGS_INSTANT_RELOAD:
            bData = m_weaponConfig.bInstantReload;
        return true;
    }
    return false;
}

bool CCustomWeapon::GetFlags ( SLineOfSightFlags &flags )
{
    flags = m_weaponConfig.flags;
    return true;
}

int CCustomWeapon::GetWeaponFireTime ( CWeaponStat * pWeaponStat )
{
    float fWeaponFireTime = (pWeaponStat->GetWeaponAnimLoopStop() - pWeaponStat->GetWeaponAnimLoopStart()) * 1000.0f;
    return (int)fWeaponFireTime;
}

void CCustomWeapon::SetWeaponFireTime ( int iFireTime )
{
    m_iWeaponFireRate = iFireTime;
}

int CCustomWeapon::GetWeaponFireTime ( void )
{
    return m_iWeaponFireRate;
}

void CCustomWeapon::ResetWeaponFireTime ( void )
{
    m_iWeaponFireRate = GetWeaponFireTime ( m_pWeaponStat );
}
