/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CStaticFunctionDefinitions.h
*  PURPOSE:     Header for static function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Derek Abdine <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CStaticFunctionDefinitions;

#ifndef __CSTATICFUNCTIONDEFINITIONS_H
#define __CSTATICFUNCTIONDEFINITIONS_H

#include <gui/CGUI.h>
#include <gui/CGUIElement.h>

class CStaticFunctionDefinitions
{
public:
                                        CStaticFunctionDefinitions          ( class CLuaManager* pLuaManager,
                                                                              CEvents* pEvents,
                                                                              CCoreInterface* pCore,
                                                                              CGame* pGame,
                                                                              CClientGame* pClientGame,
                                                                              CClientManager* pManager );
                                        ~CStaticFunctionDefinitions         ( void );

    static bool                         AddEvent                            ( CLuaMain& LuaMain, const char* szName, bool bAllowRemoteTrigger );
    static bool                         AddEventHandler                     ( CLuaMain& LuaMain, const char* szName, CClientEntity& Entity, const CLuaFunctionRef& iLuaFunction, bool bPropagated );
    static bool                         RemoveEventHandler                  ( CLuaMain& LuaMain, const char* szName, CClientEntity& Entity, const CLuaFunctionRef& iLuaFunction );
    static bool                         TriggerEvent                        ( const char* szName, CClientEntity& Entity, const CLuaArguments& Arguments, bool& bWasCancelled );
    static bool                         TriggerServerEvent                  ( const char* szName, CClientEntity& CallWithEntity, CLuaArguments& Arguments );
    static bool                         CancelEvent                         ( bool bCancel );
    static bool                         WasEventCancelled                   ( void );

    // Output funcs
    static bool                         OutputConsole                       ( const char* szText );
    static bool                         OutputChatBox                       ( const char* szText, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded );
    static bool                         ShowChat                            ( bool bShow );
    static bool                         SetClipboard                        ( SString& strText );
    static bool                         GetClipboard                        ( SString& strText );

    // Element get funcs
    static CClientEntity*               GetRootElement                      ( void );
    static CClientEntity*               GetElementByID                      ( const char* szID, unsigned int uiIndex );
    static CClientEntity*               GetElementByIndex                   ( const char* szType, unsigned int uiIndex );
    static CClientEntity*               GetElementChild                     ( CClientEntity& Entity, unsigned int uiIndex );
    static bool                         GetElementMatrix                    ( CClientEntity& Entity, CMatrix& matrix );
    static bool                         GetElementPosition                  ( CClientEntity& Entity, CVector & vecPosition );
    static bool                         GetElementRotation                  ( CClientEntity& Entity, CVector & vecRotation, const char* szRotationOrder );
    static bool                         GetElementVelocity                  ( CClientEntity& Entity, CVector& vecVelocity );
    static bool                         GetElementInterior                  ( CClientEntity& Entity, unsigned char& ucInterior );
    static bool                         GetElementZoneName                  ( CClientEntity& Entity, char* szBuffer, unsigned int uiBufferLength, bool bCitiesOnly = false );
    static bool                         GetElementBoundingBox               ( CClientEntity& Entity, CVector &vecMin, CVector &vecMax );
    static bool                         GetElementRadius                    ( CClientEntity& Entity, float &fRadius );
    static CClientEntity*               GetElementAttachedTo                ( CClientEntity& Entity );
    static bool                         GetElementDistanceFromCentreOfMassToBaseOfModel ( CClientEntity& Entity, float& fDistance );
    static bool                         GetElementAttachedOffsets           ( CClientEntity& Entity, CVector & vecPosition, CVector & vecRotation );
    static bool                         GetElementAlpha                     ( CClientEntity& Entity, unsigned char& ucAlpha );
    static bool                         IsElementOnScreen                   ( CClientEntity& Entity, bool& bOnScreen );
    static bool                         GetElementHealth                    ( CClientEntity& Entity, float& fHealth );
    static bool                         GetElementModel                     ( CClientEntity& Entity, unsigned short& usModel );
    static bool                         IsElementInWater                    ( CClientEntity& Entity, bool & bInWater );
    static bool                         IsElementSyncer                     ( CClientEntity& Entity, bool & bIsSyncer );
    static bool                         IsElementCollidableWith             ( CClientEntity& Entity, CClientEntity& ThisEntity, bool & bCanCollide );
    static bool                         GetElementCollisionsEnabled         ( CClientEntity& Entity );
    static bool                         IsElementFrozen                     ( CClientEntity& Entity, bool& bFrozen );

    // Element set funcs
    static CClientDummy*                CreateElement                       ( CResource& Resource, const char* szTypeName, const char* szID );
    static bool                         DestroyElement                      ( CClientEntity& Entity );
    static bool                         SetElementID                        ( CClientEntity& Entity, const char* szID );
    static bool                         SetElementData                      ( CClientEntity& Entity, const char* szName, CLuaArgument& Variable, CLuaMain& LuaMain, bool bSynchronize );
    static bool                         RemoveElementData                   ( CClientEntity& Entity, const char* szName );
    static bool                         SetElementPosition                  ( CClientEntity& Entity, const CVector& vecPosition, bool bWarp = true );
    static bool                         SetElementRotation                  ( CClientEntity& Entity, const CVector& vecRotation, const char* szRotationOrder );
    static bool                         SetElementVelocity                  ( CClientEntity& Element, const CVector& vecVelocity );
    static bool                         SetElementParent                    ( CClientEntity& Element, CClientEntity& Parent, CLuaMain* pLuaMain );
    static bool                         SetElementInterior                  ( CClientEntity& Entity, unsigned char ucInterior, bool bSetPosition, CVector& vecPosition );
    static bool                         SetElementDimension                 ( CClientEntity& Entity, unsigned short usDimension );
    static bool                         AttachElements                      ( CClientEntity& Entity, CClientEntity& AttachedToEntity, CVector& vecPosition, CVector& vecRotation );
    static bool                         DetachElements                      ( CClientEntity& Entity, CClientEntity* pAttachedToEntity = NULL );
    static bool                         SetElementAttachedOffsets           ( CClientEntity& Entity, CVector & vecPosition, CVector & vecRotation );
    static bool                         SetElementAlpha                     ( CClientEntity& Entity, unsigned char ucAlpha );
    static bool                         SetElementHealth                    ( CClientEntity& Entity, float fHealth );
    static bool                         SetElementModel                     ( CClientEntity& Entity, unsigned short usModel );
    static bool                         SetElementCollisionsEnabled         ( CClientEntity& Entity, bool bEnabled );
    static bool                         SetElementCollidableWith            ( CClientEntity& Entity, CClientEntity& ThisEntity, bool bCanCollide );
    static bool                         SetElementFrozen                    ( CClientEntity& Entity, bool bFrozen );

