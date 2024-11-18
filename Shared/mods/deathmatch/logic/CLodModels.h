/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CLodModels.h
 *
 *****************************************************************************/
#pragma once

#include <map>
#include <memory>
#include <cstdint>

class CLodModels
{
public:
    static std::uint32_t GetObjectLowLODModel(std::uint32_t hLODModel) noexcept;
    static std::uint32_t GetObjectHighLODModel(std::uint32_t lLODModel) noexcept;

    static void SetObjectCustomLowLODModel(std::uint32_t hLODModel, std::uint32_t lLODModel) noexcept;
    static void  ResetObjectCustomLowLODModel(std::uint32_t hLODModel) noexcept;
    static void  ResetAllObjectCustomLowLODModels() noexcept;

private:
    static std::unordered_map<std::uint32_t, std::uint32_t> m_customLODModels;
};
