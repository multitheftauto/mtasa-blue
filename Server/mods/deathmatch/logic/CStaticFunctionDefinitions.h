/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CStaticFunctionDefinitions.h
 *  PURPOSE:     Lua static function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CStaticFunctionDefinitions;

#pragma once

#include "CVehicle.h"
#include "CRegistry.h"
#include "lua/CLuaFunctionParseHelpers.h"
#include <optional>

class CVector2D;
struct SLineOfSightFlags;

class CStaticFunctionDefinitions
{
public:
    CStaticFunctionDefinitions(CGame* pGame);
    ~CStaticFunctionDefinitions();

    // Event funcs
    static bool AddEvent(CLuaMain* pLuaMain, const char* szName, const char* szArguments, bool bAllowRemoteTrigger);
    static bool AddEventHandler(CLuaMain* pLuaMain, const char* szName, CElement* pElement, const CLuaFunctionRef& iLuaFunction, bool bPropagated,
                                EEventPriorityType eventPriority, float fPriorityMod);
    static bool RemoveEventHandler(CLuaMain* pLuaMain, const char* szName, CElement* pElement, const CLuaFunctionRef& iLuaFunction);
    static bool TriggerEvent(const char* szName, CElement* pElement, const CLuaArguments& Arguments, bool& bWasCancelled);
    static bool TriggerClientEvent(const std::vector<CPlayer*>& sendList, const char* szName, CElement* pCallWithElement, CLuaArguments& Arguments);
    static bool TriggerLatentClientEvent(const std::vector<CPlayer*>& sendList, const char* szName, CElement* pCallWithElement, CLuaArguments& Arguments,
                                         int iBandwidth, CLuaMain* pLuaMain, ushort usResourceNetId);

    static bool        CancelEvent(bool bCancel, const char* szReason);
    static const char* GetCancelReason();
    static bool        WasEventCancelled();

    // Element create/destroy
    static CDummy*   CreateElement(CResource* pResource, const char* szTypeName, const char* szID);
    static bool      DestroyElement(CElement* pElement);
    static CElement* CloneElement(CResource* pResource, CElement* pElement, const CVector& vecPosition, bool bCloneElement);

    // Element get funcs
    static CElement*      GetElementByID(const char* szID, unsigned int uiIndex);
    static CElement*      GetElementByIndex(const char* szType, unsigned int uiIndex);
    static CElement*      GetElementChild(CElement* pElement, unsigned int uiIndex);
    static bool           GetElementChildrenCount(CElement* pElement, unsigned int& uiCount);
    static CLuaArgument*  GetElementData(CElement* pElement, const char* szName, bool bInherit);
    static CLuaArguments* GetAllElementData(CElement* pElement, CLuaArguments* table);
    static CElement*      GetElementParent(CElement* pElement);
    static bool           GetElementMatrix(CElement* pElement, CMatrix& matrix);
    static bool           GetElementPosition(CElement* pElement, CVector& vecPosition);
    static bool           GetElementRotation(CElement* pElement, CVector& vecRotation, eEulerRotationOrder rotationOrder);
    static bool           GetElementVelocity(CElement* pElement, CVector& vecVelocity);
    static bool           GetElementTurnVelocity(CElement* pElement, CVector& vecTurnVelocity);
    static bool           GetElementInterior(CElement* pElement, unsigned char& ucInterior);
    static bool           IsElementWithinColShape(CElement* pElement, CColShape* pColShape, bool& bWithin);
    static bool           IsElementWithinMarker(CElement* pElement, CMarker* pMarker, bool& bWithin);
    static bool           GetElementDimension(CElement* pElement, unsigned short& usDimension);
    static bool           GetElementZoneName(CElement* pElement, SString& strOutName, bool bCitiesOnly = false);
    static bool           IsElementAttached(CElement* pElement);
    static CElement*      GetElementAttachedTo(CElement* pElement);
    static CColShape*     GetElementColShape(CElement* pElement);
    static bool           GetElementAlpha(CElement* pElement, unsigned char& ucAlpha);
    static bool           IsElementDoubleSided(CElement* pElement, bool& bDoubleSided);
    static bool           GetElementHealth(CElement* pElement, float& fHealth);
    static bool           GetElementModel(CElement* pElement, unsigned short& usModel);
    static bool           IsElementInWater(CElement* pElement, bool& bInWater);
    static bool           GetElementAttachedOffsets(CElement* pElement, CVector& vecPosition, CVector& vecRotation);
    static CElement*      GetElementSyncer(CElement* pElement);
    static bool           GetElementCollisionsEnabled(CElement* pElement);
    static bool           IsElementFrozen(CElement* pElement, bool& bFrozen);
    static bool           GetLowLodElement(CElement* pElement, CElement*& pOutLowLodElement);
    static bool           IsElementLowLod(CElement* pElement, bool& bOutLowLod);
    static bool           IsElementCallPropagationEnabled(CElement* pElement, bool& bOutEnabled);

    // Element set funcs
    static bool ClearElementVisibleTo(CElement* pElement);
    static bool SetElementID(CElement* pElement, const char* szID);
    static bool SetElementData(CElement* pElement, const char* szName, const CLuaArgument& Variable, ESyncType syncType);
    static bool RemoveElementData(CElement* pElement, const char* szName);
    static bool AddElementDataSubscriber(CElement* pElement, const char* szName, CPlayer* pPlayer);
    static bool RemoveElementDataSubscriber(CElement* pElement, const char* szName, CPlayer* pPlayer);
    static bool HasElementDataSubscriber(CElement* pElement, const char* szName, CPlayer* pPlayer);
    static bool SetElementParent(CElement* pElement, CElement* pParent);
    static bool SetElementMatrix(CElement* pElement, const CMatrix& matrix);
    static bool SetElementPosition(CElement* pElement, const CVector& vecPosition, bool bWarp = true);
    static bool SetElementRotation(CElement* pElement, const CVector& vecRotation, eEulerRotationOrder rotationOrder, bool bNewWay);
    static bool SetElementVelocity(CElement* pElement, const CVector& vecVelocity);
    static bool SetElementAngularVelocity(CElement* pElement, const CVector& vecTurnVelocity);
    static bool SetElementVisibleTo(CElement* pElement, CElement* pReference, bool bVisible);
    static bool SetElementInterior(CElement* pElement, unsigned char ucInterior, bool bSetPosition, CVector& vecPosition);
    static bool SetElementDimension(CElement* pElement, unsigned short usDimension);
    static bool AttachElements(CElement* pElement, CElement* pAttachedToElement, CVector& vecPosition, CVector& vecRotation);
    static bool DetachElements(CElement* pElement, CElement* pAttachedToElement = NULL);
    static bool SetElementAlpha(CElement* pElement, unsigned char ucAlpha);
    static bool SetElementDoubleSided(CElement* pElement, bool bDoubleSided);
    static bool SetElementHealth(CElement* pElement, float fHealth);
    static bool SetElementModel(CElement* pElement, unsigned short usModel);
    static bool SetElementAttachedOffsets(CElement* pElement, CVector& vecPosition, CVector& vecRotation);
    static bool SetElementSyncer(CElement* pElement, CPlayer* pPlayer, bool bEnable = true, bool bPersist = false);
    static bool SetElementCollisionsEnabled(CElement* pElement, bool bEnable);
    static bool SetElementFrozen(CElement* pElement, bool bFrozen);
    static bool SetLowLodElement(CElement* pElement, CElement* pLowLodElement);
    static bool SetElementCallPropagationEnabled(CElement* pElement, bool bEnable);

