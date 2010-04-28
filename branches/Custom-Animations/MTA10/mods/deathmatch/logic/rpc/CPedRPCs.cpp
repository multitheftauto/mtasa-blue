/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CPedRPCs.cpp
*  PURPOSE:     Ped remote procedure calls
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CPedRPCs.h"
#include "net/SyncStructures.h"

class CLuaArguments;

void CPedRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_PED_ARMOR, SetPedArmor, "SetPedArmor" );
    AddHandler ( SET_PED_ROTATION, SetPedRotation, "SetPedRotation" );
    AddHandler ( GIVE_PED_JETPACK, GivePedJetPack, "GivePedJetPack" );
    AddHandler ( REMOVE_PED_JETPACK, RemovePedJetPack, "RemovePedJetPack" );
    AddHandler ( REMOVE_PED_CLOTHES, RemovePedClothes, "RemovePedClothes" );    
    AddHandler ( SET_PED_GRAVITY, SetPedGravity, "SetPedGravity" );
    AddHandler ( SET_PED_CHOKING, SetPedChoking, "SetPedChoking" );
    AddHandler ( SET_PED_FIGHTING_STYLE, SetPedFightingStyle, "SetPedFightingStyle" );
    AddHandler ( SET_PED_MOVE_ANIM, SetPedMoveAnim, "SetPedMoveAnim" );
    AddHandler ( WARP_PED_INTO_VEHICLE, WarpPedIntoVehicle, "WarpPedIntoVehicle" );
    AddHandler ( REMOVE_PED_FROM_VEHICLE, RemovePedFromVehicle, "RemovePedFromVehicle" );
    AddHandler ( SET_PED_DOING_GANG_DRIVEBY, SetPedDoingGangDriveby, "SetPedDoingGangDriveby" );
    AddHandler ( SET_PED_ANIMATION, SetPedAnimation, "SetPedAnimation" );
    AddHandler ( SET_PED_ON_FIRE, SetPedOnFire, "SetPedOnFire" );
    AddHandler ( SET_PED_HEADLESS, SetPedHeadless, "SetPedHeadless" );
    AddHandler ( SET_PED_FROZEN, SetPedFrozen, "SetPedFrozen" );
    AddHandler ( RELOAD_PED_WEAPON, ReloadPedWeapon, "ReloadPedWeapon" );
}


void CPedRPCs::SetPedArmor ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucArmor;
    unsigned char ucTimeContext;
    if ( bitStream.ReadCompressed ( ID ) &&
         bitStream.Read ( ucArmor ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        float fArmor = static_cast < float > ( ucArmor ) / 1.25f;

        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetSyncTimeContext ( ucTimeContext );
            pPed->SetArmor ( fArmor );
        }
    }
}


void CPedRPCs::SetPedRotation ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    SPedRotationSync rotation;
    unsigned char ucTimeContext;
    if ( bitStream.ReadCompressed ( ID ) &&
         bitStream.Read ( &rotation ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetCurrentRotation ( rotation.data.fRotation );
            if ( !IS_PLAYER ( pPed ) )
                pPed->SetCameraRotation ( rotation.data.fRotation );
            pPed->SetSyncTimeContext ( ucTimeContext );
        }
    }
}


void CPedRPCs::GivePedJetPack ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    if ( bitStream.ReadCompressed ( ID ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetHasJetPack ( true );
        }
    }
}


void CPedRPCs::RemovePedJetPack ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    if ( bitStream.ReadCompressed ( ID ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetHasJetPack ( false );
        }
    }
}


void CPedRPCs::RemovePedClothes ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucType;
    if ( bitStream.ReadCompressed ( ID ) && bitStream.Read ( ucType ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            CClientPlayerClothes* pClothes = pPed->GetClothes ();
            if ( pClothes->RemoveClothes ( ucType, false ) )
            {
                pPed->RebuildModel ( true );
            }
        }
    }
}


void CPedRPCs::SetPedGravity ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    float fGravity;
    if ( bitStream.ReadCompressed ( ID ) && bitStream.Read ( fGravity ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            if ( pPed->IsLocalPlayer () )
            {
                g_pCore->GetMultiplayer ()->SetLocalPlayerGravity ( fGravity );
            }
            else
            {
                CRemoteDataStorage* pData = pPed->GetRemoteData ();
                if ( pData )
                {
                    pData->SetGravity ( fGravity );
                }
            }
        }
    }
}


void CPedRPCs::SetPedChoking ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    bool bChoking;
    if ( bitStream.ReadCompressed ( ID ) && bitStream.ReadBit ( bChoking ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetChoking ( bChoking );
        }
    }
}


void CPedRPCs::SetPedFightingStyle ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucStyle;
    if ( bitStream.ReadCompressed ( ID ) && bitStream.Read ( ucStyle ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetFightingStyle ( ( eFightingStyle ) ucStyle );
        }
    }
}


void CPedRPCs::SetPedMoveAnim ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned int uiMoveAnim;
    if ( bitStream.ReadCompressed ( ID ) && bitStream.ReadCompressed ( uiMoveAnim ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetMoveAnim ( (eMoveAnim)uiMoveAnim );
        }
    }
}


