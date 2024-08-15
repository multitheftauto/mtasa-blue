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
#include "lua/CLuaFunctionParser.h"

constexpr std::uint8_t MAX_BLIP_SIZE = 25;

void CLuaBlipDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Blip create funcs
        {"createBlip", ArgumentParserWarn<false, CreateBlip>},
        {"createBlipAttachedTo", ArgumentParserWarn<false, CreateBlipAttachedTo>},

        // Blip get funcs
        {"getBlipIcon", ArgumentParserWarn<false, GetBlipIcon>},
        {"getBlipSize", ArgumentParserWarn<false, GetBlipSize>},
        {"getBlipColor", ArgumentParserWarn<false, GetBlipColor>},
        {"getBlipOrdering", ArgumentParserWarn<false, GetBlipOrdering>},
        {"getBlipVisibleDistance", ArgumentParserWarn<false, GetBlipVisibleDistance>},

        // Blip set funcs
        {"setBlipIcon", ArgumentParserWarn<false, SetBlipIcon>},
        {"setBlipSize", ArgumentParserWarn<false, SetBlipSize>},
        {"setBlipColor", ArgumentParserWarn<false, SetBlipColor>},
        {"setBlipOrdering", ArgumentParserWarn<false, SetBlipOrdering>},
        {"setBlipVisibleDistance", ArgumentParserWarn<false, SetBlipVisibleDistance>},
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

std::variant<CBlip*, bool> CLuaBlipDefs::CreateBlip(lua_State* const luaVM, const CVector vecPosition, const std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, const std::optional<std::uint8_t> r, const std::optional<std::uint8_t> g, const std::optional<std::uint8_t> b, const std::optional<std::uint8_t> a, const std::optional<std::int16_t> ordering, const std::optional<std::uint16_t> visibleDistance, const std::optional<CElement*> visibleTo)
{
    if (icon.has_value() && !CBlipManager::IsValidIcon(icon.value()))
        throw std::invalid_argument("Invalid icon");

    if (size.has_value() && size.value() > MAX_BLIP_SIZE)
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond %i is no longer supported (got %i). It will be clamped between 0 and %i.", MAX_BLIP_SIZE, size.value(), MAX_BLIP_SIZE));
        size = MAX_BLIP_SIZE;
    }

    CResource* resource = &lua_getownerresource(luaVM);
    if (!resource)
    {
        m_pScriptDebugging->LogError(luaVM, "Couldn't find the resource element");
        return false;
    }

    CBlip* radarMarker = CStaticFunctionDefinitions::CreateBlip(resource, vecPosition, icon.value_or(0), size.value_or(2), SColorRGBA(r.value_or(255), g.value_or(0), b.value_or(0), a.value_or(255)), ordering.value_or(0), visibleDistance.value_or(16383), visibleTo.value_or(nullptr));
    if (!radarMarker)
        return false;

    CElementGroup* elementGroup = resource->GetElementGroup();
    if (elementGroup)
        elementGroup->Add(radarMarker);

    return radarMarker;
}

std::variant<CBlip*, bool> CLuaBlipDefs::CreateBlipAttachedTo(lua_State* const luaVM, CElement* const entity, const std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, const std::optional<std::uint8_t> r, const std::optional<std::uint8_t> g, const std::optional<std::uint8_t> b, const std::optional<std::uint8_t> a, const std::optional<std::int16_t> ordering, const std::optional<std::uint16_t> visibleDistance, const std::optional<CElement*> visibleTo)
{
    if (icon.has_value() && !CBlipManager::IsValidIcon(icon.value()))
        throw std::invalid_argument("Invalid icon");

    if (size.has_value() && size.value() > MAX_BLIP_SIZE)
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond %i is no longer supported (got %i). It will be clamped between 0 and %i.", MAX_BLIP_SIZE, size.value(), MAX_BLIP_SIZE));
        size = MAX_BLIP_SIZE;
    }

    CResource* resource = &lua_getownerresource(luaVM);
    if (!resource)
    {
        m_pScriptDebugging->LogError(luaVM, "Couldn't find the resource element");
        return false;
    }

    CBlip* radarMarker = CStaticFunctionDefinitions::CreateBlipAttachedTo(resource, entity, icon.value_or(0), size.value_or(2), SColorRGBA(r.value_or(255), g.value_or(0), b.value_or(0), a.value_or(255)), ordering.value_or(0), visibleDistance.value_or(16383), visibleTo.value_or(nullptr));
    if (!radarMarker)
        return false;

    CElementGroup* elementGroup = resource->GetElementGroup();
    if (elementGroup)
        elementGroup->Add(radarMarker);

    return radarMarker;
}

std::uint8_t CLuaBlipDefs::GetBlipIcon(CBlip* const radarMarker) noexcept
{
    return static_cast<std::uint8_t>(radarMarker->m_ucIcon);
}

std::uint8_t CLuaBlipDefs::GetBlipSize(CBlip* const radarMarker) noexcept
{
    return static_cast<std::uint8_t>(radarMarker->m_ucSize);
}

CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t> CLuaBlipDefs::GetBlipColor(CBlip* const radarMarker) noexcept
{
    SColor color = radarMarker->GetColor();
    return {color.R, color.G, color.B, color.A};
}

std::int16_t CLuaBlipDefs::GetBlipOrdering(CBlip* const radarMarker) noexcept
{
    return static_cast<std::int16_t>(radarMarker->m_sOrdering);
}

std::uint16_t CLuaBlipDefs::GetBlipVisibleDistance(CBlip* const radarMarker) noexcept
{
    return static_cast<std::uint16_t>(radarMarker->m_usVisibleDistance);
}

bool CLuaBlipDefs::SetBlipIcon(CElement* const radarMarker, const std::uint8_t icon)
{
    if (!CBlipManager::IsValidIcon(icon))
        throw std::invalid_argument("Invalid icon");

    return CStaticFunctionDefinitions::SetBlipIcon(radarMarker, icon);
}

bool CLuaBlipDefs::SetBlipSize(lua_State* const luaVM, CElement* const radarMarker, std::uint8_t size) noexcept
{
    if (size > MAX_BLIP_SIZE)
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond %i is no longer supported (got %i). It will be clamped between 0 and %i.", MAX_BLIP_SIZE, size, MAX_BLIP_SIZE));
        size = MAX_BLIP_SIZE;
    }

    return CStaticFunctionDefinitions::SetBlipSize(radarMarker, size);
}

bool CLuaBlipDefs::SetBlipColor(CElement* const radarMarker, const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) noexcept
{
    return CStaticFunctionDefinitions::SetBlipColor(radarMarker, SColorRGBA(r, g, b, a));
}

bool CLuaBlipDefs::SetBlipOrdering(CElement* const radarMarker, const std::int16_t ordering) noexcept
{
    return CStaticFunctionDefinitions::SetBlipOrdering(radarMarker, ordering);
}

bool CLuaBlipDefs::SetBlipVisibleDistance(CElement* const radarMarker, const std::uint16_t visibleDistance) noexcept
{
    return CStaticFunctionDefinitions::SetBlipVisibleDistance(radarMarker, visibleDistance);
}