    // Radio funcs
    static bool                         SetRadioChannel                     ( unsigned char& ucChannel );
    static bool                         GetRadioChannel                     ( unsigned char& ucChannel );

    // Player get funcs
    static CClientPlayer*               GetLocalPlayer                      ( void );
    static CClientPlayer*               GetPlayerFromName                   ( const char* szNick );
    static bool                         GetPlayerMoney                      ( long& lMoney );
    static bool                         GetPlayerWantedLevel                ( char& cWanted );
    static bool                         GetPlayerNametagText                ( CClientPlayer & Player, char *  szBuffer, unsigned int uiLength );
    static bool                         GetPlayerNametagColor               ( CClientPlayer & Player, unsigned char & ucR, unsigned char & ucG, unsigned char & ucB );

    // Player set funcs
    static bool                         ShowPlayerHudComponent              ( eHudComponent component, bool bShow );
    static bool                         SetPlayerMoney                      ( long lMoney );
    static bool                         GivePlayerMoney                     ( long lMoney );
    static bool                         TakePlayerMoney                     ( long lMoney );
    static bool                         SetPlayerNametagText                ( CClientEntity& Entity, const char* szText );
    static bool                         SetPlayerNametagColor               ( CClientEntity& Entity, bool bRemoveOverride, unsigned char ucR, unsigned char ucG, unsigned char ucB );
    static bool                         SetPlayerNametagShowing             ( CClientEntity& Entity, bool bShowing );

    // Ped funcs
    static CClientPed*                  CreatePed                           ( CResource& Resource, unsigned long ulModel, const CVector& vecPosition, float fRotation );

    static CClientEntity*               GetPedTarget                        ( CClientPed& Ped );
    static bool                         GetPedTargetCollision               ( CClientPed& Ped, CVector& vecOrigin );
    static bool                         GetPedTask                          ( CClientPed& Ped, bool bPrimary, unsigned int uiTaskType, std::vector < SString >& outTaskHierarchy );
    static char*                        GetPedSimplestTask                  ( CClientPed& Ped );
    static bool                         IsPedDoingTask                      ( CClientPed& Ped, const char* szTaskName, bool& bIsDoingTask );
    static bool                         GetPedBonePosition                  ( CClientPed& Ped, eBone bone, CVector & vecPosition );
    static bool                         GetPedClothes                       ( CClientPed& Ped, unsigned char ucType, char* szTextureReturn, char* szModelReturn );
    static bool                         GetPedControlState                  ( CClientPed& Ped, const char * szControl, bool & bState );
    static bool                         IsPedDoingGangDriveby               ( CClientPed& Ped, bool & bDoingGangDriveby );
    static bool                         GetPedAnimation                     ( CClientPed& Ped, char * szBlockName, char * szAnimName, unsigned int uiLength );
    static bool                         GetPedMoveAnim                      ( CClientPed& Ped, unsigned int& iMoveAnim );
    static bool                         GetPedMoveState                     ( CClientPed & Ped, std::string& strMoveState );
    static bool                         IsPedHeadless                       ( CClientPed& Ped, bool & bHeadless );
    static bool                         IsPedFrozen                         ( CClientPed& Ped, bool & bFrozen );
    static bool                         IsPedFootBloodEnabled               ( CClientPed& Ped, bool & bHasFootBlood );
    static bool                         GetPedCameraRotation                ( CClientPed& Ped, float & fRotation );
    static bool                         GetPedWeaponMuzzlePosition          ( CClientPed& Ped, CVector& vecPosition );
    static bool                         IsPedOnFire                         ( CClientPed& Ped, bool & bOnFire );
    static bool                         IsPedInVehicle                      ( CClientPed& Ped, bool & bInVehicle );

    static bool                         SetPedWeaponSlot                    ( CClientEntity& Entity, int iSlot );
    static bool                         SetPedRotation                      ( CClientEntity& Entity, float fRotation );
    static bool                         SetPedCanBeKnockedOffBike           ( CClientEntity& Entity, bool bCanBeKnockedOffBike );
    static bool                         SetPedAnimation                     ( CClientEntity& Entity, const char * szBlockName, const char * szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame );
    static bool                         SetPedMoveAnim                      ( CClientEntity& Entity, unsigned int iMoveAnim );
    static bool                         AddPedClothes                       ( CClientEntity& Entity, char* szTexture, char* szModel, unsigned char ucType );
    static bool                         RemovePedClothes                    ( CClientEntity& Entity, unsigned char ucType );
    static bool                         SetPedControlState                  ( CClientEntity& Entity, const char* szControl, bool bState );
    static bool                         SetPedDoingGangDriveby              ( CClientEntity& Entity, bool bGangDriveby );
    static bool                         SetPedLookAt                        ( CClientEntity& Entity, CVector & vecPosition, int iTime, int iBlend, CClientEntity * pTarget );
    static bool                         SetPedHeadless                      ( CClientEntity& Entity, bool bHeadless );
    static bool                         SetPedFrozen                        ( CClientEntity& Entity, bool bFrozen );
    static bool                         SetPedFootBloodEnabled              ( CClientEntity& Entity, bool bHasFootBlood );
    static bool                         SetPedCameraRotation                ( CClientEntity& Entity, float fRotation );
    static bool                         SetPedAimTarget                     ( CClientEntity& Entity, CVector & vecTarget );
    static bool                         SetPedOnFire                        ( CClientEntity& Entity, bool bOnFire );

