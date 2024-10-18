/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CSettingsSA.cpp
 *  PURPOSE:     Game settings
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include "CAudioEngineSA.h"
#include "CCoronasSA.h"
#include "CGameSA.h"
#include "CHudSA.h"
#include "CSettingsSA.h"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

static const float MOUSE_SENSITIVITY_MIN = 0.000312f;
static const float MOUSE_SENSITIVITY_DEFAULT = 0.0025f;
static const float MOUSE_SENSITIVITY_MAX = MOUSE_SENSITIVITY_DEFAULT * 2 - MOUSE_SENSITIVITY_MIN;

#define VAR_CurVideoMode (*((uint*)(0x08D6220)))
#define VAR_SavedVideoMode (*((uint*)(0x0BA6820)))
#define VAR_CurAdapter (*((uint*)(0x0C920F4)))

#define HOOKPOS_GetFxQuality 0x49EA50
void HOOK_GetFxQuality();

#define HOOKPOS_StoreShadowForVehicle 0x70BDA0
DWORD RETURN_StoreShadowForVehicle = 0x70BDA9;
void  HOOK_StoreShadowForVehicle();

float ms_fVehicleLODDistance, ms_fTrainPlaneLODDistance, ms_fPedsLODDistance;

CSettingsSA::CSettingsSA()
{
    m_pInterface = (CSettingsSAInterface*)CLASS_CMenuManager;
    m_pInterface->bFrameLimiter = false;
    m_bVolumetricShadowsEnabled = false;
    m_bVolumetricShadowsSuspended = false;
    m_bBlurViaScript = false;
    m_bDynamicPedShadowsEnabled = false;
    m_bCoronaReflectionsViaScript = false;
    SetAspectRatio(ASPECT_RATIO_4_3);
    HookInstall(HOOKPOS_GetFxQuality, (DWORD)HOOK_GetFxQuality, 5);
    HookInstall(HOOKPOS_StoreShadowForVehicle, (DWORD)HOOK_StoreShadowForVehicle, 9);
    m_iDesktopWidth = 0;
    m_iDesktopHeight = 0;
    MemPut<BYTE>(0x6FF420, 0xC3);            // Truncate CalculateAspectRatio

    MemPut(0x732926, &ms_fVehicleLODDistance);
    MemPut(0x732940, &ms_fTrainPlaneLODDistance);
    MemPut(0x73295E, &ms_fPedsLODDistance);

    // Set "radar map and radar" as default radar mode
    SetRadarMode(RADAR_MODE_ALL);
}

bool CSettingsSA::IsWideScreenEnabled()
{
    return m_pInterface->bUseWideScreen;
}

void CSettingsSA::SetWideScreenEnabled(bool bEnabled)
{
    m_pInterface->bUseWideScreen = bEnabled;
}

unsigned int CSettingsSA::GetNumVideoModes()
{
    // RwEngineGetNumVideoModes
    return ((unsigned int(__cdecl*)())0x7F2CC0)();
}

VideoMode* CSettingsSA::GetVideoModeInfo(VideoMode* modeInfo, unsigned int modeIndex)
{
    // RwEngineGetVideoModeInfo
    return ((VideoMode*(__cdecl*)(VideoMode*, unsigned int))0x7F2CF0)(modeInfo, modeIndex);
}

unsigned int CSettingsSA::GetCurrentVideoMode()
{
    // RwEngineGetCurrentVideoMode
    return ((unsigned int(__cdecl*)())0x7F2D20)();
}

void CSettingsSA::SetCurrentVideoMode(unsigned int modeIndex, bool bOnRestart)
{
    if (!bOnRestart)
    {
        ((void(__cdecl*)(unsigned int))0x745C70)(modeIndex);
    }
    // Only update settings variables for fullscreen modes
    if (modeIndex)
        m_pInterface->dwVideoMode = modeIndex;
}

