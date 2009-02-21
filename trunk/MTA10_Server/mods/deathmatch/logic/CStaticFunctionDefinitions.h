/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CStaticFunctionDefinitions.h
*  PURPOSE:     Lua static function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CStaticFunctionDefinitions;

#ifndef __CSTATICFUNCTIONDEFINITIONS_H
#define __CSTATICFUNCTIONDEFINITIONS_H

class CGame;
struct CRegistryResult;
class CElement;
class CColCircle;
class CColCuboid;
class CColSphere;
class CColRectangle;
class CColTube;
class CColShape;
class CLuaArguments;
class CLuaMain;
class CMarker;
class CPlayer;
class CBlip;
class CObject;
class CPickup;
class CRadarArea;
class CResource;
class CTeam;
class CVector;
class CVehicle;
class CAccount;
class CVector2D;
class CXMLNode;
class CLuaArgument;
class CDummy;
class CPed;
class CClient;

#ifndef NULL
#define NULL 0
#endif

class CStaticFunctionDefinitions
{
public:
                                CStaticFunctionDefinitions          ( CGame * pGame );
                                ~CStaticFunctionDefinitions         ( void );

    // Event funcs
    static bool                 AddEvent                            ( CLuaMain* pLuaMain, const char* szName, const char* szArguments, bool bAllowRemoteTrigger );
    static bool                 AddEventHandler                     ( CLuaMain* pLuaMain, const char* szName, CElement* pElement, int iLuaFunction, bool bPropagated );
    static bool                 RemoveEventHandler                  ( CLuaMain* pLuaMain, const char* szName, CElement* pElement, int iLuaFunction );
    static bool                 TriggerEvent                        ( const char* szName, CElement* pElement, const CLuaArguments& Arguments, bool & bWasCancelled );
    static bool                 TriggerClientEvent                  ( CElement* pElement, const char* szName, CElement* pCallWithElement, CLuaArguments& Arguments );

    static bool                 CancelEvent                         ( bool bCancel, const char* szReason );
	static bool					GetCancelReason						( char* szReason );
	static bool                 WasEventCancelled                   ( void );

    // Element create/destroy
    static CDummy*              CreateElement                       ( CResource* pResource, const char* szTypeName, const char* szID );
    static bool                 DestroyElement                      ( CElement* pElement );
    static CElement*            CloneElement                        ( CResource* pResource, CElement* pElement, const CVector& vecPosition, bool bCloneElement );

    // Element get funcs
    static CElement*            GetElementByID                      ( const char* szID, unsigned int uiIndex );
    static CElement*            GetElementByIndex                   ( const char* szType, unsigned int uiIndex );
    static CElement*            GetElementChild                     ( CElement* pElement, unsigned int uiIndex );
    static bool                 GetElementChildrenCount             ( CElement* pElement, unsigned int& uiCount );
    static CLuaArgument*        GetElementData                      ( CElement* pElement, const char* szName, bool bInherit );
    static CLuaArguments*       GetAllElementData                   ( CElement* pElement, CLuaArguments * table );
    static CElement*            GetElementParent                    ( CElement* pElement );
    static bool                 GetElementPosition                  ( CElement* pElement, CVector& vecPosition );
    static bool                 GetElementVelocity                  ( CElement* pElement, CVector& vecVelocity );
    static bool                 GetElementInterior                  ( CElement* pElement, unsigned char& ucInterior );
    static bool                 IsElementWithinColShape             ( CElement* pElement, CColShape* pColShape, bool& bWithin );
    static bool                 IsElementWithinMarker               ( CElement* pElement, CMarker* pMarker, bool& bWithin );
    static bool                 GetElementDimension                 ( CElement* pElement, unsigned short& usDimension );
    static bool                 GetElementZoneName                  ( CElement* pElement, char* szBuffer, unsigned int uiBufferLength, bool bCitiesOnly = false );
    static bool                 IsElementAttached                   ( CElement* pElement );
    static CElement*            GetElementAttachedTo                ( CElement* pElement );
    static CColShape*           GetElementColShape                  ( CElement* pElement );
    static bool                 GetElementAlpha                     ( CElement* pElement, unsigned char& ucAlpha );
    static bool                 GetElementHealth                    ( CElement* pElement, float& fHealth );
    static bool                 GetElementModel                     ( CElement* pElement, unsigned short & usModel );
    static bool                 IsElementInWater                    ( CElement* pElement, bool& bInWater );
    static CElement*            GetElementSyncer                    ( CElement* pElement );
    // Element set funcs
    static bool                 ClearElementVisibleTo               ( CElement* pElement );
    static bool                 SetElementID                        ( CElement* pElement, const char* szID );
    static bool                 SetElementData                      ( CElement* pElement, char* szName, const CLuaArgument& Variable, CLuaMain* pLuaMain, bool bSynchronize );
    static bool                 RemoveElementData                   ( CElement* pElement, const char* szName );
    static bool                 SetElementParent                    ( CElement* pElement, CElement* pParent );
    static bool                 SetElementPosition                  ( CElement* pElement, const CVector& vecPosition );
    static bool                 SetElementVelocity                  ( CElement* pElement, const CVector& vecVelocity );
    static bool                 SetElementVisibleTo                 ( CElement* pElement, CElement* pReference, bool bVisible );
    static bool                 SetElementInterior                  ( CElement* pElement, unsigned char ucInterior, bool bSetPosition, CVector& vecPosition );
    static bool                 SetElementDimension                 ( CElement* pElement, unsigned short usDimension );
    static bool                 AttachElements                      ( CElement* pElement, CElement* pAttachedToElement, CVector& vecPosition, CVector& vecRotation );
    static bool                 DetachElements                      ( CElement* pElement, CElement* pAttachedToElement = NULL );
    static bool                 SetElementAlpha                     ( CElement* pElement, unsigned char ucAlpha );
    static bool                 SetElementHealth                    ( CElement* pElement, float fHealth );
    static bool                 SetElementModel                     ( CElement* pElement, unsigned short usModel );

