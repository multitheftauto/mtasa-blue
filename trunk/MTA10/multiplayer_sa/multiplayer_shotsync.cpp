/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/multiplayer_shotsync.cpp
*  PURPOSE:     Multiplayer module shooting sync methods
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// These includes have to be fixed!
#include "../game_sa/CPedSA.h"
#include "../game_sa/CEventDamageSA.h"
#include "../game_sa/CColPointSA.h"

extern CMultiplayerSA* pMultiplayer;

using std::list;

list<CShotSyncData*>        ShotSyncData;
CShotSyncData               LocalShotSyncData;

float * fDirectionX;
float * fDirectionY;
CPedIKSAInterface * pPedIKInterface;
CPedSAInterface * pPedInterfaceTemp;
CPed * pPedTemp;
CPed * pLocalPedTemp;
CVector * vecOrigin;
CEntitySAInterface * pProjectileOwner = 0;
eWeaponType projectileWeaponType =WEAPONTYPE_UNARMED;
CVector * projectileOrigin = NULL;
float projectileForce = 0.0f;
CVector * projectileTarget = NULL;
CEntitySAInterface * projectileTargetEntityInterface = 0;
CEntity* projectileTargetEntity = 0;
class CProjectileSAInterface * pProjectile = 0;
DWORD dwProjectileInfoIndex;
bool bWeaponFire = false;

CColPointSAInterface ** ppInstantHitColPoint;
CEntitySAInterface ** ppInstantHitEntity;
CVector * pInstantHitStart = NULL, * pInstantHitEnd = NULL;

CVector vecLastOrigin;
CVector vecLastLocalPlayerBulletStart;
CVector vecLastLocalPlayerBulletEnd;

char cTempGunDirection;

DWORD vecTargetPosition;
DWORD vecAltPos;
CPedSAInterface * pShootingPed;
EDamageReasonType g_GenerateDamageEventReason = EDamageReason::OTHER;

CPedSAInterface* pBulletImpactInitiator;
CEntitySAInterface* pBulletImpactVictim;
CVector* pBulletImpactStartPosition;
CVector* pBulletImpactEndPosition;
CVector vecSavedBulletImpactEndPosition;

extern PreWeaponFireHandler* m_pPreWeaponFireHandler;
extern PostWeaponFireHandler* m_pPostWeaponFireHandler;
extern BulletImpactHandler* m_pBulletImpactHandler;
extern BulletFireHandler* m_pBulletFireHandler;
extern DamageHandler* m_pDamageHandler;
extern DeathHandler* m_pDeathHandler;
extern FireHandler* m_pFireHandler;
extern ProjectileHandler* m_pProjectileHandler;
extern ProjectileStopHandler* m_pProjectileStopHandler;

char szDebug[255] = {'\0'};

DWORD RETURN_CProjectile__AddProjectile = 0x401C3D;
DWORD RETURN_CProjectile__CProjectile = 0x4037B3;
CPools * m_pools = 0;

#define VAR_CWorld_IncludeCarTyres 0xb7cd70 // Used for CWorld_ProcessLineOfSight

void InitFireInstantHit_MidHooks ( void );

VOID InitShotsyncHooks()
{
    HookInstall ( HOOKPOS_CWeapon__Fire, (DWORD)HOOK_CWeapon__Fire, 6 );
    HookInstall ( HOOKPOS_CWeapon__PostFire, (DWORD)HOOK_CWeapon__PostFire, 6 );
    HookInstall ( HOOKPOS_CWeapon__PostFire2, (DWORD)HOOK_CWeapon__PostFire2, 6 );
    HookInstall ( HOOKPOS_CPedIK__PointGunInDirection, (DWORD)HOOK_CPedIK__PointGunInDirection, 7 );
    HookInstall ( HOOKPOS_CTaskSimpleGangDriveBy__PlayerTarget, (DWORD)HOOK_CTaskSimpleGangDriveBy__PlayerTarget, 6 );
    HookInstall ( HOOKPOS_CWeapon__Fire_Sniper, (DWORD)HOOK_CWeapon__Fire_Sniper, 6 );
    HookInstall ( HOOKPOS_CEventDamage__AffectsPed, (DWORD)HOOK_CEventDamage__AffectsPed, 6 );
    HookInstall ( HOOKPOS_CEventVehicleExplosion__AffectsPed, (DWORD)HOOK_CEventVehicleExplosion__AffectsPed, 5 );
    HookInstall ( HOOKPOS_CFireManager__StartFire, (DWORD)HOOK_CFireManager__StartFire, 6 );
    HookInstall ( HOOKPOS_CFireManager__StartFire_, (DWORD)HOOK_CFireManager__StartFire_, 6 );
    HookInstall ( HOOKPOS_CProjectileInfo__AddProjectile, (DWORD)HOOK_CProjectileInfo__AddProjectile, 7 );
    HookInstall ( HOOKPOS_CProjectile__CProjectile, (DWORD)HOOK_CProjectile__CProjectile, 7 );
    HookInstall ( HOOKPOS_IKChainManager_PointArm, (DWORD)HOOK_IKChainManager_PointArm, 7 );
    HookInstall ( HOOKPOS_IKChainManager_LookAt, (DWORD)HOOK_IKChainManager_LookAt, 7 );
    HookInstall ( HOOKPOS_IKChainManager_SkipAim, (DWORD)HOOK_SkipAim, 6 );
    HookInstall ( HOOKPOS_CTaskSimpleUsegun_ProcessPed, (DWORD)HOOK_CTaskSimpleUsegun_ProcessPed, 8 );
    HookInstall ( HOOKPOS_CWeapon_FireInstantHit, (DWORD)HOOK_CWeapon_FireInstantHit, 9 );
    HookInstall ( HOOKPOS_CWeapon_FireInstantHit_CameraMode, (DWORD)HOOK_CWeapon_FireInstantHit_CameraMode, 6 );
    HookInstall ( HOOKPOS_CWeapon_FireInstantHit_IsPlayer, (DWORD)HOOK_CWeapon_FireInstantHit_IsPlayer, 7 );
    HookInstall ( HOOKPOS_CWeapon_DoBulletImpact, (DWORD)HOOK_CWeapon_DoBulletImpact, 7 );

    InitFireInstantHit_MidHooks ();
    /*  
    MemPut < BYTE > ( 0x73FDEC, 0x90 );
    MemPut < BYTE > ( 0x73FDED, 0xE9 );
    */

    m_pools = pGameInterface->GetPools();
}

