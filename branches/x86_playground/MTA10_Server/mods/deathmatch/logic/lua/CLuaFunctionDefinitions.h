/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefinitions.h
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaFunctionDefinitions;

#ifndef __CLUAFUNCTIONDEFINITIONS_H
#define __CLUAFUNCTIONDEFINITIONS_H

#include "LuaCommon.h"
#include "CLuaMain.h"
#include "CLuaTimerManager.h"

#define LUA_ERROR() lua_pushboolean ( luaVM, false ); return 0;

class CLuaFunctionDefinitions
{
public:
    static void     SetBlipManager                      ( class CBlipManager* pBlipManager );
    static void     SetLuaManager                       ( class CLuaManager* pLuaManager );
    static void     SetMarkerManager                    ( class CMarkerManager* pMarkerManager );
    static void     SetObjectManager                    ( class CObjectManager* pObjectManager );
    static void     SetPickupManager                    ( class CPickupManager* pPickupManager );
    static void     SetPlayerManager                    ( class CPlayerManager* pPlayerManager );
    static void     SetRegisteredCommands               ( class CRegisteredCommands* pRegisteredCommands );
    static void     SetRootElement                      ( class CElement* pElement );
    static void     SetRadarAreaManager                 ( class CRadarAreaManager* pRadarAreaManager );
    static void     SetScriptDebugging                  ( class CScriptDebugging* pScriptDebugging );
    static void     SetVehicleManager                   ( class CVehicleManager* pVehicleManager );
    static void     SetTeamManager                      ( class CTeamManager* pTeamManager );
    static void     SetAccountManager                   ( class CAccountManager* pAccountManager );
    static void     SetColManager                       ( class CColManager* pColManager );
    static void     SetResourceManager                  ( class CResourceManager* pResourceManager );
    static void     SetACL                              ( class CAccessControlListManager* pACLManager );

    // Reserved functions
    static int      DisabledFunction                    ( lua_State* luaVM );

    //
    static int      CallRemote                          ( lua_State* luaVM );

    // Event functions
    static int      AddEvent                            ( lua_State* luaVM );
    static int      AddEventHandler                     ( lua_State* luaVM );
    static int      RemoveEventHandler                  ( lua_State* luaVM );
    static int      TriggerEvent                        ( lua_State* luaVM );
    static int      TriggerClientEvent                  ( lua_State* luaVM );
    static int      CancelEvent                         ( lua_State* luaVM );
    static int      GetCancelReason                     ( lua_State* luaVM );
    static int      WasEventCancelled                   ( lua_State* luaVM );

    // Player get functions
    static int      GetPlayerCount                      ( lua_State* luaVM );
    static int      GetPedAmmoInClip                    ( lua_State* luaVM );
    static int      GetPedTotalAmmo                     ( lua_State* luaVM );
    static int      SetPlayerAmmo                       ( lua_State* luaVM );
    static int      GetPlayerFromName                   ( lua_State* luaVM );
    static int      GetPlayerMoney                      ( lua_State* luaVM );
    static int      GetPlayerPing                       ( lua_State* luaVM );
    static int      GetRandomPlayer                     ( lua_State* luaVM );
    static int      IsPlayerMuted                       ( lua_State* luaVM );
    static int      GetPlayerTeam                       ( lua_State* luaVM );
    static int      CanPlayerUseFunction                ( lua_State* luaVM );
    static int      GetPlayerWantedLevel                ( lua_State* luaVM );
    static int      GetAlivePlayers                     ( lua_State* luaVM );
    static int      GetDeadPlayers                      ( lua_State* luaVM );
    static int      IsPlayerScoreboardForced            ( lua_State* luaVM );
    static int      IsPlayerMapForced                   ( lua_State* luaVM );
    static int      GetPlayerNametagText                ( lua_State* luaVM );
    static int      GetPlayerNametagColor               ( lua_State* luaVM );
    static int      IsPlayerNametagShowing              ( lua_State* luaVM );
    static int      GetPlayerSerial                     ( lua_State* luaVM );
    static int      GetPlayerCommunityID                ( lua_State* luaVM );
    static int      IsPlayerDev                         ( lua_State* luaVM );
    static int      IsPlayerQA                          ( lua_State* luaVM );
    static int      GetPlayerUserName                   ( lua_State* luaVM );
    static int      GetPlayerBlurLevel                  ( lua_State* luaVM );
    static int      GetPlayerName                       ( lua_State* luaVM );
    static int      GetPlayerIP                         ( lua_State* luaVM );
    static int      GetPlayerAccount                    ( lua_State* luaVM );
    static int      GetPlayerVersion                    ( lua_State* luaVM );