uint CSettingsSA::GetNumAdapters()
{
    // RwEngineGetNumSubSystems
    return ((unsigned int(__cdecl*)())0x7F2C00)();
}

void CSettingsSA::SetAdapter(unsigned int uiAdapterIndex)
{
    // RwEngineSetSubSystem
    ((void(__cdecl*)(unsigned int))0x7F2C90)(uiAdapterIndex);
}

unsigned int CSettingsSA::GetCurrentAdapter()
{
    // RwEngineGetCurrentSubSystem
    return ((unsigned int(__cdecl*)())0x7F2C60)();
}

unsigned char CSettingsSA::GetRadioVolume()
{
    return m_pInterface->ucRadioVolume;
}

void CSettingsSA::SetRadioVolume(unsigned char ucVolume)
{
    m_pInterface->ucRadioVolume = ucVolume;
    pGame->GetAudioEngine()->SetMusicMasterVolume(ucVolume);
}

unsigned char CSettingsSA::GetSFXVolume()
{
    return m_pInterface->ucSfxVolume;
}

void CSettingsSA::SetSFXVolume(unsigned char ucVolume)
{
    m_pInterface->ucSfxVolume = ucVolume;
    pGame->GetAudioEngine()->SetEffectsMasterVolume(ucVolume);
}

unsigned int CSettingsSA::GetUsertrackMode()
{
    // 0 = radio, 1 = random, 2 = sequential
    return m_pInterface->ucUsertrackMode;
}

void CSettingsSA::SetUsertrackMode(unsigned int uiMode)
{
    m_pInterface->ucUsertrackMode = uiMode;
}

bool CSettingsSA::IsUsertrackAutoScan()
{
    // 1 = yes, 0 = no
    return m_pInterface->bUsertrackAutoScan;
}

void CSettingsSA::SetUsertrackAutoScan(bool bEnable)
{
    m_pInterface->bUsertrackAutoScan = bEnable;
}

bool CSettingsSA::IsRadioEqualizerEnabled()
{
    // 1 = on, 0 = off
    return m_pInterface->bRadioEqualizer;
}

void CSettingsSA::SetRadioEqualizerEnabled(bool bEnable)
{
    m_pInterface->bRadioEqualizer = bEnable;
}

bool CSettingsSA::IsRadioAutotuneEnabled()
{
    // 1 = on, 0 = off
    return m_pInterface->bRadioAutotune;
}

void CSettingsSA::SetRadioAutotuneEnabled(bool bEnable)
{
    m_pInterface->bRadioAutotune = bEnable;
}

// Minimum is 0.925 and maximum is 1.8
float CSettingsSA::GetDrawDistance()
{
    return m_pInterface->fDrawDistance;
}

void CSettingsSA::SetDrawDistance(float fDistance)
{
    MemPutFast<float>(0x8CD800, fDistance);            // CRenderer::ms_lodDistScale
    m_pInterface->fDrawDistance = fDistance;
}

unsigned int CSettingsSA::GetBrightness()
{
    // up to 384
    return m_pInterface->dwBrightness;
}

void CSettingsSA::SetBrightness(unsigned int uiBrightness)
{
    m_pInterface->dwBrightness = uiBrightness;
}

unsigned int CSettingsSA::GetFXQuality()
{
    // 0 = low, 1 = medium, 2 = high, 3 = very high
    return *(BYTE*)VAR_ucFxQuality;
}

void CSettingsSA::SetFXQuality(unsigned int fxQualityId)
{
    MemPutFast<BYTE>(VAR_ucFxQuality, fxQualityId);
}

float CSettingsSA::GetMouseSensitivity()
{
    float fRawValue = *(float*)VAR_fMouseSensitivity;
    return UnlerpClamped(MOUSE_SENSITIVITY_MIN, fRawValue, MOUSE_SENSITIVITY_MAX);            // Remap to 0-1
}

