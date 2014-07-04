/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CHandlingRPCs.cpp
*  PURPOSE:     Handling remote procedure calls
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CHandlingRPCs.h"
#include "net/SyncStructures.h"

void CHandlingRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_VEHICLE_HANDLING, SetVehicleHandling, "SetVehicleHandling" );
    AddHandler ( SET_VEHICLE_HANDLING_PROPERTY, SetVehicleHandlingProperty, "SetVehicleHandlingProperty" );
    AddHandler ( RESET_VEHICLE_HANDLING_PROPERTY, RestoreVehicleHandlingProperty, "RestoreVehicleHandlingProperty" );
    AddHandler ( RESET_VEHICLE_HANDLING, RestoreVehicleHandling, "RestoreVehicleHandling" );
}

void CHandlingRPCs::SetVehicleHandling ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Check it's type
    if ( pSource && pSource->GetType () == CCLIENTVEHICLE )
    {
        // Grab the vehicle handling entry
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( *pSource );
        CHandlingEntry* pEntry = Vehicle.GetHandlingData();

        SVehicleHandlingSync handling;
        bitStream.Read ( &handling );
        pEntry->SetMass ( handling.data.fMass );
        pEntry->SetTurnMass ( handling.data.fTurnMass );
        pEntry->SetDragCoeff ( handling.data.fDragCoeff );
        pEntry->SetCenterOfMass ( handling.data.vecCenterOfMass );
        pEntry->SetPercentSubmerged ( handling.data.ucPercentSubmerged );
        pEntry->SetTractionMultiplier ( handling.data.fTractionMultiplier );
        pEntry->SetCarDriveType ( (CHandlingEntry::eDriveType)handling.data.ucDriveType );
        pEntry->SetCarEngineType ( (CHandlingEntry::eEngineType)handling.data.ucEngineType );
        pEntry->SetNumberOfGears ( handling.data.ucNumberOfGears );
        pEntry->SetEngineAcceleration ( handling.data.fEngineAcceleration );
        pEntry->SetEngineInertia ( handling.data.fEngineInertia );
        pEntry->SetMaxVelocity ( handling.data.fMaxVelocity );
        pEntry->SetBrakeDeceleration ( handling.data.fBrakeDeceleration );
        pEntry->SetBrakeBias ( handling.data.fBrakeBias );
        pEntry->SetABS ( handling.data.bABS );
        pEntry->SetSteeringLock ( handling.data.fSteeringLock );
        pEntry->SetTractionLoss ( handling.data.fTractionLoss );
        pEntry->SetTractionBias ( handling.data.fTractionBias );
        pEntry->SetSuspensionForceLevel ( handling.data.fSuspensionForceLevel );
        pEntry->SetSuspensionDamping ( handling.data.fSuspensionDamping );
        pEntry->SetSuspensionHighSpeedDamping ( handling.data.fSuspensionHighSpdDamping );
        pEntry->SetSuspensionUpperLimit ( handling.data.fSuspensionUpperLimit );
        pEntry->SetSuspensionLowerLimit ( handling.data.fSuspensionLowerLimit );
        pEntry->SetSuspensionFrontRearBias ( handling.data.fSuspensionFrontRearBias );
        pEntry->SetSuspensionAntiDiveMultiplier ( handling.data.fSuspensionAntiDiveMultiplier );
        pEntry->SetCollisionDamageMultiplier ( handling.data.fCollisionDamageMultiplier );
        pEntry->SetModelFlags ( handling.data.uiModelFlags );
        pEntry->SetHandlingFlags ( handling.data.uiHandlingFlags );
        pEntry->SetSeatOffsetDistance ( handling.data.fSeatOffsetDistance );
        //pEntry->SetMonetary ( handling.data.uiMonetary );
        //pEntry->SetHeadLight ( (CHandlingEntry::eLightType)handling.data.ucHeadLight );
        //pEntry->SetTailLight ( (CHandlingEntry::eLightType)handling.data.ucTailLight );
        //pEntry->SetAnimGroup ( handling.data.ucAnimGroup );
        
        Vehicle.ApplyHandling();
    }
}


