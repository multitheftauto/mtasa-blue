/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/mods/shared_logic/lua/CLuaFunctionParseHelpers.cpp
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*****************************************************************************/

#include "StdInc.h"
#define MIN_SERVER_REQ_CANSUPPLY_MINCLIENTREQ    "1.3.0-9.04431"

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


IMPLEMENT_ENUM_BEGIN( CGUIVerticalAlign )
    ADD_ENUM ( CGUI_ALIGN_TOP,              "top" )
    ADD_ENUM ( CGUI_ALIGN_BOTTOM,           "bottom" )
    ADD_ENUM ( CGUI_ALIGN_VERTICALCENTER,   "center" )
IMPLEMENT_ENUM_END( "vertical-align" )


IMPLEMENT_ENUM_BEGIN( CGUIHorizontalAlign )
    ADD_ENUM ( CGUI_ALIGN_LEFT,             "left" )
    ADD_ENUM ( CGUI_ALIGN_RIGHT,            "right" )
    ADD_ENUM ( CGUI_ALIGN_HORIZONTALCENTER, "center" )
IMPLEMENT_ENUM_END( "horizontal-align" )


IMPLEMENT_ENUM_BEGIN( eInputMode )
    ADD_ENUM ( INPUTMODE_ALLOW_BINDS,       "allow_binds" )
    ADD_ENUM ( INPUTMODE_NO_BINDS,          "no_binds" )
    ADD_ENUM ( INPUTMODE_NO_BINDS_ON_EDIT,  "no_binds_when_editing" )
IMPLEMENT_ENUM_END( "input-mode" )


IMPLEMENT_ENUM_BEGIN( eAccessType )
    ADD_ENUM ( ACCESS_PUBLIC,               "public" )
    ADD_ENUM ( ACCESS_PRIVATE,              "private" )
IMPLEMENT_ENUM_END( "access-type" )


IMPLEMENT_ENUM_BEGIN( eDXHorizontalAlign )
    ADD_ENUM ( DX_ALIGN_LEFT,           "left" )
    ADD_ENUM ( DX_ALIGN_HCENTER,        "center" )
    ADD_ENUM ( DX_ALIGN_RIGHT,          "right" )
IMPLEMENT_ENUM_END( "horizontal-align" )


IMPLEMENT_ENUM_BEGIN( eDXVerticalAlign )
    ADD_ENUM ( DX_ALIGN_TOP,            "top" )
    ADD_ENUM ( DX_ALIGN_VCENTER,        "center" )
    ADD_ENUM ( DX_ALIGN_BOTTOM,         "bottom" )
IMPLEMENT_ENUM_END( "vertical-align" )


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

IMPLEMENT_ENUM_BEGIN( eAmbientSoundType )
    ADD_ENUM ( AMBIENT_SOUND_GENERAL,           "general" )
    ADD_ENUM ( AMBIENT_SOUND_GUNFIRE,           "gunfire" )
IMPLEMENT_ENUM_END( "ambient-sound-type" )

IMPLEMENT_ENUM_BEGIN( eCGUIType )
    ADD_ENUM ( CGUI_BUTTON,         "gui-button" )
    ADD_ENUM ( CGUI_CHECKBOX,       "gui-checkbox" )
    ADD_ENUM ( CGUI_EDIT,           "gui-edit" )
    ADD_ENUM ( CGUI_GRIDLIST,       "gui-gridlist" )
    ADD_ENUM ( CGUI_LABEL,          "gui-label" )
    ADD_ENUM ( CGUI_MEMO,           "gui-memo" )
    ADD_ENUM ( CGUI_PROGRESSBAR,    "gui-progressbar" )
    ADD_ENUM ( CGUI_RADIOBUTTON,    "gui-radiobutton" )
    ADD_ENUM ( CGUI_STATICIMAGE,    "gui-staticimage" )
    ADD_ENUM ( CGUI_TAB,            "gui-tab" )
    ADD_ENUM ( CGUI_TABPANEL,       "gui-tabpanel" )
    ADD_ENUM ( CGUI_WINDOW,         "gui-window" )
    ADD_ENUM ( CGUI_SCROLLPANE,     "gui-scrollpane" )
    ADD_ENUM ( CGUI_SCROLLBAR,      "gui-scrollbar" )
    ADD_ENUM ( CGUI_COMBOBOX,       "gui-combobox" )
IMPLEMENT_ENUM_END( "gui-type" )