void CSettingsSA::SetMouseSensitivity(float fSensitivity)
{
    float fRawValue = Lerp(MOUSE_SENSITIVITY_MIN, fSensitivity, MOUSE_SENSITIVITY_MAX);
    MemPutFast<float>(VAR_fMouseSensitivity, fRawValue);
}

unsigned int CSettingsSA::GetAntiAliasing()
{
    // 1 = disabled, 2 = 1x, 3 = 2x, 4 = 3x
    return m_pInterface->dwAntiAliasing;
}

void CSettingsSA::SetAntiAliasing(unsigned int uiAntiAliasing, bool bOnRestart)
{
    if (!bOnRestart)
    {
        DWORD dwFunc = FUNC_SetAntiAliasing;
        _asm
        {
            push    uiAntiAliasing
            call    dwFunc
            add     esp, 4
        }
        SetCurrentVideoMode(m_pInterface->dwVideoMode, false);
    }

    m_pInterface->dwAntiAliasing = uiAntiAliasing;
}

bool CSettingsSA::IsMipMappingEnabled()
{
    return m_pInterface->bMipMapping;
}

void CSettingsSA::SetMipMappingEnabled(bool bEnable)
{
    m_pInterface->bMipMapping = bEnable;
}

void CSettingsSA::Save()
{
    _asm
    {
        mov ecx, CLASS_CMenuManager
        mov eax, FUNC_CMenuManager_Save
        call eax
    }
}

bool CSettingsSA::IsVolumetricShadowsEnabled() const noexcept
{
    return m_bVolumetricShadowsEnabled && !m_bVolumetricShadowsSuspended;
}

void CSettingsSA::SetVolumetricShadowsEnabled(bool bEnable)
{
    m_bVolumetricShadowsEnabled = bEnable;

    // Disable rendering ped real time shadows when they sit on bikes
    // if vehicle volumetric shadows are disabled because it looks a bit weird
    MemPut<BYTE>(0x5E682A + 1, bEnable);
}


bool CSettingsSA::GetVolumetricShadowsEnabledByVideoSetting() const noexcept
{
    bool volumetricShadow;
    g_pCore->GetCVars()->Get("volumetric_shadows", volumetricShadow);
    return volumetricShadow;
}

bool CSettingsSA::ResetVolumetricShadows() noexcept
{
    pGame->GetSettings()->SetVolumetricShadowsEnabled(pGame->GetSettings()->GetVolumetricShadowsEnabledByVideoSetting());
    return true;
}

void CSettingsSA::SetVolumetricShadowsSuspended(bool bSuspended)
{
    m_bVolumetricShadowsSuspended = bSuspended;
}

bool CSettingsSA::IsDynamicPedShadowsEnabled()
{
    return m_bDynamicPedShadowsEnabled;
}

void CSettingsSA::SetDynamicPedShadowsEnabled(bool bEnable)
{
    m_bDynamicPedShadowsEnabled = bEnable;
}

bool CSettingsSA::IsDynamicPedShadowsEnabledByVideoSetting() const noexcept
{
    bool pedDynamicShadows;
    g_pCore->GetCVars()->Get("dynamic_ped_shadows", pedDynamicShadows);
    return pedDynamicShadows;
}

bool CSettingsSA::ResetDynamicPedShadows() noexcept
{
    pGame->GetSettings()->SetDynamicPedShadowsEnabled(pGame->GetSettings()->IsDynamicPedShadowsEnabledByVideoSetting());
    return true;
}


//
// Volumetric shadow hooks
//
DWORD dwFxQualityValue = 0;
WORD  usCallingForVehicleModel = 0;

