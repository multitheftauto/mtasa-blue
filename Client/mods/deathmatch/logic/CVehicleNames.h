/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CVehicleNames.h
 *  PURPOSE:     Vehicle names class header
 *
 *****************************************************************************/

#pragma once

class CVehicleNames
{
public:
    static bool IsValidModel(std::uint32_t ulModel);
    static bool IsModelTrailer(std::uint32_t ulModel);

    static const char*  GetVehicleName(std::uint32_t ulModel);
    static std::uint32_t GetVehicleModel(const char* szName);

    static const char* GetVehicleTypeName(std::uint32_t ulModel);
};
