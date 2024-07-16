/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaProjectileDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaProjectileDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        {"createProjectile", ArgumentParser<CreateProjectile>},

        {"getProjectileType", GetProjectileType},
        {"getProjectileTarget", GetProjectileTarget},
        {"getProjectileCreator", GetProjectileCreator},
        {"getProjectileForce", GetProjectileForce},
        {"getProjectileCounter", GetProjectileCounter},

        {"setProjectileCounter", SetProjectileCounter},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaProjectileDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createProjectile");
    lua_classfunction(luaVM, "getCreator", "getProjectileCreator");
    lua_classfunction(luaVM, "getForce", "getProjectileForce");
    lua_classfunction(luaVM, "getTarget", "getProjectileTarget");
    lua_classfunction(luaVM, "getType", "getProjectileType");
    lua_classfunction(luaVM, "getCounter", "getProjectileCounter");
    lua_classfunction(luaVM, "setCounter", "setProjectileCounter");

    lua_classvariable(luaVM, "creator", NULL, "getProjectileCreator");
    lua_classvariable(luaVM, "force", NULL, "getProjectileForce");
    lua_classvariable(luaVM, "target", NULL, "getProjectileTarget");
    lua_classvariable(luaVM, "type", NULL, "getProjectileType");
    lua_classvariable(luaVM, "counter", "setProjectileCounter", "getProjectileCounter");

    lua_registerclass(luaVM, "Projectile", "Element");
}

std::variant<bool, CClientProjectile*> CLuaProjectileDefs::CreateProjectile(lua_State* luaVM, CClientEntity* creator, std::uint8_t type,
                                                                            std::optional<CVector> origin, std::optional<float> force,
                                                                            std::optional<CClientEntity*> target, std::optional<CVector> rot,
                                                                            std::optional<CVector> speed, std::optional<std::uint16_t> model) noexcept
{
    // TODO: Refactor CClientEntity class
    // GetPosition should RETURN a value
    if (!origin.has_value())
    {
        CVector temp;
        creator->GetPosition(temp);
        origin = temp;
    }
    if (!force.has_value())
        force = 1.0f;
    if (!target.has_value())
        target = nullptr;
    if (!rot.has_value())
        rot = CVector();
    if (!speed.has_value())
        speed = CVector();
    if (!model.has_value())
        model = 0;

    CResource* resource = &lua_getownerresource(luaVM);

    CClientProjectile* projectile = CStaticFunctionDefinitions::CreateProjectile(*resource, *creator, type, origin.value(), force.value(), target.value(),
        rot.value(), speed.value(), model.value());
        
    if (!projectile)
        return false;

    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(projectile);

    return projectile;
}

int CLuaProjectileDefs::GetProjectileType(lua_State* luaVM)
{
    CClientProjectile* pProjectile = NULL;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pProjectile);

    if (!argStream.HasErrors())
    {
        unsigned char ucWeapon = pProjectile->GetWeaponType();
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucWeapon));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaProjectileDefs::GetProjectileTarget(lua_State* luaVM)
{
    CClientProjectile* pProjectile = NULL;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pProjectile);

    if (!argStream.HasErrors())
    {
        unsigned char ucWeapon = pProjectile->GetWeaponType();
        if (ucWeapon == WEAPONTYPE_ROCKET_HS)
        {
            lua_pushelement(luaVM, pProjectile->GetTargetEntity());
            return 1;
        }
        else if (ucWeapon == WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
        {
            lua_pushelement(luaVM, pProjectile->GetSatchelAttachedTo());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaProjectileDefs::GetProjectileCreator(lua_State* luaVM)
{
    // Verify the argument
    CClientProjectile* pProjectile = NULL;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pProjectile);

    if (!argStream.HasErrors())
    {
        lua_pushelement(luaVM, pProjectile->GetCreator());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaProjectileDefs::GetProjectileForce(lua_State* luaVM)
{
    // Verify the argument
    CClientProjectile* pProjectile = NULL;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pProjectile);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, static_cast<lua_Number>(pProjectile->GetForce()));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaProjectileDefs::SetProjectileCounter(lua_State* luaVM)
{
    CClientProjectile* pProjectile = NULL;
    DWORD              dwCounter = 0;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pProjectile);
    argStream.ReadNumber(dwCounter);

    // Verify the argument
    if (!argStream.HasErrors())
    {
        pProjectile->SetCounter(dwCounter);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaProjectileDefs::GetProjectileCounter(lua_State* luaVM)
{
    CClientProjectile* pProjectile = NULL;
    DWORD              dwCounter = 0;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pProjectile);

    // Verify the argument
    if (!argStream.HasErrors())
    {
        dwCounter = pProjectile->GetCounter();
        lua_pushnumber(luaVM, dwCounter);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
