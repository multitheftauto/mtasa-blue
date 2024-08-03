/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CHandlingManagerSA.h
 *  PURPOSE:     Header file for vehicle handling manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHandlingManager.h>
#include "CHandlingEntrySA.h"
#include "CFlyingHandlingEntrySA.h"
#include "CBoatHandlingEntrySA.h"
#include "CBikeHandlingEntrySA.h"

class CHandlingManagerSA : public CHandlingManager
{
public:
    CHandlingManagerSA();
    ~CHandlingManagerSA();

    CHandlingEntry*       CreateHandlingData();
    CFlyingHandlingEntry* CreateFlyingHandlingData();
    CBoatHandlingEntry*   CreateBoatHandlingData();
    CBikeHandlingEntry*   CreateBikeHandlingData();

    const CHandlingEntry*       GetOriginalHandlingData(eVehicleTypes eModel) const;
    const CFlyingHandlingEntry* GetOriginalFlyingHandlingData(eVehicleTypes eModel) const;
    const CBoatHandlingEntry*   GetOriginalBoatHandlingData(eVehicleTypes eModel) const;
    const CBikeHandlingEntry*   GetOriginalBikeHandlingData(eVehicleTypes eModel) const;

    eHandlingProperty GetPropertyEnumFromName(const std::string& strName) const;

    void CheckSuspensionChanges(CHandlingEntry* pEntry) noexcept;

private:
    void InitializeDefaultHandlings();

    eHandlingTypes GetHandlingID(eVehicleTypes eModel) const;
};
