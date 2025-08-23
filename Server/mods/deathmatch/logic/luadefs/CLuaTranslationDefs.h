/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaTranslationDefs.h
 *  PURPOSE:     Shared Lua translation definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaTranslationDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    LUA_DECLARE(GetTranslation);
    LUA_DECLARE(GetAvailableTranslations);
};
