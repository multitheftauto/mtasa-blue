/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.BodyClothes.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GetBodyPartName(lua_State* luaVM)
{
    unsigned char    ucID = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucID);

    if (!argStream.HasErrors())
    {
        SString strBuffer;
        if (CStaticFunctionDefinitions::GetBodyPartName(ucID, strBuffer))
        {
            lua_pushstring(luaVM, strBuffer);
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
    unsigned char    ucType = 0;
    unsigned char    ucIndex = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucType);
    argStream.ReadNumber(ucIndex);

    if (!argStream.HasErrors())
    {
        SString strTexture, strModel;
        if (CStaticFunctionDefinitions::GetClothesByTypeIndex(ucType, ucIndex, strTexture, strModel))
        {
            lua_pushstring(luaVM, strTexture);
            lua_pushstring(luaVM, strModel);
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
    SString          strTexture = "", strModel = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTexture);
    argStream.ReadString(strModel, "");

    if (!argStream.HasErrors())
    {
        unsigned char ucType, ucIndex;
        if (CStaticFunctionDefinitions::GetTypeIndexFromClothes(strTexture, strModel == "" ? NULL : strModel.c_str(), ucType, ucIndex))
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
    unsigned char    ucType = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucType);

    if (!argStream.HasErrors())
    {
        SString strName;
        if (CStaticFunctionDefinitions::GetClothesTypeName(ucType, strName))
        {
            lua_pushstring(luaVM, strName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::AddClothingModel(lua_State* luaVM)
{
    std::uint8_t     clothingType = 0;
    SString          texture, model;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(texture);
    argStream.ReadString(model, "");
    argStream.ReadNumber(clothingType);

    if (!argStream.HasErrors())
    {
        if (CClientPlayerClothes::AddClothingModel(texture, model, clothingType))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::RemoveClothingModel(lua_State* luaVM)
{
    std::uint8_t     clothingType = 0;
    SString          texture, model;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(texture);
    argStream.ReadString(model, "");
    argStream.ReadNumber(clothingType);

    if (!argStream.HasErrors())
    {
        if (CClientPlayerClothes::RemoveClothingModel(texture, model, clothingType))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }

    lua_pushboolean(luaVM, false);
    return 1;
}