CShotSyncData * GetLocalPedShotSyncData ( )
{
    return &LocalShotSyncData;
}

bool IsLocalPlayer ( CPed * pPed )
{
    CPed * pLocalPed = m_pools->GetPedFromRef ( (DWORD)1 );
    return ( pPed == pLocalPed );
}

bool IsLocalPlayer ( CPedSAInterface * pPedInterface )
{
    CPed * pPed = m_pools->GetPed ( (DWORD *)pPedInterface );
    return IsLocalPlayer ( pPed );
}

VOID WriteGunDirectionDataForPed ( CPedSAInterface * pPedInterface, float * fGunDirectionX, float * fGunDirectionY, char * cGunDirection )
{        
    if ( !IsLocalPlayer ( pPedInterface ) )
    {
        CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( pPedInterface );
        if ( data )
        {
            if ( data->ProcessPlayerWeapon () )
            {
                if ( fGunDirectionX && fGunDirectionY )
                {                
                    *fGunDirectionX = data->m_shotSyncData.m_fArmDirectionX;
                    *fGunDirectionY = data->m_shotSyncData.m_fArmDirectionY;
                }

                if ( cGunDirection )
                {
                    *cGunDirection = data->m_shotSyncData.m_cInVehicleAimDirection;
                }
            }
        }
    }
    else
    {
        // store it
        if ( fGunDirectionX && fGunDirectionY )
        {
            // Make sure our pitch is updated (fixes first-person weapons not moving)
            *fGunDirectionY = pGameInterface->GetCamera ()->Find3rdPersonQuickAimPitch ();

            LocalShotSyncData.m_fArmDirectionX = *fGunDirectionX;
            LocalShotSyncData.m_fArmDirectionY = *fGunDirectionY;
        }

        if ( cGunDirection )
        {
            LocalShotSyncData.m_cInVehicleAimDirection = *cGunDirection;
        }
    }
}

// Return false to stop bullet impact to being calculated here
//   (False also assumes lag compensation movement has not been performed)
bool WriteTargetDataForPed ( CPedSAInterface * pPed, DWORD vecTargetPos, CVector * origin )
{
    bool bDoBulletTraceHere = true;
    vecLastOrigin = *origin;

    // vecTargetPosition is a pointer to a vecTargetPosition*
    CPed * pTargetingPed = m_pools->GetPed ( (DWORD *)pPed );
    CPlayerPed* pTargetingPlayerPed = dynamic_cast < CPlayerPed* > ( pTargetingPed );
    if ( !pTargetingPlayerPed ) return true;

    if ( m_pPreWeaponFireHandler && pTargetingPed )
    {
        bDoBulletTraceHere = m_pPreWeaponFireHandler ( pTargetingPlayerPed, true );
    }

    if ( !IsLocalPlayer( pTargetingPed ) )
    {
        CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( pTargetingPlayerPed );
        if ( data )
        {
            if ( data->ProcessPlayerWeapon () )
            {
                DWORD dwPointerToVector = (DWORD)&data->m_shotSyncData.m_vecShotTarget;
                MemPutFast < DWORD > ( vecTargetPos, dwPointerToVector );
                if ( data->m_shotSyncData.m_bUseOrigin )
                    *origin = data->m_shotSyncData.m_vecShotOrigin;
            }

            // If current weapon has bullet sync enabled, don't allow firing
            return bDoBulletTraceHere;
        }
    }
    else
    {
        // local ped firing
        LocalShotSyncData.m_vecShotOrigin = *origin;
    }

    // Allow shot to be fired here
    return true;
}

void Event_PostFire ( void )
{
    CPed * pTargetingPed = m_pools->GetPed ( (DWORD *)pShootingPed );

    if ( m_pPostWeaponFireHandler )
    {
        m_pPostWeaponFireHandler ();
    }
    bWeaponFire = false;
}

static void Event_BulletImpact ( void )
{
    if ( m_pBulletImpactHandler )
    {
        CPed * pInitiator = m_pools->GetPed ( (DWORD *)pBulletImpactInitiator );
        if ( pInitiator )
        {
            CEntity* pVictim = m_pools->GetEntity ( (DWORD *)pBulletImpactVictim );

            if ( IsLocalPlayer ( pInitiator ) )
            {
                // Correct weapon range if local player
                float fRange = pInitiator->GetCurrentWeaponRange ();
                CVector vecDir = *pBulletImpactEndPosition - *pBulletImpactStartPosition;
                float fLength = vecDir.Length ();
                if ( fRange < fLength )
                {
                    vecDir.Normalize ();
                    *pBulletImpactEndPosition = *pBulletImpactStartPosition + vecDir * fRange;
                }
                // These two will be the same when shooting without aiming, so correct them
                if ( vecLastLocalPlayerBulletStart == vecLastLocalPlayerBulletEnd )
                {
                    vecLastLocalPlayerBulletStart = *pBulletImpactStartPosition;
                    vecLastLocalPlayerBulletEnd = *pBulletImpactEndPosition;
                }
                m_pBulletImpactHandler ( pInitiator, pVictim, &vecLastLocalPlayerBulletStart, &vecLastLocalPlayerBulletEnd );
            }
            else
            {
                // Correct start postion if remote player
                CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( pBulletImpactInitiator );
                if ( data )
                {
                    if ( data->ProcessPlayerWeapon () )
                    {
                        if ( data->m_shotSyncData.m_bRemoteBulletSyncVectorsValid )
                        {
                            *pBulletImpactStartPosition = data->m_shotSyncData.m_vecRemoteBulletSyncStart;
                            data->m_shotSyncData.m_bRemoteBulletSyncVectorsValid = false;

                            // Ensure weapon fire event gets the correct vectors
                            data->m_shotSyncData.m_vecShotOrigin = data->m_shotSyncData.m_vecRemoteBulletSyncStart;
                            data->m_shotSyncData.m_vecShotTarget = data->m_shotSyncData.m_vecRemoteBulletSyncEnd;
                        }
                        else
                            *pBulletImpactStartPosition = data->m_shotSyncData.m_vecShotOrigin;
                    }
                }
                m_pBulletImpactHandler ( pInitiator, pVictim, pBulletImpactStartPosition, pBulletImpactEndPosition );
            }
        }
        vecSavedBulletImpactEndPosition = *pBulletImpactEndPosition;    // Saved for vehicle damage event parameters
    }
}

