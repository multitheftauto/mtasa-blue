/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/lua/CLuaFunctionParseHelpers.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CSettings.h>

//
// enum values <-> script strings
//

IMPLEMENT_ENUM_BEGIN(eLuaType)
ADD_ENUM(LUA_TNONE, "none")
ADD_ENUM(LUA_TNIL, "nil")
ADD_ENUM(LUA_TBOOLEAN, "boolean")
ADD_ENUM(LUA_TLIGHTUSERDATA, "lightuserdata")
ADD_ENUM(LUA_TNUMBER, "number")
ADD_ENUM(LUA_TSTRING, "string")
ADD_ENUM(LUA_TTABLE, "table")
ADD_ENUM(LUA_TFUNCTION, "function")
ADD_ENUM(LUA_TUSERDATA, "userdata")
ADD_ENUM(LUA_TTHREAD, "thread")
IMPLEMENT_ENUM_END("lua-type")

IMPLEMENT_ENUM_BEGIN(CGUIVerticalAlign)
ADD_ENUM(CGUI_ALIGN_TOP, "top")
ADD_ENUM(CGUI_ALIGN_BOTTOM, "bottom")
ADD_ENUM(CGUI_ALIGN_VERTICALCENTER, "center")
IMPLEMENT_ENUM_END("vertical-align")

IMPLEMENT_ENUM_BEGIN(CGUIHorizontalAlign)
ADD_ENUM(CGUI_ALIGN_LEFT, "left")
ADD_ENUM(CGUI_ALIGN_RIGHT, "right")
ADD_ENUM(CGUI_ALIGN_HORIZONTALCENTER, "center")
IMPLEMENT_ENUM_END("horizontal-align")

IMPLEMENT_ENUM_BEGIN(eInputMode)
ADD_ENUM(INPUTMODE_ALLOW_BINDS, "allow_binds")
ADD_ENUM(INPUTMODE_NO_BINDS, "no_binds")
ADD_ENUM(INPUTMODE_NO_BINDS_ON_EDIT, "no_binds_when_editing")
IMPLEMENT_ENUM_END("input-mode")

IMPLEMENT_ENUM_BEGIN(eAccessType)
ADD_ENUM(ACCESS_PUBLIC, "public")
ADD_ENUM(ACCESS_PRIVATE, "private")
IMPLEMENT_ENUM_END("access-type")

IMPLEMENT_ENUM_BEGIN(eDXHorizontalAlign)
ADD_ENUM(DX_ALIGN_LEFT, "left")
ADD_ENUM(DX_ALIGN_HCENTER, "center")
ADD_ENUM(DX_ALIGN_RIGHT, "right")
IMPLEMENT_ENUM_END("horizontal-align")

IMPLEMENT_ENUM_BEGIN(eDXVerticalAlign)
ADD_ENUM(DX_ALIGN_TOP, "top")
ADD_ENUM(DX_ALIGN_VCENTER, "center")
ADD_ENUM(DX_ALIGN_BOTTOM, "bottom")
IMPLEMENT_ENUM_END("vertical-align")

IMPLEMENT_ENUM_BEGIN(TrafficLight::EColor)
ADD_ENUM(TrafficLight::GREEN, "green")
ADD_ENUM(TrafficLight::YELLOW, "yellow")
ADD_ENUM(TrafficLight::RED, "red")
IMPLEMENT_ENUM_END("traffic-light-color")

IMPLEMENT_ENUM_BEGIN(TrafficLight::EState)
ADD_ENUM(TrafficLight::AUTO, "auto")
ADD_ENUM(TrafficLight::DISABLED, "disabled")
IMPLEMENT_ENUM_END("traffic-light-state")

IMPLEMENT_ENUM_BEGIN(CEasingCurve::eType)
ADD_ENUM(CEasingCurve::Linear, "Linear")
ADD_ENUM(CEasingCurve::InQuad, "InQuad")
ADD_ENUM(CEasingCurve::OutQuad, "OutQuad")
ADD_ENUM(CEasingCurve::InOutQuad, "InOutQuad")
ADD_ENUM(CEasingCurve::OutInQuad, "OutInQuad")
ADD_ENUM(CEasingCurve::InElastic, "InElastic")
ADD_ENUM(CEasingCurve::OutElastic, "OutElastic")
ADD_ENUM(CEasingCurve::InOutElastic, "InOutElastic")
ADD_ENUM(CEasingCurve::OutInElastic, "OutInElastic")
ADD_ENUM(CEasingCurve::InBack, "InBack")
ADD_ENUM(CEasingCurve::OutBack, "OutBack")
ADD_ENUM(CEasingCurve::InOutBack, "InOutBack")
ADD_ENUM(CEasingCurve::OutInBack, "OutInBack")
ADD_ENUM(CEasingCurve::InBounce, "InBounce")
ADD_ENUM(CEasingCurve::OutBounce, "OutBounce")
ADD_ENUM(CEasingCurve::InOutBounce, "InOutBounce")
ADD_ENUM(CEasingCurve::OutInBounce, "OutInBounce")
ADD_ENUM(CEasingCurve::SineCurve, "SineCurve")
ADD_ENUM(CEasingCurve::CosineCurve, "CosineCurve")
IMPLEMENT_ENUM_END_DEFAULTS("easing-type", CEasingCurve::EASING_INVALID, "Invalid")

IMPLEMENT_ENUM_BEGIN(eHudComponent)
ADD_ENUM(HUD_AMMO, "ammo")
ADD_ENUM(HUD_WEAPON, "weapon")
ADD_ENUM(HUD_HEALTH, "health")
ADD_ENUM(HUD_BREATH, "breath")
ADD_ENUM(HUD_ARMOUR, "armour")
ADD_ENUM(HUD_MONEY, "money")
ADD_ENUM(HUD_VEHICLE_NAME, "vehicle_name")
ADD_ENUM(HUD_AREA_NAME, "area_name")
ADD_ENUM(HUD_RADAR, "radar")
ADD_ENUM(HUD_CLOCK, "clock")
ADD_ENUM(HUD_RADIO, "radio")
ADD_ENUM(HUD_WANTED, "wanted")
ADD_ENUM(HUD_CROSSHAIR, "crosshair")
ADD_ENUM(HUD_ALL, "all")
IMPLEMENT_ENUM_END("hud-component")

IMPLEMENT_ENUM_CLASS_BEGIN(eHudComponentProperty)
ADD_ENUM(eHudComponentProperty::POSITION, "position")
ADD_ENUM(eHudComponentProperty::SIZE, "size")
ADD_ENUM(eHudComponentProperty::FILL_COLOR, "fillColor")
ADD_ENUM(eHudComponentProperty::FILL_COLOR_SECONDARY, "fillColorSecondary")
ADD_ENUM(eHudComponentProperty::DRAW_BLACK_BORDER, "drawBlackBorder")
ADD_ENUM(eHudComponentProperty::DRAW_PERCENTAGE, "drawPercentage")
ADD_ENUM(eHudComponentProperty::BLINKING_HP_VALUE, "blinkingValue")
ADD_ENUM(eHudComponentProperty::DROP_COLOR, "dropColor")
ADD_ENUM(eHudComponentProperty::TEXT_OUTLINE, "fontOutline")
ADD_ENUM(eHudComponentProperty::TEXT_SHADOW, "fontShadow")
ADD_ENUM(eHudComponentProperty::TEXT_STYLE, "fontStyle")
ADD_ENUM(eHudComponentProperty::TEXT_ALIGNMENT, "fontAlignment")
ADD_ENUM(eHudComponentProperty::TEXT_PROPORTIONAL, "proportional")
ADD_ENUM(eHudComponentProperty::CUSTOM_ALPHA, "useCustomAlpha")
ADD_ENUM(eHudComponentProperty::TEXT_SIZE, "textSize")
ADD_ENUM(eHudComponentProperty::ALL_PROPERTIES, "all")
IMPLEMENT_ENUM_CLASS_END("hud-component-property")

IMPLEMENT_ENUM_CLASS_BEGIN(eFontStyle)
ADD_ENUM(eFontStyle::FONT_GOTHIC, "gothic")
ADD_ENUM(eFontStyle::FONT_MENU, "menu")
ADD_ENUM(eFontStyle::FONT_PRICEDOWN, "pricedown")
ADD_ENUM(eFontStyle::FONT_SUBTITLES, "subtitles")
IMPLEMENT_ENUM_CLASS_END("hud-component-font-style")

IMPLEMENT_ENUM_CLASS_BEGIN(eFontAlignment)
ADD_ENUM(eFontAlignment::ALIGN_CENTER, "center")
ADD_ENUM(eFontAlignment::ALIGN_LEFT, "left")
ADD_ENUM(eFontAlignment::ALIGN_RIGHT, "right")
IMPLEMENT_ENUM_CLASS_END("hud-component-font-alignment")

IMPLEMENT_ENUM_BEGIN(eAmbientSoundType)
ADD_ENUM(AMBIENT_SOUND_GENERAL, "general")
ADD_ENUM(AMBIENT_SOUND_GUNFIRE, "gunfire")
IMPLEMENT_ENUM_END("ambient-sound-type")

IMPLEMENT_ENUM_BEGIN(eCGUIType)
ADD_ENUM(CGUI_BUTTON, "gui-button")
ADD_ENUM(CGUI_CHECKBOX, "gui-checkbox")
ADD_ENUM(CGUI_EDIT, "gui-edit")
ADD_ENUM(CGUI_GRIDLIST, "gui-gridlist")
ADD_ENUM(CGUI_LABEL, "gui-label")
ADD_ENUM(CGUI_MEMO, "gui-memo")
ADD_ENUM(CGUI_PROGRESSBAR, "gui-progressbar")
ADD_ENUM(CGUI_RADIOBUTTON, "gui-radiobutton")
ADD_ENUM(CGUI_STATICIMAGE, "gui-staticimage")
ADD_ENUM(CGUI_TAB, "gui-tab")
ADD_ENUM(CGUI_TABPANEL, "gui-tabpanel")
ADD_ENUM(CGUI_WINDOW, "gui-window")
ADD_ENUM(CGUI_SCROLLPANE, "gui-scrollpane")
ADD_ENUM(CGUI_SCROLLBAR, "gui-scrollbar")
ADD_ENUM(CGUI_COMBOBOX, "gui-combobox")
ADD_ENUM(CGUI_WEBBROWSER, "gui-browser")
IMPLEMENT_ENUM_END("gui-type")