    // Extra Clothes functions
    static bool                         GetBodyPartName                     ( unsigned char ucID, char* szName );
    static bool                         GetClothesByTypeIndex               ( unsigned char ucType, unsigned char ucIndex, char* szTextureReturn, char* szModelReturn );
    static bool                         GetTypeIndexFromClothes             ( char* szTexture, char* szModel, unsigned char& ucTypeReturn, unsigned char& ucIndexReturn );
    static bool                         GetClothesTypeName                  ( unsigned char ucType, char* szNameReturn );


    // Vehicle get funcs
    static CClientVehicle*              CreateVehicle                       ( CResource& Resource, unsigned short usModel, const CVector& vecPosition, const CVector& vecRotation, const char* szRegPlate = NULL );
    static bool                         GetVehicleModelFromName             ( const char* szName, unsigned short& usModel );
    static bool                         GetVehicleUpgradeSlotName           ( unsigned char ucSlot, char* szName, unsigned short len );
    static bool                         GetVehicleUpgradeSlotName           ( unsigned short usUpgrade, char* szName, unsigned short len );
    static bool                         GetVehicleDoorsLocked               ( CClientVehicle& Vehicle, bool& bLocked );
    static bool                         GetVehicleNameFromModel             ( unsigned short usModel, char* szName, unsigned short len );
    static bool                         GetHelicopterRotorSpeed             ( CClientVehicle& Vehicle, float& fSpeed );
    static bool                         GetVehicleEngineState               ( CClientVehicle& Vehicle, bool& bState );
    static bool                         IsVehicleDamageProof                ( CClientVehicle& Vehicle, bool& bDamageProof );
    static bool                         IsTrainDerailed                     ( CClientVehicle& Vehicle, bool& bDerailed );
    static bool                         IsTrainDerailable                   ( CClientVehicle& Vehicle, bool& bIsDerailable );
    static bool                         GetTrainDirection                   ( CClientVehicle& Vehicle, bool& bDirection );
    static bool                         GetTrainSpeed                       ( CClientVehicle& Vehicle, float& fSpeed );
    static bool                         IsVehicleBlown                      ( CClientVehicle& Vehicle, bool& bBlown );
    static bool                         GetVehicleHeadLightColor            ( CClientVehicle& Vehicle, SColor& outColor );
    static bool                         GetVehicleCurrentGear               ( CClientVehicle& Vehicle, unsigned short& currentGear );

    // Vehicle set functions
    static bool                         FixVehicle                          ( CClientEntity& Entity );
    static bool                         BlowVehicle                         ( CClientEntity& Entity, bool bExplode );
    static bool                         SetVehicleColor                     ( CClientEntity& Entity, const CVehicleColor& color );
    static bool                         SetVehicleLandingGearDown           ( CClientEntity& Entity, bool bLandingGearDown );
    static bool                         SetVehicleLocked                    ( CClientEntity& Entity, bool bLocked );
    static bool                         SetVehicleDoorsUndamageable         ( CClientEntity& Entity, bool bDoorsUndamageable );
    static bool                         SetVehicleRotation                  ( CClientEntity& Entity, const CVector& vecRotation );
    static bool                         SetVehicleSirensOn                  ( CClientEntity& Entity, bool bSirensOn );
    static bool                         SetVehicleTurnVelocity              ( CClientEntity& Entity, const CVector& vecTurnVelocity );
    static bool                         AddVehicleUpgrade                   ( CClientEntity& Entity, unsigned short usUpgrade );
    static bool                         AddAllVehicleUpgrades               ( CClientEntity& Entity );
    static bool                         RemoveVehicleUpgrade                ( CClientEntity& Entity, unsigned short usUpgrade );
    static bool                         SetVehicleDoorState                 ( CClientEntity& Entity, unsigned char ucDoor, unsigned char ucState );
    static bool                         SetVehicleWheelStates               ( CClientEntity& Entity, int iFrontLeft, int iRearLeft = -1, int iFrontRight = -1, int iRearRight = -1 );
    static bool                         SetVehicleLightState                ( CClientEntity& Entity, unsigned char ucLight, unsigned char ucState );
    static bool                         SetVehiclePanelState                ( CClientEntity& Entity, unsigned char ucPanel, unsigned char ucState );
    static bool                         SetVehicleOverrideLights            ( CClientEntity& Entity, unsigned char ucLights );
    static bool                         AttachTrailerToVehicle              ( CClientVehicle& Vehicle, CClientVehicle& Trailer );
    static bool                         DetachTrailerFromVehicle            ( CClientVehicle& Vehicle, CClientVehicle* pTrailer = NULL );
    static bool                         SetVehicleEngineState               ( CClientEntity& Entity, bool bState );
    static bool                         SetVehicleDirtLevel                 ( CClientEntity& Entity, float fDirtLevel );
    static bool                         SetVehicleDamageProof               ( CClientEntity& Entity, bool bDamageProof );
    static bool                         SetVehiclePaintjob                  ( CClientEntity& Entity, unsigned char ucPaintjob );
    static bool                         SetVehicleFuelTankExplodable        ( CClientEntity& Entity, bool bExplodable );
    static bool                         SetVehicleFrozen                    ( CClientEntity& Entity, bool bFrozen );
    static bool                         SetVehicleAdjustableProperty        ( CClientEntity& Entity, unsigned short usAdjustableProperty );
    static bool                         SetHelicopterRotorSpeed             ( CClientVehicle& Vehicle, float fSpeed );
    static bool                         SetTrainDerailed                    ( CClientVehicle& Vehicle, bool bDerailed );
    static bool                         SetTrainDerailable                  ( CClientVehicle& Vehicle, bool bDerailable );
    static bool                         SetTrainDirection                   ( CClientVehicle& Vehicle, bool bDirection );
    static bool                         SetTrainSpeed                       ( CClientVehicle& Vehicle, float fSpeed );
    static bool                         SetVehicleHeadLightColor            ( CClientEntity& Vehicle, const SColor color );
    static bool                         SetVehicleDoorOpenRatio             ( CClientEntity& Vehicle, unsigned char ucDoor, float fRatio, unsigned long ulTime = 0 );

