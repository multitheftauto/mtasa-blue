/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CHandlingRPCs.cpp
*  PURPOSE:     Handling remote procedure calls
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CHandlingRPCs.h"

void CHandlingRPCs::LoadFunctions ( void )
{
    AddHandler ( HANDLING_SET_DEFAULT, HandlingAddDefault, "HandlingAddDefault" );
    AddHandler ( HANDLING_RESTORE_DEFAULT, HandlingRemoveDefault, "HandlingRemoveDefault" );
    AddHandler ( HANDLING_SET_PROPERTY, HandlingSetProperty, "HandlingSetProperty" );
}


void CHandlingRPCs::HandlingAddDefault ( NetBitStreamInterface& bitStream )
{
    // Read out handling id and vehicle type
    ElementID ID;
    unsigned char ucVehicleType;
    if ( bitStream.Read ( ID ) && 
         bitStream.Read ( ucVehicleType ) )
    {
        // Calculate vehicle ID
        eVehicleTypes eModel = static_cast < eVehicleTypes > ( ucVehicleType + 400 );

        // Grab it and check its type
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity && pEntity->GetType () == CCLIENTHANDLING )
        {
            // Make it as default for all of the given vehicle id's
            static_cast < CClientHandling* > ( pEntity ) ->AddDefaultTo ( eModel );
        }
    }
}


void CHandlingRPCs::HandlingRemoveDefault ( NetBitStreamInterface& bitStream )
{
    // Read out handling id and vehicle type
    ElementID ID;
    unsigned char ucVehicleType;
    if ( bitStream.Read ( ID ) && 
         bitStream.Read ( ucVehicleType ) )
    {
        // Calculate vehicle ID
        eVehicleTypes eModel = static_cast < eVehicleTypes > ( ucVehicleType + 400 );

        // Grab it and check its type
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity && pEntity->GetType () == CCLIENTHANDLING )
        {
            // Make it as default for all of the given vehicle id's
            static_cast < CClientHandling* > ( pEntity ) ->RemoveDefaultTo ( eModel );
        }
    }
}


// Enum with property id's for handling
enum eHandlingProperty
{
    PROPERTY_MASS,
    PROPERTY_TURNMASS,
    PROPERTY_DRAGCOEFF,
    PROPERTY_CENTEROFMASS,
    PROPERTY_PERCENTSUBMERGED,
    PROPERTY_TRACTIONMULTIPLIER,
    PROPERTY_DRIVETYPE,
    PROPERTY_ENGINETYPE,
    PROPERTY_NUMOFGEARS,
    PROPERTY_ENGINEACCELLERATION,
    PROPERTY_ENGINEINERTIA,
    PROPERTY_MAXVELOCITY,
    PROPERTY_BRAKEDECELLERATION,
    PROPERTY_BRAKEBIAS,
    PROPERTY_ABS,
    PROPERTY_STEERINGLOCK,
    PROPERTY_TRACTIONLOSS,
    PROPERTY_TRACTIONBIAS,
    PROPERTY_SUSPENSION_FORCELEVEL,
    PROPERTY_SUSPENSION_DAMPING,
    PROPERTY_SUSPENSION_HIGHSPEEDDAMPING,
    PROPERTY_SUSPENSION_UPPER_LIMIT,
    PROPERTY_SUSPENSION_LOWER_LIMIT,
    PROPERTY_SUSPENSION_FRONTREARBIAS,
    PROPERTY_SUSPENSION_ANTIDIVEMULTIPLIER,
    PROPERTY_COLLISIONDAMAGEMULTIPLIER,
    PROPERTY_SEATOFFSETDISTANCE,
    PROPERTY_HANDLINGFLAGS,
    PROPERTY_MODELFLAGS,
    PROPERTY_HEADLIGHT,
    PROPERTY_TAILLIGHT,
    PROPERTY_ANIMGROUP,
};


