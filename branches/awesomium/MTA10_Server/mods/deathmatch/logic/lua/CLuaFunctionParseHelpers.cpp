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
    ADD_ENUM ( HUD_CROSSHAIR,       "crosshair" )
    ADD_ENUM ( HUD_ALL,             "all" )
IMPLEMENT_ENUM_END( "hud-component" )

IMPLEMENT_ENUM_BEGIN( eWeaponType )
    // Compatible with getWeaponNameFromID                          From setWeaponProperty before r4523
    ADD_ENUM ( WEAPONTYPE_UNARMED,              "fist" )
                                                                    ADD_ENUM ( WEAPONTYPE_BRASSKNUCKLE,         "brass_knuckle" )
    ADD_ENUM ( WEAPONTYPE_BRASSKNUCKLE,         "brassknuckle" )
    ADD_ENUM ( WEAPONTYPE_GOLFCLUB,             "golfclub" )
    ADD_ENUM ( WEAPONTYPE_NIGHTSTICK,           "nightstick" )
    ADD_ENUM ( WEAPONTYPE_KNIFE,                "knife" )
                                                                    ADD_ENUM ( WEAPONTYPE_BASEBALLBAT,          "baseball_bat" )
    ADD_ENUM ( WEAPONTYPE_BASEBALLBAT,          "bat" )
    ADD_ENUM ( WEAPONTYPE_SHOVEL,               "shovel" )
                                                                    ADD_ENUM ( WEAPONTYPE_POOL_CUE,             "pool_cue" )
    ADD_ENUM ( WEAPONTYPE_POOL_CUE,             "poolstick" )
    ADD_ENUM ( WEAPONTYPE_KATANA,               "katana" )
    ADD_ENUM ( WEAPONTYPE_CHAINSAW,             "chainsaw" )
                                                                    ADD_ENUM ( WEAPONTYPE_DILDO1,               "dildo1" )
                                                                    ADD_ENUM ( WEAPONTYPE_DILDO2,               "dildo2" )
    ADD_ENUM ( WEAPONTYPE_DILDO2,               "dildo" )
    ADD_ENUM ( WEAPONTYPE_DILDO1,               "dildo" )
                                                                    ADD_ENUM ( WEAPONTYPE_VIBE1,                "vibe1" )
                                                                    ADD_ENUM ( WEAPONTYPE_VIBE2,                "vibe2" )
    ADD_ENUM ( WEAPONTYPE_VIBE2,                "vibrator" )
    ADD_ENUM ( WEAPONTYPE_VIBE1,                "vibrator" )
                                                                    ADD_ENUM ( WEAPONTYPE_FLOWERS,              "flowers" )
    ADD_ENUM ( WEAPONTYPE_FLOWERS,              "flower" )
    ADD_ENUM ( WEAPONTYPE_CANE,                 "cane" )
    ADD_ENUM ( WEAPONTYPE_GRENADE,              "grenade" )
    ADD_ENUM ( WEAPONTYPE_TEARGAS,              "teargas" )
    ADD_ENUM ( WEAPONTYPE_MOLOTOV,              "molotov" )
                                                                    ADD_ENUM ( WEAPONTYPE_PISTOL,               "pistol" )
    ADD_ENUM ( WEAPONTYPE_PISTOL,               "colt 45" )
                                                                    ADD_ENUM ( WEAPONTYPE_PISTOL_SILENCED,      "silenced_pistol" )
    ADD_ENUM ( WEAPONTYPE_PISTOL_SILENCED,      "silenced" )
                                                                    ADD_ENUM ( WEAPONTYPE_DESERT_EAGLE,         "desert_eagle" )
    ADD_ENUM ( WEAPONTYPE_DESERT_EAGLE,         "deagle" )
    ADD_ENUM ( WEAPONTYPE_SHOTGUN,              "shotgun" )
                                                                    ADD_ENUM ( WEAPONTYPE_SAWNOFF_SHOTGUN,      "sawnoff_shotgun" )
    ADD_ENUM ( WEAPONTYPE_SAWNOFF_SHOTGUN,      "sawed-off" )
                                                                    ADD_ENUM ( WEAPONTYPE_SPAS12_SHOTGUN,       "spas12_shotgun" )
    ADD_ENUM ( WEAPONTYPE_SPAS12_SHOTGUN,       "combat shotgun" )
    ADD_ENUM ( WEAPONTYPE_MICRO_UZI,            "uzi" )
    ADD_ENUM ( WEAPONTYPE_MP5,                  "mp5" )
                                                                    ADD_ENUM ( WEAPONTYPE_AK47,                 "ak47" )
    ADD_ENUM ( WEAPONTYPE_AK47,                 "ak-47" )
    ADD_ENUM ( WEAPONTYPE_M4,                   "m4" )   
                                                                    ADD_ENUM ( WEAPONTYPE_TEC9,                 "tec9" )
    ADD_ENUM ( WEAPONTYPE_TEC9,                 "tec-9" )
    ADD_ENUM ( WEAPONTYPE_COUNTRYRIFLE,         "rifle" )
                                                                    ADD_ENUM ( WEAPONTYPE_SNIPERRIFLE,          "sniper_rifle" )
    ADD_ENUM ( WEAPONTYPE_SNIPERRIFLE,          "sniper" )
                                                                    ADD_ENUM ( WEAPONTYPE_ROCKETLAUNCHER,       "rocket_launcher" )
    ADD_ENUM ( WEAPONTYPE_ROCKETLAUNCHER,       "rocket launcher" )
                                                                    ADD_ENUM ( WEAPONTYPE_ROCKETLAUNCHER_HS,    "rocket_launcher_hs" )
    ADD_ENUM ( WEAPONTYPE_ROCKETLAUNCHER_HS,    "rocket launcher hs" )
    ADD_ENUM ( WEAPONTYPE_FLAMETHROWER,         "flamethrower" )
    ADD_ENUM ( WEAPONTYPE_MINIGUN,              "minigun" )
    ADD_ENUM ( WEAPONTYPE_REMOTE_SATCHEL_CHARGE,"satchel" )
                                                                    ADD_ENUM ( WEAPONTYPE_DETONATOR,            "detonator" )
    ADD_ENUM ( WEAPONTYPE_DETONATOR,            "bomb" )
    ADD_ENUM ( WEAPONTYPE_SPRAYCAN,             "spraycan" )
                                                                    ADD_ENUM ( WEAPONTYPE_EXTINGUISHER,         "extinguisher" )
    ADD_ENUM ( WEAPONTYPE_EXTINGUISHER,         "fire extinguisher" )
    ADD_ENUM ( WEAPONTYPE_CAMERA,               "camera" )
    ADD_ENUM ( WEAPONTYPE_NIGHTVISION,          "nightvision" )
    ADD_ENUM ( WEAPONTYPE_INFRARED,             "infrared" )
    ADD_ENUM ( WEAPONTYPE_PARACHUTE,            "parachute" )
