/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleNames.h
 *  PURPOSE:     GTA vehicle name definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CVehicleNames
{
public:
    static bool IsValidModel(std::uint32_t ulModel);

    static const char*  GetVehicleName(std::uint32_t ulModel);
    static std::uint32_t GetVehicleModel(const char* szName);

    static const char* GetVehicleTypeName(std::uint32_t ulModel);
};
