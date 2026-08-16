/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleSirens.cpp
 *  PURPOSE:     Custom vehicle siren hooks (addVehicleSirens / setVehicleSirens support)
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CWorld.h>

#define HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeSingle 0x6AB366
DWORD RETN_CVehicle_ProcessStuff_TestSirenTypeSingle = 0x6AB36D;

// GTA's CAutomobile::PreRender only renders siren coronas for models 407-599
// (gate at 0x6AB350 skips when (modelIndex - 407) > 0xC0). This hook lets custom
// (engineRequestModel) and out-of-range stock vehicles with scripted sirens render too.
#define HOOKPOS_CAutomobile_PreRender_SirenModelGate 0x6AB350
DWORD RETN_CAutomobile_PreRender_SirenRender = 0x6AB366;  // continue into the siren type dispatch
DWORD RETN_CAutomobile_PreRender_SirenSkip = 0x6ABC71;    // original "no siren" path

// Bikes, BMX, boats, planes and helis don't draw scripted sirens natively. Each of these PreRender
// methods starts with the same 13-byte SEH prologue (push -1; push <handler>; mov eax, fs:[0]); we
// hook the entry, draw our sirens, then replay it and continue at entry+0xD so the body is untouched.
#define HOOKPOS_CBike_PreRender_Siren  0x6BD090
#define HOOKPOS_CBmx_PreRender_Siren   0x6C0810
#define HOOKPOS_CBoat_PreRender_Siren  0x6F1180
#define HOOKPOS_CPlane_PreRender_Siren 0x6C94A0
#define HOOKPOS_CHeli_PreRender_Siren  0x6C5420
DWORD RETN_CBike_PreRender_Siren = 0x6BD09D;
DWORD RETN_CBmx_PreRender_Siren = 0x6C081D;
DWORD RETN_CBoat_PreRender_Siren = 0x6F118D;
DWORD RETN_CPlane_PreRender_Siren = 0x6C94AD;
DWORD RETN_CHeli_PreRender_Siren = 0x6C542D;

// CTrain::PreRender (0x6F5570) overrides CVehicle::PreRender but has a plain prologue
// (push esi; mov esi, ecx; call CVehicle::PreRender), so it needs its own entry hook.
#define HOOKPOS_CTrain_PreRender_Siren 0x6F5570
DWORD CALL_CVehicle_PreRender = 0x6D6480;
DWORD RETN_CTrain_PreRender_Siren = 0x6F5578;

#define HOOKPOS_CVehicle_ProcessStuff_SetSirenPositionSingle 0x6ABC51
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionSingle = 0x6ABC64;

#define HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeDual 0x6AB382
DWORD RETN_CVehicle_ProcessStuff_TestSirenTypeDual = 0x6AB389;

#define HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition1 0x6ABA47
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1 = 0x6ABA5E;

#define HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition2 0x6AB9F7
DWORD RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2 = 0x6ABA07;

#define HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionBlue 0x6BD4C3
DWORD RETN_CMotorBike_ProcessStuff_PushSirenPositionDualBlue = 0x6BD4DB;

#define HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionRed 0x6BD516
DWORD RETN_CMotorBike_ProcessStuff_PushSirenPositionDualRed = 0x6BD52C;

#define HOOKPOS_CMotorbike_ProcessStuff_TestVehicleModel 0x6BD40F
DWORD RETN_CMotorbike_ProcessStuff_TestVehicleModel = 0x6BD415;

#define HOOKPOS_CVehicle_DoesVehicleUseSiren 0x6D8470
DWORD RETN_CVehicleDoesVehicleUseSirenRetn = 0x6D8497;

#define HOOKPOS_CVehicle_ProcessStuff_TestCameraPosition 0x6ABC17
DWORD RETN_CVehicle_ProcessStuff_TestCameraPosition = 0x6ABC1C;
DWORD RETN_CVehicle_ProcessStuff_TestCameraPosition2 = 0x6ABC1E;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound1 0x501FC2
DWORD RETN_CVehicleAudio_ProcessSirenSound1 = 0x501FC7;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound2 0x502067
DWORD RETN_CVehicleAudio_ProcessSirenSound2 = 0x50206C;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound3 0x5021AE
DWORD RETN_CVehicleAudio_ProcessSirenSound3 = 0x5021B3;

#define HOOKPOS_CVehicleAudio_ProcessSirenSound 0x4F62BB
DWORD RETN_CVehicleAudio_GetVehicleSirenType = 0x4F62C1;

// CAEVehicleAudioEntity::ProcessVehicle only runs the siren sound for car/bike/bmx audio types. Hook
// the switch dispatch so boats, aircraft and trains with scripted sirens also play it.
#define HOOKPOS_CAEVehicleAudioEntity_ProcessVehicle_SirenSound 0x501EEB

#define HOOKPOS_CVehicle_ProcessStuff_PushRGBPointLights 0x6AB7A5
DWORD RETN_CVehicle_ProcessStuff_PushRGBPointLights = 0x6AB7D5;

#define HOOKPOS_CVehicle_ProcessStuff_StartPointLightCode 0x6AB722
DWORD RETN_CVehicle_ProcessStuff_StartPointLightCode = 0x6AB729;
DWORD RETN_CVehicle_ProcessStuff_IgnorePointLightCode = 0x6AB823;