void CHandlingRPCs::SetVehicleHandlingProperty ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the property id
    unsigned char ucProperty;
    if ( bitStream.Read ( ucProperty ) )
    {
        // Check its type
        if ( pSource && pSource->GetType () == CCLIENTVEHICLE )
        {
            // Grab the vehicle handling entry
            CClientVehicle& vehicle = static_cast < CClientVehicle& > ( *pSource );
            CHandlingEntry* pHandlingEntry = vehicle.GetHandlingData();

            // Temporary storage for reading out data
            union
            {
                unsigned char ucChar;
                unsigned int uiInt;
                float fFloat;
            };

            // Depending on what property
            switch ( ucProperty )
            {
                case HANDLING_MASS:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetMass ( fFloat );
                    break;
                    
                case HANDLING_TURNMASS:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetTurnMass ( fFloat );
                    break;

                case HANDLING_DRAGCOEFF:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetDragCoeff ( fFloat );
                    break;

                case HANDLING_CENTEROFMASS:
                {
                    CVector vecVector;
                    bitStream.Read ( vecVector.fX );
                    bitStream.Read ( vecVector.fY );
                    bitStream.Read ( vecVector.fZ );
                    pHandlingEntry->SetCenterOfMass ( vecVector );
                    break;
                }

                case HANDLING_PERCENTSUBMERGED:
                    bitStream.Read ( uiInt );
                    pHandlingEntry->SetPercentSubmerged ( uiInt );
                    break;

                case HANDLING_TRACTIONMULTIPLIER:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetTractionMultiplier ( fFloat );
                    break;

                case HANDLING_DRIVETYPE:
                {
                    bitStream.Read ( ucChar );
                    if ( ucChar != CHandlingEntry::FOURWHEEL &&
                         ucChar != CHandlingEntry::RWD &&
                         ucChar != CHandlingEntry::FWD )
                    {
                        ucChar = CHandlingEntry::RWD;
                    }

                    pHandlingEntry->SetCarDriveType ( static_cast < CHandlingEntry::eDriveType > ( ucChar ) );
                    break;
                }

                case HANDLING_ENGINETYPE:
                {
                    bitStream.Read ( ucChar );
                    if ( ucChar != CHandlingEntry::DIESEL &&
                         ucChar != CHandlingEntry::ELECTRIC &&
                         ucChar != CHandlingEntry::PETROL )
                    {
                        ucChar = CHandlingEntry::PETROL;
                    }

                    pHandlingEntry->SetCarEngineType ( static_cast < CHandlingEntry::eEngineType > ( ucChar ) );
                    break;
                }

                case HANDLING_NUMOFGEARS:
                    bitStream.Read ( ucChar );
                    pHandlingEntry->SetNumberOfGears ( ucChar );
                    break;

                case HANDLING_ENGINEACCELERATION:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetEngineAcceleration ( fFloat );
                    break;

                case HANDLING_ENGINEINERTIA:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetEngineInertia ( fFloat );
                    break;

                case HANDLING_MAXVELOCITY:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetMaxVelocity ( fFloat );
                    break;

                case HANDLING_BRAKEDECELERATION:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetBrakeDeceleration ( fFloat );
                    break;

                case HANDLING_BRAKEBIAS:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetBrakeBias ( fFloat );
                    break;

                case HANDLING_ABS:
                    bitStream.Read ( ucChar );
                    pHandlingEntry->SetABS ( ucChar != 0 );
                    break;

                case HANDLING_STEERINGLOCK:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSteeringLock ( fFloat );
                    break;

                case HANDLING_TRACTIONLOSS:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetTractionLoss ( fFloat );
                    break;

                case HANDLING_TRACTIONBIAS:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetTractionBias ( fFloat );
                    break;

                case HANDLING_SUSPENSION_FORCELEVEL:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSuspensionForceLevel ( fFloat );
                    break;

                case HANDLING_SUSPENSION_DAMPING:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSuspensionDamping ( fFloat );
                    break;

                case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSuspensionHighSpeedDamping ( fFloat );
                    break;

                case HANDLING_SUSPENSION_UPPER_LIMIT:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSuspensionUpperLimit ( fFloat );
                    break;

                case HANDLING_SUSPENSION_LOWER_LIMIT:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSuspensionLowerLimit ( fFloat );
                    break;

                case HANDLING_SUSPENSION_FRONTREARBIAS:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSuspensionFrontRearBias ( fFloat );
                    break;

                case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSuspensionAntiDiveMultiplier ( fFloat );
                    break;

                case HANDLING_COLLISIONDAMAGEMULTIPLIER:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetCollisionDamageMultiplier ( fFloat );
                    break;

                case HANDLING_SEATOFFSETDISTANCE:
                    bitStream.Read ( fFloat );
                    pHandlingEntry->SetSeatOffsetDistance ( fFloat );
                    break;

                /*case HANDLING_MONETARY:
                    bitStream.Read ( uiInt );
                    pHandlingEntry->SetMonetary ( uiInt );
                    break;*/

                case HANDLING_HANDLINGFLAGS:
                    bitStream.Read ( uiInt );
                    pHandlingEntry->SetHandlingFlags ( uiInt );
                    break;

                case HANDLING_MODELFLAGS:
                    bitStream.Read ( uiInt );
                    pHandlingEntry->SetModelFlags ( uiInt );
                    break;

                /*case HANDLING_HEADLIGHT:
                    bitStream.Read ( ucChar );
                    if ( ucChar > CHandlingEntry::TALL )
                        ucChar = CHandlingEntry::TALL;

                    pHandlingEntry->SetHeadLight ( static_cast < CHandlingEntry::eLightType > ( ucChar ) );
                    break;

                case HANDLING_TAILLIGHT:
                    bitStream.Read ( ucChar );
                    if ( ucChar > CHandlingEntry::TALL ) 
                        ucChar = CHandlingEntry::TALL;

                    pHandlingEntry->SetTailLight ( static_cast < CHandlingEntry::eLightType > ( ucChar ) );
                    break;*/

                case HANDLING_ANIMGROUP:
                    bitStream.Read ( ucChar );
                    //pHandlingEntry->SetAnimGroup ( ucChar );
                    break;
            }

            vehicle.ApplyHandling();
        }
    }
}


