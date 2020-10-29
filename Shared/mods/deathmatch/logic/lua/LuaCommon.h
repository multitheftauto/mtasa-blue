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


// Public use:

// Predeclarations of our classes
class CLuaTimer;
class CResource;
class CXMLNode;
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

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

// Internal use functions
template<class T>
const char* GetClassNameIfOOPEnabled(lua_State* luaVM, T* object)
{
#ifdef MTA_CLIENT
    extern CClientGame* g_pGame;
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
#else
    extern CGame* g_pGame;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
#endif
    return (pLuaMain && pLuaMain->IsOOPEnabled()) ? CLuaClassDefs::GetLuaClassName(object) : nullptr;
}

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

// This one should only be used in LuaCommon
void lua_pushobject(lua_State* luaVM, const char* szClass, SArrayId id, bool bSkipCache = false);

// Public use:
#ifdef MTA_CLIENT
CClientEntity* lua_toelement(lua_State* luaVM, int iArgument);
#else
CElement* lua_toelement(lua_State* luaVM, int iArgument);
#endif

void lua_pushuserdata(lua_State* luaVM, void* value);
void lua_pushobject(lua_State* luaVM, const char* szClass, void* pObject, bool bSkipCache = false);

void lua_pushobject(lua_State* luaVM, const char* szClass, SArrayId id, bool bSkipCache = false);
void lua_pushobject(lua_State* luaVM, const CVector2D& vector);
void lua_pushobject(lua_State* luaVM, const CVector& vector);
void lua_pushobject(lua_State* luaVM, const CVector4D& vector);
void lua_pushobject(lua_State* luaVM, const CMatrix& matrix);

template<class T>
void lua_pushobject(lua_State* luaVM, T* object)
{
    static_assert(!std::is_pointer_v<T> && !std::is_reference_v<T>, "T must be an object, not a pointer to a pointer, or something..");

    const auto push = [luaVM, object](const auto& value) {
    #ifdef MTA_CLIENT
        CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
    #else
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
    #endif
        const auto className = (pLuaMain && pLuaMain->IsOOPEnabled()) ? CLuaClassDefs::GetLuaClassName(object) : nullptr;
        lua_pushobject(luaVM, className, (void*)reinterpret_cast<unsigned int*>(value));
    };

#ifdef MTA_CLIENT
    using Element_t = CClientEntity;
#else
    using Element_t = CElement;
#endif

    using Decayed_t = std::decay_t<T>;
    if constexpr (std::is_same_v<CXMLNode, Decayed_t>)
        push(object->GetID());
    else if constexpr (std::is_base_of_v<Element_t, Decayed_t>) // Handle types that derive from 
    {
        auto element = static_cast<Element_t*>(object);
        if (element->IsBeingDeleted())
            lua_pushboolean(luaVM, false);
        else if (const auto ID = element->GetID(); ID != INVALID_ELEMENT_ID)
            push(ID.Value());
        else
            lua_pushnil(luaVM); // Invalid element ID
    }
#ifndef MTA_CLIENT
    else if constexpr (std::is_same_v<CDbJobData, Decayed_t>)
        push(object->GetId());

    else if constexpr (std::is_base_of_v<CClient, Decayed_t>) // Handle types deriving from CClient (such as CPlayer)
        lua_pushobject<CElement>(luaVM, static_cast<CClient*>(object)->GetElement()); // Get the underlaying element, and call us
#endif
    else // Everything else should work with this. If not just add an std::is_same_v before this
        push(object->GetScriptID());
}

// Internal use
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

// Include the RPC functions enum
#include "net/rpc_enums.h"

enum
{
    AUDIO_FRONTEND,
    AUDIO_MISSION_PRELOAD,
    AUDIO_MISSION_PLAY
};

// Lua debug info for logging
enum
{
    DEBUG_INFO_NONE,
    DEBUG_INFO_FILE_AND_LINE,
    DEBUG_INFO_SHORT_SRC,
};

#define INVALID_LINE_NUMBER (-1)

struct SLuaDebugInfo
{
    SLuaDebugInfo() : iLine(INVALID_LINE_NUMBER), infoType(DEBUG_INFO_NONE) {}
    SLuaDebugInfo(const SString& strFile, int iLine, const SString& strShortSrc = "")
        : strFile(strFile), strShortSrc(strShortSrc), iLine(iLine), infoType(DEBUG_INFO_FILE_AND_LINE)
    {
    }
    SString strFile;
    SString strShortSrc;
    int     iLine;
    int     infoType;
};