__declspec(noinline) void _cdecl MaybeAlterFxQualityValue(DWORD dwAddrCalledFrom)
{
    // Handle all calls from CVolumetricShadowMgr
    if (dwAddrCalledFrom > 0x70F990 && dwAddrCalledFrom < 0x711EB0)
    {
        // Force blob shadows if volumetric shadows are not enabled
        if (!pGame->GetSettings()->IsVolumetricShadowsEnabled())
            dwFxQualityValue = 0;

        // These vehicles seem to have problems with volumetric shadows, so force blob shadows
        switch (usCallingForVehicleModel)
        {
            case 460:            // Skimmer
            case 511:            // Beagle
            case 572:            // Mower
            case 590:            // Box Freight
            case 592:            // Andromada
                dwFxQualityValue = 0;
        }
        usCallingForVehicleModel = 0;
    }
    else
        // Handle all calls from CPed::PreRenderAfterTest
        if (dwAddrCalledFrom > 0x5E65A0 && dwAddrCalledFrom < 0x5E7680)
    {
        dwFxQualityValue = pGame->GetSettings()->IsDynamicPedShadowsEnabled() ? 2 : 0;
    }
}

// Hooked from 0x49EA50
void _declspec(naked) HOOK_GetFxQuality()
{
    _asm
    {
        pushad
        mov     eax, [ecx+054h]            // Current FxQuality setting
        mov     dwFxQualityValue, eax

        mov     eax, [esp+32]            // Address GetFxQuality was called from
        push    eax
        call    MaybeAlterFxQualityValue
        add     esp, 4

        popad
        mov     eax, dwFxQualityValue
        retn
    }
}

// Hook to discover what vehicle will be calling GetFxQuality
void _declspec(naked) HOOK_StoreShadowForVehicle()
{
    _asm
    {
        // Hooked from 0x70BDA0  5 bytes
        mov     eax, [esp+4]            // Get vehicle
        mov     ax, [eax+34]            // pEntity->m_nModelIndex
        mov     usCallingForVehicleModel, ax
        sub     esp, 44h
        push    ebx
        mov     eax, 0x70F9B0            // CVolumetricShadowMgr::IsAvailable
        call    eax
        jmp     RETURN_StoreShadowForVehicle
    }
}

////////////////////////////////////////////////
//
// AspectRatio
//
////////////////////////////////////////////////
eAspectRatio CSettingsSA::GetAspectRatio()
{
    return m_AspectRatio;
}

float CSettingsSA::GetAspectRatioValue()
{
    return *(float*)0xC3EFA4;
}

void CSettingsSA::SetAspectRatio(eAspectRatio aspectRatio, bool bAdjustmentEnabled)
{
    // Process change
    m_AspectRatio = aspectRatio;

    float fValue;
    if (m_AspectRatio == ASPECT_RATIO_AUTO)
    {
        VideoMode modeInfo;
        pGame->GetSettings()->GetVideoModeInfo(&modeInfo, pGame->GetSettings()->GetCurrentVideoMode());
        fValue = modeInfo.width / (float)modeInfo.height;
    }
    else if (m_AspectRatio == ASPECT_RATIO_4_3)
    {
        fValue = 4 / 3.f;
    }
    else if (m_AspectRatio == ASPECT_RATIO_16_10)
    {
        fValue = 16 / 10.f;
    }
    else            // ASPECT_RATIO_16_9
    {
        fValue = 16 / 9.f;
    }

    MemPutFast<float>(0xC3EFA4, fValue);

    // Adjust position and size of our HUD components
    if (bAdjustmentEnabled)
        pGame->GetHud()->AdjustComponents(fValue);
    else
        pGame->GetHud()->ResetComponentAdjustment();
}

////////////////////////////////////////////////
//
// Grass
//
////////////////////////////////////////////////
bool CSettingsSA::IsGrassEnabled()
{
    return *(BYTE*)0x05DBAED == 0x85;
}

void CSettingsSA::SetGrassEnabled(bool bEnable)
{
    MemPut<BYTE>(0x05DBAED, bEnable ? 0x85 : 0x33);
}

////////////////////////////////////////////////
//
// HUD mode (radar map + blips, blips only, nothing)
//
////////////////////////////////////////////////
eRadarMode CSettingsSA::GetRadarMode()
{
    return *(eRadarMode*)VAR_RadarMode;
}

