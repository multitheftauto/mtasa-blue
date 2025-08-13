/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/LuaCommon.cpp
 *  PURPOSE:     Lua common functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

// Temporary until we change these funcs:
#include "../luadefs/CLuaDefs.h"
// End of temporary

// Prevent the warning issued when doing unsigned short -> void*
#pragma warning(disable:4312)

CClientEntity* lua_toelement(lua_State* luaVM, int iArgument)
{
    if (lua_type(luaVM, iArgument) == LUA_TLIGHTUSERDATA)
    {
        ElementID      ID = TO_ELEMENTID(lua_touserdata(luaVM, iArgument));
        CClientEntity* pEntity = CElementIDs::GetElement(ID);
        if (!pEntity || pEntity->IsBeingDeleted())
            return NULL;
        return pEntity;
    }
    else if (lua_type(luaVM, iArgument) == LUA_TUSERDATA)
    {
        ElementID      ID = TO_ELEMENTID(*((void**)lua_touserdata(luaVM, iArgument)));
        CClientEntity* pEntity = CElementIDs::GetElement(ID);
        if (!pEntity || pEntity->IsBeingDeleted())
            return NULL;
        return pEntity;
    }

    return NULL;
}

void lua_pushelement(lua_State* luaVM, CClientEntity* pElement)
{
    if (pElement)
    {
        if (pElement->IsBeingDeleted())
        {
            lua_pushboolean(luaVM, false);
            return;
        }

        ElementID ID = pElement->GetID();
        if (ID != INVALID_ELEMENT_ID)
        {
            const char* szClass = NULL;
            CLuaMain*   pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
            if (pLuaMain->IsOOPEnabled())
                szClass = CLuaClassDefs::GetEntityClass(pElement);

            lua_pushobject(luaVM, szClass, (void*)reinterpret_cast<unsigned int*>(ID.Value()));
            return;
        }
    }

    lua_pushnil(luaVM);
}

void lua_pushresource(lua_State* luaVM, CResource* pResource)
{
    const char* szClass = NULL;
    CLuaMain*   pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
    if (pLuaMain->IsOOPEnabled())
        szClass = CLuaClassDefs::GetResourceClass(pResource);

    lua_pushobject(luaVM, szClass, (void*)reinterpret_cast<unsigned int*>(pResource->GetScriptID()));
}

void lua_pushtimer(lua_State* luaVM, CLuaTimer* pTimer)
{
    const char* szClass = NULL;
    CLuaMain*   pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
    if (pLuaMain->IsOOPEnabled())
        szClass = CLuaClassDefs::GetTimerClass(pTimer);

    lua_pushobject(luaVM, szClass, (void*)reinterpret_cast<unsigned int*>(pTimer->GetScriptID()));
}

void lua_pushxmlnode(lua_State* luaVM, CXMLNode* pElement)
{
    const char* szClass = NULL;
    CLuaMain*   pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
    if (pLuaMain->IsOOPEnabled())
        szClass = CLuaClassDefs::GetXmlNodeClass(pElement);

    lua_pushobject(luaVM, szClass, (void*)reinterpret_cast<unsigned int*>(pElement->GetID()));
}

void lua_pushuserdata(lua_State* luaVM, void* pData)
{
    if (CClientEntity* pEntity = UserDataCast((CClientEntity*)pData, luaVM))
        return lua_pushelement(luaVM, pEntity);
    else if (CResource* pResource = UserDataCast((CResource*)pData, luaVM))
        return lua_pushresource(luaVM, pResource);
    else if (CXMLNode* pNode = UserDataCast((CXMLNode*)pData, luaVM))
        return lua_pushxmlnode(luaVM, pNode);
    else if (CLuaTimer* pTimer = UserDataCast((CLuaTimer*)pData, luaVM))
        return lua_pushtimer(luaVM, pTimer);
    else if (CLuaVector2D* pVector = UserDataCast((CLuaVector2D*)pData, luaVM))
        return lua_pushvector(luaVM, *pVector);
    else if (CLuaVector3D* pVector = UserDataCast((CLuaVector3D*)pData, luaVM))
        return lua_pushvector(luaVM, *pVector);
    else if (CLuaVector4D* pVector = UserDataCast((CLuaVector4D*)pData, luaVM))
        return lua_pushvector(luaVM, *pVector);
    else if (CLuaMatrix* pMatrix = UserDataCast((CLuaMatrix*)pData, luaVM))
        return lua_pushmatrix(luaVM, *pMatrix);

    lua_pushobject(luaVM, NULL, pData);
}