void HOOK_CVehicle_ProcessStuff_TestSirenTypeSingle();
void HOOK_CAutomobile_PreRender_SirenModelGate();
void HOOK_CBike_PreRender_Siren();
void HOOK_CBmx_PreRender_Siren();
void HOOK_CBoat_PreRender_Siren();
void HOOK_CPlane_PreRender_Siren();
void HOOK_CHeli_PreRender_Siren();
void HOOK_CTrain_PreRender_Siren();
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle();
void HOOK_CVehicle_ProcessStuff_TestSirenTypeDual();
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed();
void HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue();
void HOOK_CVehicle_DoesVehicleUseSiren();
void HOOK_CVehicle_ProcessStuff_TestCameraPosition();
void HOOK_CVehicleAudio_ProcessSirenSound();
void HOOK_CAEVehicleAudioEntity_ProcessVehicle_SirenSound();
void HOOK_CVehicleAudio_ProcessSirenSound1();
void HOOK_CVehicleAudio_ProcessSirenSound2();
void HOOK_CVehicleAudio_ProcessSirenSound3();
void HOOK_CMotorBike_ProcessStuff_PushSirenPositionBlue();
void HOOK_CMotorBike_ProcessStuff_PushSirenPositionRed();
void HOOK_CMotorbike_ProcessStuff_TestVehicleModel();
void HOOK_CVehicle_ProcessStuff_PushRGBPointLights();
void HOOK_CVehicle_ProcessStuff_StartPointLightCode();

void CMultiplayerSA::InitHooks_VehicleSirens()
{
    HookInstall(HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeSingle, (DWORD)HOOK_CVehicle_ProcessStuff_TestSirenTypeSingle,
                7);  // Test siren type is single for a jump
    HookInstall(HOOKPOS_CAutomobile_PreRender_SirenModelGate, (DWORD)HOOK_CAutomobile_PreRender_SirenModelGate,
                22);  // model-id siren gate (movsx+lea+cmp+ja) - allow scripted sirens on any model
    // Draw scripted sirens for vehicle classes GTA never renders them for (bikes, BMX, boats, planes, helis, trains)
    HookInstall(HOOKPOS_CBike_PreRender_Siren, (DWORD)HOOK_CBike_PreRender_Siren, 13);
    HookInstall(HOOKPOS_CBmx_PreRender_Siren, (DWORD)HOOK_CBmx_PreRender_Siren, 13);
    HookInstall(HOOKPOS_CBoat_PreRender_Siren, (DWORD)HOOK_CBoat_PreRender_Siren, 13);
    HookInstall(HOOKPOS_CPlane_PreRender_Siren, (DWORD)HOOK_CPlane_PreRender_Siren, 13);
    HookInstall(HOOKPOS_CHeli_PreRender_Siren, (DWORD)HOOK_CHeli_PreRender_Siren, 13);
    HookInstall(HOOKPOS_CTrain_PreRender_Siren, (DWORD)HOOK_CTrain_PreRender_Siren, 8);
    HookInstall(HOOKPOS_CVehicle_ProcessStuff_SetSirenPositionSingle, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle,
                19);  // mov before Push for the siren position (overhook so we can get RGBA)
    HookInstall(HOOKPOS_CVehicle_ProcessStuff_TestSirenTypeDual, (DWORD)HOOK_CVehicle_ProcessStuff_TestSirenTypeDual,
                7);  // test siren type is dual for a jump
    HookInstall(HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition1, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed,
                15);  // mov before push for the siren position (overhook so we can get RGBA)
    HookInstall(HOOKPOS_CVehicle_ProcessStuff_PostPushSirenPosition2, (DWORD)HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue,
                15);  // mov before push for the siren position (overhook so we can get RGBA)
    HookInstall(HOOKPOS_CVehicle_DoesVehicleUseSiren, (DWORD)HOOK_CVehicle_DoesVehicleUseSiren, 5);  // Does vehicle have a siren
    HookInstall(HOOKPOS_CVehicle_ProcessStuff_TestCameraPosition, (DWORD)HOOK_CVehicle_ProcessStuff_TestCameraPosition,
                5);  // Fix for single sirens being 360 degrees
    // Breaks Rear wheel rendering leave for now
    // HookInstall ( HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionBlue, (DWORD)HOOK_CMotorBike_ProcessStuff_PushSirenPositionBlue, 15 ); // mov before the
    // push for the sien position (overhook so we can get RGBA) HookInstall ( HOOKPOS_CMotorBike_ProcessStuff_PushSirenPositionRed,
    // (DWORD)HOOK_CMotorBike_ProcessStuff_PushSirenPositionRed, 22 ); // mov before the push for the sien position (overhook so we can get RGBA)

    // HookInstall ( HOOKPOS_CMotorbike_ProcessStuff_TestVehicleModel, (DWORD)HOOK_CMotorbike_ProcessStuff_TestVehicleModel, 6 );
    // HookInstall ( HOOKPOS_CVehicle_ProcessStuff_PushRGBPointLights, (DWORD)HOOK_CVehicle_ProcessStuff_PushRGBPointLights, 48 );
    HookInstall(HOOKPOS_CVehicle_ProcessStuff_StartPointLightCode, (DWORD)HOOK_CVehicle_ProcessStuff_StartPointLightCode, 5);

    HookInstall(HOOKPOS_CVehicleAudio_ProcessSirenSound1, (DWORD)HOOK_CVehicleAudio_ProcessSirenSound1, 5);
    HookInstall(HOOKPOS_CVehicleAudio_ProcessSirenSound2, (DWORD)HOOK_CVehicleAudio_ProcessSirenSound2, 5);
    HookInstall(HOOKPOS_CVehicleAudio_ProcessSirenSound3, (DWORD)HOOK_CVehicleAudio_ProcessSirenSound3, 5);
    HookInstall(HOOKPOS_CVehicleAudio_ProcessSirenSound, (DWORD)HOOK_CVehicleAudio_ProcessSirenSound, 6);
    HookInstall(HOOKPOS_CAEVehicleAudioEntity_ProcessVehicle_SirenSound, (DWORD)HOOK_CAEVehicleAudioEntity_ProcessVehicle_SirenSound,
                7);  // boats/aircraft/trains: process the siren sound too
}

