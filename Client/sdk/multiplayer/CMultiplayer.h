/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/multiplayer/CMultiplayer.h
 *  PURPOSE:     Multiplayer subsystem interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CMatrix.h>
#include <CVector.h>

#include <game/CExplosion.h>
#include <game/CStats.h>
#include "CLimits.h"
#include <../Client/game_sa/CAnimBlendAssociationSA.h>
#include <../Client/game_sa/CAnimBlendStaticAssociationSA.h>

class CEntitySAInterface;

struct SRwResourceStats
{
    uint uiTextures;
    uint uiRasters;
    uint uiGeometries;
};

struct SClothesCacheStats
{
    uint uiCacheHit;
    uint uiCacheMiss;
    uint uiNumTotal;
    uint uiNumUnused;
    uint uiNumRemoved;
};

enum EVehicleWeaponType : int
{
    INVALID,
    WATER_CANNON,
    TANK_GUN,
    ROCKET,
    HEAT_SEEKING_ROCKET,
};

enum class eGrainMultiplierType
{
    MASTER = 0,
    INFRARED,
    NIGHT,
    RAIN,
    OVERLAY,
    ALL
};

struct SVehicleWeaponHitEvent
{
    EVehicleWeaponType  weaponType;
    CEntitySAInterface* pGameVehicle;
    CEntitySAInterface* pHitGameEntity;
    CVector             vecPosition;
    int                 iModel;
    int                 iColSurface;
};

class CAnimBlendAssociationSAInterface;
class CAnimBlendStaticAssociationSAInterface;
class CAnimBlendAssocGroupSAInterface;
class CIFPAnimations;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;
enum class eAnimGroup;
enum class eAnimID;

typedef bool(ExplosionHandler)(class CEntity* pExplodingEntity, class CEntity* pCreator, const CVector& vecPosition, enum eExplosionType ExplosionType);
typedef void(PreContextSwitchHandler)(class CPlayerPed* pPlayer);
typedef void(PostContextSwitchHandler)();
typedef bool(PreWeaponFireHandler)(class CPlayerPed* pPlayer, bool bStopIfUsingBulletSync);
typedef void(PostWeaponFireHandler)();
typedef void(BulletImpactHandler)(class CPed* pInitiator, class CEntity* pVictim, const CVector* pvecStartPosition, const CVector* pvecEndPosition);
typedef void(BulletFireHandler)(class CPed* pInitiator, const CVector* pvecStartPosition, const CVector* pvecEndPosition);
typedef bool(DamageHandler)(class CPed* pDamagePed, class CEventDamage* pEvent);
typedef void(DeathHandler)(class CPed* pKilledPed, unsigned char ucDeathReason, unsigned char ucBodyPart);
typedef void(FireHandler)(class CFire* pFire);
typedef bool(ProjectileStopHandler)(class CEntity* owner, enum eWeaponType weaponType, class CVector* origin, float fForce, class CVector* target,
                                    class CEntity* targetEntity);
typedef void(ProjectileHandler)(class CEntity* owner, class CProjectile* projectile, class CProjectileInfo* projectileInfo, enum eWeaponType weaponType,
                                class CVector* origin, float fForce, class CVector* target, class CEntity* targetEntity);
typedef bool(BreakTowLinkHandler)(class CVehicle* towingVehicle);
typedef bool(ProcessCamHandler)(class CCam* pCam);
typedef void(DrawRadarAreasHandler)();
typedef void(Render3DStuffHandler)();
typedef void(PreRenderSkyHandler)();
typedef void(RenderHeliLightHandler)();
typedef bool(ChokingHandler)(unsigned char ucWeaponType);
typedef void(PreWorldProcessHandler)();
typedef void(PostWorldProcessHandler)();
typedef void(PostWorldProcessPedsAfterPreRenderHandler)();
typedef void(IdleHandler)();
typedef void(PreFxRenderHandler)();
typedef void(PostColorFilterRenderHandler)();
typedef void(PreHudRenderHandler)();
typedef void(RenderEverythingBarRoadsHandler)();
typedef CAnimBlendAssociationSAInterface*(AddAnimationHandler)(RpClump* pClump, AssocGroupId animGroup, AnimationId animID);
typedef CAnimBlendAssociationSAInterface*(AddAnimationAndSyncHandler)(RpClump* pClump, CAnimBlendAssociationSAInterface* pAnimAssocToSyncWith,
                                                                      AssocGroupId animGroup, AnimationId animID);
