/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CVehiclesConfig.cpp
 *  PURPOSE:     Vehicles XML configuration loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehiclesConfig.h"
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>

CVehiclesConfig::CVehiclesConfig(std::string filePath) : m_filePath(std::move(filePath))
{
}

bool CVehiclesConfig::Load()
{
    m_vehicles.clear();

    if (!g_pServerInterface || !g_pServerInterface->GetXML())
        return false;

    CXMLFile* xmlFile = g_pServerInterface->GetXML()->CreateXML(m_filePath.c_str());
    if (!xmlFile)
        return false;

    if (!xmlFile->Parse())
    {
        delete xmlFile;
        return false;
    }

    CXMLNode* rootNode = xmlFile->GetRootNode();
    if (!rootNode)
    {
        delete xmlFile;
        return false;
    }

    for (auto it = rootNode->ChildrenBegin(); it != rootNode->ChildrenEnd(); ++it)
    {
        CXMLNode* node = *it;
        if (!node || node->GetTagName() != "vehicle")
            continue;

        CXMLAttributes& attributes = node->GetAttributes();
        CXMLAttribute*  attribute = nullptr;

        VehicleConfigEntry entry;

        attribute = attributes.Find("modelID");
        if (!attribute)
            continue;
        entry.modelId = std::stoul(attribute->GetValue());

        attribute = attributes.Find("name");
        if (attribute)
            entry.name = attribute->GetValue();

        attribute = attributes.Find("type");
        if (attribute)
            entry.vehicleType = ParseVehicleType(attribute->GetValue());

        attribute = attributes.Find("attributes");
        if (attribute)
            entry.attributes = std::stoul(attribute->GetValue());

        attribute = attributes.Find("maxPassngers");
        if (attribute)
            entry.maxPassengers = static_cast<std::uint8_t>(std::stoul(attribute->GetValue()));

        attribute = attributes.Find("variantsCount");
        if (attribute)
        {
            int variants = std::stoi(attribute->GetValue());
            entry.variantsCount = (variants < 0) ? 255 : static_cast<std::uint8_t>(variants);
        }

        attribute = attributes.Find("hasDoors");
        if (attribute)
            entry.hasDoors = (attribute->GetValue() == "true");

        m_vehicles[entry.modelId] = entry;
    }

    delete xmlFile;
    return !m_vehicles.empty();
}

const VehicleConfigEntry* CVehiclesConfig::GetVehicleEntry(std::uint32_t modelId) const
{
    auto it = m_vehicles.find(modelId);
    return (it != m_vehicles.end()) ? &it->second : nullptr;
}

eVehicleType CVehiclesConfig::ParseVehicleType(std::string_view typeStr) const
{
    if (typeStr == "car")
        return VEHICLE_CAR;
    if (typeStr == "mtruck")
        return VEHICLE_MONSTERTRUCK;
    if (typeStr == "heli")
        return VEHICLE_HELI;
    if (typeStr == "boat")
        return VEHICLE_BOAT;
    if (typeStr == "plane")
        return VEHICLE_PLANE;
    if (typeStr == "bike")
        return VEHICLE_BIKE;
    if (typeStr == "bmx")
        return VEHICLE_BMX;
    if (typeStr == "quad")
        return VEHICLE_QUADBIKE;
    if (typeStr == "trailer")
        return VEHICLE_TRAILER;
    if (typeStr == "train")
        return VEHICLE_TRAIN;

    return VEHICLE_CAR;
}
