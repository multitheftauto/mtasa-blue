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
DWORD RETN_CVehicle_ProcessStuff_TestSirenTypeSingle          = 0x6AB36D;

#define HOOKPOS_CVehicle_ProcessStuff_SetSirenPositionSingle    0x6ABC51
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionSingle  = 0x6ABC64;

#define HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeDual         0x6AB382
DWORD RETN_CVehicle_ProcessStuff_TestSirenTypeDual            = 0x6AB389;

#define HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition1    0x6ABA47
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1   = 0x6ABA5E;

#define HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition2    0x6AB9F7
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2   = 0x6ABA07;

#define HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionBlue   0x6BD4C3
DWORD RETN_CMotorBike_ProcessStuff_PushSirenPositionDualBlue  = 0x6BD4DB;

#define HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionRed    0x6BD516
DWORD RETN_CMotorBike_ProcessStuff_PushSirenPositionDualRed   = 0x6BD52C;

#define HOOKPOS_CMotorbike_ProcessStuff_TestVehicleModel        0x6BD40F
DWORD RETN_CMotorbike_ProcessStuff_TestVehicleModel           = 0x6BD415;

#define HOOKPOS_CVehicle_DoesVehicleUseSiren                    0x6D8470
DWORD RETN_CVehicleDoesVehicleUseSirenRetn                    = 0x6D8497;

#define HOOKPOS_CVehicle_ProcessStuff_TestCameraPosition        0x6ABC17
DWORD RETN_CVehicle_ProcessStuff_TestCameraPosition           = 0x6ABC1C;
DWORD RETN_CVehicle_ProcessStuff_TestCameraPosition2          = 0x6ABC1E;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound1                 0x501FC2
DWORD RETN_CVehicleAudio_ProcessSirenSound1                   = 0x501FC7;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound2                0x502067
DWORD RETN_CVehicleAudio_ProcessSirenSound2                   = 0x50206C;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound3                0x5021AE
DWORD RETN_CVehicleAudio_ProcessSirenSound3                   = 0x5021B3;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound             0x4F62BB
DWORD RETN_CVehicleAudio_GetVehicleSirenType             =  0x4F62C1;

#define HOOKPOS_CVehicle_ProcessStuff_PushRGBPointLights        0x6AB7A5
DWORD RETN_CVehicle_ProcessStuff_PushRGBPointLights          =  0x6AB7D5;

#define HOOKPOS_CVehicle_ProcessStuff_StartPointLightCode       0x6AB722
DWORD RETN_CVehicle_ProcessStuff_StartPointLightCode         =  0x6AB729;
DWORD RETN_CVehicle_ProcessStuff_IgnorePointLightCode        =  0x6AB823;