    // Player get funcs
    static unsigned int       GetPlayerCount();
    static CPlayer*           GetPlayerFromName(const char* szNick);
    static bool               GetPlayerPing(CPlayer* pPlayer, unsigned int& uiPing);
    static bool               GetPlayerMoney(CPlayer* pPlayer, long& lMoney);
    static bool               GetPlayerSourceIP(CPlayer* pPlayer, SString& strOutIP);
    static CPlayer*           GetRandomPlayer();
    static bool               IsPlayerMuted(CPlayer* pPlayer, bool& bMuted);
    static CTeam*             GetPlayerTeam(CPlayer* pPlayer);
    static bool               CanPlayerUseFunction(CPlayer* pPlayer, const char* szFunction, bool& bCanUse);
    static bool               IsPlayerDebuggerVisible(CPlayer* pPlayer, bool& bVisible);
    static bool               GetPlayerWantedLevel(CPlayer* pPlayer, unsigned int& uiWantedLevel);
    static bool               IsPlayerScoreboardForced(CPlayer* pPlayer, bool& bForced);
    static bool               IsPlayerMapForced(CPlayer* pPlayer, bool& bForced);
    static bool               GetPlayerNametagText(CPlayer* pPlayer, SString& strOutText);
    static bool               GetPlayerNametagColor(CPlayer* pPlayer, unsigned char& ucR, unsigned char& ucG, unsigned char& ucB);
    static bool               IsPlayerNametagShowing(CPlayer* pPlayer, bool& bShowing);
    static const std::string& GetPlayerSerial(CPlayer* pPlayer, uint uiIndex);
    static const std::string& GetPlayerUserName(CPlayer* pPlayer);
    static const std::string& GetPlayerCommunityID(CPlayer* pPlayer);
    static bool               GetPlayerBlurLevel(CPlayer* pPlayer, unsigned char& ucLevel);
    static bool               GetPlayerName(CElement* pElement, SString& strOutName);
    static bool               GetPlayerIP(CElement* pElement, SString& strOutIP);
    static CAccount*          GetPlayerAccount(CElement* pElement);
    static const CMtaVersion& GetPlayerVersion(CPlayer* pPlayer);

    // Player set functions
    static bool SetPlayerMoney(CElement* pElement, long lMoney, bool bInstant);
    static bool SetPlayerAmmo(CElement* pElement, unsigned char ucSlot, unsigned short usAmmo, unsigned short usAmmoInClip);
    static bool GivePlayerMoney(CElement* pElement, long lMoney);
    static bool TakePlayerMoney(CElement* pElement, long lMoney);
    static bool ShowPlayerHudComponent(CElement* pElement, eHudComponent component, bool bShow);
    static bool SetPlayerDebuggerVisible(CElement* pElement, bool bVisible);
    static bool SetPlayerScriptDebugLevel(CElement* pElement, unsigned int uiLevel);
    static bool SetPlayerWantedLevel(CElement* pElement, unsigned int iLevel);
    static bool ForcePlayerMap(CElement* pElement, bool bVisible);
    static bool SetPlayerNametagText(CElement* pElement, const char* szText);
    static bool SetPlayerNametagColor(CElement* pElement, bool bRemoveOverride, unsigned char ucR, unsigned char ucG, unsigned char ucB);
    static bool SetPlayerNametagShowing(CElement* pElement, bool bShowing);
    static bool SpawnPlayer(CPlayer* pPlayer, const CVector& vecPosition, float fRotation, unsigned long ulModel, unsigned char ucInterior,
                            unsigned short usDimension, CTeam* pTeam = NULL);
    static bool SetPlayerMuted(CElement* pElement, bool bMuted);
    static bool SetPlayerBlurLevel(CElement* pElement, unsigned char ucLevel);
    static bool RedirectPlayer(CElement* pElement, const char* szHost, unsigned short usPort, const char* szPassword);
    static bool SetPlayerName(CElement* pElement, const char* szName);
    static bool DetonateSatchels(CElement* pElement);
    static bool TakePlayerScreenShot(CElement* pElement, uint uiSizeX, uint uiSizeY, const SString& strTag, uint uiQuality, uint uiMaxBandwidth,
                                     uint uiMaxPacketSize, CResource* pResource);

