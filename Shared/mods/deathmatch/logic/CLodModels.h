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
    static std::variant<bool, std::uint32_t> GetModelLowLOD(std::uint32_t hLODModel) noexcept;
    static std::variant<bool, std::uint32_t> GetModelHighLOD(std::uint32_t lLODModel) noexcept;

    static bool SetModelLOD(std::uint32_t hLODModel, std::uint32_t lLODModel) noexcept;
    static bool ResetModelLODByHigh(std::uint32_t hLODModel) noexcept;
    static bool ResetModelLODByLow(std::uint32_t lLODModel) noexcept;

    static void ResetAllModelLOD() noexcept;

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