IMPLEMENT_ENUM_BEGIN( eDxTestMode )
    ADD_ENUM ( DX_TEST_MODE_NONE,           "none" )
    ADD_ENUM ( DX_TEST_MODE_NO_MEM,         "no_mem" )
    ADD_ENUM ( DX_TEST_MODE_LOW_MEM,        "low_mem" )
    ADD_ENUM ( DX_TEST_MODE_NO_SHADER,      "no_shader" )
IMPLEMENT_ENUM_END( "dx-test-mode" )

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
    ADD_ENUM ( WEAPON_LIFE_SPAN,                        "life_span" ) // Disabled for release
    ADD_ENUM ( WEAPON_FIRING_SPEED,                     "firing_speed" ) // Disabled for release
    ADD_ENUM ( WEAPON_SPREAD,                           "spread" ) // Disabled for release
    ADD_ENUM ( WEAPON_MAX_CLIP_AMMO,                    "maximum_clip_ammo" )
    ADD_ENUM ( WEAPON_MOVE_SPEED,                       "move_speed" )
    ADD_ENUM ( WEAPON_FLAGS,                            "flags" )
    ADD_ENUM ( WEAPON_ANIM_GROUP,                       "anim_group" ) // Disabled for release

    ADD_ENUM ( WEAPON_ANIM_LOOP_START,                  "anim_loop_start" ) 
    ADD_ENUM ( WEAPON_ANIM_LOOP_STOP,                   "anim_loop_stop" ) 
    ADD_ENUM ( WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME,    "anim_loop_bullet_fire" ) 

    ADD_ENUM ( WEAPON_ANIM2_LOOP_START,                 "anim2_loop_start" ) 
    ADD_ENUM ( WEAPON_ANIM2_LOOP_STOP,                  "anim2_loop_stop" ) 
    ADD_ENUM ( WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME,   "anim2_loop_bullet_fire" ) 

    ADD_ENUM ( WEAPON_ANIM_BREAKOUT_TIME,               "anim_breakout_time" ) 

    // Set is Disabled >= FIRETYPE - Move above WEAPON_TYPE_SET_DISABLED to re-enable and move it above here to make it less confusing.
    ADD_ENUM ( WEAPON_FIRETYPE,                         "fire_type" )

    ADD_ENUM ( WEAPON_MODEL,                            "model" )
    ADD_ENUM ( WEAPON_MODEL2,                           "model2" ) 
    
    ADD_ENUM ( WEAPON_SLOT,                             "weapon_slot" ) 

    ADD_ENUM ( WEAPON_FIRE_OFFSET,                      "fire_offset" ) 
    
    ADD_ENUM ( WEAPON_SKILL_LEVEL,                      "skill_level" ) 
    ADD_ENUM ( WEAPON_REQ_SKILL_LEVEL,                  "required_skill_level" ) 
    
    ADD_ENUM ( WEAPON_RADIUS,                           "radius" ) 
    
    ADD_ENUM ( WEAPON_AIM_OFFSET,                       "aim_offset" ) 

    ADD_ENUM ( WEAPON_DEFAULT_COMBO,                    "default_combo" ) 
    ADD_ENUM ( WEAPON_COMBOS_AVAILABLE,                 "combos_available" ) 
IMPLEMENT_ENUM_END( "weapon-property" )

IMPLEMENT_ENUM_BEGIN( eWeaponSkill )
ADD_ENUM ( WEAPONSKILL_POOR,                            "poor" )
ADD_ENUM ( WEAPONSKILL_STD,                             "std" )
ADD_ENUM ( WEAPONSKILL_PRO,                             "pro" )
IMPLEMENT_ENUM_END( "weapon-skill" )

IMPLEMENT_ENUM_BEGIN( ERenderFormat )
    ADD_ENUM ( RFORMAT_UNKNOWN,      "unknown" )
    ADD_ENUM ( RFORMAT_ARGB,         "argb" )
    ADD_ENUM ( RFORMAT_DXT1,         "dxt1" )
    ADD_ENUM ( RFORMAT_DXT2,         "dxt2" )
    ADD_ENUM ( RFORMAT_DXT3,         "dxt3" )
    ADD_ENUM ( RFORMAT_DXT4,         "dxt4" )
    ADD_ENUM ( RFORMAT_DXT5,         "dxt5" )
    ADD_ENUM ( RFORMAT_INTZ,         "intz" )
    ADD_ENUM ( RFORMAT_DF24,         "df24" )
    ADD_ENUM ( RFORMAT_DF16,         "df16" )
    ADD_ENUM ( RFORMAT_RAWZ,         "rawz" )
