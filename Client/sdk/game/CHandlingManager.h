/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/game/CHandlingManager.h
 *  PURPOSE:     Vehicle handling manager interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include "Common.h"

class CBikeHandlingEntry;
class CBoatHandlingEntry;
class CFlyingHandlingEntry;
class CHandlingEntry;

class CHandlingManager
{
public:
    virtual CHandlingEntry*       CreateHandlingData() = 0;
    virtual CFlyingHandlingEntry* CreateFlyingHandlingData() = 0;
    virtual CBoatHandlingEntry*   CreateBoatHandlingData() = 0;
    virtual CBikeHandlingEntry*   CreateBikeHandlingData() = 0;

    virtual const CHandlingEntry*       GetOriginalHandlingData(enum eVehicleTypes eModel) const = 0;
    virtual const CFlyingHandlingEntry* GetOriginalFlyingHandlingData(enum eVehicleTypes eModel) const = 0;
    virtual const CBoatHandlingEntry*   GetOriginalBoatHandlingData(enum eVehicleTypes eModel) const = 0;
    virtual const CBikeHandlingEntry*   GetOriginalBikeHandlingData(enum eVehicleTypes eModel) const = 0;

    virtual eHandlingProperty GetPropertyEnumFromName(const std::string& strName) const = 0;

    virtual void CheckSuspensionChanges(CHandlingEntry* pEntry) noexcept = 0;
};
