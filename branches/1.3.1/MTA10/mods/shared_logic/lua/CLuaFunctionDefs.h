/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.h
*  PURPOSE:     Lua function definitions class header
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

class CLuaFunctionDefinitions;

#ifndef __CLUAFUNCTIONDEFINITIONS_H
#define __CLUAFUNCTIONDEFINITIONS_H

#include "LuaCommon.h"
#include "CLuaMain.h"
#include "CLuaTimerManager.h"

class CRegisteredCommands;

#define LUA_DECLARE(x) static int x ( lua_State * luaVM );

class CLuaFunctionDefs
{
public:
    static void Initialize ( class CLuaManager* pLuaManager,
                              class CScriptDebugging* pScriptDebugging,
                              class CClientGame* pClientGame );

    // Reserved functions
    LUA_DECLARE ( DisabledFunction );

    // Resource functions
    LUA_DECLARE ( Call );
    LUA_DECLARE ( GetThisResource );
    LUA_DECLARE ( GetResourceConfig );
    LUA_DECLARE ( GetResourceName );
    LUA_DECLARE ( GetResourceFromName );
    LUA_DECLARE ( GetResourceRootElement );
    LUA_DECLARE ( GetResourceGUIElement );
    LUA_DECLARE ( GetResourceDynamicElementRoot );
    LUA_DECLARE ( GetResourceExportedFunctions );

    // Event functions
    LUA_DECLARE ( AddEvent );
    LUA_DECLARE ( AddEventHandler );
    LUA_DECLARE ( RemoveEventHandler );
    LUA_DECLARE ( TriggerEvent );
    LUA_DECLARE ( TriggerServerEvent );
    LUA_DECLARE ( CancelEvent );
    LUA_DECLARE ( WasEventCancelled );
    LUA_DECLARE ( TriggerLatentServerEvent );
    LUA_DECLARE ( GetLatentEventHandles );
    LUA_DECLARE ( GetLatentEventStatus );
    LUA_DECLARE ( CancelLatentEvent );

    // Sound effects and synth functions
    LUA_DECLARE ( PlaySound );
    LUA_DECLARE ( PlaySound3D );
    LUA_DECLARE ( StopSound );
    LUA_DECLARE ( SetSoundPosition );
    LUA_DECLARE ( GetSoundPosition );
    LUA_DECLARE ( GetSoundLength );
    LUA_DECLARE ( SetSoundPaused );
    LUA_DECLARE ( IsSoundPaused );
    LUA_DECLARE ( SetSoundVolume );
    LUA_DECLARE ( GetSoundVolume );
    LUA_DECLARE ( SetSoundSpeed );
    LUA_DECLARE ( GetSoundSpeed );
    LUA_DECLARE ( SetSoundProperties );
    LUA_DECLARE ( GetSoundProperties );
    LUA_DECLARE ( GetSoundFFTData );
    LUA_DECLARE ( GetSoundWaveData );
    LUA_DECLARE ( SetSoundPanEnabled );
    LUA_DECLARE ( IsSoundPanEnabled );
    LUA_DECLARE ( GetSoundLevelData );
    LUA_DECLARE ( GetSoundBPM );
    LUA_DECLARE ( SetSoundMinDistance );
    LUA_DECLARE ( GetSoundMinDistance );
    LUA_DECLARE ( SetSoundMaxDistance );
    LUA_DECLARE ( GetSoundMaxDistance );
    LUA_DECLARE ( GetSoundMetaTags );
    LUA_DECLARE ( SetSoundEffectEnabled );
    LUA_DECLARE ( GetSoundEffects );

    // Output functions
    LUA_DECLARE ( OutputConsole );
    LUA_DECLARE ( OutputChatBox );
    LUA_DECLARE ( ShowChat );
    LUA_DECLARE ( OutputClientDebugString );
    LUA_DECLARE ( SetClipboard );
    LUA_DECLARE ( GetClipboard );

    // Element get functions
    LUA_DECLARE ( GetRootElement );
    LUA_DECLARE ( IsElement );
    LUA_DECLARE ( GetElementChildren );
    LUA_DECLARE ( GetElementChild );
    LUA_DECLARE ( GetElementChildrenCount );
    LUA_DECLARE ( GetElementID );
    LUA_DECLARE ( GetElementByID );
    LUA_DECLARE ( GetElementByIndex );
    LUA_DECLARE ( GetElementData );
    LUA_DECLARE ( GetElementParent );
    LUA_DECLARE ( GetElementMatrix );
    LUA_DECLARE ( GetElementPosition );
    LUA_DECLARE ( GetElementRotation );
    LUA_DECLARE ( GetElementVelocity );
    LUA_DECLARE ( GetElementType );
    LUA_DECLARE ( GetElementsByType );
    LUA_DECLARE ( GetElementInterior );
    LUA_DECLARE ( IsElementWithinColShape );
    LUA_DECLARE ( IsElementWithinMarker );
    LUA_DECLARE ( GetElementsWithinColShape );
    LUA_DECLARE ( GetElementDimension );
    LUA_DECLARE ( GetElementZoneName );
    LUA_DECLARE ( GetElementBoundingBox );
    LUA_DECLARE ( GetElementRadius );
    LUA_DECLARE ( IsElementAttached );
    LUA_DECLARE ( GetElementAttachedTo );
    LUA_DECLARE ( GetAttachedElements );
    LUA_DECLARE ( GetElementDistanceFromCentreOfMassToBaseOfModel );
    LUA_DECLARE ( IsElementLocal );
    LUA_DECLARE ( GetElementAttachedOffsets );
    LUA_DECLARE ( GetElementAlpha );
    LUA_DECLARE ( IsElementOnScreen );
    LUA_DECLARE ( GetElementHealth );
    LUA_DECLARE ( IsElementStreamedIn );
    LUA_DECLARE ( IsElementStreamable );
    LUA_DECLARE ( GetElementModel );
    LUA_DECLARE ( GetElementColShape );
    LUA_DECLARE ( IsElementInWater );
    LUA_DECLARE ( IsElementSyncer );
    LUA_DECLARE ( IsElementCollidableWith );
    LUA_DECLARE ( IsElementDoubleSided );
    LUA_DECLARE ( GetElementCollisionsEnabled );
    LUA_DECLARE ( IsElementFrozen );
    LUA_DECLARE ( GetLowLodElement );
    LUA_DECLARE ( IsElementLowLod );