// Siren Stuff
CVector* vecRelativeSirenPosition;

unsigned char ucSirenCount = 0;
unsigned char ucRandomiser = 0;

CVehicleSAInterface* pVehicleWithTheSiren = NULL;

float fTime = *((float*)0xB7C4E4);

DWORD dwSirenType = 0;
DWORD dwSirenType2 = 3;
DWORD dwRed = 0;
DWORD dwGreen = 0;
DWORD dwBlue = 0;
DWORD dwSirenTypePostHook = 0;
bool  bPointLights = false;
bool  DoesVehicleHaveSiren()
{
    // Static function to check if the vehicle has sirens natively if so we ignore those for now
    return (pVehicleWithTheSiren->m_nModelIndex == 596 || pVehicleWithTheSiren->m_nModelIndex == 597 || pVehicleWithTheSiren->m_nModelIndex == 598 ||
            pVehicleWithTheSiren->m_nModelIndex == 599 ||

            pVehicleWithTheSiren->m_nModelIndex == 490 || pVehicleWithTheSiren->m_nModelIndex == 601 || pVehicleWithTheSiren->m_nModelIndex == 528 ||
            pVehicleWithTheSiren->m_nModelIndex == 407 || pVehicleWithTheSiren->m_nModelIndex == 416 || pVehicleWithTheSiren->m_nModelIndex == 433 ||
            pVehicleWithTheSiren->m_nModelIndex == 427 || pVehicleWithTheSiren->m_nModelIndex == 544 || pVehicleWithTheSiren->m_nModelIndex == 523 ||
            pVehicleWithTheSiren->m_nModelIndex == 432 || pVehicleWithTheSiren->m_nModelIndex == 423 || pVehicleWithTheSiren->m_nModelIndex == 420 ||
            pVehicleWithTheSiren->m_nModelIndex == 438);
}
void GetVehicleSirenType()
{
    if (DoesVehicleHaveSiren())
    {
        // QUICK RUN
        return;
    }
    // Valid?
    if (pVehicleWithTheSiren)
    {
        // Grab the CVehicle
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleWithTheSiren);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        // Valid ? I see a pattern here!
        if (pVehicle)
        {
            // Get the vehicles siren type (dual or single and the default colours such as red/yellow for fire truck or red/blue for the police car)
            // in SA the siren type also defines position so we totally ignore that.
            DWORD dwVehicleSirenType = pVehicle->GetVehicleSirenType();
            // siren type's 1 and 2 are single
            if (dwVehicleSirenType == 1 || dwVehicleSirenType == 2)
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
    if (dwSirenType > 2)
    {
        dwSirenType = 0;
    }
    if (dwSirenType2 < 0 || dwSirenType2 > 5)
    {
        dwSirenType2 = 5;
    }
}

// Resolves a native vehicle interface to its CVehicle wrapper, but only when that wrapper actually
// carries scripted sirens. Shared by the model gate, the custom render and the custom sound below,
// which otherwise each repeated the same pool lookup and null/sirens check on their own.
CVehicle* GetSirenVehicle(CVehicleSAInterface* vehicleInterface)
{
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicleInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    return (pVehicle && pVehicle->DoesVehicleHaveSirens()) ? pVehicle : nullptr;
}

// Index used for the native siren-type table reads when a vehicle is outside the stock
// 407-599 range. 596 (police) minus 407; its value is discarded by the TestSirenType* override
// hooks, so any in-range index works, it only keeps the table reads in bounds.
#define SIREN_SAFE_MODEL_INDEX_ADJ 189

// modelIndex - 407, so it goes negative for the stock 400-406 range; that is why this stays a
// signed int rather than unsigned. The unsigned cast below is only a wraparound trick that turns
// the two-sided range check into a single comparison.
int g_iSirenModelIndexAdj = 0;

// Decide whether the siren block in CAutomobile::PreRender should run for this
// vehicle. Stock models 407-599 keep the original behaviour; anything else (custom
// engineRequestModel ids, stock 400-406 / 600-611) renders only when it has scripted sirens.
bool ProcessCustomSirenModelGate()
{
    // Native siren range: behave exactly like the original code (keep edi = modelIndex - 407).
    if (static_cast<unsigned int>(g_iSirenModelIndexAdj) <= 0xC0)
        return true;

    // Out of native range: only render when the vehicle has scripted sirens, and clamp the
    // index so the native siren-type table reads stay in bounds.
    if (!GetSirenVehicle(pVehicleWithTheSiren))
        return false;

    g_iSirenModelIndexAdj = SIREN_SAFE_MODEL_INDEX_ADJ;
    return true;
}

static void __declspec(naked) HOOK_CAutomobile_PreRender_SirenModelGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Replicate the original index calculation: eax = modelIndex - 407
        movsx eax, word ptr [esi+0x22]
        sub   eax, 0x197
        mov   g_iSirenModelIndexAdj, eax
        mov   pVehicleWithTheSiren, esi
    }
    // clang-format on
    // The bool comes back in al, and nothing between the call and the test below touches eax.
    // esi/ebx/edi/ebp are untouched by any ordinary call under our calling convention, so there is
    // nothing left here that a pushad would still need to protect.
    ProcessCustomSirenModelGate();
    // clang-format off
    __asm
    {
        test  al, al
        je    skip_sirens
        // edi = (possibly clamped) modelIndex - 407, consumed by the native siren-type table reads
        mov   edi, g_iSirenModelIndexAdj
        jmp   RETN_CAutomobile_PreRender_SirenRender
    skip_sirens:
        jmp   RETN_CAutomobile_PreRender_SirenSkip
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CVehicle_ProcessStuff_TestSirenTypeSingle()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        // Grab our original siren type
        movzx edx, byte ptr [edi+6ACCD0h]
        // Put it into dwSirenType
        mov dwSirenType, edx
        // Grab our siren vehicle
        mov pVehicleWithTheSiren, esi
    }
    // clang-format on
    //   Call our Get siren type function which edits dwSirenType to our desired type
    GetVehicleSirenType();
    // clang-format off
    __asm
    {
        popad
        // put our new siren type into edx or old one for default/no sirens
        mov edx, dwSirenType
        // Jump back to the original code
        JMP RETN_CVehicle_ProcessStuff_TestSirenTypeSingle
    }
    // clang-format on
}

