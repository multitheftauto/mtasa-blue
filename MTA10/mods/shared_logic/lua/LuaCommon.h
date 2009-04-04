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

	LUALIB_API int luaM_toref (lua_State *L, int i);
}

#define abs_index(L, i) \
	((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : \
	lua_gettop(L) + (i) + 1)
#define FREELIST_REF	0
#define RESERVED_REFS	2

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

// Predeclarations of our classes
class CClientColModel;
class CClientColShape;
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
class CClientRadarArea;
class CLuaTimer;
class CResource;
class CXMLNode;


#define VERIFY_FUNCTION(func) (func!=LUA_REFNIL)


// Lua push/pop macros for our datatypes
CClientRadarMarker*     lua_toblip          ( lua_State* luaVM, int iArgument );
CClientColModel*        lua_tocolmodel      ( lua_State* luaVM, int iArgument );
CClientColShape*        lua_tocolshape      ( lua_State* luaVM, int iArgument );
CClientDFF*             lua_todff           ( lua_State* luaVM, int iArgument );
CClientEntity*          lua_toelement       ( lua_State* luaVM, int iArgument );
CClientGUIElement*      lua_toguielement    ( lua_State* luaVM, int iArgument );
CClientMarker*          lua_tomarker        ( lua_State* luaVM, int iArgument );
CClientObject*          lua_toobject        ( lua_State* luaVM, int iArgument );
CClientPed*             lua_toped           ( lua_State* luaVM, int iArgument );
CClientPickup*          lua_topickup        ( lua_State* luaVM, int iArgument );
CClientPlayer*          lua_toplayer        ( lua_State* luaVM, int iArgument );
CLuaTimer*              lua_totimer         ( lua_State* luaVM, int iArgument );
CResource*              lua_toresource      ( lua_State* luaVM, int iArgument );
CClientSound*			lua_tosound         ( lua_State* luaVM, int iArgument );
CClientTeam*            lua_toteam          ( lua_State* luaVM, int iArgument );
CClientTXD*             lua_totxd           ( lua_State* luaVM, int iArgument );
CClientVehicle*         lua_tovehicle       ( lua_State* luaVM, int iArgument );
CClientWater*           lua_towater         ( lua_State* luaVM, int iArgument );
CXMLNode*               lua_toxmlnode       ( lua_State* luaVM, int iArgument );
CClientRadarArea*		lua_toradararea		( lua_State* luaVM, int iArgument );

void                    lua_pushelement     ( lua_State* luaVM, CClientEntity* pElement );
void                    lua_pushresource    ( lua_State* luaVM, CResource* pElement );
void                    lua_pushtimer       ( lua_State* luaVM, CLuaTimer* pElement );
void                    lua_pushxmlnode     ( lua_State* luaVM, CXMLNode* pElement );

#endif