void CHandlingRPCs::HandlingSetProperty ( NetBitStreamInterface& bitStream )
{
    // Read out the handling id and property id
    ElementID ID;
    unsigned char ucProperty;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucProperty ) )
    {
        // Grab it and check its type
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity && pEntity->GetType () == CCLIENTHANDLING )
        {
            // Grab the handling class
            CClientHandling& HandlingElement = static_cast < CClientHandling& > ( *pEntity );
            
            // Temporary storage for reading out data
            union
            {
                unsigned char ucChar;
                unsigned int uiInt;
                float fFloat;
            };

            // Depending on what property...
            switch ( ucProperty )
            {
                case PROPERTY_MASS:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetMass ( fFloat );
                    break;
                    
                case PROPERTY_TURNMASS:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetTurnMass ( fFloat );
                    break;

                case PROPERTY_DRAGCOEFF:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetDragCoeff ( fFloat );
                    break;

                case PROPERTY_CENTEROFMASS:
                {
                    CVector vecVector;
                    bitStream.Read ( vecVector.fX );
                    bitStream.Read ( vecVector.fY );
                    bitStream.Read ( vecVector.fZ );
                    HandlingElement.SetCenterOfMass ( vecVector );
                    break;
                }

                case PROPERTY_PERCENTSUBMERGED:
                    bitStream.Read ( uiInt );
                    HandlingElement.SetPercentSubmerged ( uiInt );
                    break;

                case PROPERTY_TRACTIONMULTIPLIER:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetTractionMultiplier ( fFloat );
                    break;

                case PROPERTY_DRIVETYPE:
                {
                    bitStream.Read ( ucChar );
                    if ( ucChar != CHandlingEntry::FOURWHEEL &&
                         ucChar != CHandlingEntry::RWD &&
                         ucChar != CHandlingEntry::FWD )
                    {
                        ucChar = CHandlingEntry::RWD;
                    }

                    HandlingElement.SetDriveType ( static_cast < CHandlingEntry::eDriveType > ( ucChar ) );
                    break;
                }

                case PROPERTY_ENGINETYPE:
                {
                    bitStream.Read ( ucChar );
                    if ( ucChar != CHandlingEntry::DIESEL &&
                         ucChar != CHandlingEntry::ELECTRIC &&
                         ucChar != CHandlingEntry::PETROL )
                    {
                        ucChar = CHandlingEntry::PETROL;
                    }

                    HandlingElement.SetEngineType ( static_cast < CHandlingEntry::eEngineType > ( ucChar ) );
                    break;
                }

                case PROPERTY_NUMOFGEARS:
                    bitStream.Read ( ucChar );
                    HandlingElement.SetNumberOfGears ( ucChar );
                    break;

                case PROPERTY_ENGINEACCELLERATION:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetEngineAccelleration ( fFloat );
                    break;

                case PROPERTY_ENGINEINERTIA:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetEngineInertia ( fFloat );
                    break;

                case PROPERTY_MAXVELOCITY:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetMaxVelocity ( fFloat );
                    break;

                case PROPERTY_BRAKEDECELLERATION:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetBrakeDecelleration ( fFloat );
                    break;

                case PROPERTY_BRAKEBIAS:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetBrakeBias ( fFloat );
                    break;

                case PROPERTY_ABS:
                    bitStream.Read ( ucChar );
                    HandlingElement.SetABS ( ucChar != 0 );
                    break;

                case PROPERTY_STEERINGLOCK:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSteeringLock ( fFloat );
                    break;

                case PROPERTY_TRACTIONLOSS:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetTractionLoss ( fFloat );
                    break;

                case PROPERTY_TRACTIONBIAS:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetTractionBias ( fFloat );
                    break;

                case PROPERTY_SUSPENSION_FORCELEVEL:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSuspensionForceLevel ( fFloat );
                    break;

                case PROPERTY_SUSPENSION_DAMPING:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSuspensionDamping ( fFloat );
                    break;

                case PROPERTY_SUSPENSION_HIGHSPEEDDAMPING:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSuspensionHighSpeedDamping ( fFloat );
                    break;

                case PROPERTY_SUSPENSION_UPPER_LIMIT:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSuspensionUpperLimit ( fFloat );
                    break;

                case PROPERTY_SUSPENSION_LOWER_LIMIT:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSuspensionLowerLimit ( fFloat );
                    break;

                case PROPERTY_SUSPENSION_FRONTREARBIAS:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSuspensionFrontRearBias ( fFloat );
                    break;

                case PROPERTY_SUSPENSION_ANTIDIVEMULTIPLIER:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSuspensionAntidiveMultiplier ( fFloat );
                    break;

                case PROPERTY_COLLISIONDAMAGEMULTIPLIER:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetCollisionDamageMultiplier ( fFloat );
                    break;

                case PROPERTY_SEATOFFSETDISTANCE:
                    bitStream.Read ( fFloat );
                    HandlingElement.SetSeatOffsetDistance ( fFloat );
                    break;

                case PROPERTY_HANDLINGFLAGS:
                    bitStream.Read ( uiInt );
                    HandlingElement.SetHandlingFlags ( uiInt );
                    break;

                case PROPERTY_MODELFLAGS:
                    bitStream.Read ( uiInt );
                    HandlingElement.SetModelFlags ( uiInt );
                    break;

                case PROPERTY_HEADLIGHT:
                    bitStream.Read ( ucChar );
                    if ( ucChar > CHandlingEntry::TALL )
                        ucChar = CHandlingEntry::TALL;

                    HandlingElement.SetHeadLight ( static_cast < CHandlingEntry::eLightType > ( ucChar ) );
                    break;

                case PROPERTY_TAILLIGHT:
                    bitStream.Read ( ucChar );
                    if ( ucChar > CHandlingEntry::TALL )
                        ucChar = CHandlingEntry::TALL;

                    HandlingElement.SetTailLight ( static_cast < CHandlingEntry::eLightType > ( ucChar ) );
                    break;

                case PROPERTY_ANIMGROUP:
                    bitStream.Read ( ucChar );
                    HandlingElement.SetAnimGroup ( ucChar );
                    break;
            }
        }
    }
}