/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA.h
 *  PURPOSE:     Multiplayer module class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CGame.h>

#include "multiplayersa_init.h"
#include "CLimitsSA.h"

#include "CRemoteDataSA.h"

class CRemoteDataSA;
#define DEFAULT_NEAR_CLIP_DISTANCE  ( 0.3f )
#define DEFAULT_SHADOWS_OFFSET      ( 0.013f ) // GTA default = 0.06f

enum eRadioStationID
{
    UNKNOWN = 0,
    Playback_FM,
    K_Rose,
    K_DST,
    BOUNCE_FM,
    SF_UR,
    RLS,
    RADIO_X,
    CSR_1039,
    K_JAH_WEST,
    Master_Sounds,
    WCTR,
};

typedef void*(__cdecl* hCAnimBlendAssociation_NewOperator)(size_t iSizeInBytes);

class CMultiplayerSA : public CMultiplayer
{
    friend class COffsetsMP;

private:
    CRemoteDataSA* RemoteData;

public:
    ZERO_ON_NEW

    CMultiplayerSA();
    void                InitHooks();
    void                InitHooks_CrashFixHacks();
    void                Init_13();
    void                InitHooks_13();
    void                InitMemoryCopies_13();
    void                InitHooks_ClothesSpeedUp();
    void                InitHooks_FixBadAnimId();
    void                InitHooks_HookDestructors();
    void                InitHooks_RwResources();
    void                InitHooks_ClothesCache();
    void                InitHooks_Files();
    void                InitHooks_Weapons();
    void                InitHooks_Peds();
    void                InitHooks_ObjectCollision();
    void                InitHooks_VehicleCollision();
    void                InitHooks_VehicleDummies();
    void                InitHooks_Vehicles();
    void                InitHooks_Rendering();
    void                InitHooks_LicensePlate();
    void                InitHooks_VehicleLights();
    void                InitHooks_VehicleDamage();
    void                InitHooks_VehicleWeapons();
    void                InitHooks_Direct3D();
    void                InitHooks_FixLineOfSightArgs();
    void                InitHooks_Streaming();
    void                InitHooks_FrameRateFixes();
    void                InitHooks_ProjectileCollisionFix();
    void                InitHooks_ObjectStreamerOptimization();
    void                InitHooks_Postprocess();
    void                InitHooks_DeviceSelection();
    CRemoteDataStorage* CreateRemoteDataStorage();
    void                DestroyRemoteDataStorage(CRemoteDataStorage* pData);
    void                AddRemoteDataStorage(CPlayerPed* pPed, CRemoteDataStorage* pData);
    CRemoteDataStorage* GetRemoteDataStorage(CPlayerPed* pPed);
    void                RemoveRemoteDataStorage(CPlayerPed* pPed);
    void                EnableHooks_ClothesMemFix(bool bEnable);

    CPed* GetContextSwitchedPed();

    void PreventLeavingVehicles();
    void HideRadar(bool bHide);
    void SetCenterOfWorld(CEntity* entity, CVector* vecPosition, FLOAT fHeading);
    void DisablePadHandler(bool bDisabled);
    void DisableEnterExitVehicleKey(bool bDisabled);
    void DisableAllVehicleWeapons(bool bDisable);
    void DisableBirds(bool bDisabled);
    void DisableQuickReload(bool bDisable);
    void DisableCloseRangeDamage(bool bDisable);
    void DisableBadDrivebyHitboxes(bool bDisable) { m_bBadDrivebyHitboxesDisabled = bDisable; }