IMPLEMENT_ENUM_END( "texture-format" )

IMPLEMENT_ENUM_BEGIN( ETextureType )
    ADD_ENUM ( TTYPE_TEXTURE,           "2d" )
    ADD_ENUM ( TTYPE_VOLUMETEXTURE,     "3d" )
    ADD_ENUM ( TTYPE_CUBETEXTURE,       "cube" )
IMPLEMENT_ENUM_END( "texture-type" )

IMPLEMENT_ENUM_BEGIN( ETextureAddress )
    ADD_ENUM ( TADDRESS_WRAP,           "wrap" )
    ADD_ENUM ( TADDRESS_MIRROR,         "mirror" )
    ADD_ENUM ( TADDRESS_CLAMP,          "clamp" )
    ADD_ENUM ( TADDRESS_BORDER,         "border" )
    ADD_ENUM ( TADDRESS_MIRRORONCE,     "mirror-once" )
IMPLEMENT_ENUM_END( "texture-edge" )

IMPLEMENT_ENUM_BEGIN( EPixelsFormatType )
    ADD_ENUM ( EPixelsFormat::UNKNOWN,      "unknown" )
    ADD_ENUM ( EPixelsFormat::PLAIN,        "plain" )
    ADD_ENUM ( EPixelsFormat::JPEG,         "jpeg" )
    ADD_ENUM ( EPixelsFormat::PNG,          "png" )
IMPLEMENT_ENUM_END( "pixel-format" )

IMPLEMENT_ENUM_BEGIN( EBlendModeType )
    ADD_ENUM ( EBlendMode::BLEND,           "blend" )
    ADD_ENUM ( EBlendMode::ADD,             "add" )
    ADD_ENUM ( EBlendMode::MODULATE_ADD,    "modulate_add" )
    ADD_ENUM ( EBlendMode::OVERWRITE,       "overwrite" )
IMPLEMENT_ENUM_END( "blend-mode" )

IMPLEMENT_ENUM_BEGIN( EEntityTypeMask )
    ADD_ENUM ( TYPE_MASK_NONE,          "none" )
    ADD_ENUM ( TYPE_MASK_WORLD,         "world" )
    ADD_ENUM ( TYPE_MASK_PED,           "ped" )
    ADD_ENUM ( TYPE_MASK_VEHICLE,       "vehicle" )
    ADD_ENUM ( TYPE_MASK_OBJECT,        "object" )
    ADD_ENUM ( TYPE_MASK_OTHER,         "other" )
    ADD_ENUM ( TYPE_MASK_ALL,           "all" )
IMPLEMENT_ENUM_END( "entity-type-mask" )

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

IMPLEMENT_ENUM_BEGIN( eVehicleComponent )
    ADD_ENUM ( VEHICLE_COMPONENT_SPECIAL_1,                     "special_1" )
    ADD_ENUM ( VEHICLE_COMPONENT_SPECIAL_2,                     "special_2" )
    ADD_ENUM ( VEHICLE_COMPONENT_SPECIAL_3,                     "special_3" )
    ADD_ENUM ( VEHICLE_COMPONENT_SPECIAL_4,                     "special_4" )
    ADD_ENUM ( VEHICLE_COMPONENT_SPECIAL_5,                     "special_5" )
    ADD_ENUM ( VEHICLE_COMPONENT_BONET,                         "bonnet" )
    ADD_ENUM ( VEHICLE_COMPONENT_BOOT,                          "boot" )
    ADD_ENUM ( VEHICLE_COMPONENT_BUMPER_1,                      "bumper_1" )
    ADD_ENUM ( VEHICLE_COMPONENT_BUMPER_2,                      "bumper_2" )
    ADD_ENUM ( VEHICLE_COMPONENT_CHASIS,                        "chassis" )
    ADD_ENUM ( VEHICLE_COMPONENT_DOOR_1,                        "door_1" )
    ADD_ENUM ( VEHICLE_COMPONENT_DOOR_2,                        "door_2" )
    ADD_ENUM ( VEHICLE_COMPONENT_DOOR_3,                        "door_3" )
    ADD_ENUM ( VEHICLE_COMPONENT_DOOR_4,                        "door_4" )
    ADD_ENUM ( VEHICLE_COMPONENT_EXHAUST,                       "exhaust" )
    ADD_ENUM ( VEHICLE_COMPONENT_WINGLF,                        "wing_lf" )
    ADD_ENUM ( VEHICLE_COMPONENT_WINGRF,                        "wing_rf" )
    ADD_ENUM ( VEHICLE_COMPONENT_EXTRA_1,                       "extra_1" )
    ADD_ENUM ( VEHICLE_COMPONENT_EXTRA_2,                       "extra_2" )
