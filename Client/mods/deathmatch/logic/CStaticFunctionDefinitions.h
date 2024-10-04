/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CStaticFunctionDefinitions.h
 *  PURPOSE:     Header for static function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CStaticFunctionDefinitions;

#pragma once

#include <gui/CGUI.h>
#include <gui/CGUIElement.h>

class CStaticFunctionDefinitions
{
public:
    CStaticFunctionDefinitions(class CLuaManager* pLuaManager, CEvents* pEvents, CCoreInterface* pCore, CGame* pGame, CClientGame* pClientGame,
                               CClientManager* pManager);
    ~CStaticFunctionDefinitions();

    static bool AddEvent(CLuaMain& LuaMain, const char* szName, bool bAllowRemoteTrigger);
    static bool AddEventHandler(CLuaMain& LuaMain, const char* szName, CClientEntity& Entity, const CLuaFunctionRef& iLuaFunction, bool bPropagated,
                                EEventPriorityType eventPriority, float fPriorityMod);
    static bool RemoveEventHandler(CLuaMain& LuaMain, const char* szName, CClientEntity& Entity, const CLuaFunctionRef& iLuaFunction);
    static bool TriggerEvent(const char* szName, CClientEntity& Entity, const CLuaArguments& Arguments, bool& bWasCancelled);
    static bool TriggerServerEvent(const char* szName, CClientEntity& CallWithEntity, CLuaArguments& Arguments);
    static bool TriggerLatentServerEvent(const char* szName, CClientEntity& CallWithEntity, CLuaArguments& Arguments, int bandwidth, CLuaMain* pLuaMain,
                                         ushort usResourceNetId);
    static bool CancelEvent(bool bCancel);
    static bool WasEventCancelled();

    // Misc funcs
    static bool DownloadFile(CResource* pResource, const char* szFile, CResource* pRequestResource, CChecksum checksum = CChecksum());

    // Output funcs
    static bool OutputConsole(const char* szText);
    static bool OutputChatBox(const char* szText, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded);
    static bool SetClipboard(SString& strText);
    static bool SetWindowFlashing(bool flash, uint count);
    static bool ClearChatBox();

    // Notification funcs
    static bool CreateTrayNotification(SString strText, eTrayIconType eType, bool useSound);
    static bool IsTrayNotificationEnabled();

    // Element get funcs
    static CClientEntity* GetRootElement();
    static CClientEntity* GetElementByID(const char* szID, unsigned int uiIndex);
    static CClientEntity* GetElementByIndex(const char* szType, unsigned int uiIndex);
    static CClientEntity* GetElementChild(CClientEntity& Entity, unsigned int uiIndex);
    static bool           GetElementMatrix(CClientEntity& Entity, CMatrix& matrix);
    static bool           GetElementPosition(CClientEntity& Entity, CVector& vecPosition);
    static bool           GetElementRotation(CClientEntity& Entity, CVector& vecRotation, eEulerRotationOrder rotationOrder);
    static bool           GetElementVelocity(CClientEntity& Entity, CVector& vecVelocity);
    static bool           GetElementTurnVelocity(CClientEntity& Entity, CVector& vecTurnVelocity);
    static bool           GetElementInterior(CClientEntity& Entity, unsigned char& ucInterior);
    static bool           GetElementBoundingBox(CClientEntity& Entity, CVector& vecMin, CVector& vecMax);
    static bool           GetElementRadius(CClientEntity& Entity, float& fRadius);
    static CClientEntity* GetElementAttachedTo(CClientEntity& Entity);
    static bool           GetElementDistanceFromCentreOfMassToBaseOfModel(CClientEntity& Entity, float& fDistance);
    static bool           GetElementAttachedOffsets(CClientEntity& Entity, CVector& vecPosition, CVector& vecRotation);
    static bool           GetElementAlpha(CClientEntity& Entity, unsigned char& ucAlpha);
    static bool           IsElementOnScreen(CClientEntity& Entity, bool& bOnScreen);
    static bool           GetElementHealth(CClientEntity& Entity, float& fHealth);
    static bool           GetElementModel(CClientEntity& Entity, unsigned short& usModel);
    static bool           IsElementInWater(CClientEntity& Entity, bool& bInWater);
    static bool           IsElementSyncer(CClientEntity& Entity, bool& bIsSyncer);
    static bool           IsElementCollidableWith(CClientEntity& Entity, CClientEntity& ThisEntity, bool& bCanCollide);
    static bool           GetElementCollisionsEnabled(CClientEntity& Entity);
    static bool           IsElementFrozen(CClientEntity& Entity, bool& bFrozen);
    static bool           GetLowLodElement(CClientEntity& Entity, CClientEntity*& pOutLowLodEntity);
    static bool           IsElementLowLod(CClientEntity& Entity, bool& bOutIsLowLod);
    static bool           IsElementCallPropagationEnabled(CClientEntity& Entity, bool& bOutEnabled);

    // Element set funcs
    static CClientDummy* CreateElement(CResource& Resource, const char* szTypeName, const char* szID);
    static bool          DestroyElement(CClientEntity& Entity);
    static bool          SetElementID(CClientEntity& Entity, const char* szID);
    static bool          SetElementData(CClientEntity& Entity, const char* szName, CLuaArgument& Variable, bool bSynchronize);
    static bool          RemoveElementData(CClientEntity& Entity, const char* szName);
    static bool          SetElementMatrix(CClientEntity& Entity, const CMatrix& matrix);
    static bool          SetElementPosition(CClientEntity& Entity, const CVector& vecPosition, bool bWarp = true);
    static bool          SetElementRotation(CClientEntity& Entity, const CVector& vecRotation, eEulerRotationOrder rotationOrder, bool bNewWay);
    static bool          SetElementVelocity(CClientEntity& Element, const CVector& vecVelocity);
    static bool          SetElementAngularVelocity(CClientEntity& Element, const CVector& vecTurnVelocity);
    static bool          SetElementParent(CClientEntity& Element, CClientEntity& Parent, CLuaMain* pLuaMain);
    static bool          SetElementInterior(CClientEntity& Entity, unsigned char ucInterior, bool bSetPosition, CVector& vecPosition);
    static bool          SetElementDimension(CClientEntity& Entity, unsigned short usDimension);
    static bool          AttachElements(CClientEntity& Entity, CClientEntity& AttachedToEntity, CVector& vecPosition, CVector& vecRotation);
    static bool          DetachElements(CClientEntity& Entity, CClientEntity* pAttachedToEntity = NULL);
    static bool          SetElementAttachedOffsets(CClientEntity& Entity, CVector& vecPosition, CVector& vecRotation);
    static bool          SetElementAlpha(CClientEntity& Entity, unsigned char ucAlpha);
    static bool          SetElementHealth(CClientEntity& Entity, float fHealth);
    static bool          SetElementModel(CClientEntity& Entity, unsigned short usModel);
    static bool          SetElementCollisionsEnabled(CClientEntity& Entity, bool bEnabled);
    static bool          SetElementCollidableWith(CClientEntity& Entity, CClientEntity& ThisEntity, bool bCanCollide);
    static bool          SetElementFrozen(CClientEntity& Entity, bool bFrozen);
    static bool          SetLowLodElement(CClientEntity& Entity, CClientEntity* pLowLodEntity);
    static bool          SetElementCallPropagationEnabled(CClientEntity& Entity, bool bEnabled);
    static bool          IsElementFrozenWaitingForGroundToLoad(CClientEntity& Entity, bool& bWaitingForGroundToLoad);

