/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/LuaCommon.cpp
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

// Lua push/pop macros for our datatypes
CElement* lua_toelement ( lua_State* luaVM, int iArgument )
{
    if ( lua_type ( luaVM, iArgument ) == LUA_TLIGHTUSERDATA )
    {
        ElementID ID = TO_ELEMENTID ( lua_touserdata ( luaVM, iArgument ) );
        CElement* pElement = CElementIDs::GetElement ( ID );
        if ( !pElement || pElement->IsBeingDeleted () )
            return NULL;
        return pElement;
    }
    else if ( lua_type ( luaVM, iArgument ) == LUA_TUSERDATA )
    {
        ElementID ID = TO_ELEMENTID ( * ( ( void ** ) lua_touserdata ( luaVM, iArgument ) ) );
        CElement* pElement = CElementIDs::GetElement ( ID );
        if ( !pElement || pElement->IsBeingDeleted () )
            return NULL;
        return pElement;
    }

    return NULL;
}

void lua_pushelement ( lua_State* luaVM, CElement* pElement )
{
    if ( pElement )
    {
        ElementID ID = pElement->GetID ();
        if ( ID != INVALID_ELEMENT_ID )
        {
            const char* szClass = NULL;
            CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
            if ( pLuaMain->IsOOPEnabled () )
                szClass = CLuaClassDefs::GetElementClass ( pElement );

            lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( ID.Value () ) );
            return;
        }
    }

    lua_pushnil ( luaVM );
}


void lua_pushacl ( lua_State* luaVM, CAccessControlList* pACL )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetACLClass ( pACL );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pACL->GetScriptID () ) );
}


void lua_pushaclgroup ( lua_State* luaVM, CAccessControlListGroup* pGroup )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetACLGroupClass ( pGroup );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pGroup->GetScriptID () ) );
}


void lua_pushaccount ( lua_State* luaVM, CAccount* pAccount )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetAccountClass ( pAccount );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pAccount->GetScriptID () ) );
}


void lua_pushresource ( lua_State* luaVM, CResource* pResource )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetResourceClass ( pResource );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pResource->GetScriptID () ) );
}


void lua_pushtextdisplay ( lua_State* luaVM, CTextDisplay* pDisplay )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetTextDisplayClass ( pDisplay );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pDisplay->GetScriptID () ) );
}


void lua_pushtextitem ( lua_State* luaVM, CTextItem* pItem )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetTextItemClass ( pItem );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pItem->GetScriptID () ) );
}


void lua_pushtimer ( lua_State* luaVM, CLuaTimer* pTimer )
{    
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetTimerClass ( pTimer );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pTimer->GetScriptID () ) );
}


void lua_pushxmlnode ( lua_State* luaVM, CXMLNode* pElement )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetXmlNodeClass ( pElement );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pElement->GetID () ) );
}


void lua_pushban ( lua_State* luaVM, CBan* pBan )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetBanClass ( pBan );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pBan->GetScriptID () ) );
}


void lua_pushquery ( lua_State* luaVM, CDbJobData* pJobData )
{
    const char* szClass = NULL;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain->IsOOPEnabled () )
        szClass = CLuaClassDefs::GetQueryClass ( pJobData );

    lua_pushobject ( luaVM, szClass, ( void* ) reinterpret_cast < unsigned int * > ( pJobData->GetId() ) );
}

void lua_pushuserdata ( lua_State* luaVM, void* pData )
{
    if ( CElement* pEntity = UserDataCast < CElement > ( ( CElement* ) NULL, pData, NULL ) )
        return lua_pushelement ( luaVM, pEntity );
    if ( CPlayer* pEntity = UserDataCast < CPlayer > ( ( CPlayer* ) NULL, pData, NULL ) )
        return lua_pushelement ( luaVM, pEntity );
    else if ( CResource* pResource = UserDataCast < CResource > ( ( CResource* ) NULL, pData, NULL ) )
        return lua_pushresource ( luaVM, pResource );
    else if ( CXMLNode* pNode = UserDataCast < CXMLNode > ( ( CXMLNode* ) NULL, pData, NULL ) )
        return lua_pushxmlnode ( luaVM, pNode );
    else if ( CLuaTimer* pTimer = UserDataCast < CLuaTimer > ( ( CLuaTimer* ) NULL, pData, luaVM ) )
        return lua_pushtimer ( luaVM, pTimer );

    lua_pushobject ( luaVM, NULL, pData );
}


