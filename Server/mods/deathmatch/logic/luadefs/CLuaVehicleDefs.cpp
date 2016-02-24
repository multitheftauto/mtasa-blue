/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaVehicleDefs.cpp
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaVehicleDefs::LoadFunctions()
{
    // Vehicle create/destroy funcs
    CLuaCFunctions::AddFunction ( "createVehicle", CreateVehicle );

    // Vehicle get funcs
    CLuaCFunctions::AddFunction ( "getVehicleType", GetVehicleType );
    CLuaCFunctions::AddFunction ( "getVehicleVariant", GetVehicleVariant );
    CLuaCFunctions::AddFunction ( "getVehicleColor", GetVehicleColor );
    CLuaCFunctions::AddFunction ( "getVehicleModelFromName", GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleLandingGearDown", GetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "getVehicleName", GetVehicleName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromModel", GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getVehicleOccupant", GetVehicleOccupant );
    CLuaCFunctions::AddFunction ( "getVehicleOccupants", GetVehicleOccupants );
    CLuaCFunctions::AddFunction ( "getVehicleController", GetVehicleController );
    CLuaCFunctions::AddFunction ( "getVehicleRotation", GetVehicleRotation );
    CLuaCFunctions::AddFunction ( "getVehicleSirensOn", GetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "getVehicleTurnVelocity", GetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "getVehicleTurretPosition", GetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "getVehicleMaxPassengers", GetVehicleMaxPassengers );
    CLuaCFunctions::AddFunction ( "isVehicleLocked", IsVehicleLocked );
    CLuaCFunctions::AddFunction ( "getVehiclesOfType", GetVehiclesOfType );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeOnSlot", GetVehicleUpgradeOnSlot );
    CLuaCFunctions::AddFunction ( "getVehicleUpgrades", GetVehicleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeSlotName", GetVehicleUpgradeSlotName );
    CLuaCFunctions::AddFunction ( "getVehicleCompatibleUpgrades", GetVehicleCompatibleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleDoorState", GetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "getVehicleWheelStates", GetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "getVehicleLightState", GetVehicleLightState );
    CLuaCFunctions::AddFunction ( "getVehiclePanelState", GetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "getVehicleOverrideLights", GetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "getVehicleTowedByVehicle", GetVehicleTowedByVehicle );
    CLuaCFunctions::AddFunction ( "getVehicleTowingVehicle", GetVehicleTowingVehicle );
    CLuaCFunctions::AddFunction ( "getVehiclePaintjob", GetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "getVehiclePlateText", GetVehiclePlateText );
    CLuaCFunctions::AddFunction ( "isVehicleDamageProof", IsVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "isVehicleFuelTankExplodable", IsVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "isVehicleFrozen", IsVehicleFrozen );
    CLuaCFunctions::AddFunction ( "isVehicleOnGround", IsVehicleOnGround );
    CLuaCFunctions::AddFunction ( "getVehicleEngineState", GetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "isTrainDerailed", IsTrainDerailed );
    CLuaCFunctions::AddFunction ( "isTrainDerailable", IsTrainDerailable );
    CLuaCFunctions::AddFunction ( "getTrainDirection", GetTrainDirection );
    CLuaCFunctions::AddFunction ( "getTrainSpeed", GetTrainSpeed );
    CLuaCFunctions::AddFunction ( "getTrainTrack", GetTrainTrack );
    CLuaCFunctions::AddFunction ( "getTrainPosition", GetTrainPosition );
    CLuaCFunctions::AddFunction ( "isVehicleBlown", IsVehicleBlown );
    CLuaCFunctions::AddFunction ( "getVehicleHeadLightColor", GetVehicleHeadLightColor );
    CLuaCFunctions::AddFunction ( "getVehicleDoorOpenRatio", GetVehicleDoorOpenRatio );

    // Vehicle set funcs
    CLuaCFunctions::AddFunction ( "fixVehicle", FixVehicle );
    CLuaCFunctions::AddFunction ( "blowVehicle", BlowVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleRotation", SetVehicleRotation );
    CLuaCFunctions::AddFunction ( "setVehicleTurnVelocity", SetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "setVehicleColor", SetVehicleColor );
    CLuaCFunctions::AddFunction ( "setVehicleLandingGearDown", SetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "setVehicleLocked", SetVehicleLocked );
    CLuaCFunctions::AddFunction ( "setVehicleDoorsUndamageable", SetVehicleDoorsUndamageable );
    CLuaCFunctions::AddFunction ( "setVehicleSirensOn", SetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "setVehicleTaxiLightOn", SetVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "isVehicleTaxiLightOn", IsVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "addVehicleUpgrade", AddVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "removeVehicleUpgrade", RemoveVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "setVehicleDoorState", SetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "setVehicleWheelStates", SetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "setVehicleLightState", SetVehicleLightState );
    CLuaCFunctions::AddFunction ( "setVehiclePanelState", SetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "toggleVehicleRespawn", ToggleVehicleRespawn );
    CLuaCFunctions::AddFunction ( "setVehicleRespawnDelay", SetVehicleRespawnDelay );
    CLuaCFunctions::AddFunction ( "setVehicleIdleRespawnDelay", SetVehicleIdleRespawnDelay );
    CLuaCFunctions::AddFunction ( "setVehicleRespawnPosition", SetVehicleRespawnPosition );
    CLuaCFunctions::AddFunction ( "respawnVehicle", RespawnVehicle );
    CLuaCFunctions::AddFunction ( "resetVehicleExplosionTime", ResetVehicleExplosionTime );
    CLuaCFunctions::AddFunction ( "resetVehicleIdleTime", ResetVehicleIdleTime );
    CLuaCFunctions::AddFunction ( "spawnVehicle", SpawnVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleOverrideLights", SetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "attachTrailerToVehicle", AttachTrailerToVehicle );
    CLuaCFunctions::AddFunction ( "detachTrailerFromVehicle", DetachTrailerFromVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleEngineState", SetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "setVehicleDirtLevel", SetVehicleDirtLevel );
    CLuaCFunctions::AddFunction ( "setVehicleDamageProof", SetVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "setVehiclePaintjob", SetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "setVehicleFuelTankExplodable", SetVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "setVehicleFrozen", SetVehicleFrozen );
    CLuaCFunctions::AddFunction ( "setTrainDerailed", SetTrainDerailed );
    CLuaCFunctions::AddFunction ( "setTrainDerailable", SetTrainDerailable );
    CLuaCFunctions::AddFunction ( "setTrainDirection", SetTrainDirection );
    CLuaCFunctions::AddFunction ( "setTrainSpeed", SetTrainSpeed );
    CLuaCFunctions::AddFunction ( "setTrainTrack", SetTrainTrack );
    CLuaCFunctions::AddFunction ( "setTrainPosition", SetTrainPosition );
    CLuaCFunctions::AddFunction ( "setVehicleHeadLightColor", SetVehicleHeadLightColor );
    CLuaCFunctions::AddFunction ( "setVehicleTurretPosition", SetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "setVehicleDoorOpenRatio", SetVehicleDoorOpenRatio );
    CLuaCFunctions::AddFunction ( "setVehicleVariant", SetVehicleVariant );
    CLuaCFunctions::AddFunction ( "addVehicleSirens", GiveVehicleSirens );
    CLuaCFunctions::AddFunction ( "removeVehicleSirens", RemoveVehicleSirens );
    CLuaCFunctions::AddFunction ( "setVehicleSirens", SetVehicleSirens );
    CLuaCFunctions::AddFunction ( "getVehicleSirens", GetVehicleSirens );
    CLuaCFunctions::AddFunction ( "getVehicleSirenParams", GetVehicleSirenParams );
    CLuaCFunctions::AddFunction ( "setVehiclePlateText", SetVehiclePlateText );
}


void CLuaVehicleDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getModelFromName", "getVehicleModelFromName" );
    lua_classfunction ( luaVM, "getNameFromModel", "getVehicleNameFromModel" );
    lua_classfunction ( luaVM, "getAllOfType", "getVehiclesOfType" );
    lua_classfunction ( luaVM, "getModelHandling", "getModelHandling" );
    lua_classfunction ( luaVM, "setModelHandling", "setModelHandling" );
    lua_classfunction ( luaVM, "getOriginalHandling", "getOriginalHandling" );
    lua_classfunction ( luaVM, "getUpgradeSlotName", "getVehicleUpgradeSlotName" );

    lua_classfunction ( luaVM, "create", "createVehicle" );
    lua_classfunction ( luaVM, "blow", "blowVehicle" );
    lua_classfunction ( luaVM, "fix", "fixVehicle" );
    lua_classfunction ( luaVM, "detachTrailer", "detachTrailerFromVehicle" );
    lua_classfunction ( luaVM, "addUpgrade", "addVehicleUpgrade" );
    lua_classfunction ( luaVM, "attachTrailer", "attachTrailerToVehicle" );
    lua_classfunction ( luaVM, "removeUpgrade", "removeVehicleUpgrade" );
    lua_classfunction ( luaVM, "resetIdleTime", "resetVehicleIdleTime" );
    lua_classfunction ( luaVM, "resetExplosionTime", "resetVehicleExplosionTime" );
    lua_classfunction ( luaVM, "respawn", "respawnVehicle" );
    lua_classfunction ( luaVM, "spawn", "spawnVehicle" );
    lua_classfunction ( luaVM, "toggleRespawn", "toggleVehicleRespawn" );
    lua_classfunction ( luaVM, "removeSirens", "removeVehicleSirens" );
    lua_classfunction ( luaVM, "addSirens", "addVehicleSirens" );

    lua_classfunction ( luaVM, "isDamageProof", "isVehicleDamageProof" );
    lua_classfunction ( luaVM, "isFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classfunction ( luaVM, "isLocked", "isVehicleLocked" );
    lua_classfunction ( luaVM, "isOnGround", "isVehicleOnGround" );
    lua_classfunction ( luaVM, "isDerailable", "isTrainDerailable" );
    lua_classfunction ( luaVM, "isDerailed", "isTrainDerailed" );
    lua_classfunction ( luaVM, "isBlown", "isVehicleBlown" );
    lua_classfunction ( luaVM, "isTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classfunction ( luaVM, "getSirenParams", "getVehicleSirenParams" );
    lua_classfunction ( luaVM, "getVariant", "getVehicleVariant" );
    lua_classfunction ( luaVM, "getSirens", "getVehicleSirens" );
    lua_classfunction ( luaVM, "getDirection", "getTrainDirection" );
    lua_classfunction ( luaVM, "getTrainSpeed", "getTrainSpeed" );
    lua_classfunction ( luaVM, "getTrack", "getTrainTrack" );
    lua_classfunction ( luaVM, "getTrainPosition", "getTrainPosition" );
    lua_classfunction ( luaVM, "getHeadLightColor", "getVehicleHeadLightColor" );
    lua_classfunction ( luaVM, "getColor", "getVehicleColor" );
    lua_classfunction ( luaVM, "getCompatibleUpgrades", "getVehicleCompatibleUpgrades" );
    lua_classfunction ( luaVM, "getController", "getVehicleController" );
    lua_classfunction ( luaVM, "getDoorState", "getVehicleDoorState" );
    lua_classfunction ( luaVM, "getEngineState", "getVehicleEngineState" );
    lua_classfunction ( luaVM, "getLandingGearDown", "getVehicleLandingGearDown" );
    lua_classfunction ( luaVM, "getLightState", "getVehicleLightState" );
    lua_classfunction ( luaVM, "getMaxPassenger", "getVehicleMaxPassengers" );
    lua_classfunction ( luaVM, "getName", "getVehicleName" );
    lua_classfunction ( luaVM, "getOccupant", "getVehicleOccupant" );
    lua_classfunction ( luaVM, "getOccupants", "getVehicleOccupants" );
    lua_classfunction ( luaVM, "getOverrideLights", "getVehicleOverrideLights" );
    lua_classfunction ( luaVM, "getPlateText", "getVehiclePlateText" );
    lua_classfunction ( luaVM, "getPaintjob", "getVehiclePaintjob" );
    lua_classfunction ( luaVM, "getPanelState", "getVehiclePanelState" );
    lua_classfunction ( luaVM, "areSirensOn", "getVehicleSirensOn" );
    lua_classfunction ( luaVM, "getTowedByVehicle", "getVehicleTowedByVehicle" );
    lua_classfunction ( luaVM, "getTowingVehicle", "getVehicleTowingVehicle" );
    lua_classfunction ( luaVM, "getTurnVelocity", "getVehicleTurnVelocity", CLuaVehicleDefs::OOP_GetVehicleTurnVelocity );
    lua_classfunction ( luaVM, "getTurretPosition", "getVehicleTurretPosition" );
    lua_classfunction ( luaVM, "getVehicleType", "getVehicleType" ); // This isn't "getType" because it would overwrite Element.getType
    lua_classfunction ( luaVM, "getUpgradeOnSlot", "getVehicleUpgradeOnSlot" );
    lua_classfunction ( luaVM, "getUpgrades", "getVehicleUpgrades" );
    lua_classfunction ( luaVM, "getWheelStates", "getVehicleWheelStates" );
    lua_classfunction ( luaVM, "getDoorOpenRatio", "getVehicleDoorOpenRatio" );
    lua_classfunction ( luaVM, "getHandling", "getVehicleHandling" );

    lua_classfunction ( luaVM, "setColor", "setVehicleColor" );
    lua_classfunction ( luaVM, "setDamageProof", "setVehicleDamageProof" );
    lua_classfunction ( luaVM, "setDoorState", "setVehicleDoorState" );
    lua_classfunction ( luaVM, "setDoorsUndamageable", "setVehicleDoorsUndamageable" );
    lua_classfunction ( luaVM, "setEngineState", "setVehicleEngineState" );
    lua_classfunction ( luaVM, "setFuelTankExplodable", "setVehicleFuelTankExplodable" );
    lua_classfunction ( luaVM, "setIdleRespawnDelay", "setVehicleIdleRespawnDelay" );
    lua_classfunction ( luaVM, "setLandingGearDown", "setVehicleLandingGearDown" );
    lua_classfunction ( luaVM, "setLightState", "setVehicleLightState" );
    lua_classfunction ( luaVM, "setLocked", "setVehicleLocked" );
    lua_classfunction ( luaVM, "setOverrideLights", "setVehicleOverrideLights" );
    lua_classfunction ( luaVM, "setPaintjob", "setVehiclePaintjob" );
    lua_classfunction ( luaVM, "setPanelState", "setVehiclePanelState" );
    lua_classfunction ( luaVM, "setRespawnDelay", "setVehicleRespawnDelay" );
    lua_classfunction ( luaVM, "setRespawnPosition", "setVehicleRespawnPosition" );
    lua_classfunction ( luaVM, "setSirensOn", "setVehicleSirensOn" );
    lua_classfunction ( luaVM, "setTurretPosition", "setVehicleTurretPosition" );
    lua_classfunction ( luaVM, "setDoorOpenRatio", "setVehicleDoorOpenRatio" );
    lua_classfunction ( luaVM, "setHandling", "setVehicleHandling" );
    lua_classfunction ( luaVM, "setTurnVelocity", "setVehicleTurnVelocity" );
    lua_classfunction ( luaVM, "setWheelStates", "setVehicleWheelStates" );
    lua_classfunction ( luaVM, "setHeadLightColor", "setVehicleHeadLightColor" );
    lua_classfunction ( luaVM, "setTaxiLightOn", "setVehicleTaxiLightOn" );
    lua_classfunction ( luaVM, "setVariant", "setVehicleVariant" );
    lua_classfunction ( luaVM, "setSirens", "setVehicleSirens" );
    lua_classfunction ( luaVM, "setPlateText", "setVehiclePlateText" );
    lua_classfunction ( luaVM, "setDerailable", "setTrainDerailable" );
    lua_classfunction ( luaVM, "setDerailed", "setTrainDerailed" );
    lua_classfunction ( luaVM, "setDirection", "setTrainDirection" );
    lua_classfunction ( luaVM, "setTrack", "setTrainTrack" );
    lua_classfunction ( luaVM, "setTrainPosition", "setTrainPosition" );
    lua_classfunction ( luaVM, "setTrainSpeed", "setTrainSpeed" ); // Reduce confusion

    lua_classvariable ( luaVM, "damageProof", "setVehicleDamageProof", "isVehicleDamageProof" );
    lua_classvariable ( luaVM, "locked", "setVehicleLocked", "isVehicleLocked" );
    lua_classvariable ( luaVM, "derailable", "setTrainDerailable", "isTrainDerailable" );
    lua_classvariable ( luaVM, "derailed", "setTrainDerailed", "isTrainDerailed" );
    lua_classvariable ( luaVM, "blown", "blowVehicle", "isVehicleBlown" );
    lua_classvariable ( luaVM, "direction", "setTrainDirection", "getTrainDirection" );
    lua_classvariable ( luaVM, "trainSpeed", "setTrainSpeed", "getTrainSpeed" );
    lua_classvariable ( luaVM, "track", "setTrainTrack", "getTrainTrack" );
    lua_classvariable ( luaVM, "trainPosition", "setTrainPosition", "getTrainPosition" );
    lua_classvariable ( luaVM, "taxiLightOn", "setVehicleTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classvariable ( luaVM, "fuelTankExplodable", "setVehicleFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classvariable ( luaVM, "plateText", "setVehiclePlateText", "getVehiclePlateText" );
    lua_classvariable ( luaVM, "sirensOn", "setVehicleSirensOn", "getVehicleSirensOn" );
    lua_classvariable ( luaVM, "sirenParams", NULL, "getVehicleSirenParams" );
    lua_classvariable ( luaVM, "controller", NULL, "getVehicleController" );
    lua_classvariable ( luaVM, "engineState", "setVehicleEngineState", "getVehicleEngineState" );
    lua_classvariable ( luaVM, "paintjob", "setVehiclePaintjob", "getVehiclePaintjob" );
    lua_classvariable ( luaVM, "occupants", NULL, "getVehicleOccupants" );
    lua_classvariable ( luaVM, "compatibleUpgrades", NULL, "getVehicleCompatibleUpgrades" );
    lua_classvariable ( luaVM, "doorsUndamageable", "setVehicleDoorsUndamageable", NULL );
    lua_classvariable ( luaVM, "towingVehicle", NULL, "getVehicleTowingVehicle" );
    lua_classvariable ( luaVM, "towedByVehicle", NULL, "getVehicleTowedByVehicle" );
    lua_classvariable ( luaVM, "landingGearDown", "setVehicleLandingGearDown", "getVehicleLandingGearDown" );
    lua_classvariable ( luaVM, "maxPassengers", NULL, "getVehicleMaxPassengers" );
    lua_classvariable ( luaVM, "upgrades", NULL, "getVehicleUpgrades" );
    lua_classvariable ( luaVM, "turretPosition", "setVehicleTurretPosition", "getVehicleTurretPosition" );
    lua_classvariable ( luaVM, "turnVelocity", "setVehicleTurnVelocity", "getVehicleTurnVelocity", SetVehicleTurnVelocity, OOP_GetVehicleTurnVelocity );
    lua_classvariable ( luaVM, "overrideLights", "setVehicleOverrideLights", "getVehicleOverrideLights" );
    lua_classvariable ( luaVM, "idleRespawnDelay", "setVehicleIdleRespawnDelay", NULL );
    lua_classvariable ( luaVM, "respawnDelay", "setVehicleRespawnDelay", NULL );
    lua_classvariable ( luaVM, "respawnPosition", "setVehicleRespawnPosition", NULL );
    lua_classvariable ( luaVM, "onGround", NULL, "isVehicleOnGround" );
    lua_classvariable ( luaVM, "name", NULL, "getVehicleName" );
    lua_classvariable ( luaVM, "vehicleType", NULL, "getVehicleType" );
    //lua_classvariable ( luaVM, "upgradeOnSlot", NULL, "getVehicleUpgradeOnSlot", NULL, CLuaOOPDefs::GetVehicleUpgradeOnSlot ); // .upgradeOnSlot[slot]=int
    //lua_classvariable ( luaVM, "doorOpenRatio", "setVehicleDoorOpenRatio", "getVehicleDoorOpenRatio", CLuaOOPDefs::SetVehicleDoorOpenRation, CLuaOOPDefs::GetVehicleDoorOpenRatio ); // .doorOpenRatio[door]=ratio
    //lua_classvariable ( luaVM, "doorState", "setVehicleDoorState", "getVehicleDoorState", CLuaOOPDefs::SetVehicleDoorState, CLuaOOPDefs::GetVehicleDoorState ); // .doorState[id]=state
    //lua_classvariable ( luaVM, "lightState", "setVehicleLightState", "getVehicleLightState", CLuaOOPDefs::SetVehicleLightState, CLuaOOPDefs::GetVehicleLightState ); // .lightState[id]=state
    //lua_classvariable ( luaVM, "panelState", "setVehiclePanelState", "getVehiclePanelState", CLuaOOPDefs::SetVehiclePanelState, CLuaOOPDefs::GetVehiclePanelState ); // .panelState[panel]=state
    //lua_classvariable ( luaVM, "headLightColor", "setVehicleHeadLightColor", "getVehicleHeadLightColor" );
    //lua_classvariable ( luaVM, "color", "setVehicleColor", "getVehicleColor" );
    lua_classvariable ( luaVM, "sirens", NULL, "getVehicleSirens" ); // TODO: support .sirens[point] = {...}
    lua_classvariable ( luaVM, "handling", NULL, "setVehicleHandling" ); // .handling[property]=value
    lua_classvariable ( luaVM, "occupant", NULL, "getVehicleOccupant" ); // Currrently returns driver, support .occupant[seat]=ped

    lua_registerclass ( luaVM, "Vehicle", "Element" );
}


int CLuaVehicleDefs::CreateVehicle ( lua_State* luaVM )
{
    //  vehicle createVehicle ( int model, float x, float y, float z, [float rx, float ry, float rz, string numberplate, bool bDirection, int variant1, int variant2] )
    ushort usModel; CVector vecPosition; CVector vecRotation; SString strNumberPlate; uchar ucVariant; uchar ucVariant2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModel );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, CVector () );
    argStream.ReadString ( strNumberPlate, "" );
    if ( argStream.NextIsBool () )
    {
        bool bDirection;
        argStream.ReadBool ( bDirection );
    }
    argStream.ReadNumber ( ucVariant, 254 );
    argStream.ReadNumber ( ucVariant2, 254 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                //if ( usModel != 570 || m_pResourceManager->GetMinClientRequirement () > "1.3.2-xx" ) // Todo: On merge: Please insert the revision
                {
                    // Create the vehicle and return its handle
                    CVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( pResource, usModel, vecPosition, vecRotation, strNumberPlate, ucVariant, ucVariant2 );
                    if ( pVehicle )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup ();
                        if ( pGroup )
                        {
                            pGroup->Add ( pVehicle );
                        }
                        lua_pushelement ( luaVM, pVehicle );
                        return 1;
                    }
                }
                /*else
                m_pScriptDebugging->LogCustom ( luaVM, "Please set min_mta_version to xxx" ); // Todo*/
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleType ( lua_State* luaVM )
{
    unsigned long ulModel;

    CScriptArgReader argStream ( luaVM );

    if ( !argStream.NextIsNumber () )
    {
        CVehicle* pVehicle;
        argStream.ReadUserData ( pVehicle );

        if ( !argStream.HasErrors () )
        {
            ulModel = pVehicle->GetModel ();
        }
    }
    else
        argStream.ReadNumber ( ulModel );

    if ( !argStream.HasErrors () )
    {
        if ( ulModel >= 400 && ulModel < 610 )
        {
            lua_pushstring ( luaVM, CVehicleNames::GetVehicleTypeName ( ulModel ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleVariant ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucVariant;
    unsigned char ucVariant2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucVariant, 0xFE );
    argStream.ReadNumber ( ucVariant2, 0xFE );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleVariant ( pVehicle, ucVariant, ucVariant2 ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleVariant ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucVariant = -1;
        unsigned char ucVariant2 = -1;
        if ( CStaticFunctionDefinitions::GetVehicleVariant ( pVehicle, ucVariant, ucVariant2 ) )
        {
            lua_pushnumber ( luaVM, ucVariant );
            lua_pushnumber ( luaVM, ucVariant2 );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleColor ( lua_State* luaVM )
{
    //  getVehicleColor ( vehicle theVehicle[, bool bRGB ] )
    CVehicle* pVehicle;
    bool bRGB;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bRGB, false );

    if ( !argStream.HasErrors () )
    {
        CVehicleColor color;
        if ( CStaticFunctionDefinitions::GetVehicleColor ( pVehicle, color ) )
        {

            if ( bRGB )
            {
                for ( uint i = 0; i < 4; i++ )
                {
                    SColor RGBColor = color.GetRGBColor ( i );
                    lua_pushnumber ( luaVM, RGBColor.R );
                    lua_pushnumber ( luaVM, RGBColor.G );
                    lua_pushnumber ( luaVM, RGBColor.B );
                }
                return 12;
            }
            else
            {
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 0 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 1 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 2 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 3 ) );
                return 4;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleModelFromName ( lua_State* luaVM )
{
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        unsigned short usModel;
        if ( CStaticFunctionDefinitions::GetVehicleModelFromName ( strName, usModel ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usModel ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleLandingGearDown ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        if ( CVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
        {
            lua_pushboolean ( luaVM, pVehicle->IsLandingGearDown () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleLocked ( lua_State* luaVM )
{
    CElement* pElement;
    bool bLocked;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bLocked );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLocked ( pElement, bLocked ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleDoorsUndamageable ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDoorsUndamageable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bDoorsUndamageable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorsUndamageable ( pElement, bDoorsUndamageable ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::RemoveVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;

    argStream.ReadUserData ( pVehicle );

    if ( argStream.HasErrors () == false )
    {
        if ( pVehicle )
        {
            if ( CStaticFunctionDefinitions::RemoveVehicleSirens ( pVehicle ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    unsigned char ucSirenID = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSirenID );
    if ( ucSirenID > 0 && ucSirenID < 9 )
    {
        // Array indicies start at 0 so compensate here. This way all code works properly and we get nice 1-8 numbers for API
        ucSirenID--;
        argStream.ReadVector3D ( tSirenInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.R );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.G );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.B );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.A, 255 );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha, 0 );
        if ( argStream.HasErrors () == false )
        {
            if ( pVehicle )
            {
                if ( CStaticFunctionDefinitions::SetVehicleSirens ( pVehicle, ucSirenID, tSirenInfo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleSirenParams ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors () == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Create a new table
            lua_newtable ( luaVM );

            lua_pushstring ( luaVM, "SirenCount" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenCount );
            lua_settable ( luaVM, -3 ); // End of SirenCount Property

            lua_pushstring ( luaVM, "SirenType" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenType );
            lua_settable ( luaVM, -3 ); // End of SirenType Property

            lua_pushstring ( luaVM, "Flags" );
            lua_newtable ( luaVM );

            lua_pushstring ( luaVM, "360" );
            lua_pushboolean ( luaVM, tSirenInfo.m_b360Flag );
            lua_settable ( luaVM, -3 ); // End of 360 Property

            lua_pushstring ( luaVM, "DoLOSCheck" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bDoLOSCheck );
            lua_settable ( luaVM, -3 ); // End of DoLOSCheck Property

            lua_pushstring ( luaVM, "UseRandomiser" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bUseRandomiser );
            lua_settable ( luaVM, -3 ); // End of UseRandomiser Property

            lua_pushstring ( luaVM, "Silent" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bSirenSilent );
            lua_settable ( luaVM, -3 ); // End of Silent Property

            lua_settable ( luaVM, -3 ); // End of Flags table

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors () == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Create a new table
            lua_newtable ( luaVM );

            for ( int i = 0; i < tSirenInfo.m_ucSirenCount; i++ )
            {
                lua_pushnumber ( luaVM, i + 1 );
                lua_newtable ( luaVM );

                SSirenBeaconInfo info = tSirenInfo.m_tSirenInfo[i];

                lua_pushstring ( luaVM, "Min_Alpha" );
                lua_pushnumber ( luaVM, info.m_dwMinSirenAlpha );
                lua_settable ( luaVM, -3 ); // End of Min_Alpha property

                lua_pushstring ( luaVM, "Red" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.R );
                lua_settable ( luaVM, -3 ); // End of Red property

                lua_pushstring ( luaVM, "Green" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.G );
                lua_settable ( luaVM, -3 ); // End of Green property

                lua_pushstring ( luaVM, "Blue" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.B );
                lua_settable ( luaVM, -3 ); // End of Blue property

                lua_pushstring ( luaVM, "Alpha" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.A );
                lua_settable ( luaVM, -3 ); // End of Alpha property

                lua_pushstring ( luaVM, "x" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fX );
                lua_settable ( luaVM, -3 ); // End of X property

                lua_pushstring ( luaVM, "y" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fY );
                lua_settable ( luaVM, -3 ); // End of Y property

                lua_pushstring ( luaVM, "z" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fZ );
                lua_settable ( luaVM, -3 ); // End of Z property

                lua_settable ( luaVM, -3 ); // End of Table
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GiveVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    unsigned char ucSirenType = 0;
    unsigned char ucSirenCount = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSirenCount );
    argStream.ReadNumber ( ucSirenType );
    if ( ucSirenCount > 0 )
    {
        argStream.ReadBool ( tSirenInfo.m_b360Flag, false );
        argStream.ReadBool ( tSirenInfo.m_bDoLOSCheck, true );
        argStream.ReadBool ( tSirenInfo.m_bUseRandomiser, true );
        argStream.ReadBool ( tSirenInfo.m_bSirenSilent, false );
        if ( argStream.HasErrors () == false )
        {
            if ( pVehicle )
            {
                if ( CStaticFunctionDefinitions::GiveVehicleSirens ( pVehicle, ucSirenType, ucSirenCount, tSirenInfo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleMaxPassengers ( lua_State* luaVM )
{
    unsigned int uiModel = 0;

    CScriptArgReader argStream ( luaVM );

    if ( argStream.NextIsUserData () )
    {
        CVehicle * pVehicle;
        argStream.ReadUserData ( pVehicle );

        if ( !argStream.HasErrors () )
            uiModel = pVehicle->GetModel ();
    }
    else
        argStream.ReadNumber ( uiModel );

    if ( !argStream.HasErrors () )
    {
        if ( !CVehicleManager::IsValidModel ( uiModel ) )
        {
            m_pScriptDebugging->LogBadType ( luaVM );
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        unsigned int uiMaxPassengers = CVehicleManager::GetMaxPassengers ( uiModel );

        if ( uiMaxPassengers != 0xFF )
        {
            lua_pushnumber ( luaVM, uiMaxPassengers );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleName ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        SString strVehicleName;
        if ( CStaticFunctionDefinitions::GetVehicleName ( pVehicle, strVehicleName ) )
        {
            lua_pushstring ( luaVM, strVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleNameFromModel ( lua_State* luaVM )
{
    unsigned short usModel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModel );

    if ( !argStream.HasErrors () )
    {
        SString strVehicleName;

        if ( CStaticFunctionDefinitions::GetVehicleNameFromModel ( usModel, strVehicleName ) )
        {
            lua_pushstring ( luaVM, strVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleOccupant ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned int uiSeat;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( uiSeat, 0 );

    if ( !argStream.HasErrors () )
    {
        CPed* pPed = CStaticFunctionDefinitions::GetVehicleOccupant ( pVehicle, uiSeat );
        if ( pPed )
        {
            lua_pushelement ( luaVM, pPed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleOccupants ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Get the maximum amount of passengers
        unsigned char ucMaxPassengers = pVehicle->GetMaxPassengers ();

        // Make sure that if the vehicle doesn't have any seats, the function returns false
        if ( ucMaxPassengers == 255 )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        // Add All Occupants
        for ( unsigned char ucSeat = 0; ucSeat <= ucMaxPassengers; ++ucSeat )
        {
            CPed* pPed = pVehicle->GetOccupant ( ucSeat );
            if ( pPed )
            {
                lua_pushnumber ( luaVM, ucSeat );
                lua_pushelement ( luaVM, pPed );
                lua_settable ( luaVM, -3 );
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleController ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CPed* pPed = CStaticFunctionDefinitions::GetVehicleController ( pVehicle );
        if ( pPed )
        {
            lua_pushelement ( luaVM, pPed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleRotation ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector vecRotation;
        if ( CStaticFunctionDefinitions::GetVehicleRotation ( pVehicle, vecRotation ) )
        {
            lua_pushnumber ( luaVM, vecRotation.fX );
            lua_pushnumber ( luaVM, vecRotation.fY );
            lua_pushnumber ( luaVM, vecRotation.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleSirensOn ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        // Does the vehicle have Sirens?
        if ( CVehicleManager::HasSirens ( pVehicle->GetModel () ) || pVehicle->DoesVehicleHaveSirens () )
        {
            lua_pushboolean ( luaVM, pVehicle->IsSirenActive () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleTurnVelocity ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector vecTurnVelocity;
        if ( CStaticFunctionDefinitions::GetVehicleTurnVelocity ( pVehicle, vecTurnVelocity ) )
        {
            lua_pushnumber ( luaVM, vecTurnVelocity.fX );
            lua_pushnumber ( luaVM, vecTurnVelocity.fY );
            lua_pushnumber ( luaVM, vecTurnVelocity.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::OOP_GetVehicleTurnVelocity ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector vecTurnVelocity;
        if ( CStaticFunctionDefinitions::GetVehicleTurnVelocity ( pVehicle, vecTurnVelocity ) )
        {
            lua_pushvector ( luaVM, vecTurnVelocity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleTurretPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector2D vecPosition;
        if ( CStaticFunctionDefinitions::GetVehicleTurretPosition ( pVehicle, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsVehicleLocked ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bLocked;
        if ( CStaticFunctionDefinitions::IsVehicleLocked ( pVehicle, bLocked ) )
        {
            lua_pushboolean ( luaVM, bLocked );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehiclesOfType ( lua_State* luaVM )
{
    unsigned int uiModel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiModel );

    if ( !argStream.HasErrors () )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all the vehicles with a matching model
        m_pVehicleManager->GetVehiclesOfType ( uiModel, luaVM );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleUpgradeOnSlot ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSlot );

    if ( !argStream.HasErrors () )
    {
        unsigned short usUpgrade;
        if ( CStaticFunctionDefinitions::GetVehicleUpgradeOnSlot ( pVehicle, ucSlot, usUpgrade ) )
        {
            lua_pushnumber ( luaVM, usUpgrade );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleUpgradeSlotName ( lua_State* luaVM )
{
    unsigned short usNumber;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usNumber );

    if ( !argStream.HasErrors () )
    {
        if ( usNumber < 17 )
        {
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned char >( usNumber ), strUpgradeName ) )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
        else if ( usNumber >= 1000 && usNumber <= 1193 )
        {
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( usNumber, strUpgradeName ) )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleUpgrades ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            // Create a new table
            lua_newtable ( luaVM );

            // Add all the upgrades to the table
            const SSlotStates& usSlotStates = pUpgrades->GetSlotStates ();

            unsigned int uiIndex = 0;
            unsigned char ucSlot = 0;
            for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++ )
            {
                if ( usSlotStates[ucSlot] != 0 )
                {
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushnumber ( luaVM, usSlotStates[ucSlot] );
                    lua_settable ( luaVM, -3 );
                }
            }

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleCompatibleUpgrades ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors () )
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            // Create a new table
            lua_newtable ( luaVM );

            unsigned int uiIndex = 0;
            for ( unsigned short usUpgrade = 1000; usUpgrade <= 1193; usUpgrade++ )
            {
                if ( pUpgrades->IsUpgradeCompatible ( usUpgrade ) )
                {
                    if ( ucSlot != 0xFF )
                    {
                        unsigned char ucUpgradeSlot;
                        if ( !pUpgrades->GetSlotFromUpgrade ( usUpgrade, ucUpgradeSlot ) )
                            continue;

                        if ( ucUpgradeSlot != ucSlot )
                            continue;
                    }

                    // Add this one to a list
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushnumber ( luaVM, usUpgrade );
                    lua_settable ( luaVM, -3 );
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleDoorState ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucDoor;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucDoor );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehicleDoorState ( pVehicle, ucDoor, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleWheelStates ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucFrontLeft, ucRearLeft, ucFrontRight, ucRearRight;
        if ( CStaticFunctionDefinitions::GetVehicleWheelStates ( pVehicle, ucFrontLeft, ucRearLeft, ucFrontRight, ucRearRight ) )
        {
            lua_pushnumber ( luaVM, ucFrontLeft );
            lua_pushnumber ( luaVM, ucRearLeft );
            lua_pushnumber ( luaVM, ucFrontRight );
            lua_pushnumber ( luaVM, ucRearRight );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleLightState ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucLight );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehicleLightState ( pVehicle, ucLight, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehiclePanelState ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucPanel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucPanel );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehiclePanelState ( pVehicle, ucPanel, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleOverrideLights ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucLights;
        if ( CStaticFunctionDefinitions::GetVehicleOverrideLights ( pVehicle, ucLights ) )
        {
            lua_pushnumber ( luaVM, ucLights );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleTowedByVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVehicle* pTowedVehicle = pVehicle->GetTowedVehicle ();
        if ( pTowedVehicle )
        {
            lua_pushelement ( luaVM, pTowedVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleTowingVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVehicle* pTowedByVehicle = pVehicle->GetTowedByVehicle ();
        if ( pTowedByVehicle )
        {
            lua_pushelement ( luaVM, pTowedByVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehiclePaintjob ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucPaintjob;
        if ( CStaticFunctionDefinitions::GetVehiclePaintjob ( pVehicle, ucPaintjob ) )
        {
            lua_pushnumber ( luaVM, ucPaintjob );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehiclePlateText ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        char szPlateText[9] = { 0 }; // 8 chars + \0
        if ( CStaticFunctionDefinitions::GetVehiclePlateText ( pVehicle, szPlateText ) )
        {
            lua_pushstring ( luaVM, szPlateText );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsVehicleDamageProof ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDamageProof;
        if ( CStaticFunctionDefinitions::IsVehicleDamageProof ( pVehicle, bDamageProof ) )
        {
            lua_pushboolean ( luaVM, bDamageProof );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsVehicleFuelTankExplodable ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bExplodable;
        if ( CStaticFunctionDefinitions::IsVehicleFuelTankExplodable ( pVehicle, bExplodable ) )
        {
            lua_pushboolean ( luaVM, bExplodable );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsVehicleFrozen ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bFrozen = false;
        if ( CStaticFunctionDefinitions::IsVehicleFrozen ( pVehicle, bFrozen ) )
        {
            lua_pushboolean ( luaVM, bFrozen );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsVehicleOnGround ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bOnGround;
        if ( CStaticFunctionDefinitions::IsVehicleOnGround ( pVehicle, bOnGround ) )
        {
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleEngineState ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::GetVehicleEngineState ( pVehicle, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsTrainDerailed ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDerailed;
        if ( CStaticFunctionDefinitions::IsTrainDerailed ( pVehicle, bDerailed ) )
        {
            lua_pushboolean ( luaVM, bDerailed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsTrainDerailable ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDerailable;
        if ( CStaticFunctionDefinitions::IsTrainDerailable ( pVehicle, bDerailable ) )
        {
            lua_pushboolean ( luaVM, bDerailable );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetTrainDirection ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDirection;
        if ( CStaticFunctionDefinitions::GetTrainDirection ( pVehicle, bDirection ) )
        {
            lua_pushboolean ( luaVM, bDirection );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetTrainSpeed ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        float fSpeed;
        if ( CStaticFunctionDefinitions::GetTrainSpeed ( pVehicle, fSpeed ) )
        {
            lua_pushnumber ( luaVM, fSpeed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetTrainPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        float fPosition;
        if ( CStaticFunctionDefinitions::GetTrainPosition ( pVehicle, fPosition ) )
        {
            lua_pushnumber ( luaVM, fPosition );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetTrainTrack ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        uchar ucTrack;
        if ( CStaticFunctionDefinitions::GetTrainTrack ( pVehicle, ucTrack ) )
        {
            lua_pushnumber ( luaVM, ucTrack );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::FixVehicle ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::FixVehicle ( pElement ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::BlowVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    bool bExplode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bExplode, true );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::BlowVehicle ( pElement, bExplode ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsVehicleBlown ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::IsVehicleBlown ( pVehicle ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleHeadLightColor ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetVehicleHeadLightColor ( pVehicle, color ) )
        {
            lua_pushnumber ( luaVM, color.R );
            lua_pushnumber ( luaVM, color.G );
            lua_pushnumber ( luaVM, color.B );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleRotation ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecRotation );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRotation ( pElement, vecRotation ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleTurnVelocity ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecTurnVelocity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecTurnVelocity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTurnVelocity ( pElement, vecTurnVelocity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleColor ( lua_State* luaVM )
{
    CElement* pElement;
    uchar ucParams[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    int i = 0;
    for ( ; i < 12; ++i )
    {
        if ( argStream.NextCouldBeNumber () )
        {
            argStream.ReadNumber ( ucParams[i] );
        }
        else
            break;
    }

    if ( !argStream.HasErrors () )
    {
        CVehicleColor color;

        if ( i == 4 )
        {
            // 4 args mean palette colours
            color.SetPaletteColors ( ucParams[0], ucParams[1], ucParams[2], ucParams[3] );
        }
        else if ( i % 3 == 0 )
        {
            // 3,6,9 or 12 args mean rgb colours
            color.SetRGBColors ( SColorRGBA ( ucParams[0], ucParams[1], ucParams[2], 0 ),
                SColorRGBA ( ucParams[3], ucParams[4], ucParams[5], 0 ),
                SColorRGBA ( ucParams[6], ucParams[7], ucParams[8], 0 ),
                SColorRGBA ( ucParams[9], ucParams[10], ucParams[11], 0 ) );
        }
        else
            argStream.SetCustomError ( "Incorrect number of color arguments" );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetVehicleColor ( pElement, color ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleLandingGearDown ( lua_State* luaVM )
{
    CElement* pElement;
    bool bLandingGearDown;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bLandingGearDown );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLandingGearDown ( pElement, bLandingGearDown ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::IsVehicleTaxiLightOn ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pVehicle->IsTaxiLightOn () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleTaxiLightOn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bTaxiLightOn;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bTaxiLightOn );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTaxiLightOn ( pElement, bTaxiLightOn ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleSirensOn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bSirensOn;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bSirensOn );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleSirensOn ( pElement, bSirensOn ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::AddVehicleUpgrade ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usUpgrade;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( argStream.NextIsString () )
        {
            SString strUpgrade = "";
            argStream.ReadString ( strUpgrade );
            if ( strUpgrade == "all" )
            {
                lua_pushboolean ( luaVM, CStaticFunctionDefinitions::AddAllVehicleUpgrades ( pElement ) );
                return 1;
            }
            else
                argStream.m_iIndex--;
        }

        argStream.ReadNumber ( usUpgrade );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::AddVehicleUpgrade ( pElement, usUpgrade ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::RemoveVehicleUpgrade ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usUpgrade;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( usUpgrade );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemoveVehicleUpgrade ( pElement, usUpgrade ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleDoorState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucDoor;
    unsigned char ucState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucDoor );
    argStream.ReadNumber ( ucState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorState ( pElement, ucDoor, ucState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleWheelStates ( lua_State* luaVM )
{
    CElement* pElement;
    int iFrontLeft;
    int iRearLeft;
    int iFrontRight;
    int iRearRight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( iFrontLeft );
    argStream.ReadNumber ( iRearLeft, -1 );
    argStream.ReadNumber ( iFrontRight, -1 );
    argStream.ReadNumber ( iRearRight, -1 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleWheelStates ( pElement, iFrontLeft, iRearLeft, iFrontRight, iRearRight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleLightState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLight;
    unsigned char ucState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucLight );
    argStream.ReadNumber ( ucState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLightState ( pElement, ucLight, ucState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehiclePanelState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucPanel;
    unsigned char ucState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucPanel );
    argStream.ReadNumber ( ucState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePanelState ( pElement, ucPanel, ucState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::ToggleVehicleRespawn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bRespawn;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bRespawn );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ToggleVehicleRespawn ( pElement, bRespawn ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleRespawnDelay ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned long ulTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ulTime );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRespawnDelay ( pElement, ulTime ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleRespawnPosition ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecPosition;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, CVector () );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRespawnPosition ( pElement, vecPosition, vecRotation ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleIdleRespawnDelay ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned long ulTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ulTime );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleIdleRespawnDelay ( pElement, ulTime ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::RespawnVehicle ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RespawnVehicle ( pElement ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::ResetVehicleExplosionTime ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ResetVehicleExplosionTime ( pElement ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::ResetVehicleIdleTime ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ResetVehicleIdleTime ( pElement ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SpawnVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecPosition;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, CVector () );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SpawnVehicle ( pElement, vecPosition, vecRotation ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleOverrideLights ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLights;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucLights );

    if ( !argStream.HasErrors () )
    {
        if ( ucLights <= 2 && CStaticFunctionDefinitions::SetVehicleOverrideLights ( pElement, ucLights ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::AttachTrailerToVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    CVehicle* pTrailer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadUserData ( pTrailer );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::AttachTrailerToVehicle ( pVehicle, pTrailer ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::DetachTrailerFromVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    CVehicle* pTrailer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadUserData ( pTrailer, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::DetachTrailerFromVehicle ( pVehicle, pTrailer ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleEngineState ( lua_State* luaVM )
{
    CElement* pElement;
    bool bState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleEngineState ( pElement, bState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleDirtLevel ( lua_State* luaVM )
{
    CElement* pElement;
    float fDirtLevel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( fDirtLevel );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDirtLevel ( pElement, fDirtLevel ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleDamageProof ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDamageProof;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bDamageProof );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDamageProof ( pElement, bDamageProof ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehiclePaintjob ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucPaintjob;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucPaintjob );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePaintjob ( pElement, ucPaintjob ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleFuelTankExplodable ( lua_State* luaVM )
{
    CElement* pElement;
    bool bExplodable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bExplodable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleFuelTankExplodable ( pElement, bExplodable ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleFrozen ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bFrozen;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bFrozen );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleFrozen ( pVehicle, bFrozen ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetTrainDerailed ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDerailed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bDerailed );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainDerailed ( pVehicle, bDerailed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetTrainDerailable ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDerailable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bDerailable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainDerailable ( pVehicle, bDerailable ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetTrainDirection ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bDirection );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainDirection ( pVehicle, bDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetTrainSpeed ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fSpeed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( fSpeed );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainSpeed ( pVehicle, fSpeed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetTrainTrack ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    uchar ucTrack;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucTrack );

    if ( ucTrack > 3 )
        argStream.SetCustomError ( "Invalid track number range (0-3)" );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrack ( pVehicle, ucTrack ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }   
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetTrainPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( fPosition );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainPosition ( pVehicle, fPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleHeadLightColor ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    SColor color;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( color.R );
    argStream.ReadNumber ( color.G );
    argStream.ReadNumber ( color.B );
    color.A = 0xFF;

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleHeadLightColor ( pVehicle, color ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleTurretPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fHorizontal;
    float fVertical;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( fHorizontal );
    argStream.ReadNumber ( fVertical );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTurretPosition ( pVehicle, fHorizontal, fVertical ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucDoor;
    float fRatio;
    unsigned long ulTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucDoor );
    argStream.ReadNumber ( fRatio );
    argStream.ReadNumber ( ulTime, 0 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorOpenRatio ( pElement, ucDoor, fRatio, ulTime ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::GetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucDoor;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucDoor );

    if ( !argStream.HasErrors () )
    {
        float fRatio;

        if ( CStaticFunctionDefinitions::GetVehicleDoorOpenRatio ( pVehicle, ucDoor, fRatio ) )
        {
            lua_pushnumber ( luaVM, fRatio );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaVehicleDefs::SetVehiclePlateText ( lua_State* luaVM )
{
    //  bool setVehiclePlateText ( vehicle theVehicle, string plateText )
    CElement* pElement; SString strText;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strText );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePlateText ( pElement, strText ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
