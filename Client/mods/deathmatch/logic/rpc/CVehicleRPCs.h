/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CVehicleRPCs.h
 *  PURPOSE:     Header for vehicle RPC class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CVehicleRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(DestroyAllVehicles);
    DECLARE_ELEMENT_RPC(FixVehicle);
    DECLARE_ELEMENT_RPC(BlowVehicle);
    DECLARE_ELEMENT_RPC(SetVehicleRotation);
    DECLARE_ELEMENT_RPC(SetVehicleTurnSpeed);
    DECLARE_ELEMENT_RPC(SetVehicleColor);
    DECLARE_ELEMENT_RPC(SetVehicleModel);
    DECLARE_ELEMENT_RPC(SetVehicleLocked);
    DECLARE_ELEMENT_RPC(SetVehicleDoorsUndamageable);
    DECLARE_ELEMENT_RPC(SetVehicleSireneOn);
    DECLARE_ELEMENT_RPC(SetVehicleLandingGearDown);
    DECLARE_ELEMENT_RPC(SetHelicopterRotorSpeed);
    DECLARE_ELEMENT_RPC(AddVehicleUpgrade);
    DECLARE_ELEMENT_RPC(AddAllVehicleUpgrades);
    DECLARE_ELEMENT_RPC(RemoveVehicleUpgrade);
    DECLARE_ELEMENT_RPC(SetVehicleDamageState);
    DECLARE_ELEMENT_RPC(SetVehicleOverrideLights);
    DECLARE_ELEMENT_RPC(SetVehicleEngineState);
    DECLARE_ELEMENT_RPC(SetVehicleDirtLevel);
    DECLARE_ELEMENT_RPC(SetVehicleDamageProof);
    DECLARE_ELEMENT_RPC(SetVehiclePaintjob);
    DECLARE_ELEMENT_RPC(SetVehicleFuelTankExplodable);
    DECLARE_ELEMENT_RPC(SetVehicleWheelStates);
    DECLARE_ELEMENT_RPC(SetVehicleFrozen);
    DECLARE_ELEMENT_RPC(SetTrainDerailed);
    DECLARE_ELEMENT_RPC(SetTrainDerailable);
    DECLARE_ELEMENT_RPC(SetTrainDirection);
    DECLARE_ELEMENT_RPC(SetTrainSpeed);
    DECLARE_ELEMENT_RPC(SetTrainTrack);
    DECLARE_ELEMENT_RPC(SetTrainPosition);
    DECLARE_ELEMENT_RPC(SetVehicleTaxiLightOn);
    DECLARE_ELEMENT_RPC(SetVehicleHeadLightColor);
    DECLARE_ELEMENT_RPC(SetVehicleTurretPosition);
    DECLARE_ELEMENT_RPC(SetVehicleDoorOpenRatio);
    DECLARE_ELEMENT_RPC(SetVehicleVariant);
    DECLARE_ELEMENT_RPC(GiveVehicleSirens);
    DECLARE_ELEMENT_RPC(RemoveVehicleSirens);
    DECLARE_ELEMENT_RPC(SetVehicleSirens);
    DECLARE_ELEMENT_RPC(SetVehiclePlateText);
    DECLARE_ELEMENT_RPC(SpawnVehicleFlyingComponent);
};
