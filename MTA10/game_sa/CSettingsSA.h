/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CSettingsSA.h
*  PURPOSE:     Header file for game settings class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
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

#define FUNC_CMenuManager_Save  0x57C660

#define VAR_bMouseSteering      0xC1CC02
#define VAR_bMouseFlying        0xC1CC03
#define VAR_fFxQuality          0xA9AE54

#define CLASS_CAudioEngine 0xB6BC90
#define FUNC_CAudioEngine_SetEffectsMasterVolume 0x506E10
#define FUNC_CAudioEngine_SetMusicMasterVolume 0x506DE0

#define CLASS_CGamma 0xC92134
#define FUNC_CGamma_SetGamma 0x747200

struct CSettingsSAInterface // see code around 0x57CE9A for where these are
{
    BYTE pad1[4];
    float fStatsScrollSpeed;    // 0x4
    BYTE pad2[0x34];
    DWORD dwBrightness;
    float fDrawDistance;
    bool bSubtitles;            // 0x44
    bool pad3[5];
    bool bLegend;               // 0x4A
    bool bUseWideScreen;        // 0x4B
    bool bFrameLimiter;         // 0x4C
    bool bAutoRetune;           // 0x4D
    bool pad4;
    BYTE ucSfxVolume;           // 0x4F
    BYTE ucRadioVolume;         // 0x50
    BYTE ucRadioEq;             // 0x51
    BYTE ucRadioStation;        // 0x52
    BYTE pad5[0x5E];
    bool bInvertPadX1;          // 0xB1
    bool bInvertPadY1;          // 0xB2
    bool bInvertPadX2;          // 0xB3
    bool bInvertPadY2;          // 0xB4
    bool bSwapPadAxis1;         // 0xB5
    bool bSwapPadAxis2;         // 0xB6
    BYTE pad6[0x19];
    bool bUseKeyboardAndMouse;  // 0xD0
    BYTE pad7[3];
    DWORD dwVideoMode;          // 0xD4
    DWORD dwPrevVideoMode;      // 0xD8
};

class CSettingsSA : public CGameSettings
{
    friend class            COffsets;

private:
    CSettingsSAInterface*   m_pInterface;

public:
                            CSettingsSA                 ( void );

    bool                    IsFrameLimiterEnabled       ( void );
    void                    SetFrameLimiterEnabled      ( bool bEnabled );
    bool                    IsWideScreenEnabled         ( void );
    void                    SetWideScreenEnabled        ( bool bEnabled );
    unsigned int            GetNumVideoModes            ( void );
    VideoMode *             GetVideoModeInfo            ( VideoMode * modeInfo, unsigned int modeIndex );
    unsigned int            GetCurrentVideoMode         ( void );
    void                    SetCurrentVideoMode         ( unsigned int modeIndex, bool bOnRestart );
    unsigned char           GetRadioVolume              ( void );
    void                    SetRadioVolume              ( unsigned char ucVolume );
    unsigned char           GetSFXVolume                ( void );
    void                    SetSFXVolume                ( unsigned char ucVolume );

    float                   GetDrawDistance             ( void );
    void                    SetDrawDistance             ( float fDrawDistance );
 
    unsigned int            GetBrightness               ( void );
    void                    SetBrightness               ( unsigned int uiBrightness );

    unsigned int            GetFXQuality                ( void );
    void                    SetFXQuality                ( unsigned int fxQualityId );

    void                    Save                        ( void );

private:
    static unsigned long    FUNC_GetNumVideoModes;
    static unsigned long    FUNC_GetVideoModeInfo;
    static unsigned long    FUNC_GetCurrentVideoMode;
    static unsigned long    FUNC_SetCurrentVideoMode;
    static unsigned long    FUNC_SetRadioVolume;
	static unsigned long    FUNC_SetDrawDistance;
};

#endif