    // Element set funcs
    LUA_DECLARE ( CreateElement );
    LUA_DECLARE ( DestroyElement );
    LUA_DECLARE ( SetElementID );
    LUA_DECLARE ( SetElementData );
    LUA_DECLARE ( RemoveElementData );
    LUA_DECLARE ( SetElementMatrix );
    LUA_DECLARE ( SetElementPosition );
    LUA_DECLARE ( SetElementRotation );
    LUA_DECLARE ( SetElementVelocity );
    LUA_DECLARE ( SetElementParent );  
    LUA_DECLARE ( SetElementInterior );
    LUA_DECLARE ( SetElementDimension );
    LUA_DECLARE ( AttachElements );
    LUA_DECLARE ( DetachElements );
    LUA_DECLARE ( SetElementAttachedOffsets );
    LUA_DECLARE ( SetElementCollisionsEnabled );
    LUA_DECLARE ( SetElementAlpha );
    LUA_DECLARE ( SetElementHealth );
    LUA_DECLARE ( SetElementStreamable );
    LUA_DECLARE ( SetElementModel );
    LUA_DECLARE ( SetElementCollidableWith );
    LUA_DECLARE ( SetElementDoubleSided );
    LUA_DECLARE ( SetElementFrozen );
    LUA_DECLARE ( SetLowLodElement );

    // Radio functions
    LUA_DECLARE ( SetRadioChannel );
    LUA_DECLARE ( GetRadioChannel );
    LUA_DECLARE ( GetRadioChannelName );

    // Player get functions
    LUA_DECLARE ( GetLocalPlayer );
    LUA_DECLARE ( GetPlayerName );
    LUA_DECLARE ( GetPlayerFromName );
    LUA_DECLARE ( GetPlayerNametagText );
    LUA_DECLARE ( GetPlayerNametagColor );
    LUA_DECLARE ( IsPlayerNametagShowing );
    LUA_DECLARE ( GetPlayerPing );
    LUA_DECLARE ( GetPlayerTeam );
    LUA_DECLARE ( IsPlayerDead );
    LUA_DECLARE ( GetPlayerMoney );
    LUA_DECLARE ( GetPlayerWantedLevel );
    
    // Player set funcs
    LUA_DECLARE ( ShowPlayerHudComponent );
    LUA_DECLARE ( IsPlayerHudComponentVisible );
    LUA_DECLARE ( SetPlayerMoney );
    LUA_DECLARE ( GivePlayerMoney );
    LUA_DECLARE ( TakePlayerMoney );
    LUA_DECLARE ( SetPlayerNametagText );
    LUA_DECLARE ( SetPlayerNametagColor );
    LUA_DECLARE ( SetPlayerNametagShowing );

    // Ped funcs
    LUA_DECLARE ( CreatePed );

    LUA_DECLARE ( DetonateSatchels );

    LUA_DECLARE ( GetPedVoice );
    LUA_DECLARE ( SetPedVoice );
    LUA_DECLARE ( GetPedTarget );
    LUA_DECLARE ( GetPedTargetStart );
    LUA_DECLARE ( GetPedTargetEnd );
    LUA_DECLARE ( GetPedTargetRange );
    LUA_DECLARE ( GetPedTargetCollision );
    LUA_DECLARE ( GetPedWeaponSlot );
    LUA_DECLARE ( GetPedWeapon );
    LUA_DECLARE ( GetPedAmmoInClip );
    LUA_DECLARE ( GetPedTotalAmmo );
    LUA_DECLARE ( GetPedWeaponMuzzlePosition );
    LUA_DECLARE ( GetPedStat );
    LUA_DECLARE ( GetPedOccupiedVehicle );
    LUA_DECLARE ( GetPedArmor );
    LUA_DECLARE ( IsPedChoking );
    LUA_DECLARE ( IsPedDucked );
    LUA_DECLARE ( IsPedInVehicle );
    LUA_DECLARE ( DoesPedHaveJetPack );
    LUA_DECLARE ( IsPedOnGround );
    LUA_DECLARE ( GetPedTask );
    LUA_DECLARE ( GetPedSimplestTask );
    LUA_DECLARE ( IsPedDoingTask );
    LUA_DECLARE ( GetPedContactElement );
    LUA_DECLARE ( GetPedRotation );
    LUA_DECLARE ( CanPedBeKnockedOffBike );
    LUA_DECLARE ( GetPedBonePosition );
    LUA_DECLARE ( GetPedClothes );
    LUA_DECLARE ( GetPedControlState );
    LUA_DECLARE ( GetPedAnalogControlState );
    LUA_DECLARE ( IsPedSunbathing );
    LUA_DECLARE ( IsPedDoingGangDriveby );
    LUA_DECLARE ( GetPedAnimation );
    LUA_DECLARE ( GetPedMoveState );
    LUA_DECLARE ( GetPedMoveAnim );
    LUA_DECLARE ( IsPedHeadless );
    LUA_DECLARE ( IsPedFrozen );
    LUA_DECLARE ( IsPedFootBloodEnabled );
    LUA_DECLARE ( GetPedCameraRotation );
    LUA_DECLARE ( IsPedOnFire );
    LUA_DECLARE ( GetWeaponProperty );
    LUA_DECLARE ( GetOriginalWeaponProperty );
    LUA_DECLARE ( GetPedOxygenLevel );