IMPLEMENT_ENUM_END( "weapon-type" )

IMPLEMENT_ENUM_BEGIN( eWeaponProperty )
    ADD_ENUM ( WEAPON_WEAPON_RANGE,                     "weapon_range" )
    ADD_ENUM ( WEAPON_TARGET_RANGE,                     "target_range" )
    ADD_ENUM ( WEAPON_ACCURACY,                         "accuracy" )
    ADD_ENUM ( WEAPON_DAMAGE,                           "damage" )
    ADD_ENUM ( WEAPON_LIFE_SPAN,                        "life_span" )
    ADD_ENUM ( WEAPON_FIRING_SPEED,                     "firing_speed" )
    ADD_ENUM ( WEAPON_SPREAD,                           "spread" )
    ADD_ENUM ( WEAPON_MAX_CLIP_AMMO,                    "maximum_clip_ammo" )
    ADD_ENUM ( WEAPON_MOVE_SPEED,                       "move_speed" )
    ADD_ENUM ( WEAPON_FLAGS,                            "flags" )
    ADD_ENUM ( WEAPON_ANIM_GROUP,                       "anim_group" )

    // Set is Disabled >= FIRETYPE - Move above WEAPON_TYPE_SET_DISABLED to re-enable and move it above here to make it less confusing.
    ADD_ENUM ( WEAPON_FIRETYPE,                         "fire_type" )

    ADD_ENUM ( WEAPON_MODEL,                            "model" )
    ADD_ENUM ( WEAPON_MODEL2,                           "model2" ) 

    ADD_ENUM ( WEAPON_SLOT,                             "weapon_slot" ) 

    ADD_ENUM ( WEAPON_FIRE_OFFSET,                      "fire_offset" ) 

    ADD_ENUM ( WEAPON_SKILL_LEVEL,                      "skill_level" ) 
    ADD_ENUM ( WEAPON_REQ_SKILL_LEVEL,                  "required_skill_level" ) 

    ADD_ENUM ( WEAPON_ANIM_LOOP_START,                  "anim_loop_start" ) 
    ADD_ENUM ( WEAPON_ANIM_LOOP_STOP,                   "anim_loop_stop" ) 
    ADD_ENUM ( WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME,    "anim_loop_bullet_fire" ) 

    ADD_ENUM ( WEAPON_ANIM2_LOOP_START,                 "anim2_loop_start" ) 
    ADD_ENUM ( WEAPON_ANIM2_LOOP_STOP,                  "anim2_loop_stop" ) 
    ADD_ENUM ( WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME,   "anim2_loop_bullet_fire" ) 

    ADD_ENUM ( WEAPON_ANIM_BREAKOUT_TIME,               "anim_breakout_time" ) 

    ADD_ENUM ( WEAPON_RADIUS,                           "radius" ) 

    ADD_ENUM ( WEAPON_AIM_OFFSET,                       "aim_offset" ) 

    ADD_ENUM ( WEAPON_DEFAULT_COMBO,                    "default_combo" ) 
    ADD_ENUM ( WEAPON_COMBOS_AVAILABLE,                 "combos_available" ) 

    ADD_ENUM ( WEAPON_FLAG_AIM_NO_AUTO,                 "flag_aim_no_auto" ) 
    ADD_ENUM ( WEAPON_FLAG_AIM_ARM,                     "flag_aim_arm" ) 
    ADD_ENUM ( WEAPON_FLAG_AIM_1ST_PERSON,              "flag_aim_1st_person" ) 
    ADD_ENUM ( WEAPON_FLAG_AIM_FREE,                    "flag_aim_free" ) 
    ADD_ENUM ( WEAPON_FLAG_MOVE_AND_AIM,                "flag_move_and_aim" ) 
    ADD_ENUM ( WEAPON_FLAG_MOVE_AND_SHOOT,              "flag_move_and_shoot" ) 
    ADD_ENUM ( WEAPON_FLAG_TYPE_THROW,                  "flag_type_throw" ) 
    ADD_ENUM ( WEAPON_FLAG_TYPE_HEAVY,                  "flag_type_heavy" ) 
    ADD_ENUM ( WEAPON_FLAG_TYPE_CONSTANT,               "flag_type_constant" ) 
    ADD_ENUM ( WEAPON_FLAG_TYPE_DUAL,                   "flag_type_dual" ) 
    ADD_ENUM ( WEAPON_FLAG_ANIM_RELOAD,                 "flag_anim_reload" ) 
    ADD_ENUM ( WEAPON_FLAG_ANIM_CROUCH,                 "flag_anim_crouch" ) 
    ADD_ENUM ( WEAPON_FLAG_ANIM_RELOAD_LOOP,            "flag_anim_reload_loop" ) 
    ADD_ENUM ( WEAPON_FLAG_ANIM_RELOAD_LONG,            "flag_anim_reload_long" ) 
    ADD_ENUM ( WEAPON_FLAG_SHOT_SLOWS,                  "flag_shot_slows" ) 
    ADD_ENUM ( WEAPON_FLAG_SHOT_RAND_SPEED,             "flag_shot_rand_speed" ) 
    ADD_ENUM ( WEAPON_FLAG_SHOT_ANIM_ABRUPT,            "flag_shot_anim_abrupt" ) 
    ADD_ENUM ( WEAPON_FLAG_SHOT_EXPANDS,                "flag_shot_expands" ) 