    // Scoreboard
    static bool                 AddScoreboardColumn                 ( const char* szID, const char* szName, float fWidth );
    static bool                 RemoveScoreboardColumn              ( const char* szID );

    // Player get funcs
    static unsigned int         GetPlayerCount                      ( void );
    static bool                 GetPlayerAmmoInClip                 ( CPlayer* pPlayer, unsigned short& ucAmmo );
    static bool                 GetPlayerTotalAmmo                  ( CPlayer* pPlayer, unsigned short& usAmmo );
    static bool                 GetPlayerWeapon                     ( CPlayer* pPlayer, unsigned char& ucWeapon );
    static CPlayer*             GetPlayerFromNick                   ( const char* szNick );
    static bool                 GetPlayerPing                       ( CPlayer* pPlayer, unsigned int& uiPing );
    static bool                 GetPlayerMoney                      ( CPlayer* pPlayer, long& lMoney );
    static bool                 GetPlayerSourceIP                   ( CPlayer* pPlayer, char* szIP );
    static CPlayer*             GetRandomPlayer                     ( void );
    static bool                 IsPlayerMuted                       ( CPlayer* pPlayer, bool& bMuted );
	static CTeam*               GetPlayerTeam                       ( CPlayer* pPlayer );
    static bool                 CanPlayerUseFunction                ( CPlayer* pPlayer, char* szFunction, bool& bCanUse );
    static bool                 IsPlayerDebuggerVisible             ( CPlayer* pPlayer, bool& bVisible );
    static bool                 GetPlayerWantedLevel                ( CPlayer* pPlayer, unsigned int& uiWantedLevel );
    static bool                 IsPlayerScoreboardForced            ( CPlayer* pPlayer, bool& bForced );
    static bool                 IsPlayerMapForced                   ( CPlayer* pPlayer, bool& bForced );
    static bool                 GetPlayerNametagText                ( CPlayer* pPlayer, char* szBuffer, unsigned int uiBufferLength );
    static bool                 GetPlayerNametagColor               ( CPlayer* pPlayer, unsigned char& ucR, unsigned char& ucG, unsigned char& ucB );
    static bool                 IsPlayerNametagShowing              ( CPlayer* pPlayer, bool& bShowing );
    static const std::string&   GetPlayerSerial                     ( CPlayer* pPlayer );
    static const std::string&   GetPlayerUserName                   ( CPlayer* pPlayer );
    static const std::string&   GetPlayerCommunityID                ( CPlayer* pPlayer );
    static bool                 GetPlayerBlurLevel                  ( CPlayer* pPlayer, unsigned char& ucLevel );
    static bool                 GetPlayerName                       ( CElement* pElement, char* szName );
    static bool                 GetPlayerIP                         ( CElement* pElement, char* szIP );
    static CAccount*            GetPlayerAccount                    ( CElement* pElement );

    // Player set functions
    static bool                 SetPlayerMoney                      ( CElement* pElement, long lMoney );
    static bool                 SetPlayerAmmo                       ( CElement* pElement, unsigned char ucSlot, unsigned short usAmmo, unsigned short usAmmoInClip );
	static bool                 GivePlayerMoney                     ( CElement* pElement, long lMoney );
    static bool                 TakePlayerMoney                     ( CElement* pElement, long lMoney );
    static bool                 ShowPlayerHudComponent              ( CElement* pElement, unsigned char ucComponent, bool bShow );
    static bool                 SetPlayerDebuggerVisible            ( CElement* pElement, bool bVisible );
    static bool                 SetPlayerWantedLevel                ( CElement* pElement, unsigned int iLevel );
    static bool                 ForcePlayerMap                      ( CElement* pElement, bool bVisible );
    static bool                 SetPlayerNametagText                ( CElement* pElement, const char* szText );
    static bool                 SetPlayerNametagColor               ( CElement* pElement, bool bRemoveOverride, unsigned char ucR, unsigned char ucG, unsigned char ucB );
    static bool                 SetPlayerNametagShowing             ( CElement* pElement, bool bShowing );
    static bool                 SpawnPlayer                         ( CPlayer* pPlayer, const CVector& vecPosition, float fRotation, unsigned long ulModel, unsigned char ucInterior, unsigned short usDimension, CTeam* pTeam = NULL );
	static bool					SetPlayerMuted						( CElement* pElement, bool bMuted );
    static bool                 SetPlayerBlurLevel                  ( CElement* pElement, unsigned char ucLevel );
    static bool                 RedirectPlayer                      ( CElement* pElement, const char* szHost, unsigned short usPort, const char* szPassword );
    static bool                 SetPlayerName                       ( CElement* pElement, const char* szName );

