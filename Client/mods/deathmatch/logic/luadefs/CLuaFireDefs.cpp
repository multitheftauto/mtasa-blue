/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaFireDefs.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include "CLuaFireDefs.h"

void CLuaFireDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createFire", CreateFire},
        {"extinguishFire", ExtinguishFire},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

int CLuaFireDefs::CreateFire(lua_State* luaVM)
{
    // bool createFire ( float x, float y, float z [, float size = 1.8 ] )
    CVector vecPosition;
    float   fSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fSize, 1.8f);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::CreateFire(vecPosition, fSize))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFireDefs::ExtinguishFire(lua_State* luaVM)
{
    // bool extinguishFire ( [ float x, float y, float z [, float radius = 1.0 ] ] )
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsNone())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::ExtinguishAllFires());
        return 1;
    }

    CVector vecPosition;
    float   fRadius;

    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRadius, 1.0f);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::ExtinguishFireInRadius(vecPosition, fRadius))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
