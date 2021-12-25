/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CHandlingManager.h
 *  PURPOSE:     Vehicle handling manager interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CHandlingEntry.h"
#include "CFlyingHandlingEntry.h"
#include "CBoatHandlingEntry.h"
#include "CBikeHandlingEntry.h"

class CHandlingManager
{
public:
    virtual CHandlingEntry*       CreateHandlingData() = 0;
    virtual CFlyingHandlingEntry* CreateFlyingHandlingData() = 0;
    virtual CBoatHandlingEntry*   CreateBoatHandlingData() = 0;
    virtual CBikeHandlingEntry*   CreateBikeHandlingData() = 0;

    virtual const CHandlingEntry*       GetOriginalHandlingData(enum eVehicleTypes eModel) = 0;
    virtual const CFlyingHandlingEntry* GetOriginalFlyingHandlingData(enum eVehicleTypes eModel) = 0;
    virtual const CBoatHandlingEntry*   GetOriginalBoatHandlingData(enum eVehicleTypes eModel) = 0;
    virtual const CBikeHandlingEntry*   GetOriginalBikeHandlingData(enum eVehicleTypes eModel) = 0;

    virtual eHandlingProperty GetPropertyEnumFromName(std::string strName) = 0;
    virtual void              RemoveChangedVehicle() = 0;
    virtual void              CheckSuspensionChanges(CHandlingEntry* pEntry) = 0;
};