IMPLEMENT_ENUM_BEGIN(eDxTestMode)
ADD_ENUM(DX_TEST_MODE_NONE, "none")
ADD_ENUM(DX_TEST_MODE_NO_MEM, "no_mem")
ADD_ENUM(DX_TEST_MODE_LOW_MEM, "low_mem")
ADD_ENUM(DX_TEST_MODE_NO_SHADER, "no_shader")
IMPLEMENT_ENUM_END("dx-test-mode")

IMPLEMENT_ENUM_BEGIN(eWeaponType)
// Compatible with getWeaponNameFromID                          From setWeaponProperty before r4523
ADD_ENUM(WEAPONTYPE_UNARMED, "fist")
ADD_ENUM(WEAPONTYPE_BRASSKNUCKLE, "brass_knuckle")
ADD_ENUM(WEAPONTYPE_BRASSKNUCKLE, "brassknuckle")
ADD_ENUM(WEAPONTYPE_GOLFCLUB, "golfclub")
ADD_ENUM(WEAPONTYPE_NIGHTSTICK, "nightstick")
ADD_ENUM(WEAPONTYPE_KNIFE, "knife")
ADD_ENUM(WEAPONTYPE_BASEBALLBAT, "baseball_bat")
ADD_ENUM(WEAPONTYPE_BASEBALLBAT, "bat")
ADD_ENUM(WEAPONTYPE_SHOVEL, "shovel")
ADD_ENUM(WEAPONTYPE_POOL_CUE, "pool_cue")
ADD_ENUM(WEAPONTYPE_POOL_CUE, "poolstick")
ADD_ENUM(WEAPONTYPE_KATANA, "katana")
ADD_ENUM(WEAPONTYPE_CHAINSAW, "chainsaw")
ADD_ENUM(WEAPONTYPE_DILDO1, "dildo1")
ADD_ENUM(WEAPONTYPE_DILDO2, "dildo2")
ADD_ENUM(WEAPONTYPE_DILDO2, "dildo")
ADD_ENUM(WEAPONTYPE_DILDO1, "dildo")
ADD_ENUM(WEAPONTYPE_VIBE1, "vibe1")
ADD_ENUM(WEAPONTYPE_VIBE2, "vibe2")
ADD_ENUM(WEAPONTYPE_VIBE2, "vibrator")
ADD_ENUM(WEAPONTYPE_VIBE1, "vibrator")
ADD_ENUM(WEAPONTYPE_FLOWERS, "flowers")
ADD_ENUM(WEAPONTYPE_FLOWERS, "flower")
ADD_ENUM(WEAPONTYPE_CANE, "cane")
ADD_ENUM(WEAPONTYPE_GRENADE, "grenade")
ADD_ENUM(WEAPONTYPE_TEARGAS, "teargas")
ADD_ENUM(WEAPONTYPE_MOLOTOV, "molotov")
ADD_ENUM(WEAPONTYPE_PISTOL, "pistol")
ADD_ENUM(WEAPONTYPE_PISTOL, "colt 45")
ADD_ENUM(WEAPONTYPE_PISTOL_SILENCED, "silenced_pistol")
ADD_ENUM(WEAPONTYPE_PISTOL_SILENCED, "silenced")
ADD_ENUM(WEAPONTYPE_DESERT_EAGLE, "desert_eagle")
ADD_ENUM(WEAPONTYPE_DESERT_EAGLE, "deagle")
ADD_ENUM(WEAPONTYPE_SHOTGUN, "shotgun")
ADD_ENUM(WEAPONTYPE_SAWNOFF_SHOTGUN, "sawnoff_shotgun")
ADD_ENUM(WEAPONTYPE_SAWNOFF_SHOTGUN, "sawed-off")
ADD_ENUM(WEAPONTYPE_SPAS12_SHOTGUN, "spas12_shotgun")
ADD_ENUM(WEAPONTYPE_SPAS12_SHOTGUN, "combat shotgun")
ADD_ENUM(WEAPONTYPE_MICRO_UZI, "uzi")
ADD_ENUM(WEAPONTYPE_MP5, "mp5")
ADD_ENUM(WEAPONTYPE_AK47, "ak47")
ADD_ENUM(WEAPONTYPE_AK47, "ak-47")
ADD_ENUM(WEAPONTYPE_M4, "m4")
ADD_ENUM(WEAPONTYPE_TEC9, "tec9")
ADD_ENUM(WEAPONTYPE_TEC9, "tec-9")
ADD_ENUM(WEAPONTYPE_COUNTRYRIFLE, "rifle")
ADD_ENUM(WEAPONTYPE_SNIPERRIFLE, "sniper_rifle")
ADD_ENUM(WEAPONTYPE_SNIPERRIFLE, "sniper")
ADD_ENUM(WEAPONTYPE_ROCKETLAUNCHER, "rocket_launcher")
ADD_ENUM(WEAPONTYPE_ROCKETLAUNCHER, "rocket launcher")
ADD_ENUM(WEAPONTYPE_ROCKETLAUNCHER_HS, "rocket_launcher_hs")
ADD_ENUM(WEAPONTYPE_ROCKETLAUNCHER_HS, "rocket launcher hs")
ADD_ENUM(WEAPONTYPE_FLAMETHROWER, "flamethrower")
ADD_ENUM(WEAPONTYPE_MINIGUN, "minigun")
ADD_ENUM(WEAPONTYPE_REMOTE_SATCHEL_CHARGE, "satchel")
ADD_ENUM(WEAPONTYPE_DETONATOR, "detonator")
ADD_ENUM(WEAPONTYPE_DETONATOR, "bomb")
ADD_ENUM(WEAPONTYPE_SPRAYCAN, "spraycan")
ADD_ENUM(WEAPONTYPE_EXTINGUISHER, "extinguisher")
ADD_ENUM(WEAPONTYPE_EXTINGUISHER, "fire extinguisher")
ADD_ENUM(WEAPONTYPE_CAMERA, "camera")
ADD_ENUM(WEAPONTYPE_NIGHTVISION, "nightvision")
ADD_ENUM(WEAPONTYPE_INFRARED, "infrared")
ADD_ENUM(WEAPONTYPE_PARACHUTE, "parachute")
IMPLEMENT_ENUM_END("weapon-type")

IMPLEMENT_ENUM_BEGIN(eWeaponProperty)
ADD_ENUM(WEAPON_WEAPON_RANGE, "weapon_range")
ADD_ENUM(WEAPON_TARGET_RANGE, "target_range")
ADD_ENUM(WEAPON_ACCURACY, "accuracy")
ADD_ENUM(WEAPON_DAMAGE, "damage")
ADD_ENUM(WEAPON_LIFE_SPAN, "life_span")                  // Disabled for release
ADD_ENUM(WEAPON_FIRING_SPEED, "firing_speed")            // Disabled for release
ADD_ENUM(WEAPON_SPREAD, "spread")                        // Disabled for release
ADD_ENUM(WEAPON_MAX_CLIP_AMMO, "maximum_clip_ammo")
ADD_ENUM(WEAPON_MOVE_SPEED, "move_speed")
ADD_ENUM(WEAPON_FLAGS, "flags")
ADD_ENUM(WEAPON_ANIM_GROUP, "anim_group")            // Disabled for release

ADD_ENUM(WEAPON_ANIM_LOOP_START, "anim_loop_start")
ADD_ENUM(WEAPON_ANIM_LOOP_STOP, "anim_loop_stop")
ADD_ENUM(WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME, "anim_loop_bullet_fire")

ADD_ENUM(WEAPON_ANIM2_LOOP_START, "anim2_loop_start")
ADD_ENUM(WEAPON_ANIM2_LOOP_STOP, "anim2_loop_stop")
ADD_ENUM(WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME, "anim2_loop_bullet_fire")

ADD_ENUM(WEAPON_ANIM_BREAKOUT_TIME, "anim_breakout_time")

// Set is Disabled >= FIRETYPE - Move above WEAPON_TYPE_SET_DISABLED to re-enable and move it above here to make it less confusing.
ADD_ENUM(WEAPON_FIRETYPE, "fire_type")

ADD_ENUM(WEAPON_MODEL, "model")
ADD_ENUM(WEAPON_MODEL2, "model2")

ADD_ENUM(WEAPON_SLOT, "weapon_slot")

ADD_ENUM(WEAPON_FIRE_OFFSET, "fire_offset")

ADD_ENUM(WEAPON_SKILL_LEVEL, "skill_level")
ADD_ENUM(WEAPON_REQ_SKILL_LEVEL, "required_skill_level")

ADD_ENUM(WEAPON_RADIUS, "radius")

ADD_ENUM(WEAPON_AIM_OFFSET, "aim_offset")

ADD_ENUM(WEAPON_DEFAULT_COMBO, "default_combo")
ADD_ENUM(WEAPON_COMBOS_AVAILABLE, "combos_available")

ADD_ENUM(WEAPON_FLAG_AIM_NO_AUTO, "flag_aim_no_auto")
ADD_ENUM(WEAPON_FLAG_AIM_ARM, "flag_aim_arm")
ADD_ENUM(WEAPON_FLAG_AIM_1ST_PERSON, "flag_aim_1st_person")
ADD_ENUM(WEAPON_FLAG_AIM_FREE, "flag_aim_free")
ADD_ENUM(WEAPON_FLAG_MOVE_AND_AIM, "flag_move_and_aim")
ADD_ENUM(WEAPON_FLAG_MOVE_AND_SHOOT, "flag_move_and_shoot")
ADD_ENUM(WEAPON_FLAG_TYPE_THROW, "flag_type_throw")
ADD_ENUM(WEAPON_FLAG_TYPE_HEAVY, "flag_type_heavy")
ADD_ENUM(WEAPON_FLAG_TYPE_CONSTANT, "flag_type_constant")
ADD_ENUM(WEAPON_FLAG_TYPE_DUAL, "flag_type_dual")
ADD_ENUM(WEAPON_FLAG_ANIM_RELOAD, "flag_anim_reload")
ADD_ENUM(WEAPON_FLAG_ANIM_CROUCH, "flag_anim_crouch")
ADD_ENUM(WEAPON_FLAG_ANIM_RELOAD_LOOP, "flag_anim_reload_loop")
ADD_ENUM(WEAPON_FLAG_ANIM_RELOAD_LONG, "flag_anim_reload_long")
ADD_ENUM(WEAPON_FLAG_SHOT_SLOWS, "flag_shot_slows")
ADD_ENUM(WEAPON_FLAG_SHOT_RAND_SPEED, "flag_shot_rand_speed")
ADD_ENUM(WEAPON_FLAG_SHOT_ANIM_ABRUPT, "flag_shot_anim_abrupt")
ADD_ENUM(WEAPON_FLAG_SHOT_EXPANDS, "flag_shot_expands")

