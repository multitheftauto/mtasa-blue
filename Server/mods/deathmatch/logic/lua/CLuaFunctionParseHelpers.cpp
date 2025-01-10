/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/lua/CLuaFunctionParseHelpers.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaFunctionParseHelpers.h"
#include "CScriptArgReader.h"
#include "CResource.h"
#include "CResourceManager.h"
#include "CGame.h"
#include "CRemoteCalls.h"
#include "CMainConfig.h"
#include "CAccessControlListManager.h"
#include "CDatabaseManager.h"
#include "CBan.h"

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

IMPLEMENT_ENUM_BEGIN(eJSONPrettyType)
ADD_ENUM(JSONPRETTY_SPACES, "spaces")
ADD_ENUM(JSONPRETTY_NONE, "none")
ADD_ENUM(JSONPRETTY_TABS, "tabs")
IMPLEMENT_ENUM_END("json-pretty-type")

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
ADD_ENUM(WEAPON_LIFE_SPAN, "life_span")
ADD_ENUM(WEAPON_FIRING_SPEED, "firing_speed")
ADD_ENUM(WEAPON_SPREAD, "spread")
ADD_ENUM(WEAPON_MAX_CLIP_AMMO, "maximum_clip_ammo")
ADD_ENUM(WEAPON_MOVE_SPEED, "move_speed")
ADD_ENUM(WEAPON_FLAGS, "flags")
ADD_ENUM(WEAPON_ANIM_GROUP, "anim_group")

// Set is Disabled >= FIRETYPE - Move above WEAPON_TYPE_SET_DISABLED to re-enable and move it above here to make it less confusing.
ADD_ENUM(WEAPON_FIRETYPE, "fire_type")

ADD_ENUM(WEAPON_MODEL, "model")
ADD_ENUM(WEAPON_MODEL2, "model2")

ADD_ENUM(WEAPON_SLOT, "weapon_slot")

ADD_ENUM(WEAPON_FIRE_OFFSET, "fire_offset")

ADD_ENUM(WEAPON_SKILL_LEVEL, "skill_level")
ADD_ENUM(WEAPON_REQ_SKILL_LEVEL, "required_skill_level")

ADD_ENUM(WEAPON_ANIM_LOOP_START, "anim_loop_start")
ADD_ENUM(WEAPON_ANIM_LOOP_STOP, "anim_loop_stop")
ADD_ENUM(WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME, "anim_loop_bullet_fire")

ADD_ENUM(WEAPON_ANIM2_LOOP_START, "anim2_loop_start")
ADD_ENUM(WEAPON_ANIM2_LOOP_STOP, "anim2_loop_stop")
ADD_ENUM(WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME, "anim2_loop_bullet_fire")

ADD_ENUM(WEAPON_ANIM_BREAKOUT_TIME, "anim_breakout_time")

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
IMPLEMENT_ENUM_END("weapon-property")

IMPLEMENT_ENUM_BEGIN(eWeaponSkill)
ADD_ENUM(WEAPONSKILL_POOR, "poor")
ADD_ENUM(WEAPONSKILL_STD, "std")
ADD_ENUM(WEAPONSKILL_PRO, "pro")
IMPLEMENT_ENUM_END("weapon-skill")

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

IMPLEMENT_ENUM_BEGIN(CAccessControlListRight::ERightType)
ADD_ENUM(CAccessControlListRight::RIGHT_TYPE_COMMAND, "command")
ADD_ENUM(CAccessControlListRight::RIGHT_TYPE_FUNCTION, "function")
ADD_ENUM(CAccessControlListRight::RIGHT_TYPE_RESOURCE, "resource")
ADD_ENUM(CAccessControlListRight::RIGHT_TYPE_GENERAL, "general")
IMPLEMENT_ENUM_END("right-type")