typedef void(CAnimBlendAssocDestructorHandler)(CAnimBlendAssociationSAInterface* pThis);
typedef bool(AssocGroupCopyAnimationHandler)(CAnimBlendAssociationSAInterface* pAnimAssoc, RpClump* pClump, CAnimBlendAssocGroupSAInterface* pAnimAssocGroup,
                                             eAnimID animID);
typedef bool(BlendAnimationHierarchyHandler)(CAnimBlendAssociationSAInterface* pAnimAssoc, CAnimBlendHierarchySAInterface** pOutAnimHierarchy, int* pFlags,
                                             RpClump* pClump);
typedef bool(ProcessCollisionHandler)(class CEntitySAInterface* pThisInterface, class CEntitySAInterface* pOtherInterface);
typedef bool(VehicleCollisionHandler)(class CVehicleSAInterface*& pCollidingVehicle, class CEntitySAInterface* pCollidedVehicle, int iModelIndex,
                                      float fDamageImpulseMag, float fCollidingDamageImpulseMag, uint16 usPieceType, CVector vecCollisionPos,
                                      CVector vecCollisionVelocity, bool isProjectile);
typedef bool(VehicleDamageHandler)(CEntitySAInterface* pVehicle, float fLoss, CEntitySAInterface* pAttacker, eWeaponType weaponType,
                                   const CVector& vecDamagePos, uchar ucTyre);
typedef bool(HeliKillHandler)(class CVehicleSAInterface* pVehicle, class CEntitySAInterface* pHitInterface);
typedef bool(ObjectDamageHandler)(class CObjectSAInterface* pObject, float fLoss, class CEntitySAInterface* pAttacker);
typedef bool(ObjectBreakHandler)(class CObjectSAInterface* pObject, class CEntitySAInterface* pAttacker);
typedef bool(WaterCannonHitHandler)(class CVehicleSAInterface* pCannonVehicle, class CPedSAInterface* pHitPed);
typedef bool(VehicleFellThroughMapHandler)(class CVehicleSAInterface* pVehicle);
typedef void(GameObjectDestructHandler)(CEntitySAInterface* pObject);
typedef void(GameVehicleDestructHandler)(CEntitySAInterface* pVehicle);
typedef void(GamePlayerDestructHandler)(CEntitySAInterface* pPlayer);
typedef void(GameProjectileDestructHandler)(CEntitySAInterface* pProjectile);
typedef void(GameModelRemoveHandler)(ushort usModelId);
typedef void(GameRunNamedAnimDestructorHandler)(class CTaskSimpleRunNamedAnimSAInterface* pTask);
typedef void(GameEntityRenderHandler)(CEntitySAInterface* pEntity);
typedef void(FxSystemDestructionHandler)(void* pFxSA);
typedef AnimationId(DrivebyAnimationHandler)(AnimationId animGroup, AssocGroupId animId);
typedef void(PedStepHandler)(CPedSAInterface* pPed, bool bFoot);
typedef void(AudioZoneRadioSwitchHandler)(DWORD dwStationID);

using VehicleWeaponHitHandler = void(SVehicleWeaponHitEvent& event);

enum class eVehicleAimDirection : unsigned char;

/**
 * This class contains information used for shot syncing, one exists per player.
 */