    LUA_DECLARE ( SetPedRotation );
    LUA_DECLARE ( SetPedCanBeKnockedOffBike );
    LUA_DECLARE ( SetPedAnimation );
    LUA_DECLARE ( SetPedAnimationProgress );
    LUA_DECLARE ( SetPedMoveAnim );
    LUA_DECLARE ( SetPedWeaponSlot );
    LUA_DECLARE ( AddPedClothes );
    LUA_DECLARE ( RemovePedClothes );
    LUA_DECLARE ( SetPedControlState );
    LUA_DECLARE ( SetPedAnalogControlState );
    LUA_DECLARE ( SetPedSunbathing );
    LUA_DECLARE ( SetPedDoingGangDriveby );
    LUA_DECLARE ( SetPedLookAt );
    LUA_DECLARE ( SetPedHeadless );
    LUA_DECLARE ( SetPedFrozen );
    LUA_DECLARE ( SetPedFootBloodEnabled );
    LUA_DECLARE ( SetPedCameraRotation );
    LUA_DECLARE ( SetPedAimTarget );
    LUA_DECLARE ( SetPedOnFire );
    LUA_DECLARE ( WarpPedIntoVehicle );
    LUA_DECLARE ( RemovePedFromVehicle );
    LUA_DECLARE ( SetPedOxygenLevel );

    // Clothes and body functions
    LUA_DECLARE ( GetBodyPartName );
    LUA_DECLARE ( GetClothesByTypeIndex );
    LUA_DECLARE ( GetTypeIndexFromClothes );
    LUA_DECLARE ( GetClothesTypeName );

    // Vehicle get functions
    LUA_DECLARE ( CreateVehicle );

    LUA_DECLARE ( GetVehicleType );
    LUA_DECLARE ( GetVehicleVariant );
    LUA_DECLARE ( GetVehicleColor );
    LUA_DECLARE ( GetModel );
    LUA_DECLARE ( GetVehicleModelFromName );
    LUA_DECLARE ( GetVehicleLandingGearDown );
    LUA_DECLARE ( GetVehicleMaxPassengers );
    LUA_DECLARE ( GetVehicleOccupant );
    LUA_DECLARE ( GetVehicleOccupants );
    LUA_DECLARE ( GetVehicleController );
    LUA_DECLARE ( GetVehicleRotation );
    LUA_DECLARE ( GetVehicleSirensOn );
    LUA_DECLARE ( GetVehicleTurnVelocity );
    LUA_DECLARE ( GetVehicleTurretPosition );
    LUA_DECLARE ( IsVehicleLocked );
    LUA_DECLARE ( GetVehicleUpgradeOnSlot );
    LUA_DECLARE ( GetVehicleUpgrades );
    LUA_DECLARE ( GetVehicleUpgradeSlotName );
    LUA_DECLARE ( GetVehicleCompatibleUpgrades );
    LUA_DECLARE ( GetVehicleWheelStates );
    LUA_DECLARE ( GetVehicleDoorState );
    LUA_DECLARE ( GetVehicleLightState );
    LUA_DECLARE ( GetVehiclePanelState );
    LUA_DECLARE ( GetVehicleOverrideLights );
    LUA_DECLARE ( GetVehicleTowedByVehicle );
    LUA_DECLARE ( GetVehicleTowingVehicle );
    LUA_DECLARE ( GetVehiclePaintjob );
    LUA_DECLARE ( GetVehiclePlateText );
    LUA_DECLARE ( IsVehicleDamageProof );
    LUA_DECLARE ( IsVehicleFuelTankExplodable );
    LUA_DECLARE ( IsVehicleFrozen );
    LUA_DECLARE ( IsVehicleOnGround );
    LUA_DECLARE ( GetVehicleName );
    LUA_DECLARE ( GetVehicleNameFromModel );
    LUA_DECLARE ( GetVehicleAdjustableProperty );
    LUA_DECLARE ( GetHelicopterRotorSpeed );
    LUA_DECLARE ( GetVehicleEngineState );
    LUA_DECLARE ( IsTrainDerailed );
    LUA_DECLARE ( IsTrainDerailable );
    LUA_DECLARE ( GetTrainDirection );
    LUA_DECLARE ( GetTrainSpeed );
    LUA_DECLARE ( GetVehicleGravity );
    LUA_DECLARE ( IsVehicleBlown );
    LUA_DECLARE ( GetVehicleHeadLightColor );
    LUA_DECLARE ( GetVehicleCurrentGear );
    LUA_DECLARE ( GetVehicleHandling );
    LUA_DECLARE ( GetOriginalHandling )
    LUA_DECLARE ( GetVehicleDoorOpenRatio );
    LUA_DECLARE ( IsVehicleNitroRecharging );
    LUA_DECLARE ( IsVehicleNitroActivated );
    LUA_DECLARE ( GetVehicleNitroCount );
    LUA_DECLARE ( GetVehicleNitroLevel );

    // Vehicle set functions
    LUA_DECLARE ( FixVehicle );
    LUA_DECLARE ( BlowVehicle );
    LUA_DECLARE ( SetVehicleRotation );
    LUA_DECLARE ( SetVehicleTurnVelocity );
    LUA_DECLARE ( SetVehicleColor );
    LUA_DECLARE ( SetVehicleLandingGearDown );
    LUA_DECLARE ( SetVehicleLocked );
    LUA_DECLARE ( SetVehicleModel );
    LUA_DECLARE ( SetVehicleDoorsUndamageable );
    LUA_DECLARE ( SetVehicleSirensOn );
    LUA_DECLARE ( AddVehicleUpgrade );
    LUA_DECLARE ( RemoveVehicleUpgrade );
    LUA_DECLARE ( SetVehicleDoorState );
    LUA_DECLARE ( SetVehicleWheelStates );
    LUA_DECLARE ( SetVehicleLightState );
    LUA_DECLARE ( SetVehiclePanelState );
    LUA_DECLARE ( ToggleVehicleRespawn );
    LUA_DECLARE ( SetVehicleRespawnDelay );
    LUA_DECLARE ( SetVehicleIdleRespawnDelay );
    LUA_DECLARE ( SetVehicleRespawnPosition );
    LUA_DECLARE ( ResetVehicleExplosionTime );
    LUA_DECLARE ( ResetVehicleIdleTime );
    LUA_DECLARE ( RespawnVehicle );
    LUA_DECLARE ( SetVehicleOverrideLights );
    LUA_DECLARE ( SetVehicleTaxiLightOn );
    LUA_DECLARE ( IsVehicleTaxiLightOn );
    LUA_DECLARE ( AttachTrailerToVehicle );
    LUA_DECLARE ( DetachTrailerFromVehicle );
    LUA_DECLARE ( SetVehicleEngineState );
    LUA_DECLARE ( SetVehicleDirtLevel );
    LUA_DECLARE ( SetVehicleDamageProof );
    LUA_DECLARE ( SetVehiclePaintjob );
    LUA_DECLARE ( SetVehicleFuelTankExplodable );
    LUA_DECLARE ( SetVehicleFrozen );
    LUA_DECLARE ( SetVehicleAdjustableProperty );
    LUA_DECLARE ( SetHelicopterRotorSpeed );
    LUA_DECLARE ( SetTrainDerailed );
    LUA_DECLARE ( SetTrainDerailable );
    LUA_DECLARE ( SetTrainDirection );
    LUA_DECLARE ( SetTrainSpeed );
    LUA_DECLARE ( SetVehicleGravity );
    LUA_DECLARE ( SetVehicleHeadLightColor );
    LUA_DECLARE ( SetVehicleTurretPosition );
    LUA_DECLARE ( SetVehicleDoorOpenRatio );
    LUA_DECLARE ( SetVehicleSirens );
    LUA_DECLARE ( GetVehicleSirens );
    LUA_DECLARE ( GetVehicleSirenParams );
    LUA_DECLARE ( SetVehicleNitroActivated );
    LUA_DECLARE ( SetVehicleNitroCount );
    LUA_DECLARE ( SetVehicleNitroLevel );