void SetupSirenColour(CVehicle* pVehicle)
{
    // Set our time based alpha to 10% of the current time float
    fTime = *((float*)0xB7C4E4) * 0.1f;
    // Get our minimum alpha
    DWORD dwMinimumAlpha = pVehicle->GetVehicleSirenMinimumAlpha(ucSirenCount);
    // Get our Siren RGB Colour
    SColor tSirenColour = pVehicle->GetVehicleSirenColour(ucSirenCount);
    // Times the alpha (255) by a multiplier to get it 0.0f-1.0f this multiplier was gained by doing 1.0 / 255.
    float fMaximumAlpha = tSirenColour.A * 0.003921568627451f;
    // Times the min alpha (255) by a multiplier to get it 0.0f-1.0f this multiplier was gained by doing 1.0 / 255.
    float fMinimumAlpha = dwMinimumAlpha * 0.003921568627451f;
    // if our time is less than or equal to the minimum alpha
    if (fTime <= dwMinimumAlpha)
    {
        // Set it to the minimum
        fTime = fMinimumAlpha;
    }
    if (fTime >= fMaximumAlpha)
    {
        fTime = fMaximumAlpha;
    }
    if (bPointLights == false)
    {
        // times the R,G and B components by the fTime variable ( to get our time based brightness )
        dwRed = (DWORD)(tSirenColour.R * fTime);
        dwGreen = (DWORD)(tSirenColour.G * fTime);
        dwBlue = (DWORD)(tSirenColour.B * fTime);
        // pVehicle->SetPointLightColour ( tSirenColour );
    }
    else
    {
        SColor tSirenColour = pVehicle->GetPointLightColour();
        // times the R,G and B components by the fTime variable ( to get our time based brightness )
        dwRed = 0x3DCCCCCD;
        dwGreen = 0x3DCCCCCD;
        dwBlue = 0x3D4CCCCD;
    }
}