    bool GetExplosionsDisabled();
    void DisableExplosions(bool bDisabled);
    void SetExplosionHandler(ExplosionHandler* pExplosionHandler);
    void SetDamageHandler(DamageHandler* pDamageHandler);
    void SetDeathHandler(DeathHandler* pDeathHandler);
    void SetProjectileHandler(ProjectileHandler* pProjectileHandler);
    void SetProjectileStopHandler(ProjectileStopHandler* pProjectileHandler);
    void SetFireHandler(FireHandler* pFireHandler);
    void SetBreakTowLinkHandler(BreakTowLinkHandler* pBreakTowLinkHandler);
    void SetProcessCamHandler(ProcessCamHandler* pProcessCamHandler);
    void SetChokingHandler(ChokingHandler* pChokingHandler);
    void SetPreWorldProcessHandler(PreWorldProcessHandler* pHandler);
    void SetPostWorldProcessHandler(PostWorldProcessHandler* pHandler);
    void SetPostWorldProcessPedsAfterPreRenderHandler(PostWorldProcessPedsAfterPreRenderHandler* pHandler);
    void SetIdleHandler(IdleHandler* pHandler);
    void SetPreFxRenderHandler(PreFxRenderHandler* pHandler);
    void SetPostColorFilterRenderHandler(PostColorFilterRenderHandler* pHandler) override;
    void SetPreHudRenderHandler(PreHudRenderHandler* pHandler);
    void DisableCallsToCAnimBlendNode(bool bDisableCalls);
    void SetCAnimBlendAssocDestructorHandler(CAnimBlendAssocDestructorHandler* pHandler);
    void SetAddAnimationHandler(AddAnimationHandler* pHandler);
    void SetAddAnimationAndSyncHandler(AddAnimationAndSyncHandler* pHandler);
    void SetAssocGroupCopyAnimationHandler(AssocGroupCopyAnimationHandler* pHandler);
    void SetBlendAnimationHierarchyHandler(BlendAnimationHierarchyHandler* pHandler);
    void SetProcessCollisionHandler(ProcessCollisionHandler* pHandler);
    void SetVehicleCollisionHandler(VehicleCollisionHandler* pHandler);
    void SetVehicleDamageHandler(VehicleDamageHandler* pHandler);
    void SetHeliKillHandler(HeliKillHandler* pHandler);
    void SetObjectDamageHandler(ObjectDamageHandler* pHandler);
    void SetObjectBreakHandler(ObjectBreakHandler* pHandler);
    void SetWaterCannonHitHandler(WaterCannonHitHandler* pHandler);
    void SetVehicleFellThroughMapHandler(VehicleFellThroughMapHandler* pHandler);
    void SetGameObjectDestructHandler(GameObjectDestructHandler* pHandler);
    void SetGameVehicleDestructHandler(GameVehicleDestructHandler* pHandler);
    void SetGamePlayerDestructHandler(GamePlayerDestructHandler* pHandler);
    void SetGameProjectileDestructHandler(GameProjectileDestructHandler* pHandler);
    void SetGameModelRemoveHandler(GameModelRemoveHandler* pHandler);
    void SetGameRunNamedAnimDestructorHandler(GameRunNamedAnimDestructorHandler* pHandler);
    void SetGameEntityRenderHandler(GameEntityRenderHandler* pHandler);
    void SetFxSystemDestructionHandler(FxSystemDestructionHandler* pHandler);
    void SetDrivebyAnimationHandler(DrivebyAnimationHandler* pHandler);
    void SetPedStepHandler(PedStepHandler* pHandler);
    void SetVehicleWeaponHitHandler(VehicleWeaponHitHandler* pHandler) override;
    void SetAudioZoneRadioSwitchHandler(AudioZoneRadioSwitchHandler* pHandler);

