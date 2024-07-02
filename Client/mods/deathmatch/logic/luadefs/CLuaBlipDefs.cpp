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
#include "lua/CLuaFunctionParser.h"

void CLuaBlipDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Create functions
        {"createBlip", ArgumentParserWarn<false, CreateBlip>},
        {"createBlipAttachedTo", ArgumentParserWarn<false, CreateBlipAttachedTo>},

        // Get functions
        {"getBlipIcon", ArgumentParserWarn<false, GetBlipIcon>},
        {"getBlipSize", ArgumentParserWarn<false, GetBlipSize>},
        {"getBlipColor", ArgumentParserWarn<false, GetBlipColor>},
        {"getBlipOrdering", ArgumentParserWarn<false, GetBlipOrdering>},
        {"getBlipVisibleDistance", ArgumentParserWarn<false, GetBlipVisibleDistance>},

        // Set functions
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
    // lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor" );

    lua_registerclass(luaVM, "Blip", "Element");
}

std::variant<CClientRadarMarker*, bool> CLuaBlipDefs::CreateBlip(lua_State* luaVM, CVector vecPosition, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int32_t> ordering, std::optional<std::uint32_t> visibleDistance)
{
    if (icon.has_value() && !CClientRadarMarkerManager::IsValidIcon(icon.value()))
        throw std::invalid_argument("Invalid icon");

    if (size.has_value() && (size.value() < 0 || size.value() > 25))
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond 25 is no longer supported (got %i). It will be clamped between 0 and 25.", size.value()));
        size = Clamp(0ui8, size.value(), 25ui8);
    }

    if (ordering.has_value())
        ordering = Clamp(std::numeric_limits<std::int16_t>().min(), (std::int16_t)ordering.value(), std::numeric_limits<std::int16_t>().max());

    if (visibleDistance.has_value())
        visibleDistance = Clamp(std::numeric_limits<std::uint16_t>().min(), (std::uint16_t)visibleDistance.value(), std::numeric_limits<std::uint16_t>().max());

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
    {
        m_pScriptDebugging->LogError(luaVM, "Couldn't find the virtual machine");
        return false;
    }

    CResource* resource = luaMain->GetResource();
    if (!resource)
    {
        m_pScriptDebugging->LogError(luaVM, "Couldn't find the resource element");
        return false;
    }

    CClientRadarMarker* radarMarker = CStaticFunctionDefinitions::CreateBlip(*resource, vecPosition, icon.value_or(0), size.value_or(2), SColorRGBA(r.value_or(255), g.value_or(0), b.value_or(0), a.value_or(255)), ordering.value_or(0), visibleDistance.value_or(16383));
    if (!radarMarker)
        return false;

    CElementGroup* elementGroup = resource->GetElementGroup();
    if (elementGroup)
        elementGroup->Add(radarMarker);

    return radarMarker;
}

std::variant<CClientRadarMarker*, bool> CLuaBlipDefs::CreateBlipAttachedTo(lua_State* luaVM, CClientEntity* entity, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int32_t> ordering, std::optional<std::uint32_t> visibleDistance)
{
    if (icon.has_value() && !CClientRadarMarkerManager::IsValidIcon(icon.value()))
        throw std::invalid_argument("Invalid icon");

    if (size.has_value() && (size.value() < 0 || size.value() > 25))
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond 25 is no longer supported (got %i). It will be clamped between 0 and 25.", size.value()));
        size = Clamp(0ui8, size.value(), 25ui8);
    }

    if (ordering.has_value())
        ordering = Clamp(std::numeric_limits<std::int16_t>().min(), (std::int16_t)ordering.value(), std::numeric_limits<std::int16_t>().max());

    if (visibleDistance.has_value())
        visibleDistance = Clamp(std::numeric_limits<std::uint16_t>().min(), (std::uint16_t)visibleDistance.value(), std::numeric_limits<std::uint16_t>().max());

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
    {
        m_pScriptDebugging->LogError(luaVM, "Couldn't find the virtual machine");
        return false;
    }

    CResource* resource = luaMain->GetResource();
    if (!resource)
    {
        m_pScriptDebugging->LogError(luaVM, "Couldn't find the resource element");
        return false;
    }

    CClientRadarMarker* radarMarker = CStaticFunctionDefinitions::CreateBlipAttachedTo(*resource, *entity, icon.value_or(0), size.value_or(2), SColorRGBA(r.value_or(255), g.value_or(0), b.value_or(0), a.value_or(255)), ordering.value_or(0), visibleDistance.value_or(16383));
    if (!radarMarker)
        return false;

    CElementGroup* elementGroup = resource->GetElementGroup();
    if (elementGroup)
        elementGroup->Add(radarMarker);

    return radarMarker;
}

std::uint8_t CLuaBlipDefs::GetBlipIcon(CClientRadarMarker* radarMarker) noexcept
{
    return std::uint8_t(radarMarker->GetSprite());
}

std::uint8_t CLuaBlipDefs::GetBlipSize(CClientRadarMarker* radarMarker) noexcept
{
    return radarMarker->GetScale();
}

CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t> CLuaBlipDefs::GetBlipColor(CClientRadarMarker* radarMarker) noexcept
{
    SColor color = radarMarker->GetColor();
    return {color.R, color.G, color.B, color.A};
}

std::int16_t CLuaBlipDefs::GetBlipOrdering(CClientRadarMarker* radarMarker) noexcept
{
    return radarMarker->GetOrdering();
}

std::uint16_t CLuaBlipDefs::GetBlipVisibleDistance(CClientRadarMarker* radarMarker) noexcept
{
    return radarMarker->GetVisibleDistance();
}

bool CLuaBlipDefs::SetBlipIcon(CClientRadarMarker* radarMarker, std::uint8_t icon)
{
    if (!CClientRadarMarkerManager::IsValidIcon(icon))
        throw std::invalid_argument("Invalid icon");

    return CStaticFunctionDefinitions::SetBlipIcon(*radarMarker, icon);
}

bool CLuaBlipDefs::SetBlipSize(lua_State* luaVM, CClientRadarMarker* radarMarker, std::uint8_t size) noexcept
{
    if (size < 0 || size > 25)
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond 25 is no longer supported (got %i). It will be clamped between 0 and 25.", size));
        size = Clamp(0ui8, size, 25ui8);
    }

    return CStaticFunctionDefinitions::SetBlipSize(*radarMarker, size);
}

bool CLuaBlipDefs::SetBlipColor(CClientRadarMarker* radarMarker, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept
{
    return CStaticFunctionDefinitions::SetBlipColor(*radarMarker, SColorRGBA(r, g, b, a));
}

bool CLuaBlipDefs::SetBlipOrdering(CClientRadarMarker* radarMarker, std::int32_t ordering) noexcept
{
    return CStaticFunctionDefinitions::SetBlipOrdering(*radarMarker, Clamp(std::numeric_limits<std::int16_t>().min(), (std::int16_t)ordering, std::numeric_limits<std::int16_t>().max()));
}

bool CLuaBlipDefs::SetBlipVisibleDistance(CClientRadarMarker* radarMarker, std::uint32_t visibleDistance) noexcept
{
    return CStaticFunctionDefinitions::SetBlipVisibleDistance(*radarMarker, Clamp(std::numeric_limits<std::uint16_t>().min(), (std::uint16_t)visibleDistance, std::numeric_limits<std::uint16_t>().max()));
}
