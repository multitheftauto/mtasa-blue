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
    virtual CHandlingEntry*       CreateHandlingData() const noexcept = 0;
    virtual CFlyingHandlingEntry* CreateFlyingHandlingData() const noexcept = 0;
    virtual CBoatHandlingEntry*   CreateBoatHandlingData() const noexcept = 0;
    virtual CBikeHandlingEntry*   CreateBikeHandlingData() const noexcept = 0;

    virtual const CHandlingEntry*       GetOriginalHandlingData(std::uint32_t uiModel) const noexcept = 0;
    virtual const CFlyingHandlingEntry* GetOriginalFlyingHandlingData(std::uint32_t uiModel) const noexcept = 0;
    virtual const CBoatHandlingEntry*   GetOriginalBoatHandlingData(std::uint32_t uiModel) const noexcept = 0;
    virtual const CBikeHandlingEntry*   GetOriginalBikeHandlingData(std::uint32_t uiModel) const noexcept = 0;

    virtual eHandlingProperty GetPropertyEnumFromName(const std::string& strName) const noexcept = 0;

    virtual void CheckSuspensionChanges(const CHandlingEntry* pEntry) const noexcept = 0;
};
