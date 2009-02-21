/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CVehicleRPCs.h
*  PURPOSE:     Header for vehicle RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVehicleRPCs_H
#define __CVehicleRPCs_H

#include "CRPCFunctions.h"

class CVehicleRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( DestroyAllVehicles );
    DECLARE_RPC ( FixVehicle );
    DECLARE_RPC ( BlowVehicle );
    DECLARE_RPC ( SetVehicleRotation );
    DECLARE_RPC ( SetVehicleTurnSpeed );
    DECLARE_RPC ( SetVehicleColor );
    DECLARE_RPC ( SetVehicleModel );
    DECLARE_RPC ( SetVehicleLocked );
    DECLARE_RPC ( SetVehicleDoorsUndamageable );
    DECLARE_RPC ( SetVehicleSireneOn );
    DECLARE_RPC ( SetVehicleLandingGearDown );
    DECLARE_RPC ( SetHelicopterRotorSpeed );
	DECLARE_RPC ( AddVehicleUpgrade );
	DECLARE_RPC ( AddAllVehicleUpgrades );
    DECLARE_RPC ( RemoveVehicleUpgrade );
	DECLARE_RPC ( SetVehicleDamageState );
    DECLARE_RPC ( SetVehicleOverrideLights );
    DECLARE_RPC ( SetVehicleEngineState );
    DECLARE_RPC ( SetVehicleDirtLevel );
    DECLARE_RPC ( SetVehicleDamageProof );
    DECLARE_RPC ( SetVehiclePaintjob );
    DECLARE_RPC ( SetVehicleFuelTankExplodable );
    DECLARE_RPC ( SetVehicleWheelStates );
    DECLARE_RPC ( SetVehicleFrozen );
    DECLARE_RPC ( SetTrainDerailed );
};

#endif