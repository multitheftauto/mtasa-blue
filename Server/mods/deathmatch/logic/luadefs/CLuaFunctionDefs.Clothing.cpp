/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Clothing.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaFunctionDefs.h"
#include "CScriptArgReader.h"
#include "CStaticFunctionDefinitions.h"

int CLuaFunctionDefs::GetBodyPartName(lua_State* luaVM)
{
    unsigned char ucID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucID);

    if (!argStream.HasErrors())
    {
        char szBuffer[256];
        if (CStaticFunctionDefinitions::GetBodyPartName(ucID, szBuffer))
        {
            lua_pushstring(luaVM, szBuffer);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetClothesByTypeIndex(lua_State* luaVM)
{
    unsigned char ucType;
    unsigned char ucIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucType);
    argStream.ReadNumber(ucIndex);

    if (!argStream.HasErrors())
    {
        char szTexture[128], szModel[128];
        if (CStaticFunctionDefinitions::GetClothesByTypeIndex(ucType, ucIndex, szTexture, szModel))
        {
            lua_pushstring(luaVM, szTexture);
            lua_pushstring(luaVM, szModel);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetTypeIndexFromClothes(lua_State* luaVM)
{
    SString strTexture;
    SString strModel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTexture);
    argStream.ReadString(strModel, "");

    if (!argStream.HasErrors())
    {
        unsigned char ucType, ucIndex;
        if (CStaticFunctionDefinitions::GetTypeIndexFromClothes(strTexture, strModel.empty() ? NULL : strModel.c_str(), ucType, ucIndex))
        {
            lua_pushnumber(luaVM, ucType);
            lua_pushnumber(luaVM, ucIndex);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetClothesTypeName(lua_State* luaVM)
{
    unsigned char ucType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucType);

    if (!argStream.HasErrors())
    {
        char szName[40];
        if (CStaticFunctionDefinitions::GetClothesTypeName(ucType, szName))
        {
            lua_pushstring(luaVM, szName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
