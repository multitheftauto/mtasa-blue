/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CSettingsSA.cpp
*  PURPOSE:     Game settings
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static const float MOUSE_SENSITIVITY_MIN     = 0.000312f;
static const float MOUSE_SENSITIVITY_DEFAULT = 0.0025f;
static const float MOUSE_SENSITIVITY_MAX     = MOUSE_SENSITIVITY_DEFAULT * 2 - MOUSE_SENSITIVITY_MIN;

unsigned long CSettingsSA::FUNC_GetNumVideoModes;
unsigned long CSettingsSA::FUNC_GetVideoModeInfo;
unsigned long CSettingsSA::FUNC_GetCurrentVideoMode;
unsigned long CSettingsSA::FUNC_SetCurrentVideoMode;
unsigned long CSettingsSA::FUNC_SetDrawDistance;

#define HOOKPOS_GetFxQuality                0x49EA50
void HOOK_GetFxQuality ();

#define HOOKPOS_StoreShadowForVehicle       0x70BDA0
DWORD RETURN_StoreShadowForVehicle =        0x70BDA9;
void HOOK_StoreShadowForVehicle ();

CSettingsSA::CSettingsSA ( void )
{
    m_pInterface = (CSettingsSAInterface *)CLASS_CMenuManager;
    m_pInterface->bFrameLimiter = false;
    m_bVolumetricShadowsEnabled = false;
    SetAspectRatio ( ASPECT_RATIO_4_3 );
    HookInstall ( HOOKPOS_GetFxQuality, (DWORD)HOOK_GetFxQuality, 5 );
    HookInstall ( HOOKPOS_StoreShadowForVehicle, (DWORD)HOOK_StoreShadowForVehicle, 9 );

    MemPut < BYTE > ( 0x6FF420, 0xC3 );     // Truncate CalculateAspectRatio
}

bool CSettingsSA::IsWideScreenEnabled ( void )
{
    return m_pInterface->bUseWideScreen;
}

void CSettingsSA::SetWideScreenEnabled ( bool bEnabled )
{
    m_pInterface->bUseWideScreen = bEnabled;
}

unsigned int CSettingsSA::GetNumVideoModes ( void )
{
    unsigned int uiReturn = 0;
    _asm
    {
        call    FUNC_GetNumVideoModes
        mov     uiReturn, eax
    }
    return uiReturn;
}

VideoMode * CSettingsSA::GetVideoModeInfo ( VideoMode * modeInfo, unsigned int modeIndex )
{
    _asm
    {
        push    modeIndex
        push    modeInfo
        call    FUNC_GetVideoModeInfo
        add     esp, 8
    }
    return modeInfo;
}

unsigned int CSettingsSA::GetCurrentVideoMode ( void )
{
    unsigned int uiReturn = 0;
    _asm
    {
        call    FUNC_GetCurrentVideoMode
        mov     uiReturn, eax
    }
    return uiReturn;
}

void CSettingsSA::SetCurrentVideoMode ( unsigned int modeIndex, bool bOnRestart )
{
    if ( !bOnRestart )
    {
        _asm
        {
            push    modeIndex
            call    FUNC_SetCurrentVideoMode
            add     esp, 4
        }
    }
    // Only update settings variables for fullscreen modes
    if ( modeIndex )
        m_pInterface->dwVideoMode = modeIndex;
}

unsigned char CSettingsSA::GetRadioVolume ( void )
{
    return m_pInterface->ucRadioVolume;
}

void CSettingsSA::SetRadioVolume ( unsigned char ucVolume )
{
    m_pInterface->ucRadioVolume = ucVolume;
    pGame->GetAudioEngine ()->SetMusicMasterVolume ( ucVolume );
}

unsigned char CSettingsSA::GetSFXVolume ( void )
{
    return m_pInterface->ucSfxVolume;
}

void CSettingsSA::SetSFXVolume ( unsigned char ucVolume )
{
    m_pInterface->ucSfxVolume = ucVolume;
    pGame->GetAudioEngine ()->SetEffectsMasterVolume ( ucVolume );
}

unsigned int CSettingsSA::GetUsertrackMode ( void )
{
    // 0 = radio, 1 = random, 2 = sequential
    return m_pInterface->ucUsertrackMode;
}