IMPLEMENT_ENUM_BEGIN(CElement::EElementType)
ADD_ENUM(CElement::DUMMY, "dummy")
ADD_ENUM(CElement::PLAYER, "player")
ADD_ENUM(CElement::VEHICLE, "vehicle")
ADD_ENUM(CElement::OBJECT, "object")
ADD_ENUM(CElement::MARKER, "marker")
ADD_ENUM(CElement::BLIP, "blip")
ADD_ENUM(CElement::PICKUP, "pickup")
ADD_ENUM(CElement::RADAR_AREA, "radararea")
ADD_ENUM(CElement::SPAWNPOINT_DEPRECATED, "spawnpoint_deprecated")
ADD_ENUM(CElement::REMOTECLIENT_DEPRECATED, "remoteclient_deprecated")
ADD_ENUM(CElement::CONSOLE, "console")
ADD_ENUM(CElement::PATH_NODE_UNUSED, "pathnode_unused")
ADD_ENUM(CElement::WORLD_MESH_UNUSED, "worldmesh_unused")
ADD_ENUM(CElement::TEAM, "team")
ADD_ENUM(CElement::PED, "ped")
ADD_ENUM(CElement::COLSHAPE, "colshape")
ADD_ENUM(CElement::SCRIPTFILE, "scriptfile")
ADD_ENUM(CElement::WATER, "water")
ADD_ENUM(CElement::DATABASE_CONNECTION, "db-connection")
ADD_ENUM(CElement::ROOT, "root")
ADD_ENUM(CElement::UNKNOWN, "unknown")
IMPLEMENT_ENUM_END_DEFAULTS("element-type", CElement::UNKNOWN, "unknown")

IMPLEMENT_ENUM_BEGIN(CAccountPassword::EAccountPasswordType)
ADD_ENUM(CAccountPassword::PLAINTEXT, "plaintext")
ADD_ENUM(CAccountPassword::SHA256, "sha256")
ADD_ENUM(CAccountPassword::MD5, "md5")
IMPLEMENT_ENUM_END("account-password-type")

IMPLEMENT_ENUM_CLASS_BEGIN(ESyncType)
ADD_ENUM(ESyncType::BROADCAST, "broadcast")
ADD_ENUM(ESyncType::LOCAL, "local")
ADD_ENUM(ESyncType::SUBSCRIBE, "subscribe")
IMPLEMENT_ENUM_CLASS_END("sync-mode")

IMPLEMENT_ENUM_CLASS_BEGIN(eCustomDataClientTrust)
ADD_ENUM(eCustomDataClientTrust::UNSET, "default")
ADD_ENUM(eCustomDataClientTrust::ALLOW, "allow")
ADD_ENUM(eCustomDataClientTrust::DENY, "deny")
IMPLEMENT_ENUM_CLASS_END("client-trust-mode")

//
// CResource from userdata
//
CResource* UserDataCast(CResource* ptr, lua_State* luaState)
{
    return g_pGame->GetResourceManager()->GetResourceFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CXMLNode from userdata
//
CXMLNode* UserDataCast(CXMLNode* ptr, lua_State* luaState)
{
    return g_pServerInterface->GetXML()->GetNodeFromID(reinterpret_cast<unsigned long>(ptr));
}

//
// CLuaTimer from userdata
//
CLuaTimer* UserDataCast(CLuaTimer* ptr, lua_State* luaState)
{
    if (CLuaMain* luaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaState); luaMain != nullptr)
    {
        return luaMain->GetTimerManager()->GetTimerFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }

    return nullptr;
}