CPedSAInterface* pAPed = NULL;
float fTempPosX = 0, fTempPosY = 0, fTempPosZ = 0;
CPed * pATargetingPed = NULL;
CVector* pTempVec;
bool* pSkipAim;
CRemoteDataStorageSA* pTempRemote;

VOID _declspec(naked) HOOK_CTaskSimpleUsegun_ProcessPed ()
{
    // We can use EAX
    _asm
    {
        // Store the ped pointer for our later hook (SkipAim)
        mov         eax, [esp+4]
        mov         pAPed, eax

        // Replace original code
        push        0FFFFFFFFh 
        mov         eax,dword ptr fs:[00000000h] 

        // Jump back to func. I think we can use EDX here
        mov         edx, HOOKPOS_CTaskSimpleUsegun_ProcessPed
        add         edx, 8
        jmp         edx
    }
}

VOID _declspec(naked) HOOK_SkipAim ()
{
    // We can use ECX
    // Return to 0x62AEED for normal aiming
    // Return to 0x62A565 for arms up. Should probably set [esi+0Eh] to 1 too.
    _asm
    {
        // Store the pointer to the skipaim argument
        mov         ecx, esi
        add         ecx, 0Eh
        mov         pSkipAim, ecx

        // Store all the registers
        pushad
    }

    // Grab the player for this interface
    pATargetingPed = m_pools->GetPed ( (DWORD *)pAPed );
    if ( pATargetingPed )
    {
        // If this is the local player
        if ( IsLocalPlayer ( pATargetingPed ) )
        {
            // Store wheter he's aiming up or not
            pMultiplayer->m_bAkimboTargetUp = *pSkipAim;
        }
        else
        {
            // Grab his remote storage
            pTempRemote = CRemoteDataSA::GetRemoteDataStorage ( pAPed );    // TODO: Can be optimized further by using the PlayerPed class. Not sure how to convert CPed to CPlayerPed
            if ( pTempRemote )
            {
                if ( pTempRemote->ProcessPlayerWeapon () )
                {
                    // If this is remote player, use the data in his remote storage
                    *pSkipAim = pTempRemote->m_bAkimboTargetUp;
                }
            }
            else
                *pSkipAim = false;
        }
    }

    // Return to the correct place wheter we put our arms up or not
    if ( *pSkipAim )
    {
        _asm
        {
            // Restore all registers
            _asm popad;

            // Return to 0x62A565 (arms up)
            mov         ecx, 0x62A565
            jmp         ecx
        }
    }
    else
    {
        _asm
        {
            // Restore all registers
            _asm popad;

            // Return to 0x62AEED (normal aim)
            mov         ecx, 0x62AEED
            jmp         ecx
        }
    }
}


float* pTargetVector;

VOID _declspec(naked) HOOK_IKChainManager_PointArm ()
{
    // We can use edx
    _asm
    {
        // Grab the ped whose aiming the gun and the pointer to the vector we aim at
        mov         edx, [esp+24]
        mov         pTargetVector, edx
        mov         edx, [esp+12]
        mov         pAPed, edx

        // Store all the registers on the stack
        pushad
    }
    
    // Grab the player for this interface
    pATargetingPed = m_pools->GetPed ( (DWORD *)pAPed );
    if ( pATargetingPed )
    {
        // If this is the local player
        if ( IsLocalPlayer ( pATargetingPed ) )
        {
            // Store the position his aiming at
            pMultiplayer->m_vecAkimboTarget.fX = pTargetVector [0];
            pMultiplayer->m_vecAkimboTarget.fY = pTargetVector [1];
            pMultiplayer->m_vecAkimboTarget.fZ = pTargetVector [2];
        }
        else
        {
            // Grab his remote storage
            pTempRemote = CRemoteDataSA::GetRemoteDataStorage ( pAPed );    // TODO: Can be optimized further by using the PlayerPed class. Not sure how to convert CPed to CPlayerPed
            if ( pTempRemote )
            {
                if ( pTempRemote->ProcessPlayerWeapon () )
                {
                    // If this is remote player, use the data in his remote storage to point his aim position
                    pTargetVector [0] = pTempRemote->m_vecAkimboTarget.fX;
                    pTargetVector [1] = pTempRemote->m_vecAkimboTarget.fY;
                    pTargetVector [2] = pTempRemote->m_vecAkimboTarget.fZ;
                }
            }
        }
    }

    _asm
    {
        // Restore all the registers from the stack
        popad

        // Run the instructions we replaced when this hook was added
        push        0FFFFFFFFh 
        push        83F0D6h 

        // Continue execution where we intercepted
        mov     edx, HOOKPOS_IKChainManager_PointArm
        add     edx, 7
        jmp     edx
    }
}

