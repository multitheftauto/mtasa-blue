/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Output.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::CreateTrayNotification(lua_State* luaVM)
{
    // bool createTrayNotification ( string text [, string type = "default", bool sound = true ] )
    SString          strText = "";
    eTrayIconType    eType;
    bool             useSound = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strText);
    argStream.ReadEnumString(eType, ICON_TYPE_DEFAULT);
    argStream.ReadBool(useSound, true);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::CreateTrayNotification(strText, eType, useSound));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::IsTrayNotificationEnabled(lua_State* luaVM)
{
    // bool isTrayNotificationEnabled ( )
    lua_pushboolean(luaVM, CStaticFunctionDefinitions::IsTrayNotificationEnabled());
    return 1;
}