void HOOK_CVehicle_ProcessStuff_TestSirenTypeSingle ( );
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle ( );
void HOOK_CVehicle_ProcessStuff_TestSirenTypeDual ( );
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed ( );
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue ( );
void HOOK_CVehicle_DoesVehicleUseSiren ( );
void HOOK_CVehicle_ProcessStuff_TestCameraPosition ( );
void HOOK_CVehicleAudio_ProcessSirenSound ( );
void HOOK_CVehicleAudio_ProcessSirenSound1 ( );
void HOOK_CVehicleAudio_ProcessSirenSound2 ( );
void HOOK_CVehicleAudio_ProcessSirenSound3 ( );
void HOOK_CMotorBike_ProcessStuff_PushSirenPositionBlue ( );
void HOOK_CMotorBike_ProcessStuff_PushSirenPositionRed ( );
void HOOK_CMotorBike_ProcessStuff_PushSirenPosition2 ( );
void HOOK_CMotorbike_ProcessStuff_TestVehicleModel ( );
void HOOK_CVehicle_ProcessStuff_PushRGBPointLights ( );
void HOOK_CVehicle_ProcessStuff_StartPointLightCode ( );

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
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_SetSirenPositionSingle, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle, 19 ); // mov before Push for the siren position (overhook so we can get RGBA)
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeDual, (DWORD)HOOK_CVehicle_ProcessStuff_TestSirenTypeDual, 7 ); // test siren type is dual for a jump
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition1, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed, 15 ); // mov before push for the siren position (overhook so we can get RGBA)
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition2, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue, 15 ); // mov before push for the siren position (overhook so we can get RGBA)
    HookInstall ( HOOKPOS_CVehicle_DoesVehicleUseSiren, (DWORD)HOOK_CVehicle_DoesVehicleUseSiren, 5 ); // Does vehicle have a siren
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_TestCameraPosition, (DWORD)HOOK_CVehicle_ProcessStuff_TestCameraPosition, 5 ); // Fix for single sirens being 360 degrees
    // Breaks Rear wheel rendering leave for now
    //HookInstall ( HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionBlue, (DWORD)HOOK_CMotorBike_ProcessStuff_PushSirenPositionBlue, 15 ); // mov before the push for the sien position (overhook so we can get RGBA)
    //HookInstall ( HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionRed, (DWORD)HOOK_CMotorBike_ProcessStuff_PushSirenPositionRed, 22 ); // mov before the push for the sien position (overhook so we can get RGBA)

    //HookInstall ( HOOKPOS_CMotorbike_ProcessStuff_TestVehicleModel, (DWORD)HOOK_CMotorbike_ProcessStuff_TestVehicleModel, 6 );
    //HookInstall ( HOOKPOS_CVehicle_ProcessStuff_PushRGBPointLights, (DWORD)HOOK_CVehicle_ProcessStuff_PushRGBPointLights, 48 );
    HookInstall ( HOOKPOS_CVehicle_ProcessStuff_StartPointLightCode, (DWORD)HOOK_CVehicle_ProcessStuff_StartPointLightCode, 5 );

    HookInstall ( HOOKPOS_CVehicleAudio_ProcessSirenSound1, (DWORD) HOOK_CVehicleAudio_ProcessSirenSound1, 5 );
    HookInstall ( HOOKPOS_CVehicleAudio_ProcessSirenSound2, (DWORD) HOOK_CVehicleAudio_ProcessSirenSound2, 5 );
    HookInstall ( HOOKPOS_CVehicleAudio_ProcessSirenSound3, (DWORD) HOOK_CVehicleAudio_ProcessSirenSound3, 5 );
    HookInstall ( HOOKPOS_CVehicleAudio_ProcessSirenSound, (DWORD) HOOK_CVehicleAudio_ProcessSirenSound, 6 );
}