    // Ped get funcs
    static CPed*                CreatePed                           ( CResource* pResource, unsigned short usModel, const CVector& vecPosition, float fRotation = 0.0f, bool bSynced = true );
    static bool                 GetPedArmor                         ( CPed* pPed, float& fArmor );
    static bool                 GetPedRotation                      ( CPed* pPed, float& fRotation );
    static bool                 IsPedDead                           ( CPed* pPed, bool& bDead );
    static bool                 IsPedDucked                         ( CPed* pPed, bool& bDucked );
    static bool					GetPedStat						    ( CPed* pPed, unsigned short usStat, float& fValue );
	static CElement*            GetPedTarget                        ( CPed* pPed );
    static bool                 GetPedClothes                       ( CPed* pPed, unsigned char ucType, char* szTextureReturn, char* szModelReturn );
    static bool                 DoesPedHaveJetPack                  ( CPed* pPed, bool& bHasJetPack );
    static bool                 IsPedOnGround                       ( CPed* pPed, bool& bOnGround );
    static bool                 GetPedFightingStyle                 ( CPed* pPed, unsigned char& ucStyle );
    static bool                 GetPedMoveAnim                      ( CPed* pPed, unsigned int& iMoveAnim );
    static bool                 GetPedGravity                       ( CPed* pPed, float& fGravity );
    static CElement*            GetPedContactElement                ( CPed* pPed );
    static bool                 GetPedWeaponSlot                    ( CPed* pPed, unsigned char& ucWeaponSlot );
    static bool                 IsPedDoingGangDriveby               ( CPed* pPed, bool & bDoingGangDriveby );
    static bool                 IsPedOnFire                         ( CPed* pPed, bool & bIsOnFire );
    static bool                 IsPedHeadless                       ( CPed* pPed, bool & bIsHeadless );
    static CVehicle*            GetPedOccupiedVehicle               ( CPed* pPed );
    static bool                 GetPedOccupiedVehicleSeat           ( CPed* pPed, unsigned int& uiSeat );

    // Ped set funcs
    static bool                 SetPedArmor                         ( CElement* pElement, float fArmor );
    static bool                 KillPed                             ( CElement* pElement, CElement* pKiller = NULL, unsigned char ucKillerWeapon = 0xFF, unsigned char ucBodyPart = 0xFF, bool bStealth = false );
    static bool                 SetPedRotation                      ( CElement* pElement, float fRotation );
    static bool					SetPedStat						    ( CElement* pElement, unsigned short usStat, float fValue );
    static bool                 AddPedClothes                       ( CElement* pElement, char* szTexture, char* szModel, unsigned char ucType );
    static bool                 RemovePedClothes                    ( CElement* pElement, unsigned char ucType, char* szTexture = NULL, char* szModel = NULL );
    static bool                 GivePedJetPack                      ( CElement* pElement );
    static bool                 RemovePedJetPack                    ( CElement* pElement );
    static bool                 SetPedFightingStyle                 ( CElement* pElement, unsigned char ucStyle );
    static bool                 SetPedMoveAnim                      ( CElement* pElement, unsigned int iMoveAnim );
    static bool                 SetPedGravity                       ( CElement* pElement, float fGravity );
	static bool                 SetPedChoking                       ( CElement* pElement, bool bChoking );
    static bool                 SetPedWeaponSlot                    ( CElement* pElement, unsigned char ucWeaponSlot );
    static bool                 WarpPedIntoVehicle                  ( CPed* pPed, CVehicle* pVehicle, unsigned int uiSeat = 0 );
    static bool                 RemovePedFromVehicle                ( CElement* pElement );
    static bool                 SetPedDoingGangDriveby              ( CElement * pElement, bool bGangDriveby );
    static bool                 SetPedAnimation                     ( CElement * pElement, const char * szBlockName, const char * szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInteruptable );
    static bool                 SetPedOnFire                        ( CElement * pElement, bool bIsOnFire );
    static bool                 SetPedHeadless                      ( CElement * pElement, bool bIsHeadless );