IMPLEMENT_ENUM_END( "weapon-property" )

IMPLEMENT_ENUM_BEGIN( eWeaponSkill )
    ADD_ENUM ( WEAPONSKILL_POOR,                            "poor" )
    ADD_ENUM ( WEAPONSKILL_STD,                             "std" )
    ADD_ENUM ( WEAPONSKILL_PRO,                             "pro" )
IMPLEMENT_ENUM_END( "weapon-skill" )

IMPLEMENT_ENUM_BEGIN( eWeaponState )
    ADD_ENUM ( WEAPONSTATE_READY,                           "ready" )
    ADD_ENUM ( WEAPONSTATE_FIRING,                          "firing" )
    ADD_ENUM ( WEAPONSTATE_RELOADING,                       "reloading" )
IMPLEMENT_ENUM_END( "weapon-state" )


IMPLEMENT_ENUM_BEGIN ( eWeaponFlags )
    ADD_ENUM ( WEAPONFLAGS_DISABLE_MODEL,          "disable_model")
    ADD_ENUM ( WEAPONFLAGS_FLAGS,                  "flags" )
    ADD_ENUM ( WEAPONFLAGS_INSTANT_RELOAD,         "instant_reload")
    ADD_ENUM ( WEAPONFLAGS_SHOOT_IF_OUT_OF_RANGE,  "shoot_if_out_of_range")
    ADD_ENUM ( WEAPONFLAGS_SHOOT_IF_TARGET_BOCKED, "shoot_if_blocked")