ADD_ENUM(WEAPON_FIRE_ROTATION, "fire_rotation")
IMPLEMENT_ENUM_END("weapon-property")

IMPLEMENT_ENUM_BEGIN(eWeaponSkill)
ADD_ENUM(WEAPONSKILL_POOR, "poor")
ADD_ENUM(WEAPONSKILL_STD, "std")
ADD_ENUM(WEAPONSKILL_PRO, "pro")
IMPLEMENT_ENUM_END("weapon-skill")

IMPLEMENT_ENUM_BEGIN(ERenderFormat)
ADD_ENUM(RFORMAT_UNKNOWN, "unknown")
ADD_ENUM(RFORMAT_ARGB, "argb")
ADD_ENUM(RFORMAT_XRGB, "xrgb")
ADD_ENUM(RFORMAT_RGB,  "rgb")
ADD_ENUM(RFORMAT_DXT1, "dxt1")
ADD_ENUM(RFORMAT_DXT2, "dxt2")
ADD_ENUM(RFORMAT_DXT3, "dxt3")
ADD_ENUM(RFORMAT_DXT4, "dxt4")
ADD_ENUM(RFORMAT_DXT5, "dxt5")
ADD_ENUM(RFORMAT_INTZ, "intz")
ADD_ENUM(RFORMAT_DF24, "df24")
ADD_ENUM(RFORMAT_DF16, "df16")
ADD_ENUM(RFORMAT_RAWZ, "rawz")
IMPLEMENT_ENUM_END("texture-format")

IMPLEMENT_ENUM_BEGIN(ETextureType)
ADD_ENUM(TTYPE_TEXTURE, "2d")
ADD_ENUM(TTYPE_VOLUMETEXTURE, "3d")
ADD_ENUM(TTYPE_CUBETEXTURE, "cube")
IMPLEMENT_ENUM_END("texture-type")

IMPLEMENT_ENUM_BEGIN(ETextureAddress)
ADD_ENUM(TADDRESS_WRAP, "wrap")
ADD_ENUM(TADDRESS_MIRROR, "mirror")
ADD_ENUM(TADDRESS_CLAMP, "clamp")
ADD_ENUM(TADDRESS_BORDER, "border")
ADD_ENUM(TADDRESS_MIRRORONCE, "mirror-once")
IMPLEMENT_ENUM_END("texture-edge")

IMPLEMENT_ENUM_BEGIN(EPixelsFormatType)
ADD_ENUM(EPixelsFormat::UNKNOWN, "unknown")
ADD_ENUM(EPixelsFormat::PLAIN, "plain")
ADD_ENUM(EPixelsFormat::JPEG, "jpeg")
ADD_ENUM(EPixelsFormat::PNG, "png")
ADD_ENUM(EPixelsFormat::DDS, "dds")
IMPLEMENT_ENUM_END("pixel-format")

IMPLEMENT_ENUM_BEGIN(EBlendModeType)
ADD_ENUM(EBlendMode::BLEND, "blend")
ADD_ENUM(EBlendMode::ADD, "add")
ADD_ENUM(EBlendMode::MODULATE_ADD, "modulate_add")
ADD_ENUM(EBlendMode::OVERWRITE, "overwrite")
IMPLEMENT_ENUM_END("blend-mode")

IMPLEMENT_ENUM_BEGIN(EEntityTypeMask)
ADD_ENUM(TYPE_MASK_NONE, "none")
ADD_ENUM(TYPE_MASK_WORLD, "world")
ADD_ENUM(TYPE_MASK_PED, "ped")
ADD_ENUM(TYPE_MASK_VEHICLE, "vehicle")
ADD_ENUM(TYPE_MASK_OBJECT, "object")
ADD_ENUM(TYPE_MASK_OTHER, "other")
ADD_ENUM(TYPE_MASK_ALL, "all")
IMPLEMENT_ENUM_END("entity-type-mask")

IMPLEMENT_ENUM_BEGIN(eWeaponState)
ADD_ENUM(WEAPONSTATE_READY, "ready")
ADD_ENUM(WEAPONSTATE_FIRING, "firing")
ADD_ENUM(WEAPONSTATE_RELOADING, "reloading")
IMPLEMENT_ENUM_END("weapon-state")

IMPLEMENT_ENUM_BEGIN(eWeaponFlags)
ADD_ENUM(WEAPONFLAGS_DISABLE_MODEL, "disable_model")
ADD_ENUM(WEAPONFLAGS_FLAGS, "flags")
ADD_ENUM(WEAPONFLAGS_INSTANT_RELOAD, "instant_reload")
ADD_ENUM(WEAPONFLAGS_SHOOT_IF_OUT_OF_RANGE, "shoot_if_out_of_range")
ADD_ENUM(WEAPONFLAGS_SHOOT_IF_TARGET_BOCKED, "shoot_if_blocked")
IMPLEMENT_ENUM_END("weapon-flags")

IMPLEMENT_ENUM_BEGIN(eVehicleComponent)
ADD_ENUM(VEHICLE_COMPONENT_SPECIAL_1, "special_1")
ADD_ENUM(VEHICLE_COMPONENT_SPECIAL_2, "special_2")
ADD_ENUM(VEHICLE_COMPONENT_SPECIAL_3, "special_3")
ADD_ENUM(VEHICLE_COMPONENT_SPECIAL_4, "special_4")
ADD_ENUM(VEHICLE_COMPONENT_SPECIAL_5, "special_5")
ADD_ENUM(VEHICLE_COMPONENT_BONET, "bonnet")
ADD_ENUM(VEHICLE_COMPONENT_BOOT, "boot")
ADD_ENUM(VEHICLE_COMPONENT_BUMPER_1, "bumper_1")
ADD_ENUM(VEHICLE_COMPONENT_BUMPER_2, "bumper_2")
ADD_ENUM(VEHICLE_COMPONENT_CHASIS, "chassis")
ADD_ENUM(VEHICLE_COMPONENT_DOOR_1, "door_1")
ADD_ENUM(VEHICLE_COMPONENT_DOOR_2, "door_2")
ADD_ENUM(VEHICLE_COMPONENT_DOOR_3, "door_3")
ADD_ENUM(VEHICLE_COMPONENT_DOOR_4, "door_4")
ADD_ENUM(VEHICLE_COMPONENT_EXHAUST, "exhaust")
ADD_ENUM(VEHICLE_COMPONENT_WINGLF, "wing_lf")
ADD_ENUM(VEHICLE_COMPONENT_WINGRF, "wing_rf")
ADD_ENUM(VEHICLE_COMPONENT_EXTRA_1, "extra_1")
ADD_ENUM(VEHICLE_COMPONENT_EXTRA_2, "extra_2")
IMPLEMENT_ENUM_END("vehicle-component")

IMPLEMENT_ENUM_BEGIN(eObjectProperty)
ADD_ENUM(OBJECT_PROPERTY_ALL, "all")
ADD_ENUM(OBJECT_PROPERTY_MASS, "mass")
ADD_ENUM(OBJECT_PROPERTY_TURNMASS, "turn_mass")
ADD_ENUM(OBJECT_PROPERTY_AIRRESISTANCE, "air_resistance")
ADD_ENUM(OBJECT_PROPERTY_ELASTICITY, "elasticity")
ADD_ENUM(OBJECT_PROPERTY_CENTEROFMASS, "center_of_mass")
ADD_ENUM(OBJECT_PROPERTY_BUOYANCY, "buoyancy")
IMPLEMENT_ENUM_END("object-property")

IMPLEMENT_ENUM_BEGIN(eObjectGroup::Modifiable)
ADD_ENUM(eObjectGroup::Modifiable::MASS, "mass")
ADD_ENUM(eObjectGroup::Modifiable::TURNMASS, "turn_mass")
ADD_ENUM(eObjectGroup::Modifiable::AIRRESISTANCE, "air_resistance")
ADD_ENUM(eObjectGroup::Modifiable::ELASTICITY, "elasticity")
ADD_ENUM(eObjectGroup::Modifiable::BUOYANCY, "buoyancy")
ADD_ENUM(eObjectGroup::Modifiable::UPROOTLIMIT, "uproot_limit")
ADD_ENUM(eObjectGroup::Modifiable::COLDAMAGEMULTIPLIER, "col_damage_multiplier")
ADD_ENUM(eObjectGroup::Modifiable::COLDAMAGEEFFECT, "col_damage_effect")
ADD_ENUM(eObjectGroup::Modifiable::SPECIALCOLRESPONSE, "special_col_response")
ADD_ENUM(eObjectGroup::Modifiable::CAMERAAVOID, "avoid_camera")
ADD_ENUM(eObjectGroup::Modifiable::EXPLOSION, "cause_explosion")
ADD_ENUM(eObjectGroup::Modifiable::FXTYPE, "fx_type")
ADD_ENUM(eObjectGroup::Modifiable::FXOFFSET, "fx_offset")
ADD_ENUM(eObjectGroup::Modifiable::FXSYSTEM, "fx_system")
ADD_ENUM(eObjectGroup::Modifiable::SMASHMULTIPLIER, "smash_multiplier")
ADD_ENUM(eObjectGroup::Modifiable::BREAKVELOCITY, "break_velocity")
ADD_ENUM(eObjectGroup::Modifiable::BREAKVELOCITYRAND, "break_velocity_randomness")
ADD_ENUM(eObjectGroup::Modifiable::BREAKMODE, "break_mode")
ADD_ENUM(eObjectGroup::Modifiable::SPARKSONIMPACT, "sparks_on_impact")
IMPLEMENT_ENUM_END("objectgroup-modifiable")

