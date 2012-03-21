/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA.cpp
*  PURPOSE:     Multiplayer module class 1.3
*  DEVELOPERS:  Cazomino05 <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

WaterCannonHitHandler* m_pWaterCannonHitHandler = NULL;

#define HOOKPOS_CEventHitByWaterCannon                      0x729899
DWORD RETURN_CWaterCannon_PushPeds_RETN = 0x7298A7;
DWORD CALL_CEventHitByWaterCannon = 0x4B1290;
DWORD RETURN_CWaterCannon_PushPeds_RETN_Cancel = 0x729AEB;

void HOOK_CEventHitByWaterCannon ( );

#define HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeSingle       0x6AB366
DWORD RETN_CVehicle_ProcessStuff_TestSirenTypeSingle = 0x6AB36D;

#define HOOKPOS_CVehicle_ProcessStuff_SetSirenPositionSingle    0x6ABC51
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionSingle = 0x6ABC56;

#define HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeDual         0x6AB382
DWORD RETN_CVehicle_ProcessStuff_TestSirenTypeDual = 0x6AB389;

#define HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition1    0x6ABA47
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1 = 0x6ABA5E;

#define HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition2    0x6AB9F7
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2 = 0x6ABA07;

#define HOOKPOS_CVehicle_DoesVehicleUseSiren                    0x6D8470
DWORD RETN_CVehicleDoesVehicleUseSirenRetn = 0x6D8497;

void HOOK_CVehicle_ProcessStuff_TestSirenTypeSingle ( );
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle ( );
void HOOK_CVehicle_ProcessStuff_TestSirenTypeDual ( );
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed ( );
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue ( );
void HOOK_CVehicle_DoesVehicleUseSiren ( );

void CMultiplayerSA::Init_13 ( void )
{
    InitHooks_13 ( );
    InitMemoryCopies_13 ( );
}

void CMultiplayerSA::InitHooks_13 ( void )
{
    // HookInstalls go here
    HookInstall ( HOOKPOS_CEventHitByWaterCannon, (DWORD)HOOK_CEventHitByWaterCannon, 9 );

    // Siren hooks
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeSingle, (DWORD)HOOK_CVehicle_ProcessStuff_TestSirenTypeSingle, 7 ); // Test siren type is single for a jump
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_SetSirenPositionSingle, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle, 5 ); // mov before Push for the siren position
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeDual, (DWORD)HOOK_CVehicle_ProcessStuff_TestSirenTypeDual, 7 ); // test siren type is dual for a jump
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition1, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed, 15 ); // mov before push for the siren position (overhook so we can get RGBA)
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition2, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue, 15 ); // mov before push for the siren position (overhook so we can get RGBA)
    HookInstall ( HOOKPOS_CVehicle_DoesVehicleUseSiren, (DWORD)HOOK_CVehicle_DoesVehicleUseSiren, 5 ); // Does vehicle have a siren
}

void CMultiplayerSA::InitMemoryCopies_13 ( void )
{
    // Memory based fixes go here

}

// Siren Stuff
CVector * vecRelativeSirenPosition;

unsigned char ucSirenCount = 0;
unsigned char ucRandomiser = 0;

CVehicleSAInterface * pVehicleWithTheSiren = NULL;

bool bSiren = false;

bool bContinue = false;

float fTime = *((float*)0xB7C4E4);

DWORD dwSirenType = 0;
DWORD dwSirenType2 = 3;
DWORD dwRed = 0;
DWORD dwGreen = 0;
DWORD dwBlue = 0;
DWORD dwMinAlphaValue = 0x46;
DWORD dwSirenTypePostHook = 0;

