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
    settings = (Settings *)CLASS_CMenuManager;
}

bool CSettingsSA::IsFrameLimiterEnabled ( void )
{
    return settings->bFrameLimiter;
}

void CSettingsSA::SetFrameLimiterEnabled ( bool bEnabled )
{
    settings->bFrameLimiter = bEnabled;
}

bool CSettingsSA::IsWideScreenEnabled ( void )
{
    return settings->bUseWideScreen;
}

void CSettingsSA::SetWideScreenEnabled ( bool bEnabled )
{
    settings->bUseWideScreen = bEnabled;
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
}

unsigned char CSettingsSA::GetRadioVolume ( void )
{
    return *(BYTE *)VAR_RadioVolume;
}

void CSettingsSA::SetRadioVolume ( unsigned char ucVolume )
{
    *(BYTE *)VAR_RadioVolume = ucVolume;
}

unsigned char CSettingsSA::GetSFXVolume ( void )
{
    return *(BYTE *)VAR_SfxVolume;
}

void CSettingsSA::SetSFXVolume ( unsigned char ucVolume )
{
    *(BYTE *)VAR_SfxVolume = ucVolume;
}