IMPLEMENT_ENUM_END( "weapon-flags" )

IMPLEMENT_ENUM_BEGIN( CAccessControlListRight::ERightType )
    ADD_ENUM ( CAccessControlListRight::RIGHT_TYPE_COMMAND,      "command" )
    ADD_ENUM ( CAccessControlListRight::RIGHT_TYPE_FUNCTION,     "function" )
    ADD_ENUM ( CAccessControlListRight::RIGHT_TYPE_RESOURCE,     "resource" )
    ADD_ENUM ( CAccessControlListRight::RIGHT_TYPE_GENERAL,      "general" )
IMPLEMENT_ENUM_END( "right-type" )

IMPLEMENT_ENUM_BEGIN( CElement::EElementType )
    ADD_ENUM( CElement::DUMMY,                  "dummy" )
    ADD_ENUM( CElement::PLAYER,                 "player" )
    ADD_ENUM( CElement::VEHICLE,                "vehicle" )
    ADD_ENUM( CElement::OBJECT,                 "object" )
    ADD_ENUM( CElement::MARKER,                 "marker" )
    ADD_ENUM( CElement::BLIP,                   "blip" )
    ADD_ENUM( CElement::PICKUP,                 "pickup" )
    ADD_ENUM( CElement::RADAR_AREA,             "radararea" )
    ADD_ENUM( CElement::SPAWNPOINT_DEPRECATED,  "spawnpoint_deprecated" )
    ADD_ENUM( CElement::REMOTECLIENT_DEPRECATED,"remoteclient_deprecated" )
    ADD_ENUM( CElement::CONSOLE,                "console" )
    ADD_ENUM( CElement::PATH_NODE_UNUSED,       "pathnode_unused" )
    ADD_ENUM( CElement::WORLD_MESH_UNUSED,      "worldmesh_unused" )
    ADD_ENUM( CElement::TEAM,                   "team" )
    ADD_ENUM( CElement::PED,                    "ped" )
    ADD_ENUM( CElement::COLSHAPE,               "colshape" )
    ADD_ENUM( CElement::SCRIPTFILE,             "scriptfile" )
    ADD_ENUM( CElement::WATER,                  "water" )
    ADD_ENUM( CElement::DATABASE_CONNECTION,    "db-connection" )
    ADD_ENUM( CElement::ROOT,                   "root" )
    ADD_ENUM( CElement::UNKNOWN,                "unknown" )
IMPLEMENT_ENUM_END_DEFAULTS( "element-type", CElement::UNKNOWN, "unknown" )



//
// Get best guess at name of userdata type
//
SString GetUserDataClassName ( void* ptr, lua_State* luaVM )
{
    // Try element
    if ( CElement* pElement = UserDataCast < CElement > ( (CElement*)NULL, ptr, NULL ) )
    {
        return pElement->GetTypeName ();
    }

    // Try xml node
    if ( CXMLNode* pXMLNode = UserDataCast < CXMLNode > ( (CXMLNode*)NULL, ptr, NULL ) )
    {
        return GetClassTypeName ( pXMLNode );
    }

    // Try timer
    if ( CLuaTimer* pLuaTimer = UserDataCast < CLuaTimer > ( (CLuaTimer*)NULL, ptr, luaVM ) )
    {
        return GetClassTypeName ( pLuaTimer );
    }

    // Try resource
    if ( CResource* pResource = UserDataCast < CResource > ( (CResource*)NULL, ptr, NULL ) )
    {
        return GetClassTypeName ( pResource );
    }

    return "";
}