    // Object create funcs
    LUA_DECLARE ( CreateObject );
    
    // Object get funcs
    LUA_DECLARE ( IsObjectStatic );
    LUA_DECLARE ( GetObjectScale );
    LUA_DECLARE ( IsObjectBreakable );

    // Object set funcs
    LUA_DECLARE ( SetObjectRotation );
    LUA_DECLARE ( MoveObject );
    LUA_DECLARE ( StopObject );
    LUA_DECLARE ( SetObjectScale );
    LUA_DECLARE ( SetObjectStatic );
    LUA_DECLARE ( SetObjectBreakable );

    // Explosion functions
    LUA_DECLARE ( CreateExplosion );

    // Fire functions
    LUA_DECLARE ( CreateFire );

    // Audio funcs
    LUA_DECLARE ( PlayMissionAudio );
    LUA_DECLARE ( PlaySoundFrontEnd );
    LUA_DECLARE ( PreloadMissionAudio );
    LUA_DECLARE ( SetAmbientSoundEnabled );
    LUA_DECLARE ( IsAmbientSoundEnabled );
    LUA_DECLARE ( ResetAmbientSounds );
    LUA_DECLARE ( SetWorldSoundEnabled );
    LUA_DECLARE ( IsWorldSoundEnabled );
    LUA_DECLARE ( ResetWorldSounds );

    // Blip funcs
    LUA_DECLARE ( CreateBlip );
    LUA_DECLARE ( CreateBlipAttachedTo );
    LUA_DECLARE ( GetBlipIcon );
    LUA_DECLARE ( GetBlipSize );
    LUA_DECLARE ( GetBlipColor );
    LUA_DECLARE ( GetBlipOrdering );
    LUA_DECLARE ( GetBlipVisibleDistance );

    LUA_DECLARE ( SetBlipIcon );
    LUA_DECLARE ( SetBlipSize );
    LUA_DECLARE ( SetBlipColor );
    LUA_DECLARE ( SetBlipOrdering );
    LUA_DECLARE ( SetBlipVisibleDistance );

    // Marker funcs
    LUA_DECLARE ( CreateMarker );

    // Marker get functions
    LUA_DECLARE ( GetMarkerCount );
    LUA_DECLARE ( GetMarkerType );
    LUA_DECLARE ( GetMarkerSize );
    LUA_DECLARE ( GetMarkerColor );
    LUA_DECLARE ( GetMarkerTarget );
    LUA_DECLARE ( GetMarkerIcon );

    // Marker set functions
    LUA_DECLARE ( SetMarkerType );
    LUA_DECLARE ( SetMarkerSize );
    LUA_DECLARE ( SetMarkerColor );
    LUA_DECLARE ( SetMarkerTarget );
    LUA_DECLARE ( SetMarkerIcon );

    // Radar-area funcs
    LUA_DECLARE ( CreateRadarArea );
    LUA_DECLARE ( GetRadarAreaColor );
    LUA_DECLARE ( GetRadarAreaSize );
    LUA_DECLARE ( IsRadarAreaFlashing );
    LUA_DECLARE ( SetRadarAreaColor );
    LUA_DECLARE ( SetRadarAreaFlashing );
    LUA_DECLARE ( SetRadarAreaSize );
    LUA_DECLARE ( IsInsideRadarArea );

    // Pickup create/destroy funcs
    LUA_DECLARE ( CreatePickup );

    // Pickup get funcs
    LUA_DECLARE ( GetPickupType );
    LUA_DECLARE ( GetPickupWeapon );
    LUA_DECLARE ( GetPickupAmount );
    LUA_DECLARE ( GetPickupAmmo );

    // Pickup get funcs
    LUA_DECLARE ( SetPickupType );

    // Cam get funcs
    LUA_DECLARE ( GetCameraViewMode );
    LUA_DECLARE ( GetCameraMatrix );
    LUA_DECLARE ( GetCameraTarget );
    LUA_DECLARE ( GetCameraInterior );
    LUA_DECLARE ( GetCameraGoggleEffect );

    // Cam set funcs
    LUA_DECLARE ( SetCameraMatrix );
    LUA_DECLARE ( SetCameraTarget );
    LUA_DECLARE ( SetCameraInterior );
    LUA_DECLARE ( FadeCamera );
    LUA_DECLARE ( SetCameraClip );
    LUA_DECLARE ( SetCameraViewMode );
    LUA_DECLARE ( SetCameraGoggleEffect );

    // Cursor funcs
    LUA_DECLARE ( GetCursorPosition );
    LUA_DECLARE ( SetCursorPosition );
    LUA_DECLARE ( IsCursorShowing );
    
    LUA_DECLARE ( ShowCursor );

