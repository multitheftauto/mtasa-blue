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

    eHandlingTypes GetHandlingID(std::uint32_t model) const noexcept;

    // Helper functions
    eHandlingProperty GetPropertyEnumFromName(const std::string& name) const noexcept;
    bool              HasModelHandlingChanged(std::uint32_t model) const noexcept;
    void              SetModelHandlingHasChanged(std::uint32_t model, bool bChanged) noexcept;

    std::map<std::string, eHandlingProperty> m_HandlingNames;

private:
    void InitializeDefaultHandlings() noexcept;

    // Original handling data unaffected by handling.cfg changes
    static SFixedArray<tHandlingData, HT_MAX> m_OriginalHandlingData;

    // Array with the original handling entries
    static SFixedArray<std::unique_ptr<CHandlingEntry>, HT_MAX> m_OriginalEntries;

    // Array with the model handling entries
    static SFixedArray<std::unique_ptr<CHandlingEntry>, HT_MAX> m_ModelEntries;

    SFixedArray<bool, HT_MAX> m_bModelHandlingChanged;
};