class CShotSyncData
{
public:
    CVector m_vecShotTarget;
    CVector m_vecShotOrigin;
    // so we can make the arm move vertically (mainly while on foot) and horizontally (mainly while in vehicles)
    float m_fArmDirectionX;
    float m_fArmDirectionY;
    // only for in-vehicle shooting
    eVehicleAimDirection m_cInVehicleAimDirection;
    // use origin
    bool m_bUseOrigin;

    bool    m_bRemoteBulletSyncVectorsValid;
    CVector m_vecRemoteBulletSyncStart;
    CVector m_vecRemoteBulletSyncEnd;
};

class CStatsData
{
public:
    float StatTypesFloat[MAX_FLOAT_STATS];
    int   StatTypesInt[MAX_INT_STATS];
    float StatReactionValue[MAX_REACTION_STATS];
};

class CRemoteDataStorage
{
public:
    virtual class CControllerState* CurrentControllerState() = 0;
    virtual class CControllerState* LastControllerState() = 0;
    virtual class CShotSyncData*    ShotSyncData() = 0;
    virtual class CStatsData*       Stats() = 0;
    virtual float                   GetCameraRotation() = 0;
    virtual void                    SetCameraRotation(float fCameraRotation) = 0;
    virtual void                    SetGravity(float fGravity) = 0;
    virtual void                    SetProcessPlayerWeapon(bool bProcess) = 0;
    virtual CVector&                GetAkimboTarget() = 0;
    virtual bool                    GetAkimboTargetUp() = 0;
    virtual void                    SetAkimboTarget(const CVector& vecTarget) = 0;
    virtual void                    SetAkimboTargetUp(bool bUp) = 0;
};

/**
 * \todo Remove the commented out functions if they aren't actually needed (from here and the source)
 */
class CMultiplayer
{
public:
    enum EFastClothesLoading
    {
        FAST_CLOTHES_OFF = 0,
        FAST_CLOTHES_AUTO = 1,
        FAST_CLOTHES_ON = 2,
    };

    virtual CRemoteDataStorage* CreateRemoteDataStorage() = 0;
    virtual void                DestroyRemoteDataStorage(CRemoteDataStorage* pData) = 0;
    virtual void                AddRemoteDataStorage(CPlayerPed* pPed, CRemoteDataStorage* pData) = 0;
    virtual CRemoteDataStorage* GetRemoteDataStorage(CPlayerPed* pPed) = 0;
    virtual void                RemoveRemoteDataStorage(CPlayerPed* pPed) = 0;

    virtual class CPed* GetContextSwitchedPed() = 0;

    virtual void PreventLeavingVehicles() = 0;
    virtual void HideRadar(bool bHide) = 0;
    virtual void SetCenterOfWorld(class CEntity* entity, class CVector* vecPosition, FLOAT fHeading) = 0;
    virtual void DisablePadHandler(bool bDisabled) = 0;
    virtual void DisableAllVehicleWeapons(bool bDisable) = 0;
    virtual void DisableBirds(bool bDisabled) = 0;
    virtual void DisableQuickReload(bool bDisable) = 0;
    virtual void DisableCloseRangeDamage(bool bDisable) = 0;
    virtual void DisableBadDrivebyHitboxes(bool bDisable) = 0;

