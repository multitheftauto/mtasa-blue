/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaWaterCannonDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaWaterCannonDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createWaterCannon", ArgumentParser<CreateWaterCannon>},
        {"setWaterCannonDirection", ArgumentParser<SetWaterCannonDirection>},
        {"getWaterCannonDirection", ArgumentParser<GetWaterCannonDirection>},
        {"setWaterCannonForce", ArgumentParser<SetWaterCannonForce>},
        {"getWaterCannonForce", ArgumentParser<GetWaterCannonForce>},
        {"setWaterCannonEnabled", ArgumentParser<SetWaterCannonEnabled>},
        {"isWaterCannonEnabled", ArgumentParser<IsWaterCannonEnabled>},
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaWaterCannonDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createWaterCannon");

    lua_classfunction(luaVM, "setDirection", "setWaterCannonDirection");
    lua_classfunction(luaVM, "setForce", "setWaterCannonForce");
    lua_classfunction(luaVM, "setEnabled", "setWaterCannonEnabled");

    lua_classfunction(luaVM, "getDirection", "getWaterCannonDirection");
    lua_classfunction(luaVM, "getForce", "getWaterCannonForce");
    lua_classfunction(luaVM, "isEnabled", "isWaterCannonEnabled");

    lua_classvariable(luaVM, "direction", "setWaterCannonDirection", "getWaterCannonDirection");
    lua_classvariable(luaVM, "force", "setWaterCannonForce", "getWaterCannonForce");
    lua_classvariable(luaVM, "enabled", "setWaterCannonEnabled", "isWaterCannonEnabled");

    lua_registerclass(luaVM, "WaterCannon", "Element");
}

std::variant<CClientWaterCannon*, bool> CLuaWaterCannonDefs::CreateWaterCannon(lua_State* luaVM, CVector vecPosition)
{
    CResource&           resource = lua_getownerresource(luaVM);
    CClientWaterCannon* pCannon = CStaticFunctionDefinitions::CreateWaterCannon(resource, vecPosition);
    if (!pCannon)
        return false;

    if (CElementGroup* elementGroup = resource.GetElementGroup())
        elementGroup->Add(pCannon);

    return pCannon;
}

bool CLuaWaterCannonDefs::SetWaterCannonDirection(CClientWaterCannon* pCannon, CVector vecDirection)
{
    pCannon->SetDirection(vecDirection);
    return true;
}

CVector CLuaWaterCannonDefs::GetWaterCannonDirection(CClientWaterCannon* pCannon)
{
    return pCannon->GetDirection();
}

bool CLuaWaterCannonDefs::SetWaterCannonForce(CClientWaterCannon* pCannon, float fForce)
{
    pCannon->SetForce(fForce);
    return true;
}

float CLuaWaterCannonDefs::GetWaterCannonForce(CClientWaterCannon* pCannon)
{
    return pCannon->GetForce();
}

bool CLuaWaterCannonDefs::SetWaterCannonEnabled(CClientWaterCannon* pCannon, bool bEnabled)
{
    pCannon->SetEnabled(bEnabled);
    return true;
}

bool CLuaWaterCannonDefs::IsWaterCannonEnabled(CClientWaterCannon* pCannon)
{
    return pCannon->IsEnabled();
}
