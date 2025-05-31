/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaFileDefs.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CLuaFileDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

private:
    LUA_DECLARE(File);
    LUA_DECLARE(fileOpen);
    LUA_DECLARE(fileCreate);
    LUA_DECLARE(fileExists);
    LUA_DECLARE(fileCopy);
    LUA_DECLARE(fileRename);
    LUA_DECLARE(fileDelete);

    LUA_DECLARE(fileClose);
    LUA_DECLARE(fileFlush);
    LUA_DECLARE(fileRead);
    LUA_DECLARE(fileWrite);
    static std::optional<std::string> fileGetContents(lua_State* L, CScriptFile* scriptFile, std::optional<bool> maybeVerifyContents);

    LUA_DECLARE(fileGetPos);
    LUA_DECLARE(fileGetSize);
    LUA_DECLARE(fileGetPath);
    LUA_DECLARE(fileIsEOF);

    LUA_DECLARE(fileSetPos);

    LUA_DECLARE(fileCloseGC);
};