    // Engine (RenderWare lower-level) functions
    LUA_DECLARE ( EngineLoadDFF );
    LUA_DECLARE ( EngineLoadTXD );
    LUA_DECLARE ( EngineLoadCOL );
    LUA_DECLARE ( EngineImportTXD );
    LUA_DECLARE ( EngineReplaceCOL );
    LUA_DECLARE ( EngineRestoreCOL );
    LUA_DECLARE ( EngineReplaceModel );
    LUA_DECLARE ( EngineRestoreModel );
    LUA_DECLARE ( EngineReplaceMatchingAtomics );
    LUA_DECLARE ( EngineReplaceWheelAtomics );
    LUA_DECLARE ( EnginePositionAtomic );
    LUA_DECLARE ( EnginePositionSeats );
    LUA_DECLARE ( EngineAddAllAtomics );
    LUA_DECLARE ( EngineReplaceVehiclePart );
    LUA_DECLARE ( EngineGetModelLODDistance );
    LUA_DECLARE ( EngineSetModelLODDistance );
    LUA_DECLARE ( EngineSetAsynchronousLoading );
    LUA_DECLARE ( EngineApplyShaderToWorldTexture );
    LUA_DECLARE ( EngineRemoveShaderFromWorldTexture );
    LUA_DECLARE ( EngineGetModelNameFromID );
    LUA_DECLARE ( EngineGetModelIDFromName );
    LUA_DECLARE ( EngineGetModelTextureNames );
    LUA_DECLARE ( EngineGetVisibleTextureNames );
    LUA_DECLARE ( SetVehicleComponentPosition );
    LUA_DECLARE ( GetVehicleComponentPosition );
    LUA_DECLARE ( SetVehicleComponentRotation );
    LUA_DECLARE ( GetVehicleComponentRotation );
    LUA_DECLARE ( ResetVehicleComponentPosition );
    LUA_DECLARE ( ResetVehicleComponentRotation );
    LUA_DECLARE ( SetVehicleComponentVisible );
    LUA_DECLARE ( GetVehicleComponentVisible );
    LUA_DECLARE ( GetVehicleComponents );

    // Drawing funcs (low-level)
    LUA_DECLARE ( dxDrawLine );
    LUA_DECLARE ( dxDrawLine3D );
    LUA_DECLARE ( dxDrawMaterialLine3D );
    LUA_DECLARE ( dxDrawMaterialSectionLine3D );
    LUA_DECLARE ( dxDrawText );
    LUA_DECLARE ( dxDrawRectangle );
    LUA_DECLARE ( dxDrawImage );
    LUA_DECLARE ( dxDrawImageSection );
    LUA_DECLARE ( dxGetTextWidth );
    LUA_DECLARE ( dxGetFontHeight );
    LUA_DECLARE ( dxCreateFont );
    LUA_DECLARE ( dxCreateTexture );
    LUA_DECLARE ( dxCreateShader );
    LUA_DECLARE ( dxCreateRenderTarget );
    LUA_DECLARE ( dxCreateScreenSource );
    LUA_DECLARE ( dxGetMaterialSize );
    LUA_DECLARE ( dxSetShaderValue );
    LUA_DECLARE ( dxSetShaderTessellation );
    LUA_DECLARE ( dxSetShaderTransform );
    LUA_DECLARE ( dxSetRenderTarget );
    LUA_DECLARE ( dxUpdateScreenSource );
    LUA_DECLARE ( dxGetStatus );
    LUA_DECLARE ( dxSetTestMode );
    LUA_DECLARE ( dxGetTexturePixels );
    LUA_DECLARE ( dxSetTexturePixels );
    LUA_DECLARE ( dxGetPixelsSize );
    LUA_DECLARE ( dxGetPixelsFormat );
    LUA_DECLARE ( dxConvertPixels );
    LUA_DECLARE ( dxGetPixelColor );
    LUA_DECLARE ( dxSetPixelColor );
    LUA_DECLARE ( dxSetBlendMode );
    LUA_DECLARE ( dxGetBlendMode );

    // Util functions to make scripting easier for the end user
    // Some of these are based on standard mIRC script funcs as a lot of people will be used to them
    LUA_DECLARE ( GetTok );
    LUA_DECLARE ( Split );
    LUA_DECLARE ( SetTimer );
    LUA_DECLARE ( KillTimer );
    LUA_DECLARE ( ResetTimer );
    LUA_DECLARE ( GetTimers );
    LUA_DECLARE ( IsTimer );
    LUA_DECLARE ( GetTimerDetails );
    LUA_DECLARE ( GetTickCount_ );
    LUA_DECLARE ( GetCTime );
    LUA_DECLARE ( tocolor );
    LUA_DECLARE ( Reference );
    LUA_DECLARE ( Dereference );
    LUA_DECLARE ( GetColorFromString );
    LUA_DECLARE ( GetValidPedModels );
    LUA_DECLARE ( SetDevelopmentMode );
    LUA_DECLARE ( GetDevelopmentMode );