void lua_pushobject ( lua_State* luaVM, const char* szClass, void* pObject )
{
    if ( szClass )
    {
        lua_pushstring ( luaVM, "ud" );
        lua_rawget ( luaVM, LUA_REGISTRYINDEX );

        assert ( lua_istable ( luaVM, -1 ) );

        // First we want to check if we have a userdata for this already
        lua_pushlightuserdata ( luaVM, pObject );
        lua_rawget ( luaVM, -2 );

        if ( lua_isnil ( luaVM, -1 ) )
        {
            lua_pop ( luaVM, 1 );

            // we don't have it, create it
            * ( void ** ) lua_newuserdata ( luaVM, sizeof ( void * ) ) = pObject;

            // save in ud table
            lua_pushlightuserdata ( luaVM, pObject );
            lua_pushvalue ( luaVM, -2 );
            lua_rawset ( luaVM, -4 );
        }

        // userdata is already on the stack, just remove the table
        lua_remove ( luaVM, -2 );

        // Assign the class metatable
        lua_getclass ( luaVM, szClass );
        lua_setmetatable ( luaVM, -2 ); // element
        return;
    }
    lua_pushlightuserdata ( luaVM, pObject );
}

void lua_pushvector ( lua_State* luaVM, CVector& vector )
{
    CLuaVector3D* pVector = new CLuaVector3D ( vector );
    lua_pushobject ( luaVM, "Vector3", ( void* ) reinterpret_cast < unsigned int * > ( pVector->GetScriptID () ) );
}

void lua_pushmatrix ( lua_State* luaVM, CMatrix& matrix )
{
    CLuaMatrix* pMatrix = new CLuaMatrix ( matrix );
    lua_pushobject ( luaVM, "Matrix", ( void* ) reinterpret_cast < unsigned int * > ( pMatrix->GetScriptID () ) );
}

// Just do a type check vs LUA_TNONE before calling this, or bant
const char* lua_makestring ( lua_State* luaVM, int iArgument )
{
    if ( lua_type ( luaVM, iArgument ) == LUA_TSTRING )
    {
        return lua_tostring ( luaVM, iArgument );
    }
    lua_pushvalue ( luaVM, iArgument );
    lua_getglobal ( luaVM, "tostring" );
    lua_pushvalue ( luaVM, -2 );
    lua_call ( luaVM, 1, 1 );

    const char* szString = lua_tostring ( luaVM, -1 );
    lua_pop ( luaVM, 2 );

    return szString;
}

void lua_initclasses ( lua_State* luaVM )
{
    lua_newtable ( luaVM );
    lua_setfield ( luaVM, LUA_REGISTRYINDEX, "mt" );

    lua_newtable ( luaVM );
    lua_newtable ( luaVM );
    lua_pushstring ( luaVM, "v" );
    lua_setfield ( luaVM, -2, "__mode" );
    lua_setmetatable ( luaVM, -2 );
    lua_setfield ( luaVM, LUA_REGISTRYINDEX, "ud" );

    lua_getfield ( luaVM, LUA_REGISTRYINDEX, "mt" );
    lua_newtable ( luaVM );
    lua_pushcfunction ( luaVM, CLuaClassDefs::Call );
    lua_setfield ( luaVM, -2, "__call" );
    lua_setfield ( luaVM, -2, "Generic" );
    lua_pop ( luaVM, 1 );
}


void lua_newclass ( lua_State* luaVM )
{
    lua_newtable ( luaVM );

    lua_pushstring ( luaVM, "__class" );
    lua_newtable ( luaVM );
    lua_getclass ( luaVM, "Generic" );
    lua_setmetatable ( luaVM, -2 );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__get" );
    lua_newtable ( luaVM );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__set" );
    lua_newtable ( luaVM );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__index" );
    lua_pushvalue ( luaVM, -2 );
    lua_pushcclosure ( luaVM, CLuaClassDefs::Index, 1 );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "__newindex" );
    lua_pushvalue ( luaVM, -2 );
    lua_pushcclosure ( luaVM, CLuaClassDefs::NewIndex, 1 );
    lua_rawset ( luaVM, -3 );
}


void lua_getclass ( lua_State* luaVM, const char* szName )
{
    lua_pushstring ( luaVM, "mt" ); // "mt"
    lua_rawget ( luaVM, LUA_REGISTRYINDEX ); // mt

    assert ( lua_istable ( luaVM, -1 ) );

    lua_pushstring ( luaVM, szName ); // mt, class name
    lua_rawget ( luaVM, -2 ); // mt, class

    lua_remove ( luaVM, -2 ); // class
}


