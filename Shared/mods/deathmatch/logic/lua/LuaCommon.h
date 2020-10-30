/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/LuaCommon.h
 *  PURPOSE:     Lua common functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "CIdArray.h"
#include <type_traits>

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

CLuaFunctionRef luaM_toref(lua_State* luaVM, int iArgument);
// Public use:

// Predeclarations of our classes
class CLuaTimer;
class CResource;
class CXMLNode;

class CMatrix;
class CVector2D;
class CVector;
class CVector4D;
class CLuaMatrix;
class CLuaVector2D;
class CLuaVector3D;
class CLuaVector4D;

#ifdef MTA_CLIENT
class CClientEntity;
class CClientColModel;
class CClientColShape;
class CScriptFile;
class CClientDFF;
class CClientEntity;
class CClientGUIElement;
class CClientMarker;
class CClientObject;
class CClientPed;
class CClientPickup;
class CClientPlayer;
class CClientRadarMarker;
class CClientTeam;
class CClientTXD;
class CClientVehicle;
class CClientWater;
class CClientWeapon;
class CClientRadarArea;
class CClientPointLights;
#else
class CElement;
class CDbJobData;
class CClient;
#endif

// Internal use functions
template<class T>
const char* GetClassNameIfOOPEnabled(lua_State* luaVM, T* object);

// Lua class things - creating a class, etc.. -
void lua_initclasses(lua_State* luaVM);

void lua_newclass(lua_State* luaVM);
void lua_getclass(lua_State* luaVM, const char* szName);
void lua_registerclass(lua_State* luaVM, const char* szName, const char* szParent = NULL, bool bRegisterWithEnvironment = true);
void lua_registerstaticclass(lua_State* luaVM, const char* szName);
void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* fn);
void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* set, const char* get);
void lua_classmetamethod(lua_State* luaVM, const char* szName, lua_CFunction fn);

#ifdef MTA_CLIENT
void lua_classfunction(lua_State* luaVM, const char* szFunction, lua_CFunction fn);
void lua_classvariable(lua_State* luaVM, const char* szVariable, lua_CFunction set, lua_CFunction get);
#else
void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* szACLName, lua_CFunction fn);
void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* szACLNameSet, const char* szACLNameGet, lua_CFunction set, lua_CFunction get, bool bACLIgnore = true);
#endif

// Public things:

// Helper
#ifdef MTA_CLIENT
CClientEntity* lua_toelement(lua_State* luaVM, int iArgument);
#else
CElement* lua_toelement(lua_State* luaVM, int iArgument);
#endif

void lua_pushuserdata(lua_State* luaVM, void* value);

// pushobject section:

// This one should only be used in LuaCommon
void lua_pushobject(lua_State* luaVM, const char* szClass, SArrayId id, bool bSkipCache = false);

// Only disable for CElement/CClientEntity as it needs special handling
// Everything else will call the reload anyways, because it'll fail here (because it doesnt have `GetScriptID()`)
#ifdef MTA_CLIENT
template<class T, typename = std::enable_if_t<!std::is_base_of_v<CClientEntity, T>>>
#else
template<class T, typename = std::enable_if_t<!std::is_base_of_v<CElement, T>>>
#endif
void lua_pushobject(lua_State* luaVM, T* object)
{
    static_assert(!std::is_pointer_v<T> && !std::is_reference_v<T>, "T must be an object, not a pointer to a pointer, or something..");
    lua_pushobject(luaVM, GetClassNameIfOOPEnabled(luaVM, object), (SArrayId)object->GetScriptID());
}

void lua_pushobject(lua_State* luaVM, CXMLNode* node);
void lua_pushobject(lua_State* luaVM, const CVector4D& vector);
void lua_pushobject(lua_State* luaVM, const CVector& vector);
void lua_pushobject(lua_State* luaVM, const CVector2D& vector);
void lua_pushobject(lua_State* luaVM, const CMatrix& matrix);

#ifdef MTA_CLIENT
void lua_pushobject(lua_State* luaVM, CClientEntity* element);
#else
void lua_pushobject(lua_State* luaVM, CDbJobData* jobdata);
void lua_pushobject(lua_State* luaVM, CElement* element);
#endif





// Solve circular dependency between LuaBasic.h (is included in CLuaDefs) and LuaCommon.h
// by declaring GetClassNameIfOOPEnabled on top, and then defining it here.. Hacky, but no better solution (yet)

#include "luadefs/CLuaClassDefs.h"
template<class T>
const char* GetClassNameIfOOPEnabled(lua_State* luaVM, T* object)
{
#ifdef MTA_CLIENT
    extern CClientGame* g_pClientGame;
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
#else
    extern CGame* g_pGame;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
#endif
    return (pLuaMain && pLuaMain->IsOOPEnabled()) ? CLuaClassDefs::GetLuaClassName(object) : nullptr;
}

// Not even God knows what kind of evil enum this is...
enum
{
    AUDIO_FRONTEND,
    AUDIO_MISSION_PRELOAD,
    AUDIO_MISSION_PLAY
};
