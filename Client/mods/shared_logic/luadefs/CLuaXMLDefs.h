/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaXMLDefs.cpp
*  PURPOSE:     Lua definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaXMLDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    LUA_DECLARE ( XMLNodeFindChild );
    LUA_DECLARE ( XMLNodeGetChildren );
    LUA_DECLARE ( XMLNodeGetValue );
    LUA_DECLARE ( XMLNodeSetValue );
    LUA_DECLARE ( XMLNodeGetAttributes );
    LUA_DECLARE ( XMLNodeGetAttribute );
    LUA_DECLARE ( XMLNodeSetAttribute );
    LUA_DECLARE ( XMLNodeGetParent );
    LUA_DECLARE ( XMLLoadFile );
    LUA_DECLARE ( XMLCreateFile );
    LUA_DECLARE ( XMLUnloadFile );
    LUA_DECLARE ( XMLSaveFile );
    LUA_DECLARE ( XMLCreateChild );
    LUA_DECLARE ( XMLDestroyNode );
    LUA_DECLARE ( XMLCopyFile );
    LUA_DECLARE ( XMLNodeGetName );
    LUA_DECLARE ( XMLNodeSetName );
};