    // Object get funcs
    static CClientObject*               CreateObject                        ( CResource& Resource, unsigned short usModelID, const CVector& vecPosition, const CVector& vecRotation );
    static bool                         IsObjectStatic                      ( CClientObject& Object, bool & bStatic );
    static bool                         GetObjectScale                      ( CClientObject& Object, float& fScale );

    // Object set funcs
    static bool                         SetObjectRotation                   ( CClientEntity& Entity, const CVector& vecRotation );
    static bool                         MoveObject                          ( CClientEntity& Entity, unsigned long ulTime, const CVector& vecPosition, const CVector& vecDeltaRotation, CEasingCurve::eType a_eEasingType, double a_fEasingPeriod, double a_fEasingAmplitude, double a_fEasingOvershoot );
    static bool                         StopObject                          ( CClientEntity& Entity );
    static bool                         SetObjectScale                      ( CClientEntity& Entity, float fScale );
    static bool                         SetObjectStatic                     ( CClientEntity& Entity, bool bStatic );
    
    // Radar-area get funcs
    static CClientRadarArea*            CreateRadarArea                     ( CResource& Resource, const CVector2D& vecPosition2D, const CVector2D& vecSize, const SColor color );
    static bool                         GetRadarAreaColor                   ( CClientRadarArea* RadarArea, SColor& outColor );
    static bool                         GetRadarAreaSize                    ( CClientRadarArea* RadarArea, CVector2D& vecSize );
    static bool                         IsRadarAreaFlashing                 ( CClientRadarArea* RadarArea, bool& flashing );
    static bool                         SetRadarAreaColor                   ( CClientRadarArea* RadarArea, const SColor color );
    static bool                         SetRadarAreaSize                    ( CClientRadarArea* RadarArea, CVector2D vecSize );
    static bool                         SetRadarAreaFlashing                ( CClientRadarArea* RadarArea, bool flashing );
    static bool                         IsInsideRadarArea                   ( CClientRadarArea* RadarArea, CVector2D vecPosition, bool& inside );


    // Pickup create/destroy funcs
    static CClientPickup*               CreatePickup                        ( CResource& Resource, const CVector& vecPosition, unsigned char ucType, double dFive, unsigned long ulRespawnInterval, double dSix );

    // Pickup set funcs
    static bool                         SetPickupType                       ( CClientEntity& Entity, unsigned char ucType, double dThree, double dFour );

    // Explosion funcs
    static bool                         CreateExplosion                     ( CVector& vecPosition, unsigned char ucType, bool bMakeSound, float fCamShake, bool bDamaging );
    static bool                         DetonateSatchels                    ( void );

    // Fire funcs
    static bool                         CreateFire                          ( CVector& vecPosition, float fSize );

    // Audio funcs
    static bool                         PlayMissionAudio                    ( const CVector& vecPosition, unsigned short usSound );
    static bool                         PlaySoundFrontEnd                   ( unsigned char ucSound );
    static bool                         PreloadMissionAudio                 ( unsigned short usSound, unsigned short usSlot );
    static bool                         SetAmbientSoundEnabled              ( eAmbientSoundType eType, bool bMute );
    static bool                         IsAmbientSoundEnabled               ( eAmbientSoundType eType, bool& bOutMute );
    static bool                         ResetAmbientSounds                  ( void );

    // Blip funcs
    static CClientRadarMarker*          CreateBlip                          ( CResource& Resource, const CVector& vecPosition, unsigned char ucIcon, unsigned char ucSize, const SColor color, short sOrdering, unsigned short usVisibleDistance );
    static CClientRadarMarker*          CreateBlipAttachedTo                ( CResource& Resource, CClientEntity& Entity, unsigned char ucIcon, unsigned char ucSize, const SColor color, short sOrdering, unsigned short usVisibleDistance );

    static bool                         SetBlipIcon                         ( CClientEntity& Entity, unsigned char ucIcon );
    static bool                         SetBlipSize                         ( CClientEntity& Entity, unsigned char ucSize );
    static bool                         SetBlipColor                        ( CClientEntity& Entity, const SColor color );
    static bool                         SetBlipOrdering                     ( CClientEntity& Entity, short sOrdering );
    static bool                         SetBlipVisibleDistance              ( CClientEntity& Entity, unsigned short usVisibleDistance );

    // Marker create/destroy funcs
    static CClientMarker*               CreateMarker                        ( CResource& Resource, const CVector& vecPosition, const char* szType, float fSize, const SColor color );

    // Marker get funcs
    static bool                         GetMarkerTarget                     ( CClientMarker& Marker, CVector& vecTarget );

