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
        // Audio funcs
        {"svgTest", TestFunc},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaVectorGraphicDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "test", "svgTest");

    lua_registerclass(luaVM, "SVG");
}

int CLuaVectorGraphicDefs::TestFunc(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    bool bBool = false;
    argStream.ReadBool(bBool);

    if (!argStream.HasErrors())
    {
        bool bTest = CStaticFunctionDefinitions::SvgTest(bBool);
        lua_pushboolean(luaVM, bTest);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
