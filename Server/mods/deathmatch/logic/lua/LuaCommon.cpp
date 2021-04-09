/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/LuaCommon.cpp
 *  PURPOSE:     Lua common functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/LuaBasic.h>

extern CGame* g_pGame;

// Lua push/pop macros for our datatypes
CElement* lua_toelement(lua_State* luaVM, int iArgument)
{
    if (lua_type(luaVM, iArgument) == LUA_TLIGHTUSERDATA)
    {
        ElementID ID = TO_ELEMENTID(lua_touserdata(luaVM, iArgument));
        CElement* pElement = CElementIDs::GetElement(ID);
        if (!pElement || pElement->IsBeingDeleted())
            return NULL;
        return pElement;
    }
    else if (lua_type(luaVM, iArgument) == LUA_TUSERDATA)
    {
        ElementID ID = TO_ELEMENTID(*((void**)lua_touserdata(luaVM, iArgument)));
        CElement* pElement = CElementIDs::GetElement(ID);
        if (!pElement || pElement->IsBeingDeleted())
            return NULL;
        return pElement;
    }

    return NULL;
}

void lua_pushuserdata(lua_State* luaVM, void* pData)
{
    if (CElement* pEntity = UserDataCast<CElement>((CElement*)NULL, pData, NULL))
        return lua::Push(luaVM, pEntity);
    if (CPlayer* pEntity = UserDataCast<CPlayer>((CPlayer*)NULL, pData, NULL))
        return lua::Push(luaVM, pEntity);
    else if (CResource* pResource = UserDataCast<CResource>((CResource*)NULL, pData, NULL))
        return lua::Push(luaVM, pResource);
    else if (CXMLNode* pNode = UserDataCast<CXMLNode>((CXMLNode*)NULL, pData, NULL))
        return lua::Push(luaVM, pNode);
    else if (CLuaTimer* pTimer = UserDataCast<CLuaTimer>((CLuaTimer*)NULL, pData, luaVM))
        return lua::Push(luaVM, pTimer);
    else if (CLuaVector2D* pVector = UserDataCast<CLuaVector2D>((CLuaVector2D*)NULL, pData, luaVM))
        return lua::Push(luaVM, *pVector);
    else if (CLuaVector3D* pVector = UserDataCast<CLuaVector3D>((CLuaVector3D*)NULL, pData, luaVM))
        return lua::Push(luaVM, *pVector);
    else if (CLuaVector4D* pVector = UserDataCast<CLuaVector4D>((CLuaVector4D*)NULL, pData, luaVM))
        return lua::Push(luaVM, *pVector);
    else if (CLuaMatrix* pMatrix = UserDataCast<CLuaMatrix>((CLuaMatrix*)NULL, pData, luaVM))
        return lua::Push(luaVM, *pMatrix);
    else if (CAccount* pAccount = UserDataCast<CAccount>((CAccount*)NULL, pData, luaVM))
        return lua::Push(luaVM, pAccount);
    else if (CAccessControlList* pACL = UserDataCast<CAccessControlList>((CAccessControlList*)NULL, pData, luaVM))
        return lua::Push(luaVM, pACL);
    else if (CAccessControlListGroup* pACLGroup = UserDataCast<CAccessControlListGroup>((CAccessControlListGroup*)NULL, pData, luaVM))
        return lua::Push(luaVM, pACLGroup);
    else if (CBan* pBan = UserDataCast<CBan>((CBan*)NULL, pData, luaVM))
        return lua::Push(luaVM, pBan);
    else if (CTextDisplay* pTextDisplay = UserDataCast<CTextDisplay>((CTextDisplay*)NULL, pData, luaVM))
        return lua::Push(luaVM, pTextDisplay);
    else if (CTextItem* pTextItem = UserDataCast<CTextItem>((CTextItem*)NULL, pData, luaVM))
        return lua::Push(luaVM, pTextItem);
    else if (CDbJobData* pQuery = UserDataCast<CDbJobData>((CDbJobData*)NULL, pData, luaVM))
        return lua::Push(luaVM, pQuery);
    lua_pushlightuserdata(luaVM, pData);
}