    // Ped get funcs
    static CPed*     CreatePed(CResource* pResource, unsigned short usModel, const CVector& vecPosition, float fRotation = 0.0f, bool bSynced = true);
    static bool      GetPedArmor(CPed* pPed, float& fArmor);
    static bool      GetPedRotation(CPed* pPed, float& fRotation);
    static bool      IsPedDead(CPed* pPed, bool& bDead);
    static bool      IsPedDucked(CPed* pPed, bool& bDucked);
    static bool      GetPedStat(CPed* pPed, unsigned short usStat, float& fValue);
    static CElement* GetPedTarget(CPed* pPed);
    static bool      GetPedClothes(CPed* pPed, unsigned char ucType, SString& strOutTexture, SString& strOutModel);
    static bool      DoesPedHaveJetPack(CPed* pPed, bool& bHasJetPack);
    static bool      IsPedOnGround(CPed* pPed, bool& bOnGround);
    static bool      GetPedFightingStyle(CPed* pPed, unsigned char& ucStyle);
    static bool      GetPedMoveAnim(CPed* pPed, unsigned int& iMoveAnim);
    static bool      GetPedGravity(CPed* pPed, float& fGravity);
    static CElement* GetPedContactElement(CPed* pPed);
    static bool      GetPedWeaponSlot(CPed* pPed, unsigned char& ucWeaponSlot);
    static bool      IsPedDoingGangDriveby(CPed* pPed, bool& bDoingGangDriveby);
    static bool      IsPedOnFire(CPed* pPed, bool& bIsOnFire);
    static bool      IsPedHeadless(CPed* pPed, bool& bIsHeadless);
    static bool      IsPedFrozen(CPed* pPed, bool& bIsFrozen);
    static CVehicle* GetPedOccupiedVehicle(CPed* pPed);
    static bool      GetPedOccupiedVehicleSeat(CPed* pPed, unsigned int& uiSeat);
    static bool      GetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float& fData);
    static bool      GetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, int& sData);
    static bool      GetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, CVector& vecData);
    static bool      GetWeaponPropertyFlag(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, bool& bEnable);
    static bool      GetOriginalWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float& fData);
    static bool      GetOriginalWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, int& sData);
    static bool      GetOriginalWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, CVector& vecData);
    static bool      GetOriginalWeaponPropertyFlag(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, bool& bEnable);

    // Ped set funcs
    static bool SetPedArmor(CElement* pElement, float fArmor);
    static bool KillPed(CElement* pElement, CElement* pKiller = NULL, unsigned char ucKillerWeapon = 0xFF, unsigned char ucBodyPart = 0xFF,
                        bool bStealth = false);
    static bool SetPedRotation(CElement* pElement, float fRotation, bool bNewWay);
    static bool SetPedStat(CElement* pElement, unsigned short usStat, float fValue);
    static bool AddPedClothes(CElement* pElement, const char* szTexture, const char* szModel, unsigned char ucType);
    static bool RemovePedClothes(CElement* pElement, unsigned char ucType, const char* szTexture = NULL, const char* szModel = NULL);
    static bool GivePedJetPack(CElement* pElement);
    static bool RemovePedJetPack(CElement* pElement);
    static bool SetPedWearingJetpack(CElement* pElement, bool bJetPack);
    static bool SetPedFightingStyle(CElement* pElement, unsigned char ucStyle);
    static bool SetPedMoveAnim(CElement* pElement, unsigned int iMoveAnim);
    static bool SetPedGravity(CElement* pElement, float fGravity);
    static bool SetPedChoking(CElement* pElement, bool bChoking);
    static bool SetPedWeaponSlot(CElement* pElement, unsigned char ucWeaponSlot);
    static bool WarpPedIntoVehicle(CPed* pPed, CVehicle* pVehicle, unsigned int uiSeat = 0);
    static bool RemovePedFromVehicle(CElement* pElement);
    static bool SetPedDoingGangDriveby(CElement* pElement, bool bGangDriveby);
    static bool SetPedAnimation(CElement* pElement, const SString& blockName, const SString& animName, int iTime, int iBlend, bool bLoop, bool bUpdatePosition,
                                bool bInterruptable, bool bFreezeLastFrame, bool bTaskToBeRestoredOnAnimEnd);
    static bool SetPedAnimationProgress(CElement* pElement, const SString& animName, float fProgress);
    static bool SetPedAnimationSpeed(CElement* pElement, const SString& animName, float fSpeed);
    static bool SetPedOnFire(CElement* pElement, bool bIsOnFire);
    static bool SetPedHeadless(CElement* pElement, bool bIsHeadless);
    static bool SetPedFrozen(CElement* pElement, bool bIsFrozen);
    static bool reloadPedWeapon(CElement* pElement);
    static bool SetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float fData);
    static bool SetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, int sData);
    static bool SetWeaponPropertyFlag(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, bool bEnable);

    // Camera get functions
    static bool      GetCameraMatrix(CPlayer* pPlayer, CVector& vecPosition, CVector& vecLookAt, float& fRoll, float& fFOV);
    static CElement* GetCameraTarget(CPlayer* pPlayer);
    static bool      GetCameraInterior(CPlayer* pPlayer, unsigned char& ucInterior);

    // Camera set functions
    static bool SetCameraMatrix(CElement* pElement, const CVector& vecPosition, CVector* pvecLookAt, float fRoll, float fFOV);
    static bool SetCameraTarget(CElement* pElement, CElement* pTarget);
    static bool SetCameraInterior(CElement* pElement, unsigned char ucInterior);
    static bool FadeCamera(CElement* pElement, bool bFadeIn, float fFadeTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);

    // Weapon give/take functions
    static bool GiveWeapon(CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo, bool bSetAsCurrent = false);
    static bool TakeWeapon(CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo = 9999);
    static bool TakeAllWeapons(CElement* pElement);
    static bool SetWeaponAmmo(CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo, unsigned short usAmmoInClip);

    // Vehicle create/destroy functions
    static CVehicle* CreateVehicle(CResource* pResource, unsigned short usModel, const CVector& vecPosition, const CVector& vecRotation, const char* szRegPlate,
                                   unsigned char ucVariant, unsigned char ucVariant2, bool bSynced);

    // Vehicle get functions
    static bool  GetVehicleVariant(CVehicle* pVehicle, unsigned char& ucVariant, unsigned char& ucVariant2);
    static bool  GetVehicleColor(CVehicle* pVehicle, CVehicleColor& color);
    static bool  GetVehicleModelFromName(const char* szName, unsigned short& usID);
    static bool  GetVehicleMaxPassengers(CVehicle* pVehicle, unsigned char& ucMaxPassengers);
    static bool  GetVehicleName(CVehicle* pVehicle, SString& strOutName);
    static bool  GetVehicleNameFromModel(unsigned short usModel, SString& strOutName);
    static CPed* GetVehicleOccupant(CVehicle* pVehicle, unsigned int uiSeat);
    static CPed* GetVehicleController(CVehicle* pVehicle);
    static bool  GetVehicleRotation(CVehicle* pVehicle, CVector& vecRotation);
    static bool  GetVehicleTurnVelocity(CVehicle* pVehicle, CVector& vecTurnVelocity);
    static bool  GetVehicleTurretPosition(CVehicle* pVehicle, CVector2D& vecPosition);
    static bool  IsVehicleLocked(CVehicle* pVehicle, bool& bLocked);
    static bool  GetVehicleUpgradeOnSlot(CVehicle* pVehicle, unsigned char ucSlot, unsigned short& usUpgrade);
    static bool  GetVehicleUpgradeSlotName(unsigned char ucSlot, SString& strOutName);
    static bool  GetVehicleUpgradeSlotName(unsigned short usUpgrade, SString& strOutName);
    static bool  GetVehicleDoorState(CVehicle* pVehicle, unsigned char ucDoor, unsigned char& ucState);
    static bool  GetVehicleWheelStates(CVehicle* pVehicle, unsigned char& ucFrontLeft, unsigned char& ucRearLeft, unsigned char& ucFrontRight,
                                       unsigned char& ucRearRight);
    static bool  GetVehicleLightState(CVehicle* pVehicle, unsigned char ucLight, unsigned char& ucState);
    static bool  GetVehiclePanelState(CVehicle* pVehicle, unsigned char ucPanel, unsigned char& ucState);
    static bool  GetVehicleOverrideLights(CVehicle* pVehicle, unsigned char& ucLights);
    static bool  GetVehiclePaintjob(CVehicle* pVehicle, unsigned char& ucPaintjob);
    static bool  GetVehiclePlateText(CVehicle* pVehicle, char* szPlateText);
    static bool  IsVehicleDamageProof(CVehicle* pVehicle, bool& bDamageProof);
    static bool  IsVehicleFuelTankExplodable(CVehicle* pVehicle, bool& bExplodable);
    static bool  IsVehicleFrozen(CVehicle* pVehicle, bool& bFrozen);
    static bool  IsVehicleOnGround(CVehicle* pVehicle, bool& bOnGround);
    static bool  GetVehicleEngineState(CVehicle* pVehicle, bool& bState);
    static bool  IsTrainDerailed(CVehicle* pVehicle, bool& bDerailed);
    static bool  IsTrainDerailable(CVehicle* pVehicle, bool& bDerailable);
    static bool  GetTrainDirection(CVehicle* pVehicle, bool& bDirection);
    static bool  GetTrainSpeed(CVehicle* pVehicle, float& fSpeed);
    static bool  GetTrainPosition(CVehicle* pVehicle, float& fPosition);
    static bool  GetVehicleHeadLightColor(CVehicle* pVehicle, SColor& outColor);
    static bool  GetVehicleDoorOpenRatio(CVehicle* pVehicle, unsigned char ucDoor, float& fRatio);

    static bool GetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, float& fValue);
    static bool GetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, CVector& vecValue);
    static bool GetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, std::string& strValue);
    static bool GetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, unsigned int& uiValue);
    static bool GetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, unsigned char& ucValue);
    static bool GetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, float& fValue, bool origin = false);
    static bool GetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, CVector& vecValue, bool origin = false);
    static bool GetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, std::string& strValue, bool origin = false);
    static bool GetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, unsigned int& uiValue, bool origin = false);
    static bool GetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, unsigned char& ucValue, bool origin = false);
    static bool GetEntryHandling(const CHandlingEntry* pEntry, eHandlingProperty eProperty, float& fValue);
    static bool GetEntryHandling(const CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string& strValue);
    static bool GetEntryHandling(const CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int& uiValue);
    static bool GetEntryHandling(const CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char& ucValue);

    // Vehicle set functions
    static bool FixVehicle(CElement* pElement);
    static bool BlowVehicle(CElement* pElement, std::optional<bool> withExplosion);
    static bool SetVehicleColor(CElement* pElement, const CVehicleColor& color);
    static bool SetVehicleLandingGearDown(CElement* pElement, bool bLandingGearDown);
    static bool SetVehicleLocked(CElement* pElement, bool bLocked);
    static bool SetVehicleDoorsUndamageable(CElement* pElement, bool bDoorsUndamageable);
    static bool SetVehicleRotation(CElement* pElement, const CVector& vecRotation);
    static bool SetVehicleSirensOn(CElement* pElement, bool bSirensOn);
    static bool SetVehicleTaxiLightOn(CElement* pElement, bool bTaxiLightState);
    static bool SetVehicleTurnVelocity(CElement* pElement, const CVector& vecTurnVelocity);
    static bool AddVehicleUpgrade(CElement* pElement, unsigned short usUpgrade);
    static bool AddAllVehicleUpgrades(CElement* pElement);
    static bool RemoveVehicleUpgrade(CElement* pElement, unsigned short usUpgrade);
    static bool SetVehicleDoorState(CElement* pElement, unsigned char ucDoor, unsigned char ucState, bool spawnFlyingComponent);
    static bool SetVehicleWheelStates(CElement* pElement, int iFrontLeft, int iRearLeft = -1, int iFrontRight = -1, int iRearRight = -1);
    static bool SetVehicleLightState(CElement* pElement, unsigned char ucLight, unsigned char ucState);
    static bool SetVehiclePanelState(CElement* pElement, unsigned char ucPanel, unsigned char ucState);
    static bool SetVehicleIdleRespawnDelay(CElement* pElement, unsigned long ulTime);
    static bool SetVehicleRespawnDelay(CElement* pElement, unsigned long ulTime);
    static bool GetVehicleRespawnPosition(CElement* pElement, CVector& vecPosition);
    static bool GetVehicleRespawnRotation(CElement* pElement, CVector& vecRotation);
    static bool SetVehicleRespawnRotation(CElement* pElement, const CVector& vecRotation);
    static bool SetVehicleRespawnPosition(CElement* pElement, const CVector& vecPosition);
    static bool ToggleVehicleRespawn(CElement* pElement, bool bRespawn);
    static bool ResetVehicleExplosionTime(CElement* pElement);
    static bool ResetVehicleIdleTime(CElement* pElement);
    static bool SpawnVehicle(CElement* pElement, const CVector& vecPosition, const CVector& vecRotation);
    static bool RespawnVehicle(CElement* pElement);
    static bool SetVehicleOverrideLights(CElement* pElement, unsigned char ucLights);
    static bool AttachTrailerToVehicle(CVehicle* pVehicle, CVehicle* pTrailer);
    static bool DetachTrailerFromVehicle(CVehicle* pVehicle, CVehicle* pTrailer = NULL);
    static bool SetVehicleEngineState(CElement* pElement, bool bState);
    static bool SetVehicleDirtLevel(CElement* pElement, float fDirtLevel);
    static bool SetVehicleDamageProof(CElement* pElement, bool bDamageProof);
    static bool SetVehiclePaintjob(CElement* pElement, unsigned char ucPaintjob);
    static bool SetVehicleFuelTankExplodable(CElement* pElement, bool bExplodable);
    static bool SetVehicleFrozen(CVehicle* pVehicle, bool bFrozen);
    static bool SetTrainDerailed(CVehicle* pVehicle, bool bDerailed);
    static bool SetTrainDerailable(CVehicle* pVehicle, bool bDerailable);
    static bool SetTrainDirection(CVehicle* pVehicle, bool bDirection);
    static bool SetTrainSpeed(CVehicle* pVehicle, float fSpeed);
    static bool SetTrainPosition(CVehicle* pVehicle, float fPosition);
    static bool SetVehicleHeadLightColor(CVehicle* pVehicle, const SColor color);
    static bool SetVehicleTurretPosition(CVehicle* pVehicle, float fHorizontal, float fVertical);
    static bool SetVehicleDoorOpenRatio(CElement* pElement, unsigned char ucDoor, float fRatio, unsigned long ulTime = 0);
    static bool SetVehiclePlateText(CElement* pElement, const SString& strPlateText);

    // static bool SetVehicleHandling(CVehicle* pVehicle, bool bValue);
    static bool ResetVehicleHandling(CVehicle* pVehicle, bool bUseOriginal);
    static bool ResetVehicleHandlingProperty(CVehicle* pVehicle, eHandlingProperty eProperty, bool bUseOriginal);
    static bool SetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, float fValue);
    static bool SetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, CVector vecValue);
    static bool SetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, std::string strValue);
    static bool SetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, unsigned int uiValue);
    static bool SetVehicleHandling(CVehicle* pVehicle, eHandlingProperty eProperty, unsigned char ucValue);
    static bool ResetModelHandling(eVehicleTypes eModel);
    static bool ResetModelHandlingProperty(eVehicleTypes eModel, eHandlingProperty eProperty);
    static bool SetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, float fValue);
    static bool SetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, CVector vecValue);
    static bool SetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, std::string strValue);
    static bool SetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, unsigned int uiValue);
    static bool SetModelHandling(eVehicleTypes eModel, eHandlingProperty eProperty, unsigned char ucValue);
    static bool SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, float fValue);
    static bool SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, CVector vecValue);
    static bool SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string strValue);
    static bool SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int uiValue);
    static bool SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char ucValue);
    static bool SetVehicleVariant(CVehicle* pVehicle, unsigned char ucVariant, unsigned char ucVariant2);
    static bool SetVehicleSirens(CVehicle* pVehicle, unsigned char ucSirenID, SSirenInfo tSirenInfo);
    static bool GiveVehicleSirens(CVehicle* pVehicle, unsigned char ucSirenType, unsigned char ucSirenCount, SSirenInfo tSirenInfo);
    static bool RemoveVehicleSirens(CVehicle* pVehicle);

    // Marker create/destroy functions
    static CMarker* CreateMarker(CResource* pResource, const CVector& vecPosition, const char* szType, float fSize, const SColor color, CElement* pVisibleTo, bool ignoreAlphaLimits);

    // Marker get functions
    static bool GetMarkerCount(unsigned int& uiCount);
    static bool GetMarkerType(CMarker* pMarker, char* szType);
    static bool GetMarkerSize(CMarker* pMarker, float& fSize);
    static bool GetMarkerColor(CMarker* pMarker, SColor& outColor);
    static bool GetMarkerTarget(CMarker* pMarker, CVector& vecTarget);
    static bool GetMarkerIcon(CMarker* pMarker, char* szIcon);

    // Marker set functions
    static bool SetMarkerType(CElement* pElement, const char* szType);
    static bool SetMarkerSize(CElement* pElement, float fSize);
    static bool SetMarkerColor(CElement* pElement, const SColor color);
    static bool SetMarkerTarget(CElement* pElement, const CVector* pTarget);
    static bool SetMarkerIcon(CElement* pElement, const char* szIcon);

    static bool SetMarkerTargetArrowProperties(CElement* Element, const SColor color, float size);

    // Blip create/destroy functions
    static CBlip* CreateBlip(CResource* pResource, const CVector& vecPosition, unsigned char ucIcon, unsigned char ucSize, const SColor color, short sOrdering,
                             unsigned short usVisibleDistance, CElement* pVisibleTo);
    static CBlip* CreateBlipAttachedTo(CResource* pResource, CElement* pElement, unsigned char ucIcon, unsigned char ucSize, const SColor color,
                                       short sOrdering, unsigned short usVisibleDistance, CElement* pVisibleTo);

    // Blip get functions
    static bool GetBlipIcon(CBlip* pBlip, unsigned char& ucIcon);
    static bool GetBlipSize(CBlip* pBlip, unsigned char& ucSize);
    static bool GetBlipColor(CBlip* pBlip, SColor& outColor);
    static bool GetBlipOrdering(CBlip* pBlip, short& sOrdering);
    static bool GetBlipVisibleDistance(CBlip* pBlip, unsigned short& usVisibleDistance);

    // Blip set functions
    static bool SetBlipIcon(CElement* pElement, unsigned char ucIcon);
    static bool SetBlipSize(CElement* pElement, unsigned char ucSize);
    static bool SetBlipColor(CElement* pElement, const SColor color);
    static bool SetBlipOrdering(CElement* pElement, short sOrdering);
    static bool SetBlipVisibleDistance(CElement* pElement, unsigned short usVisibleDistance);

    // Object create/destroy functions
    static CObject* CreateObject(CResource* pResource, unsigned short usModelID, const CVector& vecPosition, const CVector& vecRotation, bool bIsLowLod);

    // Object get functions
    static bool GetObjectRotation(CObject* pObject, CVector& vecRotation);
    static bool IsObjectVisibleInAllDimensions(CElement* pElement);
    static bool IsObjectBreakable(CElement* pElement);

    // Object set functions
    static bool SetObjectRotation(CElement* pElement, const CVector& vecRotation);
    static bool SetObjectScale(CElement* pElement, const CVector& vecScale);
    static bool MoveObject(CResource* pResource, CElement* pElement, unsigned long ulTime, const CVector& vecPosition, const CVector& vecRotation,
                           CEasingCurve::eType a_easingType, double a_fEasingPeriod, double a_fEasingAmplitude, double a_fEasingOvershoot);
    static bool StopObject(CElement* pElement);
    static bool BreakObject(CElement* pElement);
    static bool SetObjectVisibleInAllDimensions(CElement* pElement, bool bVisible, unsigned short usNewDimension = 0);
    static bool SetObjectBreakable(CElement* pElement, const bool bBreakable);
    static bool RespawnObject(CElement* const pElement) noexcept;
    static bool ToggleObjectRespawn(CElement* const pElement, const bool bRespawn) noexcept;

    // Radar area create/destroy funcs
    static CRadarArea* CreateRadarArea(CResource* pResource, const CVector2D& vecPosition, const CVector2D& vecSize, const SColor color, CElement* pVisibleTo);

    // Radar area get funcs
    static bool GetRadarAreaSize(CRadarArea* pRadarArea, CVector2D& vecSize);
    static bool GetRadarAreaColor(CRadarArea* pRadarArea, SColor& outColor);
    static bool IsRadarAreaFlashing(CRadarArea* pRadarArea);
    static bool IsInsideRadarArea(CRadarArea* pRadarArea, const CVector2D& vecPosition, bool& bInside);

    // Radar area set funcs
    static bool SetRadarAreaSize(CElement* pElement, const CVector2D& vecSize);
    static bool SetRadarAreaColor(CElement* pElement, const SColor color);
    static bool SetRadarAreaFlashing(CElement* pElement, bool bFlashing);

    // Pickup create/destroy funcs
    static CPickup* CreatePickup(CResource* pResource, const CVector& vecPosition, unsigned char ucType, double dFive, unsigned long ulRespawnInterval,
                                 double dSix);

    // Pickup get funcs
    static bool GetPickupType(CPickup* pPickup, unsigned char& ucType);
    static bool GetPickupWeapon(CPickup* pPickup, unsigned char& ucWeapon);
    static bool GetPickupAmount(CPickup* pPickup, float& fAmount);
    static bool GetPickupAmmo(CPickup* pPickup, unsigned short& ucAmmo);
    static bool GetPickupRespawnInterval(CPickup* pPickup, unsigned long& ulInterval);
    static bool IsPickupSpawned(CPickup* pPickup, bool& bSpawned);

    // Pickup set funcs
    static bool SetPickupType(CElement* pElement, unsigned char ucType, double dThree, double dFour);
    static bool SetPickupRespawnInterval(CElement* pElement, unsigned long ulInterval);
    static bool UsePickup(CElement* pElement, CPlayer* pPlayer);

    // Shape create funcs
    static class CColCircle*    CreateColCircle(CResource* pResource, const CVector2D& vecPosition, float fRadius);
    static class CColCuboid*    CreateColCuboid(CResource* pResource, const CVector& vecPosition, const CVector& vecSize);
    static class CColSphere*    CreateColSphere(CResource* pResource, const CVector& vecPosition, float fRadius);
    static class CColRectangle* CreateColRectangle(CResource* pResource, const CVector2D& vecPosition, const CVector2D& vecSize);
    static class CColPolygon*   CreateColPolygon(CResource* pResource, const std::vector<CVector2D>& vecPointList);
    static class CColTube*      CreateColTube(CResource* pResource, const CVector& vecPosition, float fRadius, float fHeight);
    static bool                 IsInsideColShape(CColShape* pColShape, const CVector& vecPosition, bool& inside);
    static void                 RefreshColShapeColliders(CColShape* pColShape);

    // Shape get functions
    static bool GetColShapeRadius(CColShape* pColShape, float& fRadius);
    static bool GetColPolygonPointPosition(CColPolygon* pColPolygon, uint uiPointIndex, CVector2D& vecPoint);

    // Shape set functions
    static bool SetColShapeRadius(CColShape* pColShape, float fRadius);
    static bool SetColShapeSize(CColShape* pColShape, CVector& vecSize);
    static bool SetColPolygonPointPosition(CColPolygon* pColPolygon, uint uiPointIndex, const CVector2D& vecPoint);

    static bool AddColPolygonPoint(CColPolygon* pColPolygon, const CVector2D& vecPoint);
    static bool AddColPolygonPoint(CColPolygon* pColPolygon, uint uiPointIndex, const CVector2D& vecPoint);
    static bool RemoveColPolygonPoint(CColPolygon* pColPolygon, uint uiPointIndex);
    static bool SetColPolygonHeight(CColPolygon* pColPolygon, float fFloor, float fCeil);

    // Weapon funcs
    static CCustomWeapon* CreateWeapon(CResource* pResource, eWeaponType weaponType, CVector vecPosition);
    static bool           GetWeaponNameFromID(unsigned char ucID, char* szName);
    static bool           GetWeaponIDFromName(const char* szName, unsigned char& ucID);
    static bool           FireWeapon(CCustomWeapon* pWeapon);
    static bool           SetWeaponProperty(CCustomWeapon* pWeapon, eWeaponProperty eProperty, short sData);
    static bool           GetWeaponProperty(CCustomWeapon* pWeapon, eWeaponProperty eProperty, short& sData);
    static bool           GetWeaponProperty(CCustomWeapon* pWeapon, eWeaponProperty eProperty, float& fData);
    static bool           SetWeaponProperty(CCustomWeapon* pWeapon, eWeaponProperty eProperty, float fData);
    static bool           SetWeaponState(CCustomWeapon* pWeapon, eWeaponState weaponState);
    static bool           SetWeaponTarget(CCustomWeapon* pWeapon, CElement* pTarget, int boneTarget);
    static bool           SetWeaponTarget(CCustomWeapon* pWeapon, CVector vecTarget);
    static bool           ClearWeaponTarget(CCustomWeapon* pWeapon);
    static bool           SetWeaponOwner(CCustomWeapon* pWeapon, CPlayer* pPlayer);

    static bool SetWeaponFlags(CCustomWeapon* pWeapon, eWeaponFlags flag, bool bData);
    static bool SetWeaponFlags(CCustomWeapon* pWeapon, const SLineOfSightFlags& flags);
    static bool GetWeaponFlags(CCustomWeapon* pWeapon, SLineOfSightFlags& flags);
    static bool GetWeaponFlags(CCustomWeapon* pWeapon, eWeaponFlags flag, bool& bData);

    static bool SetWeaponFiringRate(CCustomWeapon* pWeapon, int iFiringRate);
    static bool ResetWeaponFiringRate(CCustomWeapon* pWeapon);
    static bool GetWeaponFiringRate(CCustomWeapon* pWeapon, int& iFiringRate);
    static bool GetWeaponClipAmmo(CCustomWeapon* pWeapon, int& iClipAmmo);
    static bool GetWeaponAmmo(CCustomWeapon* pWeapon, int& iAmmo);
    static bool SetWeaponAmmo(CCustomWeapon* pWeapon, int iAmmo);
    static bool SetWeaponClipAmmo(CCustomWeapon* pWeapon, int iAmmo);

    // Explosion funcs
    static bool CreateExplosion(const CVector& vecPosition, unsigned char ucType, CElement* pElement);

    // Fire funcs
    static bool CreateFire(const CVector& vecPosition, float fSize, CElement* pElement);

    // Audio funcs
    static bool PlaySoundFrontEnd(CElement* pElement, unsigned char ucSound);
    static bool PlayMissionAudio(CElement* pElement, CVector* vecPosition, unsigned short usSlot);
    static bool PreloadMissionAudio(CElement* pElement, unsigned short usAudio, unsigned short usSlot);

    // Ped body?
    static bool GetBodyPartName(unsigned char ucID, char* szName);
    static bool GetClothesByTypeIndex(unsigned char ucType, unsigned char ucIndex, char* szTextureReturn, char* szModelReturn);
    static bool GetTypeIndexFromClothes(const char* szTexture, const char* szModel, unsigned char& ucTypeReturn, unsigned char& ucIndexReturn);
    static bool GetClothesTypeName(unsigned char ucType, char* szNameReturn);

    // Input funcs
    static bool BindKey(CPlayer* pPlayer, const char* szKey, const char* szHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction,
                        CLuaArguments& Arguments);
    static bool BindKey(CPlayer* pPlayer, const char* szKey, const char* szHitState, const char* szCommandName, const char* szArguments,
                        const char* szResource);
    static bool UnbindKey(CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState = NULL,
                          const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    static bool UnbindKey(CPlayer* pPlayer, const char* szKey, const char* szHitState, const char* szCommandName, const char* szResource);
    static bool IsKeyBound(CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState, const CLuaFunctionRef& iLuaFunction, bool& bBound);
    static bool GetControlState(CPlayer* pPlayer, const char* szControl, bool& bState);
    static bool IsControlEnabled(CPlayer* pPlayer, const char* szControl, bool& bEnabled);

    static bool SetControlState(CPlayer* pPlayer, const char* szControl, bool bState);
    static bool ToggleControl(CPlayer* pPlayer, const char* szControl, bool bEnabled);
    static bool ToggleAllControls(CPlayer* pPlayer, bool bGTAControls, bool bMTAControls, bool bEnabled);

    // Team get funcs
    static CTeam* CreateTeam(CResource* pResource, const char* szTeamName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);
    static CTeam* GetTeamFromName(const char* szTeamName);
    static bool   GetTeamName(CTeam* pTeam, SString& strOutName);
    static bool   GetTeamColor(CTeam* pTeam, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue);
    static bool   CountPlayersInTeam(CTeam* pTeam, unsigned int& uiCount);
    static bool   GetTeamFriendlyFire(CTeam* pTeam, bool& bFriendlyFire);

    // Team set funcs
    static bool SetTeamName(CTeam* pTeam, const char* szTeamName);
    static bool SetTeamColor(CTeam* pTeam, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);
    static bool SetPlayerTeam(CPlayer* pPlayer, CTeam* pTeam);
    static bool SetTeamFriendlyFire(CTeam* pTeam, bool bFriendlyFire);

    // Water funcs
    static CWater* CreateWater(CResource* pResource, CVector* pV1, CVector* pV2, CVector* pV3, CVector* pV4, bool bShallow);
    static bool    SetElementWaterLevel(CWater* pWater, float fLevel);
    static bool    SetAllElementWaterLevel(float fLevel);
    static bool    SetWorldWaterLevel(float fLevel, bool bIncludeWorldNonSeaLevel, bool bIncludeWorldSeaLevel, bool bIncludeOutsideWorldLevel);
    static bool    ResetWorldWaterLevel();
    static bool    GetWaterVertexPosition(CWater* pWater, int iVertexIndex, CVector& vecPosition);
    static bool    SetWaterVertexPosition(CWater* pWater, int iVertexIndex, CVector& vecPosition);
    static bool    GetWaterColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha);
    static bool    SetWaterColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha);
    static bool    ResetWaterColor();

    // Standard server functions
    static unsigned int GetMaxPlayers();
    static bool         SetMaxPlayers(unsigned int uiMax);
    static bool OutputChatBox(const char* szText, CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded,
                              CLuaMain* pLuaMain);
    static void OutputChatBox(const char* szText, const std::vector<CPlayer*>& sendList, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue,
                              bool bColorCoded);
    static void OutputConsole(const char* szText, CElement* pElement);
    static bool SetServerPassword(const SString& strPassword, bool bSave);
    static void ClearChatBox(CElement* pElement);

    // General world get funcs
    static bool GetTime(unsigned char& ucHour, unsigned char& ucMinute);
    static bool GetWeather(unsigned char& ucWeather, unsigned char& ucWeatherBlendingTo);
    static bool GetZoneName(CVector& vecPosition, SString& strOutName, bool bCitiesOnly = false);
    static bool GetGravity(float& fGravity);
    static bool GetGameSpeed(float& fSpeed);
    static bool GetWaveHeight(float& fHeight);
    static bool GetFPSLimit(unsigned short& usLimit);
    static bool GetMinuteDuration(unsigned long& ulDuration);
    static bool IsGarageOpen(unsigned char ucGarageID, bool& bIsOpen);
    static bool GetTrafficLightState(unsigned char& ucState);
    static bool GetTrafficLightsLocked(bool& bLocked);
    static bool GetJetpackMaxHeight(float& fMaxHeight);
    static bool GetAircraftMaxVelocity(float& fVelocity);
    static bool GetInteriorSoundsEnabled(bool& bEnabled);
    static bool GetRainLevel(float& fRainLevel);
    static bool GetSunSize(float& fSunSize);
    static bool GetSunColor(unsigned char& ucCoreR, unsigned char& ucCoreG, unsigned char& ucCoreB, unsigned char& ucCoronaR, unsigned char& ucCoronaG,
                            unsigned char& ucCoronaB);
    static bool GetWindVelocity(float& fVelX, float& fVelY, float& fVelZ);
    static bool GetFarClipDistance(float& fFarClip);
    static bool GetFogDistance(float& fFogDist);
    static bool GetAircraftMaxHeight(float& fMaxHeight);
    static bool GetOcclusionsEnabled(bool& bEnabled);
    static bool GetMoonSize(int& iSize);

    // General world set funcs
    static bool SetTime(unsigned char ucHour, unsigned char ucMinute);
    static bool SetWeather(unsigned char ucWeather);
    static bool SetWeatherBlended(unsigned char ucWeather);
    static bool SetGravity(float fGravity);
    static bool SetGameSpeed(float fSpeed);
    static bool SetWaveHeight(float fHeight);
    static bool GetSkyGradient(unsigned char& ucTopRed, unsigned char& ucTopGreen, unsigned char& ucTopBlue, unsigned char& ucBottomRed,
                               unsigned char& ucBottomGreen, unsigned char& ucBottomBlue);
    static bool SetSkyGradient(unsigned char ucTopRed, unsigned char ucTopGreen, unsigned char ucTopBlue, unsigned char ucBottomRed,
                               unsigned char ucBottomGreen, unsigned char ucBottomBlue);
    static bool ResetSkyGradient();
    static bool GetHeatHaze(SHeatHazeSettings& heatHazeSettings);
    static bool SetHeatHaze(const SHeatHazeSettings& heatHazeSettings);
    static bool ResetHeatHaze();
    static bool SetFPSLimit(unsigned short usLimit, bool bSave);
    static bool SetMinuteDuration(unsigned long ulDuration);
    static bool SetGarageOpen(unsigned char ucGarageID, bool bIsOpen);
    static bool SetGlitchEnabled(const std::string& strGlitchName, bool bEnabled);
    static bool IsGlitchEnabled(const std::string& strGlitchName, bool& bEnabled);
    static bool GetJetpackWeaponEnabled(eWeaponType weaponType, bool& bEnabled);
    static bool SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled);
    static bool SetCloudsEnabled(bool bEnabled);
    static bool GetCloudsEnabled();
    static bool SetTrafficLightState(unsigned char ucState, bool bForced = false);
    static bool SetTrafficLightsLocked(bool bLocked);
    static bool SetJetpackMaxHeight(float fMaxHeight);
    static bool SetInteriorSoundsEnabled(bool bEnable);
    static bool SetRainLevel(float fRainLevel);
    static bool SetSunSize(float fSunSize);
    static bool SetSunColor(unsigned char ucCoreR, unsigned char ucCoreG, unsigned char ucCoreB, unsigned char ucCoronaR, unsigned char ucCoronaG,
                            unsigned char ucCoronaB);
    static bool SetWindVelocity(float fVelX, float fVelY, float fVelZ);
    static bool SetFarClipDistance(float fFarClip);
    static bool SetFogDistance(float fFogDist);
    static bool SetAircraftMaxHeight(float fMaxHeight);
    static bool SetAircraftMaxVelocity(float fVelocity);
    static bool SetOcclusionsEnabled(bool bEnabled);
    static bool ResetRainLevel();
    static bool ResetSunSize();
    static bool ResetSunColor();
    static bool ResetWindVelocity();
    static bool ResetFarClipDistance();
    static bool ResetFogDistance();
    static bool RemoveWorldModel(unsigned short usModel, float fRadius, const CVector& vecPosition, char cInterior);
    static bool RestoreWorldModel(unsigned short usModel, float fRadius, const CVector& vecPosition, char cInterior);
    static bool RestoreAllWorldModels();
    static bool SendSyncIntervals(CPlayer* pPlayer = NULL);
    static bool SetMoonSize(int iMoonSize);
    static bool ResetMoonSize();
    static bool IsWorldSpecialPropertyEnabled(WorldSpecialProperty property);
    static bool SetWorldSpecialPropertyEnabled(WorldSpecialProperty property, bool isEnabled);

    // Loaded Map Functions
    static CElement* GetRootElement();
    static CElement* LoadMapData(CLuaMain* pLuaMain, CElement* pParent, CXMLNode* pNode);
    static CXMLNode* SaveMapData(CElement* pElement, CXMLNode* pNode, bool bChildren);

    // All-Seeing Eye related Functions
    static bool        SetGameType(const char* szGameType);
    static bool        SetMapName(const char* szMapName);
    static const char* GetRuleValue(const char* szKey);
    static bool        SetRuleValue(const char* szKey, const char* szValue);
    static bool        RemoveRuleValue(const char* szKey);
    static bool        GetPlayerAnnounceValue(CElement* pElement, const std::string& strKey, std::string& strOutValue);
    static bool        SetPlayerAnnounceValue(CElement* pElement, const std::string& strKey, const std::string& strValue);

    // Registry funcs
    static const std::string& SQLGetLastError();
    static void               ExecuteSQLCreateTable(const std::string& strTable, const std::string& strDefinition);
    static void               ExecuteSQLDropTable(const std::string& strTable);
    static bool               ExecuteSQLDelete(const std::string& strTable, const std::string& strWhere);
    static bool               ExecuteSQLInsert(const std::string& strTable, const std::string& strValues, const std::string& strColumns);
    static bool               ExecuteSQLSelect(const std::string& strTable, const std::string& strColumns, const std::string& strWhere, unsigned int uiLimit,
                                               CRegistryResult* pResult);
    static bool               ExecuteSQLUpdate(const std::string& strTable, const std::string& strSet, const std::string& strWhere);
    static bool               ExecuteSQLQuery(const std::string& str, CLuaArguments* pArgs, CRegistryResult* pResult);

    // Account get funcs
    static CAccount*                     GetAccount(const char* szName, const char* szPassword, bool bCaseSensitive = true);
    static void                          GetAccounts(lua_State* pLua);
    static CClient*                      GetAccountPlayer(CAccount* pAccount);
    static bool                          IsGuestAccount(CAccount* pAccount, bool& bGuest);
    static std::shared_ptr<CLuaArgument> GetAccountData(CAccount* pAccount, const char* szKey);
    static bool                          GetAllAccountData(lua_State* pLua, CAccount* pAccount);
    static bool                          GetAccountsByData(const SString& dataName, const SString& value, std::vector<CAccount*>& outAccounts);
    static bool                          GetAccountSerial(CAccount* pAccount, SString& strSerial);
    static bool                          GetAccountsBySerial(const SString& strSerial, std::vector<CAccount*>& outAccounts);
    static bool                          GetAccountID(CAccount* pAccount, int& ID);
    static bool                          GetAccountByID(int ID, CAccount*& outAccount);
    static bool                          GetAccountIP(CAccount* pAccount, SString& strIP);
    static bool                          GetAccountsByIP(const SString& strIP, std::vector<CAccount*>& outAccounts);

    // Account set funcs
    static CAccount* AddAccount(const SString& strName, const SString& strPassword, bool bAllowCaseVariations, SString& strOutError);
    static bool      RemoveAccount(CAccount* pAccount);
    static bool      SetAccountName(CAccount* pAccount, SString strNewName, bool bAllowCaseVariations, SString& strOutError);
    static bool      SetAccountPassword(CAccount* pAccount, SString szPassword, CAccountPassword::EAccountPasswordType ePasswordType);
    static bool      SetAccountData(CAccount* pAccount, const char* szKey, CLuaArgument* pArgument);
    static bool      CopyAccountData(CAccount* pAccount, CAccount* pFromAccount);

    // Log in/out funcs
    static bool LogIn(CPlayer* pPlayer, CAccount* pAccount, const char* szPassword);
    static bool LogOut(CPlayer* pPlayer);

    // Admin funcs
    static bool  KickPlayer(CPlayer* pPlayer, SString strResponsible = "Console", SString strReason = "");
    static CBan* BanPlayer(CPlayer* pPlayer, bool bIP, bool bUsername, bool bSerial, CPlayer* pResponsible = NULL, SString strResponsible = "Console",
                           SString strReason = "", time_t tUnban = 0);

    static CBan* AddBan(SString strIP, SString strUsername, SString strSerial, CPlayer* pResponsible = NULL, SString strResponsible = "Console",
                        SString strReason = "", time_t tUnban = 0);
    static bool  RemoveBan(CBan* pBan, CPlayer* pResponsible = NULL);

    static bool GetBans(lua_State* luaVM);
    static bool ReloadBanList();

    static bool GetBanIP(CBan* pBan, SString& strOutIP);
    static bool GetBanSerial(CBan* pBan, SString& strOutSerial);
    static bool GetBanUsername(CBan* pBan, SString& strOutUsername);
    static bool GetBanNick(CBan* pBan, SString& strOutNick);
    static bool GetBanReason(CBan* pBan, SString& strOutReason);
    static bool GetBanAdmin(CBan* pBan, SString& strOutAdmin);

    static bool GetBanTime(CBan* pBan, time_t& time);
    static bool GetUnbanTime(CBan* pBan, time_t& time);

    static bool SetUnbanTime(CBan* pBan, time_t time);
    static bool SetBanReason(CBan* pBan, const SString& strReason);
    static bool SetBanAdmin(CBan* pBan, const SString& strAdminName);
    static bool SetBanNick(CBan* pBan, const SString& strNick);

    // Cursor get funcs
    static bool IsCursorShowing(CPlayer* pPlayer, bool& bShowing);

    // Cursor set funcs
    static bool ShowCursor(CElement* pElement, CLuaMain* pLuaMain, bool bShow, bool bToggleControls);

    // Chat funcs
    static bool ShowChat(CElement* pElement, bool bShow, bool bInputBlocked);

    // Misc funcs
    static bool ResetMapInfo(CElement* pElement);
    static void SendClientTransferBoxVisibility(CPlayer* player = nullptr);
    static bool SetClientTransferBoxVisible(bool visible);

    // Resource funcs
    static CElement* GetResourceMapRootElement(CResource* pResource, const char* szMap);
    static CXMLNode* AddResourceMap(CResource* pResource, const std::string& strFilePath, const std::string& strMapName, int iDimension, CLuaMain* pLUA);
    static CXMLNode* AddResourceConfig(CResource* pResource, const std::string& strFilePath, const std::string& strConfigName, int iType, CLuaMain* pLUA);
    static bool      RemoveResourceFile(CResource* pResource, const char* szFilename);

    // Version funcs
    static unsigned long GetVersion();
    static const char*   GetVersionString();
    static const char*   GetVersionName();
    static SString       GetVersionBuildType();
    static unsigned long GetNetcodeVersion();
    static const char*   GetOperatingSystemName();
    static const char*   GetVersionBuildTag();
    static CMtaVersion   GetVersionSortable();
};
