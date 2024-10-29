/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
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
    static std::uint32_t GetObjectLODModel(std::uint32_t objectID);
    static std::uint32_t GetObjectModelOfLOD(std::uint32_t lodModelID);

private:
    // Static pointer to a map that is initialized in the .cpp file
    static std::unique_ptr<const std::map<std::uint32_t, std::uint32_t>> LOD_MODELS;
};