    // GUI funcs
    LUA_DECLARE ( GUIGetInputEnabled );
    LUA_DECLARE ( GUIGetInputMode );
    LUA_DECLARE ( GUISetInputEnabled );
    LUA_DECLARE ( GUISetInputMode );
    LUA_DECLARE ( GUIIsChatBoxInputActive );
    LUA_DECLARE ( GUIIsConsoleActive );
    LUA_DECLARE ( GUIIsDebugViewActive );
    LUA_DECLARE ( GUIIsMainMenuActive );
    LUA_DECLARE ( GUIIsMTAWindowActive );
    LUA_DECLARE ( GUIIsTransferBoxActive );
    LUA_DECLARE ( GUICreateWindow );
    LUA_DECLARE ( GUICreateLabel );
    LUA_DECLARE ( GUICreateButton );
    LUA_DECLARE ( GUICreateEdit );
    LUA_DECLARE ( GUICreateMemo );
    LUA_DECLARE ( GUICreateGridList );
    LUA_DECLARE ( GUICreateScrollBar );
    LUA_DECLARE ( GUICreateScrollPane );
    LUA_DECLARE ( GUICreateTabPanel );
    LUA_DECLARE ( GUICreateTab );
    LUA_DECLARE ( GUICreateProgressBar );
    LUA_DECLARE ( GUICreateCheckBox );
    LUA_DECLARE ( GUICreateRadioButton );
    LUA_DECLARE ( GUICreateStaticImage );
    LUA_DECLARE ( GUICreateFont );
    LUA_DECLARE ( GUIStaticImageLoadImage );
    LUA_DECLARE ( GUIGetSelectedTab );
    LUA_DECLARE ( GUISetSelectedTab );
    LUA_DECLARE ( GUIDeleteTab );
    LUA_DECLARE ( GUIScrollBarSetScrollPosition );
    LUA_DECLARE ( GUIScrollBarGetScrollPosition );
    LUA_DECLARE ( GUIGridListSetSortingEnabled );
    LUA_DECLARE ( GUIGridListAddColumn );
    LUA_DECLARE ( GUIGridListRemoveColumn );
    LUA_DECLARE ( GUIGridListSetColumnWidth );
    LUA_DECLARE ( GUIGridListSetColumnTitle );
    LUA_DECLARE ( GUIGridListGetColumnTitle );
    LUA_DECLARE ( GUIGridListSetScrollBars );
    LUA_DECLARE ( GUIGridListGetRowCount );
    LUA_DECLARE ( GUIGridListGetColumnCount );
    LUA_DECLARE ( GUIGridListAddRow );
    LUA_DECLARE ( GUIGridListInsertRowAfter );
    LUA_DECLARE ( GUIGridListRemoveRow );
    LUA_DECLARE ( GUIGridListAutoSizeColumn );
    LUA_DECLARE ( GUIGridListClear );
    LUA_DECLARE ( GUIGridListSetItemText );
    LUA_DECLARE ( GUIGridListGetItemText );
    LUA_DECLARE ( GUIGridListSetItemData );
    LUA_DECLARE ( GUIGridListGetItemData );
    LUA_DECLARE ( GUIGridListSetItemColor );
    LUA_DECLARE ( GUIGridListGetItemColor );
    LUA_DECLARE ( GUIGridListSetSelectionMode );
    LUA_DECLARE ( GUIGridListGetSelectedItem );
    LUA_DECLARE ( GUIGridListGetSelectedItems );
    LUA_DECLARE ( GUIGridListGetSelectedCount );
    LUA_DECLARE ( GUIGridListSetSelectedItem );
    LUA_DECLARE ( GUIGridListSetHorizontalScrollPosition );
    LUA_DECLARE ( GUIGridListGetHorizontalScrollPosition );
    LUA_DECLARE ( GUIGridListSetVerticalScrollPosition );
    LUA_DECLARE ( GUIGridListGetVerticalScrollPosition );
    LUA_DECLARE ( GUIScrollPaneSetScrollBars );
    LUA_DECLARE ( GUIScrollPaneSetHorizontalScrollPosition );
    LUA_DECLARE ( GUIScrollPaneGetHorizontalScrollPosition );
    LUA_DECLARE ( GUIScrollPaneSetVerticalScrollPosition );
    LUA_DECLARE ( GUIScrollPaneGetVerticalScrollPosition );
    LUA_DECLARE ( GUIDestroyElement );
    LUA_DECLARE ( GUISetEnabled );
    LUA_DECLARE ( GUISetText );
    LUA_DECLARE ( GUISetFont );
    LUA_DECLARE ( GUISetSize );
    LUA_DECLARE ( GUISetPosition );
    LUA_DECLARE ( GUISetVisible );
    LUA_DECLARE ( GUISetAlpha );
    LUA_DECLARE ( GUISetProperty );
    LUA_DECLARE ( GUIBringToFront );
    LUA_DECLARE ( GUIMoveToBack );
    LUA_DECLARE ( GUICheckBoxSetSelected );
    LUA_DECLARE ( GUIRadioButtonSetSelected );    
    LUA_DECLARE ( GUIGetEnabled );
    LUA_DECLARE ( GUIGetText );
    LUA_DECLARE ( GUIGetFont );
    LUA_DECLARE ( GUIGetSize );
    LUA_DECLARE ( GUIGetPosition );
    LUA_DECLARE ( GUIGetVisible );
    LUA_DECLARE ( GUIGetAlpha );
    LUA_DECLARE ( GUIGetProperty );
    LUA_DECLARE ( GUIGetProperties );
    LUA_DECLARE ( GUICheckBoxGetSelected );
    LUA_DECLARE ( GUIRadioButtonGetSelected );
    LUA_DECLARE ( GUIGetScreenSize );
    LUA_DECLARE ( GUIProgressBarSetProgress );
    LUA_DECLARE ( GUIProgressBarGetProgress );
    LUA_DECLARE ( GUIEditSetReadOnly );
    LUA_DECLARE ( GUIEditSetMasked );
    LUA_DECLARE ( GUIEditSetMaxLength );
    LUA_DECLARE ( GUIEditSetCaratIndex );
    LUA_DECLARE ( GUIMemoSetReadOnly );
    LUA_DECLARE ( GUIMemoSetCaratIndex );
    LUA_DECLARE ( GUIWindowSetMovable );
    LUA_DECLARE ( GUIWindowSetSizable );
    LUA_DECLARE ( GUIWindowGetMovable );
    LUA_DECLARE ( GUIWindowGetSizable );
    LUA_DECLARE ( GUIWindowGetCloseButtonEnabled );
    LUA_DECLARE ( GUIWindowGetTitleBarEnabled );
    LUA_DECLARE ( GUILabelSetColor );
    LUA_DECLARE ( GUILabelGetColor );
    LUA_DECLARE ( GUILabelSetVerticalAlign );
    LUA_DECLARE ( GUILabelSetHorizontalAlign );
    LUA_DECLARE ( GUILabelGetTextExtent );
    LUA_DECLARE ( GUILabelGetFontHeight );
    LUA_DECLARE ( GUIGetChatboxLayout );
    LUA_DECLARE ( GUICreateComboBox );
    LUA_DECLARE ( GUIComboBoxAddItem );
    LUA_DECLARE ( GUIComboBoxRemoveItem );
    LUA_DECLARE ( GUIComboBoxClear );
    LUA_DECLARE ( GUIComboBoxGetSelected );
    LUA_DECLARE ( GUIComboBoxSetSelected );
    LUA_DECLARE ( GUIComboBoxGetItemText );
    LUA_DECLARE ( GUIComboBoxSetItemText );