    void  AllowMouseMovement(bool bAllow);
    void  DoSoundHacksOnLostFocus(bool bLostFocus);
    bool  HasSkyColor();
    void  GetSkyColor(unsigned char& TopRed, unsigned char& TopGreen, unsigned char& TopBlue, unsigned char& BottomRed, unsigned char& BottomGreen,
                      unsigned char& BottomBlue);
    void  SetSkyColor(unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen,
                      unsigned char BottomBlue);
    void  SetHeatHaze(const SHeatHazeSettings& settings);
    void  GetHeatHaze(SHeatHazeSettings& settings);
    void  ResetColorFilter();
    void  SetColorFilter(DWORD dwPass0Color, DWORD dwPass1Color);
    void  GetColorFilter(DWORD& dwPass0Color, DWORD& dwPass1Color, bool isOriginal);
    void  SetGrainMultiplier(eGrainMultiplierType type, float fMultiplier);
    void  SetGrainLevel(BYTE ucLevel);
    void  ResetHeatHaze();
    void  SetHeatHazeEnabled(bool bEnabled);
    void  ApplyHeatHazeEnabled();
    void  ResetSky();
    bool  HasWaterColor();
    void  GetWaterColor(float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha);
    void  SetWaterColor(float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha);
    void  ResetWater();
    void  SetCloudsEnabled(bool bDisabled);
    void  RebuildMultiplayerPlayer(CPed* player);
    bool  GetInteriorSoundsEnabled();
    void  SetInteriorSoundsEnabled(bool bEnabled);
    bool  GetInteriorFurnitureEnabled(char cRoomId);
    void  SetInteriorFurnitureEnabled(char cRoomId, bool bEnabled);
    void  SetWindVelocity(float fX, float fY, float fZ);
    void  GetWindVelocity(float& fX, float& fY, float& fZ);
    void  RestoreWindVelocity();
    float GetFarClipDistance();
    void  SetFarClipDistance(float fDistance);
    void  RestoreFarClipDistance();
    float GetNearClipDistance();
    void  SetNearClipDistance(float fDistance);
    void  RestoreNearClipDistance();
    float GetFogDistance();
    void  SetFogDistance(float fDistance);
    void  RestoreFogDistance();
    void  GetSunColor(unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed, unsigned char& ucCoronaGreen,
                      unsigned char& ucCoronaBlue);
    void  SetSunColor(unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed, unsigned char ucCoronaGreen,
                      unsigned char ucCoronaBlue);
    void  ResetSunColor();
    float GetSunSize();
    void  SetSunSize(float fSize);
    void  ResetSunSize();
    void  SetMoonSize(int iSize);
    int   GetMoonSize();
    void  ResetMoonSize();

    void  GetAmbientColor(float& red, float& green, float& blue) const;
    bool  SetAmbientColor(float red, float green, float blue);
    bool  ResetAmbientColor();

    void  GetAmbientObjectColor(float& red, float& green, float& blue) const;
    bool  SetAmbientObjectColor(float red, float green, float blue);
    bool  ResetAmbientObjectColor();

    void  GetDirectionalColor(float& red, float& green, float& blue) const;
    bool  SetDirectionalColor(float red, float green, float blue);
    bool  ResetDirectionalColor();

    float GetSpriteSize() const;
    bool  SetSpriteSize(float size);
    bool  ResetSpriteSize();

    float GetSpriteBrightness() const;
    bool  SetSpriteBrightness(float brightness);
    bool  ResetSpriteBrightness();

    int16 GetPoleShadowStrength() const;
    bool  SetPoleShadowStrength(int16 strength);
    bool  ResetPoleShadowStrength();

    int16 GetShadowStrength() const;
    bool  SetShadowStrength(int16 strength);
    bool  ResetShadowStrength();

    float GetShadowsOffset() const;
    bool  SetShadowsOffset(float offset);
    bool  ResetShadowsOffset();

    float GetLightsOnGroundBrightness() const;
    bool  SetLightsOnGroundBrightness(float brightness);
    bool  ResetLightsOnGroundBrightness();

    void  GetLowCloudsColor(int16& red, int16& green, int16& blue) const;
    bool  SetLowCloudsColor(int16 red, int16 green, int16 blue);
    bool  ResetLowCloudsColor();

    void  GetBottomCloudsColor(int16& red, int16& green, int16& blue) const;
    bool  SetBottomCloudsColor(int16 red, int16 green, int16 blue);
    bool  ResetBottomCloudsColor();

    float GetCloudsAlpha1() const;
    bool  SetCloudsAlpha1(float alpha);
    bool  ResetCloudsAlpha1();

