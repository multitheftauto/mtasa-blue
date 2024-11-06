/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CSettings.h
 *  PURPOSE:     Game settings interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#if (!defined(RWFORCEENUMSIZEINT))
#define RWFORCEENUMSIZEINT ((int)((~((unsigned int)0))>>1))
#endif /* (!defined(RWFORCEENUMSIZEINT)) */

enum VideoModeFlag            // RwVideoModeFlag
{
    rwVIDEOMODEEXCLUSIVE = 0x0001,   /**<Exclusive (i.e. full-screen) */
    rwVIDEOMODEINTERLACE = 0x0002,   /**<Interlaced                   */
    rwVIDEOMODEFFINTERLACE = 0x0004, /**<Flicker Free Interlaced      */

    /* Platform specific video mode flags. */

    rwVIDEOMODE_PS2_FSAASHRINKBLIT = 0x0100,
    /**< \if sky2
     *   Full-screen antialiasing mode 0
     *   \endif
     */
    rwVIDEOMODE_PS2_FSAAREADCIRCUIT = 0x0200,
    /**< \if sky2
     *   Full-screen antialiasing mode 1
     *   \endif
     */

    rwVIDEOMODE_XBOX_WIDESCREEN = 0x0100,
    /**< \if xbox
     *   Wide screen.
     *   \endif
     */
    rwVIDEOMODE_XBOX_PROGRESSIVE = 0x0200,
    /**< \if xbox
     *   Progressive.
     *   \endif
     */
    rwVIDEOMODE_XBOX_FIELD = 0x0400,
    /**< \if xbox
     *   Field rendering.
     *   \endif
     */
    rwVIDEOMODE_XBOX_10X11PIXELASPECT = 0x0800,
    /**< \if xbox
     *   The frame buffer is centered on the display.
     *   On a TV that is 704 pixels across, this would leave 32 pixels of black
     *   border on the left and 32 pixels of black border on the right.
     *   \endif
     */

    rwVIDEOMODEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

struct VideoMode            // RwVideoMode
{
    int           width;   /**< Width  */
    int           height;  /**< Height */
    int           depth;   /**< Depth  */
    VideoModeFlag flags;   /**< Flags  */
    int           refRate; /**< Approximate refresh rate */
    int           format;  /**< Raster format
                            * \see RwRasterFormat
                            */
};

enum eAspectRatio
{
    ASPECT_RATIO_AUTO,
    ASPECT_RATIO_4_3,
    ASPECT_RATIO_16_10,
    ASPECT_RATIO_16_9,
};

enum eRadarMode
{
    RADAR_MODE_ALL = 0,
    RADAR_MODE_BLIPS_ONLY,
    RADAR_MODE_NO_HUD
};

class CGameSettings
{
public:
    virtual bool          IsWideScreenEnabled() = 0;
    virtual void          SetWideScreenEnabled(bool bEnabled) = 0;
    virtual unsigned int  GetNumVideoModes() = 0;
    virtual VideoMode*    GetVideoModeInfo(VideoMode* modeInfo, unsigned int modeIndex) = 0;
    virtual unsigned int  GetCurrentVideoMode() = 0;
    virtual void          SetCurrentVideoMode(unsigned int modeIndex, bool bOnRestart) = 0;
    virtual unsigned int  GetNumAdapters() = 0;
    virtual unsigned int  GetCurrentAdapter() = 0;
    virtual bool          HasUnsafeResolutions() = 0;
    virtual bool          IsUnsafeResolution(int iWidth, int iHeight) = 0;
    virtual void          SetAdapter(unsigned int uiAdapterIndex) = 0;
    virtual unsigned char GetRadioVolume() = 0;
    virtual void          SetRadioVolume(unsigned char ucVolume) = 0;
    virtual unsigned char GetSFXVolume() = 0;
    virtual void          SetSFXVolume(unsigned char ucVolume) = 0;
    virtual unsigned int  GetUsertrackMode() = 0;
    virtual void          SetUsertrackMode(unsigned int uiMode) = 0;
    virtual bool          IsUsertrackAutoScan() = 0;
    virtual void          SetUsertrackAutoScan(bool bEnable) = 0;
    virtual bool          IsRadioEqualizerEnabled() = 0;
    virtual void          SetRadioEqualizerEnabled(bool bEnable) = 0;
    virtual bool          IsRadioAutotuneEnabled() = 0;
    virtual void          SetRadioAutotuneEnabled(bool bEnable) = 0;

