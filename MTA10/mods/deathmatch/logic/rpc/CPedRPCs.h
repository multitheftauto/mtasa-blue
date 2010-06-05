/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CPedRPCs.h
*  PURPOSE:     Header for ped RPC class
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPedRPCs_H
#define __CPedRPCs_H

#include "CRPCFunctions.h"

class CPedRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( SetPedArmor );
    DECLARE_RPC ( SetPedRotation );    
    DECLARE_RPC ( GivePedJetPack );
    DECLARE_RPC ( RemovePedJetPack );
    DECLARE_RPC ( RemovePedClothes );
    DECLARE_RPC ( SetPedGravity );
    DECLARE_RPC ( SetPedChoking );
    DECLARE_RPC ( SetPedFightingStyle );
    DECLARE_RPC ( SetPedMoveAnim );
    DECLARE_RPC ( WarpPedIntoVehicle );
    DECLARE_RPC ( RemovePedFromVehicle );
    DECLARE_RPC ( SetPedDoingGangDriveby );
    DECLARE_RPC ( SetPedAnimation );
    DECLARE_RPC ( SetPedOnFire );
    DECLARE_RPC ( SetPedHeadless );
    DECLARE_RPC ( SetPedFrozen );
    DECLARE_RPC ( ReloadPedWeapon );
};

#endif