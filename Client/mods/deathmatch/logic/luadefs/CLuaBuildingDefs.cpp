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

    if (!g_pGame->GetPools()->HasFreeBuildingSlot())
        throw std::invalid_argument("No free slot in buildings pool");

    if (pos.fX < -3000.0f || pos.fX > 3000.0f || pos.fY < -3000.0f || pos.fY > 3000.0f)
        throw std::invalid_argument("Position is outside of game world");

    ConvertDegreesToRadians(rot);

    CClientBuilding* pBuilding = new CClientBuilding(m_pManager, INVALID_ELEMENT_ID, modelId, pos, rot, interior.value_or(0));

    CClientEntity* pRoot = pResource->GetResourceDynamicEntity();
    pBuilding->SetParent(pRoot);

    return pBuilding;
}
