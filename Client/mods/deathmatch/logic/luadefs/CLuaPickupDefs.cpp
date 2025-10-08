/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPickupDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaPickupDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createPickup", CreatePickup},

        {"getPickupType", GetPickupType}, {"getPickupWeapon", GetPickupWeapon}, {"getPickupAmount", GetPickupAmount}, {"getPickupAmmo", GetPickupAmmo},

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

int CLuaPickupDefs::CreatePickup(lua_State* luaVM)
{
    CVector          vecPosition;
    unsigned long    ulRespawnInterval = 30000;
    double           dblAmmo = 50.0;
    unsigned char    ucType = 0;
    double           dArgumentDependant = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(ucType);
    argStream.ReadNumber(dArgumentDependant);
    argStream.ReadNumber(ulRespawnInterval, 30000);
    argStream.ReadNumber(dblAmmo, 50.0);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CClientPickup* pPickup =
                    CStaticFunctionDefinitions::CreatePickup(*pResource, vecPosition, ucType, dArgumentDependant, ulRespawnInterval, dblAmmo);
                if (pPickup)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pPickup);
                    }

                    // Return the handle
                    lua_pushelement(luaVM, pPickup);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
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