VOID _declspec(naked) HOOK_IKChainManager_LookAt ()
{
    // We can use eax
    _asm
    {
        // Grab the player ped and the vector pointer from the stack
        mov         eax, [esp+24]
        mov         pTargetVector, eax
        mov         eax, [esp+8]
        mov         pAPed, eax

        // Store all the registers on the stack
        pushad
    }

    // Jax: this gets called on vehicle collision and pTargetVector is null
    if ( pTargetVector )
    {
        // Grab the player for this interface
        pATargetingPed = m_pools->GetPed ( (DWORD *)pAPed );
        if ( pATargetingPed )
        {
            // If this is the local player
            if ( IsLocalPlayer ( pATargetingPed ) )
            {
                // Store the position his aiming at
                pMultiplayer->m_vecAkimboTarget.fX = pTargetVector [0];
                pMultiplayer->m_vecAkimboTarget.fY = pTargetVector [1];
                pMultiplayer->m_vecAkimboTarget.fZ = pTargetVector [2];
            }
            else
            {
                // Grab his remote storage
                pTempRemote = CRemoteDataSA::GetRemoteDataStorage ( pAPed );    // TODO: Can be optimized further by using the PlayerPed class. Not sure how to convert CPed to CPlayerPed
                if ( pTempRemote )
                {
                    if ( pTempRemote->ProcessPlayerWeapon () )
                    {
                        // If this is remote player, use the data in his remote storage to point his aim position
                        pTargetVector [0] = pTempRemote->m_vecAkimboTarget.fX;
                        pTargetVector [1] = pTempRemote->m_vecAkimboTarget.fY;
                        pTargetVector [2] = pTempRemote->m_vecAkimboTarget.fZ;
                    }
                }
            }
        }
    }

    _asm
    {
        // Restore all the registers from the stack
        popad

        // Run the instructions we replaced when this hook was added
        push    0FFFFFFFFh 
        push    83F0B6h 

        // Continue execution where we intercepted
        mov     edx, HOOKPOS_IKChainManager_LookAt
        add     edx, 7
        jmp     edx
    }
}


VOID _declspec(naked) HOOK_CWeapon__Fire()
{
    _asm
    {
        push    ebx
        mov     ebx, esp
        add     ebx, 0x18
        mov     vecTargetPosition, ebx
        mov     ebx, esp
        add     ebx, 0xC
        mov     ebx, [ebx]
        mov     vecOrigin, ebx
        mov     ebx, [esp+8]
        mov     pShootingPed, ebx
        pop     ebx

        pushad
    }    

    // Weapon inaccuracy and animations problems may be fixed by blanking out the CWeapon variables nTimer and beyond.

    bWeaponFire = true;
    if ( !WriteTargetDataForPed ( pShootingPed, vecTargetPosition, vecOrigin ) )
    {
        // Don't fire shot
         _asm
        {
            popad
            mov     al, 1
            retn    18h
        }
    }

     _asm
    {
        popad

        sub     esp, 0x3c
        push    ebx
        push    esi
        push    edi
    }

    _asm
    {
        mov     esi, HOOKPOS_CWeapon__Fire
        add     esi, 6
        jmp     esi
    }
}

VOID _declspec(naked) HOOK_CWeapon__PostFire()
{
    _asm
    {
        pushad
    }

    Event_PostFire ();

    _asm
    {
        popad

        pop     edi
        pop     esi
        pop     ebx
        add     esp, 3Ch
        ret     18h
    }
}

VOID _declspec(naked) HOOK_CWeapon__PostFire2() // handles the FALSE exit point at 0x074241E
{
    _asm
    {
        pushad
    }

    Event_PostFire ();

    _asm
    {
        popad

        add     esp, 3Ch
        ret     18h
    }
}

static const DWORD CWeapon_DoBulletImpact_RET = 0x73B557;
void _declspec(naked) HOOK_CWeapon_DoBulletImpact ()
{
    _asm
    {
        mov     eax, [esp+4]
        mov     pBulletImpactInitiator, eax
        mov     eax, [esp+8]
        mov     pBulletImpactVictim, eax
        mov     eax, [esp+12]
        mov     pBulletImpactStartPosition, eax
        mov     eax, [esp+16]
        mov     pBulletImpactEndPosition, eax
        pushad
    }

    Event_BulletImpact ();

    _asm
    {
        popad
        push    0xFFFFFFFF
        push    0x00848E50
        jmp     CWeapon_DoBulletImpact_RET
    }
}

VOID _declspec(naked) HOOK_CTaskSimpleGangDriveBy__PlayerTarget()
{
    // Replacement code
    _asm
    {
        mov     cTempGunDirection, al
        mov     pPedInterfaceTemp, edi
        pushad
    }

    // either store or change the data
    WriteGunDirectionDataForPed ( pPedInterfaceTemp, 0, 0, &cTempGunDirection );

    // cTempGunDirection may be modified by the function, so write it back
    _asm
    {
        popad
        movsx   eax, cTempGunDirection

        mov     edx, HOOKPOS_CTaskSimpleGangDriveBy__PlayerTarget
        add     edx, 6
        cmp     eax, 3 // have to do this down here or it doesn't work
        jmp     edx

    }
}

VOID _declspec(naked) HOOK_CPedIK__PointGunInDirection()
{
    _asm
    {
        mov     pPedIKInterface, ecx
        mov     edx, esp
        add     edx, 4
        mov     fDirectionX, edx
        add     edx, 4
        mov     fDirectionY, edx
        pushad
    }

    // either store or change the data
    WriteGunDirectionDataForPed ( (CPedSAInterface *)((DWORD)pPedIKInterface - 1292), fDirectionX, fDirectionY, 0 );

    // replacement code
    _asm
    {
        popad
        sub     esp, 0x10
        push    ebx
        push    ebp
        mov     ebp, ecx

        // Let the hooked func resume
        mov     edx, HOOKPOS_CPedIK__PointGunInDirection
        add     edx, 7
        jmp     edx
    }
}

// This hook prevents remote players always hitting local players if both players are targeting with sniper.
// This was because it then used the FireSniper mode rather than FireInstantHit so it appeared that the local
// player was shooting himself!
void _declspec(naked) HOOK_CWeapon__Fire_Sniper()
{
    /*
    007424A6   8B4424 14        MOV EAX,DWORD PTR SS:[ESP+14]
    007424AA   85C0             TEST EAX,EAX
    */

    _asm
    {
        mov     pPedInterfaceTemp, edi
        pushad
    }

    if ( IsLocalPlayer(pPedInterfaceTemp) )
    {
        // use sniper (local players)
        _asm
        {
            popad
            mov     ecx, HOOKPOS_CWeapon__Fire_Sniper
            add     ecx, 6
            
            mov     eax, dword ptr [esp+0x14]
            test    eax, eax
            
            jmp     ecx
        }
    }
    else
    {
        // use instanthit (remote players)
        _asm
        {
            popad
            mov     ecx, HOOKRET_CWeapon__Fire_Sniper
            jmp     ecx
        }
    }

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
            pEvent->SetDamageReason ( g_GenerateDamageEventReason );
            // Call the event
            bool bReturn = m_pDamageHandler ( pPed, pEvent );
            // Destroy the CEventDamageSA (so we dont get a leak)
            pEvent->Destroy ();
            // Finally, return
            return bReturn;
        }
    }
    return true;
}