bool ProcessVehicleSirenPosition()
{
    // Valid interface
    if (pVehicleWithTheSiren)
    {
        // Grab our vehicle from the interface
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleWithTheSiren);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        // Valid - Wait this seems familiar
        if (pVehicle)
        {
            // Disable our original siren based vehicles from this hook
            if (DoesVehicleHaveSiren() && pVehicle->DoesVehicleHaveSirens() == false)
            {
                ucSirenCount = 0;
                // Set our time based alpha to 10% of the current time float
                fTime = *((float*)0xB7C4E4) * 0.1f;
                // Get our minimum alpha
                DWORD dwMinimumAlpha = pVehicle->GetVehicleSirenMinimumAlpha(ucSirenCount);
                // Times the min alpha (255) by a multiplier to get it 0.0f-1.0f this multiplier was gained by doing 1.0 / 255.
                float fMinimumAlpha = dwMinimumAlpha * 0.003921568627451f;
                // if our time is less than or equal to the minimum alpha
                if (fTime <= dwMinimumAlpha)
                {
                    // Set it to the minimum
                    fTime = fMinimumAlpha;
                }
                // Set our red based on 255 and our blue based on 255 and green based on 0.. default SA values of course.
                /*if ( dwRed > 0 )
                    dwRed = (DWORD)( dwRed * fTime );
                if ( dwBlue > 0 )
                    dwBlue = (DWORD)( dwBlue * fTime );
                if ( dwGreen > 0 )
                    dwGreen = (DWORD)( dwGreen * fTime );*/
                bPointLights = false;
                // return false so our hook knows we decided not to edit anything
                return false;
            }
            // Does the vehicle have sirens and is the siren count greater than 0
            if (pVehicle->DoesVehicleHaveSirens() && pVehicle->GetVehicleSirenCount() > 0)
            {
                // Get our siren count
                unsigned char ucVehicleSirenCount = pVehicle->GetVehicleSirenCount();
                // Get our current Siren ID
                ucSirenCount = pVehicle->GetVehicleCurrentSirenID();

                // Get our randomiser
                ucRandomiser = pVehicle->GetSirenRandomiser();

                if (pVehicle->IsSirenRandomiserEnabled())
                {
                    // Make sure we aren't beyond our limit
                    if (ucSirenCount > ucVehicleSirenCount)
                    {
                        // if we have more than 1 sirens
                        if (ucVehicleSirenCount > 0)
                            // Set our Randomiser
                            ucRandomiser = rand() % ucVehicleSirenCount;
                        else
                            // Set our Randomiser
                            ucRandomiser = 0;

                        if (bPointLights == false)
                        {
                            // Update our stored Randomiser
                            pVehicle->SetSirenRandomiser(ucRandomiser);
                        }
                    }
                    else
                    {
                        // Set our Randomiser
                        ucRandomiser = rand() % ucVehicleSirenCount;
                        if (bPointLights == false)
                        {
                            // Update our stored Randomiser
                            pVehicle->SetSirenRandomiser(ucRandomiser);
                        }
                    }
                }
                else
                {
                    ucRandomiser++;
                    if (ucRandomiser >= ucVehicleSirenCount)
                    {
                        ucRandomiser = 0;
                    }
                    if (bPointLights == false)
                    {
                        // Update our stored Randomiser
                        pVehicle->SetSirenRandomiser(ucRandomiser);
                    }
                }
                ucSirenCount = ucRandomiser;

                if (bPointLights == false)
                {
                    // Gete our siren position for this siren count
                    pVehicle->GetVehicleSirenPosition(ucSirenCount, *vecRelativeSirenPosition);
                }

                if (bPointLights == false)
                {
                    // Are we skipping LOS Checks?
                    if (pVehicle->IsSirenLOSCheckEnabled())
                    {
                        // Storage 'n stuff
                        CMatrix matCamera;
                        CMatrix matVehicle;
                        // Grab our vehicle matrix
                        pVehicle->GetMatrix(&matVehicle);

                        // Get our Camera
                        CCamera* pCamera = pGameInterface->GetCamera();
                        // Get the Camera Matrix
                        pCamera->GetMatrix(&matCamera);

                        // Get our sirens ACTUAL position from the relative value
                        CVector vecSirenPosition = matVehicle.TransformVector(*vecRelativeSirenPosition);

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
                        pGameInterface->GetWorld()->IgnoreEntity(NULL);
                        // Variables 'n tings
                        CColPoint* pColPoint = NULL;
                        CEntity*   pGameEntity = NULL;
                        // Check if we can see it
                        if (pGameInterface->GetWorld()->IsLineOfSightClear(&matCamera.vPos, &vecSirenPosition, flags) == false)
                        {
                            // Nope? Invisible
                            dwRed = 0;
                            dwGreen = 0;
                            dwBlue = 0;
                        }
                        else
                        {
                            // Yep?
                            SetupSirenColour(pVehicle);
                        }
                    }
                    else
                    {
                        // Skip LOS Checks.
                        SetupSirenColour(pVehicle);
                    }
                    // Set our current Siren ID after we increment it
                    pVehicle->SetVehicleCurrentSirenID(++ucSirenCount);
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

// CCoronas::RegisterCorona (texture overload). With attachTo set, pos is treated as relative to the
// entity (the corona system transforms it by the entity matrix at render time).
using RegisterCorona_t = void(__cdecl*)(unsigned int id, void* attachTo, unsigned char r, unsigned char g, unsigned char b, unsigned char intensity,
                                        const CVector& pos, float size, float range, void* texture, int flareType, int reflType, int losCheck, int trail,
                                        float normalAngle, bool neonFade, float pullTowardsCam, bool fullBright, float fadeSpeed, bool onlyFromBelow,
                                        bool whiteCore);
#define FUNC_CCoronas_RegisterCorona 0x6FC580

// Draw the scripted sirens (addVehicleSirens) for a vehicle whose game class doesn't
// render sirens natively. Called from the PreRender entry hooks of CBike/CBmx/CBoat/CPlane/CHeli.
// Reuses ProcessVehicleSirenPosition() for the per-frame slot pick / randomiser / LOS / colour,
// then registers one corona attached to the vehicle (so the relative position is transformed for us).
//
// Takes the vehicle interface directly so its callers can call straight through with the this
// pointer already sitting in ecx. ProcessVehicleSirenPosition() and DoesVehicleHaveSiren() below
// still read it back through the shared pVehicleWithTheSiren global, so that still gets set here
// rather than separately at each call site.
void __fastcall RenderCustomVehicleSirens(CVehicleSAInterface* vehicleInterface)
{
    if (!vehicleInterface)
        return;
    pVehicleWithTheSiren = vehicleInterface;

    // The HPV1000 (523) draws its siren through the native bike code; don't double up.
    if (DoesVehicleHaveSiren())
        return;

    CVehicle* pVehicle = GetSirenVehicle(vehicleInterface);
    if (!pVehicle || pVehicle->GetVehicleSirenCount() == 0)
        return;

    // Pick the siren slot for this frame and fill in dwRed/dwGreen/dwBlue + the relative position.
    CVector vecRelative;
    vecRelativeSirenPosition = &vecRelative;
    bPointLights = false;
    dwRed = dwGreen = dwBlue = 0;
    if (!ProcessVehicleSirenPosition())
        return;

    // Nothing to draw this frame (off phase of the flash, or hidden by the line-of-sight check).
    if ((dwRed | dwGreen | dwBlue) == 0)
        return;

    // Match the native vehicle-siren corona look (size 0.4, far clip = corona brightness * 150).
    float fRange = *reinterpret_cast<float*>(0xB6F118) * 150.0f;
    reinterpret_cast<RegisterCorona_t>(FUNC_CCoronas_RegisterCorona)(
        reinterpret_cast<unsigned int>(vehicleInterface) + 0x15, vehicleInterface, static_cast<unsigned char>(dwRed), static_cast<unsigned char>(dwGreen),
        static_cast<unsigned char>(dwBlue), 0xFF, vecRelative, 0.4f, fRange, nullptr, 0, 0, 0, 0, 0.0f, false, 1.5f, false, 15.0f, false, true);
}

// Each of these five hooks saves every register, calls straight through with the vehicle interface
// (the this pointer) already sitting in ecx, restores everything, then replays the original SEH
// prologue (push -1; push <handler>; mov eax, fs:[0]) and continues at PreRender+0xD. ecx has to
// survive the call intact since the replayed prologue and the native PreRender body after it both
// still need it, which is why pushad/popad stay here even though the call itself needs no setup.
// The hooks only differ in the per-function SEH handler address and the return address.
static void __declspec(naked) HOOK_CBike_PreRender_Siren()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call RenderCustomVehicleSirens
        popad
        push 0FFFFFFFFh
        push 848321h
        mov  eax, dword ptr fs:[0]
        jmp  RETN_CBike_PreRender_Siren
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CBmx_PreRender_Siren()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call RenderCustomVehicleSirens
        popad
        push 0FFFFFFFFh
        push 848401h
        mov  eax, dword ptr fs:[0]
        jmp  RETN_CBmx_PreRender_Siren
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CBoat_PreRender_Siren()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call RenderCustomVehicleSirens
        popad
        push 0FFFFFFFFh
        push 848918h
        mov  eax, dword ptr fs:[0]
        jmp  RETN_CBoat_PreRender_Siren
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CPlane_PreRender_Siren()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call RenderCustomVehicleSirens
        popad
        push 0FFFFFFFFh
        push 8485ABh
        mov  eax, dword ptr fs:[0]
        jmp  RETN_CPlane_PreRender_Siren
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CHeli_PreRender_Siren()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call RenderCustomVehicleSirens
        popad
        push 0FFFFFFFFh
        push 848488h
        mov  eax, dword ptr fs:[0]
        jmp  RETN_CHeli_PreRender_Siren
    }
    // clang-format on
}

// CTrain::PreRender has a plain (non-SEH) prologue, so it gets its own hook that replays
// push esi; mov esi, ecx; call CVehicle::PreRender before continuing at PreRender+8. ecx (the
// vehicle interface) has to survive the call for that replay, same reasoning as the five hooks above.
static void __declspec(naked) HOOK_CTrain_PreRender_Siren()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call RenderCustomVehicleSirens
        popad
        push esi
        mov esi, ecx
        call CALL_CVehicle_PreRender
        jmp RETN_CTrain_PreRender_Siren
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionSingle()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Get our siren position into edx
        lea edx, [esp+64h]
        pushad
        // Grab our siren vehicle
        mov pVehicleWithTheSiren, esi
        // Put edx into our position variable
        mov vecRelativeSirenPosition, edx
    }
    // clang-format on
    bPointLights = false;
    // Call our main siren Process function
    if (ProcessVehicleSirenPosition())
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
}

