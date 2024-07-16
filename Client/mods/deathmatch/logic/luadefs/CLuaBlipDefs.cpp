/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaBlipDefs.cpp
 *  PURPOSE:     Lua blip definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaBlipDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        {"createBlip", ArgumentParser<CreateBlip>},
        {"createBlipAttachedTo", ArgumentParser<CreateBlipAttachedTo>},
        {"getBlipIcon", GetBlipIcon},
        {"getBlipSize", GetBlipSize},
        {"getBlipColor", GetBlipColor},
        {"getBlipOrdering", GetBlipOrdering},
        {"getBlipVisibleDistance", GetBlipVisibleDistance},

        {"setBlipIcon", SetBlipIcon},
        {"setBlipSize", SetBlipSize},
        {"setBlipColor", SetBlipColor},
        {"setBlipOrdering", SetBlipOrdering},
        {"setBlipVisibleDistance", SetBlipVisibleDistance},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaBlipDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createBlip");
    lua_classfunction(luaVM, "createAttachedTo", "createBlipAttachedTo");

    lua_classfunction(luaVM, "getColor", "getBlipColor");
    lua_classfunction(luaVM, "getVisibleDistance", "getBlipVisibleDistance");
    lua_classfunction(luaVM, "getOrdering", "getBlipOrdering");
    lua_classfunction(luaVM, "getSize", "getBlipSize");
    lua_classfunction(luaVM, "getIcon", "getBlipIcon");

    lua_classfunction(luaVM, "setColor", "setBlipColor");
    lua_classfunction(luaVM, "setVisibleDistance", "setBlipVisibleDistance");
    lua_classfunction(luaVM, "setOrdering", "setBlipOrdering");
    lua_classfunction(luaVM, "setSize", "setBlipSize");
    lua_classfunction(luaVM, "setIcon", "setBlipIcon");

    lua_classvariable(luaVM, "icon", "setBlipIcon", "getBlipIcon");
    lua_classvariable(luaVM, "size", "setBlipSize", "getBlipSize");
    lua_classvariable(luaVM, "ordering", "setBlipOrdering", "getBlipOrdering");
    lua_classvariable(luaVM, "visibleDistance", "setBlipVisibleDistance", "getBlipVisibleDistance");
    // lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor" );

    lua_registerclass(luaVM, "Blip", "Element");
}

std::variant<bool, CClientRadarMarker*> CLuaBlipDefs::CreateBlip(lua_State* luaVM, CVector pos, std::optional<std::uint8_t> icon, std::optional<int> size,
                                                                 std::optional<SColor> color, std::optional<std::int16_t> ordering,
                                                                 std::optional<std::uint16_t> visibleDistance)
{
    if (!icon.has_value())
        icon = 0;
    if (!size.has_value())
        size = 2;
    if (!color.has_value())
        color = SColorRGBA(255, 0, 0, 255);
    if (!ordering.has_value())
        ordering = 0;
    if (!visibleDistance.has_value())
        visibleDistance = 16383;

    if (!CClientRadarMarkerManager::IsValidIcon(icon.value()))
        throw LuaFunctionError("Invalid icon");

    if (size.value() < 0 || size.value() > 25) {
        SString err("Blip size beyond 25 is no longer supported (got %i). It will be clamped between 0 and 25.", size.value());
        throw LuaFunctionError(err.c_str());
    }

    size = Clamp(0, size.value(), 25);

    CResource* resource = &lua_getownerresource(luaVM);

    // Create the blip
    CClientRadarMarker* marker = CStaticFunctionDefinitions::CreateBlip(*resource, pos, icon.value(), size.value(),
        color.value(), ordering.value(), visibleDistance.value());

    if (!marker)
        return false;

    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(marker);

    return marker;
}

