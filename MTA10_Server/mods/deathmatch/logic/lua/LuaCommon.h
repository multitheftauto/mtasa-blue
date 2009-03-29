/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/LuaCommon.h
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __LUACOMMON_H
#define __LUACOMMON_H

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
	
	LUALIB_API int luaM_toref (lua_State *L, int i);
}

#define abs_index(L, i) \
	((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : \
	lua_gettop(L) + (i) + 1)
#define FREELIST_REF	0
#define RESERVED_REFS	2

// Lua pop macros for our datatypes
class CElement*         lua_toelement           ( lua_State* luaVM, int iArgument );
class CAccount*         lua_toaccount           ( lua_State* luaVM, int iArgument );
class CAccessControlList*       lua_toacl       ( lua_State* luaVM, int iArgument );
class CAccessControlListGroup*  lua_toaclgroup  ( lua_State* luaVM, int iArgument );
class CBlip*            lua_toblip              ( lua_State* luaVM, int iArgument );
class CColShape*        lua_tocolshape          ( lua_State* luaVM, int iArgument );
class CScriptFile*      lua_tofile              ( lua_State* luaVM, int iArgument );
class CHandling*        lua_tohandling          ( lua_State* luaVM, int iArgument );
class CMarker*          lua_tomarker            ( lua_State* luaVM, int iArgument );
class CObject*          lua_toobject            ( lua_State* luaVM, int iArgument );
class CPed*             lua_toped               ( lua_State* luaVM, int iArgument );
class CPickup*          lua_topickup            ( lua_State* luaVM, int iArgument );
class CPlayer*          lua_toplayer            ( lua_State* luaVM, int iArgument );
class CResource*        lua_toresource          ( lua_State* luaVM, int iArgument );
class CRadarArea*       lua_toradararea         ( lua_State* luaVM, int iArgument );
class CTeam*            lua_toteam              ( lua_State* luaVM, int iArgument );
class CTextDisplay*     lua_totextdisplay       ( lua_State* luaVM, int iArgument );
class CTextItem*        lua_totextitem          ( lua_State* luaVM, int iArgument );
class CLuaTimer*        lua_totimer             ( lua_State* luaVM, int iArgument );
class CVehicle*         lua_tovehicle           ( lua_State* luaVM, int iArgument );
class CXMLNode*         lua_toxmlnode           ( lua_State* luaVM, int iArgument );
class CBan*             lua_toban               ( lua_State* luaVM, int iArgument );
class CWater*           lua_towater             ( lua_State* luaVM, int iArgument );


// Lua push macros for our datatypes
void                    lua_pushelement         ( lua_State* luaVM, class CElement* pElement );
void                    lua_pushacl             ( lua_State* luaVM, class CAccessControlList* pACL );
void                    lua_pushaclgroup        ( lua_State* luaVM, class CAccessControlListGroup* pACL );
void                    lua_pushaccount         ( lua_State* luaVM, class CAccount* pAccount );
void                    lua_pushresource        ( lua_State* luaVM, class CResource* pResource );
void                    lua_pushtextdisplay     ( lua_State* luaVM, class CTextDisplay* pDisplay );
void                    lua_pushtextitem        ( lua_State* luaVM, class CTextItem* pItem );
void                    lua_pushtimer           ( lua_State* luaVM, class CLuaTimer* pTimer );
void                    lua_pushxmlnode         ( lua_State* luaVM, class CXMLNode* pNode );
void                    lua_pushban             ( lua_State* luaVM, class CBan* pBan );

enum eLuaNetworkDependantFunctions
{
    DONT_USE_0 = 0,
    SET_TIME,
    SET_WEATHER,
    SET_WEATHER_BLENDED,
	SET_MINUTE_DURATION,

    SET_ELEMENT_PARENT,
    SET_ELEMENT_DATA,
    SET_ELEMENT_POSITION,
    SET_ELEMENT_VELOCITY,
    SET_ELEMENT_INTERIOR,
    SET_ELEMENT_DIMENSION,
    ATTACH_ELEMENTS,
    DETACH_ELEMENTS,
    SET_ELEMENT_ALPHA,
	SET_ELEMENT_NAME,
    SET_ELEMENT_HEALTH,
    SET_ELEMENT_MODEL,
    
    SET_PLAYER_MONEY,
    SHOW_PLAYER_HUD_COMPONENT,
    FORCE_PLAYER_MAP,
    SET_PLAYER_NAMETAG_TEXT,
    REMOVE_PLAYER_NAMETAG_COLOR,
    SET_PLAYER_NAMETAG_COLOR,
    SET_PLAYER_NAMETAG_SHOWING,              	

    SET_PED_ARMOR,
    SET_PED_ROTATION,
    GIVE_PED_JETPACK,
    REMOVE_PED_JETPACK,
    REMOVE_PED_CLOTHES,
    SET_PED_GRAVITY,
    SET_PED_CHOKING,
    SET_PED_FIGHTING_STYLE,
    SET_PED_MOVE_ANIM,
    WARP_PED_INTO_VEHICLE,
    REMOVE_PED_FROM_VEHICLE,
    SET_PED_DOING_GANG_DRIVEBY,
    SET_PED_ANIMATION,
    SET_PED_ON_FIRE,
    SET_PED_HEADLESS,

    DESTROY_ALL_VEHICLES,
    FIX_VEHICLE,
    BLOW_VEHICLE,
    SET_VEHICLE_ROTATION,
    SET_VEHICLE_TURNSPEED,
    SET_VEHICLE_COLOR,
    SET_VEHICLE_LOCKED,
    SET_VEHICLE_DOORS_UNDAMAGEABLE,
    SET_VEHICLE_SIRENE_ON,
    SET_VEHICLE_LANDING_GEAR_DOWN,
    SET_HELICOPTER_ROTOR_SPEED,
	ADD_VEHICLE_UPGRADE,
    ADD_ALL_VEHICLE_UPGRADES,
	REMOVE_VEHICLE_UPGRADE,
	SET_VEHICLE_DAMAGE_STATE,
    SET_VEHICLE_OVERRIDE_LIGHTS,
    SET_VEHICLE_ENGINE_STATE,
    SET_VEHICLE_DIRT_LEVEL,
    SET_VEHICLE_DAMAGE_PROOF,
    SET_VEHICLE_PAINTJOB,
    SET_VEHICLE_FUEL_TANK_EXPLODABLE,
    SET_VEHICLE_WHEEL_STATES,
    SET_VEHICLE_FROZEN,
    SET_TRAIN_DERAILED,
    SET_TRAIN_DERAILABLE,
    SET_TRAIN_DIRECTION,
    SET_TRAIN_SPEED,

    GIVE_WEAPON,
    TAKE_WEAPON,
    TAKE_ALL_WEAPONS,
	GIVE_WEAPON_AMMO,
    TAKE_WEAPON_AMMO,
    SET_WEAPON_AMMO,
	SET_WEAPON_SLOT,

    DESTROY_ALL_BLIPS,
    SET_BLIP_ICON,
    SET_BLIP_SIZE,
    SET_BLIP_COLOR,
    SET_BLIP_ORDERING,

    DESTROY_ALL_OBJECTS,
    SET_OBJECT_ROTATION,
    MOVE_OBJECT,
    STOP_OBJECT,

    DESTROY_ALL_RADAR_AREAS,
    SET_RADAR_AREA_SIZE,
    SET_RADAR_AREA_COLOR,
    SET_RADAR_AREA_FLASHING,

    DESTROY_ALL_MARKERS,
    SET_MARKER_TYPE,
    SET_MARKER_COLOR,
    SET_MARKER_SIZE,
    SET_MARKER_TARGET,
    SET_MARKER_ICON,

    DESTROY_ALL_PICKUPS,
    SET_PICKUP_TYPE,
    SET_PICKUP_VISIBLE,

	PLAY_SOUND,

    BIND_KEY,
    UNBIND_KEY,
    TOGGLE_CONTROL_ABILITY,
    TOGGLE_ALL_CONTROL_ABILITY,
    SET_CONTROL_STATE,
    FORCE_RECONNECT,

    SET_TEAM_NAME,
    SET_TEAM_COLOR,
    SET_PLAYER_TEAM,
    SET_TEAM_FRIENDLY_FIRE,

    SET_WANTED_LEVEL,

    SET_CAMERA_MATRIX,
    SET_CAMERA_TARGET,
    SET_CAMERA_INTERIOR,
    FADE_CAMERA,

    SHOW_CURSOR,
	SHOW_CHAT,

    SET_GRAVITY,
    SET_GAME_SPEED,
    SET_WAVE_HEIGHT,
    SET_SKY_GRADIENT,
    RESET_SKY_GRADIENT,
    SET_BLUR_LEVEL,
    SET_FPS_LIMIT,
    SET_GARAGE_OPEN,
    RESET_MAP_INFO,

    REMOVE_ELEMENT_DATA,

    HANDLING_SET_DEFAULT,
    HANDLING_RESTORE_DEFAULT,
    HANDLING_SET_PROPERTY,

    TOGGLE_DEBUGGER,

    SET_WATER_LEVEL,
    SET_WATER_VERTEX_POSITION,

    NUM_RPC_FUNCS
};

enum
{
	AUDIO_FRONTEND,
	AUDIO_MISSION_PRELOAD,
	AUDIO_MISSION_PLAY
};

#endif