void TestSirenTypeDualDefaultFix()
{
    // if we have a siren normally
    if (DoesVehicleHaveSiren())
    {
        // Set our siren type to the post hook value
        dwSirenType2 = dwSirenTypePostHook;
    }
    else
    {
        // Set our siren type to the stored value
        GetVehicleSirenType();
    }
}

static void __declspec(naked) HOOK_CVehicle_ProcessStuff_TestSirenTypeDual()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Grab our default siren type into edi
        movzx edi, byte ptr ds:[edi+06ACDACh]
        pushad
        // Store our Vehicle interface
        mov pVehicleWithTheSiren, esi
        // Store our post hook default siren type
        mov dwSirenTypePostHook, edi
    }
    // clang-format on
    //   Do our test and edit dwSirenType2 appropriately
    TestSirenTypeDualDefaultFix();
    // clang-format off
    __asm
    {
        popad
        // Move dwSirenType into edi
        mov edi, dwSirenType2
        // Return back to SA
        JMP RETN_CVehicle_ProcessStuff_TestSirenTypeDual
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualRed()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Grab our siren position vector
        lea eax, [esp+130h]
        mov ebp, [esp+8Ch]
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, eax
        mov dwRed, ebp
        mov dwGreen, edx
        mov dwBlue, ecx
    }
    // clang-format on
    bPointLights = false;

    // Call our main process siren function
    if (ProcessVehicleSirenPosition())
    {
        // clang-format off
        __asm
        {
            popad
            // Push our position
            push eax
            // move our R,G,B components into registers
            mov ebp, dwRed // Red
            mov edx, dwGreen // Green
            mov ecx, dwBlue // Blue
            push 0FFh
            // Push our R,G,B components (inverse order)
            push ecx
            push edx
            push ebp
            // Return control
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            // Push our position
            push eax
            // Edit our components to get Alpha fixing working.
            mov ebp, dwRed // Red
            mov edx, dwGreen // Green
            mov ecx, dwBlue // Blue
            push 0FFh
            // Push our R,G,B components (inverse order)
            push ecx
            push edx
            push ebp
            // Return control
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual1
        }
        // clang-format on
    }
}