CLuaMain& lua_getownercluamain(lua_State* L)
{
    return *static_cast<class CLuaMain*>(lua_getmtasaowner(L));
}

CResource& lua_getownerresource(lua_State* L)
{
    return *lua_getownercluamain(L).GetResource();
}

// Just do a type check vs LUA_TNONE before calling this, or bant
const char* lua_makestring(lua_State* luaVM, int iArgument)
{
    if (lua_type(luaVM, iArgument) == LUA_TSTRING)
    {
        return lua_tostring(luaVM, iArgument);
    }
    lua_pushvalue(luaVM, iArgument);
    lua_getglobal(luaVM, "tostring");
    lua_pushvalue(luaVM, -2);
    lua_call(luaVM, 1, 1);

    const char* szString = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 2);

    return szString;
}

void lua_initclasses(lua_State* luaVM)
{
    lua_newtable(luaVM);
    lua_setfield(luaVM, LUA_REGISTRYINDEX, "mt");

    lua_newtable(luaVM);
    lua_newtable(luaVM);
    lua_pushstring(luaVM, "v");
    lua_setfield(luaVM, -2, "__mode");
    lua_setmetatable(luaVM, -2);
    lua_setfield(luaVM, LUA_REGISTRYINDEX, "ud");

    lua_getfield(luaVM, LUA_REGISTRYINDEX, "mt");
    lua_newtable(luaVM);
    lua_pushcfunction(luaVM, CLuaClassDefs::Call);
    lua_setfield(luaVM, -2, "__call");
    lua_setfield(luaVM, -2, "Generic");
    lua_pop(luaVM, 1);
}

void lua_newclass(lua_State* luaVM)
{
    lua_newtable(luaVM);

    lua_pushstring(luaVM, "__class");
    lua_newtable(luaVM);
    lua_getclass(luaVM, "Generic");
    lua_setmetatable(luaVM, -2);
    lua_rawset(luaVM, -3);

    lua_pushstring(luaVM, "__get");
    lua_newtable(luaVM);
    lua_rawset(luaVM, -3);

    lua_pushstring(luaVM, "__set");
    lua_newtable(luaVM);
    lua_rawset(luaVM, -3);

    lua_pushstring(luaVM, "__index");
    lua_pushvalue(luaVM, -2);
    lua_pushcclosure(luaVM, CLuaClassDefs::Index, 1);
    lua_rawset(luaVM, -3);

    lua_pushstring(luaVM, "__newindex");
    lua_pushvalue(luaVM, -2);
    lua_pushcclosure(luaVM, CLuaClassDefs::NewIndex, 1);
    lua_rawset(luaVM, -3);
}

void lua_getclass(lua_State* luaVM, const char* szName)
{
    lua_pushstring(luaVM, "mt");                     // "mt"
    lua_rawget(luaVM, LUA_REGISTRYINDEX);            // mt

    assert(lua_istable(luaVM, -1));

    lua_pushstring(luaVM, szName);            // mt, class name
    lua_rawget(luaVM, -2);                    // mt, class

    lua_remove(luaVM, -2);            // class
}

void lua_registerclass(lua_State* luaVM, const char* szName, const char* szParent, bool bRegisterWithEnvironment)
{
    if (szParent != NULL)
    {
        lua_pushstring(luaVM, "mt");                     // class table, "mt"
        lua_rawget(luaVM, LUA_REGISTRYINDEX);            // class table, mt table
        lua_getfield(luaVM, -1, szParent);               // class table, mt table, parent table

        assert(lua_istable(luaVM, -1));

        lua_setfield(luaVM, -3, "__parent");            // class table, mt table

        lua_pop(luaVM, 1);            // class table
    }

    lua_pushstring(luaVM, "mt");
    lua_rawget(luaVM, LUA_REGISTRYINDEX);

    // store in registry
    lua_pushvalue(luaVM, -2);
    lua_setfield(luaVM, -2, szName);

    lua_pop(luaVM, 1);

    if (bRegisterWithEnvironment)
    {
        lua_getfield(luaVM, -1, "__class");
        lua_setglobal(luaVM, szName);
    }

    lua_pop(luaVM, 1);
}