bool DoesVehicleHaveSiren ( )
{
    return ( pVehicleWithTheSiren->m_nModelIndex == 596 || pVehicleWithTheSiren->m_nModelIndex == 597 ||
        pVehicleWithTheSiren->m_nModelIndex == 598 || pVehicleWithTheSiren->m_nModelIndex == 599 ||

        pVehicleWithTheSiren->m_nModelIndex == 490 || pVehicleWithTheSiren->m_nModelIndex == 601 ||
        pVehicleWithTheSiren->m_nModelIndex == 528 || pVehicleWithTheSiren->m_nModelIndex == 407 ||
        pVehicleWithTheSiren->m_nModelIndex == 416 || pVehicleWithTheSiren->m_nModelIndex == 433 ||
        pVehicleWithTheSiren->m_nModelIndex == 427 || pVehicleWithTheSiren->m_nModelIndex == 544 ||
        pVehicleWithTheSiren->m_nModelIndex == 523 || pVehicleWithTheSiren->m_nModelIndex == 432 );
}
void GetVehicleSirenType ( )
{
    if ( DoesVehicleHaveSiren ( ) )
    {
        return;
    }
    if ( pVehicleWithTheSiren )
    {
        CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
        if ( pVehicle )
        {
            DWORD dwVehicleSirenType = pVehicle->GetVehicleSirenType ( );
            if ( dwVehicleSirenType >= 1 && dwVehicleSirenType <= 2 )
            {
                // Single Sirens
                dwSirenType = ++dwVehicleSirenType;
                dwSirenType2 = 5;
            }
            else
            {
                dwVehicleSirenType -= 2;
                dwSirenType = 0;
                dwSirenType2 = dwVehicleSirenType;
            }
        }
    }
}
void _declspec(naked) HOOK_CVehicle_ProcessStuff_TestSirenTypeSingle ( )
{
    _asm
    {
        pushad
            movzx edx, byte ptr [edi+6ACCD0h]
        mov dwSirenType, edx
            mov pVehicleWithTheSiren, esi
    }
    GetVehicleSirenType ( );
    _asm
    {
        popad
            mov edx, dwSirenType
            JMP RETN_CVehicle_ProcessStuff_TestSirenTypeSingle
    }
}

void ProcessVehicleSirenPosition ( )
{
    if ( DoesVehicleHaveSiren ( ) )
    {
        bContinue = false;
        return;
    }
    bContinue = true;
    if ( pVehicleWithTheSiren )
    {
        CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
        if ( pVehicle )
        {
            if ( pVehicle->DoesVehicleHaveSirens ( ) && pVehicle->GetVehicleSirenCount ( ) >= 0 )
            {
                unsigned char ucVehicleSirenCount = pVehicle->GetVehicleSirenCount ( );
                ucSirenCount = pVehicle->GetVehicleCurrentSirenID ( );
                if ( ucSirenCount > ucVehicleSirenCount )
                {
                    ucSirenCount = ucRandomiser;
                    if ( ucVehicleSirenCount > 0 )
                        ucRandomiser = rand () % pVehicle->GetVehicleSirenCount ( );
                    else
                        ucRandomiser = 0;
                }
                pVehicle->GetVehicleSirenPosition ( ucSirenCount, *vecRelativeSirenPosition );
                CMatrix matCamera;
                CMatrix matVehicle;
                pVehicle->GetMatrix( &matVehicle );

                CCamera* pCamera = pGameInterface->GetCamera ();
                pCamera->GetMatrix( &matCamera );


                CVector vecSirenPosition = matVehicle.TransformVector ( *vecRelativeSirenPosition );

                SLineOfSightFlags flags;
                flags.bCheckBuildings = false;
                flags.bCheckDummies = false;
                flags.bCheckObjects = false;
                flags.bCheckPeds = false;
                flags.bCheckVehicles = true;
                flags.bIgnoreSomeObjectsForCamera = false;
                flags.bSeeThroughStuff = false;
                flags.bShootThroughStuff = false;
                pGameInterface->GetWorld()->IgnoreEntity ( NULL );
                CColPoint* pColPoint = NULL;
                CEntity* pGameEntity = NULL;
                if ( pGameInterface->GetWorld()->ProcessLineOfSight( &matCamera.vPos, &vecSirenPosition, &pColPoint, &pGameEntity, flags ) == true )
                {
                    dwRed = 0;
                    dwGreen = 0;
                    dwBlue = 0;
                }
                else
                {
                    fTime = *((float*)0xB7C4E4) * 0.1f;
                    float fMinimumAlpha = pVehicle->GetVehicleSirenMinimumAlpha ( ucSirenCount );
                    if ( fTime <= fMinimumAlpha )
                    {
                        fTime = fMinimumAlpha;
                    }
                    SColor tSirenColour = pVehicle->GetVehicleSirenColour ( ucSirenCount );

                    dwRed = (DWORD)( tSirenColour.R * fTime );
                    dwGreen = (DWORD)( tSirenColour.G * fTime );
                    dwBlue = (DWORD)( tSirenColour.B * fTime );
                }
                pVehicle->SetVehicleCurrentSirenID ( ++ucSirenCount );
            }
        }
    }
}

void _declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle ( )
{
    _asm
    {
        lea edx, [esp+64h]
        pushad
            mov pVehicleWithTheSiren, esi
            mov vecRelativeSirenPosition, edx
    }
    ProcessVehicleSirenPosition ( );
    _asm
    {
        popad
            push edx
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionSingle
    }
}

