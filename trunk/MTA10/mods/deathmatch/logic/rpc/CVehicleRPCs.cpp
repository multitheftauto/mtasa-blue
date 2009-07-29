/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CVehicleRPCs.cpp
*  PURPOSE:     Vehicle remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CVehicleRPCs.h"

void CVehicleRPCs::LoadFunctions ( void )
{
    AddHandler ( DESTROY_ALL_VEHICLES, DestroyAllVehicles, "DestroyAllVehicles" );
    AddHandler ( FIX_VEHICLE, FixVehicle, "FixVehicle" );
    AddHandler ( BLOW_VEHICLE, BlowVehicle, "BlowVehicle" );
    AddHandler ( SET_VEHICLE_ROTATION, SetVehicleRotation, "SetVehicleRotation" );
    AddHandler ( SET_VEHICLE_TURNSPEED, SetVehicleTurnSpeed, "SetVehicleTurnSpeed" );
    AddHandler ( SET_VEHICLE_COLOR, SetVehicleColor, "SetVehicleColor" );
    AddHandler ( SET_VEHICLE_LOCKED, SetVehicleLocked, "SetVehicleLocked" );
    AddHandler ( SET_VEHICLE_DOORS_UNDAMAGEABLE, SetVehicleDoorsUndamageable, "" );
    AddHandler ( SET_VEHICLE_SIRENE_ON, SetVehicleSireneOn, "SetVehicleSireneOn" );
    AddHandler ( SET_VEHICLE_LANDING_GEAR_DOWN, SetVehicleLandingGearDown, "SetVehicleLandingGearDown" );
    AddHandler ( SET_HELICOPTER_ROTOR_SPEED, SetHelicopterRotorSpeed, "SetHelicopterRotorSpeed" );
	AddHandler ( ADD_VEHICLE_UPGRADE, AddVehicleUpgrade, "AddVehicleUpgrade" );
    AddHandler ( ADD_ALL_VEHICLE_UPGRADES, AddAllVehicleUpgrades, "AddAllVehicleUpgrades" );
	AddHandler ( REMOVE_VEHICLE_UPGRADE, RemoveVehicleUpgrade, "RemoveVehicleUpgrade" );
	AddHandler ( SET_VEHICLE_DAMAGE_STATE, SetVehicleDamageState, "SetVehicleDamageState" );
    AddHandler ( SET_VEHICLE_OVERRIDE_LIGHTS, SetVehicleOverrideLights, "SetVehicleOverrideLights" );
    AddHandler ( SET_VEHICLE_ENGINE_STATE, SetVehicleEngineState, "SetVehicleEngineState" );
    AddHandler ( SET_VEHICLE_DIRT_LEVEL, SetVehicleDirtLevel, "SetVehicleDirtLevel" );
    AddHandler ( SET_VEHICLE_DAMAGE_PROOF, SetVehicleDamageProof, "SetVehicleDamageProof" );
    AddHandler ( SET_VEHICLE_PAINTJOB, SetVehiclePaintjob, "SetVehiclePaintjob" );
    AddHandler ( SET_VEHICLE_FUEL_TANK_EXPLODABLE, SetVehicleFuelTankExplodable, "SetVehicleFuelTankExplodable" );
    AddHandler ( SET_VEHICLE_WHEEL_STATES, SetVehicleWheelStates, "SetVehicleWheelStates" );
    AddHandler ( SET_VEHICLE_FROZEN, SetVehicleFrozen, "SetVehicleFrozen" );
    AddHandler ( SET_TRAIN_DERAILED, SetTrainDerailed, "SetTrainDerailed" );
    AddHandler ( SET_TRAIN_DERAILABLE, SetTrainDerailable, "SetTrainDerailable" );
    AddHandler ( SET_TRAIN_DIRECTION, SetTrainDirection, "SetTrainDirection" );
    AddHandler ( SET_TRAIN_SPEED, SetTrainSpeed, "SetTrainSpeed" );
    AddHandler ( SET_TAXI_LIGHT_ON, SetVehicleTaxiLightOn, "SetVehicleTaxiLightOn" );
    AddHandler ( SET_VEHICLE_HEADLIGHT_COLOR, SetVehicleHeadLightColor, "SetVehicleHeadLightColor" );
}


void CVehicleRPCs::DestroyAllVehicles ( NetBitStreamInterface& bitStream )
{
    m_pVehicleManager->DeleteAll ();
}


void CVehicleRPCs::FixVehicle ( NetBitStreamInterface& bitStream )
{
    // Read out the vehicle id
    ElementID ID;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            // Fix it
            pVehicle->Fix ();
            pVehicle->SetSyncTimeContext ( ucTimeContext );
        }
    }
}


void CVehicleRPCs::BlowVehicle ( NetBitStreamInterface& bitStream )
{
    // Read out the vehicle id and whether to explode or not
    ElementID ID;
    unsigned char ucExplode;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucExplode ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            // Blow it and change the time context
            pVehicle->Blow ( true );
            pVehicle->SetSyncTimeContext ( ucTimeContext );
        }
    }
}


