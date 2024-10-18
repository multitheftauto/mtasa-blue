/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CSettingsSA.h
 *  PURPOSE:     Header file for game settings class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// R* have this info inside CMenuManager but I can't believe that makes much sense

#include <game/CSettings.h>

#define CLASS_CMenuManager      0xBA6748

#define FUNC_CMenuManager_Save  0x57C660

#define VAR_ucFxQuality         0xA9AE54
#define VAR_fMouseSensitivity   0xB6EC1C
#define VAR_RadarMode           0xBA676C

#define CLASS_CAudioEngine 0xB6BC90
#define FUNC_CAudioEngine_SetEffectsMasterVolume 0x506E10
#define FUNC_CAudioEngine_SetMusicMasterVolume 0x506DE0

#define CLASS_CGamma 0xC92134
#define FUNC_CGamma_SetGamma 0x747200

#define FUNC_SetAntiAliasing    0x7F8A90

#define DEFAULT_VEHICLE_LOD_DISTANCE    ( 70.0f )
#define DEFAULT_PEDS_LOD_DISTANCE       ( 60.0f )
// Default train distance is 150, so make it relative to default vehicle distance
#define TRAIN_LOD_DISTANCE_MULTIPLIER   ( 2.14f )
#define MAX_VEHICLE_LOD_DISTANCE        ( 500.0f )
#define MAX_PEDS_LOD_DISTANCE           ( 500.0f )
#define DEFAULT_BLUR_LEVEL              ( 36 )

struct CSettingsSAInterface            // see code around 0x57CE9A for where these are
{
    BYTE  pad1[4];
    float fStatsScrollSpeed;            // 0x4
    BYTE  pad2[0x34];
    DWORD dwBrightness;
    float fDrawDistance;
    bool  bSubtitles;            // 0x44
    bool  pad3[5];
    bool  bLegend;                   // 0x4A
    bool  bUseWideScreen;            // 0x4B
    bool  bFrameLimiter;             // 0x4C
    bool  bRadioAutotune;            // 0x4D
    bool  pad4;
    BYTE  ucSfxVolume;                // 0x4F
    BYTE  ucRadioVolume;              // 0x50
    bool  bRadioEqualizer;            // 0x51
    BYTE  ucRadioStation;             // 0x52
    BYTE  pad5[0x5D];
    BYTE  ucUsertrackMode;            // 0xB0
    bool  bInvertPadX1;               // 0xB1
    bool  bInvertPadY1;               // 0xB2
    bool  bInvertPadX2;               // 0xB3
    bool  bInvertPadY2;               // 0xB4
    bool  bSwapPadAxis1;              // 0xB5
    bool  bSwapPadAxis2;              // 0xB6
    BYTE  pad6[0xD];
    bool  bMipMapping;                   // 0xC4 (setting appears to have no effect in gta 1.0)
    bool  bUsertrackAutoScan;            // 0xC5
    BYTE  pad7[0x2];
    DWORD dwAntiAliasing;                  // 0xC8
    DWORD dwFrontendAA;                    // 0xCC (anti-aliasing value in the single-player settings menu. Useless for MTA).
    bool  bUseKeyboardAndMouse;            // 0xD0
    BYTE  pad8[3];
    DWORD dwVideoMode;                // 0xD4
    DWORD dwPrevVideoMode;            // 0xD8
};

class CSettingsSA : public CGameSettings
{
    friend class COffsets;

private:
    CSettingsSAInterface* m_pInterface;
    bool                  m_bVolumetricShadowsEnabled;
    bool                  m_bVolumetricShadowsSuspended;
    bool                  m_bDynamicPedShadowsEnabled;
    eAspectRatio          m_AspectRatio;
    int                   m_iDesktopWidth;
    int                   m_iDesktopHeight;
    bool                  m_bBlurViaScript;
    bool                  m_bCoronaReflectionsViaScript;

public:
    CSettingsSA();