void CSettingsSA::SetRadarMode(eRadarMode hudMode)
{
    MemPutFast<DWORD>(VAR_RadarMode, hudMode);
}

////////////////////////////////////////////////
//
// Camera field of view. Player follow and car follow only
//
////////////////////////////////////////////////
float ms_fFOV = 70;
float ms_fFOVCar = 70;
float ms_fFOVCarMax = 100;            // at high vehicle velocity
bool  ms_bFOVPlayerFromScript = false;
bool  ms_bFOVVehicleFromScript = false;

// consider moving this to the camera class - qaisjp
float CSettingsSA::GetFieldOfViewPlayer()
{
    return ms_fFOV;
}

float CSettingsSA::GetFieldOfViewVehicle()
{
    return ms_fFOVCar;
}

float CSettingsSA::GetFieldOfViewVehicleMax()
{
    return ms_fFOVCarMax;
}

void CSettingsSA::UpdateFieldOfViewFromSettings()
{
    float fFieldOfView;
    g_pCore->GetCVars()->Get("fov", fFieldOfView);
    fFieldOfView = Clamp(70.f, fFieldOfView, 100.f);
    SetFieldOfViewPlayer(fFieldOfView, false);
    SetFieldOfViewVehicle(fFieldOfView, false);
    SetFieldOfViewVehicleMax(100, false);
}

void CSettingsSA::ResetFieldOfViewFromScript()
{
    ms_bFOVPlayerFromScript = false;
    ms_bFOVVehicleFromScript = false;
    UpdateFieldOfViewFromSettings();
}

void CSettingsSA::SetFieldOfViewPlayer(float fAngle, bool bFromScript)
{
    if (!bFromScript && ms_bFOVPlayerFromScript)
        return;
    ms_bFOVPlayerFromScript = bFromScript;
    ms_fFOV = fAngle;
    MemPut<void*>(0x0522F3A, &ms_fFOV);
    MemPut<void*>(0x0522F5D, &ms_fFOV);
    MemPut<float>(0x0522F7A, ms_fFOV);
}

void CSettingsSA::SetFieldOfViewVehicle(float fAngle, bool bFromScript)
{
    if (!bFromScript && ms_bFOVVehicleFromScript)
        return;
    ms_bFOVVehicleFromScript = bFromScript;
    ms_fFOVCar = fAngle;
    MemPut<void*>(0x0524B76, &ms_fFOVCar);
    MemPut<void*>(0x0524B9A, &ms_fFOVCar);
    MemPut<void*>(0x0524BA2, &ms_fFOVCar);
    MemPut<void*>(0x0524BD3, &ms_fFOVCar);
    MemPut<float>(0x0524BE4, ms_fFOVCar);
}

void CSettingsSA::SetFieldOfViewVehicleMax(float fAngle, bool bFromScript)
{
    if (!bFromScript && ms_bFOVVehicleFromScript)
        return;
    ms_bFOVVehicleFromScript = bFromScript;
    ms_fFOVCarMax = fAngle;
    MemPut<void*>(0x0524BB4, &ms_fFOVCarMax);
    MemPut<float>(0x0524BC5, ms_fFOVCarMax);
}

////////////////////////////////////////////////
//
// Vehicles LOD draw distance
//
////////////////////////////////////////////////
bool ms_bMaxVehicleLODDistanceFromScript = false;

void CSettingsSA::SetVehiclesLODDistance(float fVehiclesLODDistance, float fTrainsPlanesLODDistance, bool bFromScript)
{
    ms_fVehicleLODDistance = fVehiclesLODDistance;
    ms_fTrainPlaneLODDistance = fTrainsPlanesLODDistance;
    ms_bMaxVehicleLODDistanceFromScript = bFromScript;
}