void CSettingsSA::SetUsertrackMode ( unsigned int uiMode )
{
    m_pInterface->ucUsertrackMode = uiMode;
}

bool CSettingsSA::IsUsertrackAutoScan ( void )
{
    // 1 = yes, 0 = no
    return m_pInterface->bUsertrackAutoScan;
}

void CSettingsSA::SetUsertrackAutoScan ( bool bEnable )
{
    m_pInterface->bUsertrackAutoScan = bEnable;
}

bool CSettingsSA::IsRadioEqualizerEnabled ( void )
{
    // 1 = on, 0 = off
    return m_pInterface->bRadioEqualizer;
}

void CSettingsSA::SetRadioEqualizerEnabled ( bool bEnable )
{
    m_pInterface->bRadioEqualizer = bEnable;
}

bool CSettingsSA::IsRadioAutotuneEnabled ( void )
{
    // 1 = on, 0 = off
    return m_pInterface->bRadioAutotune;
}

void CSettingsSA::SetRadioAutotuneEnabled ( bool bEnable )
{
    m_pInterface->bRadioAutotune = bEnable;
}

// Minimum is 0.925 and maximum is 1.8
float CSettingsSA::GetDrawDistance ( void )
{
    return m_pInterface->fDrawDistance;
}

void CSettingsSA::SetDrawDistance ( float fDistance )
{
    _asm
    {
        push    fDistance
        call    FUNC_SetDrawDistance
        add     esp, 4
    }
    m_pInterface->fDrawDistance = fDistance;
}

unsigned int CSettingsSA::GetBrightness ( )
{
    // up to 384
    return m_pInterface->dwBrightness;
}

void CSettingsSA::SetBrightness ( unsigned int uiBrightness )
{
    m_pInterface->dwBrightness = uiBrightness;
}

unsigned int CSettingsSA::GetFXQuality ( )
{
    // 0 = low, 1 = medium, 2 = high, 3 = very high
    return *(BYTE *)VAR_ucFxQuality;
}

void CSettingsSA::SetFXQuality ( unsigned int fxQualityId )
{
    MemPutFast < BYTE > ( VAR_ucFxQuality, fxQualityId );
}

float CSettingsSA::GetMouseSensitivity ( )
{
    float fRawValue = *(FLOAT *)VAR_fMouseSensitivity;
    return UnlerpClamped( MOUSE_SENSITIVITY_MIN, fRawValue, MOUSE_SENSITIVITY_MAX );    // Remap to 0-1
}

void CSettingsSA::SetMouseSensitivity ( float fSensitivity )
{
    float fRawValue = Lerp( MOUSE_SENSITIVITY_MIN, fSensitivity, MOUSE_SENSITIVITY_MAX );
    MemPutFast < FLOAT > ( VAR_fMouseSensitivity, fRawValue );
}

unsigned int CSettingsSA::GetAntiAliasing ( )
{
    // 1 = disabled, 2 = 1x, 3 = 2x, 4 = 3x
    return m_pInterface->dwAntiAliasing;
}

void CSettingsSA::SetAntiAliasing ( unsigned int uiAntiAliasing, bool bOnRestart )
{
    if ( !bOnRestart )
    {
        DWORD dwFunc = FUNC_SetAntiAliasing;
        _asm
        {
            push    uiAntiAliasing
            call    dwFunc
            add     esp, 4
        }
        SetCurrentVideoMode ( m_pInterface->dwVideoMode, false );
    }

    m_pInterface->dwAntiAliasing = uiAntiAliasing;
}

bool CSettingsSA::IsMipMappingEnabled ( void )
{
    return m_pInterface->bMipMapping;
}

void CSettingsSA::SetMipMappingEnabled ( bool bEnable )
{
    m_pInterface->bMipMapping = bEnable;
}

void CSettingsSA::Save ()
{
    _asm
    {
        mov ecx, CLASS_CMenuManager
        mov eax, FUNC_CMenuManager_Save
        call eax
    }
}

bool CSettingsSA::IsVolumetricShadowsEnabled ( void )
{
    return m_bVolumetricShadowsEnabled;
}

void CSettingsSA::SetVolumetricShadowsEnabled ( bool bEnable )
{
    m_bVolumetricShadowsEnabled = bEnable;
}

//
// Volumetric shadow hooks
//
DWORD dwFxQualityValue = 0;
WORD usCallingForVehicleModel = 0;