    // Radio funcs
    static bool SetRadioChannel(unsigned char& ucChannel);
    static bool GetRadioChannel(unsigned char& ucChannel);

    // Player get funcs
    static CClientPlayer* GetLocalPlayer();
    static CClientPlayer* GetPlayerFromName(const char* szNick);
    static bool           GetPlayerMoney(long& lMoney);
    static bool           GetPlayerWantedLevel(char& cWanted);
    static bool           GetPlayerNametagText(CClientPlayer& Player, SString& strOutText);
    static bool           GetPlayerNametagColor(CClientPlayer& Player, unsigned char& ucR, unsigned char& ucG, unsigned char& ucB);

    // Player set funcs
    static bool ShowPlayerHudComponent(eHudComponent component, bool bShow);
    static bool IsPlayerHudComponentVisible(eHudComponent component, bool& bOutIsVisible);
    static bool SetPlayerMoney(long lMoney, bool bInstant);
    static bool GivePlayerMoney(long lMoney);
    static bool TakePlayerMoney(long lMoney);
    static bool SetPlayerNametagText(CClientEntity& Entity, const char* szText);
    static bool SetPlayerNametagColor(CClientEntity& Entity, bool bRemoveOverride, unsigned char ucR, unsigned char ucG, unsigned char ucB);
    static bool SetPlayerNametagShowing(CClientEntity& Entity, bool bShowing);

    // Ped funcs
    static CClientPed* CreatePed(CResource& Resource, unsigned long ulModel, const CVector& vecPosition, float fRotation);
    static bool        KillPed(CClientEntity& Entity, CClientEntity* pKiller, unsigned char ucKillerWeapon, unsigned char ucBodyPart, bool bStealth);