void CSettingsSA::ResetVehiclesLODDistance(bool bForceDefault)
{
    if (ms_bMaxVehicleLODDistanceFromScript && !bForceDefault)
        return;

    bool bHighDetailVehicles;
    g_pCore->GetCVars()->Get("high_detail_vehicles", bHighDetailVehicles);

    if (bHighDetailVehicles)
    {
        ms_fVehicleLODDistance = MAX_VEHICLE_LOD_DISTANCE;
        ms_fTrainPlaneLODDistance = MAX_VEHICLE_LOD_DISTANCE;
    }
    else
    {
        ms_fVehicleLODDistance = DEFAULT_VEHICLE_LOD_DISTANCE;
        ms_fTrainPlaneLODDistance = DEFAULT_VEHICLE_LOD_DISTANCE * TRAIN_LOD_DISTANCE_MULTIPLIER;
    }

    ms_bMaxVehicleLODDistanceFromScript = false;
}

void CSettingsSA::GetVehiclesLODDistance(float& fVehiclesLODDistance, float& fTrainsPlanesLODDistance)
{
    fVehiclesLODDistance = ms_fVehicleLODDistance;
    fTrainsPlanesLODDistance = ms_fTrainPlaneLODDistance;
}

////////////////////////////////////////////////
//
// Peds LOD draw distance
//
////////////////////////////////////////////////
bool ms_bMaxPedsLODDistanceFromScript = false;

void CSettingsSA::SetPedsLODDistance(float fPedsLODDistance, bool bFromScript)
{
    ms_fPedsLODDistance = fPedsLODDistance;
    ms_bMaxPedsLODDistanceFromScript = bFromScript;
}

void CSettingsSA::ResetPedsLODDistance(bool bForceDefault)
{
    if (ms_bMaxPedsLODDistanceFromScript && !bForceDefault)
        return;

    bool bHighDetailPeds;
    g_pCore->GetCVars()->Get("high_detail_peds", bHighDetailPeds);

    if (bHighDetailPeds)
        ms_fPedsLODDistance = MAX_PEDS_LOD_DISTANCE;
    else
        ms_fPedsLODDistance = DEFAULT_PEDS_LOD_DISTANCE;

    ms_bMaxPedsLODDistanceFromScript = false;
}

float CSettingsSA::GetPedsLODDistance()
{
    return ms_fPedsLODDistance;
}

////////////////////////////////////////////////
//
// Blur
//
// When blur is controlled by script changing this option
// in settings doesn't produce any effect
//
////////////////////////////////////////////////
void CSettingsSA::ResetBlurEnabled()
{
    if (m_bBlurViaScript)
        return;

    bool bEnabled;
    g_pCore->GetCVars()->Get("blur", bEnabled);
    pGame->SetBlurLevel(bEnabled ? DEFAULT_BLUR_LEVEL : 0);
}

void CSettingsSA::SetBlurControlledByScript(bool bByScript)
{
    m_bBlurViaScript = bByScript;
}

// Corona rain reflections
//
// When corona reflections are controlled by script changing this option
// in settings doesn't produce any effect
//
////////////////////////////////////////////////
void CSettingsSA::ResetCoronaReflectionsEnabled()
{
    if (m_bCoronaReflectionsViaScript)
        return;

    bool bEnabled;
    g_pCore->GetCVars()->Get("corona_reflections", bEnabled);
    g_pCore->GetGame()->GetCoronas()->SetCoronaReflectionsEnabled(bEnabled ? 1 : 0);
}

void CSettingsSA::SetCoronaReflectionsControlledByScript(bool bViaScript)
{
    m_bCoronaReflectionsViaScript = bViaScript;
}