void CHandlingRPCs::RestoreVehicleHandlingProperty ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the property id
    unsigned char ucProperty;
    if ( bitStream.Read ( ucProperty ) )
    {
        // Check its type
        if ( pSource && pSource->GetType () == CCLIENTVEHICLE )
        {
            // Grab the vehicle handling entry and the original handling
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( *pSource );
            CHandlingEntry* pHandlingEntry = Vehicle.GetHandlingData ();
            const CHandlingEntry* pOriginalEntry = Vehicle.GetOriginalHandlingData ();

            // Depending on what property
            switch ( ucProperty )
            {
                case HANDLING_MASS:
                    pHandlingEntry->SetMass ( pOriginalEntry->GetMass () );
                    break;
                    
                case HANDLING_TURNMASS:
                    pHandlingEntry->SetTurnMass ( pOriginalEntry->GetTurnMass () );
                    break;

                case HANDLING_DRAGCOEFF:
                    pHandlingEntry->SetDragCoeff ( pOriginalEntry->GetDragCoeff () );
                    break;

                case HANDLING_CENTEROFMASS:
                    pHandlingEntry->SetCenterOfMass ( pOriginalEntry->GetCenterOfMass () );
                    break;

                case HANDLING_PERCENTSUBMERGED:
                    pHandlingEntry->SetPercentSubmerged ( pOriginalEntry->GetPercentSubmerged () );
                    break;

                case HANDLING_TRACTIONMULTIPLIER:
                    pHandlingEntry->SetTractionMultiplier ( pOriginalEntry->GetTractionMultiplier () );
                    break;

                case HANDLING_DRIVETYPE:
                    pHandlingEntry->SetCarDriveType ( pOriginalEntry->GetCarDriveType () );
                    break;

                case HANDLING_ENGINETYPE:
                    pHandlingEntry->SetCarEngineType ( pOriginalEntry->GetCarEngineType () );
                    break;

                case HANDLING_NUMOFGEARS:
                    pHandlingEntry->SetNumberOfGears ( pOriginalEntry->GetNumberOfGears () );
                    break;

                case HANDLING_ENGINEACCELERATION:
                    pHandlingEntry->SetEngineAcceleration ( pOriginalEntry->GetEngineAcceleration () );
                    break;

                case HANDLING_ENGINEINERTIA:
                    pHandlingEntry->SetEngineInertia ( pOriginalEntry->GetEngineInertia () );
                    break;

                case HANDLING_MAXVELOCITY:
                    pHandlingEntry->SetMaxVelocity ( pOriginalEntry->GetMaxVelocity () );
                    break;

                case HANDLING_BRAKEDECELERATION:
                    pHandlingEntry->SetBrakeDeceleration ( pOriginalEntry->GetBrakeDeceleration () );
                    break;

                case HANDLING_BRAKEBIAS:
                    pHandlingEntry->SetBrakeBias ( pOriginalEntry->GetBrakeBias () );
                    break;

                case HANDLING_ABS:
                    pHandlingEntry->SetABS ( pOriginalEntry->GetABS () );
                    break;

                case HANDLING_STEERINGLOCK:
                    pHandlingEntry->SetSteeringLock ( pOriginalEntry->GetSteeringLock () );
                    break;

                case HANDLING_TRACTIONLOSS:
                    pHandlingEntry->SetTractionLoss ( pOriginalEntry->GetTractionLoss () );
                    break;

                case HANDLING_TRACTIONBIAS:
                    pHandlingEntry->SetTractionBias ( pOriginalEntry->GetTractionBias () );
                    break;

                case HANDLING_SUSPENSION_FORCELEVEL:
                    pHandlingEntry->SetSuspensionForceLevel ( pOriginalEntry->GetSuspensionForceLevel () );
                    break;

                case HANDLING_SUSPENSION_DAMPING:
                    pHandlingEntry->SetSuspensionDamping ( pOriginalEntry->GetSuspensionDamping () );
                    break;

                case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
                    pHandlingEntry->SetSuspensionHighSpeedDamping ( pOriginalEntry->GetSuspensionHighSpeedDamping () );
                    break;

                case HANDLING_SUSPENSION_UPPER_LIMIT:
                    pHandlingEntry->SetSuspensionUpperLimit ( pOriginalEntry->GetSuspensionUpperLimit () );
                    break;

                case HANDLING_SUSPENSION_LOWER_LIMIT:
                    pHandlingEntry->SetSuspensionLowerLimit ( pOriginalEntry->GetSuspensionLowerLimit () );
                    break;

                case HANDLING_SUSPENSION_FRONTREARBIAS:
                    pHandlingEntry->SetSuspensionFrontRearBias ( pOriginalEntry->GetSuspensionFrontRearBias () );
                    break;

                case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
                    pHandlingEntry->SetSuspensionAntiDiveMultiplier ( pOriginalEntry->GetSuspensionAntiDiveMultiplier () );
                    break;

                case HANDLING_COLLISIONDAMAGEMULTIPLIER:
                    pHandlingEntry->SetCollisionDamageMultiplier ( pOriginalEntry->GetCollisionDamageMultiplier () );
                    break;

                case HANDLING_SEATOFFSETDISTANCE:
                    pHandlingEntry->SetSeatOffsetDistance ( pOriginalEntry->GetSeatOffsetDistance () );
                    break;

                case HANDLING_HANDLINGFLAGS:
                    pHandlingEntry->SetHandlingFlags ( pOriginalEntry->GetHandlingFlags () );
                    break;

                case HANDLING_MODELFLAGS:
                    pHandlingEntry->SetModelFlags ( pOriginalEntry->GetModelFlags () );
                    break;

                case HANDLING_HEADLIGHT:
                    pHandlingEntry->SetHeadLight ( pOriginalEntry->GetHeadLight () );
                    break;

                case HANDLING_TAILLIGHT:
                    pHandlingEntry->SetTailLight ( pOriginalEntry->GetTailLight () );
                    break;

                case HANDLING_ANIMGROUP:
                    pHandlingEntry->SetAnimGroup ( pOriginalEntry->GetAnimGroup () );
                    break;
            }

            Vehicle.ApplyHandling();
        }
    }
}


void CHandlingRPCs::RestoreVehicleHandling ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Check its type
    if ( pSource && pSource->GetType () == CCLIENTVEHICLE )
    {
        // Grab the vehicle handling entry and restore all data
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( *pSource );
        Vehicle.GetHandlingData()->Assign ( Vehicle.GetOriginalHandlingData () );

        Vehicle.ApplyHandling();
    }
}