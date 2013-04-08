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

void                    lua_pushelement     ( lua_State* luaVM, CClientEntity* pElement );
void                    lua_pushresource    ( lua_State* luaVM, CResource* pElement );
void                    lua_pushtimer       ( lua_State* luaVM, CLuaTimer* pElement );
void                    lua_pushxmlnode     ( lua_State* luaVM, CXMLNode* pElement );

const char*             lua_makestring      ( lua_State* luaVM, int iArgument );

#endif