    // Marker set functions
    static bool                         SetMarkerType                       ( CClientEntity& Entity, const char* szType );
    static bool                         SetMarkerSize                       ( CClientEntity& Entity, float fSize );
    static bool                         SetMarkerColor                      ( CClientEntity& Entity, const SColor color );
    static bool                         SetMarkerTarget                     ( CClientEntity& Entity, const CVector* pTarget );
    static bool                         SetMarkerIcon                       ( CClientEntity& Entity, const char* szIcon );

    // Camera get funcs
    static bool                         GetCameraViewMode                       ( unsigned short& ucMode );
    static bool                         GetCameraMatrix                     ( CVector& vecPosition, CVector& vecLookAt, float& fRoll, float& fFOV );
    static CClientEntity *              GetCameraTarget                     ( void );
    static bool                         GetCameraInterior                   ( unsigned char & ucInterior );

    // Camera set funcs
    static bool                         SetCameraMatrix                     ( CVector& vecPosition, CVector* pvecLookAt, float fRoll, float fFOV );
    static bool                         SetCameraTarget                     ( CClientEntity * pEntity);
    static bool                         SetCameraInterior                   ( unsigned char ucInterior );
    static bool                         FadeCamera                          ( bool bFadeIn, float fFadeTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );
    static bool                         SetCameraViewMode                       ( unsigned short ucMode );

    // Cursor funcs
    static bool                         GetCursorPosition                   ( CVector2D& vecCursor, CVector& vecWorld );
    static bool                         IsCursorShowing                     ( bool& bShowing );

    // Drawing funcs
    static void                         DrawText                            ( int iLeft, int iTop, int iRight, int iBottom, unsigned long dwColor, const char* szText, float fScaleX, float fScaleY, unsigned long ulFormat, ID3DXFont* pDXFont, bool bPostGUI );
    static CClientDxFont*               CreateDxFont                        ( const SString& strFullFilePath, uint uiSize, bool bBold, const SString& strMetaPath, CResource* pResource );
    static CClientGuiFont*              CreateGuiFont                       ( const SString& strFullFilePath, uint uiSize, const SString& strMetaPath, CResource* pResource );
    static ID3DXFont*                   ResolveD3DXFont                     ( const SString& strFontName, CClientDxFont* pDxFontElement );

    // GUI funcs
    static bool                         GUIGetInputEnabled                  ( void );
    static eInputMode                   GUIGetInputMode                     ( void );
    static void                         GUISetInputMode                     ( eInputMode inputMode );