IMPLEMENT_ENUM_BEGIN(eObjectGroup::DamageEffect)
ADD_ENUM(eObjectGroup::DamageEffect::NO_EFFECT, "none")
ADD_ENUM(eObjectGroup::DamageEffect::CHANGE_MODEL, "change_model")
ADD_ENUM(eObjectGroup::DamageEffect::SMASH_COMPLETELY, "smash")
ADD_ENUM(eObjectGroup::DamageEffect::CHANGE_THEN_SMASH, "change_smash")
ADD_ENUM(eObjectGroup::DamageEffect::BREAKABLE, "breakable")
ADD_ENUM(eObjectGroup::DamageEffect::BREAKABLE_REMOVED, "breakable_remove")
IMPLEMENT_ENUM_END("objectgroup-damageeffect")

IMPLEMENT_ENUM_BEGIN(eObjectGroup::CollisionResponse)
ADD_ENUM(eObjectGroup::CollisionResponse::NO_RESPONSE, "none")
ADD_ENUM(eObjectGroup::CollisionResponse::LAMPPOST, "lamppost")
ADD_ENUM(eObjectGroup::CollisionResponse::SMALLBOX, "small_box")
ADD_ENUM(eObjectGroup::CollisionResponse::BIGBOX, "big_box")
ADD_ENUM(eObjectGroup::CollisionResponse::FENCEPART, "fence_part")
ADD_ENUM(eObjectGroup::CollisionResponse::GRENADE, "grenade")
ADD_ENUM(eObjectGroup::CollisionResponse::SWINGDOOR, "swingdoor")
ADD_ENUM(eObjectGroup::CollisionResponse::LOCKDOOR, "lockdoor")
ADD_ENUM(eObjectGroup::CollisionResponse::HANGING, "hanging")
ADD_ENUM(eObjectGroup::CollisionResponse::POOLBALL, "poolball")
IMPLEMENT_ENUM_END("objectgroup-collisionresponse")

IMPLEMENT_ENUM_BEGIN(eObjectGroup::FxType)
ADD_ENUM(eObjectGroup::FxType::NO_FX, "none")
ADD_ENUM(eObjectGroup::FxType::PLAY_ON_HIT, "play_on_hit")
ADD_ENUM(eObjectGroup::FxType::PLAY_ON_DESTROYED, "play_on_destroyed")
ADD_ENUM(eObjectGroup::FxType::PLAY_ON_HIT_DESTROYED, "play_on_hitdestroyed")
IMPLEMENT_ENUM_END("objectgroup-fxtype")

IMPLEMENT_ENUM_BEGIN(eObjectGroup::BreakMode)
ADD_ENUM(eObjectGroup::BreakMode::NOT_BY_GUN, "not_by_gun")
ADD_ENUM(eObjectGroup::BreakMode::BY_GUN, "by_gun")
ADD_ENUM(eObjectGroup::BreakMode::SMASHABLE, "smashable")
IMPLEMENT_ENUM_END("objectgroup-breakmode")

IMPLEMENT_ENUM_BEGIN(eFontType)
ADD_ENUM(FONT_DEFAULT, "default")
ADD_ENUM(FONT_DEFAULT_BOLD, "default-bold")
ADD_ENUM(FONT_CLEAR, "clear")
ADD_ENUM(FONT_ARIAL, "arial")
ADD_ENUM(FONT_SANS, "sans")
ADD_ENUM(FONT_PRICEDOWN, "pricedown")
ADD_ENUM(FONT_BANKGOTHIC, "bankgothic")
ADD_ENUM(FONT_DIPLOMA, "diploma")
ADD_ENUM(FONT_BECKETT, "beckett")
ADD_ENUM(FONT_UNIFONT, "unifont")
IMPLEMENT_ENUM_END_DEFAULTS("font-type", FONT_DEFAULT, "")

IMPLEMENT_ENUM_BEGIN(eFontQuality)
ADD_ENUM(FONT_QUALITY_DEFAULT, "default")
ADD_ENUM(FONT_QUALITY_DRAFT, "draft")
ADD_ENUM(FONT_QUALITY_PROOF, "proof")
#if( WINVER >= 0x0400 )
ADD_ENUM(FONT_QUALITY_NONANTIALIASED, "nonantialiased")
ADD_ENUM(FONT_QUALITY_ANTIALIASED, "antialiased")
#endif
#if( _WIN32_WINNT >= _WIN32_WINNT_WINXP )
ADD_ENUM(FONT_QUALITY_CLEARTYPE, "cleartype")
ADD_ENUM(FONT_QUALITY_CLEARTYPE_NATURAL, "cleartype_natural")
#endif
IMPLEMENT_ENUM_END_DEFAULTS("font-quality", FONT_QUALITY_DEFAULT, "")

IMPLEMENT_ENUM_BEGIN(eAudioLookupIndex)
ADD_ENUM(AUDIO_LOOKUP_FEET, "feet")
ADD_ENUM(AUDIO_LOOKUP_GENRL, "genrl")
ADD_ENUM(AUDIO_LOOKUP_PAIN_A, "pain_a")
ADD_ENUM(AUDIO_LOOKUP_SCRIPT, "script")
ADD_ENUM(AUDIO_LOOKUP_SPC_EA, "spc_ea")
ADD_ENUM(AUDIO_LOOKUP_SPC_FA, "spc_fa")
ADD_ENUM(AUDIO_LOOKUP_SPC_GA, "spc_ga")
ADD_ENUM(AUDIO_LOOKUP_SPC_NA, "spc_na")
ADD_ENUM(AUDIO_LOOKUP_SPC_PA, "spc_pa")
ADD_ENUM(AUDIO_LOOKUP_RADIO, "radio")
IMPLEMENT_ENUM_END("audio-lookup-index")

IMPLEMENT_ENUM_BEGIN(eAspectRatio)
ADD_ENUM(ASPECT_RATIO_AUTO, "auto")
ADD_ENUM(ASPECT_RATIO_4_3, "4:3")
ADD_ENUM(ASPECT_RATIO_16_10, "16:10")
ADD_ENUM(ASPECT_RATIO_16_9, "16:9")
IMPLEMENT_ENUM_END("aspectratio")

IMPLEMENT_ENUM_BEGIN(eRadioStreamIndex)
ADD_ENUM(RADIO_STREAM_ADVERTS, "Adverts")
ADD_ENUM(RADIO_STREAM_AMBIENCE, "Ambience")
ADD_ENUM(RADIO_STREAM_POLICE, "Police")
ADD_ENUM(RADIO_STREAM_PLAYBACK, "Playback FM")
ADD_ENUM(RADIO_STREAM_KROSE, "K-Rose")
ADD_ENUM(RADIO_STREAM_KDST, "K-DST")
ADD_ENUM(RADIO_STREAM_CUTSCENE, "Cutscene")
ADD_ENUM(RADIO_STREAM_BEATS, "Beats")
ADD_ENUM(RADIO_STREAM_BOUNCE, "Bounce FM")
ADD_ENUM(RADIO_STREAM_SFUR, "SF-UR")
ADD_ENUM(RADIO_STREAM_RADIO_LOS_SANTOS, "Radio Los Santos")
ADD_ENUM(RADIO_STREAM_RADIO_X, "Radio X")
ADD_ENUM(RADIO_STREAM_CSR, "CSR 103.9")
ADD_ENUM(RADIO_STREAM_KJAH, "K-Jah West")
ADD_ENUM(RADIO_STREAM_MASTER_SOUNDS, "Master Sounds 98.3")
ADD_ENUM(RADIO_STREAM_WCTR, "WCTR")
IMPLEMENT_ENUM_END("radio-stream-index")

IMPLEMENT_ENUM_BEGIN(EComponentBase::EComponentBaseType)
ADD_ENUM(EComponentBaseType::WORLD, "world")
ADD_ENUM(EComponentBaseType::ROOT, "root")
ADD_ENUM(EComponentBaseType::PARENT, "parent")
IMPLEMENT_ENUM_END("component-base")

IMPLEMENT_ENUM_BEGIN(eWebBrowserMouseButton)
ADD_ENUM(BROWSER_MOUSEBUTTON_LEFT, "left")
ADD_ENUM(BROWSER_MOUSEBUTTON_MIDDLE, "middle")
ADD_ENUM(BROWSER_MOUSEBUTTON_RIGHT, "right")
IMPLEMENT_ENUM_END("webbrowser-mouse-button")

IMPLEMENT_ENUM_BEGIN(eFieldOfViewMode)
ADD_ENUM(FOV_MODE_PLAYER, "player")
ADD_ENUM(FOV_MODE_VEHICLE, "vehicle")
ADD_ENUM(FOV_MODE_VEHICLE_MAX, "vehicle_max")
ADD_ENUM(FOV_MODE_AIMING, "aiming")
IMPLEMENT_ENUM_END("fieldofview-mode")

IMPLEMENT_ENUM_BEGIN(eTrayIconType)
ADD_ENUM(ICON_TYPE_DEFAULT, "default")
ADD_ENUM(ICON_TYPE_INFO, "info")
ADD_ENUM(ICON_TYPE_WARNING, "warning")
ADD_ENUM(ICON_TYPE_ERROR, "error")
IMPLEMENT_ENUM_END("tray-icon-type")

IMPLEMENT_ENUM_BEGIN(eJSONPrettyType)
ADD_ENUM(JSONPRETTY_SPACES, "spaces")
ADD_ENUM(JSONPRETTY_NONE, "none")
ADD_ENUM(JSONPRETTY_TABS, "tabs")
IMPLEMENT_ENUM_END("json-pretty-type")

