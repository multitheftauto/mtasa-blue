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

    CHandlingEntry* CreateHandlingData() const noexcept;

    bool ApplyHandlingData(std::uint32_t uiModel, CHandlingEntry* pEntry) const noexcept;

    const CHandlingEntry* GetModelHandlingData(std::uint32_t uiModel) const noexcept;
    const CHandlingEntry* GetOriginalHandlingData(std::uint32_t uiModel) const noexcept;

    eHandlingTypes GetHandlingID(std::uint32_t uiModel) const noexcept;

    // Helper functions
    eHandlingProperty GetPropertyEnumFromName(const std::string& strName) const noexcept;
    bool              HasModelHandlingChanged(std::uint32_t uiModel) const noexcept;
    void              SetModelHandlingHasChanged(std::uint32_t uiModel, bool bChanged) noexcept;

    std::map<std::string, eHandlingProperty> m_HandlingNames;

private:
    void InitializeDefaultHandlings() noexcept;

    // Original handling data unaffected by handling.cfg changes
    static SFixedArray<tHandlingData, HT_MAX> m_OriginalHandlingData;

    // Array with the original handling entries
    static SFixedArray<CHandlingEntry*, HT_MAX> m_pOriginalEntries;

    // Array with the model handling entries
    static SFixedArray<CHandlingEntry*, HT_MAX> m_pModelEntries;

    SFixedArray<bool, HT_MAX> m_bModelHandlingChanged;
};