    static CClientGUIElement*           GUICreateWindow                     ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative );
    static CClientGUIElement*           GUICreateLabel                      ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateButton                     ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateEdit                       ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateMemo                       ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateGridList                   ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateTabPanel                   ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateScrollPane                 ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateTab                        ( CLuaMain& LuaMain, const char *szCaption, CClientGUIElement *pParent );
    static CClientGUIElement*           GUICreateProgressBar                ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateScrollBar                  ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bHorizontal, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateCheckBox                   ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bChecked, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateRadioButton                ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateStaticImage                ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const SString& strFile, bool bRelative, CClientGUIElement* pParent );
    static CClientGUIElement*           GUICreateComboBox                   ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent );
    
    
    static bool                         GUIStaticImageLoadImage             ( CClientEntity& Element, const SString& strDir );

    static bool                         GUISetSelectedTab                   ( CClientEntity& Element, CClientEntity& Tab );
    static CClientGUIElement*           GUIGetSelectedTab                   ( CClientEntity& Element );
    static bool                         GUIDeleteTab                        ( CLuaMain& LuaMain, CClientGUIElement *pTab, CClientGUIElement *pParent );

    static void                         GUISetEnabled                       ( CClientEntity& Element, bool bFlag );
    static void                         GUISetProperty                      ( CClientEntity& Element, const char* szProperty, const char* szValue );
    static void                         GUISetText                          ( CClientEntity& Element, const char* szText );
    static void                         GUISetSize                          ( CClientEntity& Element, const CVector2D& vecSize, bool bRelative );
    static void                         GUISetPosition                      ( CClientEntity& Element, const CVector2D& vecPosition, bool bRelative );
    static void                         GUISetVisible                       ( CClientEntity& Element, bool bFlag );
    static void                         GUISetAlpha                         ( CClientEntity& Element, float fAlpha );
    static bool                         GUIBringToFront                     ( CClientEntity& Element );
    static void                         GUIMoveToBack                       ( CClientEntity& Element );

    static void                         GUICheckBoxSetSelected              ( CClientEntity& Element, bool bFlag );
    static void                         GUIRadioButtonSetSelected           ( CClientEntity& Element, bool bFlag );

    static void                         GUIProgressBarSetProgress           ( CClientEntity& Element, int iProgress );

    static void                         GUIScrollPaneSetScrollBars          ( CClientEntity& Element, bool bH, bool bV );

    static void                         GUIScrollBarSetScrollPosition       ( CClientEntity& Element, int iProgress );

    static void                         GUIScrollPaneSetHorizontalScrollPosition    ( CClientEntity& Element, int iProgress );
    static void                         GUIScrollPaneSetVerticalScrollPosition      ( CClientEntity& Element, int iProgress );

    static inline const CVector2D       GUIGetScreenSize                    ( void )                                                                                    { return g_pCore->GetGUI()->GetResolution (); };
    
    static void                         GUIEditSetReadOnly                  ( CClientEntity& Element, bool bFlag );
    static void                         GUIEditSetMasked                    ( CClientEntity& Element, bool bFlag );
    static void                         GUIEditSetMaxLength                 ( CClientEntity& Element, unsigned int iLength );
    static void                         GUIEditSetCaratIndex                ( CClientEntity& Element, unsigned int iCarat );

    static void                         GUIMemoSetReadOnly                  ( CClientEntity& Element, bool bFlag );
    static void                         GUIMemoSetCaratIndex                ( CClientEntity& Element, unsigned int iCarat );

    static void                         GUIGridListSetSortingEnabled        ( CClientEntity& Element, bool bEnabled );
    static inline unsigned int          GUIGridListAddColumn                ( CClientGUIElement& GUIElement, const char *szTitle, float fWidth )                        { return static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> AddColumn ( szTitle, fWidth ); };
    static inline void                  GUIGridListRemoveColumn             ( CClientGUIElement& GUIElement, unsigned int uiColumn )                                    { static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> RemoveColumn ( uiColumn ); };
    static inline void                  GUIGridListSetColumnWidth           ( CClientGUIElement& GUIElement, unsigned int uiColumn, float fWidth, bool bRelative = true )   { static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetColumnWidth ( uiColumn, fWidth, bRelative ); };
    static void                         GUIGridListSetScrollBars            ( CClientEntity& Element, bool bH, bool bV );
    static inline int                   GUIGridListAddRow                   ( CClientGUIElement& GUIElement, bool bFast )                                                           { return static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> AddRow ( bFast ); };
    static inline int                   GUIGridListInsertRowAfter           ( CClientGUIElement& GUIElement, int iRow )                                             { return static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> InsertRowAfter ( iRow ); };
    static inline void                  GUIGridListRemoveRow                ( CClientGUIElement& GUIElement, int iRow )                                             { static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> RemoveRow ( iRow ); };
    static inline void                  GUIGridListAutoSizeColumn           ( CClientGUIElement& GUIElement, unsigned int uiColumn )                                    { static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> AutoSizeColumn ( uiColumn ); };
    static void                         GUIGridListClear                    ( CClientEntity& Element );
    static inline void                  GUIGridListSetItemText              ( CClientGUIElement& GUIElement, int iRow, int iColumn, const char *szText, bool bSection, bool bNumber, bool bFast )   { static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetItemText ( iRow, iColumn, szText, bNumber, bSection, bFast ); };
    static void                         GUIGridListSetItemData              ( CClientGUIElement& GUIElement, int iRow, int iColumn, CLuaArgument* Variable );
    static void                         GUIItemDataDestroyCallback          ( void* m_data );
    static void                         GUIGridListSetSelectionMode         ( CClientEntity& Element, unsigned int uiMode );
    static inline void                  GUIGridListSetSelectedItem          ( CClientGUIElement& GUIElement, int iRow, int iColumn, bool bReset )                       { static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetSelectedItem ( iRow, iColumn, bReset ); };
    static inline void                  GUIGridListSetItemColor             ( CClientGUIElement& GUIElement, int iRow, int iColumn, int iRed, int iGreen, int iBlue, int iAlpha )       { static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetItemColor ( iRow, iColumn, iRed, iGreen, iBlue, iAlpha ); };


    static void                         GUIWindowSetMovable                 ( CClientEntity& Element, bool bFlag );
    static void                         GUIWindowSetSizable                 ( CClientEntity& Element, bool bFlag );

    static void                         GUILabelSetColor                    ( CClientEntity& Element, int iR, int iG, int iB );
    static void                         GUILabelSetVerticalAlign            ( CClientEntity& Element, CGUIVerticalAlign eAlign );
    static void                         GUILabelSetHorizontalAlign          ( CClientEntity& Element, CGUIHorizontalAlign eAlign );

    static int                          GUIComboBoxAddItem                  ( CClientEntity& Entity, const char* szText );
    static bool                         GUIComboBoxRemoveItem               ( CClientEntity& Entity, int index );
    static bool                         GUIComboBoxClear                    ( CClientEntity& Entity );

    static int                          GUIComboBoxGetSelected              ( CClientEntity& Entity );
    static bool                         GUIComboBoxSetSelected              ( CClientEntity& Entity, int index );
    static std::string                  GUIComboBoxGetItemText              ( CClientEntity& Entity, int index );
    static bool                         GUIComboBoxSetItemText              ( CClientEntity& Entity, int index, const char* szText );

    // World functions
    static bool                         GetTime                             ( unsigned char &ucHour, unsigned char &ucMin );
    static bool                         ProcessLineOfSight                  ( const CVector& vecStart, const CVector& vecEnd, bool& bCollision, CColPoint** pColPoint, CClientEntity** pColEntity, const SLineOfSightFlags& flags = SLineOfSightFlags(), CEntity* pIgnoredEntity = NULL, SLineOfSightBuildingResult* pBuildingResult = NULL );
    static bool                         IsLineOfSightClear                  ( const CVector& vecStart, const CVector& vecEnd, bool& bIsClear, const SLineOfSightFlags& flags = SLineOfSightFlags(), CEntity* pIgnoredEntity = NULL );
    static bool                         TestLineAgainstWater                ( CVector& vecStart, CVector& vecEnd, CVector& vecCollision );
    static CClientWater*                CreateWater                         ( CResource& resource, CVector* pV1, CVector* pV2, CVector* pV3, CVector* pV4, bool bShallow );
    static bool                         GetWaterLevel                       ( CVector& vecPosition, float& fLevel, bool bCheckWaves, CVector& vecUnknown );
    static bool                         GetWaterLevel                       ( CClientWater* pWater, float& fLevel );
    static bool                         GetWaterVertexPosition              ( CClientWater* pWater, int iVertexIndex, CVector& vecPosition );
    static bool                         SetWaterLevel                       ( CVector* pvecPosition, float fLevel, void* pChangeSource = NULL );
    static bool                         SetWaterLevel                       ( CClientWater* pWater, float fLevel, void* pChangeSource = NULL );
    static bool                         SetWaterVertexPosition              ( CClientWater* pWater, int iVertexIndex, CVector& vecPosition );
    static bool                         GetWorldFromScreenPosition          ( CVector& vecScreen, CVector& vecWorld );
    static bool                         GetScreenFromWorldPosition          ( CVector& vecWorld, CVector& vecScreen, float fEdgeTolerance, bool bRelative );
    static bool                         GetWeather                          ( unsigned char& ucWeather, unsigned char& ucWeatherBlendingTo );
    static bool                         GetGravity                          ( float& fGravity );
    static bool                         GetZoneName                         ( CVector& vecPosition, char* szBuffer, unsigned int uiBufferLength, bool bCitiesOnly = false );
    static bool                         GetGameSpeed                        ( float& fSpeed );
    static bool                         GetMinuteDuration                   ( unsigned long& ulDelay );
    static bool                         GetWaveHeight                       ( float& fHeight );
    static bool                         IsGarageOpen                        ( unsigned char ucGarageID, bool& bIsOpen );
    static bool                         GetGaragePosition                   ( unsigned char ucGarageID, CVector& vecPosition );
    static bool                         GetGarageSize                       ( unsigned char ucGarageID, float& fHeight, float& fWidth, float& fDepth );
    static bool                         GetGarageBoundingBox                ( unsigned char ucGarageID, float& fLeft, float& fRight, float& fFront, float& fBack );
    static bool                         IsWorldSpecialPropertyEnabled       ( const char* szPropName );
    static bool                         SetCloudsEnabled                    ( bool bEnabled );
    static bool                         GetCloudsEnabled                    ( void );
    static bool                         GetTrafficLightState                ( unsigned char& ucState );
    static bool                         AreTrafficLightsLocked              ( bool& bLocked );

    static bool                         SetTime                             ( unsigned char ucHour, unsigned char ucMin );
    static bool                         GetSkyGradient                      ( unsigned char& ucTopRed, unsigned char& ucTopGreen, unsigned char& ucTopBlue, unsigned char& ucBottomRed, unsigned char& ucBottomGreen, unsigned char& ucBottomBlue );
    static bool                         SetSkyGradient                      ( unsigned char ucTopRed, unsigned char ucTopGreen, unsigned char ucTopBlue, unsigned char ucBottomRed, unsigned char ucBottomGreen, unsigned char ucBottomBlue );
    static bool                         ResetSkyGradient                    ( void );
    static bool                         GetHeatHaze                         ( SHeatHazeSettings& settings );
    static bool                         SetHeatHaze                         ( const SHeatHazeSettings& settings );
    static bool                         ResetHeatHaze                       ( void );
    static bool                         GetWaterColor                       ( float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha );
    static bool                         SetWaterColor                       ( float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha );
    static bool                         ResetWaterColor                     ( void );
    static bool                         SetWeather                          ( unsigned char ucWeather );
    static bool                         SetWeatherBlended                   ( unsigned char ucWeather );
    static bool                         SetGravity                          ( float fGravity );
    static bool                         SetGameSpeed                        ( float fSpeed );
    static bool                         SetWaveHeight                       ( float fHeight );
    static bool                         SetMinuteDuration                   ( unsigned long ulDelay );
    static bool                         SetGarageOpen                       ( unsigned char ucGarageID, bool bIsOpen );
    static bool                         SetWorldSpecialPropertyEnabled      ( const char* szPropName, bool bEnabled );
    static bool                         SetBlurLevel                        ( unsigned char ucLevel );
    static bool                         SetJetpackMaxHeight                 ( float fHeight );
    static bool                         SetTrafficLightState                ( unsigned char ucState );
    static bool                         SetTrafficLightsLocked              ( bool bLocked );
    static bool                         SetWindVelocity                     ( float fX, float fY, float fZ );
    static bool                         RestoreWindVelocity                 ( void );
    static bool                         GetWindVelocity                     ( float& fX, float& fY, float& fZ );
    static bool                         SetAircraftMaxHeight                ( float fHeight );

    // Input functions
    static bool                         BindKey                             ( const char* szKey, const char* szHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments );
    static bool                         BindKey                             ( const char* szKey, const char* szHitState, const char* szCommandName, const char* szArguments, const char* szResource );
    static bool                         UnbindKey                           ( const char* szKey, CLuaMain* pLuaMain, const char* szHitState = 0, const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef () );
    static bool                         UnbindKey                           ( const char* szKey, const char* szHitState, const char* szCommandName, const char* szResource );
    static bool                         GetKeyState                         ( const char* szKey, bool& bState );
    static bool                         GetControlState                     ( const char* szControl, bool& bState );
    static bool                         GetAnalogControlState               ( const char* szControl, float& fState );
    static bool                         IsControlEnabled                    ( const char* szControl, bool& bEnabled );

    static bool                         SetControlState                     ( const char* szControl, bool bState );
    static bool                         ToggleControl                       ( const char* szControl, bool bEnabled );
    static bool                         ToggleAllControls                   ( bool bGTAControls, bool bMTAControls, bool bEnabled );

    // Projectile functions
    static CClientProjectile *          CreateProjectile                    ( CResource& Resource, CClientEntity& Creator, unsigned char ucWeaponType, CVector& vecOrigin, float fForce, CClientEntity* pTarget, CVector* pvecRotation = NULL, CVector* pvecVelocity = NULL, unsigned short usModel = 0 );

    // Shape create funcs
    static CClientColCircle*            CreateColCircle                     ( CResource& Resource, const CVector& vecPosition, float fRadius );
    static CClientColCuboid*            CreateColCuboid                     ( CResource& Resource, const CVector& vecPosition, const CVector& vecSize );
    static CClientColSphere*            CreateColSphere                     ( CResource& Resource, const CVector& vecPosition, float fRadius );
    static CClientColRectangle*         CreateColRectangle                  ( CResource& Resource, const CVector& vecPosition, const CVector2D& vecSize );
    static CClientColPolygon*           CreateColPolygon                    ( CResource& Resource, const CVector& vecPosition );
    static CClientColTube*              CreateColTube                       ( CResource& Resource, const CVector& vecPosition, float fRadius, float fHeight );
    static CClientColShape*             GetElementColShape                  ( CClientEntity* pEntity );
    static void                         RefreshColShapeColliders            ( CClientColShape *pColShape );

    // Weapon funcs
    static bool                         GetWeaponNameFromID                 ( unsigned char ucID, char* szName, unsigned short len );
    static bool                         GetWeaponIDFromName                 ( const char* szName, unsigned char& ucID );

    // Util funcs
    static bool                         GetTickCount_                       ( double& dCount );

    // Map funcs
    static bool                         IsPlayerMapForced                   ( bool & bForced );
    static bool                         IsPlayerMapVisible                  ( bool & bVisible );
    static bool                         GetPlayerMapBoundingBox             ( CVector &vecMin, CVector &vecMax );

    // Fx funcs
    static bool                         FxAddBlood                          ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness );
    static bool                         FxAddWood                           ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness );
    static bool                         FxAddSparks                         ( CVector & vecPosition, CVector & vecDirection, float fForce, int iCount, CVector vecAcrossLine, bool bBlur, float fSpread, float fLife );
    static bool                         FxAddTyreBurst                      ( CVector & vecPosition, CVector & vecDirection );
    static bool                         FxAddBulletImpact                   ( CVector & vecPosition, CVector & vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity );
    static bool                         FxAddPunchImpact                    ( CVector & vecPosition, CVector & vecDirection );
    static bool                         FxAddDebris                         ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount );
    static bool                         FxAddGlass                          ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount );
    static bool                         FxAddWaterHydrant                   ( CVector & vecPosition );
    static bool                         FxAddGunshot                        ( CVector & vecPosition, CVector & vecDirection, bool bIncludeSparks );
    static bool                         FxAddTankFire                       ( CVector & vecPosition, CVector & vecDirection );
    static bool                         FxAddWaterSplash                    ( CVector & vecPosition );
    static bool                         FxAddBulletSplash                   ( CVector & vecPosition );
    static bool                         FxAddFootSplash                     ( CVector & vecPosition );

    // Sound funcs
    static CClientSound*                PlaySound                           ( CResource* pResource, const SString& strSound, bool bIsURL, bool bLoop );
    static CClientSound*                PlaySound3D                         ( CResource* pResource, const SString& strSound, bool bIsURL, const CVector& vecPosition, bool bLoop );
    static bool                         StopSound                           ( CClientSound& Sound );
    static bool                         SetSoundPosition                    ( CClientSound& Sound, unsigned int uiPosition );
    static bool                         GetSoundPosition                    ( CClientSound& Sound, unsigned int& uiPosition );
    static bool                         GetSoundLength                      ( CClientSound& Sound, unsigned int& uiLength );
    static bool                         SetSoundPaused                      ( CClientSound& Sound, bool bPaused );
    static bool                         IsSoundPaused                       ( CClientSound& Sound, bool& bPaused );
    static bool                         SetSoundVolume                      ( CClientSound& Sound, float fVolume );
    static bool                         GetSoundVolume                      ( CClientSound& Sound, float& fVolume );
    static bool                         SetSoundSpeed                       ( CClientSound& Sound, float fSpeed );
    static bool                         GetSoundSpeed                       ( CClientSound& Sound, float& fSpeed );
    static bool                         SetSoundMinDistance                 ( CClientSound& Sound, float fDistance );
    static bool                         GetSoundMinDistance                 ( CClientSound& Sound, float& fDistance );
    static bool                         SetSoundMaxDistance                 ( CClientSound& Sound, float fDistance );
    static bool                         GetSoundMaxDistance                 ( CClientSound& Sound, float& fDistance );
    static bool                         GetSoundMetaTags                    ( CClientSound& Sound, const SString& strFormat, SString& strMetaTags );
    static bool                         SetSoundEffectEnabled               ( CClientSound& Sound, const SString& strEffectName, bool bEnable );


    // Handling funcs 
    static eHandlingProperty    GetVehicleHandlingEnum              ( std::string strProperty );
    static bool                 GetVehicleHandling                  ( CClientVehicle* pVehicle, eHandlingProperty eProperty, float& fValue );
    static bool                 GetVehicleHandling                  ( CClientVehicle* pVehicle, eHandlingProperty eProperty, CVector& vecValue );
    static bool                 GetVehicleHandling                  ( CClientVehicle* pVehicle, eHandlingProperty eProperty, std::string& strValue );
    static bool                 GetVehicleHandling                  ( CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned int& uiValue );
    static bool                 GetVehicleHandling                  ( CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned char& ucValue );
    static bool                 GetEntryHandling                    ( CHandlingEntry* pEntry, eHandlingProperty eProperty, float& fValue );
    static bool                 GetEntryHandling                    ( CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string& strValue );
    static bool                 GetEntryHandling                    ( CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int& uiValue );
    static bool                 GetEntryHandling                    ( CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char& ucValue );


#ifdef MTA_VOICE
    // Voice funcs
    static bool                         SetVoiceInputEnabled                ( bool bEnabled );
    static bool                         SetVoiceMuteAllEnabled              ( bool bEnabled );
#endif

    // Version funcs
    static unsigned long                GetVersion                          ( );
    static const char*                  GetVersionString                    ( );
    static const char*                  GetVersionName                      ( );
    static SString                      GetVersionBuildType                 ( );
    static unsigned long                GetNetcodeVersion                   ( );
    static const char*                  GetOperatingSystemName              ( );
    static const char*                  GetVersionBuildTag                  ( );
    static SString                      GetVersionSortable                  ( );
};
#endif