IMPLEMENT_ENUM_BEGIN(eCursorType)
ADD_ENUM(CURSORTYPE_NONE, "none")                            // cursor has no image
ADD_ENUM(CURSORTYPE_DEFAULT, "arrow")                        // default cursor
ADD_ENUM(CURSORTYPE_SIZING_NS, "sizing_ns")                  // N-S (up-down) sizing cursor
ADD_ENUM(CURSORTYPE_SIZING_EW, "sizing_ew")                  // E-W (left-right) sizing cursor
ADD_ENUM(CURSORTYPE_SIZING_NWSE, "sizing_nwse")              // NW-SE diagonal sizing cursor
ADD_ENUM(CURSORTYPE_SIZING_NESW, "sizing_nesw")              // NE-SW diagonal sizing cursor
ADD_ENUM(CURSORTYPE_SIZING_ESWE, "sizing_eswe")              // ES-WE horizontal sizing cursor
ADD_ENUM(CURSORTYPE_MOVE, "move")                            // move cursor
ADD_ENUM(CURSORTYPE_DRAG, "container_drag")                  // drag container cursor (note: not in use)
ADD_ENUM(CURSORTYPE_SEG_MOVING, "segment_moving")            // segment moving cursor (note: not in use)
ADD_ENUM(CURSORTYPE_SEG_SIZING, "segment_sizing")            // segment sizing cursor (note: not in use)
IMPLEMENT_ENUM_END("cursor-type")

IMPLEMENT_ENUM_BEGIN(eWheelPosition)
ADD_ENUM(FRONT_LEFT_WHEEL, "front_left")
ADD_ENUM(REAR_LEFT_WHEEL, "rear_left")
ADD_ENUM(FRONT_RIGHT_WHEEL, "front_right")
ADD_ENUM(REAR_RIGHT_WHEEL, "rear_right")
IMPLEMENT_ENUM_END("wheel-position")

IMPLEMENT_ENUM_BEGIN(D3DPRIMITIVETYPE)
ADD_ENUM(D3DPT_POINTLIST, "pointlist")
ADD_ENUM(D3DPT_LINELIST, "linelist")
ADD_ENUM(D3DPT_LINESTRIP, "linestrip")
ADD_ENUM(D3DPT_TRIANGLELIST, "trianglelist")
ADD_ENUM(D3DPT_TRIANGLESTRIP, "trianglestrip")
ADD_ENUM(D3DPT_TRIANGLEFAN, "trianglefan")
IMPLEMENT_ENUM_END("primitive-type")

IMPLEMENT_ENUM_BEGIN(eVehicleDummies)
ADD_ENUM(LIGHT_FRONT_MAIN, "light_front_main")
ADD_ENUM(LIGHT_REAR_MAIN, "light_rear_main")
ADD_ENUM(LIGHT_FRONT_SECONDARY, "light_front_second")
ADD_ENUM(LIGHT_REAR_SECONDARY, "light_rear_second")
ADD_ENUM(SEAT_FRONT, "seat_front")
ADD_ENUM(SEAT_REAR, "seat_rear")
ADD_ENUM(EXHAUST, "exhaust")
ADD_ENUM(ENGINE, "engine")
ADD_ENUM(GAS_CAP, "gas_cap")
ADD_ENUM(TRAILER_ATTACH, "trailer_attach")
ADD_ENUM(HAND_REST, "hand_rest")
ADD_ENUM(EXHAUST_SECONDARY, "exhaust_second")
ADD_ENUM(WING_AIRTRAIL, "wing_airtrail")
ADD_ENUM(VEH_GUN, "veh_gun")
IMPLEMENT_ENUM_END("vehicle-dummy")

IMPLEMENT_ENUM_CLASS_BEGIN(eGrainMultiplierType)
ADD_ENUM(eGrainMultiplierType::MASTER, "master")
ADD_ENUM(eGrainMultiplierType::INFRARED, "infrared")
ADD_ENUM(eGrainMultiplierType::NIGHT, "night")
ADD_ENUM(eGrainMultiplierType::RAIN, "rain")
ADD_ENUM(eGrainMultiplierType::OVERLAY, "overlay")
ADD_ENUM(eGrainMultiplierType::ALL, "all")
IMPLEMENT_ENUM_CLASS_END("grain-multiplier-type")

IMPLEMENT_ENUM_CLASS_BEGIN(eResizableVehicleWheelGroup)
ADD_ENUM(eResizableVehicleWheelGroup::FRONT_AXLE, "front_axle")
ADD_ENUM(eResizableVehicleWheelGroup::REAR_AXLE, "rear_axle")
ADD_ENUM(eResizableVehicleWheelGroup::ALL_WHEELS, "all_wheels")
IMPLEMENT_ENUM_CLASS_END("resizable-vehicle-wheel-group")

IMPLEMENT_ENUM_BEGIN(eSurfaceProperties)
ADD_ENUM(SURFACE_PROPERTY_AUDIO, "audio")
ADD_ENUM(SURFACE_PROPERTY_STEPWATERSPLASH, "stepwatersplash")
ADD_ENUM(SURFACE_PROPERTY_FOOTEFFECT, "footeffect")
ADD_ENUM(SURFACE_PROPERTY_STEPDUST, "stepdust")
ADD_ENUM(SURFACE_PROPERTY_CLIMBING, "canclimb")
ADD_ENUM(SURFACE_PROPERTY_BULLETEFFECT, "bulleteffect")
ADD_ENUM(SURFACE_PROPERTY_SHOOTTHROUGH, "shootthrough")
ADD_ENUM(SURFACE_PROPERTY_SEETHROUGH, "seethrough")
ADD_ENUM(SURFACE_PROPERTY_SKIDMARKTYPE, "skidmarktype")
ADD_ENUM(SURFACE_PROPERTY_TYREGRIP, "tyregrip")
ADD_ENUM(SURFACE_PROPERTY_WETGRIP, "wetgrip")
ADD_ENUM(SURFACE_PROPERTY_ADHESIONGROUP, "adhesiongroup")
ADD_ENUM(SURFACE_PROPERTY_WHEELEFFECT, "wheeleffect")
ADD_ENUM(SURFACE_PROPERTY_FRACTIONEFFECT, "fractioneffect")
ADD_ENUM(SURFACE_PROPERTY_STAIRS, "stairs")
ADD_ENUM(SURFACE_PROPERTY_ROUGHNESS, "roughness")
ADD_ENUM(SURFACE_PROPERTY_STEEPSLOPE, "steepslope")
ADD_ENUM(SURFACE_PROPERTY_GLASS, "glass")
ADD_ENUM(SURFACE_PROPERTY_PAVEMENT, "pavement")
ADD_ENUM(SURFACE_PROPERTY_SOFTLANDING, "softlanding")

// crash when enabling on surfaces without setting plants and trees
// table at offset 0xC38070 contain information about which are read from plants.dat
// i don't know did this will work on objects created by createObject function
// ADD_ENUM(SURFACE_PROPERTY_CREATEOBJECTS, "createobjects")
// ADD_ENUM(SURFACE_PROPERTY_CREATEPLANTS, "createplants")
IMPLEMENT_ENUM_END("surface-property-type")

IMPLEMENT_ENUM_BEGIN(eSurfaceAudio)
ADD_ENUM(SURFACE_AUDIO_CONCRETE, "concrete")
ADD_ENUM(SURFACE_AUDIO_GRASS, "grass")
ADD_ENUM(SURFACE_AUDIO_SAND, "sand")
ADD_ENUM(SURFACE_AUDIO_GRAVEL, "gravel")
ADD_ENUM(SURFACE_AUDIO_WOOD, "wood")
ADD_ENUM(SURFACE_AUDIO_WATER, "water")
ADD_ENUM(SURFACE_AUDIO_METAL, "metal")
// ADD_ENUM(SURFACE_AUDIO_LONGGRASS, "longgrass") // same sound as grass
IMPLEMENT_ENUM_END("surface-audio-type")

IMPLEMENT_ENUM_BEGIN(eSurfaceWheelEffect)
ADD_ENUM(SURFACE_WHEEL_EFFECT_GRASS, "grass")
ADD_ENUM(SURFACE_WHEEL_EFFECT_GRAVEL, "gravel")
ADD_ENUM(SURFACE_WHEEL_EFFECT_MUD, "mud")
ADD_ENUM(SURFACE_WHEEL_EFFECT_DUST, "dust")
ADD_ENUM(SURFACE_WHEEL_EFFECT_SAND, "sand")
ADD_ENUM(SURFACE_WHEEL_EFFECT_DISABLED, "disabled")
IMPLEMENT_ENUM_END("surface-wheel-effect")

IMPLEMENT_ENUM_BEGIN(eSurfaceBulletEffect)
ADD_ENUM(SURFACE_BULLET_EFFECT_METAL, "metal")
ADD_ENUM(SURFACE_BULLET_EFFECT_CONCRETE, "concrete")
ADD_ENUM(SURFACE_BULLET_EFFECT_SAND, "sand")
ADD_ENUM(SURFACE_BULLET_EFFECT_WOOD, "wood")
ADD_ENUM(SURFACE_BULLET_EFFECT_DISABLED, "disabled")
IMPLEMENT_ENUM_END("surface-bullet-effect")

IMPLEMENT_ENUM_BEGIN(eSurfaceSkidMarkType)
ADD_ENUM(SURFACE_SKID_MARK_ASPHALT, "asphalt")
ADD_ENUM(SURFACE_SKID_MARK_DIRT, "dirt")
ADD_ENUM(SURFACE_SKID_MARK_DUST, "dust")
ADD_ENUM(SURFACE_SKID_MARK_DISABLED, "disabled")
IMPLEMENT_ENUM_END("surface-skidmark-type")

IMPLEMENT_ENUM_BEGIN(eSurfaceAdhesionGroup)
ADD_ENUM(SURFACE_ADHESION_GROUP_RUBBER, "rubber")
ADD_ENUM(SURFACE_ADHESION_GROUP_HARD, "hard")
ADD_ENUM(SURFACE_ADHESION_GROUP_ROAD, "road")
ADD_ENUM(SURFACE_ADHESION_GROUP_LOOSE, "loose")
ADD_ENUM(SURFACE_ADHESION_GROUP_SAND, "sand")
ADD_ENUM(SURFACE_ADHESION_GROUP_WET, "wet")
IMPLEMENT_ENUM_END("surface-adhesion-group")

IMPLEMENT_ENUM_CLASS_BEGIN(eClientModelType)
ADD_ENUM(eClientModelType::PED, "ped")
ADD_ENUM(eClientModelType::OBJECT, "object")
ADD_ENUM(eClientModelType::OBJECT_DAMAGEABLE, "object-damageable")
ADD_ENUM(eClientModelType::VEHICLE, "vehicle")
ADD_ENUM(eClientModelType::TIMED_OBJECT, "timed-object")
ADD_ENUM(eClientModelType::CLUMP, "clump")
IMPLEMENT_ENUM_CLASS_END("client-model-type")