    float GetIllumination() const;
    bool  SetIllumination(float illumination);
    bool  ResetIllumination();

    void SetNightVisionEnabled(bool bEnabled, bool bNoiseEnabled);
    void SetThermalVisionEnabled(bool bEnabled, bool bNoiseEnabled);
    bool IsNightVisionEnabled();
    bool IsThermalVisionEnabled();

    void AllowWindowsCursorShowing(bool bAllow);

    CShotSyncData* GetLocalShotSyncData();

    void SetPreContextSwitchHandler(PreContextSwitchHandler* pHandler);
    void SetPostContextSwitchHandler(PostContextSwitchHandler* pHandler);
    void SetPreWeaponFireHandler(PreWeaponFireHandler* pHandler);
    void SetPostWeaponFireHandler(PostWeaponFireHandler* pHandler);
    void SetBulletImpactHandler(BulletImpactHandler* pHandler);
    void SetBulletFireHandler(BulletFireHandler* pHandler);
    void SetDrawRadarAreasHandler(DrawRadarAreasHandler* pRadarAreasHandler);
    void SetRender3DStuffHandler(Render3DStuffHandler* pHandler);
    void SetPreRenderSkyHandler(PreRenderSkyHandler* pHandler);
    void SetRenderHeliLightHandler(RenderHeliLightHandler* pHandler);
    void SetRenderEverythingBarRoadsHandler(RenderEverythingBarRoadsHandler* pHandler) override;

    void Reset();

    void ConvertEulerAnglesToMatrix(CMatrix& Matrix, float fX, float fY, float fZ);
    void ConvertMatrixToEulerAngles(const CMatrix& Matrix, float& fX, float& fY, float& fZ);

    void ConvertEulerAnglesToMatrix(CMatrix& Matrix, CVector& vecAngles) { ConvertEulerAnglesToMatrix(Matrix, vecAngles.fX, vecAngles.fY, vecAngles.fZ); }
    void ConvertMatrixToEulerAngles(const CMatrix& Matrix, CVector& vecAngles) { ConvertMatrixToEulerAngles(Matrix, vecAngles.fX, vecAngles.fY, vecAngles.fZ); }

    float GetGlobalGravity();
    void  SetGlobalGravity(float fGravity);

    float GetLocalPlayerGravity();
    void  SetLocalPlayerGravity(float fGravity);

    unsigned char GetTrafficLightState();
    void          SetTrafficLightState(unsigned char ucState);

    bool GetTrafficLightsLocked();
    void SetTrafficLightsLocked(bool bLocked);

    void  SetLocalStatValue(unsigned short usStat, float fValue);
    float GetLocalStatValue(unsigned short usStat);
    void  SetLocalStatsStatic(bool bStatic);

    void SetLocalCameraRotation(float fRotation);
    bool IsCustomCameraRotationEnabled();
    void SetCustomCameraRotationEnabled(bool bEnabled);

    void SetDebugVars(float f1, float f2, float f3);

    CVector& GetAkimboTarget() { return m_vecAkimboTarget; };
    bool     GetAkimboTargetUp() { return m_bAkimboTargetUp; };

    void SetAkimboTarget(const CVector& vecTarget) { m_vecAkimboTarget = vecTarget; };
    void SetAkimboTargetUp(bool bUp) { m_bAkimboTargetUp = bUp; };

    void AllowCreatedObjectsInVerticalLineTest(bool bOn);
    void DeleteAndDisableGangTags();

    CLimits* GetLimits() { return &m_limits; }

    void UpdateVehicleSuspension() const noexcept;

    virtual void FlushClothesCache();
    virtual void SetFastClothesLoading(EFastClothesLoading fastClothesLoading);
    virtual void SetLODSystemEnabled(bool bEnable);
    virtual void SetAltWaterOrderEnabled(bool bEnable);

    float GetAircraftMaxHeight() { return m_fAircraftMaxHeight; };
    void  SetAircraftMaxHeight(float fHeight) { m_fAircraftMaxHeight = fHeight; };