IMPLEMENT_ENUM_END( "vehicle-component" )

IMPLEMENT_ENUM_BEGIN( eFontType )
    ADD_ENUM( FONT_DEFAULT,         "default" )
    ADD_ENUM( FONT_DEFAULT_BOLD,    "default-bold" )
    ADD_ENUM( FONT_CLEAR,           "clear" )
    ADD_ENUM( FONT_ARIAL,           "arial" )
    ADD_ENUM( FONT_SANS,            "sans" )
    ADD_ENUM( FONT_PRICEDOWN,       "pricedown" )
    ADD_ENUM( FONT_BANKGOTHIC,      "bankgothic" )
    ADD_ENUM( FONT_DIPLOMA,         "diploma" )
    ADD_ENUM( FONT_BECKETT,         "beckett" )
IMPLEMENT_ENUM_END_DEFAULTS( "font-type", FONT_DEFAULT, "" )

IMPLEMENT_ENUM_BEGIN ( eAudioLookupIndex )
    ADD_ENUM ( AUDIO_LOOKUP_FEET, "feet" )
    ADD_ENUM ( AUDIO_LOOKUP_GENRL, "genrl" )
    ADD_ENUM ( AUDIO_LOOKUP_PAIN_A, "pain_a" )
    ADD_ENUM ( AUDIO_LOOKUP_SCRIPT, "script" )
    ADD_ENUM ( AUDIO_LOOKUP_SPC_EA, "spc_ea" )
    ADD_ENUM ( AUDIO_LOOKUP_SPC_FA, "spc_fa" )
    ADD_ENUM ( AUDIO_LOOKUP_SPC_GA, "spc_ga" )
    ADD_ENUM ( AUDIO_LOOKUP_SPC_NA, "spc_na" )
    ADD_ENUM ( AUDIO_LOOKUP_SPC_PA, "spc_pa" )
IMPLEMENT_ENUM_END ( "audio-lookup-index" )

IMPLEMENT_ENUM_BEGIN ( eAspectRatio )
    ADD_ENUM ( ASPECT_RATIO_AUTO, "auto" )
    ADD_ENUM ( ASPECT_RATIO_4_3, "4:3" )
    ADD_ENUM ( ASPECT_RATIO_16_10, "16:10" )
    ADD_ENUM ( ASPECT_RATIO_16_9, "16:9" )
IMPLEMENT_ENUM_END ( "aspectratio" )