//
// CAccount from userdata
//
CAccount* UserDataCast(CAccount* ptr, lua_State* luaState)
{
    return g_pGame->GetAccountManager()->GetAccountFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CDbJobData from userdata
//
CDbJobData* UserDataCast(CDbJobData* ptr, lua_State* luaState)
{
    return g_pGame->GetDatabaseManager()->GetQueryFromId(reinterpret_cast<SDbJobId>(ptr));
}

//
// CAccessControlList from userdata
//
CAccessControlList* UserDataCast(CAccessControlList* ptr, lua_State* luaState)
{
    return g_pGame->GetACLManager()->GetACLFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CAccessControlListGroup from userdata
//
CAccessControlListGroup* UserDataCast(CAccessControlListGroup* ptr, lua_State* luaState)
{
    return g_pGame->GetACLManager()->GetGroupFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CTextItem from userdata
//
CTextItem* UserDataCast(CTextItem* ptr, lua_State* luaState)
{
    if (CLuaMain* luaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaState); luaMain != nullptr)
    {
        return luaMain->GetTextItemFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }

    return nullptr;
}

//
// CTextDisplay from userdata
//
CTextDisplay* UserDataCast(CTextDisplay* ptr, lua_State* luaState)
{
    if (CLuaMain* luaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaState); luaMain != nullptr)
    {
        return luaMain->GetTextDisplayFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }

    return nullptr;
}

//
// CBan from userdata
//
CBan* UserDataCast(CBan* ptr, lua_State* luaState)
{
    return g_pGame->GetBanManager()->GetBanFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CLuaVector2D from userdata
//
CLuaVector2D* UserDataCast(CLuaVector2D* ptr, lua_State* luaState)
{
    return CLuaVector2D::GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CLuaVector3D from userdata
//
CLuaVector3D* UserDataCast(CLuaVector3D* ptr, lua_State* luaState)
{
    return CLuaVector3D::GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CLuaVector4D from userdata
//
CLuaVector4D* UserDataCast(CLuaVector4D* ptr, lua_State* luaState)
{
    return CLuaVector4D::GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CLuaMatrix from userdata
//
CLuaMatrix* UserDataCast(CLuaMatrix* ptr, lua_State* luaState)
{
    return CLuaMatrix::GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

//
// CElement from userdata
//
CElement* UserDataToElementCast(CElement* ptr, eEntityType entityType, lua_State* luaState)
{
    CElement* element = CElementIDs::GetElement(TO_ELEMENTID(ptr));

    if (element == nullptr || element->IsBeingDeleted() || (element->GetType() != entityType && entityType != -1))
        return nullptr;

    return element;
}

//
// CPed from userdata
//
CPed* UserDataCast(CPed* ptr, lua_State* luaState)
{
    ElementID ID = TO_ELEMENTID(ptr);
    CElement* pElement = CElementIDs::GetElement(ID);
    if (!pElement || pElement->IsBeingDeleted() || (pElement->GetType() != CElement::PED && pElement->GetType() != CElement::PLAYER))
        return nullptr;
    return (CPed*)pElement;
}

//
// CRemoteCall from userdata
//
CRemoteCall* UserDataCast(CRemoteCall* ptr, lua_State* luaState)
{
    if (ptr && g_pGame->GetRemoteCalls()->CallExists(ptr))
    {
        return ptr;
    }

    return nullptr;
}

//
// CPlayer from userdata
// Disallows conversion of CPeds to CPlayers
//
CPlayer* UserDataCast(CPlayer* ptr, lua_State* luaState)
{
    ElementID ID = TO_ELEMENTID(ptr);
    CElement* pElement = CElementIDs::GetElement(ID);
    if (!pElement || pElement->IsBeingDeleted() || (pElement->GetType() != CElement::PLAYER))
        return nullptr;
    return (CPlayer*)pElement;
}

//
// CClient from CConsoleClient or a CPlayer
//
CClient* UserDataCast(CClient* ptr, lua_State* luaState)
{
    ElementID ID = TO_ELEMENTID(ptr);
    CElement* pElement = CElementIDs::GetElement(ID);
    if (!pElement || pElement->IsBeingDeleted())
        return nullptr;

    CClient* pClient = nullptr;
    if (pElement->GetType() == CElement::PLAYER)
        pClient = reinterpret_cast<CPlayer*>(pElement);
    else if (pElement->GetType() == CElement::CONSOLE)
        pClient = reinterpret_cast<CConsoleClient*>(pElement);

    return pClient;
}

//
// Get best guess at name of userdata type
//
SString GetUserDataClassName(void* ptr, lua_State* luaVM, bool bFindElementType)
{
    if (CElement* pVar = UserDataCast((CElement*)ptr, luaVM))            // Try element
        return bFindElementType ? pVar->GetTypeName() : GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast(static_cast<CResource*>(ptr), luaVM))            // Try resource
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CXMLNode*)ptr, luaVM))            // Try xml node
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaTimer*)ptr, luaVM))            // Try timer
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CAccount*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CDbJobData*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CAccessControlList*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CAccessControlListGroup*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CCustomWeapon*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CBan*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CTextItem*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CTextDisplay*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaVector2D*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaVector3D*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaVector4D*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CLuaMatrix*)ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast((CRemoteCall*)ptr, luaVM))
        return GetClassTypeName(pVar);

    return "";
}

//
// Read next as resource or resource name.  Result output as string
//
void MixedReadResourceString(CScriptArgReader& argStream, SString& strOutResourceName)
{
    if (!argStream.NextIsString())
    {
        CResource* pResource;
        argStream.ReadUserData(pResource);
        if (pResource)
            strOutResourceName = pResource->GetName();
    }
    else
        argStream.ReadString(strOutResourceName);
}

//
// Read next as resource or resource name.  Result output as resource
//
void MixedReadResourceString(CScriptArgReader& argStream, CResource*& pOutResource)
{
    if (!argStream.NextIsString())
    {
        argStream.ReadUserData(pOutResource);
    }
    else
    {
        SString strResourceName;
        argStream.ReadString(strResourceName);
        pOutResource = g_pGame->GetResourceManager()->GetResource(strResourceName);

        if (!pOutResource)
            argStream.SetTypeError("resource", argStream.m_iIndex - 1);
    }
}

///////////////////////////////////////////////////////////////
//
// StringToBool
//
// Convert a string into a best guess bool equivalent
//
///////////////////////////////////////////////////////////////
bool StringToBool(const SString& strText)
{
    return (strText == "true" || strText == "1" || strText == "yes");
}