void lua_registerclass ( lua_State* luaVM, const char* szName, const char* szParent )
{
    if ( szParent != NULL )
    {
        lua_pushstring ( luaVM, "mt" ); // class table, "mt"
        lua_rawget ( luaVM, LUA_REGISTRYINDEX ); // class table, mt table
        lua_getfield ( luaVM, -1, szParent ); // class table, mt table, parent table

        assert ( lua_istable ( luaVM, -1 ) );

        lua_setfield ( luaVM, -3, "__parent" ); // class table, mt table

        lua_pop ( luaVM, 1 ); // class table
    }

    lua_pushstring ( luaVM, "mt" );
    lua_rawget ( luaVM, LUA_REGISTRYINDEX );

    // store in registry
    lua_pushvalue ( luaVM, -2 );
    lua_setfield ( luaVM, -2, szName );

    lua_pop ( luaVM, 1 );

    // register with environment
    lua_getfield ( luaVM, -1, "__class" );
    lua_setglobal ( luaVM, szName );

    lua_pop ( luaVM, 1 );
}

void lua_registerstaticclass ( lua_State* luaVM, const char* szName )
{
    lua_pushstring ( luaVM, "__newindex" );
    lua_pushvalue ( luaVM, -2 );
    lua_pushcclosure ( luaVM, CLuaClassDefs::StaticNewIndex, 1 );
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "mt" );
    lua_rawget ( luaVM, LUA_REGISTRYINDEX );

    // store in registry
    lua_pushvalue ( luaVM, -2 );
    lua_setfield ( luaVM, -2, szName );

    lua_pop ( luaVM, 1 );

    // register with environment
    lua_getfield ( luaVM, -1, "__class" );
    lua_setglobal ( luaVM, szName );

    lua_getfield ( luaVM, -1, "__class" );
    lua_pushvalue ( luaVM, -2 );
    lua_setmetatable ( luaVM, -2 );

    lua_pop ( luaVM, 2 );
}

void lua_classfunction ( lua_State* luaVM, const char* szFunction, lua_CFunction fn )
{
    if ( fn )
    {
        lua_pushstring ( luaVM, "__class" );
        lua_rawget ( luaVM, -2 );

        lua_pushstring ( luaVM, szFunction );
        lua_pushstring ( luaVM, szFunction );
        lua_pushcclosure ( luaVM, fn, 1 );
        lua_rawset ( luaVM, -3 );

        lua_pop ( luaVM, 1 );
    }
}

void lua_classfunction ( lua_State* luaVM, const char* szFunction, const char* fn )
{
    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( fn );
    if ( pFunction )
    {
        lua_classfunction ( luaVM, szFunction, pFunction->GetAddress () );
    }
}

void lua_classvariable ( lua_State* luaVM, const char* szVariable, lua_CFunction set, lua_CFunction get )
{
    lua_pushstring ( luaVM, "__set" );
    lua_rawget ( luaVM, -2 );

    if ( !set )
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, CLuaClassDefs::ReadOnly, 1 );
        lua_rawset ( luaVM, -3 );
    }
    else
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, set, 1 );
        lua_rawset ( luaVM, -3 );
    }
    lua_pop ( luaVM, 1 );

    // Get
    lua_pushstring ( luaVM, "__get" );
    lua_rawget ( luaVM, -2 );

    if ( !get )
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, CLuaClassDefs::WriteOnly, 1 );
        lua_rawset ( luaVM, -3 );
    }
    else
    {
        lua_pushstring ( luaVM, szVariable );
        lua_pushstring ( luaVM, szVariable );
        lua_pushcclosure ( luaVM, get, 1 );
        lua_rawset ( luaVM, -3 );
    }
    lua_pop ( luaVM, 1 );
}

void lua_classvariable ( lua_State* luaVM, const char* szVariable, const char* set, const char* get )
{
    lua_CFunction fnSet = NULL;
    lua_CFunction fnGet = NULL;

    if ( set )
        if ( CLuaCFunction* pSet = CLuaCFunctions::GetFunction ( set ) )
            fnSet = pSet->GetAddress();

    if ( get )
        if ( CLuaCFunction* pGet = CLuaCFunctions::GetFunction ( get ) )
            fnGet = pGet->GetAddress();

    if ( fnSet || fnGet )
        lua_classvariable ( luaVM, szVariable, fnSet, fnGet );
}

void lua_classmetamethod ( lua_State* luaVM, const char* szName, lua_CFunction fn )
{
    if ( fn )
    {
        lua_pushstring ( luaVM, szName );
        lua_pushstring ( luaVM, szName );
        lua_pushcclosure ( luaVM, fn, 1 );
        lua_rawset ( luaVM, -3 );
    }
}