    // Player set functions
    static int      SetPlayerMoney                      ( lua_State* luaVM );
    static int      GivePlayerMoney                     ( lua_State* luaVM );
    static int      TakePlayerMoney                     ( lua_State* luaVM );
    static int      SpawnPlayer                         ( lua_State* luaVM );
    static int      ShowPlayerHudComponent              ( lua_State* luaVM );
    static int      SetPlayerWantedLevel                ( lua_State* luaVM );
    static int      ForcePlayerMap                      ( lua_State* luaVM );
    static int      SetPlayerNametagText                ( lua_State* luaVM );
    static int      SetPlayerNametagColor               ( lua_State* luaVM );
    static int      SetPlayerNametagShowing             ( lua_State* luaVM );
    static int      SetPlayerMuted                      ( lua_State* luaVM );
    static int      SetPlayerBlurLevel                  ( lua_State* luaVM );
    static int      RedirectPlayer                      ( lua_State* luaVM );
    static int      SetPlayerName                       ( lua_State* luaVM );
    static int      DetonateSatchels                    ( lua_State* luaVM );

    // Ped get functions
    static int      CreatePed                           ( lua_State* luaVM );
    static int      GetPedArmor                         ( lua_State* luaVM );    
    static int      GetPedRotation                      ( lua_State* luaVM );    
    static int      IsPedChoking                        ( lua_State* luaVM );
    static int      IsPedDead                           ( lua_State* luaVM );
    static int      IsPedDucked                         ( lua_State* luaVM );    
    static int      GetPedStat                          ( lua_State* luaVM );
    static int      GetPedTarget                        ( lua_State* luaVM );    
    static int      GetPedWeapon                        ( lua_State* luaVM );
    static int      GetPedClothes                       ( lua_State* luaVM );
    static int      DoesPedHaveJetPack                  ( lua_State* luaVM );
    static int      IsPedOnGround                       ( lua_State* luaVM );    
    static int      GetPedFightingStyle                 ( lua_State* luaVM );
    static int      GetPedMoveAnim                      ( lua_State* luaVM );
    static int      GetPedGravity                       ( lua_State* luaVM );    
    static int      GetPedContactElement                ( lua_State* luaVM );
    static int      GetPedWeaponSlot                    ( lua_State* luaVM );
    static int      IsPedDoingGangDriveby               ( lua_State* luaVM );
    static int      IsPedOnFire                         ( lua_State* luaVM );
    static int      IsPedHeadless                       ( lua_State* luaVM );
    static int      IsPedFrozen                         ( lua_State* luaVM );
    static int      GetPedOccupiedVehicle               ( lua_State* luaVM );
    static int      GetPedOccupiedVehicleSeat           ( lua_State* luaVM );
    static int      IsPedInVehicle                      ( lua_State* luaVM );

    // Player set functions
    static int      SetPedArmor                         ( lua_State* luaVM );    
    static int      KillPed                             ( lua_State* luaVM );
    static int      SetPedRotation                      ( lua_State* luaVM );
    static int      SetPedStat                          ( lua_State* luaVM );
    static int      AddPedClothes                       ( lua_State* luaVM );
    static int      RemovePedClothes                    ( lua_State* luaVM );
    static int      GivePedJetPack                      ( lua_State* luaVM );
    static int      RemovePedJetPack                    ( lua_State* luaVM );
    static int      SetPedFightingStyle                 ( lua_State* luaVM );
    static int      SetPedMoveAnim                      ( lua_State* luaVM );
    static int      SetPedGravity                       ( lua_State* luaVM );
    static int      SetPedChoking                       ( lua_State* luaVM );      
    static int      WarpPedIntoVehicle                  ( lua_State* luaVM );
    static int      RemovePedFromVehicle                ( lua_State* luaVM );
    static int      SetPedDoingGangDriveby              ( lua_State* luaVM );
    static int      SetPedAnimation                     ( lua_State* luaVM );
    static int      SetPedAnimationProgress             ( lua_State* luaVM );
    static int      SetPedWeaponSlot                    ( lua_State* luaVM );
    static int      SetPedOnFire                        ( lua_State* luaVM );
    static int      SetPedHeadless                      ( lua_State* luaVM );
    static int      SetPedFrozen                        ( lua_State* luaVM );
    static int      reloadPedWeapon                     ( lua_State* luaVM );