CPedSAInterface * affectsPed = 0;
CEventDamageSAInterface * event = 0;
void _declspec(naked) HOOK_CEventDamage__AffectsPed()
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

            mov     ecx, HOOKPOS_CEventDamage__AffectsPed
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


void ProcessStartFire ( CFireSAInterface * fire )
{
    if ( m_pFireHandler )
    {
        DWORD dwID = ((DWORD)fire - 0xB71F80 /* CLASS_CFireManager */) / 40;
        CFire * pFire = pGameInterface->GetFireManager()->GetFire ( dwID );
        if ( pFire )
        {            
            m_pFireHandler ( pFire );
        }
    }
}

CFireSAInterface * tempFire;
DWORD dwStoredReturn;

//  CFire*  StartFire(CVector vecLocation, float fFireSize=DEFAULT_FIRE_PARTICLE_SIZE, bool8 bExtinguishEnabled=TRUE, CEntity* pStartedFireEntity = NULL, UInt32 ArgBurnTime = FIRE_AVERAGE_BURNTIME, Int8 NumGenerationsAllowed = 100, Bool8 bReportFire = true);
void _declspec(naked) HOOK_CFireManager__StartFire()
{
    // replacement code
    _asm
    {
        mov     edx, [esp]
        mov     dwStoredReturn, edx
        mov     edx, returnHere
        mov     [esp], edx

        push    ecx
        push    ebp
        mov     ebp, [esp+0x10]

        mov     edx, HOOKPOS_CFireManager__StartFire
        add     edx, 6
        jmp     edx

        returnHere:
        mov     tempFire, eax

        pushad
    }

    ProcessStartFire ( tempFire );

    _asm
    {
        popad

        mov     eax, tempFire
        mov     edx, dwStoredReturn
        jmp     edx
    }
}

//  CFire*  StartFire(CEntity *pBurningEntity, CEntity *pStartedFireEntity, float fFireSize=DEFAULT_FIRE_PARTICLE_SIZE, bool8 bExtinguishEnabled=TRUE, UInt32 ArgBurnTime = FIRE_AVERAGE_BURNTIME, Int8 NumGenerationsAllowed = 100);
void _declspec(naked) HOOK_CFireManager__StartFire_()
{
    
    _asm
    {
        // replacement code
        mov     edx, [esp]
        mov     dwStoredReturn, edx
        mov     edx, returnHere
        mov     [esp], edx

        push    esi
        push    edi
        mov     edi, [esp+0xC]

        //
        mov     edx, HOOKPOS_CFireManager__StartFire_
        add     edx, 6
        jmp     edx

        returnHere:
        mov     tempFire, eax

        pushad
    }

    ProcessStartFire ( tempFire );

    _asm
    {
        popad

        mov     eax, tempFire
        jmp     dwStoredReturn
    }
}

bool ProcessProjectileAdd () 
{
    if ( m_pProjectileStopHandler )
    {
        CPools * pPools = pGameInterface->GetPools ();
        CEntity * pOwner = NULL;
        if ( pProjectileOwner )
        {
            switch ( pProjectileOwner->nType )
            {
                case ENTITY_TYPE_VEHICLE:
                    pOwner = pPools->GetVehicle ( (DWORD *)pProjectileOwner );
                    break;
                case ENTITY_TYPE_PED:
                    pOwner = pPools->GetPed ( (DWORD *)pProjectileOwner );
                    break;
                case ENTITY_TYPE_OBJECT:
                    //pPools->GetObject ( (DWORD *)event->inflictor );
                default:
                    pOwner = NULL;
            }
        }
    
        if ( projectileTargetEntityInterface )
        {
            switch ( projectileTargetEntityInterface->nType )
            {
                case ENTITY_TYPE_VEHICLE:
                    projectileTargetEntity = pPools->GetVehicle ( (DWORD *)projectileTargetEntityInterface );
                    break;
                case ENTITY_TYPE_PED:
                    projectileTargetEntity = pPools->GetPed ( (DWORD *)projectileTargetEntityInterface );
                    break;
                case ENTITY_TYPE_OBJECT:
                    //pPools->GetObject ( (DWORD *)event->inflictor );
                default:
                    projectileTargetEntity = NULL;
            }
        }
        return m_pProjectileStopHandler ( pOwner, projectileWeaponType, projectileOrigin, projectileForce, projectileTarget, projectileTargetEntity );
    }
    return true;
}
    
void ProcessProjectile ( )
{
    if ( m_pProjectileHandler != NULL )
    {
        CPoolsSA * pPools = (CPoolsSA*)pGameInterface->GetPools();
        CEntity * pOwner = NULL;
        if ( pProjectileOwner )
        {
            switch ( pProjectileOwner->nType )
            {
                case ENTITY_TYPE_VEHICLE:
                    pOwner = pPools->GetVehicle ( (DWORD *)pProjectileOwner );
                    break;
                case ENTITY_TYPE_PED:
                    pOwner = pPools->GetPed ( (DWORD *)pProjectileOwner );
                    break;
                case ENTITY_TYPE_OBJECT:
                    //pPools->GetObject ( (DWORD *)event->inflictor );
                default:
                    pOwner = NULL;
            }
        }

        CProjectileInfo * projectileInfo = pGameInterface->GetProjectileInfo()->GetProjectileInfo(dwProjectileInfoIndex);
        CProjectile* projectile = pGameInterface->GetProjectileInfo()->GetProjectile(pProjectile);
        projectile->SetProjectileInfo ( projectileInfo );
        m_pProjectileHandler ( pOwner, projectile, projectileInfo, projectileWeaponType, projectileOrigin, projectileForce, projectileTarget, projectileTargetEntity );
        projectileTargetEntity = NULL;
    }
}

