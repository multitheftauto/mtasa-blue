/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CElementRPCs.h
 *  PURPOSE:     Header for element RPC class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CElementRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_ELEMENT_RPC(SetElementParent);
    DECLARE_ELEMENT_RPC(SetElementData);
    DECLARE_ELEMENT_RPC(RemoveElementData);
    DECLARE_ELEMENT_RPC(SetElementPosition);
    DECLARE_ELEMENT_RPC(SetElementVelocity);
    DECLARE_ELEMENT_RPC(SetElementAngularVelocity);
    DECLARE_ELEMENT_RPC(SetElementInterior);
    DECLARE_ELEMENT_RPC(SetElementDimension);
    DECLARE_ELEMENT_RPC(AttachElements);
    DECLARE_ELEMENT_RPC(DetachElements);
    DECLARE_ELEMENT_RPC(SetElementAlpha);
    DECLARE_ELEMENT_RPC(SetElementName);
    DECLARE_ELEMENT_RPC(SetElementHealth);
    DECLARE_ELEMENT_RPC(SetElementModel);
    DECLARE_ELEMENT_RPC(SetElementAttachedOffsets);
    DECLARE_ELEMENT_RPC(SetElementDoubleSided);
    DECLARE_ELEMENT_RPC(SetElementCollisionsEnabled);
    DECLARE_ELEMENT_RPC(SetElementFrozen);
    DECLARE_ELEMENT_RPC(SetLowLodElement);
    DECLARE_ELEMENT_RPC(FireCustomWeapon);
    DECLARE_ELEMENT_RPC(SetCustomWeaponState);
    DECLARE_ELEMENT_RPC(SetCustomWeaponClipAmmo);
    DECLARE_ELEMENT_RPC(SetCustomWeaponAmmo);
    DECLARE_ELEMENT_RPC(SetCustomWeaponTarget);
    DECLARE_ELEMENT_RPC(ResetCustomWeaponTarget);
    DECLARE_ELEMENT_RPC(SetCustomWeaponFlags);
    DECLARE_ELEMENT_RPC(SetCustomWeaponFiringRate);
    DECLARE_ELEMENT_RPC(ResetCustomWeaponFiringRate);
    DECLARE_ELEMENT_RPC(SetWeaponOwner);
    DECLARE_ELEMENT_RPC(SetWeaponConfig);
    DECLARE_ELEMENT_RPC(SetCallPropagationEnabled);
    DECLARE_ELEMENT_RPC(SetColPolygonHeight);
    DECLARE_ELEMENT_RPC(SetElementOnFire);
};