//
// Get best guess at name of userdata type
//
SString GetUserDataClassName ( void* ptr, lua_State* luaVM )
{
    // Try element
    if ( CClientEntity* pClientElement = UserDataCast < CClientEntity > ( (CClientEntity*)NULL, ptr, NULL ) )
    {
        // Try gui element
        if ( CClientGUIElement* pGuiElement = DynamicCast < CClientGUIElement > ( pClientElement ) )
        {
            return EnumToString ( pGuiElement->GetCGUIElement ()->GetType () );
        }
        return pClientElement->GetTypeName ();
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
// Reading mixed types
//

//
// DxFont/string
//
void MixedReadDxFontString ( CScriptArgReader& argStream, eFontType& outFontType, eFontType defaultFontType, CClientDxFont*& poutDxFontElement )
{
    outFontType = FONT_DEFAULT;
    poutDxFontElement = NULL;
    if ( argStream.NextIsNone () )
        return;
    else
    if ( argStream.NextIsString () )
    {
        SString strFontName;
        argStream.ReadString ( strFontName );
        StringToEnum( strFontName, outFontType );
        return;
    }
    else
        argStream.ReadUserData ( poutDxFontElement );
}


//
// GuiFont/string
//
void MixedReadGuiFontString ( CScriptArgReader& argStream, SString& strOutFontName, const char* szDefaultFontName, CClientGuiFont*& poutGuiFontElement )
{
    poutGuiFontElement = NULL;
    if ( argStream.NextIsString () || argStream.NextIsNone () )
        argStream.ReadString ( strOutFontName, szDefaultFontName );
    else
        argStream.ReadUserData ( poutGuiFontElement );
}


//
// Material/string
//
void MixedReadMaterialString ( CScriptArgReader& argStream, CClientMaterial*& pMaterialElement )
{
    pMaterialElement = NULL;
    if ( !argStream.NextIsString () )
        argStream.ReadUserData ( pMaterialElement );
    else
    {
        SString strFilePath;
        argStream.ReadString ( strFilePath );

        // If no element, auto find/create one
        CLuaMain* pLuaMain = g_pClientGame->GetLuaManager ()->GetVirtualMachine ( argStream.m_luaVM );
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( pParentResource )
        {
            CResource* pFileResource = pParentResource;
            SString strPath, strMetaPath;
            if ( CResourceManager::ParseResourcePathInput( strFilePath, pFileResource, strPath, strMetaPath ) )
            {
                SString strUniqueName = SString ( "%s*%s*%s", pParentResource->GetName (), pFileResource->GetName (), strMetaPath.c_str () ).Replace ( "\\", "/" );
                pMaterialElement = g_pClientGame->GetManager ()->GetRenderElementManager ()->FindAutoTexture ( strPath, strUniqueName );

                // Check if brand new
                if ( pMaterialElement && !pMaterialElement->GetParent () )
                {
                    // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                    pMaterialElement->SetParent ( pParentResource->GetResourceDynamicEntity() );
                }
                else
                    argStream.SetCustomError( strFilePath, "Error loading image" );
            }
            else
                argStream.SetCustomError( strFilePath, "Bad file path" );
        }
    }
}


//
// 4x4 matrix into CMatrix
//
bool ReadMatrix ( lua_State* luaVM, uint uiArgIndex, CMatrix& outMatrix )
{
    float m[4][4] = { { 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { 0,0,0,1 } };
    uint uiRow = 0;
    uint uiCell = 0;

    if ( lua_type ( luaVM, uiArgIndex ) == LUA_TTABLE )
    {
        for ( lua_pushnil ( luaVM ) ; lua_next ( luaVM, uiArgIndex ) != 0 ; lua_pop ( luaVM, 1 ), uiRow++ )
        {
            //int idx = lua_tonumber ( luaVM, -2 );
            //int iArgumentType = lua_type ( luaVM, -1 );
            if ( lua_type ( luaVM, -1 ) == LUA_TTABLE )
            {
                uint uiCol = 0;
                for ( lua_pushnil ( luaVM ) ; lua_next ( luaVM, -2 ) != 0 ; lua_pop ( luaVM, 1 ), uiCol++, uiCell++ )
                {
                    //int idx = lua_tonumber ( luaVM, -2 );
                    int iArgumentType = lua_type ( luaVM, -1 );
                    if ( iArgumentType == LUA_TNUMBER || iArgumentType == LUA_TSTRING )
                    {
                        if ( uiRow < 4 && uiCol < 4 )
                            m[uiRow][uiCol] = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );;
                    }
                }

                if ( uiCol != 4 )
                    return false;
            }
        }
    }

    if ( uiRow != 4 || uiCell != 16 )
        return false;

    outMatrix.vRight = CVector ( m[0][0], m[0][1], m[0][2] );
    outMatrix.vFront = CVector ( m[1][0], m[1][1], m[1][2] );
    outMatrix.vUp    = CVector ( m[2][0], m[2][1], m[2][2] );
    outMatrix.vPos   = CVector ( m[3][0], m[3][1], m[3][2] );
    return true;
}


//
// Check min client is correct
//
void MinClientReqCheck ( CScriptArgReader& argStream, const char* szVersionReq, const char* szReason )
{
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine ( argStream.m_luaVM );
    if ( pLuaMain )
    {
        CResource* pResource = pLuaMain->GetResource();
        if ( pResource )
        {
            if ( pResource->GetMinClientReq () < szVersionReq )
            {
                if ( MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE )
                {
                    // Check server is able to give us a MinClientReq
                    if ( pResource->GetMinServerReq () < MIN_SERVER_REQ_CANSUPPLY_MINCLIENTREQ )
                        argStream.SetVersionError ( MIN_SERVER_REQ_CANSUPPLY_MINCLIENTREQ, "server", "of technical reasons" );
                    else
                        argStream.SetVersionError ( szVersionReq, "client", szReason );
                }
            }
        }
    }
}
