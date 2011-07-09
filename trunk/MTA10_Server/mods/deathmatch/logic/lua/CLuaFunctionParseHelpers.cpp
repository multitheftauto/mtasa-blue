/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10_Server/mods/deathmatch/logic/lua/CLuaFunctionParseHelpers.cpp
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*****************************************************************************/

#include "StdInc.h"

//
// enum values <-> script strings
//

IMPLEMENT_ENUM_BEGIN( eLuaType )
    ADD_ENUM ( LUA_TNONE,           "none" )
    ADD_ENUM ( LUA_TNIL,            "nil" )
    ADD_ENUM ( LUA_TBOOLEAN,        "boolean" )
    ADD_ENUM ( LUA_TLIGHTUSERDATA,  "lightuserdata" )
    ADD_ENUM ( LUA_TNUMBER,         "number" )
    ADD_ENUM ( LUA_TSTRING,         "string" )
    ADD_ENUM ( LUA_TTABLE,          "table" )
    ADD_ENUM ( LUA_TFUNCTION,       "function" )
    ADD_ENUM ( LUA_TUSERDATA,       "userdata" )
    ADD_ENUM ( LUA_TTHREAD,         "thread" )
IMPLEMENT_ENUM_END( "lua-type" )

IMPLEMENT_ENUM_BEGIN( TrafficLight::EColor )
    ADD_ENUM ( TrafficLight::GREEN,           "green" )
    ADD_ENUM ( TrafficLight::YELLOW,          "yellow" )
    ADD_ENUM ( TrafficLight::RED,             "red" )
IMPLEMENT_ENUM_END( "traffic-light-color" )

IMPLEMENT_ENUM_BEGIN( TrafficLight::EState )
    ADD_ENUM ( TrafficLight::AUTO,            "auto" )
    ADD_ENUM ( TrafficLight::DISABLED,        "disabled" )
IMPLEMENT_ENUM_END( "traffic-light-state" )

IMPLEMENT_ENUM_BEGIN( CEasingCurve::eType )
    ADD_ENUM ( CEasingCurve::Linear,           "Linear" )
    ADD_ENUM ( CEasingCurve::InQuad,           "InQuad" )
    ADD_ENUM ( CEasingCurve::OutQuad,          "OutQuad" )
    ADD_ENUM ( CEasingCurve::InOutQuad,        "InOutQuad" )
    ADD_ENUM ( CEasingCurve::OutInQuad,        "OutInQuad" )
    ADD_ENUM ( CEasingCurve::InElastic,        "InElastic" )
    ADD_ENUM ( CEasingCurve::OutElastic,       "OutElastic" )
    ADD_ENUM ( CEasingCurve::InOutElastic,     "InOutElastic" )
    ADD_ENUM ( CEasingCurve::OutInElastic,     "OutInElastic" )
    ADD_ENUM ( CEasingCurve::InBack,           "InBack" )
    ADD_ENUM ( CEasingCurve::OutBack,          "OutBack" )
    ADD_ENUM ( CEasingCurve::InOutBack,        "InOutBack" )
    ADD_ENUM ( CEasingCurve::OutInBack,        "OutInBack" )
    ADD_ENUM ( CEasingCurve::InBounce,         "InBounce" )
    ADD_ENUM ( CEasingCurve::OutBounce,        "OutBounce" )
    ADD_ENUM ( CEasingCurve::InOutBounce,      "InOutBounce" )
    ADD_ENUM ( CEasingCurve::OutInBounce,      "OutInBounce" )
    ADD_ENUM ( CEasingCurve::SineCurve,        "SineCurve" )
    ADD_ENUM ( CEasingCurve::CosineCurve,      "CosineCurve" )
IMPLEMENT_ENUM_END_DEFAULTS( "easing-type", CEasingCurve::EASING_INVALID, "Invalid" )

IMPLEMENT_ENUM_BEGIN( eHudComponent )
    ADD_ENUM ( HUD_AMMO,            "ammo" )
    ADD_ENUM ( HUD_WEAPON,          "weapon" )
    ADD_ENUM ( HUD_HEALTH,          "health" )
    ADD_ENUM ( HUD_BREATH,          "breath" )
    ADD_ENUM ( HUD_ARMOUR,          "armour" )
    ADD_ENUM ( HUD_MONEY,           "money" )
    ADD_ENUM ( HUD_VEHICLE_NAME,    "vehicle_name" )
    ADD_ENUM ( HUD_AREA_NAME,       "area_name" )
    ADD_ENUM ( HUD_RADAR,           "radar" )
    ADD_ENUM ( HUD_CLOCK,           "clock" )
    ADD_ENUM ( HUD_RADIO,           "radio" )
    ADD_ENUM ( HUD_WANTED,          "wanted" )
    ADD_ENUM ( HUD_ALL,             "all" )
IMPLEMENT_ENUM_END( "hud-component" )
