/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/LuaCommon.h
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

#include <lua/LuaBasic.h>

CLuaFunctionRef luaM_toref(lua_State* luaVM, int iArgument);

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

// Predeclarations of our classes
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
class CLuaTimer;
class CResource;
class CXMLNode;

class CMatrix;
class CVector4D;
class CVector;
class CVector2D;

class CClientEntity* lua_toelement(lua_State* luaVM, int iArgument);

// Lua push/pop macros for our datatypes
// For new code uew lua::Push instead
// TODO: Maybe replace these functions with a direct call to lua::Push
void lua_pushelement(lua_State* luaVM, CClientEntity* pEntity) { lua::Push(luaVM, pEntity); }
void lua_pushresource(lua_State* luaVM, CResource* pResource) { lua::Push(luaVM, pResource); }
void lua_pushtimer(lua_State* luaVM, CLuaTimer* pTimer) { lua::Push(luaVM, pTimer); }
void lua_pushxmlnode(lua_State* luaVM, CXMLNode* pNode) { lua::Push(luaVM, pNode); }
void lua_pushvector(lua_State* luaVM, const CVector4D& vector) { lua::Push(luaVM, vector); }
void lua_pushvector(lua_State* luaVM, const CVector& vector) { lua::Push(luaVM, vector); }
void lua_pushvector(lua_State* luaVM, const CVector2D& vector) { lua::Push(luaVM, vector); }
void lua_pushmatrix(lua_State* luaVM, const CMatrix& matrix) { lua::Push(luaVM, matrix); }

void lua_pushuserdata(lua_State* luaVM, void* pData);

// Internal use
void lua_initclasses(lua_State* luaVM);

void lua_newclass(lua_State* luaVM);
void lua_getclass(lua_State* luaVM, const char* szName);
void lua_registerclass(lua_State* luaVM, const char* szName, const char* szParent = NULL);
void lua_registerstaticclass(lua_State* luaVM, const char* szName);
void lua_classfunction(lua_State* luaVM, const char* szFunction, lua_CFunction fn);
void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* fn);
void lua_classvariable(lua_State* luaVM, const char* szVariable, lua_CFunction set, lua_CFunction get);
void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* set, const char* get);
void lua_classmetamethod(lua_State* luaVM, const char* szName, lua_CFunction fn);

const char* lua_makestring(lua_State* luaVM, int iArgument);

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
    SLuaDebugInfo(const SString& strFile, int iLine) : strFile(strFile), iLine(iLine), infoType(DEBUG_INFO_FILE_AND_LINE) {}
    SString strFile;
    SString strShortSrc;
    int     iLine;
    int     infoType;
};
