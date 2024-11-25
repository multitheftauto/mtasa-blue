/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CLodModels.h
 *
 *****************************************************************************/
#pragma once

#include <variant>

class CLodModels
{
public:
    static std::variant<bool, std::uint32_t> GetLowLODModel(std::uint32_t hLODModel) noexcept;
    static std::variant<bool, std::uint32_t> GetHighLODModel(std::uint32_t lLODModel) noexcept;

    static bool SetLowLODModel(std::uint32_t hLODModel, std::uint32_t lLODModel) noexcept;
    static bool ResetLowLODModel(std::uint32_t hLODModel) noexcept;

    static void ResetLowLODModels() noexcept;

private:
    // This map contains all HLOD Object Model ID -> LLOD Object Model ID associations
    // according to the game's IPL files (plaintext + binary).
    static const std::unordered_map<std::uint32_t, std::uint32_t> m_predefinedLODModels;
    static const std::unordered_map<std::uint32_t, std::uint32_t> m_reversePredefinedLODModels;

    // This map is for custom definitions of LLOD models for HLOD models.
    static std::unordered_map<std::uint32_t, std::uint32_t> m_customLODModels;
    static std::unordered_map<std::uint32_t, std::uint32_t> m_reverseCustomLODModels;

    static void UpdateReverseCustomLODModels() noexcept;
};