    float GetAircraftMaxVelocity() { return m_fAircraftMaxVelocity; };
    void  SetAircraftMaxVelocity(float fVelocity)
    {
        m_fAircraftMaxVelocity = fVelocity;
        m_fAircraftMaxVelocity_Sq = fVelocity * fVelocity;
    };

    void SetAutomaticVehicleStartupOnPedEnter(bool bSet);

    void SetPedTargetingMarkerEnabled(bool bEnable);
    bool IsPedTargetingMarkerEnabled();
    bool IsConnected();

    virtual void GetRwResourceStats(SRwResourceStats& outStats);
    virtual void GetClothesCacheStats(SClothesCacheStats& outStats);
    virtual void SetIsMinimizedAndNotConnected(bool bIsMinimizedAndNotConnected);
    virtual void SetMirrorsEnabled(bool bEnabled);

    void SetBoatWaterSplashEnabled(bool bEnabled);
    void SetTyreSmokeEnabled(bool bEnabled);

    void SetLastStaticAnimationPlayed(eAnimGroup dwGroupID, eAnimID dwAnimID, DWORD dwAnimArrayAddress)
    {
        m_dwLastStaticAnimGroupID = dwGroupID;
        m_dwLastStaticAnimID = dwAnimID;
        m_dwLastAnimArrayAddress = dwAnimArrayAddress;
    }
    eAnimGroup GetLastStaticAnimationGroupID() { return m_dwLastStaticAnimGroupID; }
    eAnimID    GetLastStaticAnimationID() { return m_dwLastStaticAnimID; }
    DWORD      GetLastAnimArrayAddress() { return m_dwLastAnimArrayAddress; }

    unsigned int EntryInfoNodePool_NoOfUsedSpaces() const noexcept override;
    unsigned int PtrNodeSingleLinkPool_NoOfUsedSpaces() const noexcept override;
    unsigned int PtrNodeDoubleLinkPool_NoOfUsedSpaces() const noexcept override;

    CVector      m_vecAkimboTarget;
    bool         m_bAkimboTargetUp;
    static char* ms_PlayerImgCachePtr;
    bool         m_bBadDrivebyHitboxesDisabled;

private:
    std::vector<char>   m_PlayerImgCache;
    EFastClothesLoading m_FastClothesLoading;
    CLimitsSA           m_limits;
    bool                m_bEnabledLODSystem;
    bool                m_bEnabledAltWaterOrder;
    bool                m_bEnabledClothesMemFix;
    float               m_fAircraftMaxHeight;
    float               m_fAircraftMaxVelocity;
    float               m_fAircraftMaxVelocity_Sq;
    bool                m_bHeatHazeEnabled;
    bool                m_bHeatHazeCustomized;
    float               m_fNearClipDistance;
    float               m_fMaddDoggPoolLevel;
    eAnimGroup          m_dwLastStaticAnimGroupID;
    eAnimID             m_dwLastStaticAnimID;
    DWORD               m_dwLastAnimArrayAddress;
    float               m_fShadowsOffset;

    /*  VOID                        SetPlayerShotVectors(CPlayerPed* player, Vector3D * vecTarget, Vector3D * vecStart);
        VOID                        SetPlayerCameraVectors(CPlayerPed* player, Vector3D * vecSource, Vector3D * vecFront);
        Vector3D                    * GetLocalShotOriginVector();*/
    // Vector3D                  * GetLocalTargetVector();
    // Vector3D                  * GetLocalCrossProductVector();
    /*VOID                      SetOnFootPlayerKeyStates(CPlayerPed * player, DWORD keys);
    DWORD                       GetOnFootLocalPlayerKeyStates();
    VOID                        SetInVehiclePlayerKeyStates(CPlayerPed * player, WORD keys);
    WORD                        GetInVehicleLocalPlayerKeyStates();*/
    //  Vector3D                    * GetLocalStartVector();
    //  VOID                        SetPlayerStartVector(CPlayerPed* player, Vector3D * vecStart);
};
