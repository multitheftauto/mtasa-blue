/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaTranslationDefsClient.h
 *  PURPOSE:     Client-only Lua translation definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaTranslationDefsClient : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    LUA_DECLARE(SetCurrentTranslationLanguage);
    LUA_DECLARE(GetTranslation);
    LUA_DECLARE(GetCurrentTranslationLanguage);
    LUA_DECLARE(GetAvailableTranslations);
};