std::variant<bool, CClientRadarMarker*> CLuaBlipDefs::CreateBlipAttachedTo(lua_State* luaVM, CClientEntity* element, std::optional<std::uint8_t> icon,
                                                                           std::optional<int> size, std::optional<SColor> color,
                                                                           std::optional<std::int16_t>  ordering,
                                                                           std::optional<std::uint16_t> visibleDistance)
{
    if (!icon.has_value())
        icon = 0;
    if (!size.has_value())
        size = 2;
    if (!color.has_value())
        color = SColorRGBA(255, 0, 0, 255);
    if (!ordering.has_value())
        ordering = 0;
    if (!visibleDistance.has_value())
        visibleDistance = 16383;

    if (!CClientRadarMarkerManager::IsValidIcon(icon.value()))
        throw LuaFunctionError("Invalid icon");

    if (size.value() < 0 || size.value() > 25)
    {
        SString err("Blip size beyond 25 is no longer supported (got %i). It will be clamped between 0 and 25.", size.value());
        throw LuaFunctionError(err.c_str());
    }

    size = Clamp(0, size.value(), 25);

    CResource* resource = &lua_getownerresource(luaVM);

    // Create the blip
    CClientRadarMarker* marker = CStaticFunctionDefinitions::CreateBlipAttachedTo(*resource, *element, icon.value(),
        size.value(), color.value(), ordering.value(), visibleDistance.value());

    if (!marker)
        return false;

    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(marker);

    return marker;
}

int CLuaBlipDefs::GetBlipIcon(lua_State* luaVM)
{
    CClientRadarMarker* pMarker = NULL;
    CScriptArgReader    argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        unsigned char ucIcon = static_cast<unsigned char>(pMarker->GetSprite());
        lua_pushnumber(luaVM, ucIcon);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipSize(lua_State* luaVM)
{
    CClientRadarMarker* pMarker = NULL;
    CScriptArgReader    argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        unsigned char ucSize = static_cast<unsigned char>(pMarker->GetScale());
        lua_pushnumber(luaVM, ucSize);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipColor(lua_State* luaVM)
{
    CClientRadarMarker* pMarker = NULL;
    CScriptArgReader    argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        SColor color = pMarker->GetColor();
        lua_pushnumber(luaVM, color.R);
        lua_pushnumber(luaVM, color.G);
        lua_pushnumber(luaVM, color.B);
        lua_pushnumber(luaVM, color.A);
        return 4;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipOrdering(lua_State* luaVM)
{
    CClientRadarMarker* pMarker = NULL;
    CScriptArgReader    argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        short sOrdering = pMarker->GetOrdering();
        lua_pushnumber(luaVM, sOrdering);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipVisibleDistance(lua_State* luaVM)
{
    CClientRadarMarker* pMarker = NULL;
    CScriptArgReader    argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        unsigned short usVisibleDistance = pMarker->GetVisibleDistance();
        lua_pushnumber(luaVM, usVisibleDistance);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::SetBlipIcon(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned char    ucIcon = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucIcon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetBlipIcon(*pEntity, ucIcon))
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

int CLuaBlipDefs::SetBlipSize(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    int              iSize = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(iSize);

    if (iSize < 0 || iSize > 25)
        argStream.SetCustomWarning(SString("Blip size beyond 25 is no longer supported (got %i). It will be clamped between 0 and 25.", iSize));

    if (!argStream.HasErrors())
    {
        unsigned char ucSize = Clamp(0, iSize, 25);

        if (CStaticFunctionDefinitions::SetBlipSize(*pEntity, ucSize))
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

int CLuaBlipDefs::SetBlipColor(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    SColor           color;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(color.R);
    argStream.ReadNumber(color.G);
    argStream.ReadNumber(color.B);
    argStream.ReadNumber(color.A);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetBlipColor(*pEntity, color))
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

int CLuaBlipDefs::SetBlipOrdering(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    int              iOrdering;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(iOrdering);

    if (!argStream.HasErrors())
    {
        short sOrdering = Clamp(-32768, iOrdering, 32767);

        if (CStaticFunctionDefinitions::SetBlipOrdering(*pEntity, sOrdering))
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

int CLuaBlipDefs::SetBlipVisibleDistance(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    int              iVisibleDistance;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(iVisibleDistance);

    if (!argStream.HasErrors())
    {
        unsigned short usVisibleDistance = Clamp(0, iVisibleDistance, 65535);

        if (CStaticFunctionDefinitions::SetBlipVisibleDistance(*pEntity, usVisibleDistance))
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
