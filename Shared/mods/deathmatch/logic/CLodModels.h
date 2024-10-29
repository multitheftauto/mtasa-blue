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
    static std::uint32_t GetObjectLODModel(std::uint32_t objectID)
    {
        const auto it = LOD_MODELS->find(objectID);
        if (it != LOD_MODELS->end())
        {
            return it->second;
        }
        return 0;
    };

private:
    // Static pointer to a map that is initialized in the .cpp file
    static std::unique_ptr<const std::map<std::uint32_t, std::uint32_t>> LOD_MODELS;
};