void CVehicleRPCs::SetVehicleRotation ( NetBitStreamInterface& bitStream )
{
    // Read out the vehicle id
    ElementID ID;
    if ( bitStream.Read ( ID ) )
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            // Read out the rotation
            CVector vecRotation;
            unsigned char ucTimeContext;
            if ( bitStream.Read ( vecRotation.fX ) &&
                 bitStream.Read ( vecRotation.fY ) &&
                 bitStream.Read ( vecRotation.fZ ) &&
                 bitStream.Read ( ucTimeContext ) )
            {      
                // Set the new rotation
                pVehicle->SetRotationDegrees ( vecRotation );
                pVehicle->SetSyncTimeContext ( ucTimeContext );
            }
        }
    }
}


void CVehicleRPCs::SetVehicleTurnSpeed ( NetBitStreamInterface& bitStream )
{
    // Read out the vehicle id
    ElementID ID;
    if ( bitStream.Read ( ID ) )
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            // Read out the turn speed
            CVector vecTurnSpeed;
            if ( bitStream.Read ( vecTurnSpeed.fX ) &&
                 bitStream.Read ( vecTurnSpeed.fY ) &&
                 bitStream.Read ( vecTurnSpeed.fZ ) )
            {
                // Set the new movespeed
                pVehicle->SetTurnSpeed ( vecTurnSpeed );
            }
        }
    }
}


void CVehicleRPCs::SetVehicleColor ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    bitStream.Read ( ID );

    CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
    if ( pVehicle )
    {
        unsigned char ucColor1;
        bitStream.Read ( ucColor1 );

        unsigned char ucColor2;
        bitStream.Read ( ucColor2 );

        unsigned char ucColor3;
        bitStream.Read ( ucColor3 );

        unsigned char ucColor4;
        bitStream.Read ( ucColor4 );

        pVehicle->SetColor ( ucColor1, ucColor2, ucColor3, ucColor4 );
    }
}


void CVehicleRPCs::SetVehicleLocked ( NetBitStreamInterface& bitStream )
{
    // Read out the stuff
    ElementID ID;
    unsigned char ucLocked;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucLocked ) )
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            // Set its locked state
            pVehicle->SetDoorsLocked ( ucLocked != 0 );
        }
    }
}


void CVehicleRPCs::SetVehicleDoorsUndamageable ( NetBitStreamInterface& bitStream )
{
    // Read out the stuff
    ElementID ID;
    unsigned char ucDoorsUndamageable;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucDoorsUndamageable ) )
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            // Set its doors undamageable state
            pVehicle->SetDoorsUndamageable ( ucDoorsUndamageable != 0 );
        }
    }
}


void CVehicleRPCs::SetVehicleSireneOn ( NetBitStreamInterface& bitStream )
{
    ElementID ID = 0;
    bitStream.Read ( ID );

    CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
    if ( pVehicle )
    {
        unsigned char ucSirenesOn;
        if ( bitStream.Read ( ucSirenesOn ) )
        {
            bool bSirenesOn = false;
            if ( ucSirenesOn > 0 ) bSirenesOn = true;

            pVehicle->SetSirenOrAlarmActive ( bSirenesOn );
        }
    }
}

void CVehicleRPCs::SetVehicleTaxiLightOn ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucTaxiLightOn;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucTaxiLightOn ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetTaxiLightOn ( ucTaxiLightOn != 0 );
        }
    }
}

void CVehicleRPCs::SetVehicleLandingGearDown ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    bitStream.Read ( ID );

    CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
    if ( pVehicle )
    {
        unsigned char ucLandingGearDown;
        if ( bitStream.Read ( ucLandingGearDown ) )
        {
            bool bLandingGearDown = false;
            if ( ucLandingGearDown > 0 ) bLandingGearDown = true;

            pVehicle->SetLandingGearDown ( bLandingGearDown );
        }
    }
}


void CVehicleRPCs::SetHelicopterRotorSpeed ( NetBitStreamInterface& bitStream )
{
    ElementID ID = 0;
    bitStream.Read ( ID );

    CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
    if ( pVehicle )
    {
        unsigned char ucRotorSpeed;
        if ( bitStream.Read ( ucRotorSpeed ) )
        {
            // Convert the given rotor speed from between 0-100 to 0-0.22
            float fRotorSpeed = ( static_cast < float > ( ucRotorSpeed ) / 100.0f * 0.22f );
            pVehicle->SetHeliRotorSpeed ( fRotorSpeed );
        }
    }
}


void CVehicleRPCs::AddVehicleUpgrade ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
	if ( bitStream.Read ( ID ) )
	{
		CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
		if ( pVehicle )
		{
			unsigned short usUpgrade;
			if ( bitStream.Read ( usUpgrade ) )
			{
				CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
				if ( pUpgrades )
				{
					pUpgrades->AddUpgrade ( usUpgrade );
				}
			}
		}
	}
}


void CVehicleRPCs::AddAllVehicleUpgrades ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
	if ( bitStream.Read ( ID ) )
	{
		CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
		if ( pVehicle )
		{			
			CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
			if ( pUpgrades )
			{
				pUpgrades->AddAllUpgrades ();
            }
		}
	}
}


