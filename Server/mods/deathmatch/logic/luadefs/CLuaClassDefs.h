/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaClasses.h
 *  PURPOSE:     Lua general class functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class CLuaClassDefs : public CLuaDefs
{
public:
    LUA_DECLARE(Index);
    LUA_DECLARE(NewIndex);
    LUA_DECLARE(StaticNewIndex);
    LUA_DECLARE(Call);

    LUA_DECLARE(ReadOnly);
    LUA_DECLARE(WriteOnly);

    LUA_DECLARE(ToString);

    static constexpr const char* GetLuaClassName(CResource*) { return "Resource"; }
    static constexpr const char* GetLuaClassName(CLuaTimer*) { return "Timer"; }
    static constexpr const char* GetLuaClassName(CXMLNode*) { return "XML"; }
    static constexpr const char* GetLuaClassName(CAccessControlList*) { return "ACL"; }
    static constexpr const char* GetLuaClassName(CAccessControlListGroup*) { return "ACLGroup"; }
    static constexpr const char* GetLuaClassName(CAccount*) { return "Account"; }
    static constexpr const char* GetLuaClassName(CTextDisplay*) { return "TextDisplay"; }
    static constexpr const char* GetLuaClassName(CTextItem*) { return "TextItem"; }
    static constexpr const char* GetLuaClassName(CBan*) { return "Ban"; }
    static constexpr const char* GetLuaClassName(CDbJobData*) { return "QueryHandle"; }
    static const char* GetLuaClassName(CElement* pElement);
};
