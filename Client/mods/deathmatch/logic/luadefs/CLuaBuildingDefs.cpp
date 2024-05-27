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

    lua_registerclass(luaVM, "Building");
}

CClientBuilding* CLuaBuildingDefs::CreateBuilding(lua_State* const luaVM, uint16_t modelId, CVector pos, CVector rot, std::optional<uint8_t> interior)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    // Get the resource we belong to
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return false;

    if (!CClientBuildingManager::IsValidModel(modelId))
        throw std::invalid_argument("Invalid building model id");

    if (!g_pGame->GetPools()->GetBuildingsPool().HasFreeBuildingSlot())
        throw std::invalid_argument("No free slot in buildings pool");

    if (!CClientBuildingManager::IsValidPosition(pos))
        throw std::invalid_argument("Position is outside of game world");

    ConvertDegreesToRadians(rot);

    CClientBuilding* pBuilding = new CClientBuilding(m_pManager, INVALID_ELEMENT_ID, modelId, pos, rot, interior.value_or(0));

    CClientEntity* pRoot = pResource->GetResourceDynamicEntity();
    pBuilding->SetParent(pRoot);

    return pBuilding;
}

void CLuaBuildingDefs::RemoveAllGameBuildings()
{
    // We do not want to remove scripted buildings
    // But we need remove them from the buildings pool for a bit...
    m_pBuildingManager->DestroyAllForABit();

    // This function makes buildings backup without scripted buildings
    g_pGame->RemoveAllBuildings();

    // ... And restore here
    m_pBuildingManager->RestoreDestroyed();
}

void CLuaBuildingDefs::RestoreGameBuildings()
{
    // We want to restore the game buildings to the same positions as they were before the backup.
    // Remove scripted buildings for a bit
    m_pBuildingManager->DestroyAllForABit();

    g_pGame->RestoreGameBuildings();

    // Restore what we can
    m_pBuildingManager->RestoreDestroyed();
}