// CProjectileInfo::AddProjectile(class CEntity * owner,enum eWeaponType weapon type
// ,class CVector origin?,float 0?,class CVector * direction,class CEntity * target)
void _declspec(naked) HOOK_CProjectileInfo__AddProjectile()
{
    _asm
    {
        mov     edx, [esp+4]
        mov     pProjectileOwner, edx

        mov     edx, [esp+8]
        mov     projectileWeaponType, edx

        lea     edx, [esp+12]
        mov     projectileOrigin, edx

        mov     edx, [esp+24]
        mov     projectileForce, edx

        mov     edx, [esp+28]
        mov     projectileTarget, edx

        mov     edx, [esp+32]
        mov     projectileTargetEntityInterface, edx

        pushad
    }
    if ( ProcessProjectileAdd() )
    { // projectile should be created
        _asm
        {
            popad
            push    0xFFFFFFFF
            mov     edx, RETURN_CProjectile__AddProjectile
            jmp     edx
        }
    }
    else
    {
        _asm
        {
            popad
            xor al, al
            retn
        }
    }
}

void _declspec(naked) HOOK_CProjectile__CProjectile()
{
    _asm
    {
        mov     dwProjectileInfoIndex, ebx // it happens to be in here, luckily
        mov     pProjectile, ecx
        pushad
    }

    ProcessProjectile ();

    _asm
    {
        popad
        push    0xFFFFFFFF
        mov     edx, RETURN_CProjectile__CProjectile
        jmp     edx
    }
}


