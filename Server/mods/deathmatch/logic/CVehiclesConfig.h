/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CVehiclesConfig.h
 *  PURPOSE:     Vehicles XML configuration loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include "CCommon.h"
#include "CVehicle.h"

struct VehicleConfigEntry
{
    std::uint32_t modelId{0};
    std::string   name;
    eVehicleType  vehicleType{VEHICLE_CAR};
    unsigned long attributes{0};
    std::uint8_t  maxPassengers{1};
    std::uint8_t  variantsCount{255};
    bool          hasDoors{true};
};

class CVehiclesConfig
{
public:
    explicit CVehiclesConfig(std::string filePath);

    bool Load();

    const VehicleConfigEntry* GetVehicleEntry(std::uint32_t modelId) const;

private:
    eVehicleType ParseVehicleType(std::string_view typeStr) const;

    std::string                                           m_filePath;
    std::unordered_map<std::uint32_t, VehicleConfigEntry> m_vehicles;
};
