/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaPedDefs.h
*  PURPOSE:     Lua ped definitions class header
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPedDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    LUA_DECLARE ( CreatePed );
    LUA_DECLARE ( KillPed );
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
    LUA_DECLARE ( GetPedOccupiedVehicleSeat );
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
    LUA_DECLARE ( IsPedDead );

    LUA_DECLARE ( SetPedRotation );
    LUA_DECLARE ( SetPedCanBeKnockedOffBike );
    LUA_DECLARE ( SetPedAnimation );
    LUA_DECLARE ( SetPedAnimationProgress );
    LUA_DECLARE ( SetPedMoveAnim );
    LUA_DECLARE ( SetPedWeaponSlot );
    LUA_DECLARE ( GivePedWeapon );
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
    LUA_DECLARE_OOP ( WarpPedIntoVehicle );
    LUA_DECLARE ( RemovePedFromVehicle );
    LUA_DECLARE ( SetPedOxygenLevel );
};