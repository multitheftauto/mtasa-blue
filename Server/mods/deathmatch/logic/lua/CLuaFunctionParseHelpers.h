/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/lua/CLuaFunctionParseHelpers.h
 *
 *****************************************************************************/

#pragma once

#include "LuaCommon.h"
#include "CElementIDs.h"
#include "CConsoleClient.h"
#include "CAccount.h"
#include "CEasingCurve.h"
#include "CAccessControlListRight.h"
#include <type_traits>

class CLuaVector2D;
class CLuaVector3D;
class CLuaVector4D;
class CLuaMatrix;

// Forward declare enum reflection stuff
enum eLuaType
{
};
DECLARE_ENUM(eLuaType);
DECLARE_ENUM(TrafficLight::EColor);
DECLARE_ENUM(TrafficLight::EState);
DECLARE_ENUM(CEasingCurve::eType);
DECLARE_ENUM(eWeaponType);
DECLARE_ENUM(eWeaponProperty);
DECLARE_ENUM(eWeaponSkill);
DECLARE_ENUM(eWeaponState);
DECLARE_ENUM(eWeaponFlags);
DECLARE_ENUM(CAccessControlListRight::ERightType);
DECLARE_ENUM(CElement::EElementType);
DECLARE_ENUM(CAccountPassword::EAccountPasswordType);
DECLARE_ENUM_CLASS(ESyncType);
DECLARE_ENUM_CLASS(eCustomDataClientTrust)

enum eHudComponent
{
    HUD_AMMO,
    HUD_WEAPON,
    HUD_HEALTH,
    HUD_BREATH,
    HUD_ARMOUR,
    HUD_MONEY,
    HUD_VEHICLE_NAME,
    HUD_AREA_NAME,
    HUD_RADAR,
    HUD_CLOCK,
    HUD_RADIO,
    HUD_WANTED,
    HUD_CROSSHAIR,
    HUD_ALL,
};
DECLARE_ENUM(eHudComponent);

// class -> class type
typedef int        eEntityType;
inline eEntityType GetClassType(CElement*)
{
    return -1;
}
inline eEntityType GetClassType(class CPlayer*)
{
    return CElement::PLAYER;
}
inline eEntityType GetClassType(class CVehicle*)
{
    return CElement::VEHICLE;
}
inline eEntityType GetClassType(class CBlip*)
{
    return CElement::BLIP;
}
inline eEntityType GetClassType(class CObject*)
{
    return CElement::OBJECT;
}
inline eEntityType GetClassType(class CPickup*)
{
    return CElement::PICKUP;
}
inline eEntityType GetClassType(class CRadarArea*)
{
    return CElement::RADAR_AREA;
}
inline eEntityType GetClassType(class CMarker*)
{
    return CElement::MARKER;
}
inline eEntityType GetClassType(class CTeam*)
{
    return CElement::TEAM;
}
inline eEntityType GetClassType(class CPed*)
{
    return CElement::PED;
}
inline eEntityType GetClassType(class CColShape*)
{
    return CElement::COLSHAPE;
}
inline eEntityType GetClassType(class CDummy*)
{
    return CElement::DUMMY;
}
inline eEntityType GetClassType(class CScriptFile*)
{
    return CElement::SCRIPTFILE;
}
inline eEntityType GetClassType(class CWater*)
{
    return CElement::WATER;
}
inline eEntityType GetClassType(class CDatabaseConnectionElement*)
{
    return CElement::DATABASE_CONNECTION;
}
inline eEntityType GetClassType(class CCustomWeapon*)
{
    return CElement::WEAPON;
}

// class -> class name
inline SString GetClassTypeName(CElement*)
{
    return "element";
}
inline SString GetClassTypeName(CClient*)
{
    return "client";
}
inline SString GetClassTypeName(CPlayer*)
{
    return "player";
}
inline SString GetClassTypeName(CVehicle*)
{
    return "vehicle";
}
inline SString GetClassTypeName(CBlip*)
{
    return "blip";
}
inline SString GetClassTypeName(CObject*)
{
    return "object";
}
inline SString GetClassTypeName(CPickup*)
{
    return "pickup";
}
inline SString GetClassTypeName(CRadarArea*)
{
    return "radararea";
}
inline SString GetClassTypeName(CMarker*)
{
    return "marker";
}
inline SString GetClassTypeName(CTeam*)
{
    return "team";
}
inline SString GetClassTypeName(CPed*)
{
    return "ped";
}
inline SString GetClassTypeName(class CRemoteCall*)
{
    return "request";
}
inline SString GetClassTypeName(CColShape*)
{
    return "colshape";
}
inline SString GetClassTypeName(CDummy*)
{
    return "dummy";
}
inline SString GetClassTypeName(CScriptFile*)
{
    return "scriptfile";
}
inline SString GetClassTypeName(CWater*)
{
    return "water";
}
inline SString GetClassTypeName(CDatabaseConnectionElement*)
{
    return "db-connection";
}

inline SString GetClassTypeName(CResource*)
{
    return "resource-data";
}
inline SString GetClassTypeName(CXMLNode*)
{
    return "xml-node";
}
inline SString GetClassTypeName(CLuaTimer*)
{
    return "lua-timer";
}
inline SString GetClassTypeName(CAccount*)
{
    return "account";
}
inline SString GetClassTypeName(CDbJobData*)
{
    return "db-query";
}
inline SString GetClassTypeName(CAccessControlList*)
{
    return "acl";
}
inline SString GetClassTypeName(CAccessControlListGroup*)
{
    return "acl-group";
}
inline SString GetClassTypeName(CCustomWeapon*)
{
    return "weapon";
}
inline SString GetClassTypeName(CBan*)
{
    return "ban";
}
inline SString GetClassTypeName(CTextItem*)
{
    return "text-item";
}
inline SString GetClassTypeName(CTextDisplay*)
{
    return "text-display";
}
inline SString GetClassTypeName(CLuaVector2D*)
{
    return "vector2";
}
inline SString GetClassTypeName(CLuaVector3D*)
{
    return "vector3";
}
inline SString GetClassTypeName(CLuaVector4D*)
{
    return "vector4";
}
inline SString GetClassTypeName(CLuaMatrix*)
{
    return "matrix";
}

