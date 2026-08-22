/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelVehicle.cpp
 *  PURPOSE:     Vehicle model class implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelVehicle.h"

CModelVehicle::CModelVehicle(std::uint32_t modelId, const VehicleConfigEntry& configEntry, std::uint32_t parentModelId, CResource* resource, bool isCustom)
    : CModel(modelId, eModelType::VEHICLE, parentModelId, resource, isCustom),
      m_name(configEntry.name),
      m_vehicleType(configEntry.vehicleType),
      m_attributes(configEntry.attributes),
      m_maxPassengers(configEntry.maxPassengers),
      m_variantsCount(configEntry.variantsCount),
      m_hasDoors(configEntry.hasDoors)
{
}
