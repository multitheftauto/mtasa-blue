/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaBuildingDefs::LoadFunctions()
{
    // Backwards compatibility functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createBuilding", ArgumentParser<CreateBuilding>},
        {"removeAllGameBuildings", ArgumentParser<RemoveAllGameBuildings>},
        {"restoreAllGameBuildings", ArgumentParser<RestoreGameBuildings>},
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

CClientBuilding* CLuaBuildingDefs::CreateBuilding(lua_State* const luaVM, std::uint16_t modelId, CVector pos, std::optional<CVector> rot, std::optional<std::uint8_t> interior)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    // Get the resource we belong to
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return false;

    if (!CClientBuildingManager::IsValidModel(modelId))
        throw std::invalid_argument("Invalid building model id");

    if (!CClientBuildingManager::IsValidPosition(pos))
        throw std::invalid_argument("Position is outside of game world");

    if (rot.has_value())
        ConvertDegreesToRadians(rot.value());
    else
        rot.emplace(CVector(0, 0, 0));

    m_pBuildingManager->ResizePoolIfNeeds();

    CClientBuilding* pBuilding = new CClientBuilding(m_pManager, INVALID_ELEMENT_ID, modelId, pos, rot.value() , interior.value_or(0));

    CClientEntity* pRoot = pResource->GetResourceDynamicEntity();
    pBuilding->SetParent(pRoot);

    return pBuilding;
}

// Deprecated
void CLuaBuildingDefs::RemoveAllGameBuildings()
{
    CLuaWorldDefs::RemoveGameWorld();
}

// Deprecated
void CLuaBuildingDefs::RestoreGameBuildings()
{
    CLuaWorldDefs::RestoreGameWorld();
}
