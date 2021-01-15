/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaVectorGraphicDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"

void CLuaVectorGraphicDefs::LoadFunctions()
{
   constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"svgCreate", ArgumentParser<SVGCreate>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaVectorGraphicDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "svgCreate");

    lua_registerclass(luaVM, "SVG");
}

bool CLuaVectorGraphicDefs::SVGCreate(lua_State* luaVM, uint width, uint height)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pParentResource = pLuaMain->GetResource();

        CClientVectorGraphic* pVectorGraphic = g_pClientGame->GetManager()->GetRenderElementManager()->CreateVectorGraphic(width, height);
        if (pVectorGraphic)
        {
            // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
            pVectorGraphic->SetParent(pParentResource->GetResourceDynamicEntity());

            // Set our owner resource
            pVectorGraphic->SetResource(pParentResource);

            return true;
        }

        return false;
    }

    return false;
}
