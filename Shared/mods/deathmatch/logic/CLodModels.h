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
    static std::uint32_t GetObjectLODOfModel(std::uint32_t objectID) noexcept;
    static std::uint32_t GetObjectModelOfLOD(std::uint32_t lodID) noexcept;

private:
    using lod_type_t = std::unordered_map<std::uint32_t, std::uint32_t>;
    static const std::unique_ptr<lod_type_t> LOD_MODELS;
};
