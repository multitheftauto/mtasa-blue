/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaBlipDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaBlipDefs.h"
#include "CBlip.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"

void CLuaBlipDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        // Blip create/destroy funcs
        {"createBlip", ArgumentParser<CreateBlip>},
        {"createBlipAttachedTo", ArgumentParser<CreateBlipAttachedTo>},

        // Blip get funcs
        {"getBlipIcon", GetBlipIcon},
        {"getBlipSize", GetBlipSize},
        {"getBlipColor", GetBlipColor},
        {"getBlipOrdering", GetBlipOrdering},
        {"getBlipVisibleDistance", GetBlipVisibleDistance},

        // Blip set funcs
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
    // lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor", "" ); color

    lua_registerclass(luaVM, "Blip", "Element");
}

std::variant<bool, CBlip*> CLuaBlipDefs::CreateBlip(lua_State* luaVM, CVector pos, std::optional<std::uint8_t> icon, std::optional<int> size,
                                                    std::optional<SColor> color, std::optional<int> ordering, std::optional<int> visibleDistance,
                                                    std::optional<CElement*> visibleTo) noexcept
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
    if (!visibleTo.has_value())
        visibleTo = nullptr;

    if (!CBlipManager::IsValidIcon(icon.value()))
        throw LuaFunctionError("Invalid icon");

    CResource* resource = &lua_getownerresource(luaVM);

    size = Clamp(0, size.value(), 25);
    ordering = Clamp(-32768, ordering.value(), 32767);
    visibleDistance = Clamp(0, visibleDistance.value(), 65535);

    // Create the blip
    CBlip* blip = CStaticFunctionDefinitions::CreateBlip(resource, pos, icon.value(), size.value(), color.value(), ordering.value(), visibleDistance.value(),
                                                         visibleTo.value());
    if (!blip)
        return false;

    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(blip);

    return blip;
}

std::variant<bool, CBlip*> CLuaBlipDefs::CreateBlipAttachedTo(lua_State* luaVM, CElement* element, std::optional<std::uint8_t> icon, std::optional<int> size,
                                                              std::optional<SColor> color, std::optional<int> ordering, std::optional<int> visibleDistance,
                                                              std::optional<CElement*> visibleTo) noexcept
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
    if (!visibleTo.has_value())
        visibleTo = nullptr;

    if (!CBlipManager::IsValidIcon(icon.value()))
        throw LuaFunctionError("Invalid icon");

    CResource* resource = &lua_getownerresource(luaVM);

    size = Clamp(0, size.value(), 25);
    ordering = Clamp(-32768, ordering.value(), 32767);
    visibleDistance = Clamp(0, visibleDistance.value(), 65535);

    // Create the blip
    CBlip* blip = CStaticFunctionDefinitions::CreateBlipAttachedTo(resource, element, icon.value(), size.value(), color.value(), ordering.value(),
                                                                   visibleDistance.value(), visibleTo.value());
    if (!blip)
        return false;

    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(blip);

    return blip;
}

int CLuaBlipDefs::GetBlipIcon(lua_State* luaVM)
{
    CBlip* pBlip;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBlip);

    if (!argStream.HasErrors())
    {
        unsigned char ucIcon;
        if (CStaticFunctionDefinitions::GetBlipIcon(pBlip, ucIcon))
        {
            lua_pushnumber(luaVM, ucIcon);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipSize(lua_State* luaVM)
{
    CBlip* pBlip;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBlip);

    if (!argStream.HasErrors())
    {
        unsigned char ucSize;
        if (CStaticFunctionDefinitions::GetBlipSize(pBlip, ucSize))
        {
            lua_pushnumber(luaVM, ucSize);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipColor(lua_State* luaVM)
{
    CBlip* pBlip;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBlip);

    if (!argStream.HasErrors())
    {
        SColor color;
        if (CStaticFunctionDefinitions::GetBlipColor(pBlip, color))
        {
            lua_pushnumber(luaVM, color.R);
            lua_pushnumber(luaVM, color.G);
            lua_pushnumber(luaVM, color.B);
            lua_pushnumber(luaVM, color.A);
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipOrdering(lua_State* luaVM)
{
    CBlip* pBlip;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBlip);

    if (!argStream.HasErrors())
    {
        short sOrdering;
        if (CStaticFunctionDefinitions::GetBlipOrdering(pBlip, sOrdering))
        {
            lua_pushnumber(luaVM, sOrdering);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::GetBlipVisibleDistance(lua_State* luaVM)
{
    CBlip* pBlip;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBlip);

    if (!argStream.HasErrors())
    {
        unsigned short usVisibleDistance;
        if (CStaticFunctionDefinitions::GetBlipVisibleDistance(pBlip, usVisibleDistance))
        {
            lua_pushnumber(luaVM, usVisibleDistance);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBlipDefs::SetBlipIcon(lua_State* luaVM)
{
    CElement*     pElement;
    unsigned char ucIcon;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucIcon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetBlipIcon(pElement, ucIcon))
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
    CElement* pElement;
    int       iSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iSize);

    if (!argStream.HasErrors())
    {
        unsigned char ucSize = Clamp(0, iSize, 25);

        if (CStaticFunctionDefinitions::SetBlipSize(pElement, ucSize))
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
    CElement* pElement;
    SColor    color;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(color.R);
    argStream.ReadNumber(color.G);
    argStream.ReadNumber(color.B);
    argStream.ReadNumber(color.A);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetBlipColor(pElement, color))
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
    CElement* pElement;
    int       iOrdering;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iOrdering);

    if (!argStream.HasErrors())
    {
        short sOrdering = Clamp(-32768, iOrdering, 32767);

        if (CStaticFunctionDefinitions::SetBlipOrdering(pElement, sOrdering))
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
    CElement* pElement;
    int       iVisibleDistance;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iVisibleDistance);

    if (!argStream.HasErrors())
    {
        unsigned short usVisibleDistance = Clamp(0, iVisibleDistance, 65535);

        if (CStaticFunctionDefinitions::SetBlipVisibleDistance(pElement, usVisibleDistance))
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
