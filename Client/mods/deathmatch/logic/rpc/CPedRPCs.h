/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CPedRPCs.h
 *  PURPOSE:     Header for ped RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CPedRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_ELEMENT_RPC(SetPedArmor);
    DECLARE_ELEMENT_RPC(SetPedRotation);
    DECLARE_ELEMENT_RPC(GivePedJetPack);
    DECLARE_ELEMENT_RPC(RemovePedJetPack);
    DECLARE_ELEMENT_RPC(RemovePedClothes);
    DECLARE_ELEMENT_RPC(SetPedGravity);
    DECLARE_ELEMENT_RPC(SetPedChoking);
    DECLARE_ELEMENT_RPC(SetPedFightingStyle);
    DECLARE_ELEMENT_RPC(SetPedMoveAnim);
    DECLARE_ELEMENT_RPC(WarpPedIntoVehicle);
    DECLARE_ELEMENT_RPC(RemovePedFromVehicle);
    DECLARE_ELEMENT_RPC(SetPedDoingGangDriveby);
    DECLARE_ELEMENT_RPC(SetPedAnimation);
    DECLARE_ELEMENT_RPC(SetPedAnimationProgress);
    DECLARE_ELEMENT_RPC(SetPedAnimationSpeed);
    DECLARE_ELEMENT_RPC(SetPedOnFire);
    DECLARE_ELEMENT_RPC(SetPedHeadless);
    DECLARE_ELEMENT_RPC(SetPedFrozen);
    DECLARE_ELEMENT_RPC(ReloadPedWeapon);
};