void CVehicleRPCs::RemoveVehicleUpgrade ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
	if ( bitStream.Read ( ID ) )
	{
		CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
		if ( pVehicle )
		{
			unsigned char ucUpgrade;
			if ( bitStream.Read ( ucUpgrade ) )
			{
				// Convert back and add
				unsigned short usUpgrade = ( ucUpgrade + 1000 );

				CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
				if ( pUpgrades )
				{
					pUpgrades->RemoveUpgrade ( usUpgrade );
				}
			}
		}
	}
}


void CVehicleRPCs::SetVehicleDamageState ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
	if ( bitStream.Read ( ID ) )
	{
		CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
		if ( pVehicle )
		{
			unsigned char ucObject;
			if ( bitStream.Read ( ucObject ) )
			{
				switch ( ucObject )
				{
					case 0:	// Door
					{
						unsigned char ucDoor, ucState;
						if ( bitStream.Read ( ucDoor ) && bitStream.Read ( ucState ) )
						{
							pVehicle->SetDoorStatus ( ucDoor, ucState );
						}
						break;
					}
					case 1: // Wheel
					{
						unsigned char ucWheel, ucState;
						if ( bitStream.Read ( ucWheel ) && bitStream.Read ( ucState ) )
						{
							pVehicle->SetWheelStatus ( ucWheel, ucState, false );
						}
						break;
					}
					case 2: // Light
					{
						unsigned char ucLight, ucState;
						if ( bitStream.Read ( ucLight ) && bitStream.Read ( ucState ) )
						{
							pVehicle->SetLightStatus ( ucLight, ucState );
						}
						break;
					}
					case 3: // Panel
					{
                        unsigned char ucPanel, ucState;
						if ( bitStream.Read ( ucPanel ) && bitStream.Read ( ucState ) )
						{
							pVehicle->SetPanelStatus ( ucPanel, ucState );
						}
					}
					default: break;
				}
			}
		}
	}
}


void CVehicleRPCs::SetVehicleOverrideLights ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucLights;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucLights ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetOverrideLights ( ucLights );
        }
    }
}


void CVehicleRPCs::SetVehicleEngineState (  NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucState;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucState ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetEngineOn ( ucState == 1 );
        }
    }
}


void CVehicleRPCs::SetVehicleDirtLevel (  NetBitStreamInterface& bitStream )
{
    ElementID ID;
    float fDirtLevel;
    if ( bitStream.Read ( ID ) && bitStream.Read ( fDirtLevel ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetDirtLevel ( fDirtLevel );
        }
    }
}


void CVehicleRPCs::SetVehicleDamageProof (  NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucDamageProof;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucDamageProof ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetScriptCanBeDamaged ( ( ucDamageProof == 0 ) );
        }
    }
}


void CVehicleRPCs::SetVehiclePaintjob ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucPaintjob;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucPaintjob ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetPaintjob ( ucPaintjob );
        }
    }
}


void CVehicleRPCs::SetVehicleFuelTankExplodable ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucExplodable;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucExplodable ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetCanShootPetrolTank ( ( ucExplodable == 1 ) );
        }
    }
}


void CVehicleRPCs::SetVehicleWheelStates ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucWheelStates [ MAX_WHEELS ];
    if ( bitStream.Read ( ID ) && bitStream.Read ( ( char * ) ucWheelStates, MAX_WHEELS ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            for ( int i = 0 ; i < MAX_WHEELS ; i++ ) pVehicle->SetWheelStatus ( i, ucWheelStates [ i ], false );
        }
    }
}


void CVehicleRPCs::SetVehicleFrozen ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucFrozen;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucFrozen ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            if ( ucFrozen == 0 )
            {
                pVehicle->SetScriptFrozen ( true );
                pVehicle->SetFrozen ( true );
            }
            else
            {
                pVehicle->SetScriptFrozen ( false );
                pVehicle->SetFrozen ( false );
            }
        }
    }
}


void CVehicleRPCs::SetTrainDerailed ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucDerailed;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucDerailed ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetDerailed ( ucDerailed != 0 );
        }
    }
}


void CVehicleRPCs::SetTrainDerailable ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucDerailable;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucDerailable ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetDerailable ( ucDerailable != 0 );
        }
    }
}


void CVehicleRPCs::SetTrainDirection ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucDirection;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucDirection ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetTrainDirection ( ucDirection != 0 );
        }
    }
}


void CVehicleRPCs::SetTrainSpeed ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    float fSpeed;
    if ( bitStream.Read ( ID ) && bitStream.Read ( fSpeed ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            pVehicle->SetTrainSpeed ( fSpeed );
        }
    }
}


void CVehicleRPCs::SetVehicleHeadLightColor ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucR, ucG, ucB;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucR ) &&
         bitStream.Read ( ucG ) &&
         bitStream.Read ( ucB ) )
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
        if ( pVehicle )
        {
            RGBA color = COLOR_RGBA ( ucR, ucG, ucB, 255 );
            pVehicle->SetHeadLightColor ( color );
        }
    }
}