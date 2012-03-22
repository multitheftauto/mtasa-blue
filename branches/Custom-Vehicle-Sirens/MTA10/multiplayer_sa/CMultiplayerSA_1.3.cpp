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
    // Static function to check if the vehicle has sirens natively if so we ignore those for now
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
        // QUICK RUN
        return;
    }
    // Valid?
    if ( pVehicleWithTheSiren )
    {
        // Grab the CVehicle
        CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
        // Valid ? I see a pattern here!
        if ( pVehicle )
        {
            // Get the vehicles siren type (dual or single and the default colours such as red/yellow for fire truck or red/blue for the police car)
            // in SA the siren type also defines position so we totally ignore that.
            DWORD dwVehicleSirenType = pVehicle->GetVehicleSirenType ( );
            // siren type's 1 and 2 are single
            if ( dwVehicleSirenType == 1 || dwVehicleSirenType == 2 )
            {
                // Single Sirens
                dwSirenType = ++dwVehicleSirenType;
                // dual are off
                dwSirenType2 = 5;
            }
            else
            {
                // set the siren type minus 2 to account for the two dual sirens
                dwVehicleSirenType -= 2;
                // diable singles
                dwSirenType = 0;
                // dual are now on
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
        // Grab our original siren type
        movzx edx, byte ptr [edi+6ACCD0h]
        // Put it into dwSirenType
        mov dwSirenType, edx
        // Grab our siren vehicle
        mov pVehicleWithTheSiren, esi
    }
    // Call our Get siren type function which edits dwSirenType to our desired type
    GetVehicleSirenType ( );
    _asm
    {
        popad
        // put our new siren type into edx or old one for default/no sirens
        mov edx, dwSirenType
        // Jump back to the original code
        JMP RETN_CVehicle_ProcessStuff_TestSirenTypeSingle
    }
}

bool ProcessVehicleSirenPosition ( )
{
    // Disable our original siren based vehicles from this hook
    if ( DoesVehicleHaveSiren ( ) )
    {
        // return false so our hook knows we decided not to edit anything
        return false;
    }
    // Valid interface
    if ( pVehicleWithTheSiren )
    {
        // Grab our vehicle from the interface
        CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
        // Valid - Wait this seems familiar
        if ( pVehicle )
        {
            // Does the vehicle have sirens and is the siren count greater than 0 
            if ( pVehicle->DoesVehicleHaveSirens ( ) && pVehicle->GetVehicleSirenCount ( ) >= 0 )
            {
                // Get our siren count
                unsigned char ucVehicleSirenCount = pVehicle->GetVehicleSirenCount ( );
                // Get our current Siren ID
                ucSirenCount = pVehicle->GetVehicleCurrentSirenID ( );
                // Make sure we aren't beyond our limit
                if ( ucSirenCount > ucVehicleSirenCount )
                {
                    // Get our randomiser
                    ucSirenCount = pVehicle->GetSirenRandomiser ( );
                    // if we have more than 1 sirens
                    if ( ucVehicleSirenCount > 0 )
                        // Set our Randomiser
                        ucRandomiser = rand () % pVehicle->GetVehicleSirenCount ( );
                    else
                        // Set our Randomiser
                        ucRandomiser = 0;
                    // Update our stored Randomiser
                    pVehicle->SetSirenRandomiser ( ucRandomiser );
                }
                else
                {
                    // Set our Randomiser
                    ucRandomiser = rand () % pVehicle->GetVehicleSirenCount ( );
                    // Update our stored Randomiser
                    pVehicle->SetSirenRandomiser ( ucRandomiser );
                }
                // Gete our siren position for this siren count
                pVehicle->GetVehicleSirenPosition ( ucSirenCount, *vecRelativeSirenPosition );
                // Storage 'n stuff
                CMatrix matCamera;
                CMatrix matVehicle;
                // Grab our vehicle matrix
                pVehicle->GetMatrix( &matVehicle );

                // Get our Camera
                CCamera* pCamera = pGameInterface->GetCamera ();
                // Get the Camera Matrix
                pCamera->GetMatrix( &matCamera );

                // Get our sirens ACTUAL position from the relative value
                CVector vecSirenPosition = matVehicle.TransformVector ( *vecRelativeSirenPosition );

                // Setup our LOS flags
                SLineOfSightFlags flags;
                flags.bCheckBuildings = false;
                flags.bCheckDummies = false;
                flags.bCheckObjects = false;
                flags.bCheckPeds = false;
                flags.bCheckVehicles = true;
                flags.bIgnoreSomeObjectsForCamera = false;
                flags.bSeeThroughStuff = false;
                flags.bShootThroughStuff = false;
                // Ignore nothing
                pGameInterface->GetWorld()->IgnoreEntity ( NULL );
                // Variables 'n tings
                CColPoint* pColPoint = NULL;
                CEntity* pGameEntity = NULL;
                // Check if we can see it
                if ( pGameInterface->GetWorld()->ProcessLineOfSight( &matCamera.vPos, &vecSirenPosition, &pColPoint, &pGameEntity, flags ) == true )
                {
                    // Nope? Invisible
                    dwRed = 0;
                    dwGreen = 0;
                    dwBlue = 0;
                }
                else
                {
                    // Yep?
                    // Set our time based alpha to 10% of the current time float
                    fTime = *((float*)0xB7C4E4) * 0.1f;
                    // Get our minimum alpha
                    float fMinimumAlpha = pVehicle->GetVehicleSirenMinimumAlpha ( ucSirenCount );
                    // if our time is less than or equal to the minimum alpha
                    if ( fTime <= fMinimumAlpha )
                    {
                        // Set it to the minimum
                        fTime = fMinimumAlpha;
                    }
                    // Get our Siren RGB Colour
                    SColor tSirenColour = pVehicle->GetVehicleSirenColour ( ucSirenCount );
                    // times the R,G and B components by the fTime variable ( to get our time based brightness )
                    dwRed = (DWORD)( tSirenColour.R * fTime );
                    dwGreen = (DWORD)( tSirenColour.G * fTime );
                    dwBlue = (DWORD)( tSirenColour.B * fTime );
                }
                // Set our current Siren ID after we increment it
                pVehicle->SetVehicleCurrentSirenID ( ++ucSirenCount );
            }
        }
    }
    // Return true
    return true;
}

void _declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle ( )
{
    _asm
    {
        // Get our siren position into edx
        lea edx, [esp+64h]
        pushad
        // Grab our siren vehicle
        mov pVehicleWithTheSiren, esi
        // Put edx into our position variable
        mov vecRelativeSirenPosition, edx
    }
    // Call our main siren Process function
    ProcessVehicleSirenPosition ( );
    _asm
    {
        popad
        // push our siren position
        push edx
        // return back to SA
        JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionSingle
    }
}

void TestSirenTypeDualDefaultFix ( )
{
    // if we have a siren normally
    if ( DoesVehicleHaveSiren ( ) )
    {
        // Set our siren type to the post hook value
        dwSirenType2 = dwSirenTypePostHook;
    }
    else
    {
        // Set our siren type to the stored value
        GetVehicleSirenType ( );
    }
}
void _declspec(naked) HOOK_CVehicle_ProcessStuff_TestSirenTypeDual ( )
{
    _asm
    {
        // Grab our default siren type into edi 
        movzx edi, byte ptr ds:[edi+06ACDACh]
        pushad
        // Store our Vehicle interface
        mov pVehicleWithTheSiren, esi
        // Store our post hook default siren type
        mov dwSirenTypePostHook, edi
    }
    // Do our test and edit dwSirenType2 appropriately
    TestSirenTypeDualDefaultFix ( );
    _asm
    {
        popad
        // Move dwSirenType into edi
        mov edi, dwSirenType2
        // Return back to SA
        JMP RETN_CVehicle_ProcessStuff_TestSirenTypeDual
    }
}

void _declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed ( )
{
    _asm
    {
        // Grab our siren position vector
        lea eax,[esp+130h]
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, eax
    }

    // Call our main process siren function
    if ( ProcessVehicleSirenPosition ( ) )
    {
        _asm
        {
            popad
            // Push our position
            push eax
            // move our R,G,B components into registers
            mov eax, dwRed // Red
            mov edx, dwGreen // Green
            mov ecx, dwBlue // Blue
            push 0FFh
            // Push our R,G,B components (inverse order)
            push ecx
            push edx
            push eax
            // Return control
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1
        }
    }
    else
    {
        _asm
        {
            popad
            // Push our position
            push eax
            // move our red component into eax (you'l notice eax is used above so we are just giving it a value first)
            mov eax, DWORD PTR SS:[esp+8Ch]
            push 0FFh
            // Push our R,G,B components (inverse order)
            push ecx
            push edx
            push eax
            // Return control
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1
        }
    }
}

void _declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue ( )
{
    _asm
    {
        // Grab our siren position vector
        lea eax, [esp+130h]
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, eax
    }

    // Call our main process siren function
    if ( ProcessVehicleSirenPosition ( ) )
    {
        _asm
        {
            popad
            // Push our position
            push eax
            // move our R,G,B components into registers
            mov ecx, dwBlue // Blue
            mov edx, dwGreen // Green
            mov ebp, dwRed // Red
            push 0FFh
            // Push our R,G,B components (inverse order)
            push ecx
            push edx
            push ebp
            // Return control
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2
        }
    }
    else
    {
        _asm
        {
            popad
            // Push our position
            push eax
            push 0FFh
            // Push our R,G,B components (inverse order)
            push ecx
            push edx
            push ebp
            // Return control
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2
        }
    }
}

bool TestVehicleForSiren ( )
{
    // Grab our vehicle
    CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
    // Is it valid and it doesn't have a siren by default
    if ( pVehicle && DoesVehicleHaveSiren ( ) == false )
    {
        // Return our stored siren state
        return pVehicle->DoesVehicleHaveSirens ( );
    }
    // Return true here for default vehicles
    return true;
}

void _declspec(naked) HOOK_CVehicle_DoesVehicleUseSiren ( )
{
    _asm
    {
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, ecx
    }
    // Test our vehicle for sirens
    if ( TestVehicleForSiren ( ) )
    {
        _asm
        {
            popad 
            // Move 1 into AL (true)
            mov al, 1
            // Return
            jmp RETN_CVehicleDoesVehicleUseSirenRetn
        }
    }
    else
    {
        _asm
        {
            popad
            // xor AL (false)
            xor al, al
            // Return
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