    bool          IsWideScreenEnabled();
    void          SetWideScreenEnabled(bool bEnabled);
    unsigned int  GetNumVideoModes();
    VideoMode*    GetVideoModeInfo(VideoMode* modeInfo, unsigned int modeIndex);
    unsigned int  GetCurrentVideoMode();
    void          SetCurrentVideoMode(unsigned int modeIndex, bool bOnRestart);
    unsigned int  GetNumAdapters();
    unsigned int  GetCurrentAdapter();
    void          SetAdapter(unsigned int uiAdapterIndex);
    bool          HasUnsafeResolutions();
    bool          IsUnsafeResolution(int iWidth, int iHeight);
    unsigned char GetRadioVolume();
    void          SetRadioVolume(unsigned char ucVolume);
    unsigned char GetSFXVolume();
    void          SetSFXVolume(unsigned char ucVolume);
    unsigned int  GetUsertrackMode();
    void          SetUsertrackMode(unsigned int uiMode);
    bool          IsUsertrackAutoScan();
    void          SetUsertrackAutoScan(bool bEnable);
    bool          IsRadioEqualizerEnabled();
    void          SetRadioEqualizerEnabled(bool bEnable);
    bool          IsRadioAutotuneEnabled();
    void          SetRadioAutotuneEnabled(bool bEnable);

    float GetDrawDistance();
    void  SetDrawDistance(float fDrawDistance);

    unsigned int GetBrightness();
    void         SetBrightness(unsigned int uiBrightness);

    unsigned int GetFXQuality();
    void         SetFXQuality(unsigned int fxQualityId);

    float GetMouseSensitivity();
    void  SetMouseSensitivity(float fSensitivity);

    unsigned int GetAntiAliasing();
    void         SetAntiAliasing(unsigned int uiAntiAliasing, bool bOnRestart);

    bool IsMipMappingEnabled();
    void SetMipMappingEnabled(bool bEnable);

    bool IsVolumetricShadowsEnabled() const noexcept;
    bool GetVolumetricShadowsEnabledByVideoSetting() const noexcept;
    bool ResetVolumetricShadows() noexcept;
 
    void SetVolumetricShadowsEnabled(bool bEnable);
    void SetVolumetricShadowsSuspended(bool bSuspended);

    bool IsDynamicPedShadowsEnabled();
    void SetDynamicPedShadowsEnabled(bool bEnable);
    bool IsDynamicPedShadowsEnabledByVideoSetting() const noexcept;
    bool ResetDynamicPedShadows() noexcept;

    float        GetAspectRatioValue();
    eAspectRatio GetAspectRatio();
    void         SetAspectRatio(eAspectRatio aspectRatio, bool bAdjustmentEnabled = true);

    bool IsGrassEnabled();
    void SetGrassEnabled(bool bEnable);

    eRadarMode GetRadarMode();
    void       SetRadarMode(eRadarMode hudMode);

    void  UpdateFieldOfViewFromSettings();
    void  ResetFieldOfViewFromScript();
    void  SetFieldOfViewPlayer(float fAngle, bool bFromScript);
    void  SetFieldOfViewVehicle(float fAngle, bool bFromScript);
    void  SetFieldOfViewVehicleMax(float fAngle, bool bFromScript);
    float GetFieldOfViewPlayer();
    float GetFieldOfViewVehicle();
    float GetFieldOfViewVehicleMax();

    void SetVehiclesLODDistance(float fVehiclesLODDistance, float fTrainsPlanesLODDistance, bool bFromScript);
    void ResetVehiclesLODDistance(bool bForceDefault = false);
    void GetVehiclesLODDistance(float& fVehiclesLODDistance, float& fTrainsPlanesLODDistance);

    void ResetBlurEnabled();
    void SetBlurControlledByScript(bool bByScript);

    void ResetCoronaReflectionsEnabled();
    void SetCoronaReflectionsControlledByScript(bool bViaScript);

    void Save();

    void  SetPedsLODDistance(float fPedsLODDistance, bool bFromScript);
    void  ResetPedsLODDistance(bool bForceDefault = false);
    float GetPedsLODDistance();

    static void StaticSetHooks();

    uint FindVideoMode(int iResX, int iResY, int iColorBits);
    void SetValidVideoMode();
    int  OnSelectDevice();
};
