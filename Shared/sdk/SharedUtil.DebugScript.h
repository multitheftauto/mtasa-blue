/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/SharedUtil.DebugScript.h
 *  PURPOSE:     Debug script levels
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

using DebugScriptLevel = std::uint8_t;

namespace DebugScriptLevels
{
    constexpr DebugScriptLevel NONE = 0;
    constexpr DebugScriptLevel INFO = 1;
    constexpr DebugScriptLevel WARNINGS = 2;
    constexpr DebugScriptLevel ERRORS = 3;
    constexpr DebugScriptLevel CUSTOM = 4;
}