    // Camera get functions
    static bool                 GetCameraMatrix                     ( CPlayer * pPlayer, CVector & vecPosition, CVector & vecLookAt );
    static CElement*            GetCameraTarget                     ( CPlayer * pPlayer );
    static bool                 GetCameraInterior                   ( CPlayer * pPlayer, unsigned char & ucInterior );

    // Camera set functions
    static bool                 SetCameraMatrix                     ( CElement * pElement, const CVector& vecPosition, const CVector& vecLookAt );
    static bool                 SetCameraTarget                     ( CElement * pElement, CElement* pTarget );
    static bool                 SetCameraInterior                   ( CElement * pElement, unsigned char ucInterior );
    static bool                 FadeCamera                          ( CElement * pElement, bool bFadeIn, float fFadeTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );
    
    // Weapon give/take functions
    static bool                 GiveWeapon                          ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo, bool bSetAsCurrent = false );
    static bool                 TakeWeapon                          ( CElement* pElement, unsigned char ucWeaponID );
    static bool                 TakeAllWeapons                      ( CElement* pElement );
	static bool					GiveWeaponAmmo						( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo );
    static bool                 TakeWeaponAmmo                      ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo );
    static bool                 SetWeaponAmmo                       ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo, unsigned short usAmmoInClip );

    // Vehicle create/destroy functions
    static CVehicle*            CreateVehicle                       ( CResource* pResource, unsigned short usModel, const CVector& vecPosition, const CVector& vecRotation, char* szRegPlate = NULL, bool bDirection = false );

    // Vehicle get functions
    static bool                 GetVehicleColor                     ( CVehicle* pVehicle, unsigned char& ucColor1, unsigned char& ucColor2, unsigned char& ucColor3, unsigned char& ucColor4 );
    static bool                 GetVehicleModelFromName             ( const char* szName, unsigned short& usID );
    static bool                 GetVehicleMaxPassengers             ( CVehicle* pVehicle, unsigned char& ucMaxPassengers );
    static bool                 GetVehicleName                      ( CVehicle* pVehicle, char* szName );
    static bool                 GetVehicleNameFromModel             ( unsigned short usModel, char* szName );
    static CPed*                GetVehicleOccupant                  ( CVehicle* pVehicle, unsigned int uiSeat );
    static CPed*                GetVehicleController                ( CVehicle* pVehicle );
    static bool                 GetVehicleRotation                  ( CVehicle* pVehicle, CVector& vecRotation );
    static bool                 GetVehicleTurnVelocity              ( CVehicle* pVehicle, CVector& vecTurnVelocity );
    static bool                 GetVehicleTurretPosition            ( CVehicle* pVehicle, CVector2D& vecPosition );
    static bool                 IsVehicleLocked                     ( CVehicle* pVehicle, bool& bLocked );
	static bool					GetVehicleUpgradeOnSlot				( CVehicle* pVehicle, unsigned char ucSlot, unsigned short& usUpgrade );
	static bool					GetVehicleUpgradeSlotName			( unsigned char ucSlot, char* szName );
	static bool					GetVehicleUpgradeSlotName			( unsigned short usUpgrade, char* szName );
    static bool					GetVehicleDoorState					( CVehicle* pVehicle, unsigned char ucDoor, unsigned char& ucState );
	static bool					GetVehicleWheelStates				( CVehicle* pVehicle, unsigned char& ucFrontLeft, unsigned char& ucFrontRight, unsigned char& ucRearLeft, unsigned char& ucRearRight );
	static bool					GetVehicleLightState				( CVehicle* pVehicle, unsigned char ucLight, unsigned char& ucState );
	static bool					GetVehiclePanelState				( CVehicle* pVehicle, unsigned char ucPanel, unsigned char& ucState );
    static bool                 GetVehicleOverrideLights            ( CVehicle* pVehicle, unsigned char& ucLights );
    static bool                 GetVehicleDoorsLocked               ( CVehicle* pVehicle, bool& bLocked );
    static bool                 GetVehicleDamageProof               ( CVehicle* pVehicle, bool& bDamageProof );
    static bool                 GetVehicleGunsEnabled               ( CVehicle* pVehicle, bool& bGunsEnabled );
    static bool                 GetVehiclePaintjob                  ( CVehicle* pVehicle, unsigned char& ucPaintjob );
    static bool                 GetVehiclePlateText                 ( CVehicle* pVehicle, char* szPlateText );
    static bool                 IsVehicleFuelTankExplodable         ( CVehicle* pVehicle, bool& bExplodable );
    static bool                 IsVehicleFrozen                     ( CVehicle* pVehicle, bool& bFrozen );
    static bool                 IsVehicleOnGround                   ( CVehicle* pVehicle, bool& bOnGround );
    static bool                 GetVehicleEngineState               ( CVehicle* pVehicle, bool& bState );
    static bool                 IsTrainDerailed                     ( CVehicle* pVehicle, bool& bDerailed );

    // Vehicle set functions
    static bool                 FixVehicle                          ( CElement* pElement );
    static bool                 BlowVehicle                         ( CElement* pElement, bool bExplode );
	static bool                 SetVehicleColor                     ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha );
    static bool                 SetVehicleLandingGearDown           ( CElement* pElement, bool bLandingGearDown );
    static bool                 SetVehicleLocked                    ( CElement* pElement, bool bLocked );
    static bool                 SetVehicleDoorsUndamageable         ( CElement* pElement, bool bDoorsUndamageable );
    static bool                 SetVehicleRotation                  ( CElement* pElement, const CVector& vecRotation );
    static bool                 SetVehicleSirensOn                  ( CElement* pElement, bool bSirensOn );
    static bool                 SetVehicleTurnVelocity              ( CElement* pElement, const CVector& vecTurnVelocity );
	static bool					AddVehicleUpgrade					( CElement* pElement, unsigned short usUpgrade );
    static bool                 AddAllVehicleUpgrades               ( CElement* pElement );
    static bool					RemoveVehicleUpgrade				( CElement* pElement, unsigned short usUpgrade );
	static bool					SetVehicleDoorState					( CElement* pElement, unsigned char ucDoor, unsigned char ucState );
	static bool					SetVehicleWheelStates				( CElement* pElement, unsigned char ucFrontLeft, unsigned char ucFrontRight = -1, unsigned char ucRearLeft = -1, unsigned char ucRearRight = -1 );
	static bool					SetVehicleLightState				( CElement* pElement, unsigned char ucLight, unsigned char ucState );
	static bool					SetVehiclePanelState				( CElement* pElement, unsigned char ucPanel, unsigned char ucState );
    static bool                 SetVehicleIdleRespawnDelay          ( CElement* pElement, unsigned long ulTime );
    static bool                 SetVehicleRespawnDelay              ( CElement* pElement, unsigned long ulTime );
    static bool                 SetVehicleRespawnPosition           ( CElement* pElement, const CVector& vecPosition, const CVector& vecRotation );
    static bool                 ToggleVehicleRespawn                ( CElement* pElement, bool bRespawn );
    static bool                 ResetVehicleExplosionTime           ( CElement* pElement );
    static bool                 ResetVehicleIdleTime                ( CElement* pElement );
    static bool                 SpawnVehicle                        ( CElement* pElement, const CVector& vecPosition, const CVector& vecRotation );
    static bool                 RespawnVehicle                      ( CElement* pElement );
    static bool                 SetVehicleOverrideLights            ( CElement* pElement, unsigned char ucLights );
    static bool                 AttachTrailerToVehicle              ( CVehicle* pVehicle, CVehicle* pTrailer );
    static bool                 DetachTrailerFromVehicle            ( CVehicle* pVehicle, CVehicle* pTrailer = NULL );
    static bool                 SetVehicleEngineState               ( CElement* pElement, bool bState );
    static bool                 SetVehicleDirtLevel                 ( CElement* pElement, float fDirtLevel );
    static bool                 SetVehicleDamageProof               ( CElement* pElement, bool bDamageProof );
    static bool                 SetVehiclePaintjob                  ( CElement* pElement, unsigned char ucPaintjob );
    static bool                 SetVehicleFuelTankExplodable        ( CElement* pElement, bool bExplodable );
    static bool                 SetVehicleFrozen                    ( CVehicle* pVehicle, bool bFrozen );
    static bool                 SetTrainDerailed                    ( CVehicle* pVehicle, bool bDerailed );

    // Marker create/destroy functions
    static CMarker*             CreateMarker                        ( CResource* pResource, const CVector& vecPosition, const char* szType, float fSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, CElement* pVisibleTo );

    // Marker get functions
    static bool                 GetMarkerCount                      ( unsigned int& uiCount );
    static bool                 GetMarkerType                       ( CMarker* pMarker, char* szType );
    static bool                 GetMarkerSize                       ( CMarker* pMarker, float& fSize );
    static bool                 GetMarkerColor                      ( CMarker* pMarker, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha );
    static bool                 GetMarkerTarget                     ( CMarker* pMarker, CVector& vecTarget );
    static bool                 GetMarkerIcon                       ( CMarker* pMarker, char* szIcon );

    // Marker set functions
    static bool                 SetMarkerType                       ( CElement* pElement, const char* szType );
    static bool                 SetMarkerSize                       ( CElement* pElement, float fSize );
    static bool                 SetMarkerColor                      ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha );
    static bool                 SetMarkerTarget                     ( CElement* pElement, const CVector* pTarget );
    static bool                 SetMarkerIcon                       ( CElement* pElement, const char* szIcon );

    // Blip create/destroy functions
    static CBlip*               CreateBlip                          ( CResource* pResource, const CVector& vecPosition, unsigned char ucIcon, unsigned char ucSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, short sOrdering, CElement* pVisibleTo );
    static CBlip*               CreateBlipAttachedTo                ( CResource* pResource, CElement* pElement, unsigned char ucIcon, unsigned char ucSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, short sOrdering, CElement* pVisibleTo );
    static bool                 DestroyBlipsAttachedTo              ( CElement* pElement );

    // Blip get functions
    static bool                 GetBlipIcon                         ( CBlip* pBlip, unsigned char& ucIcon );
    static bool                 GetBlipSize                         ( CBlip* pBlip, unsigned char& ucSize );
    static bool                 GetBlipColor                        ( CBlip* pBlip, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha );
    static bool                 GetBlipOrdering                     ( CBlip* pBlip, short& sOrdering );

    // Blip set functions
    static bool                 SetBlipIcon                         ( CElement* pElement, unsigned char ucIcon );
    static bool                 SetBlipSize                         ( CElement* pElement, unsigned char ucSize );
    static bool                 SetBlipColor                        ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha );
    static bool                 SetBlipOrdering                     ( CElement* pElement, short sOrdering );

    // Object create/destroy functions
    static CObject*             CreateObject                        ( CResource* pResource, unsigned short usModelID, const CVector& vecPosition, const CVector& vecRotation );

    // Object get functions
    static bool                 GetObjectRotation                   ( CObject* pObject, CVector& vecRotation );

    // Object set functions
    static bool                 SetObjectRotation                   ( CElement* pElement, const CVector& vecRotation );
    static bool                 MoveObject                          ( CElement* pElement, unsigned long ulTime, const CVector& vecPosition, const CVector& vecRotation );
    static bool                 StopObject                          ( CElement* pElement );

    // Radar area create/destroy funcs
    static CRadarArea*          CreateRadarArea                     ( CResource* pResource, const CVector2D& vecPosition, const CVector2D& vecSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, CElement* pVisibleTo );

    // Radar area get funcs
    static bool                 GetRadarAreaSize                    ( CRadarArea* pRadarArea, CVector2D& vecSize );
    static bool                 GetRadarAreaColor                   ( CRadarArea* pRadarArea, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha );
    static bool                 IsRadarAreaFlashing                 ( CRadarArea* pRadarArea );
    static bool                 IsInsideRadarArea                   ( CRadarArea* pRadarArea, const CVector2D& vecPosition, bool& bInside );

    // Radar area set funcs
    static bool                 SetRadarAreaSize                    ( CElement* pElement, const CVector2D& vecSize );
    static bool                 SetRadarAreaColor                   ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha );
    static bool                 SetRadarAreaFlashing                ( CElement* pElement, bool bFlashing );

    // Pickup create/destroy funcs
    static CPickup*             CreatePickup                        ( CResource* pResource, const CVector& vecPosition, unsigned char ucType, double dFive, unsigned long ulRespawnInterval, double dSix );

    // Pickup get funcs
    static bool                 GetPickupType                       ( CPickup* pPickup, unsigned char& ucType );
    static bool                 GetPickupWeapon                     ( CPickup* pPickup, unsigned char& ucWeapon );
    static bool                 GetPickupAmount                     ( CPickup* pPickup, float& fAmount );
    static bool                 GetPickupAmmo                       ( CPickup* pPickup, unsigned short& ucAmmo );
    static bool                 GetPickupRespawnInterval            ( CPickup* pPickup, unsigned long& ulInterval );
    static bool                 IsPickupSpawned                     ( CPickup* pPickup, bool & bSpawned );

    // Pickup set funcs
    static bool                 SetPickupType                       ( CElement* pElement, unsigned char ucType, double dThree, double dFour );
    static bool                 SetPickupRespawnInterval            ( CElement* pElement, unsigned long ulInterval );
    static bool                 UsePickup                           ( CElement* pElement, CPlayer * pPlayer );

    // Shape create funcs
    static CColCircle*          CreateColCircle                     ( CResource* pResource, const CVector& vecPosition, float fRadius );
    static CColCuboid*          CreateColCuboid                     ( CResource* pResource, const CVector& vecPosition, const CVector& vecSize );
    static CColSphere*          CreateColSphere                     ( CResource* pResource, const CVector& vecPosition, float fRadius );
    static CColRectangle*       CreateColRectangle                  ( CResource* pResource, const CVector& vecPosition, const CVector2D& vecSize );
    static CColPolygon*         CreateColPolygon                    ( CResource* pResource, const CVector& vecPosition );
    static CColTube*            CreateColTube                       ( CResource* pResource, const CVector& vecPosition, float fRadius, float fHeight );

    // Weapon funcs
    static bool                 GetWeaponNameFromID                 ( unsigned char ucID, char* szName );
    static bool                 GetWeaponIDFromName                 ( const char* szName, unsigned char& ucID );

	// Explosion funcs
	static bool					CreateExplosion						( const CVector& vecPosition, unsigned char ucType, CElement* pElement );

	// Fire funcs
	static bool					CreateFire							( const CVector& vecPosition, float fSize, CElement* pElement );

	// Audio funcs
	static bool					PlaySoundFrontEnd					( CElement* pElement, unsigned long ulSound );
	static bool					PlayMissionAudio					( CElement* pElement, CVector* vecPosition, unsigned short usSlot );
	static bool					PreloadMissionAudio					( CElement* pElement, unsigned short usAudio, unsigned short usSlot );

    // Ped body?
    static bool                 GetBodyPartName                     ( unsigned char ucID, char* szName );
    static bool                 GetClothesByTypeIndex               ( unsigned char ucType, unsigned char ucIndex, char* szTextureReturn, char* szModelReturn );
    static bool                 GetTypeIndexFromClothes             ( char* szTexture, char* szModel, unsigned char& ucTypeReturn, unsigned char& ucIndexReturn );
    static bool                 GetClothesTypeName                  ( unsigned char ucType, char* szNameReturn );

    // Input funcs
    static bool                 BindKey                             ( CPlayer* pPlayer, const char* szKey, const char* szHitState, CLuaMain* pLuaMain, int iLuaFunction, CLuaArguments& Arguments );
    static bool                 UnbindKey                           ( CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState = NULL, int iLuaFunction = -1 );
    static bool                 IsKeyBound                          ( CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState, int iLuaFunction, bool& bBound );
    static bool                 GetControlState                     ( CPlayer* pPlayer, char* szControl, bool& bState );
    static bool                 IsControlEnabled                    ( CPlayer* pPlayer, char* szControl, bool& bEnabled );

    static bool                 SetControlState                     ( CPlayer* pPlayer, char* szControl, bool bState );
    static bool                 ToggleControl                       ( CPlayer* pPlayer, char* szControl, bool bEnabled );
    static bool                 ToggleAllControls                   ( CPlayer* pPlayer, bool bGTAControls, bool bMTAControls, bool bEnabled );
    
    // Team get funcs
    static CTeam*               CreateTeam                          ( CResource* pResource, char* szTeamName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );
    static CTeam*               GetTeamFromName                     ( const char* szTeamName );
    static bool                 GetTeamName                         ( CTeam* pTeam, char* szBuffer, unsigned int uiBufferLength );
    static bool                 GetTeamColor                        ( CTeam* pTeam, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue );
    static bool                 CountPlayersInTeam                  ( CTeam* pTeam, unsigned int& uiCount );
    static bool                 GetTeamFriendlyFire                 ( CTeam* pTeam, bool& bFriendlyFire );

    // Team set funcs
    static bool                 SetTeamName                         ( CTeam* pTeam, char* szTeamName );
    static bool                 SetTeamColor                        ( CTeam* pTeam, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );
    static bool                 SetPlayerTeam                       ( CPlayer* pPlayer, CTeam* pTeam );
    static bool                 SetTeamFriendlyFire                 ( CTeam* pTeam, bool bFriendlyFire );

	// Standard server functions
    static unsigned char        GetMaxPlayers                       ( void );
    static bool                 OutputChatBox                       ( const char* szText, CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded );
    static bool                 OutputConsole                       ( const char* szText, CElement* pElement );
    
    // General world get funcs
    static bool                 GetTime                             ( unsigned char& ucHour, unsigned char& ucMinute );
    static bool                 GetWeather                          ( unsigned char& ucWeather, unsigned char& ucWeatherBlendingTo );
    static bool                 GetZoneName                         ( CVector& vecPosition, char* szBuffer, unsigned int uiBufferLength, bool bCitiesOnly = false );
    static bool                 GetGravity                          ( float& fGravity );
    static bool                 GetGameSpeed                        ( float& fSpeed );
    static bool                 GetWaveHeight                       ( float& fHeight );
	static bool                 GetFPSLimit							( unsigned short& usLimit );
	static bool					GetMinuteDuration					( unsigned long& ulDuration );
    static bool                 IsGarageOpen                        ( unsigned char ucGarageID, bool& bIsOpen );

    // General world set funcs
    static bool                 SetTime                             ( unsigned char ucHour, unsigned char ucMinute );
    static bool                 SetWeather                          ( unsigned char ucWeather );
    static bool                 SetWeatherBlended                   ( unsigned char ucWeather );
    static bool                 SetGravity                          ( float fGravity );
    static bool                 SetGameSpeed                        ( float fSpeed );
    static bool                 SetWaveHeight                       ( float fHeight );
    static bool                 SetSkyGradient                      ( unsigned char ucTopRed, unsigned char ucTopGreen, unsigned char ucTopBlue, unsigned char ucBottomRed, unsigned char ucBottomGreen, unsigned char ucBottomBlue );
    static bool                 ResetSkyGradient                    ( void );
	static bool                 SetFPSLimit							( unsigned short usLimit );
	static bool					SetMinuteDuration					( unsigned long ulDuration );
    static bool                 SetGarageOpen                       ( unsigned char ucGarageID, bool bIsOpen );

    // Loaded Map Functions
    static CElement*            GetRootElement                      ( void );
    static CElement*            LoadMapData                         ( CLuaMain* pLuaMain, CElement* pParent, CXMLNode* pNode );
    static CXMLNode*            SaveMapData                         ( CElement* pElement, CXMLNode* pNode, bool bChildren );

    // All-Seeing Eye related Functions
    static bool                 SetGameType                         ( const char * szGameType );
    static bool                 SetMapName                          ( const char * szMapName );
    static char*                GetRuleValue                        ( char* szKey );
    static bool                 SetRuleValue                        ( char* szKey, char* szValue );
    static bool                 RemoveRuleValue                     ( char* szKey );

    // Registry funcs
    static const std::string&   SQLGetLastError                     ( void );
    static void					ExecuteSQLCreateTable				( std::string strTable, std::string strDefinition );
	static void					ExecuteSQLDropTable					( std::string strTable );
	static bool					ExecuteSQLDelete					( std::string strTable, std::string strWhere );
	static bool					ExecuteSQLInsert					( std::string strTable, std::string strValues, std::string strColumns );
    static bool                 ExecuteSQLSelect					( std::string strTable, std::string strColumns, std::string strWhere, unsigned int uiLimit, CRegistryResult* pResult );
    static bool                 ExecuteSQLUpdate                    ( std::string strTable, std::string strSet, std::string strWhere );
	static bool					ExecuteSQLQuery						( std::string str, CLuaArguments *pArgs, CRegistryResult* pResult );

    // Account get funcs
    static CAccount*            GetAccount                          ( const char* szName, const char* szPassword );
    static bool                 GetAccounts                         ( CLuaMain* pLuaMain );
    static CClient*             GetAccountClient                    ( CAccount* pAccount );
    static bool                 IsGuestAccount                      ( CAccount* pAccount, bool& bGuest );
    static CLuaArgument*        GetAccountData                      ( CAccount* pAccount, char* szKey );
    
    // Account set funcs
    static CAccount*            AddAccount                          ( const char* szName, const char* szPassword );
    static bool                 RemoveAccount                       ( CAccount* pAccount );
    static bool                 SetAccountPassword                  ( CAccount* pAccount, const char* szPassword );
    static bool                 SetAccountData                      ( CAccount* pAccount, char* szKey, CLuaArgument * pArgument );
    static bool                 CopyAccountData                     ( CAccount* pAccount, CAccount* pFromAccount );

    // Log in/out funcs
    static bool                 LogIn                               ( CPlayer* pPlayer, CAccount* pAccount, const char* szPassword );
    static bool                 LogOut                              ( CPlayer* pPlayer );

    // Admin funcs
    static bool                 KickPlayer                          ( CPlayer* pPlayer, CPlayer* pResponsible = NULL, const char* szReason = NULL );
    static CBan*                BanPlayer                           ( CPlayer* pPlayer, bool bIP, bool bUsername, bool bSerial, CPlayer* pResponsible = NULL, const char* szReason = NULL, time_t tUnban = 0 );

    static CBan*                AddBan                              ( const char* szIP, const char* szUsername, const char* szSerial, CPlayer* pResponsible, const char* szReason, time_t tUnban );
    static bool                 RemoveBan                           ( CBan* pBan, CPlayer* pResponsible = NULL );

    static bool					GetBans							    ( CLuaMain* pLuaMain );

    static bool					GetBanIP							( CBan* pBan, char* szIP, size_t size );
    static bool					GetBanSerial						( CBan* pBan, char* szSerial, size_t size );
    static bool					GetBanUsername						( CBan* pBan, char* szUsername, size_t size );
    static bool					GetBanNick							( CBan* pBan, char* szNick, size_t size );
    static bool					GetBanReason						( CBan* pBan, char* szReason, size_t size );
    static bool                 GetBanAdmin                         ( CBan* pBan, char* szAdmin, size_t size );

    static bool					GetBanTime							( CBan* pBan, time_t& time );
    static bool					GetUnbanTime						( CBan* pBan, time_t& time );

    // Cursor get funcs
    static bool                 IsCursorShowing                     ( CPlayer* pPlayer, bool& bShowing );

    // Cursor set funcs
    static bool                 ShowCursor                          ( CElement* pElement, CLuaMain* pLuaMain, bool bShow, bool bToggleControls );

	// Chat funcs
	static bool                 ShowChat							( CElement* pElement, bool bShow );

    // Misc funcs
    static bool                 ResetMapInfo                        ( CElement* pElement );

    // Resource funcs
    static CElement*            GetResourceMapRootElement           ( CResource* pResource, char* szMap );
    static CXMLNode*            AddResourceMap                      ( CResource* pResource, const char* szMapName, int iDimension, CLuaMain* pLUA );
    static CXMLNode*            AddResourceConfig                   ( CResource* pResource, const char* szConfigName, int iType, CLuaMain* pLUA );
    static bool                 RemoveResourceFile                  ( CResource* pResource, const char* szFilename );
};

#endif