    virtual bool  GetExplosionsDisabled() = 0;
    virtual void  DisableExplosions(bool bDisabled) = 0;
    virtual void  SetExplosionHandler(ExplosionHandler* pExplosionHandler) = 0;
    virtual void  SetBreakTowLinkHandler(BreakTowLinkHandler* pBreakTowLinkHandler) = 0;
    virtual void  SetDamageHandler(DamageHandler* pDamageHandler) = 0;
    virtual void  SetDeathHandler(DeathHandler* pDeathHandler) = 0;
    virtual void  SetFireHandler(FireHandler* pFireHandler) = 0;
    virtual void  SetProcessCamHandler(ProcessCamHandler* pProcessCamHandler) = 0;
    virtual void  SetChokingHandler(ChokingHandler* pChokingHandler) = 0;
    virtual void  SetProjectileHandler(ProjectileHandler* pProjectileHandler) = 0;
    virtual void  SetProjectileStopHandler(ProjectileStopHandler* pProjectileHandler) = 0;
    virtual void  SetPreWorldProcessHandler(PreWorldProcessHandler* pHandler) = 0;
    virtual void  SetPostWorldProcessHandler(PostWorldProcessHandler* pHandler) = 0;
    virtual void  SetPostWorldProcessPedsAfterPreRenderHandler(PostWorldProcessPedsAfterPreRenderHandler* pHandler) = 0;
    virtual void  SetIdleHandler(IdleHandler* pHandler) = 0;
    virtual void  SetPreFxRenderHandler(PreFxRenderHandler* pHandler) = 0;
    virtual void  SetPostColorFilterRenderHandler(PostColorFilterRenderHandler* pHandler) = 0;
    virtual void  SetPreHudRenderHandler(PreHudRenderHandler* pHandler) = 0;
    virtual void  DisableCallsToCAnimBlendNode(bool bDisableCalls) = 0;
    virtual void  SetCAnimBlendAssocDestructorHandler(CAnimBlendAssocDestructorHandler* pHandler) = 0;
    virtual void  SetAddAnimationHandler(AddAnimationHandler* pHandler) = 0;
    virtual void  SetAddAnimationAndSyncHandler(AddAnimationAndSyncHandler* pHandler) = 0;
    virtual void  SetAssocGroupCopyAnimationHandler(AssocGroupCopyAnimationHandler* pHandler) = 0;
    virtual void  SetBlendAnimationHierarchyHandler(BlendAnimationHierarchyHandler* pHandler) = 0;
    virtual void  SetProcessCollisionHandler(ProcessCollisionHandler* pHandler) = 0;
    virtual void  SetVehicleCollisionHandler(VehicleCollisionHandler* pHandler) = 0;
    virtual void  SetVehicleDamageHandler(VehicleDamageHandler* pHandler) = 0;
    virtual void  SetHeliKillHandler(HeliKillHandler* pHandler) = 0;
    virtual void  SetObjectDamageHandler(ObjectDamageHandler* pHandler) = 0;
    virtual void  SetObjectBreakHandler(ObjectBreakHandler* pHandler) = 0;
    virtual void  SetWaterCannonHitHandler(WaterCannonHitHandler* pHandler) = 0;
    virtual void  SetVehicleFellThroughMapHandler(VehicleFellThroughMapHandler* pHandler) = 0;
    virtual void  SetGameObjectDestructHandler(GameObjectDestructHandler* pHandler) = 0;
    virtual void  SetGameVehicleDestructHandler(GameVehicleDestructHandler* pHandler) = 0;
    virtual void  SetGamePlayerDestructHandler(GamePlayerDestructHandler* pHandler) = 0;
    virtual void  SetGameProjectileDestructHandler(GameProjectileDestructHandler* pHandler) = 0;
    virtual void  SetGameModelRemoveHandler(GameModelRemoveHandler* pHandler) = 0;
    virtual void  SetGameRunNamedAnimDestructorHandler(GameRunNamedAnimDestructorHandler* pHandler) = 0;
    virtual void  SetGameEntityRenderHandler(GameEntityRenderHandler* pHandler) = 0;
    virtual void  SetFxSystemDestructionHandler(FxSystemDestructionHandler* pHandler) = 0;
    virtual void  SetDrivebyAnimationHandler(DrivebyAnimationHandler* pHandler) = 0;
    virtual void  SetPedStepHandler(PedStepHandler* pHandler) = 0;
    virtual void  SetVehicleWeaponHitHandler(VehicleWeaponHitHandler* pHandler) = 0;
    virtual void  SetAudioZoneRadioSwitchHandler(AudioZoneRadioSwitchHandler* pHandler) = 0;
    virtual void  AllowMouseMovement(bool bAllow) = 0;
    virtual void  DoSoundHacksOnLostFocus(bool bLostFocus) = 0;
    virtual bool  HasSkyColor() = 0;
    virtual void  GetSkyColor(unsigned char& TopRed, unsigned char& TopGreen, unsigned char& TopBlue, unsigned char& BottomRed, unsigned char& BottomGreen,
                              unsigned char& BottomBlue) = 0;
    virtual void  SetSkyColor(unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen,
                              unsigned char BottomBlue) = 0;
    virtual void  ResetSky() = 0;
    virtual void  SetHeatHaze(const SHeatHazeSettings& settings) = 0;
    virtual void  GetHeatHaze(SHeatHazeSettings& settings) = 0;
    virtual void  ResetColorFilter() = 0;
    virtual void  SetColorFilter(DWORD dwPass0Color, DWORD dwPass1Color) = 0;
    virtual void  GetColorFilter(DWORD& dwPass0Color, DWORD& dwPass1Color, bool isOriginal) = 0;
    virtual void  SetGrainMultiplier(eGrainMultiplierType type, float fMultiplier) = 0;
    virtual void  SetGrainLevel(BYTE ucLevel) = 0;
    virtual void  ResetHeatHaze() = 0;
    virtual void  SetHeatHazeEnabled(bool bEnabled) = 0;
    virtual bool  HasWaterColor() = 0;
    virtual void  GetWaterColor(float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha) = 0;
    virtual void  SetWaterColor(float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha) = 0;
    virtual void  ResetWater() = 0;
    virtual void  SetCloudsEnabled(bool bDisabled) = 0;
    virtual bool  GetInteriorSoundsEnabled() = 0;
    virtual void  SetInteriorSoundsEnabled(bool bEnabled) = 0;
    virtual bool  GetInteriorFurnitureEnabled(char cRoomId) = 0;
    virtual void  SetInteriorFurnitureEnabled(char cRoomId, bool bEnabled) = 0;
    virtual void  SetWindVelocity(float fX, float fY, float fZ) = 0;
    virtual void  GetWindVelocity(float& fX, float& fY, float& fZ) = 0;
    virtual void  RestoreWindVelocity() = 0;
    virtual void  SetFarClipDistance(float fDistance) = 0;
    virtual float GetFarClipDistance() = 0;
    virtual void  RestoreFarClipDistance() = 0;
    virtual void  SetNearClipDistance(float fDistance) = 0;
    virtual float GetNearClipDistance() = 0;
    virtual void  RestoreNearClipDistance() = 0;
    virtual void  SetFogDistance(float fDistance) = 0;
    virtual float GetFogDistance() = 0;
    virtual void  RestoreFogDistance() = 0;
    virtual void  GetSunColor(unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed,
                              unsigned char& ucCoronaGreen, unsigned char& ucCoronaBlue) = 0;
    virtual void  SetSunColor(unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed,
                              unsigned char ucCoronaGreen, unsigned char ucCoronaBlue) = 0;
    virtual void  ResetSunColor() = 0;
    virtual float GetSunSize() = 0;
    virtual void  SetSunSize(float fSize) = 0;
    virtual void  ResetSunSize() = 0;
    virtual void  SetMoonSize(int iSize) = 0;
    virtual int   GetMoonSize() = 0;
    virtual void  ResetMoonSize() = 0;