// Sound effects
IMPLEMENT_ENUM_BEGIN(eSoundEffectType)
ADD_ENUM(BASS_FX_DX8_CHORUS, "chorus")
ADD_ENUM(BASS_FX_DX8_COMPRESSOR, "compressor")
ADD_ENUM(BASS_FX_DX8_DISTORTION, "distortion")
ADD_ENUM(BASS_FX_DX8_ECHO, "echo")
ADD_ENUM(BASS_FX_DX8_FLANGER, "flanger")
ADD_ENUM(BASS_FX_DX8_GARGLE, "gargle")
ADD_ENUM(BASS_FX_DX8_I3DL2REVERB, "i3dl2reverb")
ADD_ENUM(BASS_FX_DX8_PARAMEQ, "parameq")
ADD_ENUM(BASS_FX_DX8_REVERB, "reverb")
IMPLEMENT_ENUM_END("soundeffect-type")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::Chorus)
ADD_ENUM(eSoundEffectParams::Chorus::WET_DRY_MIX, "wetDryMix")
ADD_ENUM(eSoundEffectParams::Chorus::DEPTH, "depth")
ADD_ENUM(eSoundEffectParams::Chorus::FEEDBACK, "feedback")
ADD_ENUM(eSoundEffectParams::Chorus::FREQUENCY, "frequency")
ADD_ENUM(eSoundEffectParams::Chorus::WAVEFORM, "waveform")
ADD_ENUM(eSoundEffectParams::Chorus::DELAY, "delay")
ADD_ENUM(eSoundEffectParams::Chorus::PHASE, "phase")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-chorus")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::Compressor)
ADD_ENUM(eSoundEffectParams::Compressor::GAIN, "gain")
ADD_ENUM(eSoundEffectParams::Compressor::ATTACK, "attack")
ADD_ENUM(eSoundEffectParams::Compressor::RELEASE, "release")
ADD_ENUM(eSoundEffectParams::Compressor::THRESHOLD, "threshold")
ADD_ENUM(eSoundEffectParams::Compressor::RATIO, "ratio")
ADD_ENUM(eSoundEffectParams::Compressor::PREDELAY, "predelay")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-compressor")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::Distortion)
ADD_ENUM(eSoundEffectParams::Distortion::GAIN, "gain")
ADD_ENUM(eSoundEffectParams::Distortion::EDGE, "edge")
ADD_ENUM(eSoundEffectParams::Distortion::POST_EQ_CENTER_FREQUENCY, "postEQCenterFrequency")
ADD_ENUM(eSoundEffectParams::Distortion::POST_EQ_BANDWIDTH, "postEQBandwidth")
ADD_ENUM(eSoundEffectParams::Distortion::PRE_LOWPASS_CUTOFF, "preLowpassCutoff")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-distortion")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::Echo)
ADD_ENUM(eSoundEffectParams::Echo::WET_DRY_MIX, "wetDryMix")
ADD_ENUM(eSoundEffectParams::Echo::FEEDBACK, "feedback")
ADD_ENUM(eSoundEffectParams::Echo::LEFT_DELAY, "leftDelay")
ADD_ENUM(eSoundEffectParams::Echo::RIGHT_DELAY, "rightDelay")
ADD_ENUM(eSoundEffectParams::Echo::PAN_DELAY, "panDelay")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-echo")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::Flanger)
ADD_ENUM(eSoundEffectParams::Flanger::WET_DRY_MIX, "wetDryMix")
ADD_ENUM(eSoundEffectParams::Flanger::DEPTH, "depth")
ADD_ENUM(eSoundEffectParams::Flanger::FEEDBACK, "feedback")
ADD_ENUM(eSoundEffectParams::Flanger::FREQUENCY, "frequency")
ADD_ENUM(eSoundEffectParams::Flanger::WAVEFORM, "waveform")
ADD_ENUM(eSoundEffectParams::Flanger::DELAY, "delay")
ADD_ENUM(eSoundEffectParams::Flanger::PHASE, "phase")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-flanger")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::Gargle)
ADD_ENUM(eSoundEffectParams::Gargle::RATE_HZ, "rateHz")
ADD_ENUM(eSoundEffectParams::Gargle::WAVE_SHAPE, "waveShape")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-gargle")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::I3DL2Reverb)
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::ROOM, "room")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::ROOM_HF, "roomHF")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::ROOM_ROLLOFF_FACTOR, "roomRolloffFactor")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::DECAY_TIME, "decayTime")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::DECAY_HF_RATIO, "decayHFRatio")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::REFLECTIONS, "reflections")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::REFLECTIONS_DELAY, "reflectionsDelay")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::REVERB, "reverb")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::REVERB_DELAY, "reverbDelay")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::DIFFUSION, "diffusion")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::DENSITY, "density")
ADD_ENUM(eSoundEffectParams::I3DL2Reverb::HF_REFERENCE, "HFReference")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-i3dl2reverb")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::ParamEq)
ADD_ENUM(eSoundEffectParams::ParamEq::CENTER, "center")
ADD_ENUM(eSoundEffectParams::ParamEq::BANDWIDTH, "bandwidth")
ADD_ENUM(eSoundEffectParams::ParamEq::GAIN, "gain")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-parameq")

IMPLEMENT_ENUM_CLASS_BEGIN(eSoundEffectParams::Reverb)
ADD_ENUM(eSoundEffectParams::Reverb::IN_GAIN, "inGain")
ADD_ENUM(eSoundEffectParams::Reverb::REVERB_MIX, "reverbMix")
ADD_ENUM(eSoundEffectParams::Reverb::REVERB_TIME, "reverbTime")
ADD_ENUM(eSoundEffectParams::Reverb::HIGH_FREQ_RT_RATIO, "highFreqRTRatio")
IMPLEMENT_ENUM_CLASS_END("soundeffect-params-reverb")

IMPLEMENT_ENUM_CLASS_BEGIN(eModelIdeFlag)
ADD_ENUM(eModelIdeFlag::IS_ROAD, "is_road")
ADD_ENUM(eModelIdeFlag::DRAW_LAST, "draw_last")
ADD_ENUM(eModelIdeFlag::ADDITIVE, "additive")
ADD_ENUM(eModelIdeFlag::IGNORE_LIGHTING, "ignore_lighting")
ADD_ENUM(eModelIdeFlag::NO_ZBUFFER_WRITE, "no_zbuffer_write")
ADD_ENUM(eModelIdeFlag::DONT_RECEIVE_SHADOWS, "dont_receive_shadows")
ADD_ENUM(eModelIdeFlag::IS_GLASS_TYPE_1, "is_glass_type_1")
ADD_ENUM(eModelIdeFlag::IS_GLASS_TYPE_2, "is_glass_type_2")
ADD_ENUM(eModelIdeFlag::IS_GARAGE_DOOR, "is_garage_door")
ADD_ENUM(eModelIdeFlag::IS_DAMAGABLE, "is_damagable")
ADD_ENUM(eModelIdeFlag::IS_TREE, "is_tree")
ADD_ENUM(eModelIdeFlag::IS_PALM, "is_palm")
ADD_ENUM(eModelIdeFlag::DOES_NOT_COLLIDE_WITH_FLYER, "does_not_collide_with_flyer")
ADD_ENUM(eModelIdeFlag::IS_TAG, "is_tag")
ADD_ENUM(eModelIdeFlag::DISABLE_BACKFACE_CULLING, "disable_backface_culling")
ADD_ENUM(eModelIdeFlag::IS_BREAKABLE_STATUE, "is_breakable_statue")
ADD_ENUM(eModelIdeFlag::IS_CRANE, "is_crane")
IMPLEMENT_ENUM_CLASS_END("model-ide-flag")

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dformat
IMPLEMENT_ENUM_CLASS_BEGIN(_D3DFORMAT)
ADD_ENUM(_D3DFORMAT::D3DFMT_R8G8B8, "r8g8b8")
ADD_ENUM(_D3DFORMAT::D3DFMT_A8R8G8B8, "a8r8g8b8")
ADD_ENUM(_D3DFORMAT::D3DFMT_X8R8G8B8, "x8r8g8b8")
ADD_ENUM(_D3DFORMAT::D3DFMT_R5G6B5, "r5g6b5")
ADD_ENUM(_D3DFORMAT::D3DFMT_X1R5G5B5, "x1r5g5b5")
ADD_ENUM(_D3DFORMAT::D3DFMT_A1R5G5B5, "a1r5g5b5")
ADD_ENUM(_D3DFORMAT::D3DFMT_A4R4G4B4, "a4r4g4b4")
ADD_ENUM(_D3DFORMAT::D3DFMT_R3G3B2, "r3g3b2")
ADD_ENUM(_D3DFORMAT::D3DFMT_A8, "a8")
ADD_ENUM(_D3DFORMAT::D3DFMT_A8R3G3B2, "a8r3g3b2")
ADD_ENUM(_D3DFORMAT::D3DFMT_X4R4G4B4, "x4r4g4b4")
ADD_ENUM(_D3DFORMAT::D3DFMT_A2B10G10R10, "a2b10g10r10")
ADD_ENUM(_D3DFORMAT::D3DFMT_A8B8G8R8, "a8b8g8r8")
ADD_ENUM(_D3DFORMAT::D3DFMT_X8B8G8R8, "x8b8g8r8")
ADD_ENUM(_D3DFORMAT::D3DFMT_G16R16, "g16r16")
ADD_ENUM(_D3DFORMAT::D3DFMT_A2R10G10B10, "a2r10g10b10")
ADD_ENUM(_D3DFORMAT::D3DFMT_A16B16G16R16, "a16b16g16r16")
ADD_ENUM(_D3DFORMAT::D3DFMT_A8P8, "a8p8")
ADD_ENUM(_D3DFORMAT::D3DFMT_P8, "p8")
ADD_ENUM(_D3DFORMAT::D3DFMT_L8, "l8")
ADD_ENUM(_D3DFORMAT::D3DFMT_L16, "l16")
ADD_ENUM(_D3DFORMAT::D3DFMT_A8L8, "a8l8")
ADD_ENUM(_D3DFORMAT::D3DFMT_A4L4, "a4l4")
ADD_ENUM(_D3DFORMAT::D3DFMT_R16F, "r16f")
ADD_ENUM(_D3DFORMAT::D3DFMT_G16R16F, "g16r16f")
ADD_ENUM(_D3DFORMAT::D3DFMT_A16B16G16R16F, "a16b16g16r16f")
ADD_ENUM(_D3DFORMAT::D3DFMT_R32F, "r32f")
ADD_ENUM(_D3DFORMAT::D3DFMT_G32R32F, "g32r32f")
ADD_ENUM(_D3DFORMAT::D3DFMT_A32B32G32R32F, "a32b32g32r32f")
IMPLEMENT_ENUM_CLASS_END("surface-format")