    // Weapon give/take functions
    static int      GiveWeapon                          ( lua_State* luaVM );
    static int      TakeWeapon                          ( lua_State* luaVM );
    static int      TakeAllWeapons                      ( lua_State* luaVM );
    static int      SetWeaponAmmo                       ( lua_State* luaVM );
    static int      GetSlotFromWeapon                   ( lua_State* luaVM );

    // Vehicle create/destroy functions
    static int      CreateVehicle                       ( lua_State* luaVM );

    // Vehicle get functions
    static int      GetVehicleType                      ( lua_State* luaVM );
    static int      GetVehicleColor                     ( lua_State* luaVM );
    static int      GetVehicleModelFromName             ( lua_State* luaVM );
    static int      GetVehicleLandingGearDown           ( lua_State* luaVM );
    static int      GetVehicleMaxPassengers             ( lua_State* luaVM );
    static int      GetVehicleName                      ( lua_State* luaVM );
    static int      GetVehicleNameFromModel             ( lua_State* luaVM );
    static int      GetVehicleOccupant                  ( lua_State* luaVM );
    static int      GetVehicleOccupants                 ( lua_State* luaVM );
    static int      GetVehicleController                ( lua_State* luaVM );
    static int      GetVehicleRotation                  ( lua_State* luaVM );
    static int      GetVehicleSirensOn                  ( lua_State* luaVM );
    static int      GetVehicleTurnVelocity              ( lua_State* luaVM );
    static int      GetVehicleTurretPosition            ( lua_State* luaVM );
    static int      IsVehicleLocked                     ( lua_State* luaVM );
    static int      GetVehiclesOfType                   ( lua_State* luaVM );       
    static int      GetVehicleUpgradeOnSlot             ( lua_State* luaVM );
    static int      GetVehicleUpgrades                  ( lua_State* luaVM );
    static int      GetVehicleUpgradeSlotName           ( lua_State* luaVM );
    static int      GetVehicleCompatibleUpgrades        ( lua_State* luaVM );
    static int      GetVehicleDoorState                 ( lua_State* luaVM );
    static int      GetVehicleWheelStates               ( lua_State* luaVM );
    static int      GetVehicleLightState                ( lua_State* luaVM );
    static int      GetVehiclePanelState                ( lua_State* luaVM );
    static int      GetVehicleOverrideLights            ( lua_State* luaVM );
    static int      GetVehicleTowedByVehicle            ( lua_State* luaVM );
    static int      GetVehicleTowingVehicle             ( lua_State* luaVM );
    static int      GetVehiclePaintjob                  ( lua_State* luaVM );
    static int      GetVehiclePlateText                 ( lua_State* luaVM );
    static int      IsVehicleDamageProof                ( lua_State* luaVM );
    static int      IsVehicleFuelTankExplodable         ( lua_State* luaVM );
    static int      IsVehicleFrozen                     ( lua_State* luaVM );
    static int      IsVehicleOnGround                   ( lua_State* luaVM );
    static int      GetVehicleEngineState               ( lua_State* luaVM );
    static int      IsTrainDerailed                     ( lua_State* luaVM );
    static int      IsTrainDerailable                   ( lua_State* luaVM );
    static int      GetTrainDirection                   ( lua_State* luaVM );
    static int      GetTrainSpeed                       ( lua_State* luaVM );
    static int      IsVehicleBlown                      ( lua_State* luaVM );
    static int      GetVehicleHeadLightColor            ( lua_State* luaVM );
    static int      GetVehicleDoorOpenRatio             ( lua_State* luaVM );