    virtual void  GetAmbientColor(float& red, float& green, float& blue) const = 0;
    virtual bool  SetAmbientColor(float red, float green, float blue) = 0;
    virtual bool  ResetAmbientColor() = 0;

    virtual void  GetAmbientObjectColor(float& red, float& green, float& blue) const = 0;
    virtual bool  SetAmbientObjectColor(float red, float green, float blue) = 0;
    virtual bool  ResetAmbientObjectColor() = 0;

    virtual void  GetDirectionalColor(float& red, float& green, float& blue) const = 0;
    virtual bool  SetDirectionalColor(float red, float green, float blue) = 0;
    virtual bool  ResetDirectionalColor() = 0;

    virtual float GetSpriteSize() const = 0;
    virtual bool  SetSpriteSize(float size) = 0;
    virtual bool  ResetSpriteSize() = 0;

    virtual float GetSpriteBrightness() const = 0;
    virtual bool  SetSpriteBrightness(float brightness) = 0;
    virtual bool  ResetSpriteBrightness() = 0;

    virtual int16 GetPoleShadowStrength() const = 0;
    virtual bool  SetPoleShadowStrength(int16 strength) = 0;
    virtual bool  ResetPoleShadowStrength() = 0;

    virtual int16 GetShadowStrength() const = 0;
    virtual bool  SetShadowStrength(int16 strength) = 0;
    virtual bool  ResetShadowStrength() = 0;

