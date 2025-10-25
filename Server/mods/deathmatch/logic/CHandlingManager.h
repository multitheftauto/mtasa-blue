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
    ~CHandlingManager() {}

    static std::unique_ptr<CHandlingEntry> CreateHandlingData();

    bool ApplyHandlingData(std::uint32_t model, CHandlingEntry* pEntry) noexcept;

    const CHandlingEntry* GetOriginalHandlingData(std::uint32_t model) const noexcept;
    CHandlingEntry*       GetModelHandlingData(std::uint32_t model) noexcept;

    static eHandlingTypes GetHandlingID(std::uint32_t model) noexcept;

    // Helper functions
    eHandlingProperty GetPropertyEnumFromName(const std::string& name) const noexcept;
    bool              HasModelHandlingChanged(std::uint32_t model) const noexcept;
    void              SetModelHandlingHasChanged(std::uint32_t model, bool changed) noexcept;

private:
    void InitializeDefaultHandlings() noexcept;

    // Original handling data
    tHandlingData                   m_originalHandlingData[HT_MAX];
    std::unique_ptr<CHandlingEntry> m_originalEntries[HT_MAX];

    // Model handling data
    std::unordered_map<std::uint32_t, std::unique_ptr<CHandlingEntry>> m_modelEntries;
    std::unordered_map<std::uint32_t, bool>                            m_modelHandlingChanged;

    std::map<std::string, eHandlingProperty> m_handlingNames;
};