    // Vehicle set functions
    static int      FixVehicle                          ( lua_State* luaVM );
    static int      BlowVehicle                         ( lua_State* luaVM );
    static int      SetVehicleRotation                  ( lua_State* luaVM );
    static int      SetVehicleTurnVelocity              ( lua_State* luaVM );
    static int      SetVehicleColor                     ( lua_State* luaVM );
    static int      SetVehicleLandingGearDown           ( lua_State* luaVM );
    static int      SetVehicleLocked                    ( lua_State* luaVM );
    static int      SetVehicleModel                     ( lua_State* luaVM );
    static int      SetVehicleDoorsUndamageable         ( lua_State* luaVM );
    static int      SetVehicleSirensOn                  ( lua_State* luaVM );
    static int      SetVehicleTaxiLightOn               ( lua_State* luaVM );
    static int      IsVehicleTaxiLightOn                ( lua_State* luaVM );
    static int      AddVehicleUpgrade                   ( lua_State* luaVM );
    static int      RemoveVehicleUpgrade                ( lua_State* luaVM );
    static int      SetVehicleDoorState                 ( lua_State* luaVM );
    static int      SetVehicleWheelStates               ( lua_State* luaVM );
    static int      SetVehicleLightState                ( lua_State* luaVM );
    static int      SetVehiclePanelState                ( lua_State* luaVM );
    static int      SetVehicleIdleRespawnDelay          ( lua_State* luaVM );
    static int      SetVehicleRespawnDelay              ( lua_State* luaVM );
    static int      SetVehicleRespawnPosition           ( lua_State* luaVM );
    static int      ToggleVehicleRespawn                ( lua_State* luaVM );
    static int      ResetVehicleExplosionTime           ( lua_State* luaVM );
    static int      ResetVehicleIdleTime                ( lua_State* luaVM );
    static int      SpawnVehicle                        ( lua_State* luaVM );
    static int      RespawnVehicle                      ( lua_State* luaVM );
    static int      SetVehicleOverrideLights            ( lua_State* luaVM );
    static int      AttachTrailerToVehicle              ( lua_State* luaVM );
    static int      DetachTrailerFromVehicle            ( lua_State* luaVM );
    static int      SetVehicleEngineState               ( lua_State* luaVM );
    static int      SetVehicleDirtLevel                 ( lua_State* luaVM );
    static int      SetVehicleDamageProof               ( lua_State* luaVM );
    static int      SetVehiclePaintjob                  ( lua_State* luaVM );
    static int      SetVehicleFuelTankExplodable        ( lua_State* luaVM );
    static int      SetVehicleFrozen                    ( lua_State* luaVM );
    static int      SetTrainDerailed                    ( lua_State* luaVM );
    static int      SetTrainDerailable                  ( lua_State* luaVM );
    static int      SetTrainDirection                   ( lua_State* luaVM );
    static int      SetTrainSpeed                       ( lua_State* luaVM );
    static int      SetVehicleHeadLightColor            ( lua_State* luaVM );
    static int      SetVehicleTurretPosition            ( lua_State* luaVM );
    static int      SetVehicleDoorOpenRatio             ( lua_State* luaVM );

    // Marker create/destroy functions
    static int      CreateMarker                        ( lua_State* luaVM );

    // Marker get functions
    static int      GetMarkerCount                      ( lua_State* luaVM );
    static int      GetMarkerType                       ( lua_State* luaVM );
    static int      GetMarkerSize                       ( lua_State* luaVM );
    static int      GetMarkerColor                      ( lua_State* luaVM );
    static int      GetMarkerTarget                     ( lua_State* luaVM );
    static int      GetMarkerIcon                       ( lua_State* luaVM );

    // Marker set functions
    static int      SetMarkerType                       ( lua_State* luaVM );
    static int      SetMarkerSize                       ( lua_State* luaVM );
    static int      SetMarkerColor                      ( lua_State* luaVM );
    static int      SetMarkerTarget                     ( lua_State* luaVM );
    static int      SetMarkerIcon                       ( lua_State* luaVM );

    // Blip create/destroy functions
    static int      CreateBlip                          ( lua_State* luaVM );
    static int      CreateBlipAttachedTo                ( lua_State* luaVM );

    // Blip get functions
    static int      GetBlipIcon                         ( lua_State* luaVM );
    static int      GetBlipSize                         ( lua_State* luaVM );
    static int      GetBlipColor                        ( lua_State* luaVM );
    static int      GetBlipOrdering                     ( lua_State* luaVM );
    static int      GetBlipVisibleDistance              ( lua_State* luaVM );

