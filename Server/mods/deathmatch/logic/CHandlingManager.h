/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CHandlingManager.h
 *  PURPOSE:     Header file for vehicle handling manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

class CHandlingManager;

#pragma once

#include "CHandlingEntry.h"
#include "CCommon.h"

class CHandlingManager
{
public:
    CHandlingManager();
    ~CHandlingManager();

    std::unique_ptr<CHandlingEntry> CreateHandlingData() const noexcept;

    bool ApplyHandlingData(std::uint32_t model, CHandlingEntry* pEntry) const noexcept;

    const CHandlingEntry* GetOriginalHandlingData(std::uint32_t model) const noexcept;
    CHandlingEntry*       GetModelHandlingData(std::uint32_t model) const noexcept;

    static eHandlingTypes GetHandlingID(std::uint32_t model) noexcept;

    // Helper functions
    eHandlingProperty GetPropertyEnumFromName(const std::string& name) const noexcept;
    static bool       HasModelHandlingChanged(std::uint32_t model) noexcept;
    static void       SetModelHandlingHasChanged(std::uint32_t model, bool changed) noexcept;

private:
    void InitializeDefaultHandlings() noexcept;
};