void CPedRPCs::WarpPedIntoVehicle ( NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    ElementID PedID;
    ElementID VehicleID;
    unsigned char ucSeat;
    unsigned char ucTimeContext;
    if ( bitStream.ReadCompressed ( PedID ) &&
         bitStream.ReadCompressed ( VehicleID ) &&
         bitStream.Read ( ucSeat ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetSyncTimeContext ( ucTimeContext );

            // Grab the vehicle
            CClientVehicle* pVehicle = m_pVehicleManager->Get ( VehicleID );
            if ( pVehicle )
            {
                if ( pPed->IsLocalPlayer () )
                {
                    // Reset the vehicle in/out checks
                    m_pClientGame->ResetVehicleInOut ();

                    /*
                    // Make sure it can be damaged again (doesn't get changed back when we force the player in)
                    pVehicle->SetCanBeDamaged ( true );
                    pVehicle->SetTyresCanBurst ( true );
                    */
                }

                // Warp the player into the vehicle
                pPed->WarpIntoVehicle ( pVehicle, ucSeat );
                pPed->SetVehicleInOutState ( VEHICLE_INOUT_NONE );

                pVehicle->CalcAndUpdateCanBeDamagedFlag ();
                pVehicle->CalcAndUpdateTyresCanBurstFlag ();

                // Call the onClientPlayerEnterVehicle event
                CLuaArguments Arguments;
                Arguments.PushElement ( pVehicle );        // vehicle
                Arguments.PushNumber ( ucSeat );            // seat
                pPed->CallEvent ( "onClientPlayerVehicleEnter", Arguments, true );

                // Call the onClientVehicleEnter event
                CLuaArguments Arguments2;
                Arguments2.PushElement ( pPed );        // player
                Arguments2.PushNumber ( ucSeat );           // seat
                pVehicle->CallEvent ( "onClientVehicleEnter", Arguments2, true );
            }
        }
    }
}


void CPedRPCs::RemovePedFromVehicle ( NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    ElementID PedID;
    unsigned char ucTimeContext;
    if ( bitStream.ReadCompressed ( PedID ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetSyncTimeContext ( ucTimeContext );

            // Remove the player from his vehicle
            pPed->RemoveFromVehicle ();
            pPed->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
            if ( pPed->m_bIsLocalPlayer )
            {
                // Reset expectation of vehicle enter completion, in case we were removed while entering
                g_pClientGame->ResetVehicleInOut ();
            }
        }
    }
}


void CPedRPCs::SetPedDoingGangDriveby ( NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    ElementID PedID;
    bool bDriveby;
    unsigned char ucTimeContext;
    if ( bitStream.ReadCompressed ( PedID ) &&
         bitStream.ReadBit ( bDriveby ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetSyncTimeContext ( ucTimeContext );

            pPed->SetDoingGangDriveby ( bDriveby );
        }
    }
}


void CPedRPCs::SetPedAnimation ( NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    ElementID PedID;   
    char szBlockName [ 64 ], szAnimName [ 64 ];
    unsigned char ucBlockSize, ucAnimSize;
    int iTime;
    bool bLoop, bUpdatePosition, bInterruptable;

    if ( bitStream.ReadCompressed ( PedID ) &&
         bitStream.Read ( ucBlockSize ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            if ( ucBlockSize > 0 )
            {
                if ( bitStream.Read ( szBlockName, ucBlockSize ) &&
                    bitStream.Read ( ucAnimSize ) )
                {
                    szBlockName [ ucBlockSize ] = 0;
                    if ( bitStream.Read ( szAnimName, ucAnimSize ) &&
                         bitStream.Read ( iTime ) &&
                         bitStream.ReadBit ( bLoop ) &&
                         bitStream.ReadBit ( bUpdatePosition ) &&
                         bitStream.ReadBit ( bInterruptable ) )
                    {
                        szAnimName [ ucAnimSize ] = 0;

                        CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( szBlockName );
                        if ( pBlock )
                        {
                            pPed->RunNamedAnimation ( pBlock, szAnimName, iTime, bLoop, bUpdatePosition, bInterruptable );
                        }
                    }
                }
            }
            else
            {
                pPed->KillAnimation ();
            }
        }
    }
}


void CPedRPCs::SetPedOnFire ( NetBitStreamInterface& bitStream )
{
    ElementID PedID;
    bool bIsOnFire;

    if ( bitStream.ReadCompressed ( PedID ) &&
         bitStream.ReadBit ( bIsOnFire ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetOnFire( bIsOnFire );
        }
    }
}


void CPedRPCs::SetPedHeadless ( NetBitStreamInterface& bitStream )
{
    ElementID PedID;
    bool bIsHeadless;

    if ( bitStream.ReadCompressed ( PedID ) &&
         bitStream.ReadBit ( bIsHeadless ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetHeadless ( bIsHeadless );
        }
    }
}


void CPedRPCs::SetPedFrozen ( NetBitStreamInterface& bitStream )
{
    ElementID PedID;
    bool bIsFrozen = 0;

    if ( bitStream.ReadCompressed ( PedID ) &&
         bitStream.ReadBit ( bIsFrozen ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetFrozen ( bIsFrozen );
        }
    }
}


void CPedRPCs::ReloadPedWeapon ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    if ( bitStream.ReadCompressed ( ID ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->ReloadWeapon ();            
        }
    }
}
