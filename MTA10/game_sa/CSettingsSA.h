/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CSettingsSA.h
*  PURPOSE:     Header file for game settings class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_SETTINGS
#define __CGAMESA_SETTINGS

// R* have this info inside CMenuManager but I can't believe that makes much sense

#include <game/CSettings.h>
#include "Common.h"

#define CLASS_CMenuManager      0xBA6748

#define VAR_bMouseSteering      0xC1CC02
#define VAR_bMouseFlying        0xC1CC03

#define VAR_RadioVolume         0xBA6798
#define VAR_SfxVolume           0xBA6798

struct Settings // see code around 0x57CE9A for where these are
{
    bool bPadding[75];
    bool bUseWideScreen;
    bool bFrameLimiter;
    bool bAutoRetune;
};

class CSettingsSA : public CGameSettings
{
    friend class    COffsets;

private:
    Settings    * settings;
public:
                    CSettingsSA ( void );

    bool            IsFrameLimiterEnabled ( void );
    void            SetFrameLimiterEnabled ( bool bEnabled );
    bool            IsWideScreenEnabled ( void );
    void            SetWideScreenEnabled ( bool bEnabled );
    unsigned int    GetNumVideoModes ( void );
    VideoMode *     GetVideoModeInfo ( VideoMode * modeInfo, unsigned int modeIndex );
    unsigned int    GetCurrentVideoMode ( void );
    void            SetCurrentVideoMode ( unsigned int modeIndex );
    unsigned char   GetRadioVolume ( void );
    void            SetRadioVolume ( unsigned char ucVolume );
    unsigned char   GetSFXVolume ( void );
    void            SetSFXVolume ( unsigned char ucVolume );

private:
    static unsigned long        FUNC_GetNumVideoModes;
    static unsigned long        FUNC_GetVideoModeInfo;
    static unsigned long        FUNC_GetCurrentVideoMode;
    static unsigned long        FUNC_SetCurrentVideoMode;
};

#endif