    // Blip set functions
    static int      SetBlipIcon                         ( lua_State* luaVM );
    static int      SetBlipSize                         ( lua_State* luaVM );
    static int      SetBlipColor                        ( lua_State* luaVM );
    static int      SetBlipOrdering                     ( lua_State* luaVM );
    static int      SetBlipVisibleDistance              ( lua_State* luaVM );

    // Object create/destroy functions
    static int      CreateObject                        ( lua_State* luaVM );

    // Object get functions
    static int      GetObjectName                       ( lua_State* luaVM );
    static int      GetObjectRotation                   ( lua_State* luaVM );
    static int      GetObjectScale                      ( lua_State* luaVM );

    // Object set functions
    static int      SetObjectName                       ( lua_State* luaVM );
    static int      SetObjectRotation                   ( lua_State* luaVM );
    static int      SetObjectScale                      ( lua_State* luaVM );
    static int      MoveObject                          ( lua_State* luaVM );
    static int      StopObject                          ( lua_State* luaVM );

    // Radar area create/destroy funcs
    static int      CreateRadarArea                     ( lua_State* luaVM );

    // Radar area get funcs
    static int      GetRadarAreaSize                    ( lua_State* luaVM );
    static int      GetRadarAreaColor                   ( lua_State* luaVM );
    static int      IsRadarAreaFlashing                 ( lua_State* luaVM );
    static int      IsInsideRadarArea                   ( lua_State* luaVM );

    // Radar area set funcs
    static int      SetRadarAreaSize                    ( lua_State* luaVM );
    static int      SetRadarAreaColor                   ( lua_State* luaVM );
    static int      SetRadarAreaFlashing                ( lua_State* luaVM );    

    // Explosion funcs
    static int      CreateExplosion                     ( lua_State* luaVM );

    // Fire funcs
    static int      CreateFire                          ( lua_State* luaVM );

    // Audio funcs
    static int      PlaySoundFrontEnd                   ( lua_State* luaVM );
    static int      PlayMissionAudio                    ( lua_State* luaVM );
    static int      PreloadMissionAudio                 ( lua_State* luaVM );

    // Ped body funcs?
    static int      GetBodyPartName                     ( lua_State* luaVM );
    static int      GetClothesByTypeIndex               ( lua_State* luaVM );
    static int      GetTypeIndexFromClothes             ( lua_State* luaVM );
    static int      GetClothesTypeName                  ( lua_State* luaVM );

    // Key bind funcs
    static int      BindKey                             ( lua_State* luaVM );
    static int      UnbindKey                           ( lua_State* luaVM );
    static int      IsKeyBound                          ( lua_State* luaVM );
    static int      GetFunctionsBoundToKey              ( lua_State* luaVM );
    static int      GetKeyBoundToFunction               ( lua_State* luaVM );
    static int      GetControlState                     ( lua_State* luaVM );
    static int      IsControlEnabled                    ( lua_State* luaVM );
   
    static int      SetControlState                     ( lua_State* luaVM );
    static int      ToggleControl                       ( lua_State* luaVM );
    static int      ToggleAllControls                   ( lua_State* luaVM ); 

    // Shape create funcs
    static int      CreateColCircle                     ( lua_State* luaVM );
    static int      CreateColCuboid                     ( lua_State* luaVM );
    static int      CreateColSphere                     ( lua_State* luaVM );
    static int      CreateColRectangle                  ( lua_State* luaVM );
    static int      CreateColPolygon                    ( lua_State* luaVM );
    static int      CreateColTube                       ( lua_State* luaVM );

    // Team get funcs
    static int      CreateTeam                          ( lua_State* luaVM );    
    static int      GetTeamFromName                     ( lua_State* luaVM );
    static int      GetTeamName                         ( lua_State* luaVM );
    static int      GetTeamColor                        ( lua_State* luaVM );
    static int      GetTeamFriendlyFire                 ( lua_State* luaVM );
    static int      GetPlayersInTeam                    ( lua_State* luaVM );
    static int      CountPlayersInTeam                  ( lua_State* luaVM );

