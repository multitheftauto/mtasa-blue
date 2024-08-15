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
#include <lua/CLuaMultiReturn.h>

class CLuaBlipDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Create functions
    static std::variant<CClientRadarMarker*, bool> CreateBlip(lua_State* const luaVM, const CVector vecPosition, const std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, const std::optional<std::uint8_t> r, const std::optional<std::uint8_t> g, const std::optional<std::uint8_t> b, const std::optional<std::uint8_t> a, const std::optional<std::int16_t> ordering, const std::optional<std::uint16_t> visibleDistance);
    static std::variant<CClientRadarMarker*, bool> CreateBlipAttachedTo(lua_State* const luaVM, CClientEntity* const entity, const std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, const std::optional<std::uint8_t> r, const std::optional<std::uint8_t> g, const std::optional<std::uint8_t> b, const std::optional<std::uint8_t> a, const std::optional<std::int16_t> ordering, const std::optional<std::uint16_t> visibleDistance);

    // Get functions
    static std::uint8_t                                                            GetBlipIcon(CClientRadarMarker* const radarMarker) noexcept;
    static std::uint8_t                                                            GetBlipSize(CClientRadarMarker* const radarMarker) noexcept;
    static CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t> GetBlipColor(CClientRadarMarker* const radarMarker) noexcept;
    static std::int16_t                                                            GetBlipOrdering(CClientRadarMarker* const radarMarker) noexcept;
    static std::uint16_t                                                           GetBlipVisibleDistance(CClientRadarMarker* const radarMarker) noexcept;

    // Set functions
    static bool SetBlipIcon(CClientRadarMarker* const radarMarker, const std::uint8_t icon);
    static bool SetBlipSize(lua_State* const luaVM, CClientRadarMarker* const radarMarker, std::uint8_t size) noexcept;
    static bool SetBlipColor(CClientRadarMarker* const radarMarker, const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) noexcept;
    static bool SetBlipOrdering(CClientRadarMarker* const radarMarker, const std::int16_t ordering) noexcept;
    static bool SetBlipVisibleDistance(CClientRadarMarker* const radarMarker, const std::uint16_t visibleDistance) noexcept;
};