static void __declspec(naked) HOOK_CVehicle_ProcessStuff_PostPushSirenPositionDualBlue()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Grab our siren position vector
        lea eax, [esp+130h]
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, eax
        mov dwRed, ebp
        mov dwGreen, edx
        mov dwBlue, ecx
    }
    // clang-format on
    bPointLights = false;

    // Call our main process siren function
    if (ProcessVehicleSirenPosition())
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            // Push our position
            push eax
            push 0FFh
            // Edit our components to get Alpha fixing working.
            mov ecx, dwBlue // Blue
            mov edx, dwGreen // Green
            mov ebp, dwRed // Red
            // Push our R,G,B components (inverse order)
            push ecx
            push edx
            push ebp
            // Return control
            JMP RETN_CVehicle_ProcessStuff_PostPushSirenPositionDual2
        }
        // clang-format on
    }
}

bool TestVehicleForSiren()
{
    if (pVehicleWithTheSiren)
    {
        // Grab our vehicle
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleWithTheSiren);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        // Is it valid and it doesn't have a siren by default
        if (pVehicle)
        {
            // Return our stored siren state
            return pVehicle->DoesVehicleHaveSirens() || DoesVehicleHaveSiren();
        }
    }
    // Return true here for default vehicles
    return true;
}

static void __declspec(naked) HOOK_CVehicle_DoesVehicleUseSiren()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, ecx
    }
    // clang-format on
    //   Test our vehicle for sirens
    if (TestVehicleForSiren())
    {
        // clang-format off
        __asm
        {
            popad
            // Move 1 into AL (true)
            mov al, 1
            // Return
            jmp RETN_CVehicleDoesVehicleUseSirenRetn
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            // xor AL (false)
            xor al, al
            // Return
            jmp RETN_CVehicleDoesVehicleUseSirenRetn
        }
        // clang-format on
    }
}
bool SirenCheckCameraPosition()
{
    // Default SA sirens we don't bother processing
    // if ( DoesVehicleHaveSiren ( ) == false )
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleWithTheSiren);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        if (pVehicle)
        {
            // Do we have sirens given by us and is the 360 flag set?
            if (pVehicle->DoesVehicleHaveSirens() && pVehicle->IsSiren360EffectEnabled())
            {
                // Do 360 code
                return true;
            }
        }
    }
    // Only visible from the front
    return false;
}

static void __declspec(naked) HOOK_CVehicle_ProcessStuff_TestCameraPosition()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        // Grab our vehicle
        mov pVehicleWithTheSiren, esi
    }
    // clang-format on
    //   Check if we disable or enable the 360 effect
    if (SirenCheckCameraPosition())
    {
        // clang-format off
        __asm
        {
            popad
            // 360 effect
            // Carry on
            jmp RETN_CVehicle_ProcessStuff_TestCameraPosition
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            // 180 effect
            // Replaced code
            fnstsw ax
            test ah, 5
            // Carry on
            jmp RETN_CVehicle_ProcessStuff_TestCameraPosition2
        }
        // clang-format on
    }
}
bool DisableVehicleSiren()
{
    if (pVehicleWithTheSiren && pVehicleWithTheSiren->HasVTBL())
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleWithTheSiren);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        if (pVehicle && (pVehicle->IsSirenSilentEffectEnabled() || pVehicle->GetModelIndex() == 420 || pVehicle->GetModelIndex() == 438))
        {
            return true;
        }
    }
    return false;
}

static void __declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on
    if (DisableVehicleSiren())
    {
        // clang-format off
        __asm
        {
            popad
            mov dl, 0
            jmp RETN_CVehicleAudio_GetVehicleSirenType
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            mov dl, [ecx+42Dh]
            jmp RETN_CVehicleAudio_GetVehicleSirenType
        }
        // clang-format on
    }
}
DWORD CALL_CVehicleAudio_ProcessCarHorn = 0x5002C0;