    // World functions
    LUA_DECLARE ( GetTime_ );
    LUA_DECLARE ( GetGroundPosition );
    LUA_DECLARE ( ProcessLineOfSight );
    LUA_DECLARE ( IsLineOfSightClear );
    LUA_DECLARE ( TestLineAgainstWater );
    LUA_DECLARE ( CreateWater );
    LUA_DECLARE ( GetWaterLevel );
    LUA_DECLARE ( GetWaterVertexPosition );
    LUA_DECLARE ( GetWorldFromScreenPosition );
    LUA_DECLARE ( GetScreenFromWorldPosition  );
    LUA_DECLARE ( GetWeather );
    LUA_DECLARE ( GetZoneName );
    LUA_DECLARE ( GetGravity );
    LUA_DECLARE ( GetGameSpeed );
    LUA_DECLARE ( GetMinuteDuration );
    LUA_DECLARE ( GetWaveHeight );
    LUA_DECLARE ( IsGarageOpen );
    LUA_DECLARE ( GetGaragePosition );
    LUA_DECLARE ( GetGarageSize );
    LUA_DECLARE ( GetGarageBoundingBox );
    LUA_DECLARE ( IsWorldSpecialPropertyEnabled );
    LUA_DECLARE ( GetBlurLevel );
    LUA_DECLARE ( GetTrafficLightState );
    LUA_DECLARE ( AreTrafficLightsLocked );
    LUA_DECLARE ( GetJetpackMaxHeight );
    LUA_DECLARE ( GetAircraftMaxHeight );
    LUA_DECLARE ( GetAircraftMaxVelocity );
    LUA_DECLARE ( GetOcclusionsEnabled );

    LUA_DECLARE ( SetTime );
    LUA_DECLARE ( GetSkyGradient );
    LUA_DECLARE ( SetSkyGradient );
    LUA_DECLARE ( ResetSkyGradient );
    LUA_DECLARE ( GetHeatHaze );
    LUA_DECLARE ( SetHeatHaze );
    LUA_DECLARE ( ResetHeatHaze );
    LUA_DECLARE ( GetWaterColor );
    LUA_DECLARE ( SetWaterColor );
    LUA_DECLARE ( ResetWaterColor );
    LUA_DECLARE ( SetWeather );
    LUA_DECLARE ( SetWeatherBlended );
    LUA_DECLARE ( SetGravity );
    LUA_DECLARE ( SetGameSpeed );
    LUA_DECLARE ( SetMinuteDuration );
    LUA_DECLARE ( SetWaterLevel );
    LUA_DECLARE ( ResetWaterLevel );
    LUA_DECLARE ( IsWaterDrawnLast );
    LUA_DECLARE ( SetWaterDrawnLast );
    LUA_DECLARE ( SetWaterVertexPosition );
    LUA_DECLARE ( SetWaveHeight );
    LUA_DECLARE ( SetGarageOpen );
    LUA_DECLARE ( SetWorldSpecialPropertyEnabled );
    LUA_DECLARE ( SetBlurLevel );
    LUA_DECLARE ( SetJetpackMaxHeight );
    LUA_DECLARE ( SetCloudsEnabled );
    LUA_DECLARE ( GetCloudsEnabled );
    LUA_DECLARE ( SetTrafficLightState );
    LUA_DECLARE ( SetTrafficLightsLocked );
    LUA_DECLARE ( GetWindVelocity );
    LUA_DECLARE ( SetWindVelocity );
    LUA_DECLARE ( ResetWindVelocity );
    LUA_DECLARE ( GetInteriorSoundsEnabled );
    LUA_DECLARE ( SetInteriorSoundsEnabled );
    LUA_DECLARE ( GetRainLevel );
    LUA_DECLARE ( SetRainLevel );
    LUA_DECLARE ( ResetRainLevel );
    LUA_DECLARE ( GetFarClipDistance );
    LUA_DECLARE ( SetFarClipDistance );
    LUA_DECLARE ( ResetFarClipDistance );
    LUA_DECLARE ( GetFogDistance );
    LUA_DECLARE ( SetFogDistance );
    LUA_DECLARE ( ResetFogDistance );
    LUA_DECLARE ( GetSunColor );
    LUA_DECLARE ( SetSunColor );
    LUA_DECLARE ( ResetSunColor );
    LUA_DECLARE ( GetSunSize );
    LUA_DECLARE ( SetSunSize );
    LUA_DECLARE ( ResetSunSize );
    LUA_DECLARE ( RemoveWorldBuilding );
    LUA_DECLARE ( RestoreWorldBuildings );
    LUA_DECLARE ( RestoreWorldBuilding );
    LUA_DECLARE ( SetAircraftMaxHeight );
    LUA_DECLARE ( SetAircraftMaxVelocity );
    LUA_DECLARE ( SetOcclusionsEnabled );
    LUA_DECLARE ( CreateSWATRope );
    LUA_DECLARE ( SetBirdsEnabled );
    LUA_DECLARE ( GetBirdsEnabled );
    LUA_DECLARE ( SetPedTargetingMarkerEnabled );
    LUA_DECLARE ( IsPedTargetingMarkerEnabled );
    LUA_DECLARE ( SetMoonSize );
    LUA_DECLARE ( GetMoonSize );
    LUA_DECLARE ( ResetMoonSize );

    // Input functions
    LUA_DECLARE ( BindKey );
    LUA_DECLARE ( UnbindKey );
    LUA_DECLARE ( GetKeyState );
    LUA_DECLARE ( GetControlState );
    LUA_DECLARE ( GetAnalogControlState );
    LUA_DECLARE ( SetAnalogControlState );
    LUA_DECLARE ( IsControlEnabled );
    LUA_DECLARE ( GetBoundKeys );
    LUA_DECLARE ( GetFunctionsBoundToKey );
    LUA_DECLARE ( GetKeyBoundToFunction );
    LUA_DECLARE ( GetCommandsBoundToKey );
    LUA_DECLARE ( GetKeyBoundToCommand );

    LUA_DECLARE ( SetControlState );
    LUA_DECLARE ( ToggleControl );
    LUA_DECLARE ( ToggleAllControls );