static void CheckInVehicleDamage()
{
    CPlayerPed * pPed = dynamic_cast < CPlayerPed * > ( m_pools->GetPed ( ( DWORD * ) pShootingPed ) );
    if ( pPed && !IsLocalPlayer( pPed ) )
    {
        // Did he hit a vehicle?
        if ( *ppInstantHitEntity && (*ppInstantHitEntity)->nType == ENTITY_TYPE_VEHICLE )
        {
            // Lets do a windscreen shooting check
            CWeapon * pWeapon = pPed->GetWeapon ( pPed->GetCurrentWeaponSlot () );
            if ( pWeapon )
            {
                eWeaponType weaponType = pWeapon->GetType();
                DWORD dwFunc = FUNC_CWeapon_CheckForShootingVehicleOccupant;

                _asm
                {
                    push    pInstantHitStart
                    push    pInstantHitEnd
                    push    weaponType
                    push    ppInstantHitColPoint
                    push    ppInstantHitEntity
                    call    dwFunc
                    add     esp, 0x14
                }

            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CWeapon_FireInstantHit_Mid
//
// Called when the local player fires a bullet
// If using bullet sync, send the bullet vectors to the other players
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMy_CWeapon_FireInstantHit_Mid ( CEntitySAInterface* pEntity, CVector* pvecNonAimedStart, CVector* pvecAimedStart, CVector* pvecEnd, bool bFlag )
{
    CPed * pTargetingPed = m_pools->GetPed ( (DWORD *)pEntity );
    if ( IsLocalPlayer ( pTargetingPed ) )
    {
        CVector vecEnd = *pvecEnd;
        CVector vecStart = *pvecAimedStart;
        if ( vecEnd == vecStart )
            vecStart = *pvecNonAimedStart;

        // Trim end point to weapon range
        float fRange = pTargetingPed->GetCurrentWeaponRange ();
        CVector vecDir = vecEnd - vecStart;
        float fLength = vecDir.Length ();
        if ( fRange < fLength )
        {
            vecDir.Normalize ();
            vecEnd = vecStart + vecDir * fRange;
        }

        // Save these for BulletImpact
        vecLastLocalPlayerBulletStart = vecStart;
        vecLastLocalPlayerBulletEnd = vecEnd;

        if ( m_pBulletFireHandler )
            m_pBulletFireHandler ( pTargetingPed, &vecStart, &vecEnd );
    }
}

// Hook info
#define HOOKPOS_CWeapon_FireInstantHit_Mid                         0x740B89
#define HOOKSIZE_CWeapon_FireInstantHit_Mid                        5
DWORD RETURN_CWeapon_FireInstantHit_Mid =                          0x740B8E;
void _declspec(naked) HOOK_CWeapon_FireInstantHit_Mid ()
{
    _asm
    {
        pushad

        mov     ecx, esp
        add     ecx, 20h

        push    eax             // flag

        mov     edx, ecx
        add     edx, 10h        // end
        push    edx

        mov     edx, ecx
        add     edx, 38h        // aimed start
        push    edx

        mov     edx, ecx
        add     edx, 78h        // non aimed start
        push    edx

        push    edi
        call    OnMy_CWeapon_FireInstantHit_Mid
        add     esp, 4*5
        popad

        // Do original code and continue
        mov         ecx,dword ptr [esp+14h] 
        push        eax 
        jmp     RETURN_CWeapon_FireInstantHit_Mid
    }
}

// Hook install
VOID InitFireInstantHit_MidHooks()
{
    HookInstall ( HOOKPOS_CWeapon_FireInstantHit_Mid, (DWORD)HOOK_CWeapon_FireInstantHit_Mid, HOOKSIZE_CWeapon_FireInstantHit_Mid );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// HandleRemoteInstantHit
//
// Called when a remote player fires a bullet
//
//////////////////////////////////////////////////////////////////////////////////////////
void HandleRemoteInstantHit( void )
{
    CPed * pTargetingPed = m_pools->GetPed ( (DWORD *)pShootingPed );
    CPlayerPed* pTargetingPlayerPed = dynamic_cast < CPlayerPed* > ( pTargetingPed );
    if ( !pTargetingPlayerPed ) return;

    if ( !IsLocalPlayer( pTargetingPed ) )
    {
        CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( pTargetingPlayerPed );
        if ( data )
        {
            if ( data->ProcessPlayerWeapon () )
            {
                // Replace start and end vectors if we have any bullet sync information
                if ( data->m_shotSyncData.m_bRemoteBulletSyncVectorsValid )
                {
                    *pInstantHitStart = data->m_shotSyncData.m_vecRemoteBulletSyncStart;
                    *pInstantHitEnd = data->m_shotSyncData.m_vecRemoteBulletSyncEnd;
                }
            }
        }
    }
}



void _cdecl DoFireInstantHitPokes ( void )
{
    MemPutFast < unsigned char > ( VAR_CWorld_IncludeCarTyres, 1 );
}

DWORD dwFunc_CWeapon_FireInstantHit_ret = 0x740B6E;
DWORD dwFunc_CWorld_ProcessLineOfSight = 0x56BA00;
void _declspec(naked) HOOK_CWeapon_FireInstantHit ()
{
    /* CWeapon::FireInstantHit->CWorld::ProcessLineOfSight
    00740B42  push        1    
    00740B44  push        0    
    00740B46  push        0    
    00740B48  push        1    
    00740B4A  push        1    
    00740B4C  push        1    
    00740B4E  push        1    
    00740B50  push        1    
    00740B52  lea         eax,[esp+58h] 
    00740B56  push        eax  
    00740B57  lea         ecx,[esp+0C4h] 
    00740B5E  push        ecx  
    00740B5F  lea         edx,[esp+48h] 
    00740B63  lea         eax,[esp+64h] 
    00740B67  push        edx  
    00740B68  push        eax  
    00740B69  call        0056BA00 
    */
    _asm
    {
        push        1    
        push        0    
        push        0    
        push        1    
        push        1    
        push        1    
        push        1    
        push        1    
        lea         eax,[esp+58h]
        mov         ppInstantHitEntity, eax
        push        eax  
        lea         ecx,[esp+0C4h]
        mov         ppInstantHitColPoint, ecx
        push        ecx  
        lea         edx,[esp+48h]
        mov         pInstantHitEnd, edx
        lea         eax,[esp+64h]
        mov         pInstantHitStart, eax
        push        edx  
        push        eax
        pushad
    }

    // Make sure we include car tyres in our ProcessLineOfSight check
    _asm call DoFireInstantHitPokes

    HandleRemoteInstantHit ();

    _asm
    {
        popad
        call        dwFunc_CWorld_ProcessLineOfSight
        pushad
    }

    CheckInVehicleDamage();

    _asm
    {
        popad
        jmp         dwFunc_CWeapon_FireInstantHit_ret
    }
}

bool FireInstantHit_CameraMode ()
{
    CPlayerPed * pPed = dynamic_cast < CPlayerPed * > ( m_pools->GetPed ( ( DWORD * ) pShootingPed ) );
    if ( pPed && !IsLocalPlayer( pPed ) )
    {
        // Are we onfoot?
        if ( !pPed->GetVehicle () )
        {
            return true;
        }
    }
    return false;
}

DWORD dwFunc_CWeapon_FireInstantHit_CameraMode_ret = 0x7403C7;
DWORD dwAddr_FireInstantHit_CameraMode = 0x740389;
DWORD dwAddr_FireInstantHit_CameraMode_2 = 0x740373;
short sFireInstantHit_CameraMode_camMode = 0;
void _declspec(naked) HOOK_CWeapon_FireInstantHit_CameraMode ()
{
    /* CWeapon::FireInstantHit->'CameraMode switch'
    0074036D  cmp         ax,35h (53)   ** <hook>
    00740371  je          00740389      ** <hook>
    00740373  cmp         ax,37h (55)
    00740377  je          00740389 
    00740379  cmp         ax,41h (65)
    0074037D  je          00740389 
    0074037F  cmp         ax,31h (39)
    00740383  jne         007407C1
    */
    _asm 
    {
        mov     sFireInstantHit_CameraMode_camMode, ax
        pushad
    }

    if ( FireInstantHit_CameraMode () )
    {
        _asm
        {
            popad
            jmp          dwAddr_FireInstantHit_CameraMode
        }
    }
    else
    {
        if ( sFireInstantHit_CameraMode_camMode == 0x35 )
        {
            _asm
            {
                popad
                jmp dwAddr_FireInstantHit_CameraMode
            }
        }
        else
        {
            _asm
            {
                popad
                jmp         dwAddr_FireInstantHit_CameraMode_2
            }
        }
    }
}

CPedSAInterface * pFireInstantHit_IsPlayerPed = NULL;
bool FireInstantHit_IsPlayer ()
{
    CPlayerPed * pPed = dynamic_cast < CPlayerPed * > ( m_pools->GetPed ( ( DWORD * ) pFireInstantHit_IsPlayerPed ) );
    if ( pPed )
    {
        if ( IsLocalPlayer( pPed ) )
            return true;
        CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( pPed );
        if ( data )
        {
            if ( data->ProcessPlayerWeapon () )
            {
                return true;
            }
        }
    }
    return false;
}

DWORD RETURN_CWeapon_FireInstantHit_IsPlayer = 0x740353;
DWORD FUNC_CPlayer_IsPed = 0x5DF8F0;
void _declspec(naked) HOOK_CWeapon_FireInstantHit_IsPlayer ()
{
    /*
    0074034C  call        005DF8F0          <hook>
    00740351  test        al,al             <hook>
    00740353  je          007407C1          <return>
    */

    /*
    00740C3D  call        005DF8F0 
    00740C42  mov         eax,dword ptr [esp+1BCh] 
    00740C49  mov         ecx,dword ptr [eax] 

    00740E66  call        005DF8F0 
    00740E6B  test        al,al 
    00740E6D  je          00740E7E 
    */
    _asm
    {
        mov     pFireInstantHit_IsPlayerPed, ecx
        pushad
    }
    if ( !FireInstantHit_IsPlayer () )
    {
        _asm
        {
            popad
            xor     al, al
            test    al, al
            jmp     RETURN_CWeapon_FireInstantHit_IsPlayer
        }
    }
    else
    {
        _asm
        {
            popad
            call    FUNC_CPlayer_IsPed
            test    al,al
            jmp     RETURN_CWeapon_FireInstantHit_IsPlayer
        }
    }
}
    
/*
VOID _declspec(naked) HOOK_CCamera__Find3rdPersonCamTargetVector()
{*/
    /*
    0046FB36  |. 5D             POP EBP
    0046FB37  |. 5E             POP ESI
    0046FB38  |. 5B             POP EBX
    0046FB39  \. C2 1800        RETN 18
    */
    /*_asm
    {
        mov     ebp, esp
        add     ebp, 0x14
        mov     vecShotOrigin, ebp
        add     ebp, 0xC
        mov     esi, [ebp]
        mov     vecStartVector, esi
        add     ebp, 4
        mov     esi, [ebp]
        mov     vecTargetVector, esi

        pushad
    }
        
    if(IsNotInLocalContext() && GetContextSwitchPedID())
    {
        // its a remote player, so replace the data with the net data
        sprintf(szDebug, "Switched Target Vectors to %f  %f  %f", 
            RemotePlayerTargetVectors[GetContextSwitchPedID()].fX, 
            RemotePlayerTargetVectors[GetContextSwitchPedID()].fY, 
            RemotePlayerTargetVectors[GetContextSwitchPedID()].fZ);
        OutputDebugString(szDebug);

        sprintf(szDebug, "Switched Start Vectors to %f  %f  %f", 
            RemotePlayerStartVectors[GetContextSwitchPedID()].fX, 
            RemotePlayerStartVectors[GetContextSwitchPedID()].fY, 
            RemotePlayerStartVectors[GetContextSwitchPedID()].fZ);
        OutputDebugString(szDebug);
        
        
        MemCpy (vecTargetVector, &RemotePlayerTargetVectors[GetContextSwitchPedID()], sizeof(CVector));
        MemCpy (vecStartVector, &RemotePlayerStartVectors[GetContextSwitchPedID()], sizeof(CVector));
    }
    else
    {
        // Its the Local Player, so save the data so it can be sent
        MemCpy (&LocalPlayerShotOriginVector, vecShotOrigin, sizeof(CVector));

        sprintf(szDebug, "Saved Local Shot Origin Vector  %f  %f  %f", 
            LocalPlayerShotOriginVector.fX, 
            LocalPlayerShotOriginVector.fY, 
            LocalPlayerShotOriginVector.fZ);
        OutputDebugString(szDebug);*/
    /*  MemCpy (&LocalPlayerTargetVector, vecTargetVector, sizeof(CVector));
        MemCpy (&LocalPlayerStartVector, vecStartVector, sizeof(CVector));
        
        sprintf(szDebug, "Saved Local Target Vectors  %f  %f  %f", 
            LocalPlayerTargetVector.fX, 
            LocalPlayerTargetVector.fY, 
            LocalPlayerTargetVector.fZ);
        OutputDebugString(szDebug);

        sprintf(szDebug, "Saved Local Start Vectors  %f  %f  %f", 
            LocalPlayerStartVector.fX, 
            LocalPlayerStartVector.fY, 
            LocalPlayerStartVector.fZ);
        OutputDebugString(szDebug);*/
/*  }

    _asm
    {
        popad

        pop     ebp
        pop     esi
        pop     ebx
        retn    0x18
    }   
}*/
/*
VOID _declspec(naked) HOOK_CWeapon__FireShotgun()
{*/
    // this is used to store and replace the CrossProduct
    /*
    005CDA99   . 50                      PUSH EAX
    005CDA9A   . 8D8424 C4010000         LEA EAX,DWORD PTR SS:[ESP+1C4]
    005CDAA1   . 51                      PUSH ECX
    005CDAA2   . 50                      PUSH EAX
    005CDAA3   . E8 0826F1FF             CALL gta-vc.004E00B0
    005CDAA8   . 89D8                    MOV EAX,EBX
    005CDAAA   . 83C4 0C                 ADD ESP,0C
    */

/*  _asm pushad

    if(IsNotInLocalContext() && GetContextSwitchPedID())
    {
        _asm
        {
            popad

            push    eax
            lea     eax, dword ptr [esp+0x1C4]
            mov     vecCrossProduct, eax
            pop     eax

            pushad
        }
        sprintf(szDebug, "Switched Cross Products to %f  %f  %f (0x%X)", 
            RemotePlayerCrossProducts[GetContextSwitchPedID()].fX, 
            RemotePlayerCrossProducts[GetContextSwitchPedID()].fY, 
            RemotePlayerCrossProducts[GetContextSwitchPedID()].fZ, vecCrossProduct);
        OutputDebugString(szDebug);
        MemSet (vecCrossProduct,0,sizeof(CVector));
    //  MemCpy (vecCrossProduct, &RemotePlayerCrossProducts[GetContextSwitchPedID()], sizeof(CVector));
    }
    else
    {
        _asm
        {
            popad

            push    eax
            lea     eax, dword ptr [esp+0x1C4]
            mov     vecCrossProduct, eax
            push    ecx
            push    eax
            mov     eax, FUNC_CrossProduct
            call    eax
            mov     eax, ebx
            add     esp, 0xC

            pushad
        }

        MemCpy (&LocalPlayerCrossProduct, vecCrossProduct, sizeof(CVector));
        sprintf(szDebug, "SHOTGUN: Saved Local Cross Product  %f  %f  %f", 
            LocalPlayerCrossProduct.fX, 
            LocalPlayerCrossProduct.fY, 
            LocalPlayerCrossProduct.fZ);
        OutputDebugString(szDebug);
    }

    _asm
    {
        popad

        mov     edx, HOOKPOS_CWeapon__FireShotgun
        add     edx, 20
        jmp     edx
    }   
}*/

CPedSAInterface *CEventVehicleExplosion_pPed;

void CEventVehicleExplosion_NotifyDeathmatch()
{
    if ( m_pDeathHandler )
    {
        CPoolsSA * pPools = (CPoolsSA*)pGameInterface->GetPools();
        CPed * pPed = pPools->GetPed ( (DWORD *)CEventVehicleExplosion_pPed );

        if ( pPed ) 
            m_pDeathHandler(pPed, 63, 3);
    }
}

void _declspec(naked) HOOK_CEventVehicleExplosion__AffectsPed()
{
    _asm
    {
        // Save the ped
        mov CEventVehicleExplosion_pPed, edi
        // Replacement code for the hook
        pop     edi
        setz    al
        pop esi
        
        // Verify that the ped is affected
        cmp al, 1
        jnz return_from

        // Verify that this call is from the correct location
        cmp [esp], 0x4ab4c4
        jnz return_from
        
        pushad
    }

    // Notify Deathmatch about the death
    CEventVehicleExplosion_NotifyDeathmatch();

    _asm
    {  
        popad

return_from:
        retn 4
    }
}

