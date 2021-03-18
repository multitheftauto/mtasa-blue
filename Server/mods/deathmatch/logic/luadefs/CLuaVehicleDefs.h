/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaVehicleDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <string_view>

class CLuaVehicleDefs : public CLuaDefs
{
public:
    // Functions are defined in the source file.
    // The header is way more readable, and it compiles faster

    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Has to be here because of CLuaCompatibilityDefs
    static std::variant<std::string_view, bool> GetVehicleNameFromModel(size_t model);
    static std::variant<bool, unsigned short> GetVehicleModelFromName(CVehicle* pVehicle, std::string_view name);
};
