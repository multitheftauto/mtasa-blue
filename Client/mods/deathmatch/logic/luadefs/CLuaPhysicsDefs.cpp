/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPhysicsDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaPhysicsDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"physicsCreateWorld", PhysicsCreateWorld},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaPhysicsDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    //lua_classfunction(luaVM, "set", "PhysicsTestSet");
    //lua_classfunction(luaVM, "get", "PhysicsTestGet");


    lua_registerstaticclass(luaVM, "Physics");
}

int CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM)
{
    CClientPhysics* pPhysics = new CClientPhysics(m_pManager, INVALID_ELEMENT_ID);
    lua_pushelement(luaVM, pPhysics);
    return 1;
}
//
//int CLuaPhysicsDefs::PhysicsTestSet(lua_State* luaVM)
//{
//    CClientPhysics* pPhysics;
//    int bTest;
//    CScriptArgReader argStream(luaVM);
//    argStream.ReadUserData(pPhysics);
//    argStream.ReadNumber(bTest);
//
//    if (!argStream.HasErrors())
//    {
//        pPhysics->testint = bTest;
//        lua_pushboolean(luaVM, true);
//        return 1;
//    }
//    else
//        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
//
//    // Failed
//    lua_pushboolean(luaVM, false);
//    return 1;
//}
//
//int CLuaPhysicsDefs::PhysicsTestGet(lua_State* luaVM)
//{
//    CClientPhysics* pPhysics;
//    CScriptArgReader argStream(luaVM);
//    argStream.ReadUserData(pPhysics);
//
//    if (!argStream.HasErrors())
//    {
//        lua_pushnumber(luaVM, pPhysics->testint);
//        return 1;
//    }
//    else
//        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
//
//    // Failed
//    lua_pushboolean(luaVM, false);
//    return 1;
//}