IMPLEMENT_ENUM_CLASS_BEGIN(eRenderStage)
ADD_ENUM(eRenderStage::PRE_FX, "prefx")
ADD_ENUM(eRenderStage::POST_FX, "postfx")
ADD_ENUM(eRenderStage::POST_GUI, "postgui")
IMPLEMENT_ENUM_CLASS_END("render-stage")

IMPLEMENT_ENUM_BEGIN(ePools)
ADD_ENUM(ePools::BUILDING_POOL, "building")
ADD_ENUM(ePools::PED_POOL, "ped")
ADD_ENUM(ePools::OBJECT_POOL, "object")
ADD_ENUM(ePools::DUMMY_POOL, "dummy")
ADD_ENUM(ePools::VEHICLE_POOL, "vehicle")
ADD_ENUM(ePools::COL_MODEL_POOL, "col-model")
ADD_ENUM(ePools::TASK_POOL, "task")
ADD_ENUM(ePools::EVENT_POOL, "event")
ADD_ENUM(ePools::TASK_ALLOCATOR_POOL, "task-allocator")
ADD_ENUM(ePools::PED_INTELLIGENCE_POOL, "ped-intelligence")
ADD_ENUM(ePools::PED_ATTRACTOR_POOL, "ped-attractor")
ADD_ENUM(ePools::ENTRY_INFO_NODE_POOL, "entry-info-node")
ADD_ENUM(ePools::NODE_ROUTE_POOL, "node-route")
ADD_ENUM(ePools::PATROL_ROUTE_POOL, "patrol-route")
ADD_ENUM(ePools::POINT_ROUTE_POOL, "point-route")
ADD_ENUM(ePools::POINTER_DOUBLE_LINK_POOL, "pointer-double-link-pool")
ADD_ENUM(ePools::POINTER_SINGLE_LINK_POOL, "pointer-single-link-pool")
ADD_ENUM(ePools::ENV_MAP_MATERIAL_POOL, "env-map-material")
ADD_ENUM(ePools::ENV_MAP_ATOMIC_POOL, "env-map-atomic")
ADD_ENUM(ePools::SPEC_MAP_MATERIAL_POOL, "spec-map-material")
IMPLEMENT_ENUM_END("gta-pool")

IMPLEMENT_ENUM_CLASS_BEGIN(eFxParticleSystems)
ADD_ENUM(eFxParticleSystems::PRT_BLOOD, "blood")
ADD_ENUM(eFxParticleSystems::PRT_BOATSPLASH, "boat_splash")
ADD_ENUM(eFxParticleSystems::PRT_BUBBLE, "bubble")
ADD_ENUM(eFxParticleSystems::PRT_DEBRIS, "car_debris")
ADD_ENUM(eFxParticleSystems::PRT_SMOKE, "collision_smoke")
ADD_ENUM(eFxParticleSystems::PRT_GUNSHELL, "gunshell")
ADD_ENUM(eFxParticleSystems::PRT_SAND, "sand")
ADD_ENUM(eFxParticleSystems::PRT_SAND2, "sand2")
ADD_ENUM(eFxParticleSystems::PRT_SMOKEHUGE, "huge_smoke")
ADD_ENUM(eFxParticleSystems::PRT_SMOKE2, "smoke")
ADD_ENUM(eFxParticleSystems::PRT_SPARK, "spark")
ADD_ENUM(eFxParticleSystems::PRT_SPARK2, "spark2")
ADD_ENUM(eFxParticleSystems::PRT_SPLASH, "splash")
ADD_ENUM(eFxParticleSystems::PRT_WAKE, "wake")
ADD_ENUM(eFxParticleSystems::PRT_WATERSPLASH, "water_splash")
ADD_ENUM(eFxParticleSystems::PRT_WHEELDIRT, "wheel_dirt")
ADD_ENUM(eFxParticleSystems::PRT_GLASS, "glass")
IMPLEMENT_ENUM_CLASS_END("particle-system")

IMPLEMENT_ENUM_BEGIN(eWorldProperty)
ADD_ENUM(AMBIENT_COLOR, "AmbientColor")
ADD_ENUM(AMBIENT_OBJ_COLOR, "AmbientObjColor")
ADD_ENUM(DIRECTIONAL_COLOR, "DirectionalColor")
ADD_ENUM(SPRITE_SIZE, "SpriteSize")
ADD_ENUM(SPRITE_BRIGHTNESS, "SpriteBrightness")
ADD_ENUM(POLE_SHADOW_STRENGTH, "PoleShadowStrength")
ADD_ENUM(SHADOW_STRENGTH, "ShadowStrength")
ADD_ENUM(SHADOWS_OFFSET, "ShadowsOffset")
ADD_ENUM(LIGHTS_ON_GROUND, "LightsOnGround")
ADD_ENUM(LOW_CLOUDS_COLOR, "LowCloudsColor")
ADD_ENUM(BOTTOM_CLOUDS_COLOR, "BottomCloudsColor")
ADD_ENUM(CLOUDS_ALPHA1, "CloudsAlpha")
ADD_ENUM(ILLUMINATION, "Illumination")
ADD_ENUM(WEATHER_WET_ROADS, "WetRoads")
ADD_ENUM(WEATHER_FOGGYNESS, "Foggyness")
ADD_ENUM(WEATHER_FOG, "Fog")
ADD_ENUM(WEATHER_RAIN_FOG, "RainFog")
ADD_ENUM(WEATHER_WATER_FOG, "WaterFog")
ADD_ENUM(WEATHER_SANDSTORM, "Sandstorm")
ADD_ENUM(WEATHER_RAINBOW, "Rainbow")
IMPLEMENT_ENUM_END("world-property")

IMPLEMENT_ENUM_CLASS_BEGIN(eModelLoadState)
ADD_ENUM(eModelLoadState::LOADSTATE_NOT_LOADED, "unloaded")
ADD_ENUM(eModelLoadState::LOADSTATE_LOADED, "loaded")
ADD_ENUM(eModelLoadState::LOADSTATE_REQUESTED, "requested")
ADD_ENUM(eModelLoadState::LOADSTATE_READING, "reading")
ADD_ENUM(eModelLoadState::LOADSTATE_FINISHING, "finishing")
IMPLEMENT_ENUM_CLASS_END("model-load-state")

IMPLEMENT_ENUM_CLASS_BEGIN(PreloadAreaOption)
ADD_ENUM(PreloadAreaOption::MODELS, "models")
ADD_ENUM(PreloadAreaOption::COLLISIONS, "collisions")
ADD_ENUM(PreloadAreaOption::ALL, "all")
IMPLEMENT_ENUM_CLASS_END("preload-area-option")


IMPLEMENT_ENUM_CLASS_BEGIN(taskType)
ADD_ENUM(taskType::PRIMARY_TASK, "primary")
ADD_ENUM(taskType::SECONDARY_TASK, "secondary")
IMPLEMENT_ENUM_CLASS_END("tasks-types")

IMPLEMENT_ENUM_BEGIN(eEntityType)
ADD_ENUM(ENTITY_TYPE_NOTHING, "unknown")
ADD_ENUM(ENTITY_TYPE_BUILDING, "building")
ADD_ENUM(ENTITY_TYPE_VEHICLE, "vehicle")
ADD_ENUM(ENTITY_TYPE_PED, "ped")
ADD_ENUM(ENTITY_TYPE_OBJECT, "object")
ADD_ENUM(ENTITY_TYPE_DUMMY, "dummy")
ADD_ENUM(ENTITY_TYPE_NOTINPOOLS, "unknown")
IMPLEMENT_ENUM_END("entity-type")

//
// CResource from userdata
//
CResource* UserDataCast(CResource* ptr, lua_State* luaState)
{
    return g_pClientGame->GetResourceManager()->GetResourceFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CXMLNode from userdata
//
CXMLNode* UserDataCast(CXMLNode* ptr, lua_State* luaState)
{
    return g_pCore->GetXML()->GetNodeFromID(reinterpret_cast<unsigned long>(ptr));
}

//
// CLuaTimer from userdata
//
CLuaTimer* UserDataCast(CLuaTimer* ptr, lua_State* luaState)
{
    if (CLuaMain* luaMain = CLuaDefs::m_pLuaManager->GetVirtualMachine(luaState); luaMain)
    {
        return luaMain->GetTimerManager()->GetTimerFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }

    return nullptr;
}

//
// CLuaVector2D from userdata
//
CLuaVector2D* UserDataCast(CLuaVector2D* ptr, lua_State* luaState)
{
    return CLuaVector2D::GetFromScriptID(reinterpret_cast<unsigned int>(ptr));
}

//
// CLuaVector3D from userdata
//
CLuaVector3D* UserDataCast(CLuaVector3D* ptr, lua_State* luaState)
{
    return CLuaVector3D::GetFromScriptID(reinterpret_cast<unsigned int>(ptr));
}

//
// CLuaVector4D from userdata
//
CLuaVector4D* UserDataCast(CLuaVector4D* ptr, lua_State* luaState)
{
    return CLuaVector4D::GetFromScriptID(reinterpret_cast<unsigned int>(ptr));
}

//
// CLuaMatrix from userdata
//
CLuaMatrix* UserDataCast(CLuaMatrix* ptr, lua_State* luaState)
{
    return CLuaMatrix::GetFromScriptID(reinterpret_cast<unsigned int>(ptr));
}

//
// CClientEntity from userdata
//
CClientEntity* UserDataToElementCast(CClientEntity* ptr, SharedUtil::ClassId classId, lua_State* luaState)
{
    CClientEntity* element = CElementIDs::GetElement(TO_ELEMENTID(ptr));

    if (element == nullptr || element->IsBeingDeleted() || !element->IsA(classId))
        return nullptr;

    return element;
}

//
// CRemoteCall from userdata
//
CRemoteCall* UserDataCast(CRemoteCall* ptr, lua_State* luaState)
{
    if (ptr && g_pClientGame->GetRemoteCalls()->CallExists(ptr))
        return ptr;

    return nullptr;
}

//
// Get best guess at name of userdata type
//
SString GetUserDataClassName(void* ptr, lua_State* luaVM, bool bFindElementType)
{
    // Try element
    if (CClientEntity* pClientElement = UserDataCast((CClientEntity*)ptr, nullptr))
    {
        if (bFindElementType)
            // Try gui element first
            if (CClientGUIElement* pGuiElement = DynamicCast<CClientGUIElement>(pClientElement))
                return EnumToString(pGuiElement->GetCGUIElement()->GetType());
            else
                return pClientElement->GetTypeName();
        else
            return GetClassTypeName(pClientElement);
    }

    if (auto* pVar = UserDataCast((CResource*)ptr, luaVM))            // Try resource
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CXMLNode*)ptr, luaVM))            // Try xml node
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaTimer*)ptr, luaVM))            // Try timer
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaVector2D*)ptr, luaVM))            // Try 2D Vector
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaVector3D*)ptr, luaVM))            // Try 3D Vector
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaVector4D*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CRemoteCall*)ptr, luaVM))
        return GetClassTypeName(pVar);

    return "";
}