// CAEVehicleAudioEntity::ProcessVehicle only calls ProcessVehicleSirenAlarmHorn for the car/bike/bmx
// audio types, so boats/aircraft/trains never play the siren wail. Run it ourselves (once, before the
// type switch) for those audio types when the vehicle carries scripted sirens.
//
// Takes its three arguments straight from the registers/stack the hook below already has them in,
// rather than stashing each one into its own global first.
void __fastcall ProcessCustomSirenSound(CAEVehicleAudioEntitySAInterface* audioEntity, CVehicleSAInterface* vehicleInterface, void* vehicleParams)
{
    // m_nSettings.m_eVehicleSoundType: CAR/BIKE/BMX already handle the siren in the native switch.
    // BOAT through this enum's last entry, TRAILLER, get the extra processing here; TRAILLER is this
    // enum's name for what the native code and CAEVehicleAudioEntity.h's own eVehicleAudioType both
    // call SPECIAL. Native's out of range NO_VEHICLE, past this enum entirely, has no case either way.
    const VehicleSoundType audioType = audioEntity->m_nSettings.m_eVehicleSoundType;
    if (audioType < VehicleSoundType::BOAT || audioType > VehicleSoundType::TRAILLER)
        return;

    if (!GetSirenVehicle(vehicleInterface))
        return;

    audioEntity->ProcessVehicleSirenAlarmHorn(vehicleParams);
}

static void __declspec(naked) HOOK_CAEVehicleAudioEntity_ProcessVehicle_SirenSound()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ecx (VehicleAudioType, read by the jmp table below) has to survive the call intact, so
        // this saves every register first rather than push/popping just the three arguments.
        pushad
        mov  edx, edi          // 2nd arg: vehicle interface
        mov  ecx, esi          // 1st arg: this (CAEVehicleAudioEntity)
        lea  eax, [esp+2Ch]    // 3rd arg: &vp, esp+0Ch before pushad's own 0x20 bytes
        push eax
        call ProcessCustomSirenSound
        popad
        // original switch dispatch on ecx = VehicleAudioType
        jmp dword ptr [ecx*4 + 50224Ch]
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov pVehicleWithTheSiren, edi
        pushad
    }
    // clang-format on

    // clang-format off
    __asm
    {
        popad
        call CALL_CVehicleAudio_ProcessCarHorn
        jmp RETN_CVehicleAudio_ProcessSirenSound1
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov pVehicleWithTheSiren, edi
        pushad
    }
    // clang-format on

    // clang-format off
    __asm
    {
        popad
        call CALL_CVehicleAudio_ProcessCarHorn
        jmp RETN_CVehicleAudio_ProcessSirenSound2
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CVehicleAudio_ProcessSirenSound3()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov pVehicleWithTheSiren, edi
        pushad
    }
    // clang-format on

    // clang-format off
    __asm
    {
        popad
        call CALL_CVehicleAudio_ProcessCarHorn
        jmp RETN_CVehicleAudio_ProcessSirenSound3
    }
    // clang-format on
}

DWORD RETN_CMotorbike_ProcessStuff_PostPushSirenPositionDual1 = 0x6BD4DB;

static void __declspec(naked) HOOK_CMotorBike_ProcessStuff_PushSirenPositionBlue()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Grab our siren position vector
        lea eax,[esp+0ECh]
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, eax
    }
    // clang-format on

    bPointLights = false;
    // Call our main process siren function
    if (ProcessVehicleSirenPosition())
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
}

static void __declspec(naked) HOOK_CMotorBike_ProcessStuff_PushSirenPositionRed()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        // Grab our vehicle interface
        mov pVehicleWithTheSiren, esi
        // move our position vector pointer into our position variable
        mov vecRelativeSirenPosition, edx
    }
    // clang-format on

    bPointLights = false;
    // Call our main process siren function
    if (ProcessVehicleSirenPosition())
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
}
DWORD                         RETN_CMotorbike_ProcessStuff_TestVehicleModel2 = 0x6BD41B;
static void __declspec(naked) HOOK_CMotorbike_ProcessStuff_TestVehicleModel()
{
    // clang-format off
    __asm
    {
        pushad
        mov pVehicleWithTheSiren, esi
    }
    // clang-format on
    if (TestVehicleForSiren())
    {
        // clang-format off
        __asm
        {
            popad
            cmp word ptr [esi+22h], 20Bh
            jmp RETN_CMotorbike_ProcessStuff_TestVehicleModel2
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            cmp word ptr [esi+22h], 20Bh
            jmp RETN_CMotorbike_ProcessStuff_TestVehicleModel
        }
        // clang-format on
    }
}
DWORD                         dwValue = 0x858B4C;
static void __declspec(naked) HOOK_CVehicle_ProcessStuff_PushRGBPointLights()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        mov esi, pVehicleWithTheSiren
    }
    // clang-format on
    bPointLights = true;
    if (ProcessVehicleSirenPosition())
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
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
        // clang-format on
    }
}

static void __declspec(naked) HOOK_CVehicle_ProcessStuff_StartPointLightCode()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        mov pVehicleWithTheSiren, esi
    }
    // clang-format on
    if (DoesVehicleHaveSiren())
    {
        // clang-format off
        __asm
        {
            popad
            fld dword ptr [eax+28h]
            mov edx, [ecx]
            fadd st, st
            jmp RETN_CVehicle_ProcessStuff_StartPointLightCode
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            movzx   eax, byte ptr [esp+88h]
            mov     [esp+50h], eax
            jmp RETN_CVehicle_ProcessStuff_IgnorePointLightCode
        }
        // clang-format on
    }
}
