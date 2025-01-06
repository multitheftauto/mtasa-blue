/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaObjectDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaBuildingDefs.h"
#include "Utils.h"
#include "CDummy.h"
#include <packets/CEntityAddPacket.h>

void CLuaBuildingDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createBuilding", ArgumentParser<CreateBuilding>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaBuildingDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createBuilding");

    lua_registerclass(luaVM, "Building", "Element");
}

CBuilding* CLuaBuildingDefs::CreateBuilding(lua_State* const luaVM, std::uint16_t modelId, CVector pos, std::optional<CVector> rot,
                                                  std::optional<std::uint8_t> interior)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    // Get the resource we belong to
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        throw std::logic_error("Cannot be done in current environment");

    const CMtaVersion& minClientVersion = pResource->GetMinClientFromMetaXml();

    if (minClientVersion < CMtaVersion(SERVERSIDE_BUILDING_MIN_CLIENT_VERSION))
        throw std::logic_error("Expected client min_mta_version in meta.xml hinger or equal than " SERVERSIDE_BUILDING_MIN_CLIENT_VERSION);

    if (!CBuildingManager::IsValidModel(modelId))
        throw std::invalid_argument("Invalid building model id");

    if (!CBuildingManager::IsValidPosition(pos))
        throw std::invalid_argument("Position is outside of game world");

    if (rot.has_value())
        ConvertDegreesToRadians(rot.value());
    else
        rot.emplace(CVector(0, 0, 0));

    CBuilding* pBuilding = m_pBuildingManager->Create(pResource->GetDynamicElementRoot());

    if (!pBuilding)
        return nullptr;

    pBuilding->SetPosition(pos);
    pBuilding->SetRotation(rot.value());
    pBuilding->SetModel(modelId);

    if (pResource->IsClientSynced())
    {
        CEntityAddPacket Packet;
        Packet.Add(pBuilding);
        m_pPlayerManager->BroadcastOnlyJoined(Packet);
    }

    return pBuilding;
}
