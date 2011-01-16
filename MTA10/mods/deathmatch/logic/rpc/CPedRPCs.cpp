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


void CPedRPCs::SetPedArmor ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucArmor;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ucArmor ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        float fArmor = static_cast < float > ( ucArmor ) / 1.25f;

        CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetSyncTimeContext ( ucTimeContext );
            pPed->SetArmor ( fArmor );
        }
    }
}


void CPedRPCs::SetPedRotation ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    SPedRotationSync rotation;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( &rotation ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetCurrentRotation ( rotation.data.fRotation );
            if ( !IS_PLAYER ( pPed ) )
                pPed->SetCameraRotation ( rotation.data.fRotation );
            pPed->SetSyncTimeContext ( ucTimeContext );
        }
    }
}


void CPedRPCs::GivePedJetPack ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
    if ( pPed )
    {
        pPed->SetHasJetPack ( true );
    }
}


void CPedRPCs::RemovePedJetPack ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
    if ( pPed )
    {
        pPed->SetHasJetPack ( false );
    }
}


void CPedRPCs::RemovePedClothes ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucType;
    if ( bitStream.Read ( ucType ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
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


void CPedRPCs::SetPedGravity ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    float fGravity;
    if ( bitStream.Read ( fGravity ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
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


void CPedRPCs::SetPedChoking ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bChoking;
    if ( bitStream.ReadBit ( bChoking ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetChoking ( bChoking );
        }
    }
}


void CPedRPCs::SetPedFightingStyle ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucStyle;
    if ( bitStream.Read ( ucStyle ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetFightingStyle ( ( eFightingStyle ) ucStyle );
        }
    }
}


void CPedRPCs::SetPedMoveAnim ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned int uiMoveAnim;
    if ( bitStream.ReadCompressed ( uiMoveAnim ) )
    {
        CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetMoveAnim ( (eMoveAnim)uiMoveAnim );
        }
    }
}


void CPedRPCs::WarpPedIntoVehicle ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    ElementID VehicleID;
    unsigned char ucSeat;
    unsigned char ucTimeContext;
    if ( bitStream.ReadCompressed ( VehicleID ) &&
         bitStream.Read ( ucSeat ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
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


void CPedRPCs::RemovePedFromVehicle ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ucTimeContext ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            // Get the ped / player's occupied vehicle data before pulling it out
            CClientVehicle* pVehicle = pPed->GetOccupiedVehicle();

            // Make sure the vehicle exists (otherwise warping into specific vehicles will crash)
            if ( pVehicle )
            {
                unsigned int uiSeat = pPed->GetOccupiedVehicleSeat();

                pPed->SetSyncTimeContext ( ucTimeContext );

                // Remove the player from his vehicle
                pPed->RemoveFromVehicle ();
                pPed->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
                if ( pPed->m_bIsLocalPlayer )
                {
                    // Reset expectation of vehicle enter completion, in case we were removed while entering
                    g_pClientGame->ResetVehicleInOut ();
                }

                // Call onClientPlayerVehicleExit
                CLuaArguments Arguments;
                Arguments.PushElement ( pVehicle ); // vehicle
                Arguments.PushNumber ( uiSeat );    // seat
                Arguments.PushBoolean ( false );    // jacker
                pPed->CallEvent ( "onClientPlayerVehicleExit", Arguments, true );

                // Call onClientVehicleExit
                CLuaArguments Arguments2;
                Arguments2.PushElement ( pPed );   // player
                Arguments2.PushNumber ( uiSeat );  // seat
                pVehicle->CallEvent ( "onClientVehicleExit", Arguments2, true );
            }
        }
    }
}


void CPedRPCs::SetPedDoingGangDriveby ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    bool bDriveby;
    unsigned char ucTimeContext;
    if ( bitStream.ReadBit ( bDriveby ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetSyncTimeContext ( ucTimeContext );

            pPed->SetDoingGangDriveby ( bDriveby );
        }
    }
}


void CPedRPCs::SetPedAnimation ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the player and vehicle id
    char szBlockName [ 64 ], szAnimName [ 64 ];
    unsigned char ucBlockSize, ucAnimSize;
    int iTime;
    bool bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame;

    if ( bitStream.Read ( ucBlockSize ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
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
                         bitStream.ReadBit ( bInterruptable ) &&
                         bitStream.ReadBit ( bFreezeLastFrame ) )
                    {
                        szAnimName [ ucAnimSize ] = 0;

                        CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( szBlockName );
                        if ( pBlock )
                        {
                            pPed->RunNamedAnimation ( pBlock, szAnimName, iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame );
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


void CPedRPCs::SetPedOnFire ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bIsOnFire;

    if ( bitStream.ReadBit ( bIsOnFire ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetOnFire( bIsOnFire );
        }
    }
}


void CPedRPCs::SetPedHeadless ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bIsHeadless;

    if ( bitStream.ReadBit ( bIsHeadless ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetHeadless ( bIsHeadless );
        }
    }
}


void CPedRPCs::SetPedFrozen ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bIsFrozen = 0;

    if ( bitStream.ReadBit ( bIsFrozen ) )
    {
        // Grab the ped
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetFrozen ( bIsFrozen );
        }
    }
}


void CPedRPCs::ReloadPedWeapon ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    CClientPed* pPed = m_pPedManager->Get ( pSource->GetID (), true );
    if ( pPed )
    {
        pPed->ReloadWeapon ();            
    }
}