    // Team set funcs
    static int      SetPlayerTeam                       ( lua_State* luaVM );
    static int      SetTeamName                         ( lua_State* luaVM );
    static int      SetTeamColor                        ( lua_State* luaVM );       
    static int      SetTeamFriendlyFire                 ( lua_State* luaVM );

    // Water funcs
    static int      CreateWater                         ( lua_State* luaVM );
    static int      SetWaterLevel                       ( lua_State* luaVM );
    static int      GetWaterVertexPosition              ( lua_State* luaVM );
    static int      SetWaterVertexPosition              ( lua_State* luaVM );
    static int      GetWaterColor                       ( lua_State* luaVM );
    static int      SetWaterColor                       ( lua_State* luaVM );
    static int      ResetWaterColor                     ( lua_State* luaVM );

    // Weapon funcs
    static int      GetWeaponNameFromID                 ( lua_State* luaVM );
    static int      GetWeaponIDFromName                 ( lua_State* luaVM );

    // Console functions
    static int      AddCommandHandler                   ( lua_State* luaVM );
    static int      RemoveCommandHandler                ( lua_State* luaVM );
    static int      ExecuteCommandHandler               ( lua_State* luaVM );

    // JSON functions
    static int      toJSON                              ( lua_State* luaVM );
    static int      fromJSON                            ( lua_State* luaVM );

    // Standard server functions
    static int      GetMaxPlayers                       ( lua_State* luaVM );
    static int      SetMaxPlayers                       ( lua_State* luaVM );
    static int      OutputChatBox                       ( lua_State* luaVM );
    static int      OutputConsole                       ( lua_State* luaVM );
    static int      OutputDebugString                   ( lua_State* luaVM );
    static int      OutputServerLog                     ( lua_State* luaVM );
    static int      GetServerName                       ( lua_State* luaVM );
    static int      GetServerHttpPort                   ( lua_State* luaVM );
    static int      GetServerIP                         ( lua_State* luaVM );
    static int      GetServerPassword                   ( lua_State* luaVM );
    static int      SetServerPassword                   ( lua_State* luaVM );
    static int      GetServerConfigSetting              ( lua_State* luaVM );
    static int      SetServerConfigSetting              ( lua_State* luaVM );

    static int      shutdown                            ( lua_State* luaVM );

    // Utility vector math functions
    static int      GetDistanceBetweenPoints2D          ( lua_State* luaVM );
    static int      GetDistanceBetweenPoints3D          ( lua_State* luaVM );
    static int      GetEasingValue                      ( lua_State* luaVM );
    static int      InterpolateBetween                  ( lua_State* luaVM );

    // Util functions to make scripting easier for the end user
    // Some of these are based on standard mIRC script funcs as a lot of people will be used to them
    static int      GetTickCount_                       ( lua_State* luaVM );
    static int      GetCTime                            ( lua_State* luaVM );
    static int      Split                               ( lua_State* luaVM );
    static int      GetTok                              ( lua_State* luaVM );
    static int      SetTimer                            ( lua_State* luaVM );
    static int      KillTimer                           ( lua_State* luaVM );
    static int      ResetTimer                          ( lua_State* luaVM );
    static int      GetTimers                           ( lua_State* luaVM );
    static int      IsTimer                             ( lua_State* luaVM );
    static int      GetTimerDetails                     ( lua_State* luaVM );
    static int      GetColorFromString                  ( lua_State* luaVM );
    static int      Reference                           ( lua_State* luaVM );
    static int      Dereference                         ( lua_State* luaVM );
    static int      UtfLen                              ( lua_State* luaVM );
    static int      UtfSeek                             ( lua_State* luaVM );
    static int      UtfSub                              ( lua_State* luaVM );
    static int      UtfChar                             ( lua_State* luaVM );
    static int      UtfCode                             ( lua_State* luaVM );
    static int      GetValidPedModels                   ( lua_State* luaVM );

    // Loaded Map Functions
    static int      GetRootElement                      ( lua_State* luaVM );
    static int      LoadMapData                         ( lua_State* luaVM );
    static int      SaveMapData                         ( lua_State* luaVM );

    // Mesh Functions
    static int      CreateMesh                          ( lua_State* luaVM );