//
// Read next as resource or resource name.  Result output as string
//
void MixedReadResourceString ( CScriptArgReader& argStream, SString& strOutResourceName )
{
    if ( !argStream.NextIsString () )
    {
        CResource* pResource;
        argStream.ReadUserData ( pResource );
        if ( pResource )
            strOutResourceName = pResource->GetName ();
    }
    else
        argStream.ReadString ( strOutResourceName );
}


//
// Read next as resource or resource name.  Result output as resource
//
void MixedReadResourceString ( CScriptArgReader& argStream, CResource*& pOutResource )
{
    if ( !argStream.NextIsString () )
    {
        argStream.ReadUserData ( pOutResource );
    }
    else
    {
        SString strResourceName;
        argStream.ReadString ( strResourceName );
        pOutResource = g_pGame->GetResourceManager ()->GetResource ( strResourceName );

        if ( !pOutResource )
            argStream.SetTypeError ( "resource", argStream.m_iIndex - 1 );
    }
}


///////////////////////////////////////////////////////////////
//
// StringToBool
//
// Convert a string into a best guess bool equivalent
//
///////////////////////////////////////////////////////////////
bool StringToBool ( const SString& strText )
{
    return ( strText == "true" || strText == "1" || strText == "yes" );
}


//
// Check min server is correct
//
void MinServerReqCheck ( CScriptArgReader& argStream, const char* szVersionReq, const char* szReason )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( argStream.m_luaVM );
    if ( pLuaMain )
    {
        CResource* pResource = pLuaMain->GetResource();
        if ( pResource )
        {
            if ( pResource->GetMinServerReqFromMetaXml () < szVersionReq )
            {
#if MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE
                argStream.SetVersionError ( szVersionReq, "server", szReason );
#endif
            }
        }
    }
}


//
// Read next as preg option flags
//
void ReadPregFlags( CScriptArgReader& argStream, pcrecpp::RE_Options& pOptions )
{
    if ( argStream.NextIsNumber() )
    {
        uint uiFlags = 0;
        argStream.ReadNumber ( uiFlags );
        pOptions.set_caseless ( ( uiFlags & 1 ) != 0 );
        pOptions.set_multiline ( ( uiFlags & 2 ) != 0 );
        pOptions.set_dotall ( ( uiFlags & 4 ) != 0 );
        pOptions.set_extended ( ( uiFlags & 8 ) != 0 );
    }
    else
    if ( argStream.NextIsString() )
    {
        SString strFlags;
        argStream.ReadString ( strFlags );
        for( uint i = 0 ; i < strFlags.length() ; i++ )
        {
            switch ( strFlags[i] )
            {
                case 'i':
                    pOptions.set_caseless ( true );
                    break;
                case 'm':
                    pOptions.set_multiline ( true );
                    break;
                case 'd':
                    pOptions.set_dotall ( true );
                    break;
                case 'e':
                    pOptions.set_extended ( true );
                    break;
                default:
                    argStream.SetCustomError( "Flags all wrong" );
                    return;       
            }
        }
    }
}


//
// Return true if weapon property is a flag type
//
bool IsWeaponPropertyFlag( eWeaponProperty weaponProperty )
{
    return ( weaponProperty >= WEAPON_FLAG_FIRST && weaponProperty <= WEAPON_FLAG_LAST );
}


//
// Get bit pattern for a weapon property flag
//
uint GetWeaponPropertyFlagBit( eWeaponProperty weaponProperty )
{
    if ( !IsWeaponPropertyFlag( weaponProperty ) )
        return 0;

    // Check 20 bits from first to last
    dassert( WEAPON_FLAG_LAST + 1 - WEAPON_FLAG_FIRST == 20 );

    uint uiFlagIndex = ( weaponProperty - WEAPON_FLAG_FIRST );
    uint uiFlagBit = 1 << uiFlagIndex;
    return uiFlagBit;
}
