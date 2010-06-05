/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CWeaponRPCs.h
*  PURPOSE:     Header for weapon RPC calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWeaponRPCs_H
#define __CWeaponRPCs_H

#include "CRPCFunctions.h"

class CWeaponRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( GiveWeapon );
    DECLARE_RPC ( TakeWeapon );
    DECLARE_RPC ( TakeAllWeapons );
    DECLARE_RPC ( GiveWeaponAmmo );
    DECLARE_RPC ( TakeWeaponAmmo );
    DECLARE_RPC ( SetWeaponAmmo );
    DECLARE_RPC ( SetWeaponSlot );
};

#endif