    virtual float GetDrawDistance() = 0;
    virtual void  SetDrawDistance(float fDrawDistance) = 0;

    virtual unsigned int GetBrightness() = 0;
    virtual void         SetBrightness(unsigned int uiBrightness) = 0;

    virtual unsigned int GetFXQuality() = 0;
    virtual void         SetFXQuality(unsigned int fxQualityId) = 0;

    virtual float GetMouseSensitivity() = 0;
    virtual void  SetMouseSensitivity(float fSensitivity) = 0;

    virtual unsigned int GetAntiAliasing() = 0;
    virtual void         SetAntiAliasing(unsigned int uiAntiAliasing, bool bOnRestart) = 0;

    virtual bool IsMipMappingEnabled() = 0;
    virtual void SetMipMappingEnabled(bool bEnable) = 0;

    virtual bool IsVolumetricShadowsEnabled() const noexcept = 0;
    virtual bool GetVolumetricShadowsEnabledByVideoSetting() const noexcept = 0;
    virtual void SetVolumetricShadowsEnabled(bool bEnable) = 0;
    virtual void SetVolumetricShadowsSuspended(bool bSuspended) = 0;
    virtual bool ResetVolumetricShadows() noexcept = 0;

    virtual bool IsDynamicPedShadowsEnabled() = 0;
    virtual void SetDynamicPedShadowsEnabled(bool bEnable) = 0;
    virtual bool IsDynamicPedShadowsEnabledByVideoSetting() const noexcept = 0;
    virtual bool ResetDynamicPedShadows()  noexcept = 0;

    virtual float        GetAspectRatioValue() = 0;
    virtual eAspectRatio GetAspectRatio() = 0;
    virtual void         SetAspectRatio(eAspectRatio aspectRatio, bool bAdjustmentEnabled = true) = 0;

    virtual bool IsGrassEnabled() = 0;
    virtual void SetGrassEnabled(bool bEnable) = 0;

    virtual eRadarMode GetRadarMode() = 0;
    virtual void       SetRadarMode(eRadarMode hudMode) = 0;

    virtual void UpdateFieldOfViewFromSettings() = 0;
    virtual void ResetFieldOfViewFromScript() = 0;
    virtual void SetFieldOfViewPlayer(float fAngle, bool bFromScript) = 0;
    virtual void SetFieldOfViewVehicle(float fAngle, bool bFromScript) = 0;
    virtual void SetFieldOfViewVehicleMax(float fAngle, bool bFromScript) = 0;

    virtual float GetFieldOfViewPlayer() = 0;
    virtual float GetFieldOfViewVehicle() = 0;
    virtual float GetFieldOfViewVehicleMax() = 0;

    virtual void SetVehiclesLODDistance(float fVehiclesLODDistance, float fTrainsPlanesLODDistance, bool bFromScript) = 0;
    virtual void ResetVehiclesLODDistance(bool bForceDefault = false) = 0;
    virtual void GetVehiclesLODDistance(float& fVehiclesLODDistance, float& fTrainsPlanesLODDistance) = 0;

    virtual void  SetPedsLODDistance(float fPedsLODDistance, bool bFromScript) = 0;
    virtual void  ResetPedsLODDistance(bool bForceDefault = false) = 0;
    virtual float GetPedsLODDistance() = 0;

    virtual void ResetBlurEnabled() = 0;
    virtual void SetBlurControlledByScript(bool bByScript) = 0;

    virtual void ResetCoronaReflectionsEnabled() = 0;
    virtual void SetCoronaReflectionsControlledByScript(bool bViaScript) = 0;

    virtual void Save() = 0;
};
