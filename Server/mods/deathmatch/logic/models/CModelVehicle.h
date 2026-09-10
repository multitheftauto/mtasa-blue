/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelVehicle.h
 *  PURPOSE:     Vehicle model class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CModel.h"
#include "CVehiclesConfig.h"
#include <string>

class CModelVehicle : public CModel
{
public:
    CModelVehicle(std::uint32_t modelId, const VehicleConfigEntry& configEntry, std::uint32_t parentModelId = 0, CResource* resource = nullptr,
                  bool isCustom = false);

    const std::string& GetName() const noexcept { return m_name; }
    eVehicleType       GetVehicleType() const noexcept { return m_vehicleType; }
    unsigned long      GetAttributes() const noexcept { return m_attributes; }
    std::uint8_t       GetMaxPassengers() const noexcept { return m_maxPassengers; }
    std::uint8_t       GetVariantsCount() const noexcept { return m_variantsCount; }
    bool               HasDoors() const noexcept { return m_hasDoors; }

    void SetName(std::string name) { m_name = std::move(name); }
    void SetVehicleType(eVehicleType vehicleType) noexcept { m_vehicleType = vehicleType; }
    void SetAttributes(unsigned long attributes) noexcept { m_attributes = attributes; }
    void SetMaxPassengers(std::uint8_t maxPassengers) noexcept { m_maxPassengers = maxPassengers; }
    void SetVariantsCount(std::uint8_t variantsCount) noexcept { m_variantsCount = variantsCount; }
    void SetHasDoors(bool hasDoors) noexcept { m_hasDoors = hasDoors; }

private:
    std::string   m_name;
    eVehicleType  m_vehicleType{VEHICLE_CAR};
    unsigned long m_attributes{0};
    std::uint8_t  m_maxPassengers{1};
    std::uint8_t  m_variantsCount{255};
    bool          m_hasDoors{true};
};
