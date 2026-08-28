/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/CServerModelDefinition.h
 *  PURPOSE:     Server-authoritative model definition structure
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <string>

// Logical model ID range allocated and managed by the server.
// Values below SERVER_MODEL_ID_MIN are reserved for vanilla GTA models and client-local DFF slots.
constexpr std::uint16_t SERVER_MODEL_ID_MIN = 42341;
constexpr std::uint16_t SERVER_MODEL_ID_MAX = 65534;
constexpr std::uint16_t INVALID_LOGICAL_MODEL_ID = 0xFFFF;

enum class eServerModelType : std::uint8_t
{
    OBJECT = 0,
    VEHICLE = 1,
    PED = 2,
};

struct SServerModelDefinition
{
    std::uint16_t    logicalModelId = INVALID_LOGICAL_MODEL_ID;
    std::uint16_t    parentModelId = 0;
    eServerModelType type = eServerModelType::OBJECT;
    std::string      name;
};
