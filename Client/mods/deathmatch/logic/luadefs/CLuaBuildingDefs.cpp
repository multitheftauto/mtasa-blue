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

CClientBuilding* CLuaBuildingDefs::CreateBuilding(lua_State* const luaVM, uint16_t modelId, CVector pos, CVector4D rot, uint8_t interior)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    // Get the resource we belong to
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return false;

    if (!CClientObjectManager::IsValidModel(modelId))
        throw std::invalid_argument("Invalid model id");

    // Grab the resource root entity
    CClientEntity* pRoot = pResource->GetResourceDynamicEntity();

    // Create the building handle
    CClientBuilding* pBuilding = new CClientBuilding(m_pManager, INVALID_ELEMENT_ID, modelId, pos, rot, interior);

    pBuilding->SetParent(pRoot);

    return pBuilding;
}
