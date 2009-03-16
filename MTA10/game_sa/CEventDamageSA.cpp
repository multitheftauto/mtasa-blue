/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEventDamageSA.cpp
*  PURPOSE:		Damage event
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CEntity * CEventDamageSA::GetInflictingEntity ( void )
{
    CEntity * pReturn = NULL;
    CEntitySAInterface * pInterface = m_pInterface->inflictor;
    if ( pInterface )
    {
        CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
	    switch ( pInterface->nType )
	    {
	        case ENTITY_TYPE_PED:
		        pReturn = pPools->GetPed((DWORD *)pInterface);
		        break;
	        case ENTITY_TYPE_VEHICLE:
		        pReturn = pPools->GetVehicle ( (DWORD *)pInterface );
                break;
	        case ENTITY_TYPE_OBJECT:
		        pReturn = pPools->GetObject((DWORD *)pInterface);
		        break;
	        default: break;
	    }
    }
    return pReturn;
}


bool CEventDamageSA::HasKilledPed ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CEventDamage_HasKilledPed;
    bool bReturn = false;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


float CEventDamageSA::GetDamageApplied ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CEventDamage_GetDamageApplied;
    float fReturn = 0.0f;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }
    return fReturn;
}


AssocGroupId CEventDamageSA::GetAnimGroup ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;    
    DWORD dwFunc = FUNC_CEventDamage_GetAnimGroup;
    AssocGroupId animGroup = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     animGroup, eax
    }
    return animGroup;
}


AnimationId CEventDamageSA::GetAnimId ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;    
    DWORD dwFunc = FUNC_CEventDamage_GetAnimId;
    AnimationId animID = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     animID, eax
    }
    return animID;
}


void CEventDamageSA::ComputeDeathAnim ( CPed * pPed, bool bUnk )
{
    DWORD dwThis = ( DWORD ) m_pInterface;    
    DWORD dwPed = ( DWORD ) pPed->GetInterface ();
    DWORD dwFunc = FUNC_CEventDamage_ComputeDeathAnim;
    _asm
    {
        mov     ecx, dwThis
        push    bUnk
        push    dwPed
        call    dwFunc
    }
}