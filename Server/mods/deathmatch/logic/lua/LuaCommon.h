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

#include <lua/LuaBasic.h>

class CAccessControlList;
class CAccessControlListGroup;
class CAccount;
class CTextDisplay;
class CTextItem;
class CBan;
class CDbJobData;
class CElement;
class CResource;
class CLuaTimer;
class CXMLNode;
class CVector4D;
class CVector;
class CVector2D;
class CMatrix;

CLuaFunctionRef luaM_toref(lua_State* luaVM, int iArgument);

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

// Lua pop macros for our datatypes
class CElement* lua_toelement(lua_State* luaVM, int iArgument);

// Lua push macros for our datatypes
// For new code uew lua::Push instead
// TODO: Maybe replace these functions with a direct call to lua::Push
void lua_pushacl(lua_State* luaVM, CAccessControlList* pACL) { lua::Push(luaVM, pACL); }
void lua_pushaclgroup(lua_State* luaVM, CAccessControlListGroup* pGroup) { lua::Push(luaVM, pGroup); }
void lua_pushaccount(lua_State* luaVM, CAccount* pAccount) { lua::Push(luaVM, pAccount); }
void lua_pushtextdisplay(lua_State* luaVM, CTextDisplay* pDisplay) { lua::Push(luaVM, pDisplay); }
void lua_pushtextitem(lua_State* luaVM, CTextItem* pItem) { lua::Push(luaVM, pItem); }
void lua_pushban(lua_State* luaVM, CBan* pBan) { lua::Push(luaVM, pBan); }
void lua_pushquery(lua_State* luaVM, CDbJobData* pJobData) { lua::Push(luaVM, pJobData); }
void lua_pushelement(lua_State* luaVM, CElement* pElement) { lua::Push(luaVM, pElement); }
void lua_pushresource(lua_State* luaVM, CResource* pResource) { lua::Push(luaVM, pResource); }
void lua_pushtimer(lua_State* luaVM, CLuaTimer* pTimer) { lua::Push(luaVM, pTimer); }
void lua_pushxmlnode(lua_State* luaVM, CXMLNode* pNode) { lua::Push(luaVM, pNode); }
void lua_pushvector(lua_State* luaVM, const CVector4D& vector) { lua::Push(luaVM, vector); }
void lua_pushvector(lua_State* luaVM, const CVector& vector) { lua::Push(luaVM, vector); }
void lua_pushvector(lua_State* luaVM, const CVector2D& vector) { lua::Push(luaVM, vector); }
void lua_pushmatrix(lua_State* luaVM, const CMatrix& matrix) { lua::Push(luaVM, matrix); }

void lua_pushuserdata(lua_State* luaVM, void* value);

// Converts any type to string
const char* lua_makestring(lua_State* luaVM, int iArgument);

// Internal use
void lua_initclasses(lua_State* luaVM);

void lua_newclass(lua_State* luaVM);
void lua_getclass(lua_State* luaVM, const char* szName);
void lua_registerclass(lua_State* luaVM, const char* szName, const char* szParent = NULL, bool bRegisterWithEnvironment = true);
void lua_registerstaticclass(lua_State* luaVM, const char* szName);
void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* szACLName, lua_CFunction fn);
void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* fn);
void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* szACLNameSet, const char* szACLNameGet, lua_CFunction set, lua_CFunction get,
                       bool bACLIgnore = true);
void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* set, const char* get);
void lua_classmetamethod(lua_State* luaVM, const char* szName, lua_CFunction fn);

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