void CMultiplayerSA::InitMemoryCopies_13 ( void )
{
    // Memory based fixes go here
    //MemSet ( (void*)0x6AB35A, 0x90, 12 ); // Ignore some retarded R* if statement that checks if the model is the buffalo and jumps the siren code even though it doesn't have a siren anyway

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
float fRed = 0.0f;
float fGreen = 0.0f;
float fBlue = 0.0f;
DWORD dwMinAlphaValue = 0x46;
DWORD dwSirenTypePostHook = 0;
bool bPointLights = false;
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
    if ( dwSirenType > 2 )
    {
        dwSirenType = 0;
    }
    if ( dwSirenType2 < 0 || dwSirenType2 > 5 )
    {
        dwSirenType2 = 5;
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

void SetupSirenColour ( CVehicle * pVehicle )
{
    // Set our time based alpha to 10% of the current time float
    fTime = *((float*)0xB7C4E4) * 0.1f;
    // Get our minimum alpha
    float fMinimumAlpha = pVehicle->GetVehicleSirenMinimumAlpha ( ucSirenCount );
    // Get our Siren RGB Colour
    SColor tSirenColour = pVehicle->GetVehicleSirenColour ( ucSirenCount );
    // Times the alpha (255) by a multiplier to get it 0.0f-1.0f this multiplier was gained by doing 1.0 / 255.
    float fMaximumAlpha =  tSirenColour.A * 0.003921568627451f;
    // if our time is less than or equal to the minimum alpha
    if ( fTime <= fMinimumAlpha )
    {
        // Set it to the minimum
        fTime = fMinimumAlpha;
    }
    if ( fTime >= fMaximumAlpha )
    {
        fTime = fMaximumAlpha;
    }
    if ( bPointLights == false )
    {
        // times the R,G and B components by the fTime variable ( to get our time based brightness )
        dwRed = (DWORD)( tSirenColour.R * fTime );
        dwGreen = (DWORD)( tSirenColour.G * fTime );
        dwBlue = (DWORD)( tSirenColour.B * fTime );
        //pVehicle->SetPointLightColour ( tSirenColour );
    }
    else
    {
        SColor tSirenColour = pVehicle->GetPointLightColour ( );
        // times the R,G and B components by the fTime variable ( to get our time based brightness )
        dwRed = 0x3DCCCCCD;
        dwGreen = 0x3DCCCCCD;
        dwBlue = 0x3D4CCCCD;
    }
}

bool ProcessVehicleSirenPosition ( )
{
    // Valid interface
    if ( pVehicleWithTheSiren )
    {
        // Grab our vehicle from the interface
        CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
        // Valid - Wait this seems familiar
        if ( pVehicle )
        {    
            // Disable our original siren based vehicles from this hook
            if ( DoesVehicleHaveSiren ( ) && pVehicle->DoesVehicleHaveSirens ( ) == false )
            {
                bPointLights = false;
                // return false so our hook knows we decided not to edit anything
                return false;
            }
            // Does the vehicle have sirens and is the siren count greater than 0 
            if ( pVehicle->DoesVehicleHaveSirens ( ) && pVehicle->GetVehicleSirenCount ( ) >= 0 )
            {
                // Get our siren count
                unsigned char ucVehicleSirenCount = pVehicle->GetVehicleSirenCount ( );
                // Get our current Siren ID
                ucSirenCount = pVehicle->GetVehicleCurrentSirenID ( );

                // Get our randomiser
                ucRandomiser = pVehicle->GetSirenRandomiser ( );

                if ( pVehicle->IsSirenRandomiserEnabled ( ) )
                {
                    // Make sure we aren't beyond our limit
                    if ( ucSirenCount > ucVehicleSirenCount )
                    {
                        // if we have more than 1 sirens
                        if ( ucVehicleSirenCount > 0 )
                            // Set our Randomiser
                            ucRandomiser = rand () % ucVehicleSirenCount;
                        else
                            // Set our Randomiser
                            ucRandomiser = 0;

                        if ( bPointLights == false )
                        {
                            // Update our stored Randomiser
                            pVehicle->SetSirenRandomiser ( ucRandomiser );
                        }
                    }
                    else
                    {
                        // Set our Randomiser
                        ucRandomiser = rand () % ucVehicleSirenCount;
                        if ( bPointLights == false )
                        {
                            // Update our stored Randomiser
                            pVehicle->SetSirenRandomiser ( ucRandomiser );
                        }
                    }
                }
                else
                {
                    ucRandomiser++;
                    if ( ucRandomiser >= ucVehicleSirenCount )
                    {
                        ucRandomiser = 0;
                    }
                    if ( bPointLights == false )
                    {
                        // Update our stored Randomiser
                        pVehicle->SetSirenRandomiser ( ucRandomiser );
                    }
                }
                ucSirenCount = ucRandomiser;

                if ( bPointLights == false )
                {
                    // Gete our siren position for this siren count
                    pVehicle->GetVehicleSirenPosition ( ucSirenCount, *vecRelativeSirenPosition );
                }

                if ( bPointLights == false )
                {
                    // Are we skipping LOS Checks?
                    if ( pVehicle->IsSirenLOSCheckEnabled ( ) )
                    {
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
                        if ( pGameInterface->GetWorld()->IsLineOfSightClear ( &matCamera.vPos, &vecSirenPosition, flags ) == false )
                        {
                            // Nope? Invisible
                            dwRed = 0;
                            dwGreen = 0;
                            dwBlue = 0;
                        }
                        else
                        {
                            // Yep?
                            SetupSirenColour ( pVehicle );
                        }
                    }
                    else
                    {
                        // Skip LOS Checks.
                        SetupSirenColour ( pVehicle ); 
                    }
                    // Set our current Siren ID after we increment it
                    pVehicle->SetVehicleCurrentSirenID ( ++ucSirenCount );
                }
                bPointLights = false;
                // Return true
                return true;
            }
        }
    }
    bPointLights = false;
    // Return false
    return false;
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
    bPointLights = false;
    // Call our main siren Process function
    if ( ProcessVehicleSirenPosition ( ) )
    {
        _asm
        {
            popad
            // push our siren position
            push edx
            push 0FFh
            mov eax, dwBlue
            push eax
            mov eax, dwGreen
            push eax
            mov eax, dwRed
            push eax
            
            // return back to SA
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionSingle
        }
    }
    else
    {
        _asm
        {
            popad
            // push our siren position
            push edx
            push 0FFh
            push 0FFh
            push 0
            push 0
            // return back to SA
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionSingle
        }
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
    bPointLights = false;

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
    bPointLights = false;

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
    if ( pVehicle )
    {
        // Return our stored siren state
        return pVehicle->DoesVehicleHaveSirens ( ) || DoesVehicleHaveSiren ( );
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
bool SirenCheckCameraPosition ( )
{
    // Default SA sirens we don't bother processing
    //if ( DoesVehicleHaveSiren ( ) == false )
    {
        CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
        // Do we have sirens given by us and is the 360 flag set?
        if ( pVehicle->DoesVehicleHaveSirens ( ) && pVehicle->IsSiren360EffectEnabled ( ) )
        {
            // Do 360 code
            return true;
        }
    }
    // Only visible from the front
    return false;
}
void _declspec(naked) HOOK_CVehicle_ProcessStuff_TestCameraPosition ( )
{
    _asm
    {
        pushad
        // Grab our vehicle
        mov pVehicleWithTheSiren, esi
    }
    // Check if we disable or enable the 360 effect
    if ( SirenCheckCameraPosition ( ) )
    {
        _asm
        {
            popad
            // 360 effect
            // Carry on
            jmp RETN_CVehicle_ProcessStuff_TestCameraPosition
        }
    }
    else
    {

        _asm
        {
            popad
            // 180 effect
            // Replaced code
            fnstsw ax
            test ah, 5
            // Carry on
            jmp RETN_CVehicle_ProcessStuff_TestCameraPosition2
        }
    }
}
bool DisableVehicleSiren ( )
{
    if ( pVehicleWithTheSiren && pVehicleWithTheSiren->vtbl != NULL )
    {
        CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleWithTheSiren );
        if ( pVehicle && pVehicle->IsSirenSilentEffectEnabled ( ) )
        {
            return true;
        }
    }
    return false;
}
void _declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound ( )
{
    _asm
    {
        pushad
    }
    if ( DisableVehicleSiren ( ) )
    {
        _asm
        {
            popad
            mov dl, 0
            jmp RETN_CVehicleAudio_GetVehicleSirenType
        }
    }
    else
    {
        _asm
        {
            popad
            mov dl, [ecx+42Dh]
            jmp RETN_CVehicleAudio_GetVehicleSirenType
        }
    }
}
DWORD CALL_CVehicleAudio_ProcessCarHorn = 0x5002C0;
void _declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound1 ( )
{
    _asm
    {
        mov pVehicleWithTheSiren, edi
        pushad
    }

    _asm
    {
        popad
        call CALL_CVehicleAudio_ProcessCarHorn
        jmp RETN_CVehicleAudio_ProcessSirenSound1
    }
}void _declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound2 ( )
{
    _asm
    {
        mov pVehicleWithTheSiren, edi
        pushad
    }

    _asm
    {
        popad
        call CALL_CVehicleAudio_ProcessCarHorn
        jmp RETN_CVehicleAudio_ProcessSirenSound2
    }
}
void _declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound3 ( )
{
    _asm
    {
        mov pVehicleWithTheSiren, edi
        pushad
    }

    _asm
    {
        popad
        call CALL_CVehicleAudio_ProcessCarHorn
        jmp RETN_CVehicleAudio_ProcessSirenSound3
    }
}

DWORD RETN_CMotorbike_ProcessStuff_PostPushSirenPositionDual1 = 0x6BD4DB;

void _declspec(naked) HOOK_CMotorBike_ProcessStuff_PushSirenPositionBlue ( )
{ 
    _asm
    {
        // Grab our siren position vector
        lea eax,[esp+0ECh]
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, eax
    }

    bPointLights = false;
    // Call our main process siren function
    if ( ProcessVehicleSirenPosition ( ) )
    {
        _asm
        {
            popad
            // Push our position
            push eax
            // move our R,G,B components into registers then push them
            push 0FFh
            mov ebp, dwBlue
            push ebp
            mov ebp, dwGreen
            push ebp
            mov eax, dwRed
            lea ecx, [esi+15h]
            push eax
            // Return control
            JMP RETN_CMotorbike_ProcessStuff_PostPushSirenPositionDual1
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
            push ebp
            push ebp
            lea ecx, [esi+15h]
            push eax
            // Return control
            JMP RETN_CMotorbike_ProcessStuff_PostPushSirenPositionDual1
        }
    }
}

void _declspec(naked) HOOK_CMotorBike_ProcessStuff_PushSirenPositionRed ( )
{ 
    _asm
    {
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, edx
    }

    bPointLights = false;
    // Call our main process siren function
    if ( ProcessVehicleSirenPosition ( ) )
    {
        _asm
        {
            popad
            // Push our position
            push edx
            // Grab our siren position vector
            mov [esp+4Fh], al
            sub bl, al
            push 0FFh
            // move our R,G,B components into registers then push them ( Use eax because it's reset after )
            mov eax, dwBlue
            push eax
            mov eax, dwGreen
            push eax
            mov eax, dwRed
            push eax
            // Return control
            JMP RETN_CMotorBike_ProcessStuff_PushSirenPositionDualRed
        }
    }
    else
    {
        _asm
        {
            popad
            // Push our position
            push edx
            mov [esp+4Fh], al
            sub bl, al
            push 0FFh
            // Push our R,G,B components (inverse order)
            push eax
            push 0
            push 0
            // Return control
            JMP RETN_CMotorBike_ProcessStuff_PushSirenPositionDualRed
        }
    }
}
DWORD RETN_CMotorbike_ProcessStuff_TestVehicleModel2 = 0x6BD41B;
void _declspec(naked) HOOK_CMotorbike_ProcessStuff_TestVehicleModel ( )
{
    _asm
    {
        pushad
        mov pVehicleWithTheSiren, esi
    }
    if ( TestVehicleForSiren ( ) )
    {
        _asm
        {
            popad
            cmp word ptr [esi+22h], 20Bh
            jmp RETN_CMotorbike_ProcessStuff_TestVehicleModel2
        }
    }
    else
    {
        _asm
        {
            popad
            cmp word ptr [esi+22h], 20Bh
            jmp RETN_CMotorbike_ProcessStuff_TestVehicleModel
        }
    }
}
DWORD dwValue = 0x858B4C;
void _declspec(naked) HOOK_CVehicle_ProcessStuff_PushRGBPointLights ( )
{
    _asm
    {
        pushad
        mov esi, pVehicleWithTheSiren
    }
    bPointLights = true;
    if ( ProcessVehicleSirenPosition ( ) )
    {
        _asm
        {
            popad
            mov     edx, dwBlue
            mov     eax, dwGreen
            mov     ecx, dwRed
            mov     [esp+30h], edx
            fild    dword ptr [esp+30h]
            mov     [esp+30h], eax
            mov     eax, [esp+44h]
            fmul    dword ptr ds:[0858B4Ch]
            fstp    dword ptr [esp+8]
            fild    dword ptr [esp+30h]
            mov     [esp+30h], ecx
            mov     ecx, [esp+48h]
            fmul    dword ptr ds:[0858B4Ch]
            fstp    dword ptr [esp+4]
            fild    dword ptr [esp+30h]
            JMP RETN_CVehicle_ProcessStuff_PushRGBPointLights
        }
    }
    else
    {
        _asm
        {
            popad
            fild    dword ptr [esp+30h]
            mov     [esp+30h], eax
            mov     eax, [esp+44h]
            fmul    dword ptr ds:[0858B4Ch]
            fstp    dword ptr [esp+8]
            fild    dword ptr [esp+30h]
            mov     [esp+30h], ecx
            mov     ecx, [esp+48h]
            fmul    dword ptr ds:[0858B4Ch]
            fstp    dword ptr [esp+4]
            fild    dword ptr [esp+30h]
            JMP RETN_CVehicle_ProcessStuff_PushRGBPointLights
        }
    }
}

void _declspec(naked) HOOK_CVehicle_ProcessStuff_StartPointLightCode ( )
{
    _asm
    {
        pushad
        mov pVehicleWithTheSiren, esi
    }
    if ( DoesVehicleHaveSiren ( ) )
    {
        _asm
        {
            popad
            fld dword ptr [eax+28h]
            mov edx, [ecx]
            fadd st, st
            jmp RETN_CVehicle_ProcessStuff_StartPointLightCode
        }
    }
    else
    {
        _asm
        {
            popad
            movzx   eax, byte ptr [esp+88h]
            mov     [esp+50h], eax
            jmp RETN_CVehicle_ProcessStuff_IgnorePointLightCode
        }
    }
}
// Water Cannon Stuff

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