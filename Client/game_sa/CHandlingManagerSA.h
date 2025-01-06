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

    std::unique_ptr<CHandlingEntry>       CreateHandlingData() const noexcept;
    std::unique_ptr<CFlyingHandlingEntry> CreateFlyingHandlingData() const noexcept;
    std::unique_ptr<CBoatHandlingEntry>   CreateBoatHandlingData() const noexcept;
    std::unique_ptr<CBikeHandlingEntry>   CreateBikeHandlingData() const noexcept;

    const CHandlingEntry*       GetOriginalHandlingData(std::uint32_t model) const noexcept;
    const CFlyingHandlingEntry* GetOriginalFlyingHandlingData(std::uint32_t model) const noexcept;
    const CBoatHandlingEntry*   GetOriginalBoatHandlingData(std::uint32_t model) const noexcept;
    const CBikeHandlingEntry*   GetOriginalBikeHandlingData(std::uint32_t model) const noexcept;

    eHandlingProperty GetPropertyEnumFromName(const std::string& name) const noexcept;

    void CheckSuspensionChanges(const CHandlingEntry* const pEntry) const noexcept;

private:
    void InitializeDefaultHandlings() noexcept;

    eHandlingTypes GetHandlingID(std::uint32_t uiModel) const noexcept;
};