    // XML Functions
    LUA_DECLARE ( XMLNodeFindChild );
    LUA_DECLARE ( XMLNodeGetChildren );
    LUA_DECLARE ( XMLNodeGetValue );
    LUA_DECLARE ( XMLNodeSetValue );
    LUA_DECLARE ( XMLNodeGetAttributes );
    LUA_DECLARE ( XMLNodeGetAttribute );
    LUA_DECLARE ( XMLNodeSetAttribute );
    LUA_DECLARE ( XMLNodeGetParent );
    LUA_DECLARE ( XMLLoadFile );
    LUA_DECLARE ( XMLCreateFile );
    LUA_DECLARE ( XMLUnloadFile );
    LUA_DECLARE ( XMLSaveFile );
    LUA_DECLARE ( XMLCreateChild );
    LUA_DECLARE ( XMLDestroyNode );
    LUA_DECLARE ( XMLCopyFile );
    LUA_DECLARE ( XMLNodeGetName );
    LUA_DECLARE ( XMLNodeSetName );

    // Projectile funcs
    LUA_DECLARE ( CreateProjectile );
    LUA_DECLARE ( GetProjectileType );
	LUA_DECLARE ( GetProjectileTarget );
	LUA_DECLARE ( GetProjectileCreator );
    LUA_DECLARE ( GetProjectileForce );
    LUA_DECLARE ( GetProjectileCounter );
    LUA_DECLARE ( SetProjectileCounter );

    // Shape create funcs
    LUA_DECLARE ( CreateColCircle );
    LUA_DECLARE ( CreateColCuboid );
    LUA_DECLARE ( CreateColSphere );
    LUA_DECLARE ( CreateColRectangle );
    LUA_DECLARE ( CreateColPolygon );
    LUA_DECLARE ( CreateColTube );

    // Team get funcs  
    LUA_DECLARE ( GetTeamFromName );
    LUA_DECLARE ( GetTeamName  );
    LUA_DECLARE ( GetTeamColor );
    LUA_DECLARE ( GetTeamFriendlyFire );
    LUA_DECLARE ( GetPlayersInTeam );
    LUA_DECLARE ( CountPlayersInTeam );

    // Weapon funcs
    LUA_DECLARE ( GetWeaponNameFromID );
    LUA_DECLARE ( GetWeaponIDFromName );
    LUA_DECLARE ( GetSlotFromWeapon );
    LUA_DECLARE ( CreateWeapon );
    LUA_DECLARE ( FireWeapon );
    LUA_DECLARE ( SetWeaponProperty );
    LUA_DECLARE ( SetWeaponState );
    LUA_DECLARE ( GetWeaponState );
    LUA_DECLARE ( SetWeaponTarget );
    LUA_DECLARE ( GetWeaponTarget );
    LUA_DECLARE ( SetWeaponOwner );
    LUA_DECLARE ( GetWeaponOwner );
    LUA_DECLARE ( SetWeaponFlags );
    LUA_DECLARE ( GetWeaponFlags );
    LUA_DECLARE ( SetWeaponFiringRate );
    LUA_DECLARE ( GetWeaponFiringRate );
    LUA_DECLARE ( ResetWeaponFiringRate );
    LUA_DECLARE ( GetWeaponAmmo );
    LUA_DECLARE ( GetWeaponClipAmmo );
    LUA_DECLARE ( SetWeaponAmmo );
    LUA_DECLARE ( SetWeaponClipAmmo );

    // Command funcs
    LUA_DECLARE ( AddCommandHandler );
    LUA_DECLARE ( RemoveCommandHandler );
    LUA_DECLARE ( ExecuteCommandHandler );

    // Community funcs
    LUA_DECLARE ( GetPlayerUserName );
    LUA_DECLARE ( GetPlayerSerial );

    // Utility vector math functions
    LUA_DECLARE ( GetDistanceBetweenPoints2D );
    LUA_DECLARE ( GetDistanceBetweenPoints3D );
    LUA_DECLARE ( GetEasingValue );
    LUA_DECLARE ( InterpolateBetween );

    // Map funcs
    LUA_DECLARE ( IsPlayerMapForced );
    LUA_DECLARE ( IsPlayerMapVisible );
    LUA_DECLARE ( GetPlayerMapBoundingBox );

    // Utility
    LUA_DECLARE ( Md5 );
    LUA_DECLARE ( Sha256 );
    LUA_DECLARE ( GetNetworkUsageData );
    LUA_DECLARE ( GetNetworkStats );
    LUA_DECLARE ( GetPerformanceStats );

    LUA_DECLARE ( GetVersion );

    // JSON funcs
    LUA_DECLARE ( toJSON );
    LUA_DECLARE ( fromJSON );

    // UTF functions
    LUA_DECLARE ( UtfLen );
    LUA_DECLARE ( UtfSeek );
    LUA_DECLARE ( UtfSub );
    LUA_DECLARE ( UtfChar );
    LUA_DECLARE ( UtfCode );

    // Voice functions
    LUA_DECLARE ( IsVoiceEnabled );

private:
    // Static references to objects
    static CLuaManager*                 m_pLuaManager;
    static CScriptDebugging*            m_pScriptDebugging;
    static CClientGame*                 m_pClientGame;
    static CClientManager*              m_pManager;
    static CClientEntity*               m_pRootEntity;
    static CClientGUIManager*           m_pGUIManager;
    static CClientPedManager*           m_pPedManager;
    static CClientPlayerManager*        m_pPlayerManager;
    static CClientRadarMarkerManager*   m_pRadarMarkerManager;
    static CResourceManager*            m_pResourceManager;
    static CClientVehicleManager*       m_pVehicleManager;
    static CClientColManager*           m_pColManager;
    static CClientObjectManager*        m_pObjectManager;
    static CClientTeamManager*          m_pTeamManager;
    static CRenderWare*                 m_pRenderWare;
    static CClientMarkerManager*        m_pMarkerManager;
    static CClientPickupManager*        m_pPickupManager;
    static CClientDFFManager*           m_pDFFManager;
    static CClientColModelManager*      m_pColModelManager;
    static CRegisteredCommands*         m_pRegisteredCommands;
};

#endif
