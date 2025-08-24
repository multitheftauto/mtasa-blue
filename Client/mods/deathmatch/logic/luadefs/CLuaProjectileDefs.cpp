/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaProjectileDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaProjectileDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createProjectile", CreateProjectile},         {"getProjectileType", GetProjectileType},   {"getProjectileTarget", GetProjectileTarget},
        {"getProjectileCreator", GetProjectileCreator}, {"getProjectileForce", GetProjectileForce}, {"setProjectileCounter", SetProjectileCounter},
        {"getProjectileCounter", GetProjectileCounter},
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

int CLuaProjectileDefs::CreateProjectile(lua_State* luaVM)
{
    CVector          vecOrigin;
    CClientEntity*   pCreator = NULL;
    unsigned char    ucWeaponType = 0;
    CScriptArgReader argStream(luaVM);
    float            fForce = 1.0f;
    CClientEntity*   pTarget = NULL;
    CVector          vecRotation, vecMoveSpeed;
    unsigned short   usModel = 0;
    argStream.ReadUserData(pCreator);
    if (pCreator)
        pCreator->GetPosition(vecOrigin);

    argStream.ReadNumber(ucWeaponType);
    argStream.ReadVector3D(vecOrigin, vecOrigin);
    argStream.ReadNumber(fForce, 1.0f);
    argStream.ReadUserData(pTarget, NULL);
    argStream.ReadVector3D(vecRotation, vecRotation);
    argStream.ReadVector3D(vecMoveSpeed, vecMoveSpeed);
    argStream.ReadNumber(usModel, 0);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CClientProjectile* pProjectile = CStaticFunctionDefinitions::CreateProjectile(*pResource, *pCreator, ucWeaponType, vecOrigin, fForce, pTarget,
                                                                                              vecRotation, vecMoveSpeed, usModel);
                if (pProjectile)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pProjectile);
                    }

                    lua_pushelement(luaVM, pProjectile);
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
