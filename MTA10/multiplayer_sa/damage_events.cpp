/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/damage_events.cpp
*  PURPOSE:     damage events modification
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define HOOKPOS_CEventDamage_AffectsPed                 0x4B35A0

#define HOOKPOS_CWorld_SetWorldOnFire                   0x56b9a5
DWORD RETURN_CWorld_SetWorldOnFire =                    0x56b9af;  

#define HOOKPOS_CTaskSimplePlayerOnFire_ProcessPed      0x633687
DWORD RETURN_CTaskSimplePlayerOnFire_ProcessPed =       0x633690;

#define HOOKPOS_CWeapon_FireAreaEffect                  0x73ebfe
DWORD RETURN_CWeapon_FireAreaEffect =                   0x73ec06; 

void HOOK_CEventDamage_AffectsPed ();
void HOOK_CWorld_SetWorldOnFire ();
void HOOK_CTaskSimplePlayerOnFire_ProcessPed ();
void HOOK_CWeapon_FireAreaEffect ();

DamageHandler* m_pDamageHandler = NULL;
FireDamageHandler* m_pFireDamageHandler = NULL;

void damage_events_init ( void )
{
    HookInstall ( HOOKPOS_CEventDamage_AffectsPed, (DWORD)HOOK_CEventDamage_AffectsPed, 6 );
    HookInstall ( HOOKPOS_CWorld_SetWorldOnFire, (DWORD)HOOK_CWorld_SetWorldOnFire, 10 );
    HookInstall ( HOOKPOS_CTaskSimplePlayerOnFire_ProcessPed, (DWORD)HOOK_CTaskSimplePlayerOnFire_ProcessPed, 9 );
    HookInstall ( HOOKPOS_CWeapon_FireAreaEffect, (DWORD)HOOK_CWeapon_FireAreaEffect, 8 );
}

bool ProcessDamageEvent ( CEventDamageSAInterface * event, CPedSAInterface * affectsPed )
{
    if ( m_pDamageHandler && event )
    {
        CPoolsSA * pPools = (CPoolsSA*)pGameInterface->GetPools();
        CPed * pPed = pPools->GetPed ( (DWORD *)affectsPed );
        CEntity * pInflictor = NULL;

        if ( pPed )
        {
            // This creates a CEventDamageSA for us
            CEventDamage * pEvent = pGameInterface->GetEventList ()->GetEventDamage ( event );
            // Call the event
            bool bReturn = m_pDamageHandler ( pPed, pEvent );
            // Destroy the CEventDamageSA (so we dont get a leak)
            pEvent->Destroy ( false );
            // Finally, return
            return bReturn;
        }
    }
    return true;
}


CPedSAInterface * affectsPed = 0;
CEventDamageSAInterface * event = 0;
void _declspec(naked) HOOK_CEventDamage_AffectsPed()
{
    /*
    004B35A0   83EC 0C          SUB ESP,0C
    004B35A3   56               PUSH ESI
    004B35A4   8BF1             MOV ESI,ECX
    */

    _asm
    {
        push    esi  
    
        mov     esi, [esp+8]        
        mov     affectsPed, esi // store the ped
        mov     event, ecx // store the event pointer

        pop     esi

        pushad
    }

    if ( ProcessDamageEvent ( event, affectsPed ) )
    {
        // they want the damage to happen!
        _asm
        {
            popad
            
            sub     esp, 0xC        // replacement code
            push    esi
            mov     esi, ecx

            mov     ecx, HOOKPOS_CEventDamage_AffectsPed
            add     ecx, 6
            jmp     ecx
        }
    }
    else
    {
        // they want the player to escape unscathed

        _asm
        {
            popad
            xor     eax, eax
            retn    4 // return from the function
        }
    }
}


CEntitySAInterface * pFireCreatorInterface = NULL;
CFireSAInterface * pFireInterface = NULL;
DWORD dwCFireManager_StartFire = 0x539f00;
void _declspec(naked) HOOK_CWorld_SetWorldOnFire ()
{
    _asm
    {
        mov     eax, [esp+68]
        mov     pFireCreatorInterface, eax

        mov     ecx, 0B71F80h 
        call    dwCFireManager_StartFire
        mov     pFireInterface, eax
        pushad
    }
    // Set our fire creator as GTA is too lazy to do it for us!
    pFireInterface->entityCreator = pFireCreatorInterface;

    _asm
    {
        popad 
        jmp     RETURN_CWorld_SetWorldOnFire
    }
}

void CTaskSimplePlayerOnFire_ProcessPed ()
{
    CPed * pPed = pGameInterface->GetPools ()->GetPed ( (DWORD *)affectsPed );
    if ( pPed )
    {
        if ( m_pFireDamageHandler ) m_pFireDamageHandler ( pPed );
    }
}

DWORD dwCTaskComplexOnFire_ComputeFireDamage = 0x6333d0;
void _declspec(naked) HOOK_CTaskSimplePlayerOnFire_ProcessPed ()
{
    _asm
    {
        mov     eax, [esp]
        mov     affectsPed, eax

        call    dwCTaskComplexOnFire_ComputeFireDamage
        pushad
    }

    CTaskSimplePlayerOnFire_ProcessPed ();

    _asm
    {
        popad
        mov     al,byte ptr [esp+1Ch]
        jmp     RETURN_CTaskSimplePlayerOnFire_ProcessPed
    }
}


float fAreaEffectX, fAreaEffectY, fAreaEffectZ;
CPedSAInterface * pFireAreaEffectPedInterface = NULL;
void CWeapon_FireAreaEffect ()
{
    CPed * pPed = pGameInterface->GetPools ()->GetPed ( (DWORD *)pFireAreaEffectPedInterface );
    if ( pPed )
    {
    }
}

void _declspec(naked) HOOK_CWeapon_FireAreaEffect ()
{
    // This hook removes a 'CCreepingFire::TryToStartFireAtCoors' call
    // Possibly replace it with CFireManager::StartFire, or just set its creator
    _asm
    {
        mov     eax, [esp]
        mov     fAreaEffectX, eax
        mov     eax, [esp+4]
        mov     fAreaEffectY, eax
        mov     eax, [esp+8]
        mov     fAreaEffectZ, eax
        mov     eax, [esp+112]
        mov     pFireAreaEffectPedInterface, eax
        
        pushad
    }
    CWeapon_FireAreaEffect ();
    _asm
    {
        popad
        add     esp,1Ch
        mov     al,1
        jmp     RETURN_CWeapon_FireAreaEffect
    }
}