void lua_registerstaticclass(lua_State* luaVM, const char* szName)
{
    lua_pushstring(luaVM, "__newindex");
    lua_pushvalue(luaVM, -2);
    lua_pushcclosure(luaVM, CLuaClassDefs::StaticNewIndex, 1);
    lua_rawset(luaVM, -3);

    lua_pushstring(luaVM, "mt");
    lua_rawget(luaVM, LUA_REGISTRYINDEX);

    // store in registry
    lua_pushvalue(luaVM, -2);
    lua_setfield(luaVM, -2, szName);

    lua_pop(luaVM, 1);

    // register with environment
    lua_getfield(luaVM, -1, "__class");
    lua_setglobal(luaVM, szName);

    lua_getfield(luaVM, -1, "__class");
    lua_pushvalue(luaVM, -2);
    lua_setmetatable(luaVM, -2);

    lua_pop(luaVM, 2);
}

void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* szACLName, lua_CFunction fn)
{
    if (fn)
    {
        lua_pushstring(luaVM, "__class");
        lua_rawget(luaVM, -2);

        lua_pushstring(luaVM, szFunction);
        lua_pushstring(luaVM, szFunction);
        lua_pushstring(luaVM, szACLName);
        lua_pushcclosure(luaVM, fn, 2);
        lua_rawset(luaVM, -3);

        lua_pop(luaVM, 1);
    }
}

void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* fn)
{
    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction(fn);
    if (pFunction)
    {
        lua_classfunction(luaVM, szFunction, szFunction, pFunction->GetAddress());
    }
    else
        dassert(false);
}

void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* szACLNameSet, const char* szACLNameGet, lua_CFunction set, lua_CFunction get,
                       bool bACLIgnore)
{
    lua_pushstring(luaVM, "__set");
    lua_rawget(luaVM, -2);

    if (!set)
    {
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szACLNameSet);
        lua_pushcclosure(luaVM, CLuaClassDefs::ReadOnly, 2);
        lua_rawset(luaVM, -3);
    }
    else
    {
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szACLNameSet);
        lua_pushcclosure(luaVM, set, 2);
        lua_rawset(luaVM, -3);
    }
    lua_pop(luaVM, 1);

    // Get
    lua_pushstring(luaVM, "__get");
    lua_rawget(luaVM, -2);

    if (!get)
    {
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szACLNameGet);
        lua_pushcclosure(luaVM, CLuaClassDefs::WriteOnly, 2);
        lua_rawset(luaVM, -3);
    }
    else
    {
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szACLNameGet);
        lua_pushcclosure(luaVM, get, 2);
        lua_rawset(luaVM, -3);
    }
    lua_pop(luaVM, 1);
}

void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* set, const char* get)
{
    lua_CFunction fnSet = NULL;
    lua_CFunction fnGet = NULL;

    if (set)
    {
        if (CLuaCFunction* pSet = CLuaCFunctions::GetFunction(set))
            fnSet = pSet->GetAddress();
        else
            dassert(false);
    }

    if (get)
    {
        if (CLuaCFunction* pGet = CLuaCFunctions::GetFunction(get))
            fnGet = pGet->GetAddress();
        else
            dassert(false);
    }
    if (fnSet || fnGet)
        lua_classvariable(luaVM, szVariable, "", "", fnSet, fnGet, false);
}

void lua_classmetamethod(lua_State* luaVM, const char* szName, lua_CFunction fn)
{
    if (fn)
    {
        lua_pushstring(luaVM, szName);
        lua_pushstring(luaVM, szName);
        lua_pushcclosure(luaVM, fn, 1);
        lua_rawset(luaVM, -3);
    }
}
