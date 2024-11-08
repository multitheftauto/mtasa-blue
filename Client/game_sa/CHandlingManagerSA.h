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

    CHandlingEntry*       CreateHandlingData() const noexcept;
    CFlyingHandlingEntry* CreateFlyingHandlingData() const noexcept;
    CBoatHandlingEntry*   CreateBoatHandlingData() const noexcept;
    CBikeHandlingEntry*   CreateBikeHandlingData() const noexcept;

    const CHandlingEntry*       GetOriginalHandlingData(std::uint32_t uiModel) const noexcept;
    const CFlyingHandlingEntry* GetOriginalFlyingHandlingData(std::uint32_t uiModel) const noexcept;
    const CBoatHandlingEntry*   GetOriginalBoatHandlingData(std::uint32_t uiModel) const noexcept;
    const CBikeHandlingEntry*   GetOriginalBikeHandlingData(std::uint32_t uiModel) const noexcept;

    eHandlingProperty GetPropertyEnumFromName(const std::string& strName) const noexcept;

    void CheckSuspensionChanges(CHandlingEntry* pEntry) const noexcept;

private:
    void InitializeDefaultHandlings() noexcept;

    eHandlingTypes GetHandlingID(std::uint32_t uiModel) const noexcept;
};