void _cdecl MaybeAlterFxQualityValue ( DWORD dwAddrCalledFrom )
{
    // Handle all calls from CVolumetricShadowMgr
    if ( dwAddrCalledFrom > 0x70F990 && dwAddrCalledFrom < 0x711EB0 )
    {
        // Force blob shadows if volumetric shadows are not enabled
        if ( !pGame->GetSettings ()->IsVolumetricShadowsEnabled () )
            dwFxQualityValue = 0;

        // These vehicles seem to have problems with volumetric shadows, so force blob shadows
        switch ( usCallingForVehicleModel )
        {
            case 460:   // Skimmer
            case 511:   // Beagle
            case 572:   // Mower
            case 590:   // Box Freight
            case 592:   // Andromada
                dwFxQualityValue = 0;
        }
        usCallingForVehicleModel = 0;
    }
    else
    // Handle all calls from CPed::PreRenderAfterTest
    if ( dwAddrCalledFrom > 0x5E65A0 && dwAddrCalledFrom < 0x5E7680 )
    {
        // Always use blob shadows for peds as realtime shadows are disabled in MTA (context switching issues)
        dwFxQualityValue = 0;
    }
}

// Hooked from 0x49EA50
void _declspec(naked) HOOK_GetFxQuality ()
{
    _asm
    {
        pushad
        mov     eax, [ecx+054h]         // Current FxQuality setting
        mov     dwFxQualityValue, eax

        mov     eax, [esp+32]           // Address GetFxQuality was called from
        push    eax                     
        call    MaybeAlterFxQualityValue
        add     esp, 4

        popad
        mov     eax, dwFxQualityValue
        retn
    }
}

// Hook to discover what vehicle will be calling GetFxQuality
void _declspec(naked) HOOK_StoreShadowForVehicle ()
{
    _asm
    {
        // Hooked from 0x70BDA0  5 bytes
        mov     eax, [esp+4]        // Get vehicle
        mov     ax, [eax+34]        // pEntity->m_nModelIndex
        mov     usCallingForVehicleModel, ax
        sub     esp, 44h 
        push    ebx
        mov     eax, 0x70F9B0       // CVolumetricShadowMgr::IsAvailable
        call    eax
        jmp     RETURN_StoreShadowForVehicle
    }
}


////////////////////////////////////////////////
//
// AspectRatio
//
////////////////////////////////////////////////
eAspectRatio CSettingsSA::GetAspectRatio ( void )
{
    return m_AspectRatio;
}

void CSettingsSA::SetAspectRatio ( eAspectRatio aspectRatio )
{
    // Process change
    m_AspectRatio = aspectRatio;

    float fValue;
    if ( m_AspectRatio == ASPECT_RATIO_AUTO )
    {
        VideoMode modeInfo;
        pGame->GetSettings ()->GetVideoModeInfo ( &modeInfo, pGame->GetSettings ()->GetCurrentVideoMode () );
        fValue = modeInfo.width / (float)modeInfo.height;
    }
    else
    if ( m_AspectRatio == ASPECT_RATIO_4_3 )
    {
        fValue = 4 / 3.f;
    }
    else
    if ( m_AspectRatio == ASPECT_RATIO_16_10 )
    {
        fValue = 16 / 10.f;
    }
    else    // ASPECT_RATIO_16_9
    {
        fValue = 16 / 9.f;
    }

    MemPutFast < float > ( 0xC3EFA4, fValue );

    // Adjust position and size of our HUD components
    pGame->GetHud ()->AdjustComponents ( fValue );
}

////////////////////////////////////////////////
//
// Grass
//
////////////////////////////////////////////////
bool CSettingsSA::IsGrassEnabled ( void )
{
    return *(BYTE*)0x05DBAED == 0x85;
}

void CSettingsSA::SetGrassEnabled ( bool bEnable )
{
    MemPut < BYTE > ( 0x05DBAED, bEnable ? 0x85 : 0x33 );
}

////////////////////////////////////////////////
//
// Select device dialog for multi-monitor setups
//
////////////////////////////////////////////////
void CSettingsSA::SetSelectDeviceDialogEnabled ( bool bEnable )
{
    MemPut < BYTE > ( 0x74621F, bEnable ? 0x01 : 0x42 );
}