void lua_pushobject(lua_State* luaVM, const char* szClass, void* pObject, bool bSkipCache)
{
    if (szClass == nullptr)
    {
        lua_pushlightuserdata(luaVM, pObject);
        return;
    }

    if (bSkipCache)
    {
        *(void**)lua_newuserdata(luaVM, sizeof(void*)) = pObject;
    }
    else
    {
        // Lookup the userdata in the cache table
        lua_pushstring(luaVM, "ud");
        lua_rawget(luaVM, LUA_REGISTRYINDEX);

        assert(lua_istable(luaVM, -1));

        // First we want to check if we have a userdata for this already
        lua_pushlightuserdata(luaVM, pObject);
        lua_rawget(luaVM, -2);

        if (lua_isnil(luaVM, -1))
        {
            lua_pop(luaVM, 1);

            // we don't have it, create it
            *(void**)lua_newuserdata(luaVM, sizeof(void*)) = pObject;

            // save in ud table
            lua_pushlightuserdata(luaVM, pObject);
            lua_pushvalue(luaVM, -2);
            lua_rawset(luaVM, -4);
        }

        // userdata is already on the stack, just remove the table
        lua_remove(luaVM, -2);
    }

    // Assign the class metatable
    lua_getclass(luaVM, szClass);
    lua_setmetatable(luaVM, -2);            // element
}

void lua_pushvector(lua_State* luaVM, const CVector4D& vector)
{
    CLuaVector4D* pVector = new CLuaVector4D(vector);
    lua_pushobject(luaVM, "Vector4", (void*)reinterpret_cast<unsigned int*>(pVector->GetScriptID()), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
}

void lua_pushvector(lua_State* luaVM, const CVector& vector)
{
    CLuaVector3D* pVector = new CLuaVector3D(vector);
    lua_pushobject(luaVM, "Vector3", (void*)reinterpret_cast<unsigned int*>(pVector->GetScriptID()), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
}

void lua_pushvector(lua_State* luaVM, const CVector2D& vector)
{
    CLuaVector2D* pVector = new CLuaVector2D(vector);
    lua_pushobject(luaVM, "Vector2", (void*)reinterpret_cast<unsigned int*>(pVector->GetScriptID()), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
}

void lua_pushmatrix(lua_State* luaVM, const CMatrix& matrix)
{
    CLuaMatrix* pMatrix = new CLuaMatrix(matrix);
    lua_pushobject(luaVM, "Matrix", (void*)reinterpret_cast<unsigned int*>(pMatrix->GetScriptID()), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
}

CLuaMain& lua_getownercluamain(lua_State* L)
{
    return *static_cast<class CLuaMain*>(lua_getmtasaowner(L));
}

CResource& lua_getownerresource(lua_State* L)
{
    return *lua_getownercluamain(L).GetResource();
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

void lua_registerclass(lua_State* luaVM, const char* szName, const char* szParent)
{
    if (szParent != NULL)
    {
        lua_pushstring(luaVM, "mt");                     // class table, "mt"
        lua_rawget(luaVM, LUA_REGISTRYINDEX);            // class table, mt table
        lua_getfield(luaVM, -1, szParent);               // class table, mt table, parent table

        // Error if we can't find the parent class to extend from
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

    // register with environment
    lua_getfield(luaVM, -1, "__class");
    lua_setglobal(luaVM, szName);

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

void lua_classfunction(lua_State* luaVM, const char* szFunction, lua_CFunction fn)
{
    if (fn)
    {
        lua_pushstring(luaVM, "__class");
        lua_rawget(luaVM, -2);

        lua_pushstring(luaVM, szFunction);
        lua_pushstring(luaVM, szFunction);
        lua_pushcclosure(luaVM, fn, 1);
        lua_rawset(luaVM, -3);

        lua_pop(luaVM, 1);
    }
}

void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* fn)
{
    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction(fn);
    if (pFunction)
    {
        lua_classfunction(luaVM, szFunction, pFunction->GetFunctionAddress());
    }
    else
        dassert("lua_classfunction function does not exist" && 0);
}

void lua_classvariable(lua_State* luaVM, const char* szVariable, lua_CFunction set, lua_CFunction get)
{
    lua_pushstring(luaVM, "__set");
    lua_rawget(luaVM, -2);

    if (!set)
    {
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szVariable);
        lua_pushcclosure(luaVM, CLuaClassDefs::ReadOnly, 1);
        lua_rawset(luaVM, -3);
    }
    else
    {
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szVariable);
        lua_pushcclosure(luaVM, set, 1);
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
        lua_pushcclosure(luaVM, CLuaClassDefs::WriteOnly, 1);
        lua_rawset(luaVM, -3);
    }
    else
    {
        lua_pushstring(luaVM, szVariable);
        lua_pushstring(luaVM, szVariable);
        lua_pushcclosure(luaVM, get, 1);
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
            fnSet = pSet->GetFunctionAddress();
        else
            dassert("lua_classvariable set function does not exist" && 0);
    }

    if (get)
    {
        if (CLuaCFunction* pGet = CLuaCFunctions::GetFunction(get))
            fnGet = pGet->GetFunctionAddress();
        else
            dassert("lua_classvariable get function does not exist" && 0);
    }

    if (fnSet || fnGet)
        lua_classvariable(luaVM, szVariable, fnSet, fnGet);
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
