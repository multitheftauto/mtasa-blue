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


CAccessControlList* lua_toacl ( lua_State* luaVM, int iArgument )
{
    return g_pGame->GetACLManager ()->GetACLFromScriptID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
}


CAccessControlListGroup* lua_toaclgroup ( lua_State* luaVM, int iArgument )
{
    return g_pGame->GetACLManager ()->GetGroupFromScriptID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
}


CBlip* lua_toblip ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_BLIP ( pElement ) )
        return static_cast < CBlip* > ( pElement );
    else
        return NULL;
}


CColShape* lua_tocolshape ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_COLSHAPE ( pElement ) )
        return static_cast < CColShape* > ( pElement );
    else
        return NULL;
}


CScriptFile* lua_tofile ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_FILE ( pElement ) )
        return static_cast < CScriptFile* > ( pElement );
    else
        return NULL;
}


CMarker* lua_tomarker ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_MARKER ( pElement ) )
        return static_cast < CMarker* > ( pElement );
    else
        return NULL;
}


CObject* lua_toobject ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_OBJECT ( pElement ) )
        return static_cast < CObject* > ( pElement );
    else
        return NULL;
}


CPed* lua_toped ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_PED ( pElement ) )
        return static_cast < CPed* > ( pElement );
    else
        return NULL;
}


CPickup* lua_topickup ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_PICKUP ( pElement ) )
        return static_cast < CPickup* > ( pElement );
    else
        return NULL;
}


CPlayer* lua_toplayer ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_PLAYER ( pElement ) )
        return static_cast < CPlayer* > ( pElement );
    else
        return NULL;
}


CResource* lua_toresource ( lua_State* luaVM, int iArgument )
{
    return g_pGame->GetResourceManager ()->GetResourceFromScriptID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
}


CRadarArea* lua_toradararea ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_RADAR_AREA ( pElement ) )
        return static_cast < CRadarArea* > ( pElement );
    else
        return NULL;
}


CTeam* lua_toteam ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_TEAM ( pElement ) )
        return static_cast < CTeam* > ( pElement );
    else
        return NULL;
}


CTextDisplay* lua_totextdisplay ( lua_State* luaVM, int iArgument )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        return pLuaMain->GetTextDisplayFromScriptID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
    }

    return NULL;
}


CTextItem* lua_totextitem ( lua_State* luaVM, int iArgument )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        return pLuaMain->GetTextItemFromScriptID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
    }

    return NULL;
}


CVehicle* lua_tovehicle ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_VEHICLE ( pElement ) )
        return static_cast < CVehicle* > ( pElement );
    else
        return NULL;
}


CXMLNode* lua_toxmlnode ( lua_State* luaVM, int iArgument )
{
    return g_pServerInterface->GetXML ()->GetNodeFromID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
}


CBan* lua_toban ( lua_State* luaVM, int iArgument )
{
    return g_pGame->GetBanManager ()->GetBanFromScriptID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
}


CWater* lua_towater ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_WATER ( pElement ) )
        return static_cast < CWater* > ( pElement );
    else
        return NULL;
}


void lua_pushelement ( lua_State* luaVM, CElement* pElement )
{
    if ( pElement )
    {
        ElementID ID = pElement->GetID ();
        if ( ID != INVALID_ELEMENT_ID )
        {
#pragma message( "Assigned to: lil_Toady" )
    #if REMOVE_WHEN_FIXED
            lua_pushuserdata ( luaVM, "Element", (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
    #else
            lua_pushlightuserdata ( luaVM, (void*) reinterpret_cast<unsigned int *>(ID.Value()) );
    #endif
            return;
        }
    }

    lua_pushnil ( luaVM );
}


void lua_pushacl ( lua_State* luaVM, CAccessControlList* pACL )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pACL->GetScriptID () ) );
}


void lua_pushaclgroup ( lua_State* luaVM, CAccessControlListGroup* pGroup )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pGroup->GetScriptID () ) );
}


void lua_pushaccount ( lua_State* luaVM, CAccount* pAccount )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pAccount->GetScriptID () ) );
}