    // All-Seeing Eye Functions
    static int      GetGameType                         ( lua_State* luaVM );
    static int      GetMapName                          ( lua_State* luaVM );
    static int      SetGameType                         ( lua_State* luaVM );
    static int      SetMapName                          ( lua_State* luaVM );
    static int      GetRuleValue                        ( lua_State* luaVM );
    static int      SetRuleValue                        ( lua_State* luaVM );
    static int      RemoveRuleValue                     ( lua_State* luaVM );
    static int      GetPlayerAnnounceValue              ( lua_State* luaVM );
    static int      SetPlayerAnnounceValue              ( lua_State* luaVM );
    static int      ResendPlayerModInfo                 ( lua_State* luaVM );

    // Database funcs
    static int      DbConnect                           ( lua_State* luaVM );
    static int      DbExec                              ( lua_State* luaVM );
    static int      DbQuery                             ( lua_State* luaVM );
    static int      DbFree                              ( lua_State* luaVM );
    static int      DbPoll                              ( lua_State* luaVM );

    // Registry funcs
    static int      ExecuteSQLCreateTable               ( lua_State* luaVM );
    static int      ExecuteSQLDropTable                 ( lua_State* luaVM );
    static int      ExecuteSQLDelete                    ( lua_State* luaVM );
    static int      ExecuteSQLInsert                    ( lua_State* luaVM );
    static int      ExecuteSQLSelect                    ( lua_State* luaVM );
    static int      ExecuteSQLUpdate                    ( lua_State* luaVM );
    static int      ExecuteSQLQuery                     ( lua_State* luaVM );
    static int      GetPerformanceStats                 ( lua_State* luaVM );

    // Account get funcs
    static int      GetAccount                          ( lua_State* luaVM );
    static int      GetAccounts                         ( lua_State* luaVM );
    static int      GetAccountName                      ( lua_State* luaVM );
    static int      GetAccountPlayer                    ( lua_State* luaVM );
    static int      IsGuestAccount                      ( lua_State* luaVM );
    static int      GetAccountData                      ( lua_State* luaVM );

    // Account set funcs
    static int      AddAccount                          ( lua_State* luaVM );
    static int      RemoveAccount                       ( lua_State* luaVM );
    static int      SetAccountPassword                  ( lua_State* luaVM );
    static int      SetAccountData                      ( lua_State* luaVM );
    static int      CopyAccountData                     ( lua_State* luaVM );

    // Log in/out
    static int      LogIn                               ( lua_State* luaVM );
    static int      LogOut                              ( lua_State* luaVM );

    // Admin funcs
    static int      KickPlayer                          ( lua_State* luaVM );
    static int      BanPlayer                           ( lua_State* luaVM );

    static int      AddBan                              ( lua_State* luaVM );
    static int      RemoveBan                           ( lua_State* luaVM );

    static int      GetBans                             ( lua_State* luaVM );

    static int      GetBanIP                            ( lua_State* luaVM );
    static int      GetBanSerial                        ( lua_State* luaVM );
    static int      GetBanUsername                      ( lua_State* luaVM );
    static int      GetBanNick                          ( lua_State* luaVM );
    static int      GetBanTime                          ( lua_State* luaVM );
    static int      GetUnbanTime                        ( lua_State* luaVM );
    static int      GetBanReason                        ( lua_State* luaVM );
    static int      GetBanAdmin                         ( lua_State* luaVM );

    // Cursor get funcs
    static int      IsCursorShowing                     ( lua_State* luaVM );

    // Cursor set funcs
    static int      ShowCursor                          ( lua_State* luaVM );

    // Chat funcs
    static int      ShowChat                            ( lua_State* luaVM );

    // Misc funcs
    static int      ResetMapInfo                        ( lua_State* luaVM );
    static int      GetServerPort                       ( lua_State* luaVM );

    // Settings registry funcs
    static int      Get                                 ( lua_State* luaVM );
    static int      Set                                 ( lua_State* luaVM );

    // Utility
    static int      Md5                                 ( lua_State* luaVM );
    static int      GetNetworkUsageData                 ( lua_State* luaVM );
    static int      GetNetworkStats                     ( lua_State* luaVM );
    static int      GetVersion                          ( lua_State* luaVM );
    static int      GetModules                          ( lua_State* luaVM );
    static int      GetModuleInfo                       ( lua_State* luaVM );
};

#endif