//
// Reading mixed types
//

//
// DxFont/string
//
void MixedReadDxFontString(CScriptArgReader& argStream, eFontType& outFontType, eFontType defaultFontType, CClientDxFont*& poutDxFontElement)
{
    outFontType = FONT_DEFAULT;
    poutDxFontElement = NULL;
    if (argStream.NextIsNone())
        return;
    else if (argStream.NextIsString())
    {
        SString strFontName;
        argStream.ReadString(strFontName);
        StringToEnum(strFontName, outFontType);
        return;
    }
    else
        argStream.ReadUserData(poutDxFontElement);
}

//
// GuiFont/string
//
void MixedReadGuiFontString(CScriptArgReader& argStream, SString& strOutFontName, const char* szDefaultFontName, CClientGuiFont*& poutGuiFontElement)
{
    poutGuiFontElement = NULL;
    if (argStream.NextIsString() || argStream.NextIsNone())
        argStream.ReadString(strOutFontName, szDefaultFontName);
    else
        argStream.ReadUserData(poutGuiFontElement);
}

//
// Material/string
//
void MixedReadMaterialString(CScriptArgReader& argStream, CClientMaterial*& pMaterialElement)
{
    pMaterialElement = NULL;
    if (!argStream.NextIsString())
        argStream.ReadUserData(pMaterialElement);
    else
    {
        SString strFilePath;
        argStream.ReadString(strFilePath);

        // If no element, auto find/create one
        CLuaMain*  pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(argStream.m_luaVM);
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource() : NULL;
        if (pParentResource)
        {
            CResource* pFileResource = pParentResource;
            SString    strPath, strMetaPath;
            if (CResourceManager::ParseResourcePathInput(strFilePath, pFileResource, &strPath, &strMetaPath))
            {
                SString strUniqueName = SString("%s*%s*%s", pParentResource->GetName(), pFileResource->GetName(), strMetaPath.c_str()).Replace("\\", "/");
                pMaterialElement = g_pClientGame->GetManager()->GetRenderElementManager()->FindAutoTexture(strPath, strUniqueName);

                if (pMaterialElement)
                {
                    // Check if brand new
                    if (!pMaterialElement->GetParent())
                        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's
                        // ElementGroup? **
                        pMaterialElement->SetParent(pParentResource->GetResourceDynamicEntity());
                }
                else
                    argStream.SetCustomError(strFilePath, "Error loading image");
            }
            else
                argStream.SetCustomError(strFilePath, "Bad file path");
        }
    }
}

//
// Check 4x4 lua table
//
bool IsValidMatrixLuaTable(lua_State* luaVM, std::uint32_t argIndex) noexcept
{
    std::uint32_t cell = 0;

    if (lua_type(luaVM, argIndex) == LUA_TTABLE)
    {
        lua_pushnil(luaVM);
        for (std::uint32_t row = 0; lua_next(luaVM, argIndex) != 0; lua_pop(luaVM, 1), ++row)
        {
            if (lua_type(luaVM, -1) != LUA_TTABLE)
                return false;

            std::uint32_t col = 0;

            lua_pushnil(luaVM);
            for (; lua_next(luaVM, -2) != 0; lua_pop(luaVM, 1), ++col, ++cell)
            {
                int argumentType = lua_type(luaVM, -1);
                if (argumentType != LUA_TNUMBER && argumentType != LUA_TSTRING)
                    return false;
            }

            if (col != 4)
                return false;
        }
    }

    if (cell != 16)
        return false;

    return true;
}

//
// 4x4 matrix into CMatrix
//
bool ReadMatrix(lua_State* luaVM, uint uiArgIndex, CMatrix& outMatrix)
{
    float m[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    uint  uiRow = 0;
    uint  uiCell = 0;

    if (lua_type(luaVM, uiArgIndex) == LUA_TTABLE)
    {
        for (lua_pushnil(luaVM); lua_next(luaVM, uiArgIndex) != 0; lua_pop(luaVM, 1), uiRow++)
        {
            // int idx = lua_tonumber ( luaVM, -2 );
            // int iArgumentType = lua_type ( luaVM, -1 );
            if (lua_type(luaVM, -1) == LUA_TTABLE)
            {
                uint uiCol = 0;
                for (lua_pushnil(luaVM); lua_next(luaVM, -2) != 0; lua_pop(luaVM, 1), uiCol++, uiCell++)
                {
                    // int idx = lua_tonumber ( luaVM, -2 );
                    int iArgumentType = lua_type(luaVM, -1);
                    if (iArgumentType == LUA_TNUMBER || iArgumentType == LUA_TSTRING)
                    {
                        if (uiRow < 4 && uiCol < 4)
                            m[uiRow][uiCol] = static_cast<float>(lua_tonumber(luaVM, -1));
                    }
                }

                if (uiCol != 4)
                    return false;
            }
        }
    }

    if (uiRow != 4 || uiCell != 16)
        return false;

    outMatrix.vRight = CVector(m[0][0], m[0][1], m[0][2]);
    outMatrix.vFront = CVector(m[1][0], m[1][1], m[1][2]);
    outMatrix.vUp = CVector(m[2][0], m[2][1], m[2][2]);
    outMatrix.vPos = CVector(m[3][0], m[3][1], m[3][2]);
    return true;
}

//
// Check min client is correct
// Return false if below required
//
bool MinClientReqCheck(CScriptArgReader& argStream, const char* szVersionReq, const char* szReason)
{
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(argStream.m_luaVM);
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (pResource)
        {
            if (pResource->GetMinClientReq() < szVersionReq)
            {
                #if MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE
                if (szReason)
                    argStream.SetVersionWarning(szVersionReq, "client", szReason);
                #endif
                return false;
            }
        }
    }
    return true;
}

//
// Check min client is correct
// Thrown a error if below required
//
void MinClientReqCheck(lua_State* luaVM, const char* szVersionReq, const char* szReason)
{
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return;

    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return;
    
    if (pResource->GetMinClientReq() < szVersionReq)
    {
        #if MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE
        SString err("<min_mta_version> section in the meta.xml is incorrect or missing (expected at least client %s because %s)",
                                szVersionReq, szReason);
        throw std::invalid_argument(err);
        #endif
    }
    
}

//
// Read next as preg option flags
//
void ReadPregFlags(CScriptArgReader& argStream, pcrecpp::RE_Options& pOptions)
{
    if (argStream.NextIsNumber())
    {
        uint uiFlags = 0;
        argStream.ReadNumber(uiFlags);
        pOptions.set_caseless((uiFlags & 1) != 0);
        pOptions.set_multiline((uiFlags & 2) != 0);
        pOptions.set_dotall((uiFlags & 4) != 0);
        pOptions.set_extended((uiFlags & 8) != 0);
        pOptions.set_utf8((uiFlags & 16) != 0);
    }
    else if (argStream.NextIsString())
    {
        SString strFlags;
        argStream.ReadString(strFlags);
        for (uint i = 0; i < strFlags.length(); i++)
        {
            switch (strFlags[i])
            {
                case 'i':
                    pOptions.set_caseless(true);
                    break;
                case 'm':
                    pOptions.set_multiline(true);
                    break;
                case 'd':
                    pOptions.set_dotall(true);
                    break;
                case 'e':
                    pOptions.set_extended(true);
                    break;
                case 'u':
                    pOptions.set_utf8(true);
                    break;
                default:
                    argStream.SetCustomError("Flags all wrong");
                    return;
            }
        }
    }
}

//
// Return true if weapon property is a flag type
//
bool IsWeaponPropertyFlag(eWeaponProperty weaponProperty)
{
    return (weaponProperty >= WEAPON_FLAG_FIRST && weaponProperty <= WEAPON_FLAG_LAST);
}

//
// Get bit pattern for a weapon property flag enum
//
uint GetWeaponPropertyFlagBit(eWeaponProperty weaponProperty)
{
    if (!IsWeaponPropertyFlag(weaponProperty))
        return 0;

    // Check 20 bits from first to last
    dassert(WEAPON_FLAG_LAST + 1 - WEAPON_FLAG_FIRST == 20);

    uint uiFlagIndex = (weaponProperty - WEAPON_FLAG_FIRST);
    uint uiFlagBit = 1 << uiFlagIndex;
    return uiFlagBit;
}

//
// Set error if pThisResource does not have permission to modify pOtherResource
//
void CheckCanModifyOtherResource(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource)
{
    // No operation on the client
}

//
// Set error if pThisResource does not have permission to modify every resource in resourceList
//
void CheckCanModifyOtherResources(CScriptArgReader& argStream, CResource* pThisResource, std::initializer_list<CResource*> resourceList)
{
    // No operation on the client
}

//
// Set error if resource file access is blocked due to reasons
//
void CheckCanAccessOtherResourceFile(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource, const SString& strAbsPath,
                                     bool* pbReadOnly)
{
    // No operation on the client
}