    virtual float GetShadowsOffset() const = 0;
    virtual bool  SetShadowsOffset(float offset) = 0;
    virtual bool  ResetShadowsOffset() = 0;

    virtual float GetLightsOnGroundBrightness() const = 0;
    virtual bool  SetLightsOnGroundBrightness(float brightness) = 0;
    virtual bool  ResetLightsOnGroundBrightness() = 0;

    virtual void  GetLowCloudsColor(int16& red, int16& green, int16& blue) const = 0;
    virtual bool  SetLowCloudsColor(int16 red, int16 green, int16 blue) = 0;
    virtual bool  ResetLowCloudsColor() = 0;

    virtual void  GetBottomCloudsColor(int16& red, int16& green, int16& blue) const = 0;
    virtual bool  SetBottomCloudsColor(int16 red, int16 green, int16 blue) = 0;
    virtual bool  ResetBottomCloudsColor() = 0;

    virtual float GetCloudsAlpha1() const = 0;
    virtual bool  SetCloudsAlpha1(float alpha) = 0;
    virtual bool  ResetCloudsAlpha1() = 0;

    virtual float GetIllumination() const = 0;
    virtual bool  SetIllumination(float illumination) = 0;
    virtual bool  ResetIllumination() = 0;

    virtual void DisableEnterExitVehicleKey(bool bDisabled) = 0;

    virtual void SetNightVisionEnabled(bool bEnabled, bool bNoiseEnabled) = 0;
    virtual void SetThermalVisionEnabled(bool bEnabled, bool bNoiseEnabled) = 0;
    virtual bool IsNightVisionEnabled() = 0;
    virtual bool IsThermalVisionEnabled() = 0;

    virtual void RebuildMultiplayerPlayer(class CPed* player) = 0;

    virtual void AllowWindowsCursorShowing(bool bAllow) = 0;

    virtual class CShotSyncData* GetLocalShotSyncData() = 0;

    virtual void SetPreContextSwitchHandler(PreContextSwitchHandler* pHandler) = 0;
    virtual void SetPostContextSwitchHandler(PostContextSwitchHandler* pHandler) = 0;
    virtual void SetPreWeaponFireHandler(PreWeaponFireHandler* pHandler) = 0;
    virtual void SetPostWeaponFireHandler(PostWeaponFireHandler* pHandler) = 0;
    virtual void SetBulletImpactHandler(BulletImpactHandler* pHandler) = 0;
    virtual void SetBulletFireHandler(BulletFireHandler* pHandler) = 0;
    virtual void SetDrawRadarAreasHandler(DrawRadarAreasHandler* pRadarAreasHandler) = 0;
    virtual void SetRender3DStuffHandler(Render3DStuffHandler* pHandler) = 0;
    virtual void SetPreRenderSkyHandler(PreRenderSkyHandler* pHandler) = 0;
    virtual void SetRenderHeliLightHandler(RenderHeliLightHandler* pHandler) = 0;
    virtual void SetRenderEverythingBarRoadsHandler(RenderEverythingBarRoadsHandler* pHandler) = 0;

    virtual void Reset() = 0;

