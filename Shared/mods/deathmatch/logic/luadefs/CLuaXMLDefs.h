/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaSharedXMLDefs.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CLuaXMLDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(xmlCreateFile);
    LUA_DECLARE(xmlLoadFile);
    LUA_DECLARE(xmlLoadString);
    LUA_DECLARE(xmlCopyFile);
    LUA_DECLARE(xmlSaveFile);
    LUA_DECLARE(xmlUnloadFile);
    LUA_DECLARE(xmlCreateChild);
    LUA_DECLARE(xmlDestroyNode);
    LUA_DECLARE(xmlNodeFindChild);

    LUA_DECLARE(xmlNodeGetChildren);
    LUA_DECLARE(xmlNodeGetParent);
    LUA_DECLARE(xmlNodeGetValue);
    LUA_DECLARE(xmlNodeGetAttributes);
    LUA_DECLARE(xmlNodeGetAttribute);
    LUA_DECLARE(xmlNodeGetName);

    LUA_DECLARE(xmlNodeSetValue);
    LUA_DECLARE(xmlNodeSetAttribute);
    LUA_DECLARE(xmlNodeSetName);
};
