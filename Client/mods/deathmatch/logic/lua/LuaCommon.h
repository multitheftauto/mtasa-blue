/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/LuaCommon.h
 *  PURPOSE:     Lua common functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

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
class CClientIMG;
class CClientBuilding;
class CClientVehicle;
class CClientWater;
class CClientWeapon;
class CClientRadarArea;
class CClientPointLights;
class CClientVectorGraphic;
class CLuaTimer;
class CResource;
class CXMLNode;

// Lua push/pop macros for our datatypes

class CClientEntity* lua_toelement(lua_State* luaVM, int iArgument);

void lua_pushelement(lua_State* luaVM, CClientEntity* pElement);
void lua_pushresource(lua_State* luaVM, CResource* pElement);
void lua_pushtimer(lua_State* luaVM, CLuaTimer* pElement);
void lua_pushxmlnode(lua_State* luaVM, CXMLNode* pElement);
void lua_pushuserdata(lua_State* luaVM, void* pData);

void lua_pushobject(lua_State* luaVM, const char* szClass, void* pObject, bool bSkipCache = false);

void lua_pushvector(lua_State* luaVM, const CVector4D& vector);
void lua_pushvector(lua_State* luaVM, const CVector& vector);
void lua_pushvector(lua_State* luaVM, const CVector2D& vector);
void lua_pushmatrix(lua_State* luaVM, const CMatrix& matrix);

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

class CLuaMain&  lua_getownercluamain(lua_State* L);
class CResource& lua_getownerresource(lua_State* L);

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
