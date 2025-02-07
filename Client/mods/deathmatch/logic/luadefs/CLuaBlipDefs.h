/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaBlipDefs.h
 *  PURPOSE:     Lua blip definitions class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaBlipDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

private:
    // Create functions
    static std::variant<CClientRadarMarker*, bool> CreateBlip(lua_State* luaVM, const CVector vecPosition, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int16_t> ordering, std::optional<std::uint16_t> visibleDistance);
    static std::variant<CClientRadarMarker*, bool> CreateBlipAttachedTo(lua_State* luaVM, CClientEntity* entity, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int16_t> ordering, std::optional<std::uint16_t> visibleDistance);

    // Get functions
    static auto GetBlipIcon(CClientRadarMarker* radarMarker) noexcept;
    static auto GetBlipSize(CClientRadarMarker* radarMarker) noexcept;
    static auto GetBlipColor(CClientRadarMarker* radarMarker) noexcept;
    static auto GetBlipOrdering(CClientRadarMarker* radarMarker) noexcept;
    static auto GetBlipVisibleDistance(CClientRadarMarker* radarMarker) noexcept;

    // Set functions
    static bool SetBlipIcon(CClientRadarMarker* radarMarker, std::uint8_t icon);
    static bool SetBlipSize(lua_State* luaVM, CClientRadarMarker* radarMarker, std::uint8_t size);
    static bool SetBlipColor(CClientRadarMarker* radarMarker, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);
    static bool SetBlipOrdering(CClientRadarMarker* radarMarker, std::int16_t ordering);
    static bool SetBlipVisibleDistance(CClientRadarMarker* radarMarker, std::uint16_t visibleDistance);
};
