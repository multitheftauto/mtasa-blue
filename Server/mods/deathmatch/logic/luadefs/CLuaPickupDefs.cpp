/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaPickupDefs.cpp
 *  PURPOSE:     Lua pickup definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaPickupDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"

void CLuaPickupDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        // Create/destroy
        {"createPickup", ArgumentParser<createPickup>},

        // Get
        {"getPickupType", getPickupType},
        {"getPickupWeapon", getPickupWeapon},
        {"getPickupAmount", getPickupAmount},
        {"getPickupAmmo", getPickupAmmo},
        {"getPickupRespawnInterval", getPickupRespawnInterval},
        {"isPickupSpawned", isPickupSpawned},

        // Set
        {"setPickupType", setPickupType},
        {"setPickupRespawnInterval", setPickupRespawnInterval},
        {"usePickup", usePickup},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPickupDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createPickup");
    lua_classfunction(luaVM, "use", "usePickup");

    lua_classfunction(luaVM, "getAmmo", "getPickupAmmo");
    lua_classfunction(luaVM, "getAmount", "getPickupAmount");
    lua_classfunction(luaVM, "getWeapon", "getPickupWeapon");
    lua_classfunction(luaVM, "getRespawnInterval", "getPickupRespawnInterval");
    lua_classfunction(luaVM, "getType", "getPickupType");
    lua_classfunction(luaVM, "setType", "setPickupType");
    lua_classfunction(luaVM, "setRespawnInterval", "setPickupRespawnInterval");

    lua_classvariable(luaVM, "ammo", NULL, "getPickupAmmo");
    lua_classvariable(luaVM, "amount", NULL, "getPickupAmount");
    lua_classvariable(luaVM, "spawned", NULL, "isPickupSpawned");
    lua_classvariable(luaVM, "weapon", NULL, "getPickupWeapon");
    lua_classvariable(luaVM, "type", "setPickupType", "getPickupType");
    lua_classvariable(luaVM, "respawnInterval", "setPickupRespawnInterval", "getPickupRespawnInterval");

    lua_registerclass(luaVM, "Pickup", "Element");
}

std::variant<bool, CPickup*> CLuaPickupDefs::createPickup(lua_State* luaVM, CVector pos, std::uint8_t type, double value, std::optional<std::uint32_t> interval,
                                                          std::optional<double> ammo) noexcept
{
    if (!interval.has_value())
        interval = 30000;

    if (!ammo.has_value())
        ammo = 50.0;

    CResource* resource = &lua_getownerresource(luaVM);

    CPickup* pickup = CStaticFunctionDefinitions::CreatePickup(resource, pos, type, value, interval.value(), ammo.value());
    if (!pickup)
        return false;

    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(pickup);

    // Return the handle
    return pickup;
}

int CLuaPickupDefs::getPickupType(lua_State* luaVM)
{
    //  int getPickupType ( pickup thePickup )
    CPickup* pPickup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    if (!argStream.HasErrors())
    {
        unsigned char ucType;
        if (CStaticFunctionDefinitions::GetPickupType(pPickup, ucType))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(ucType));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::getPickupWeapon(lua_State* luaVM)
{
    //  int getPickupWeapon ( pickup thePickup )
    CPickup* pPickup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    if (!argStream.HasErrors())
    {
        unsigned char ucWeapon;
        if (CStaticFunctionDefinitions::GetPickupWeapon(pPickup, ucWeapon))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(ucWeapon));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::getPickupAmount(lua_State* luaVM)
{
    //  int getPickupAmount ( pickup thePickup )
    CPickup* pPickup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    if (!argStream.HasErrors())
    {
        float fAmount;
        if (CStaticFunctionDefinitions::GetPickupAmount(pPickup, fAmount))
        {
            lua_pushnumber(luaVM, fAmount);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::getPickupAmmo(lua_State* luaVM)
{
    //  int getPickupAmmo ( pickup thePickup )
    CPickup* pPickup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    if (!argStream.HasErrors())
    {
        unsigned short usAmmo;
        if (CStaticFunctionDefinitions::GetPickupAmmo(pPickup, usAmmo))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(usAmmo));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::getPickupRespawnInterval(lua_State* luaVM)
{
    //  int getPickupRespawnInterval ( pickup thePickup )
    CPickup* pPickup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    if (!argStream.HasErrors())
    {
        unsigned long ulInterval;
        if (CStaticFunctionDefinitions::GetPickupRespawnInterval(pPickup, ulInterval))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(ulInterval));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::isPickupSpawned(lua_State* luaVM)
{
    //  bool isPickupSpawned ( pickup thePickup )
    CPickup* pPickup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPickup);

    if (!argStream.HasErrors())
    {
        bool bSpawned;
        if (CStaticFunctionDefinitions::IsPickupSpawned(pPickup, bSpawned))
        {
            lua_pushboolean(luaVM, bSpawned);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPickupDefs::setPickupType(lua_State* luaVM)
{
    //  bool setPickupType ( pickup thePickup, int theType, int amount/weapon/model, [ int ammo ] )
    CElement*     pElement;
    unsigned char ucType;
    double        dValue;
    double        dAmmo;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucType);
    argStream.ReadNumber(dValue);
    argStream.ReadNumber(dAmmo, 0.0);

    if (!argStream.HasErrors())
    {
        // Do it
        if (CStaticFunctionDefinitions::SetPickupType(pElement, ucType, dValue, dAmmo))
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

int CLuaPickupDefs::setPickupRespawnInterval(lua_State* luaVM)
{
    //  bool setPickupRespawnInterval ( pickup thePickup, int ms )
    CElement*     pElement;
    unsigned long ulInterval;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ulInterval);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPickupRespawnInterval(pElement, ulInterval))
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

int CLuaPickupDefs::usePickup(lua_State* luaVM)
{
    //  bool usePickup ( pickup thePickup, player thePlayer )
    CElement* pElement;
    CPlayer*  pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Do it
        if (CStaticFunctionDefinitions::UsePickup(pElement, pPlayer))
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
