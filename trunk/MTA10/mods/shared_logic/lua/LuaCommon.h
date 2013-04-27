/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/LuaCommon.h
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUACOMMON_H
#define __CLUACOMMON_H

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

CLuaFunctionRef         luaM_toref              ( lua_State *luaVM, int iArgument );

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
class CLuaTimer;
class CResource;
class CXMLNode;


// Lua push/pop macros for our datatypes

class CClientEntity*    lua_toelement           ( lua_State* luaVM, int iArgument );

void                    lua_pushelement         ( lua_State* luaVM, CClientEntity* pElement );
void                    lua_pushresource        ( lua_State* luaVM, CResource* pElement );
void                    lua_pushtimer           ( lua_State* luaVM, CLuaTimer* pElement );
void                    lua_pushxmlnode         ( lua_State* luaVM, CXMLNode* pElement );
void                    lua_pushuserdata        ( lua_State* luaVM, void* pData );

void                    lua_pushobject          ( lua_State* luaVM, const char* szClass, void* pObject );

// Internal use
void                    lua_initclasses         ( lua_State* luaVM );

void                    lua_newclass            ( lua_State* luaVM );
void                    lua_getclass            ( lua_State* luaVM, const char* szName );
void                    lua_registerclass       ( lua_State* luaVM, const char* szName, const char* szParent = NULL );
void                    lua_classfunction       ( lua_State* luaVM, const char* szFunction, lua_CFunction fn );
void                    lua_classfunction       ( lua_State* luaVM, const char* szFunction, const char* fn );
void                    lua_classvariable       ( lua_State* luaVM, const char* szVariable, lua_CFunction set, lua_CFunction get );
void                    lua_classvariable       ( lua_State* luaVM, const char* szVariable, const char* set, const char* get );

const char*             lua_makestring          ( lua_State* luaVM, int iArgument );

#endif
