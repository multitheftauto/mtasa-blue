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

#include <StdInc.h>
#include "CPedRPCs.h"

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
}


void CPedRPCs::SetPedArmor ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucArmor;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ID ) &&
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
    float fRotation;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( fRotation ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetCurrentRotation ( fRotation );
            if ( !IS_PLAYER ( pPed ) )
                pPed->SetCameraRotation ( fRotation );
            pPed->SetSyncTimeContext ( ucTimeContext );
        }
    }
}


void CPedRPCs::GivePedJetPack ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    if ( bitStream.Read ( ID ) )
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
    if ( bitStream.Read ( ID ) )
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
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucType ) )
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
    if ( bitStream.Read ( ID ) && bitStream.Read ( fGravity ) )
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
    unsigned char ucChoking;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucChoking ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetChoking ( ( ucChoking == 1 ) );
        }
    }
}


void CPedRPCs::SetPedFightingStyle ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucStyle;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucStyle ) )
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
    int iMoveAnim;
    if ( bitStream.Read ( ID ) && bitStream.Read ( iMoveAnim ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            pPed->SetMoveAnim ( (eMoveAnim)iMoveAnim );
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
    if ( bitStream.Read ( PedID ) &&
         bitStream.Read ( VehicleID ) &&
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
    if ( bitStream.Read ( PedID ) &&
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
    unsigned char ucDriveby;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( PedID ) &&
         bitStream.Read ( ucDriveby ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetSyncTimeContext ( ucTimeContext );

            pPed->SetDoingGangDriveby ( ( ucDriveby == 1 ) );
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
    unsigned char ucLoop;
    unsigned char ucUpdatePosition;
    unsigned char ucInteruptable;
    if ( bitStream.Read ( PedID ) &&
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
                         bitStream.Read ( ucLoop ) &&
                         bitStream.Read ( ucUpdatePosition ) &&
                         bitStream.Read ( ucInteruptable ) )
                    {
                        szAnimName [ ucAnimSize ] = 0;

                        CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( szBlockName );
                        if ( pBlock )
                        {
                            pPed->RunNamedAnimation ( pBlock, szAnimName, iTime, ( ucLoop == 1 ), ( ucUpdatePosition == 1 ), ( ucInteruptable == 1 ) );
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
    unsigned char ucIsOnFire;

    if ( bitStream.Read ( PedID ) &&
         bitStream.Read ( ucIsOnFire ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetOnFire( ( ucIsOnFire == 1 ) );
        }
    }
}


void CPedRPCs::SetPedHeadless ( NetBitStreamInterface& bitStream )
{
    ElementID PedID;
    unsigned char ucIsHeadless;

    if ( bitStream.Read ( PedID ) &&
         bitStream.Read ( ucIsHeadless ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( PedID, true );
        if ( pPed )
        {
            pPed->SetHeadless ( ( ucIsHeadless == 1 ) );
        }
    }
}