    static CClientEntity* GetPedTarget(CClientPed& Ped);
    static bool           GetPedTargetCollision(CClientPed& Ped, CVector& vecOrigin);
    static bool           GetPedTask(CClientPed& Ped, bool bPrimary, unsigned int uiTaskType, std::vector<SString>& outTaskHierarchy);
    static const char*    GetPedSimplestTask(CClientPed& Ped);
    static bool           IsPedDoingTask(CClientPed& Ped, const char* szTaskName, bool& bIsDoingTask);
    static bool           GetPedBonePosition(CClientPed& Ped, eBone bone, CVector& vecPosition);
    static bool           GetPedClothes(CClientPed& Ped, unsigned char ucType, SString& strOutTexture, SString& strOutModel);
    static bool           GetPedControlState(CClientPed& Ped, const char* szControl, bool& bState);
    static bool           GetPedAnalogControlState(CClientPed& Ped, const char* szControl, float& fState, bool bRawInput);
    static bool           IsPedDoingGangDriveby(CClientPed& Ped, bool& bDoingGangDriveby);
    static bool           GetPedFightingStyle(CClientPed& Ped, unsigned char& ucStyle);
    static bool           GetPedMoveAnim(CClientPed& Ped, unsigned int& iMoveAnim);
    static bool           GetPedMoveState(CClientPed& Ped, std::string& strMoveState);
    static bool           IsPedHeadless(CClientPed& Ped, bool& bHeadless);
    static bool           IsPedFrozen(CClientPed& Ped, bool& bFrozen);
    static bool           GetPedOccupiedVehicleSeat(CClientPed& Ped, uint& uiSeat);
    static bool           IsPedFootBloodEnabled(CClientPed& Ped, bool& bHasFootBlood);
    static bool           GetPedCameraRotation(CClientPed& Ped, float& fRotation);
    static bool           GetPedWeaponMuzzlePosition(CClientPed& Ped, CVector& vecPosition);
    static bool           IsPedOnFire(CClientPed& Ped, bool& bOnFire);
    static bool           IsPedInVehicle(CClientPed& Ped, bool& bInVehicle);
    static bool           GetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float& fData);
    static bool           GetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, int& sData);
    static bool           GetWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, CVector& vecData);
    static bool           GetWeaponPropertyFlag(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, bool& bEnable);
    static bool           GetOriginalWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float& fData);
    static bool           GetOriginalWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, int& sData);
    static bool           GetOriginalWeaponProperty(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, CVector& vecData);
    static bool           GetOriginalWeaponPropertyFlag(eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, bool& bEnable);
    static bool           GetPedOxygenLevel(CClientPed& Ped, float& fOxygen);

    static bool SetPedWeaponSlot(CClientEntity& Entity, int iSlot);
    static bool GivePedWeapon(CClientEntity& Entity, uchar ucWeaponID, ushort usWeaponAmmo, bool bSetAsCurrent);
    static bool SetPedRotation(CClientEntity& Entity, float fRotation, bool bNewWay);
    static bool SetPedCanBeKnockedOffBike(CClientEntity& Entity, bool bCanBeKnockedOffBike);
    static bool SetPedAnimation(CClientEntity& Entity, const SString& strBlockName, const char* szAnimName, int iTime, int iBlend, bool bLoop,
                                bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame);
    static bool SetPedAnimationProgress(CClientEntity& Entity, const SString& strAnimName, float fProgress);
    static bool SetPedAnimationSpeed(CClientEntity& Entity, const SString& strAnimName, float fSpeed);
    static bool SetPedMoveAnim(CClientEntity& Entity, unsigned int iMoveAnim);
    static bool AddPedClothes(CClientEntity& Entity, const char* szTexture, const char* szModel, unsigned char ucType);
    static bool RemovePedClothes(CClientEntity& Entity, unsigned char ucType);
    static bool SetPedControlState(CClientEntity& Entity, const char* szControl, bool bState);
    static bool SetPedAnalogControlState(CClientEntity& Entity, const char* szControl, float fState);
    static bool SetPedDoingGangDriveby(CClientEntity& Entity, bool bGangDriveby);
    static bool SetPedFightingStyle(CClientEntity& Entity, unsigned char ucStyle);
    static bool SetPedLookAt(CClientEntity& Entity, CVector& vecPosition, int iTime, int iBlend, CClientEntity* pTarget);
    static bool SetPedHeadless(CClientEntity& Entity, bool bHeadless);
    static bool SetPedFrozen(CClientEntity& Entity, bool bFrozen);
    static bool SetPedFootBloodEnabled(CClientEntity& Entity, bool bHasFootBlood);
    static bool SetPedCameraRotation(CClientEntity& Entity, float fRotation);
    static bool SetPedAimTarget(CClientEntity& Entity, CVector& vecTarget);
    static bool SetPedStat(CClientEntity& Entity, ushort usStat, float fValue);
    static bool SetPedOnFire(CClientEntity& Entity, bool bOnFire);
    static bool RemovePedFromVehicle(CClientPed* pPed);
    static bool WarpPedIntoVehicle(CClientPed* pPed, CClientVehicle* pVehicle, unsigned int uiSeat);
    static bool SetPedOxygenLevel(CClientEntity& Entity, float fOxygen);

    // Extra Clothes functions
    static bool GetBodyPartName(unsigned char ucID, SString& strOutName);
    static bool GetClothesByTypeIndex(unsigned char ucType, unsigned char ucIndex, SString& strOutTexture, SString& strOutModel);
    static bool GetTypeIndexFromClothes(const char* szTexture, const char* szModel, unsigned char& ucTypeReturn, unsigned char& ucIndexReturn);
    static bool GetClothesTypeName(unsigned char ucType, SString& strOutName);

    // Vehicle get funcs
    static CClientVehicle* CreateVehicle(CResource& Resource, unsigned short usModel, const CVector& vecPosition, const CVector& vecRotation,
                                         const char* szRegPlate = NULL, unsigned char ucVariant = 5, unsigned char ucVariant2 = 5);
    static bool            GetVehicleModelFromName(const char* szName, unsigned short& usModel);
    static bool            GetVehicleUpgradeSlotName(unsigned char ucSlot, SString& strOutName);
    static bool            GetVehicleUpgradeSlotName(unsigned short usUpgrade, SString& strOutName);
    static bool            GetVehicleNameFromModel(unsigned short usModel, SString& strOutName);
    static bool            GetHelicopterRotorSpeed(CClientVehicle& Vehicle, float& fSpeed);
    static bool            GetVehicleEngineState(CClientVehicle& Vehicle, bool& bState);
    static bool            IsVehicleDamageProof(CClientVehicle& Vehicle, bool& bDamageProof);
    static bool            IsTrainDerailed(CClientVehicle& Vehicle, bool& bDerailed);
    static bool            IsTrainDerailable(CClientVehicle& Vehicle, bool& bIsDerailable);
    static bool            GetTrainDirection(CClientVehicle& Vehicle, bool& bDirection);
    static bool            GetTrainSpeed(CClientVehicle& Vehicle, float& fSpeed);
    static bool            GetTrainPosition(CClientVehicle& Vehicle, float& fPosition);
    static bool            IsTrainChainEngine(CClientVehicle& Vehicle, bool& bChainEngine);
    static bool            GetVehicleHeadLightColor(CClientVehicle& Vehicle, SColor& outColor);
    static bool            GetVehicleCurrentGear(CClientVehicle& Vehicle, unsigned short& currentGear);
    static bool            GetVehicleVariant(CClientVehicle* pVehicle, unsigned char& ucVariant, unsigned char& ucVariant2);
    static bool            IsVehicleNitroRecharging(CClientVehicle& Vehicle, bool& bRecharging);
    static bool            IsVehicleNitroActivated(CClientVehicle& Vehicle, bool& bActivated);
    static bool            GetVehicleNitroCount(CClientVehicle& Vehicle, char& cCount);
    static bool            GetVehicleNitroLevel(CClientVehicle& Vehicle, float& fLevel);
    static bool            GetHeliBladeCollisionsEnabled(CClientVehicle& Vehicle);
    static bool            IsVehicleWindowOpen(CClientVehicle& Vehicle, uchar ucWindow);
    static bool            SetVehicleModelExhaustFumesPosition(unsigned short usModel, CVector& vecPosition);
    static bool            GetVehicleModelExhaustFumesPosition(unsigned short usModel, CVector& vecPosition);
    static bool            SetVehicleModelDummyPosition(unsigned short usModel, eVehicleDummies eDummy, CVector& vecPosition);
    static bool            GetVehicleModelDummyPosition(unsigned short usModel, eVehicleDummies eDummy, CVector& vecPosition);
    static bool            GetVehicleModelDummyDefaultPosition(unsigned short usModel, eVehicleDummies eDummy, CVector& vecPosition);

    // Vehicle set functions
    static bool FixVehicle(CClientEntity& Entity);
    static bool BlowVehicle(CClientEntity& Entity, std::optional<bool> withExplosion);
    static bool SetVehicleColor(CClientEntity& Entity, const CVehicleColor& color);
    static bool SetVehicleLandingGearDown(CClientEntity& Entity, bool bLandingGearDown);
    static bool SetVehicleLocked(CClientEntity& Entity, bool bLocked);
    static bool SetVehicleDoorsUndamageable(CClientEntity& Entity, bool bDoorsUndamageable);
    static bool SetVehicleRotation(CClientEntity& Entity, const CVector& vecRotation);
    static bool SetVehicleSirensOn(CClientEntity& Entity, bool bSirensOn);
    static bool SetVehicleTurnVelocity(CClientEntity& Entity, const CVector& vecTurnVelocity);
    static bool AddVehicleUpgrade(CClientEntity& Entity, unsigned short usUpgrade);
    static bool AddAllVehicleUpgrades(CClientEntity& Entity);
    static bool RemoveVehicleUpgrade(CClientEntity& Entity, unsigned short usUpgrade);
    static bool SetVehicleDoorState(CClientEntity& Entity, unsigned char ucDoor, unsigned char ucState, bool spawnFlyingComponent);
    static bool SetVehicleWheelStates(CClientEntity& Entity, int iFrontLeft, int iRearLeft = -1, int iFrontRight = -1, int iRearRight = -1);
    static bool SetVehicleLightState(CClientEntity& Entity, unsigned char ucLight, unsigned char ucState);
    static bool SetVehiclePanelState(CClientEntity& Entity, unsigned char ucPanel, unsigned char ucState);
    static bool SetVehicleOverrideLights(CClientEntity& Entity, unsigned char ucLights);
    static bool AttachTrailerToVehicle(CClientVehicle& Vehicle, CClientVehicle& Trailer, const CVector& vecRotationOffsetDegrees);
    static bool DetachTrailerFromVehicle(CClientVehicle& Vehicle, CClientVehicle* pTrailer = NULL);
    static bool SetVehicleEngineState(CClientEntity& Entity, bool bState);
    static bool SetVehicleDirtLevel(CClientEntity& Entity, float fDirtLevel);
    static bool SetVehicleDamageProof(CClientEntity& Entity, bool bDamageProof);
    static bool SetVehiclePaintjob(CClientEntity& Entity, unsigned char ucPaintjob);
    static bool SetVehicleFuelTankExplodable(CClientEntity& Entity, bool bExplodable);
    static bool SetVehicleFrozen(CClientEntity& Entity, bool bFrozen);
    static bool SetVehicleAdjustableProperty(CClientEntity& Entity, unsigned short usAdjustableProperty);
    static bool SetHelicopterRotorSpeed(CClientVehicle& Vehicle, float fSpeed);
    static bool SetTrainDerailed(CClientVehicle& Vehicle, bool bDerailed);
    static bool SetTrainDerailable(CClientVehicle& Vehicle, bool bDerailable);
    static bool SetTrainDirection(CClientVehicle& Vehicle, bool bDirection);
    static bool SetTrainSpeed(CClientVehicle& Vehicle, float fSpeed);
    static bool SetTrainPosition(CClientVehicle& Vehicle, float fPosition);
    static bool SetVehicleHeadLightColor(CClientEntity& Vehicle, const SColor color);
    static bool SetVehicleDoorOpenRatio(CClientEntity& Vehicle, unsigned char ucDoor, float fRatio, unsigned long ulTime = 0);
    static bool SetVehicleSirens(CClientVehicle& pVehicle, unsigned char ucSirenID, SSirenInfo tSirenInfo);
    static bool SetVehicleNitroActivated(CClientEntity& Entity, bool bActivated);
    static bool SetVehicleNitroCount(CClientEntity& Entity, char cCount);
    static bool SetVehicleNitroLevel(CClientEntity& Entity, float fLevel);
    static bool SetVehiclePlateText(CClientEntity& Entity, const SString& strText);
    static bool SetHeliBladeCollisionsEnabled(CClientVehicle& Vehicle, bool bEnabled);
    static bool SetVehicleWindowOpen(CClientVehicle& Vehicle, uchar ucWindow, bool bOpen);

    // Object get funcs
    static CClientObject* CreateObject(CResource& Resource, unsigned short usModelID, const CVector& vecPosition, const CVector& vecRotation, bool bLowLod);
    static bool           GetObjectScale(CClientObject& Object, CVector& vecScale);
    static bool           IsObjectBreakable(CClientObject& Object, bool& bBreakable);
    static bool           IsObjectMoving(CClientEntity& Entity);
    static bool           GetObjectMass(CClientObject& Object, float& fMass);
    static bool           GetObjectTurnMass(CClientObject& Object, float& fTurnMass);
    static bool           GetObjectAirResistance(CClientObject& Object, float& fAirResistance);
    static bool           GetObjectElasticity(CClientObject& Object, float& fElasticity);
    static bool           GetObjectBuoyancyConstant(CClientObject& Object, float& fBuoyancyConstant);
    static bool           GetObjectCenterOfMass(CClientObject& Object, CVector& vecCenterOfMass);
    static bool           IsObjectVisibleInAllDimensions(CClientEntity& Entity);

    // Object set funcs
    static bool SetObjectRotation(CClientEntity& Entity, const CVector& vecRotation);
    static bool MoveObject(CClientEntity& Entity, unsigned long ulTime, const CVector& vecPosition, const CVector& vecDeltaRotation,
                           CEasingCurve::eType a_eEasingType, double a_fEasingPeriod, double a_fEasingAmplitude, double a_fEasingOvershoot);
    static bool StopObject(CClientEntity& Entity);
    static bool SetObjectScale(CClientEntity& Entity, const CVector& vecScale);
    static bool SetObjectStatic(CClientEntity& Entity, bool bStatic);
    static bool SetObjectBreakable(CClientEntity& Entity, bool bBreakable);
    static bool BreakObject(CClientEntity& Entity);
    static bool RespawnObject(CClientEntity& Entity);
    static bool ToggleObjectRespawn(CClientEntity& Entity, bool bRespawn);
    static bool SetObjectMass(CClientEntity& Entity, float fMass);
    static bool SetObjectTurnMass(CClientEntity& Entity, float fTurnMass);
    static bool SetObjectAirResistance(CClientEntity& Entity, float fAirResistance);
    static bool SetObjectElasticity(CClientEntity& Entity, float fElasticity);
    static bool SetObjectBuoyancyConstant(CClientEntity& Entity, float fBuoyancyConstant);
    static bool SetObjectCenterOfMass(CClientEntity& Entity, const CVector& vecCenterOfMass);
    static bool SetObjectVisibleInAllDimensions(CClientEntity& Entity, bool bVisible, unsigned short usNewDimension = 0);

    // Radar-area get funcs
    static CClientRadarArea* CreateRadarArea(CResource& Resource, const CVector2D& vecPosition2D, const CVector2D& vecSize, const SColor color);
    static bool              GetRadarAreaColor(CClientRadarArea* RadarArea, SColor& outColor);
    static bool              GetRadarAreaSize(CClientRadarArea* RadarArea, CVector2D& vecSize);
    static bool              IsRadarAreaFlashing(CClientRadarArea* RadarArea, bool& flashing);
    static bool              SetRadarAreaColor(CClientRadarArea* RadarArea, const SColor color);
    static bool              SetRadarAreaSize(CClientRadarArea* RadarArea, CVector2D vecSize);
    static bool              SetRadarAreaFlashing(CClientRadarArea* RadarArea, bool flashing);
    static bool              IsInsideRadarArea(CClientRadarArea* RadarArea, CVector2D vecPosition, bool& inside);

    // Pickup create/destroy funcs
    static CClientPickup* CreatePickup(CResource& Resource, const CVector& vecPosition, unsigned char ucType, double dFive, unsigned long ulRespawnInterval,
                                       double dSix);

    // Pickup set funcs
    static bool SetPickupType(CClientEntity& Entity, unsigned char ucType, double dThree, double dFour);

    // Explosion funcs
    static bool CreateExplosion(CVector& vecPosition, unsigned char ucType, bool bMakeSound, float fCamShake, bool bDamaging);
    static bool DetonateSatchels();

    // Fire funcs
    static bool CreateFire(CVector& vecPosition, float fSize);
    static bool ExtinguishFireInRadius(CVector& vecPosition, float fRadius);
    static bool ExtinguishAllFires();

    // Light funcs
    static CClientPointLights* CreateLight(CResource& Resource, int iMode, const CVector& vecPosition, float fRadius, SColor color, CVector& vecDirection);
    static bool                GetLightType(CClientPointLights* pLight, int& iMode);
    static bool                GetLightRadius(CClientPointLights* pLight, float& fRadius);
    static bool                GetLightColor(CClientPointLights* pLight, SColor& outColor);
    static bool                GetLightDirection(CClientPointLights* pLight, CVector& vecDirection);
    static bool                SetLightRadius(CClientPointLights* pLight, float fRadius);
    static bool                SetLightColor(CClientPointLights* pLight, SColor color);
    static bool                SetLightDirection(CClientPointLights* pLight, CVector vecDirection);

    // Search light funcs
    static CClientSearchLight* CreateSearchLight(CResource& Resource, const CVector& vecStart, const CVector& vecEnd, float startRadius, float endRadius,
                                                 bool renderSpot);

    // Audio funcs
    static bool PlaySoundFrontEnd(unsigned char ucSound);
    static bool SetAmbientSoundEnabled(eAmbientSoundType eType, bool bMute);
    static bool IsAmbientSoundEnabled(eAmbientSoundType eType, bool& bOutMute);
    static bool ResetAmbientSounds();
    static bool SetWorldSoundEnabled(uint uiGroup, uint uiIndex, bool bMute, bool bImmediate);
    static bool IsWorldSoundEnabled(uint uiGroup, uint uiIndex, bool& bOutMute);
    static bool ResetWorldSounds();
    static bool PlaySFX(CResource* pResource, eAudioLookupIndex containerIndex, int iBankIndex, int iAudioIndex, bool bLoop, CClientSound*& outSound);
    static bool PlaySFX3D(CResource* pResource, eAudioLookupIndex containerIndex, int iBankIndex, int iAudioIndex, const CVector& vecPosition, bool bLoop,
                          CClientSound*& outSound);
    static bool GetSFXStatus(eAudioLookupIndex containerIndex, bool& bOutNotCut);

    // Blip funcs
    static CClientRadarMarker* CreateBlip(CResource& Resource, const CVector& vecPosition, unsigned char ucIcon, unsigned char ucSize, const SColor color,
                                          short sOrdering, unsigned short usVisibleDistance);
    static CClientRadarMarker* CreateBlipAttachedTo(CResource& Resource, CClientEntity& Entity, unsigned char ucIcon, unsigned char ucSize, const SColor color,
                                                    short sOrdering, unsigned short usVisibleDistance);

    static bool SetBlipIcon(CClientEntity& Entity, unsigned char ucIcon);
    static bool SetBlipSize(CClientEntity& Entity, unsigned char ucSize);
    static bool SetBlipColor(CClientEntity& Entity, const SColor color);
    static bool SetBlipOrdering(CClientEntity& Entity, short sOrdering);
    static bool SetBlipVisibleDistance(CClientEntity& Entity, unsigned short usVisibleDistance);

    // Marker create/destroy funcs
    static CClientMarker* CreateMarker(CResource& Resource, const CVector& vecPosition, const char* szType, float fSize, const SColor color, bool ignoreAlphaLimits);

    // Marker get funcs
    static bool GetMarkerTarget(CClientMarker& Marker, CVector& vecTarget);

    // Marker set functions
    static bool SetMarkerType(CClientEntity& Entity, const char* szType);
    static bool SetMarkerSize(CClientEntity& Entity, float fSize);
    static bool SetMarkerColor(CClientEntity& Entity, const SColor color);
    static bool SetMarkerTarget(CClientEntity& Entity, const CVector* pTarget);
    static bool SetMarkerIcon(CClientEntity& Entity, const char* szIcon);
    static bool SetMarkerTargetArrowProperties(CClientEntity& Entity, const SColor color, float size);

    // Camera get funcs
    static bool           GetCameraMatrix(CVector& vecPosition, CVector& vecLookAt, float& fRoll, float& fFOV);
    static CClientEntity* GetCameraTarget();
    static bool           GetCameraInterior(unsigned char& ucInterior);

    // Camera set funcs
    static bool SetCameraMatrix(const CVector& vecPosition, CVector* pvecLookAt, float fRoll, float fFOV);
    static bool SetCameraTarget(CClientEntity* pEntity);
    static bool SetCameraTarget(const CVector& vecTarget);
    static bool SetCameraInterior(unsigned char ucInterior);
    static bool FadeCamera(bool bFadeIn, float fFadeTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);

    // Cursor funcs
    static bool GetCursorPosition(CVector2D& vecCursor, CVector& vecWorld);
    static bool IsCursorShowing(bool& bShowing);
    static bool GetCursorAlpha(float& fAlpha);
    static bool SetCursorAlpha(float fAlpha);

    // Drawing funcs
    static ID3DXFont*        ResolveD3DXFont(eFontType fontType, CClientDxFont* pDxFontElement);
    static inline ID3DXFont* ResolveD3DXFont(const std::variant<CClientDxFont*, eFontType>& variant)
    {
        if (std::holds_alternative<eFontType>(variant))
            return g_pCore->GetGraphics()->GetFont(std::get<eFontType>(variant));

        return std::get<CClientDxFont*>(variant)->GetD3DXFont();
    };

    // GUI funcs
    static bool        GUIGetInputEnabled();
    static eInputMode  GUIGetInputMode();
    static void        GUISetInputMode(eInputMode inputMode);
    static eCursorType GUIGetCursorType();

    static CClientGUIElement* GUICreateWindow(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bRelative);
    static CClientGUIElement* GUICreateLabel(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bRelative,
                                             CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateButton(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bRelative,
                                              CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateEdit(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bRelative,
                                            CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateMemo(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bRelative,
                                            CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateGridList(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, bool bRelative,
                                                CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateTabPanel(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, bool bRelative,
                                                CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateScrollPane(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, bool bRelative,
                                                  CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateTab(CLuaMain& LuaMain, const char* szCaption, CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateProgressBar(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, bool bRelative,
                                                   CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateScrollBar(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, bool bHorizontal, bool bRelative,
                                                 CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateCheckBox(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bChecked,
                                                bool bRelative, CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateRadioButton(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bRelative,
                                                   CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateStaticImage(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const SString& strFile, bool bRelative,
                                                   CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateComboBox(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, const char* szCaption, bool bRelative,
                                                CClientGUIElement* pParent);
    static CClientGUIElement* GUICreateBrowser(CLuaMain& LuaMain, const CVector2D& position, const CVector2D& size, bool bIsLocal, bool bIsTransparent,
                                               bool bRelative, CClientGUIElement* pParent);

    static bool GUIStaticImageLoadImage(CClientEntity& Element, const SString& strDir);
    static bool GUIStaticImageGetNativeSize(CClientEntity& Entity, CVector2D& vecSize);

    static bool               GUISetSelectedTab(CClientEntity& Element, CClientEntity& Tab);
    static CClientGUIElement* GUIGetSelectedTab(CClientEntity& Element);
    static bool               GUIDeleteTab(CLuaMain& LuaMain, CClientGUIElement* pTab, CClientGUIElement* pParent);

    static void GUISetEnabled(CClientEntity& Element, bool bFlag);
    static void GUISetProperty(CClientEntity& Element, const char* szProperty, const char* szValue);
    static void GUISetText(CClientEntity& Element, const char* szText);
    static void GUISetSize(CClientEntity& Element, const CVector2D& vecSize, bool bRelative);
    static void GUISetPosition(CClientEntity& Element, const CVector2D& vecPosition, bool bRelative);
    static void GUISetVisible(CClientEntity& Element, bool bFlag);
    static void GUISetAlpha(CClientEntity& Element, float fAlpha);
    static bool GUIBringToFront(CClientEntity& Element);
    static void GUIMoveToBack(CClientEntity& Element);
    static bool GUIBlur(CClientEntity& Element);
    static bool GUIFocus(CClientEntity& Element);

    static void GUICheckBoxSetSelected(CClientEntity& Element, bool bFlag);
    static void GUIRadioButtonSetSelected(CClientEntity& Element, bool bFlag);

    static void GUIProgressBarSetProgress(CClientEntity& Element, int iProgress);

    static void GUIScrollPaneSetScrollBars(CClientEntity& Element, bool bH, bool bV);

    static void GUIScrollBarSetScrollPosition(CClientEntity& Element, int iProgress);

    static void GUIScrollPaneSetHorizontalScrollPosition(CClientEntity& Element, float fProgress);
    static void GUIScrollPaneSetVerticalScrollPosition(CClientEntity& Element, float fProgress);

    static inline const CVector2D GUIGetScreenSize() { return g_pCore->GetGUI()->GetResolution(); };

    static void GUIEditSetReadOnly(CClientEntity& Element, bool bFlag);
    static void GUIEditSetMasked(CClientEntity& Element, bool bFlag);
    static void GUIEditSetMaxLength(CClientEntity& Element, unsigned int iLength);
    static void GUIEditSetCaretIndex(CClientEntity& Element, unsigned int iCaret);

    static void GUIMemoSetReadOnly(CClientEntity& Element, bool bFlag);
    static void GUIMemoSetCaretIndex(CClientEntity& Element, unsigned int iCaret);
    static void GUIMemoSetVerticalScrollPosition(CClientEntity& Element, float fPosition);

    static void                GUIGridListSetSortingEnabled(CClientEntity& Element, bool bEnabled);
    static inline unsigned int GUIGridListAddColumn(CClientGUIElement& GUIElement, const char* szTitle, float fWidth)
    {
        return static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->AddColumn(szTitle, fWidth);
    };
    static inline void GUIGridListRemoveColumn(CClientGUIElement& GUIElement, unsigned int uiColumn)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->RemoveColumn(uiColumn);
    };
    static inline void GUIGridListSetColumnWidth(CClientGUIElement& GUIElement, unsigned int uiColumn, float fWidth, bool bRelative = true)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->SetColumnWidth(uiColumn, fWidth, bRelative);
    };
    static inline void GUIGridListSetColumnTitle(CClientGUIElement& GUIElement, unsigned int uiColumn, const char* szTitle)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->SetColumnTitle(uiColumn, szTitle);
    };
    static void       GUIGridListSetScrollBars(CClientEntity& Element, bool bH, bool bV);
    static inline int GUIGridListAddRow(CClientGUIElement& GUIElement, bool bFast, std::vector<std::pair<SString, bool> >* m_items = NULL)
    {
        return static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->AddRow(bFast, m_items);
    };
    static inline int GUIGridListInsertRowAfter(CClientGUIElement& GUIElement, int iRow, std::vector<std::pair<SString, bool> >* m_items = NULL)
    {
        return static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->InsertRowAfter(iRow, m_items);
    };
    static inline void GUIGridListRemoveRow(CClientGUIElement& GUIElement, int iRow)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->RemoveRow(iRow);
    };
    static inline void GUIGridListAutoSizeColumn(CClientGUIElement& GUIElement, unsigned int uiColumn)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->AutoSizeColumn(uiColumn);
    };
    static void        GUIGridListClear(CClientEntity& Element);
    static inline void GUIGridListSetItemText(CClientGUIElement& GUIElement, int iRow, int iColumn, const char* szText, bool bSection, bool bNumber, bool bFast)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->SetItemText(iRow, iColumn, szText, bNumber, bSection, bFast);
    };
    static void        GUIGridListSetItemData(CClientGUIElement& GUIElement, int iRow, int iColumn, CLuaArgument* Variable);
    static void        GUIItemDataDestroyCallback(void* m_data);
    static void        GUIGridListSetSelectionMode(CClientEntity& Element, unsigned int uiMode);
    static inline void GUIGridListSetSelectedItem(CClientGUIElement& GUIElement, int iRow, int iColumn, bool bReset)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->SetSelectedItem(iRow, iColumn, bReset);
    };
    static inline void GUIGridListSetItemColor(CClientGUIElement& GUIElement, int iRow, int iColumn, int iRed, int iGreen, int iBlue, int iAlpha)
    {
        static_cast<CGUIGridList*>(GUIElement.GetCGUIElement())->SetItemColor(iRow, iColumn, iRed, iGreen, iBlue, iAlpha);
    };
    static void GUIGridListSetHorizontalScrollPosition(CClientEntity& Element, float fPosition);
    static void GUIGridListSetVerticalScrollPosition(CClientEntity& Element, float fPosition);

    static void GUIWindowSetMovable(CClientEntity& Element, bool bFlag);
    static void GUIWindowSetSizable(CClientEntity& Element, bool bFlag);

    static void GUILabelSetColor(CClientEntity& Element, int iR, int iG, int iB);
    static void GUILabelSetVerticalAlign(CClientEntity& Element, CGUIVerticalAlign eAlign);
    static void GUILabelSetHorizontalAlign(CClientEntity& Element, CGUIHorizontalAlign eAlign);

    static int  GUIComboBoxAddItem(CClientEntity& Entity, const char* szText);
    static bool GUIComboBoxRemoveItem(CClientEntity& Entity, int index);
    static bool GUIComboBoxClear(CClientEntity& Entity);

    static int         GUIComboBoxGetSelected(CClientEntity& Entity);
    static bool        GUIComboBoxSetSelected(CClientEntity& Entity, int index);
    static std::string GUIComboBoxGetItemText(CClientEntity& Entity, int index);
    static bool        GUIComboBoxSetItemText(CClientEntity& Entity, int index, const char* szText);
    static int         GUIComboBoxGetItemCount(CClientEntity& Entity);
    static bool        GUIComboBoxSetOpen(CClientEntity& Entity, bool state);
    static bool        GUIComboBoxIsOpen(CClientEntity& Entity);

    // World functions
    static bool GetTime(unsigned char& ucHour, unsigned char& ucMin);
    static bool ProcessLineOfSight(const CVector& vecStart, const CVector& vecEnd, bool& bCollision, CColPoint** pColPoint, CClientEntity** pColEntity,
                                   const SLineOfSightFlags& flags = SLineOfSightFlags(), CEntity* pIgnoredEntity = NULL,
                                   SLineOfSightBuildingResult* pBuildingResult = NULL, SProcessLineOfSightMaterialInfoResult* outMatInfo = nullptr);
    static bool IsLineOfSightClear(const CVector& vecStart, const CVector& vecEnd, bool& bIsClear, const SLineOfSightFlags& flags = SLineOfSightFlags(),
                                   CEntity* pIgnoredEntity = NULL);
    static bool TestLineAgainstWater(CVector& vecStart, CVector& vecEnd, CVector& vecCollision);
    static CClientWater* CreateWater(CResource& resource, CVector* pV1, CVector* pV2, CVector* pV3, CVector* pV4, bool bShallow);
    static bool          GetWaterLevel(CVector& vecPosition, float& fLevel, bool ignoreDistanceToWaterThreshold, CVector& vecUnknown);
    static bool          GetWaterLevel(CClientWater* pWater, float& fLevel);
    static bool          GetWaterVertexPosition(CClientWater* pWater, int iVertexIndex, CVector& vecPosition);
    static bool          SetWorldWaterLevel(float fLevel, void* pChangeSource, bool bIncludeWorldNonSeaLevel, bool bIncludeWorldSeaLevel,
                                            bool bIncludeOutsideWorldLevel);
    static bool          SetPositionWaterLevel(const CVector& vecPosition, float fLevel, void* pChangeSource);
    static bool          SetAllElementWaterLevel(float fLevel, void* pChangeSource);
    static bool          ResetWorldWaterLevel();
    static bool          SetWaterVertexPosition(CClientWater* pWater, int iVertexIndex, CVector& vecPosition);
    static bool          SetWaterDrawnLast(bool bEnabled);
    static bool          IsWaterDrawnLast(bool& bOutEnabled);
    static bool          GetWorldFromScreenPosition(CVector& vecScreen, CVector& vecWorld);
    static bool          GetScreenFromWorldPosition(CVector& vecWorld, CVector& vecScreen, float fEdgeTolerance, bool bRelative);
    static bool          GetWeather(unsigned char& ucWeather, unsigned char& ucWeatherBlendingTo);
    static bool          GetGravity(float& fGravity);
    static bool          GetZoneName(CVector& vecPosition, SString& strOutName, bool bCitiesOnly = false);
    static bool          GetGameSpeed(float& fSpeed);
    static bool          GetMinuteDuration(unsigned long& ulDelay);
    static bool          GetWaveHeight(float& fHeight);
    static bool          IsGarageOpen(unsigned char ucGarageID, bool& bIsOpen);
    static bool          GetGaragePosition(unsigned char ucGarageID, CVector& vecPosition);
    static bool          GetGarageSize(unsigned char ucGarageID, float& fHeight, float& fWidth, float& fDepth);
    static bool          GetGarageBoundingBox(unsigned char ucGarageID, float& fLeft, float& fRight, float& fFront, float& fBack);
    static bool          SetCloudsEnabled(bool bEnabled);
    static bool          GetCloudsEnabled();
    static bool          GetTrafficLightState(unsigned char& ucState);
    static bool          AreTrafficLightsLocked(bool& bLocked);
    static bool          RemoveWorldBuilding(unsigned short usModelToRemove, float fDistance, float fX, float fY, float fZ, char cInterior, uint& uiOutAmount);
    static bool          RestoreWorldBuildings(uint& uiOutAmount);
    static bool RestoreWorldBuilding(unsigned short usModelToRestore, float fDistance, float fX, float fY, float fZ, char cInterior, uint& uiOutAmount);

    static bool SetTime(unsigned char ucHour, unsigned char ucMin);
    static bool GetSkyGradient(unsigned char& ucTopRed, unsigned char& ucTopGreen, unsigned char& ucTopBlue, unsigned char& ucBottomRed,
                               unsigned char& ucBottomGreen, unsigned char& ucBottomBlue);
    static bool SetSkyGradient(unsigned char ucTopRed, unsigned char ucTopGreen, unsigned char ucTopBlue, unsigned char ucBottomRed,
                               unsigned char ucBottomGreen, unsigned char ucBottomBlue);
    static bool ResetSkyGradient();
    static bool GetHeatHaze(SHeatHazeSettings& settings);
    static bool SetHeatHaze(const SHeatHazeSettings& settings);
    static bool ResetHeatHaze();
    static bool GetWaterColor(float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha);
    static bool SetWaterColor(float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha);
    static bool ResetWaterColor();
    static bool SetWeather(unsigned char ucWeather);
    static bool SetWeatherBlended(unsigned char ucWeather);
    static bool SetGravity(float fGravity);
    static bool SetGameSpeed(float fSpeed);
    static bool SetWaveHeight(float fHeight);
    static bool SetMinuteDuration(unsigned long ulDelay);
    static bool SetGarageOpen(unsigned char ucGarageID, bool bIsOpen);
    static bool SetBlurLevel(unsigned char ucLevel);
    static bool SetJetpackMaxHeight(float fHeight);
    static bool SetTrafficLightState(unsigned char ucState);
    static bool SetTrafficLightsLocked(bool bLocked);
    static bool SetWindVelocity(float fX, float fY, float fZ);
    static bool RestoreWindVelocity();
    static bool GetWindVelocity(float& fX, float& fY, float& fZ);
    static bool SetAircraftMaxHeight(float fHeight);
    static bool SetAircraftMaxVelocity(float fVelocity);
    static bool SetOcclusionsEnabled(bool bEnabled);
    static bool CreateSWATRope(CVector vecPosition, DWORD dwDuration);
    static bool SetBirdsEnabled(bool bEnabled);
    static bool GetBirdsEnabled();
    static bool SetMoonSize(int iSize);
    static bool SetFPSLimit(int iLimit);
    static bool GetFPSLimit(int& iLimit);

    static bool ResetAllSurfaceInfo();
    static bool ResetSurfaceInfo(short sSurfaceID);

    // Input functions
    static bool BindKey(const char* szKey, const char* szHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments);
    static bool BindKey(const char* szKey, const char* szHitState, const char* szCommandName, const char* szArguments, const char* szResource);
    static bool UnbindKey(const char* szKey, CLuaMain* pLuaMain, const char* szHitState = 0, const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    static bool UnbindKey(const char* szKey, const char* szHitState, const char* szCommandName, const char* szResource);
    static bool GetKeyState(const char* szKey, bool& bState);
    static bool GetControlState(const char* szControl, bool& bState);
    static bool GetAnalogControlState(const char* szControl, float& fState, bool bRawInput);
    static bool IsControlEnabled(const char* szControl, bool& bEnabled);

    static bool SetControlState(const char* szControl, bool bState);
    static bool ToggleControl(const char* szControl, bool bEnabled);
    static bool ToggleAllControls(bool bGTAControls, bool bMTAControls, bool bEnabled);

    // Projectile functions
    static CClientProjectile* CreateProjectile(CResource& Resource, CClientEntity& Creator, unsigned char ucWeaponType, CVector& vecOrigin, float fForce,
                                               CClientEntity* pTarget, CVector& vecRotation, CVector& vecVelocity, unsigned short usModel = 0);

    // Shape create funcs
    static CClientColCircle*    CreateColCircle(CResource& Resource, const CVector2D& vecPosition, float fRadius);
    static CClientColCuboid*    CreateColCuboid(CResource& Resource, const CVector& vecPosition, const CVector& vecSize);
    static CClientColSphere*    CreateColSphere(CResource& Resource, const CVector& vecPosition, float fRadius);
    static CClientColRectangle* CreateColRectangle(CResource& Resource, const CVector2D& vecPosition, const CVector2D& vecSize);
    static CClientColPolygon*   CreateColPolygon(CResource& Resource, const CVector2D& vecPosition);
    static CClientColTube*      CreateColTube(CResource& Resource, const CVector& vecPosition, float fRadius, float fHeight);
    static CClientColShape*     GetElementColShape(CClientEntity* pEntity);
    static bool                 IsInsideColShape(CClientColShape* pColShape, const CVector& vecPosition, bool& inside);
    static void                 RefreshColShapeColliders(CClientColShape* pColShape);

    // Shape get functions
    static bool GetColShapeRadius(CClientColShape* pColShape, float& fRadius);
    static bool GetColPolygonPointPosition(CClientColPolygon* pColPolygon, uint uiPointIndex, CVector2D& vecPoint);

    // Shape set functions
    static bool SetColShapeRadius(CClientColShape* pColShape, float fRadius);
    static bool SetColShapeSize(CClientColShape* pColShape, CVector& vecSize);
    static bool SetColPolygonPointPosition(CClientColPolygon* pColPolygon, uint uiPointIndex, const CVector2D& vecPoint);

    static bool AddColPolygonPoint(CClientColPolygon* pColPolygon, const CVector2D& vecPoint);
    static bool AddColPolygonPoint(CClientColPolygon* pColPolygon, uint uiPointIndex, const CVector2D& vecPoint);
    static bool RemoveColPolygonPoint(CClientColPolygon* pColPolygon, uint uiPointIndex);

    // Weapon funcs
    static bool           GetWeaponNameFromID(unsigned char ucID, SString& strOutName);
    static bool           GetWeaponIDFromName(const char* szName, unsigned char& ucID);
    static CClientWeapon* CreateWeapon(CResource& Resource, eWeaponType weaponType, CVector vecPosition);
    static bool           FireWeapon(CClientWeapon* pWeapon);
    static bool           SetWeaponProperty(CClientWeapon* pWeapon, eWeaponProperty eProperty, short sData);
    static bool           GetWeaponProperty(CClientWeapon* pWeapon, eWeaponProperty eProperty, short& sData);
    static bool           GetWeaponProperty(CClientWeapon* pWeapon, eWeaponProperty eProperty, float& fData);
    static bool           SetWeaponProperty(CClientWeapon* pWeapon, eWeaponProperty eProperty, float fData);
    static bool           SetWeaponProperty(CClientWeapon* pWeapon, eWeaponProperty eProperty, const CVector& vecData);
    static bool           GetWeaponProperty(CClientWeapon* pWeapon, eWeaponProperty eProperty, CVector& vecData);
    static bool           SetWeaponState(CClientWeapon* pWeapon, eWeaponState weaponState);
    static bool           SetWeaponTarget(CClientWeapon* pWeapon, CClientEntity* pTarget, int boneTarget);
    static bool           SetWeaponTarget(CClientWeapon* pWeapon, CVector vecTarget);
    static bool           ClearWeaponTarget(CClientWeapon* pWeapon);

    static bool SetWeaponFlags(CClientWeapon* pWeapon, eWeaponFlags flag, bool bData);
    static bool SetWeaponFlags(CClientWeapon* pWeapon, const SLineOfSightFlags& flags);
    static bool GetWeaponFlags(CClientWeapon* pWeapon, SLineOfSightFlags& flags);
    static bool GetWeaponFlags(CClientWeapon* pWeapon, eWeaponFlags flag, bool& bData);

    static bool SetWeaponFiringRate(CClientWeapon* pWeapon, int iFiringRate);
    static bool ResetWeaponFiringRate(CClientWeapon* pWeapon);
    static bool GetWeaponFiringRate(CClientWeapon* pWeapon, int& iFiringRate);
    static bool GetWeaponClipAmmo(CClientWeapon* pWeapon, int& iClipAmmo);
    static bool GetWeaponAmmo(CClientWeapon* pWeapon, int& iAmmo);
    static bool SetWeaponAmmo(CClientWeapon* pWeapon, int iAmmo);
    static bool SetWeaponClipAmmo(CClientWeapon* pWeapon, int iAmmo);

    // Map funcs
    static bool ForcePlayerMap(bool& bForced);
    static bool IsPlayerMapForced(bool& bForced);
    static bool IsPlayerMapVisible(bool& bVisible);
    static bool GetPlayerMapBoundingBox(CVector& vecMin, CVector& vecMax);

    // Fx funcs
    static bool           FxAddBlood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness);
    static bool           FxAddWood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness);
    static bool           FxAddSparks(CVector& vecPosition, CVector& vecDirection, float fForce, int iCount, CVector vecAcrossLine, bool bBlur, float fSpread,
                                      float fLife);
    static bool           FxAddTyreBurst(CVector& vecPosition, CVector& vecDirection);
    static bool           FxAddBulletImpact(CVector& vecPosition, CVector& vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity);
    static bool           FxAddPunchImpact(CVector& vecPosition, CVector& vecDirection);
    static bool           FxAddDebris(CVector& vecPosition, RwColor& rwColor, float fScale, int iCount);
    static bool           FxAddGlass(CVector& vecPosition, RwColor& rwColor, float fScale, int iCount);
    static bool           FxAddWaterHydrant(CVector& vecPosition);
    static bool           FxAddGunshot(CVector& vecPosition, CVector& vecDirection, bool bIncludeSparks);
    static bool           FxAddTankFire(CVector& vecPosition, CVector& vecDirection);
    static bool           FxAddWaterSplash(CVector& vecPosition);
    static bool           FxAddBulletSplash(CVector& vecPosition);
    static bool           FxAddFootSplash(CVector& vecPosition);
    static bool           FxCreateParticle(eFxParticleSystems eFxParticle, CVector& vecPosition, CVector& vecDirection, float fR, float fG, float fB, float fA, bool bRandomizeColors, std::uint32_t iCount, float fBrightness, float fSize, bool bRandomizeSizes, float fLife);
    static CClientEffect* CreateEffect(CResource& Resource, const SString& strFxName, const CVector& vecPosition, bool bSoundEnable);

    // Sound funcs
    static CClientSound* PlaySound(CResource* pResource, const SString& strSound, bool bIsURL, bool bIsRawData, bool bLoop, bool bThrottle);
    static CClientSound* PlaySound3D(CResource* pResource, const SString& strSound, bool bIsURL, bool bIsRawData, const CVector& vecPosition, bool bLoop,
                                     bool bThrottle);
    static bool          StopSound(CClientSound& Sound);
    static bool          SetSoundPosition(CClientSound& Sound, double dPosition);
    static bool          GetSoundPosition(CClientSound& Sound, double& dPosition);
    static bool          GetSoundLength(CClientSound& Sound, double& dLength);
    static bool          GetSoundBufferLength(CClientSound& Sound, double& dBufferLength);
    static bool          SetSoundPaused(CClientSound& Sound, bool bPaused);
    static bool          IsSoundPaused(CClientSound& Sound, bool& bPaused);
    static bool          SetSoundVolume(CClientSound& Sound, float fVolume);
    static bool          GetSoundVolume(CClientSound& Sound, float& fVolume);
    static bool          SetSoundSpeed(CClientSound& Sound, float fSpeed);
    static bool          SetSoundProperties(CClientSound& Sound, float fSampleRate, float fTempo, float fPitch, bool bReversed);
    static bool          GetSoundProperties(CClientSound& Sound, float& fSampleRate, float& fTempo, float& fPitch, bool& bReversed);
    static float*        GetSoundFFTData(CClientSound& Sound, int iLength, int iBands = 0);
    static float*        GetSoundWaveData(CClientSound& Sound, int iLength);
    static bool          SetSoundPanEnabled(CClientSound& Sound, bool bEnabled);
    static bool          GetSoundLevelData(CClientSound& Sound, DWORD& dwLeft, DWORD& dwRight);
    static bool          GetSoundBPM(CClientSound& Sound, float& fBPM);
    static bool          IsSoundPanEnabled(CClientSound& Sound);
    static bool          GetSoundSpeed(CClientSound& Sound, float& fSpeed);
    static bool          SetSoundMinDistance(CClientSound& Sound, float fDistance);
    static bool          GetSoundMinDistance(CClientSound& Sound, float& fDistance);
    static bool          SetSoundMaxDistance(CClientSound& Sound, float fDistance);
    static bool          GetSoundMaxDistance(CClientSound& Sound, float& fDistance);
    static bool          GetSoundMetaTags(CClientSound& Sound, const SString& strFormat, SString& strMetaTags);
    static bool          SetSoundEffectEnabled(CClientSound& Sound, const SString& strEffectName, bool bEnable);
    static bool          SetSoundPan(CClientSound& Sound, float fPan);
    static bool          GetSoundPan(CClientSound& Sound, float& fPan);

    // Player Voice Sound Functions

    static bool   SetSoundPosition(CClientPlayer& Player, double dPosition);
    static bool   GetSoundPosition(CClientPlayer& Player, double& dPosition);
    static bool   GetSoundLength(CClientPlayer& Player, double& dLength);
    static bool   SetSoundPaused(CClientPlayer& Sound, bool bPaused);
    static bool   IsSoundPaused(CClientPlayer& Sound, bool& bPaused);
    static bool   SetSoundVolume(CClientPlayer& Player, float fVolume);
    static bool   GetSoundVolume(CClientPlayer& Player, float& fVolume);
    static bool   SetSoundSpeed(CClientPlayer& Player, float fSpeed);
    static float* GetSoundFFTData(CClientPlayer& Player, int iLength, int iBands = 0);
    static float* GetSoundWaveData(CClientPlayer& Player, int iLength);
    static bool   GetSoundLevelData(CClientPlayer& Player, DWORD& dwLeft, DWORD& dwRight);
    static bool   GetSoundSpeed(CClientPlayer& Player, float& fSpeed);
    static bool   SetSoundEffectEnabled(CClientPlayer& Player, const SString& strEffectName, bool bEnable);
    static bool   SetSoundPan(CClientPlayer& Player, float fPan);
    static bool   GetSoundPan(CClientPlayer& Player, float& fPan);

    // Handling funcs
    static eHandlingProperty GetVehicleHandlingEnum(std::string strProperty);
    static bool              GetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, float& fValue);
    static bool              GetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, CVector& vecValue);
    static bool              GetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, std::string& strValue);
    static bool              GetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned int& uiValue);
    static bool              GetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned char& ucValue);
    static bool              GetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, float& fValue);
    static bool              GetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, CVector& vecValue);
    static bool              GetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string& strValue);
    static bool              GetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int& uiValue);
    static bool              GetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char& ucValue);
    static bool              ResetVehicleHandling(CClientVehicle* pVehicle);
    static bool              ResetVehicleHandlingProperty(CClientVehicle* pVehicle, eHandlingProperty eProperty);
    static bool              SetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, float fValue);
    static bool              SetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, CVector vecValue);
    static bool              SetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, std::string strValue);
    static bool              SetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned int uiValue);
    static bool              SetVehicleHandling(CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned char ucValue);
    static bool              SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, float fValue);
    static bool              SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, CVector vecValue);
    static bool              SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string strValue);
    static bool              SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int uiValue);
    static bool              SetEntryHandling(CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char ucValue);

    // Version funcs
    static unsigned long GetVersion();
    static const char*   GetVersionString();
    static const char*   GetVersionName();
    static SString       GetVersionBuildType();
    static unsigned long GetNetcodeVersion();
    static const char*   GetOperatingSystemName();
    static const char*   GetVersionBuildTag();
    static SString       GetVersionSortable();
};
