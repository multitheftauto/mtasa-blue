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

unsigned long CSettingsSA::FUNC_GetNumVideoModes;
unsigned long CSettingsSA::FUNC_GetVideoModeInfo;
unsigned long CSettingsSA::FUNC_GetCurrentVideoMode;
unsigned long CSettingsSA::FUNC_SetCurrentVideoMode;
unsigned long CSettingsSA::FUNC_SetDrawDistance;

CSettingsSA::CSettingsSA ( void )
{
    m_pInterface = (CSettingsSAInterface *)CLASS_CMenuManager;
    m_pInterface->bFrameLimiter = false;
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
    pGame->GetAudio ()->SetMusicMasterVolume ( ucVolume );
}

unsigned char CSettingsSA::GetSFXVolume ( void )
{
    return m_pInterface->ucSfxVolume;
}

void CSettingsSA::SetSFXVolume ( unsigned char ucVolume )
{
    m_pInterface->ucSfxVolume = ucVolume;
    pGame->GetAudio ()->SetEffectsMasterVolume ( ucVolume );
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
    MemPut < BYTE > ( VAR_ucFxQuality, fxQualityId );
}

float CSettingsSA::GetMouseSensitivity ( )
{
    // 0.000312 (min) - 0.005000 (max)
    return *(FLOAT *)VAR_fMouseSensitivity;
}

void CSettingsSA::SetMouseSensitivity ( float fSensitivity )
{
    MemPut < FLOAT > ( VAR_fMouseSensitivity, fSensitivity );  //     *(FLOAT *)VAR_fMouseSensitivity = fSensitivity;
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
