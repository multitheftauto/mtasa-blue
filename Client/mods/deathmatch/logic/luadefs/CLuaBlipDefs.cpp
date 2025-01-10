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

static constexpr std::uint8_t MAX_BLIP_SIZE = 25;

std::variant<CClientRadarMarker*, bool> CLuaBlipDefs::CreateBlip(lua_State* luaVM, const CVector vecPosition, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int16_t> ordering, std::optional<std::uint16_t> visibleDistance)
{
    if (icon.has_value() && !CClientRadarMarkerManager::IsValidIcon(icon.value()))
        throw std::invalid_argument("Invalid icon");

    if (size.has_value() && size.value() > MAX_BLIP_SIZE)
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond %i is no longer supported (got %i). It will be clamped between 0 and %i.", MAX_BLIP_SIZE, size.value(), MAX_BLIP_SIZE));
        size = MAX_BLIP_SIZE;
    }

    CResource&          resource = lua_getownerresource(luaVM);
    CClientRadarMarker* radarMarker = CStaticFunctionDefinitions::CreateBlip(resource, vecPosition, icon.value_or(0), size.value_or(2),
                                                                             SColorRGBA(r.value_or(255), g.value_or(0), b.value_or(0), a.value_or(255)),
                                                                             ordering.value_or(0), visibleDistance.value_or(16383));

    if (!radarMarker)
        return false;

    if (CElementGroup* elementGroup = resource.GetElementGroup())
        elementGroup->Add(radarMarker);

    return radarMarker;
}

std::variant<CClientRadarMarker*, bool> CLuaBlipDefs::CreateBlipAttachedTo(lua_State* luaVM, CClientEntity* entity, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int16_t> ordering, std::optional<std::uint16_t> visibleDistance)
{
    if (icon.has_value() && !CClientRadarMarkerManager::IsValidIcon(icon.value()))
        throw std::invalid_argument("Invalid icon");

    if (size.has_value() && size.value() > MAX_BLIP_SIZE)
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond %i is no longer supported (got %i). It will be clamped between 0 and %i.", MAX_BLIP_SIZE, size.value(), MAX_BLIP_SIZE));
        size = MAX_BLIP_SIZE;
    }

    CResource&          resource = lua_getownerresource(luaVM);
    CClientRadarMarker* radarMarker = CStaticFunctionDefinitions::CreateBlipAttachedTo(
        resource, *entity, icon.value_or(0), size.value_or(2), SColorRGBA(r.value_or(255), g.value_or(0), b.value_or(0), a.value_or(255)), ordering.value_or(0),
        visibleDistance.value_or(16383));

    if (!radarMarker)
        return false;
    
    if (CElementGroup* elementGroup = resource.GetElementGroup())
        elementGroup->Add(radarMarker);

    return radarMarker;
}

auto CLuaBlipDefs::GetBlipIcon(CClientRadarMarker* const radarMarker) noexcept
{
    return static_cast<std::uint8_t>(radarMarker->GetSprite());
}

auto CLuaBlipDefs::GetBlipSize(CClientRadarMarker* const radarMarker) noexcept
{
    return radarMarker->GetScale();
}

auto CLuaBlipDefs::GetBlipColor(CClientRadarMarker* radarMarker) noexcept
{
    SColor color = radarMarker->GetColor();
    return CLuaMultiReturn<float, float, float, float>{color.R, color.G, color.B, color.A};
}

auto CLuaBlipDefs::GetBlipOrdering(CClientRadarMarker* radarMarker) noexcept
{
    return radarMarker->GetOrdering();
}

auto CLuaBlipDefs::GetBlipVisibleDistance(CClientRadarMarker* radarMarker) noexcept
{
    return radarMarker->GetVisibleDistance();
}

bool CLuaBlipDefs::SetBlipIcon(CClientRadarMarker* radarMarker, std::uint8_t icon)
{
    if (!CClientRadarMarkerManager::IsValidIcon(icon))
        throw std::invalid_argument("Invalid icon");

    return CStaticFunctionDefinitions::SetBlipIcon(*radarMarker, icon);
}

bool CLuaBlipDefs::SetBlipSize(lua_State* luaVM, CClientRadarMarker* radarMarker, std::uint8_t size)
{
    if (size > MAX_BLIP_SIZE)
    {
        m_pScriptDebugging->LogWarning(luaVM, SString("Blip size beyond %i is no longer supported (got %i). It will be clamped between 0 and %i.", MAX_BLIP_SIZE, size, MAX_BLIP_SIZE));
        size = MAX_BLIP_SIZE;
    }

    return CStaticFunctionDefinitions::SetBlipSize(*radarMarker, size);
}

bool CLuaBlipDefs::SetBlipColor(CClientRadarMarker* radarMarker, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
{
    return CStaticFunctionDefinitions::SetBlipColor(*radarMarker, SColorRGBA(r, g, b, a));
}

bool CLuaBlipDefs::SetBlipOrdering(CClientRadarMarker* radarMarker, std::int16_t ordering)
{
    return CStaticFunctionDefinitions::SetBlipOrdering(*radarMarker, ordering);
}

bool CLuaBlipDefs::SetBlipVisibleDistance(CClientRadarMarker* radarMarker, std::uint16_t visibleDistance)
{
    return CStaticFunctionDefinitions::SetBlipVisibleDistance(*radarMarker, visibleDistance);
}

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
    // TODO add oop color variable

    lua_registerclass(luaVM, "Blip", "Element");
}