////////////////////////////////////////////////
//
// CSettingsSA::HasUnsafeResolutions
//
// Return true if DirectX says we have resolutions available that are higher that the desktop
//
////////////////////////////////////////////////
bool CSettingsSA::HasUnsafeResolutions()
{
    uint numVidModes = GetNumVideoModes();
    for (uint vidMode = 0; vidMode < numVidModes; vidMode++)
    {
        VideoMode vidModeInfo;
        GetVideoModeInfo(&vidModeInfo, vidMode);

        if (vidModeInfo.flags & rwVIDEOMODEEXCLUSIVE)
        {
            if (IsUnsafeResolution(vidModeInfo.width, vidModeInfo.height))
                return true;
        }
    }
    return false;
}

////////////////////////////////////////////////
//
// CSettingsSA::IsUnsafeResolution
//
// Check if supplied resolution is higher than the desktop
//
////////////////////////////////////////////////
bool CSettingsSA::IsUnsafeResolution(int iWidth, int iHeight)
{
    // Check if we have gotten the desktop res yet
    if (m_iDesktopWidth == 0)
    {
        m_iDesktopWidth = 800;
        m_iDesktopHeight = 600;

        VideoMode currentModeInfo;
        if (GetVideoModeInfo(&currentModeInfo, 0))
        {
            m_iDesktopWidth = currentModeInfo.width;
            m_iDesktopHeight = currentModeInfo.height;
        }
    }
    return iWidth > m_iDesktopWidth || iHeight > m_iDesktopHeight;
}

