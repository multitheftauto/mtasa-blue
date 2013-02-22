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

    DECLARE_ELEMENT_RPC ( GiveWeapon );
    DECLARE_ELEMENT_RPC ( TakeWeapon );
    DECLARE_ELEMENT_RPC ( TakeAllWeapons );
    DECLARE_ELEMENT_RPC ( GiveWeaponAmmo );
    DECLARE_ELEMENT_RPC ( TakeWeaponAmmo );
    DECLARE_ELEMENT_RPC ( SetWeaponAmmo );
    DECLARE_ELEMENT_RPC ( SetWeaponSlot );
};

#endif