    virtual void ConvertEulerAnglesToMatrix(CMatrix& Matrix, float fX, float fY, float fZ) = 0;
    virtual void ConvertMatrixToEulerAngles(const CMatrix& Matrix, float& fX, float& fY, float& fZ) = 0;

    virtual void ConvertEulerAnglesToMatrix(CMatrix& Matrix, CVector& vecAngles) = 0;
    virtual void ConvertMatrixToEulerAngles(const CMatrix& Matrix, CVector& vecAngles) = 0;

    virtual float GetGlobalGravity() = 0;
    virtual void  SetGlobalGravity(float fGravity) = 0;

    virtual float GetLocalPlayerGravity() = 0;
    virtual void  SetLocalPlayerGravity(float fGravity) = 0;

    virtual unsigned char GetTrafficLightState() = 0;
    virtual void          SetTrafficLightState(unsigned char ucState) = 0;

    virtual bool GetTrafficLightsLocked() = 0;
    virtual void SetTrafficLightsLocked(bool bLocked) = 0;

    virtual void  SetLocalStatValue(unsigned short usStat, float fValue) = 0;
    virtual float GetLocalStatValue(unsigned short usStat) = 0;
    virtual void  SetLocalStatsStatic(bool bStatic) = 0;

    virtual void SetLocalCameraRotation(float fRotation) = 0;
    virtual bool IsCustomCameraRotationEnabled() = 0;
    virtual void SetCustomCameraRotationEnabled(bool bEnabled) = 0;

    virtual void SetDebugVars(float f1, float f2, float f3) = 0;

    virtual CVector& GetAkimboTarget() = 0;
    virtual bool     GetAkimboTargetUp() = 0;

    virtual void SetAkimboTarget(const CVector& vecTarget) = 0;
    virtual void SetAkimboTargetUp(bool bUp) = 0;

    virtual void AllowCreatedObjectsInVerticalLineTest(bool bOn) = 0;
    virtual void DeleteAndDisableGangTags() = 0;

    virtual CLimits* GetLimits() = 0;

    virtual void UpdateVehicleSuspension() const noexcept = 0;

    virtual void FlushClothesCache() = 0;
    virtual void SetFastClothesLoading(EFastClothesLoading fastClothesLoading) = 0;
    virtual void SetLODSystemEnabled(bool bEnable) = 0;
    virtual void SetAltWaterOrderEnabled(bool bEnable) = 0;

    virtual float GetAircraftMaxHeight() = 0;
    virtual void  SetAircraftMaxHeight(float fHeight) = 0;

    virtual float GetAircraftMaxVelocity() = 0;
    virtual void  SetAircraftMaxVelocity(float fVelocity) = 0;

    virtual void SetAutomaticVehicleStartupOnPedEnter(bool bSet) = 0;

    virtual void SetPedTargetingMarkerEnabled(bool bEnabled) = 0;
    virtual bool IsPedTargetingMarkerEnabled() = 0;

    virtual void GetRwResourceStats(SRwResourceStats& outStats) = 0;
    virtual void GetClothesCacheStats(SClothesCacheStats& outStats) = 0;
    virtual void SetIsMinimizedAndNotConnected(bool bIsMinimizedAndNotConnected) = 0;
    virtual void SetMirrorsEnabled(bool bEnabled) = 0;

    virtual void SetBoatWaterSplashEnabled(bool bEnabled) = 0;
    virtual void SetTyreSmokeEnabled(bool bEnabled) = 0;

    virtual eAnimGroup GetLastStaticAnimationGroupID() = 0;
    virtual eAnimID    GetLastStaticAnimationID() = 0;
    virtual DWORD      GetLastAnimArrayAddress() = 0;

    virtual unsigned int EntryInfoNodePool_NoOfUsedSpaces() const noexcept = 0;
    virtual unsigned int PtrNodeDoubleLinkPool_NoOfUsedSpaces() const noexcept = 0;
};