//
// Check min server is correct
//
void MinServerReqCheck(CScriptArgReader& argStream, const char* szVersionReq, const char* szReason)
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(argStream.m_luaVM);
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (pResource)
        {
            if (pResource->GetMinServerRequirement() < szVersionReq)
            {
                #if MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE
                argStream.SetVersionWarning(szVersionReq, "server", szReason);
                #endif
            }
        }
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
// Return true if weapon property is a flag type
//
bool IsWeaponPropertyFlag(eWeaponProperty weaponProperty)
{
    return (weaponProperty >= WEAPON_FLAG_FIRST && weaponProperty <= WEAPON_FLAG_LAST);
}

//
// Get bit pattern for a weapon property flag
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
// Returns the permission level of pThisResource to modify pOtherResource
//
eResourceModifyScope GetResourceModifyScope(CResource* pThisResource, CResource* pOtherResource)
{
    if (pThisResource == pOtherResource)
        return eResourceModifyScope::SINGLE_RESOURCE;

    CAccessControlListManager* const pACLManager = g_pGame->GetACLManager();
    const SString&                   strResourceName = pThisResource->GetName();

    // Check if resource has right to modify any resource
    if (pACLManager->CanObjectUseRight(strResourceName.c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, "ModifyOtherObjects",
                                       CAccessControlListRight::RIGHT_TYPE_GENERAL, false))
        return eResourceModifyScope::EVERY_RESOURCE;

    // Check if resource has right to modify only pOtherResource
    const SString strRightName("ModifyOtherObjects.%s", pOtherResource->GetName().c_str());

    if (pACLManager->CanObjectUseRight(strResourceName.c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, strRightName.c_str(),
                                       CAccessControlListRight::RIGHT_TYPE_GENERAL, false))
        return eResourceModifyScope::SINGLE_RESOURCE;

    return eResourceModifyScope::NONE;
}

//
// Set error if pThisResource does not have permission to modify pOtherResource
//
void CheckCanModifyOtherResource(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource)
{
    if (GetResourceModifyScope(pThisResource, pOtherResource) == eResourceModifyScope::NONE)
        argStream.SetCustomError(
            SString("ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName().c_str(), pOtherResource->GetName().c_str()),
            "Access denied");
}

//
// Set error if pThisResource does not have permission to modify every resource in resourceList
//
void CheckCanModifyOtherResources(CScriptArgReader& argStream, CResource* pThisResource, std::initializer_list<CResource*> resourceList)
{
    // std::unordered_set only allows unique values and resourceList can contain duplicates
    std::unordered_set<CResource*> setNoPermissionResources;

    for (CResource* pOtherResource : resourceList)
    {
        eResourceModifyScope modifyScope = GetResourceModifyScope(pThisResource, pOtherResource);

        if (modifyScope == eResourceModifyScope::SINGLE_RESOURCE)
            continue;

        if (modifyScope == eResourceModifyScope::EVERY_RESOURCE)
            return;

        setNoPermissionResources.emplace(pOtherResource);
    }

    if (setNoPermissionResources.empty())
        return;

    std::stringstream ssResourceNames;
    size_t            remainingElements = setNoPermissionResources.size();

    for (CResource* pResource : setNoPermissionResources)
    {
        ssResourceNames << pResource->GetName();

        if (remainingElements > 1)
            ssResourceNames << ", ";

        --remainingElements;
    }

    argStream.SetCustomError(
        SString("ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName().c_str(), ssResourceNames.str().c_str()), "Access denied");
}

//
// Set error if resource file access is blocked due to reasons
//
void CheckCanAccessOtherResourceFile(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource, const SString& strAbsPath,
                                     bool* pbReadOnly)
{
    if (!g_pGame->GetConfig()->IsDatabaseCredentialsProtectionEnabled())
        return;

    // Is other resource different and requested access denied
    if ((pThisResource != pOtherResource) && pOtherResource->IsFileDbConnectMysqlProtected(strAbsPath, pbReadOnly ? *pbReadOnly : false))
    {
        // No access - See if we can change to readonly
        if (pbReadOnly && *pbReadOnly == false)
        {
            if (!pOtherResource->IsFileDbConnectMysqlProtected(strAbsPath, true))
            {
                // Yes readonly access
                *pbReadOnly = true;
                return;
            }
        }
        argStream.SetCustomError(
            SString("Database credentials protection denied resource %s to access %s", *pThisResource->GetName(), *pOtherResource->GetName()), "Access denied");
    }
}
