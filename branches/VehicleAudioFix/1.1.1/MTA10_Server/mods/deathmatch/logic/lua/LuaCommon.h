/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/LuaCommon.h
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __LUACOMMON_H
#define __LUACOMMON_H

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

CLuaFunctionRef         luaM_toref              ( lua_State *luaVM, int iArgument );

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

// Lua pop macros for our datatypes
class CElement*         lua_toelement           ( lua_State* luaVM, int iArgument );
class CAccount*         lua_toaccount           ( lua_State* luaVM, int iArgument );
class CAccessControlList*       lua_toacl       ( lua_State* luaVM, int iArgument );
class CAccessControlListGroup*  lua_toaclgroup  ( lua_State* luaVM, int iArgument );
class CBlip*            lua_toblip              ( lua_State* luaVM, int iArgument );
class CColShape*        lua_tocolshape          ( lua_State* luaVM, int iArgument );
class CScriptFile*      lua_tofile              ( lua_State* luaVM, int iArgument );
class CMarker*          lua_tomarker            ( lua_State* luaVM, int iArgument );
class CObject*          lua_toobject            ( lua_State* luaVM, int iArgument );
class CPed*             lua_toped               ( lua_State* luaVM, int iArgument );
class CPickup*          lua_topickup            ( lua_State* luaVM, int iArgument );
class CPlayer*          lua_toplayer            ( lua_State* luaVM, int iArgument );
class CResource*        lua_toresource          ( lua_State* luaVM, int iArgument );
class CRadarArea*       lua_toradararea         ( lua_State* luaVM, int iArgument );
class CTeam*            lua_toteam              ( lua_State* luaVM, int iArgument );
class CTextDisplay*     lua_totextdisplay       ( lua_State* luaVM, int iArgument );
class CTextItem*        lua_totextitem          ( lua_State* luaVM, int iArgument );
class CLuaTimer*        lua_totimer             ( lua_State* luaVM, int iArgument );
class CVehicle*         lua_tovehicle           ( lua_State* luaVM, int iArgument );
class CXMLNode*         lua_toxmlnode           ( lua_State* luaVM, int iArgument );
class CBan*             lua_toban               ( lua_State* luaVM, int iArgument );
class CWater*           lua_towater             ( lua_State* luaVM, int iArgument );


// Lua push macros for our datatypes
void                    lua_pushelement         ( lua_State* luaVM, class CElement* pElement );
void                    lua_pushacl             ( lua_State* luaVM, class CAccessControlList* pACL );
void                    lua_pushaclgroup        ( lua_State* luaVM, class CAccessControlListGroup* pACL );
void                    lua_pushaccount         ( lua_State* luaVM, class CAccount* pAccount );
void                    lua_pushresource        ( lua_State* luaVM, class CResource* pResource );
void                    lua_pushtextdisplay     ( lua_State* luaVM, class CTextDisplay* pDisplay );
void                    lua_pushtextitem        ( lua_State* luaVM, class CTextItem* pItem );
void                    lua_pushtimer           ( lua_State* luaVM, class CLuaTimer* pTimer );
void                    lua_pushxmlnode         ( lua_State* luaVM, class CXMLNode* pNode );
void                    lua_pushban             ( lua_State* luaVM, class CBan* pBan );
void                    lua_pushquery           ( lua_State* luaVM, class CDbJobData* pJobData );

// Include the RPC functions enum
#include "net/rpc_enums.h"

enum
{
    AUDIO_FRONTEND,
    AUDIO_MISSION_PRELOAD,
    AUDIO_MISSION_PLAY
};

#endif