void lua_pushresource ( lua_State* luaVM, CResource* pResource )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pResource->GetScriptID () ) );
}


void lua_pushtextdisplay ( lua_State* luaVM, CTextDisplay* pDisplay )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pDisplay->GetScriptID () ) );
}


void lua_pushtextitem ( lua_State* luaVM, CTextItem* pItem )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pItem->GetScriptID () ) );
}


void lua_pushtimer ( lua_State* luaVM, CLuaTimer* pTimer )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pTimer->GetScriptID () ) );
}


void lua_pushxmlnode ( lua_State* luaVM, CXMLNode* pElement )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pElement->GetID () ) );
}


void lua_pushban ( lua_State* luaVM, CBan* pBan )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pBan->GetScriptID () ) );
}


void lua_pushquery ( lua_State* luaVM, CDbJobData* pJobData )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pJobData->GetId () ) );
}


void lua_pushuserdata ( lua_State* luaVM, const char* szClass, void* value )
{
    lua_pushstring ( luaVM, "ud" );
    lua_rawget ( luaVM, LUA_REGISTRYINDEX );

    // First we want to check if we have a userdata for this already
    lua_pushlightuserdata ( luaVM, value );
    lua_rawget ( luaVM, -2 );

    if ( lua_isnil ( luaVM, -1 ) )
    {
        lua_pop ( luaVM, 1 );

        // we don't have it, create it
        * ( void ** ) lua_newuserdata ( luaVM, sizeof ( void * ) ) = value;

        // Assign the class metatable
        {
            lua_pushstring ( luaVM, "mt" );
            lua_rawget ( luaVM, LUA_REGISTRYINDEX );
            
            lua_pushstring ( luaVM, szClass );
            lua_rawget ( luaVM, -2 );

            lua_remove ( luaVM, -2 );
            lua_setmetatable ( luaVM, -2 );
        }

        // save in ud table
        lua_pushlightuserdata ( luaVM, value );
        lua_pushvalue ( luaVM, -2 );
        lua_rawset ( luaVM, -4 );
    }

    // userdata is already on the stack, just remove the table
    lua_remove ( luaVM, -2 );
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


void lua_newclass ( lua_State* luaVM )
{
    lua_newtable ( luaVM );

    lua_pushstring ( luaVM, "__class" );
    lua_newtable ( luaVM );
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


void lua_registerclass ( lua_State* luaVM, const char* szName )
{
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

void lua_classfunction ( lua_State* luaVM, const char* szFunction, const char* szOriginal )
{
    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( szOriginal );
    if ( pFunction )
    {
        lua_pushstring ( luaVM, "__class" );
        lua_rawget ( luaVM, -2 );

        lua_pushstring ( luaVM, szFunction );
        lua_pushstring ( luaVM, szFunction );
        lua_pushcclosure ( luaVM, pFunction->GetAddress (), 1 );
        lua_rawset ( luaVM, -3 );

        lua_pop ( luaVM, 1 );
    }
}

void lua_classvariable ( lua_State* luaVM, const char* szVariable, const char* set, const char* get )
{
    // Set
    lua_pushstring ( luaVM, "__set" );
    lua_rawget ( luaVM, -2 );

    CLuaCFunction* pSet = NULL;
    CLuaCFunction* pGet = NULL;

    if ( ( !set ) || ! ( pSet = CLuaCFunctions::GetFunction ( set ) ) )
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
        lua_pushcclosure ( luaVM, pSet->GetAddress (), 1 );
        lua_rawset ( luaVM, -3 );
    }
    lua_pop ( luaVM, 1 );

    // Get
    lua_pushstring ( luaVM, "__get" );
    lua_rawget ( luaVM, -2 );

    if ( ( !get ) || ! ( pGet = CLuaCFunctions::GetFunction ( get ) ) )
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
        lua_pushcclosure ( luaVM, pGet->GetAddress (), 1 );
        lua_rawset ( luaVM, -3 );
    }
    lua_pop ( luaVM, 1 );
}