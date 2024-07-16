/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPickupDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaPickupDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        {"createPickup", ArgumentParser<CreatePickup>},

        {"getPickupType", GetPickupType},
        {"getPickupWeapon", GetPickupWeapon},
        {"getPickupAmount", GetPickupAmount},
        {"getPickupAmmo", GetPickupAmmo},

        {"setPickupType", SetPickupType},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPickupDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createPickup");
    lua_classfunction(luaVM, "getAmmo", "getPickupAmmo");
    lua_classfunction(luaVM, "getAmount", "getPickupAmount");
    lua_classfunction(luaVM, "getWeapon", "getPickupWeapon");
    lua_classfunction(luaVM, "getType", "getPickupType");
    lua_classfunction(luaVM, "setType", "setPickupType");

    lua_classvariable(luaVM, "ammo", NULL, "getPickupAmmo");
    lua_classvariable(luaVM, "amount", NULL, "getPickupAmount");
    lua_classvariable(luaVM, "weapon", NULL, "getPickupWeapon");
    lua_classvariable(luaVM, "pickupType", "setPickupType", "getPickupType");

    lua_registerclass(luaVM, "Pickup", "Element");
}

std::variant<bool, CClientPickup*> CLuaPickupDefs::CreatePickup(lua_State* luaVM, CVector pos, std::uint8_t type, double argumentDependant, std::optional<std::uint32_t> interval, std::optional<double> ammo) noexcept
{
    if(!interval.has_value())
        interval = 30000;
    if(!ammo.has_value())
        ammo = 50.0;

    CResource* resource = &lua_getownerresource(luaVM);
    
    CClientPickup* pickup = CStaticFunctionDefinitions::CreatePickup(*resource, pos, type, argumentDependant, interval.value(), ammo.value());
    if (!pickup)
        return false;
        
    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(pickup);

    return pickup;
}

int CLuaPickupDefs::GetPickupType(lua_State* luaVM)
{
    CClientPickup*   pPickup = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        unsigned char ucType = pPickup->m_ucType;
        lua_pushnumber(luaVM, ucType);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::GetPickupWeapon(lua_State* luaVM)
{
    CClientPickup*   pPickup = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        unsigned char ucWeapon = pPickup->m_ucWeaponType;
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucWeapon));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::GetPickupAmount(lua_State* luaVM)
{
    CClientPickup*   pPickup = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        float fAmount = pPickup->m_fAmount;
        lua_pushnumber(luaVM, fAmount);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::GetPickupAmmo(lua_State* luaVM)
{
    CClientPickup*   pPickup = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        unsigned short usAmmo = pPickup->m_usAmmo;
        lua_pushnumber(luaVM, static_cast<lua_Number>(usAmmo));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::SetPickupType(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    double           dblAmmo = 50.0;
    unsigned char    ucType = 0;
    double           dArgumentDependant = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucType);
    argStream.ReadNumber(dArgumentDependant);
    argStream.ReadNumber(dblAmmo, 50.0);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Do it
        if (CStaticFunctionDefinitions::SetPickupType(*pEntity, ucType, dArgumentDependant, dblAmmo))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
