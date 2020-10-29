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

void lua_pushobject(lua_State* luaVM, const char* szClass, SArrayId id, bool bSkipCache)
{
    static_assert(sizeof(void*) >= sizeof(SArrayId));

    if (szClass == nullptr)
    {
        lua_pushlightuserdata(luaVM, (void*)id);
        return;
    }

    if (bSkipCache)
    {
        *(SArrayId*)lua_newuserdata(luaVM, sizeof(SArrayId)) = id;
    }
    else
    {
        // Lookup the userdata in the cache table
        lua_pushstring(luaVM, "ud");
        lua_rawget(luaVM, LUA_REGISTRYINDEX);

        assert(lua_istable(luaVM, -1));

        // First we want to check if we have a userdata for this already
        lua_pushlightuserdata(luaVM, (void*)id);
        lua_rawget(luaVM, -2);

        if (lua_isnil(luaVM, -1))
        {
            lua_pop(luaVM, 1);

            // we don't have it, create it
            *(SArrayId*)lua_newuserdata(luaVM, sizeof(SArrayId)) = id;

            // save in ud table
            lua_pushlightuserdata(luaVM, (void*)id);
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

void lua_pushuserdata(lua_State* luaVM, void* pData)
{
#ifdef MTA_CLIENT
    if (CClientEntity* pEntity = UserDataCast<CClientEntity>((CClientEntity*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pEntity);
    else if (CResource* pResource = UserDataCast<CResource>((CResource*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pResource);
    else if (CXMLNode* pNode = UserDataCast<CXMLNode>((CXMLNode*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pNode);
    else if (CLuaTimer* pTimer = UserDataCast<CLuaTimer>((CLuaTimer*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pTimer);
    else if (CLuaVector2D* pVector = UserDataCast<CLuaVector2D>((CLuaVector2D*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pVector);
    else if (CLuaVector3D* pVector = UserDataCast<CLuaVector3D>((CLuaVector3D*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pVector);
    else if (CLuaVector4D* pVector = UserDataCast<CLuaVector4D>((CLuaVector4D*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pVector);
    else if (CLuaMatrix* pMatrix = UserDataCast<CLuaMatrix>((CLuaMatrix*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pMatrix);
#else
    if (CElement* pEntity = UserDataCast<CElement>((CElement*)NULL, pData, NULL))
        return lua_pushobject(luaVM, pEntity);
    if (CPlayer* pEntity = UserDataCast<CPlayer>((CPlayer*)NULL, pData, NULL))
        return lua_pushobject(luaVM, pEntity);
    else if (CResource* pResource = UserDataCast<CResource>((CResource*)NULL, pData, NULL))
        return lua_pushobject(luaVM, pResource);
    else if (CXMLNode* pNode = UserDataCast<CXMLNode>((CXMLNode*)NULL, pData, NULL))
        return lua_pushobject(luaVM, pNode);
    else if (CLuaTimer* pTimer = UserDataCast<CLuaTimer>((CLuaTimer*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pTimer);
    else if (CLuaVector2D* pVector = UserDataCast<CLuaVector2D>((CLuaVector2D*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pVector);
    else if (CLuaVector3D* pVector = UserDataCast<CLuaVector3D>((CLuaVector3D*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pVector);
    else if (CLuaVector4D* pVector = UserDataCast<CLuaVector4D>((CLuaVector4D*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pVector);
    else if (CLuaMatrix* pMatrix = UserDataCast<CLuaMatrix>((CLuaMatrix*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, *pMatrix);
    else if (CAccount* pAccount = UserDataCast<CAccount>((CAccount*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pAccount);
    else if (CAccessControlList* pACL = UserDataCast<CAccessControlList>((CAccessControlList*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pACL);
    else if (CAccessControlListGroup* pACLGroup = UserDataCast<CAccessControlListGroup>((CAccessControlListGroup*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pACLGroup);
    else if (CBan* pBan = UserDataCast<CBan>((CBan*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pBan);
    else if (CTextDisplay* pTextDisplay = UserDataCast<CTextDisplay>((CTextDisplay*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pTextDisplay);
    else if (CTextItem* pTextItem = UserDataCast<CTextItem>((CTextItem*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pTextItem);
    else if (CDbJobData* pQuery = UserDataCast<CDbJobData>((CDbJobData*)NULL, pData, luaVM))
        return lua_pushobject(luaVM, pQuery);
#endif
    lua_pushobject(luaVM, NULL, (SArrayId)pData);
}

#ifdef MTA_CLIENT
void lua_pushobject(lua_State* luaVM, CClientEntity* element)
#else
void lua_pushobject(lua_State* luaVM, CElement* element)
#endif
{
    if (element->IsBeingDeleted())
        lua_pushboolean(luaVM, false);
    else if (const auto ID = element->GetID(); ID != INVALID_ELEMENT_ID)
        lua_pushobject(luaVM, GetClassNameIfOOPEnabled(luaVM, element), (SArrayId)ID.Value());
    else
        lua_pushnil(luaVM); // Invalid element ID
}

void lua_pushobject(lua_State* luaVM, CXMLNode* node)
{
    lua_pushobject(luaVM, GetClassNameIfOOPEnabled(luaVM, node), (SArrayId)node->GetID());
}

#ifndef MTA_CLIENT
void lua_pushobject(lua_State* luaVM, CDbJobData* jobdata)
{
    lua_pushobject(luaVM, GetClassNameIfOOPEnabled(luaVM, jobdata), (SArrayId)jobdata->GetId());
}
#endif

void lua_pushobject(lua_State* luaVM, const CVector4D& vector)
{
    CLuaVector4D* pVector = new CLuaVector4D(vector);
    lua_pushobject(luaVM, "Vector4", pVector->GetScriptID(), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
}

void lua_pushobject(lua_State* luaVM, const CVector& vector)
{
    CLuaVector3D* pVector = new CLuaVector3D(vector);
    lua_pushobject(luaVM, "Vector3", pVector->GetScriptID(), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
}

void lua_pushobject(lua_State* luaVM, const CVector2D& vector)
{
    CLuaVector2D* pVector = new CLuaVector2D(vector);
    lua_pushobject(luaVM, "Vector2", pVector->GetScriptID(), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
}

void lua_pushobject(lua_State* luaVM, const CMatrix& matrix)
{
    CLuaMatrix* pMatrix = new CLuaMatrix(matrix);
    lua_pushobject(luaVM, "Matrix", pMatrix->GetScriptID(), true);
    lua_addtotalbytes(luaVM, LUA_GC_EXTRA_BYTES);
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

#ifdef MTA_CLIENT
void lua_classfunction(lua_State* luaVM, const char* szFunction, lua_CFunction fn)
#else
void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* szACLName, lua_CFunction fn)
#endif
{
    if (!fn)
        return;
    
    lua_pushstring(luaVM, "__class");
    lua_rawget(luaVM, -2);

    lua_pushstring(luaVM, szFunction);
    lua_pushstring(luaVM, szFunction);
#ifdef MTA_CLIENT
    lua_pushcclosure(luaVM, fn, 1);
#else
    lua_pushstring(luaVM, szACLName);
    lua_pushcclosure(luaVM, fn, 2);
#endif
    lua_rawset(luaVM, -3);

    lua_pop(luaVM, 1);
    
}

void lua_classfunction(lua_State* luaVM, const char* szFunction, const char* fn)
{
    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction(fn);
    dassert(pFunction);
#ifdef MTA_CLIENT
    lua_classfunction(luaVM, szFunction, pFunction->GetFunctionAddress());
#else
    lua_classfunction(luaVM, szFunction, szFunction, pFunction->GetAddress());
#endif
}

#ifdef MTA_CLIENT
void lua_classvariable(lua_State* luaVM, const char* szVariable, lua_CFunction set, lua_CFunction get)
#else
void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* szACLNameSet, const char* szACLNameGet, lua_CFunction set, lua_CFunction get, bool bACLIgnore)
#endif
{
    const auto setVariable = [luaVM, szVariable](const char* funcType, const char* szACLName, const auto& passedInFunction, const auto& alternativeFunction) {
        lua_pushstring(luaVM, funcType);
        lua_rawget(luaVM, -2);

        [luaVM, szVariable, szACLName](const auto& function) {
            lua_pushstring(luaVM, szVariable);
            lua_pushstring(luaVM, szVariable);
        #ifdef MTA_CLIENT // Client doesnt need ACL name
            lua_pushcclosure(luaVM, function, 1);
        #else
            lua_pushstring(luaVM, szACLName);
            lua_pushcclosure(luaVM, function, 2);
        #endif
            lua_rawset(luaVM, -3);
        }((passedInFunction) ? passedInFunction : alternativeFunction);
        
        lua_pop(luaVM, 1);
    };
#ifdef MTA_CLIENT
    setVariable("__set", nullptr, set, CLuaClassDefs::ReadOnly);
    setVariable("__get", nullptr, get, CLuaClassDefs::WriteOnly);
#else
    setVariable("__set", szACLNameSet, set, CLuaClassDefs::ReadOnly);
    setVariable("__get", szACLNameGet, get, CLuaClassDefs::WriteOnly);
#endif
}

void lua_classvariable(lua_State* luaVM, const char* szVariable, const char* set, const char* get)
{
    const auto getAddress = [](const char* szFnName) -> lua_CFunction {
        if (!szFnName)
            return nullptr;
        if (CLuaCFunction* pFn = CLuaCFunctions::GetFunction(szFnName))
#ifdef MTA_CLIENT
            return pFn->GetFunctionAddress();
#else
            return pFn->GetAddress();
#endif
        dassert("lua_classvariable function does not exist" && 0);
        return nullptr;
    };

    if (lua_CFunction pFnSet = getAddress(set), pFnGet = getAddress(get); pFnGet || pFnSet)
#ifdef MTA_CLIENT
        lua_classvariable(luaVM, szVariable, pFnSet, pFnGet);
#else
        lua_classvariable(luaVM, szVariable, "", "", pFnSet, pFnGet, false);

#endif
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