//
// CResource from userdata
//
CResource* UserDataCast(CResource* ptr, lua_State* luaState);

//
// CXMLNode from userdata
//
CXMLNode* UserDataCast(CXMLNode* ptr, lua_State* luaState);

//
// CLuaTimer from userdata
//
CLuaTimer* UserDataCast(CLuaTimer* ptr, lua_State* luaState);

//
// CAccount from userdata
//
CAccount* UserDataCast(CAccount* ptr, lua_State* luaState);

//
// CDbJobData from userdata
//
CDbJobData* UserDataCast(CDbJobData* ptr, lua_State* luaState);

//
// CAccessControlList from userdata
//
CAccessControlList* UserDataCast(CAccessControlList* ptr, lua_State* luaState);

//
// CAccessControlListGroup from userdata
//
CAccessControlListGroup* UserDataCast(CAccessControlListGroup* ptr, lua_State* luaState);

//
// CTextItem from userdata
//
CTextItem* UserDataCast(CTextItem* ptr, lua_State* luaState);

//
// CTextDisplay from userdata
//
CTextDisplay* UserDataCast(CTextDisplay* ptr, lua_State* luaState);

//
// CBan from userdata
//
CBan* UserDataCast(CBan* ptr, lua_State* luaState);

//
// CLuaVector2D from userdata
//
CLuaVector2D* UserDataCast(CLuaVector2D* ptr, lua_State* luaState);

//
// CLuaVector3D from userdata
//
CLuaVector3D* UserDataCast(CLuaVector3D* ptr, lua_State* luaState);

//
// CLuaVector4D from userdata
//
CLuaVector4D* UserDataCast(CLuaVector4D* ptr, lua_State* luaState);

//
// CLuaMatrix from userdata
//
CLuaMatrix* UserDataCast(CLuaMatrix* ptr, lua_State* luaState);

//
// CElement from userdata
//
CElement* UserDataToElementCast(CElement* ptr, eEntityType entityType, lua_State* luaState);

template <typename T, typename = std::enable_if_t<std::is_base_of_v<CElement, T>>>
T* UserDataCast(T* ptr, lua_State* luaState)
{
    return reinterpret_cast<T*>(UserDataToElementCast(ptr, GetClassType(static_cast<T*>(nullptr)), luaState));
}

//
// CPed from userdata
//
CPed* UserDataCast(CPed* ptr, lua_State* luaState);

//
// CRemoteCall from userdata
//
CRemoteCall* UserDataCast(CRemoteCall* ptr, lua_State* luaState);

//
// CPlayer from userdata
// Disallows conversion of CPeds to CPlayers
//
CPlayer* UserDataCast(CPlayer* ptr, lua_State* luaState);

//
// CClient from CConsoleClient or a CPlayer
//
CClient* UserDataCast(CClient* ptr, lua_State* luaState);

//
// CElement ( something )
//
// Returns true if T is the same class as the one wrapped by pElement
template <class T>
bool CheckWrappedUserDataType(CElement*& pElement, SString& strErrorExpectedType)
{
    // Not used server side (yet)
    strErrorExpectedType = GetClassTypeName((T*)0);
    return false;
}

//
// Get element from ID and ensure type is what is expected
//
template <class T>
T* GetElementFromId(ElementID elementId)
{
    CElement* pElement = CElementIDs::GetElement(elementId);
    if (!pElement || pElement->IsBeingDeleted() || (pElement->GetType() != GetClassType((T*)0) && GetClassType((T*)0) != -1))
        return NULL;
    return (T*)pElement;
}

class CScriptArgReader;
SString GetUserDataClassName(void* ptr, lua_State* luaVM, bool bFindElementType = true);
void    MixedReadResourceString(CScriptArgReader& argStream, SString& strOutResourceName);
void    MixedReadResourceString(CScriptArgReader& argStream, CResource*& pOutResource);
bool    StringToBool(const SString& strText);
void    MinServerReqCheck(CScriptArgReader& argStream, const char* szVersionReq, const char* szReason);
void    ReadPregFlags(CScriptArgReader& argStream, pcrecpp::RE_Options& pOptions);
bool    IsValidMatrixLuaTable(lua_State* luaVM, std::uint32_t argIndex) noexcept;
bool    ReadMatrix(lua_State* luaVM, uint uiArgIndex, CMatrix& outMatrix);

//
// Resource access helpers
//
enum class eResourceModifyScope
{
    NONE,
    SINGLE_RESOURCE,
    EVERY_RESOURCE,
};

eResourceModifyScope GetResourceModifyScope(CResource* pThisResource, CResource* pOtherResource);
void                 CheckCanModifyOtherResource(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource);
void                 CheckCanModifyOtherResources(CScriptArgReader& argStream, CResource* pThisResource, std::initializer_list<CResource*> resourceList);
void CheckCanAccessOtherResourceFile(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource, const SString& strAbsPath,
                                     bool* pbReadOnly = nullptr);

//
// Other misc helpers
//
bool IsWeaponPropertyFlag(eWeaponProperty weaponProperty);
uint GetWeaponPropertyFlagBit(eWeaponProperty weaponProperty);
