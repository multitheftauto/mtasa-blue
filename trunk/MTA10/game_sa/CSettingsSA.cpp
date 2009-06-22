/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CSettingsSA.cpp
*  PURPOSE:     Game settings
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

unsigned long CSettingsSA::FUNC_GetNumVideoModes;
unsigned long CSettingsSA::FUNC_GetVideoModeInfo;
unsigned long CSettingsSA::FUNC_GetCurrentVideoMode;
unsigned long CSettingsSA::FUNC_SetCurrentVideoMode;

CSettingsSA::CSettingsSA ( void )
{
    m_pInterface = (CSettingsSAInterface *)CLASS_CMenuManager;
}

bool CSettingsSA::IsFrameLimiterEnabled ( void )
{
    return m_pInterface->bFrameLimiter;
}

void CSettingsSA::SetFrameLimiterEnabled ( bool bEnabled )
{
    m_pInterface->bFrameLimiter = bEnabled;
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

void CSettingsSA::SetCurrentVideoMode ( unsigned int modeIndex )
{
    _asm
    {
        push    modeIndex
        call    FUNC_SetCurrentVideoMode
        add     esp, 4
    }
    m_pInterface->dwVideoMode = modeIndex;
}

unsigned char CSettingsSA::GetRadioVolume ( void )
{
    return m_pInterface->ucRadioVolume;
}

void CSettingsSA::SetRadioVolume ( unsigned char ucVolume )
{
    m_pInterface->ucRadioVolume = ucVolume;
}

unsigned char CSettingsSA::GetSFXVolume ( void )
{
    return m_pInterface->ucSfxVolume;
}

void CSettingsSA::SetSFXVolume ( unsigned char ucVolume )
{
    m_pInterface->ucSfxVolume = ucVolume;
}

// Minimum is 0.925 and maximum is 1.8
float CSettingsSA::GetDrawDistance ( void )
{
    return m_pInterface->fDrawDistance;
}

void CSettingsSA::Save ()
{
    _asm
    {
        mov ecx, CLASS_CMenuManager
        mov eax, FUNC_SAVE
        call eax
    }
}
