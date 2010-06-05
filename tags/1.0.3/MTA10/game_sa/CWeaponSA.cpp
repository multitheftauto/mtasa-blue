/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponSA.cpp
*  PURPOSE:     Weapon class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CWeaponSA::CWeaponSA( CWeaponSAInterface * weaponInterface, CPed * ped, eWeaponSlot weaponSlot )
{
	DEBUG_TRACE("CWeaponSA::CWeaponSA( CWeaponSAInterface * weaponInterface, CPed * ped, DWORD dwSlot )");
	this->owner = ped;
	this->m_weaponSlot = weaponSlot;
	internalInterface = weaponInterface;	
}

eWeaponType	CWeaponSA::GetType(  )
{
	DEBUG_TRACE("eWeaponType CWeaponSA::GetType(  )");
	return this->internalInterface->m_eWeaponType;
};

VOID CWeaponSA::SetType( eWeaponType type )
{
	DEBUG_TRACE("VOID CWeaponSA::SetType( eWeaponType type )");
	this->internalInterface->m_eWeaponType = type;
}

eWeaponState CWeaponSA::GetState(  )
{
	DEBUG_TRACE("eWeaponState CWeaponSA::GetState(  )");
	return this->internalInterface->m_eState;
}

void CWeaponSA::SetState ( eWeaponState state )
{
	DEBUG_TRACE("void CWeaponSA::SetState ( eWeaponState state )");
	this->internalInterface->m_eState = state;
}

DWORD CWeaponSA::GetAmmoInClip(  )
{
	DEBUG_TRACE("DWORD CWeaponSA::GetAmmoInClip(  )");
	return this->internalInterface->m_nAmmoInClip;
}

VOID CWeaponSA::SetAmmoInClip( DWORD dwAmmoInClip )
{
	DEBUG_TRACE("VOID CWeaponSA::SetAmmoInClip( DWORD dwAmmoInClip )");
	this->internalInterface->m_nAmmoInClip = dwAmmoInClip;
}

DWORD CWeaponSA::GetAmmoTotal(  )
{
	DEBUG_TRACE("DWORD CWeaponSA::GetAmmoTotal(  )");
	return this->internalInterface->m_nAmmoTotal;
}

VOID CWeaponSA::SetAmmoTotal( DWORD dwAmmoTotal )
{
	DEBUG_TRACE("VOID CWeaponSA::SetAmmoTotal( DWORD dwAmmoTotal )");
	this->internalInterface->m_nAmmoTotal = dwAmmoTotal;
}

CPed * CWeaponSA::GetPed()
{
	DEBUG_TRACE("CPed * CWeaponSA::GetPed()");
	return (CPed *)owner;
}

eWeaponSlot	CWeaponSA::GetSlot()
{
	DEBUG_TRACE("eWeaponSlot CWeaponSA::GetSlot()");
	return m_weaponSlot;
}

VOID CWeaponSA::SetAsCurrentWeapon()
{
	DEBUG_TRACE("VOID CWeaponSA::SetAsCurrentWeapon()");
	owner->SetCurrentWeaponSlot( m_weaponSlot );
}

void CWeaponSA::Remove ()
{
    DEBUG_TRACE("void CWeaponSA::Remove ()");
    DWORD dwFunc = FUNC_Shutdown;
    DWORD dwThis = (DWORD)this->internalInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    // If the removed weapon was the currently active weapon, switch to empty-handed
    if ( owner->GetCurrentWeaponSlot () == m_weaponSlot )
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo ( this->internalInterface->m_eWeaponType );
        if ( pInfo )
        {
            int iModel = pInfo->GetModel();
            owner->RemoveWeaponModel ( iModel );
        }
        owner->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );
    }
}