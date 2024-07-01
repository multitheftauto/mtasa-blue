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
    static std::variant<CClientRadarMarker*, bool> CreateBlip(lua_State* luaVM, CVector vecPosition, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int32_t> ordering, std::optional<std::uint32_t> visibleDistance);
    static std::variant<CClientRadarMarker*, bool> CreateBlipAttachedTo(lua_State* luaVM, CClientEntity* entity, std::optional<std::uint8_t> icon, std::optional<std::uint8_t> size, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<std::int32_t> ordering, std::optional<std::uint32_t> visibleDistance);

    // Get functions
    static std::uint8_t                                                            GetBlipIcon(CClientRadarMarker* radarMarker) noexcept;
    static std::uint8_t                                                            GetBlipSize(CClientRadarMarker* radarMarker) noexcept;
    static CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t> GetBlipColor(CClientRadarMarker* radarMarker) noexcept;
    static std::int16_t                                                            GetBlipOrdering(CClientRadarMarker* radarMarker) noexcept;
    static std::uint16_t                                                           GetBlipVisibleDistance(CClientRadarMarker* radarMarker) noexcept;

    // Set functions
    static bool SetBlipIcon(CClientRadarMarker* radarMarker, std::uint8_t icon);
    static bool SetBlipSize(lua_State* luaVM, CClientRadarMarker* radarMarker, std::uint8_t size) noexcept;
    static bool SetBlipColor(CClientRadarMarker* radarMarker, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept;
    static bool SetBlipOrdering(CClientRadarMarker* radarMarker, std::int32_t ordering) noexcept;
    static bool SetBlipVisibleDistance(CClientRadarMarker* radarMarker, std::uint32_t visibleDistance) noexcept;
};