void TestSirenTypeDualDefaultFix ( )
{
    if ( DoesVehicleHaveSiren ( ) )
    {
        dwSirenType2 = dwSirenTypePostHook;
        return;
    }
    else
    {
        GetVehicleSirenType ( );
    }
}
void _declspec(naked) HOOK_CVehicle_ProcessStuff_TestSirenTypeDual ( )
{
    _asm
    {
        movzx edi, byte ptr ds:[edi+06ACDACh]
        pushad
            mov pVehicleWithTheSiren, esi
            mov dwSirenTypePostHook, edi
    }
    TestSirenTypeDualDefaultFix ( );
    _asm
    {
        popad
            mov edi, dwSirenType2
            JMP RETN_CVehicle_ProcessStuff_TestSirenTypeDual
    }
}

void _declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed ( )
{
    _asm
    {
        lea eax,[esp+130h]
        pushad
            mov pVehicleWithTheSiren, esi
            mov vecRelativeSirenPosition, eax
    }
    ProcessVehicleSirenPosition ( );
    if ( bContinue )
    {

        _asm
        {
            popad
                push eax
                mov eax, dwRed // Red
                mov ecx, dwBlue // Blue
                mov edx, dwGreen // Green
                push 0FFh
                push ecx
                push edx
                push eax
                JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1
        }
    }
    else
    {
        _asm
        {
            popad
                push eax
                mov eax, DWORD PTR SS:[esp+8Ch]
            push 0FFh
                push ecx
                push edx
                push eax
                JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1
        }
    }
}

void _declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue ( )
{
    _asm
    {
        lea eax, [esp+130h]
        pushad
            mov pVehicleWithTheSiren, esi
            mov vecRelativeSirenPosition, eax
    }
    ProcessVehicleSirenPosition ( );
    if ( bContinue )
    {
        _asm
        {
            popad
                push eax
                mov ecx, dwBlue // Blue
                mov edx, dwGreen // Green
                mov ebp, dwRed // Red
                push 0FFh
                push ecx
                push edx
                push ebp

                JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2
        }
    }
    else
    {
        _asm
        {
            popad
                push eax
                push 0FFh
                push ecx
                push edx
                push ebp

                JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2
        }
    }
}

void TestVehicleForSiren ( )
{
    CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
    if ( pVehicle && DoesVehicleHaveSiren ( ) == false )
    {
        bSiren = pVehicle->DoesVehicleHaveSirens ( );
        return;
    }
    bSiren = true;
    return;
}

void _declspec(naked) HOOK_CVehicle_DoesVehicleUseSiren ( )
{
    _asm
    {
        pushad  
            mov pVehicleWithTheSiren, ecx
    }

    TestVehicleForSiren ( );
    if ( bSiren )
    {
        _asm
        {
            popad   
                mov al, 1
                jmp RETN_CVehicleDoesVehicleUseSirenRetn
        }
    }
    else
    {
        _asm
        {
            popad
                xor al, al
                jmp RETN_CVehicleDoesVehicleUseSirenRetn
        }
    }
}

void CMultiplayerSA::SetWaterCannonHitHandler ( WaterCannonHitHandler * pHandler )
{
    m_pWaterCannonHitHandler = pHandler;
}


CPedSAInterface * pPedHitByWaterCannonInterface = NULL;
CVehicleSAInterface * pVehicleWithTheCannonMounted = NULL;
bool TriggerTheEvent ( )
{
    // Is our handler alive
    if ( m_pWaterCannonHitHandler )
    {
        // Return our handlers return
        return !m_pWaterCannonHitHandler ( pVehicleWithTheCannonMounted, pPedHitByWaterCannonInterface );
    }
    return false;
}

void _declspec(naked) HOOK_CEventHitByWaterCannon ( )
{
    _asm
    {
        pushad
            // EDX = CWaterCannon
            // EDX+0h = CVehicle Owner
            // ESI = CPed Hit
            mov eax, [edx]
        mov pPedHitByWaterCannonInterface, esi
            mov pVehicleWithTheCannonMounted, eax
    }
    if ( TriggerTheEvent() )
    {
        _asm
        {
            popad
                // Cancel.
                jmp RETURN_CWaterCannon_PushPeds_RETN_Cancel
        }
    }
    else
    {
        _asm
        {
            popad
                // Replaced code
                push ebp
                push ecx
                lea ecx, [esp+0B0h]
            // Call our function
            call CALL_CEventHitByWaterCannon
                // Go back to execution
                jmp RETURN_CWaterCannon_PushPeds_RETN
        }
    }
}