////////////////////////////////////////////////
//
// CSettingsSA::FindVideoMode
//
// Find best matching video mode
//
////////////////////////////////////////////////
uint CSettingsSA::FindVideoMode(int iResX, int iResY, int iColorBits)
{
    int iBestMode, iBestScore = -1;

    uint numVidModes = GetNumVideoModes();
    for (uint vidMode = 0; vidMode < numVidModes; vidMode++)
    {
        VideoMode vidModeInfo;
        GetVideoModeInfo(&vidModeInfo, vidMode);

        // Remove resolutions that will make the gui unusable
        if (vidModeInfo.width < 640 || vidModeInfo.height < 480)
            continue;

        if (vidModeInfo.flags & rwVIDEOMODEEXCLUSIVE)
        {
            // Rate my res
            int iScore = abs(iResX - vidModeInfo.width) + abs(iResY - vidModeInfo.height);

            // Penalize matches with wrong bit depth
            if (vidModeInfo.depth != iColorBits)
            {
                iScore += 100000;
            }

            // Penalize matches with higher than requested resolution
            if (vidModeInfo.width > iResX || vidModeInfo.height > iResY)
            {
                iScore += 200000;
            }

            if (iScore < iBestScore || iBestScore == -1)
            {
                // Found a better match
                iBestScore = iScore;
                iBestMode = vidMode;
            }
        }
    }

    if (iBestScore != -1)
        return iBestMode;

    BrowseToSolution("no-find-res", EXIT_GAME_FIRST | ASK_GO_ONLINE, _("Can't find valid screen resolution."));
    return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CSettingsSA::SetValidVideoMode
//
// Set/validate the required video mode
//
//////////////////////////////////////////////////////////////////////////////////////////
void CSettingsSA::SetValidVideoMode()
{
    bool bValid = false;
    int  iWidth, iHeight, iColorBits, iAdapterIndex;
    bool bAllowUnsafeResolutions = false;

    // First, try to get MTA saved info
    if (!bValid)
    {
        bValid = g_pCore->GetRequiredDisplayResolution(iWidth, iHeight, iColorBits, iAdapterIndex, bAllowUnsafeResolutions);
    }

    // Otherwise deduce from GTA saved video mode
    if (!bValid)
    {
        SetAdapter(0);
        uint numVidModes = GetNumVideoModes();
        if (VAR_SavedVideoMode > 0 && VAR_SavedVideoMode < numVidModes)
        {
            VideoMode modeInfo;
            if (GetVideoModeInfo(&modeInfo, VAR_SavedVideoMode))
            {
                iWidth = modeInfo.width;
                iHeight = modeInfo.height;
                iColorBits = modeInfo.depth;
                iAdapterIndex = 0;
                bValid = true;
            }
        }
    }

    // Finally use default
    if (!bValid)
    {
        bValid = true;
        iWidth = 800;
        iHeight = 600;
        iColorBits = 32;
        iAdapterIndex = 0;
    }

    // Set adapter
    if ((uint)iAdapterIndex >= GetNumAdapters())
        iAdapterIndex = 0;
    SetAdapter(iAdapterIndex);

    // Save desktop resolution
    {
        m_iDesktopWidth = 800;
        m_iDesktopHeight = 600;

        VideoMode currentModeInfo;
        if (GetVideoModeInfo(&currentModeInfo, GetCurrentVideoMode()))
        {
            m_iDesktopWidth = currentModeInfo.width;
            m_iDesktopHeight = currentModeInfo.height;
        }
    }

    // Handle 'unsafe' resolution stuff
    if (IsUnsafeResolution(iWidth, iHeight))
    {
        if (bAllowUnsafeResolutions)
        {
            // Confirm that res should be used
            SString strMessage = _("Are you sure you want to use this screen resolution?");
            strMessage += SString("\n\n%d x %d", iWidth, iHeight);
            if (MessageBoxUTF8(NULL, strMessage, _("MTA: San Andreas"), MB_YESNO | MB_TOPMOST | MB_ICONQUESTION) == IDNO)
                bAllowUnsafeResolutions = false;
        }

        if (!bAllowUnsafeResolutions)
        {
            // Force down to desktop res if required
            iWidth = m_iDesktopWidth;
            iHeight = m_iDesktopHeight;
        }
    }

    // Ensure res is no smaller than 640 x 480
    iWidth = std::max(640, iWidth);
    iHeight = std::max(480, iHeight);

    // Find mode number which best matches required settings
    uint uiUseVideoMode = FindVideoMode(iWidth, iHeight, iColorBits);

    // Set for GTA to use
    VAR_CurVideoMode = uiUseVideoMode;
    VAR_SavedVideoMode = uiUseVideoMode;
    VAR_CurAdapter = iAdapterIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CSettingsSA::OnSelectDevice
//
// return 0 for single adapter
// return 1 for multi adapter hide dialog
// return 2 for multi adapter show dialog
//
//////////////////////////////////////////////////////////////////////////////////////////
int CSettingsSA::OnSelectDevice()
{
    if (GetNumAdapters() > 1 && g_pCore->GetDeviceSelectionEnabled())
    {
        // Show device selection
        return 1;
    }

    SetValidVideoMode();

    if (GetNumAdapters() > 1)
    {
        // Hide device selection
        return 2;
    }
    else
    {
        return 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Hook psSelectDevice so we can:
//   * Set/validate the required video mode
//   * Choose whether to show the device selection dialog box
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) int OnMY_SelectDevice()
{
    CSettingsSA* gameSettings = (CSettingsSA*)pGame->GetSettings();
    return gameSettings->OnSelectDevice();
}

// Hook info
#define HOOKPOS_SelectDevice 0x0746219
#define HOOKSIZE_SelectDevice 6
DWORD RETURN_SelectDeviceSingle = 0x0746273;
DWORD RETURN_SelectDeviceMultiHide = 0x074622C;
DWORD RETURN_SelectDeviceMultiShow = 0x0746227;
void _declspec(naked) HOOK_SelectDevice()
{
    _asm
    {
        pushad
        call    OnMY_SelectDevice
        cmp     eax, 1
        popad

        jl      single
        jz      multishow

                // multhide
        mov     eax, 1
        jmp     RETURN_SelectDeviceMultiHide

multishow:
        jmp     RETURN_SelectDeviceMultiShow

single:
        jmp     RETURN_SelectDeviceSingle
    }
}

////////////////////////////////////////////////
//
// Setup hooks
//
////////////////////////////////////////////////
void CSettingsSA::StaticSetHooks()
{
    EZHookInstall(SelectDevice);
}
