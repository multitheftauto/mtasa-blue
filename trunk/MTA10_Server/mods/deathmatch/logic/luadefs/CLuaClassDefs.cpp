/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaClasses.cpp
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
int CLuaClassDefs::Index ( lua_State* luaVM )
{
    lua_pushvalue ( luaVM, lua_upvalueindex ( 1 ) ); // ud, k, mt

    // First we look for a function
    lua_pushstring ( luaVM, "__class" ); // ud, k, mt, "__class"
    lua_rawget ( luaVM, -2 ); // ud, k, mt, __class table

    if ( !lua_istable ( luaVM, -1 ) )
    {
        lua_pop ( luaVM, 1 ); // ud, k, mt
        goto searchparent;
    }

    lua_pushvalue ( luaVM, 2 ); // ud, k, mt, __class table, k
    lua_rawget ( luaVM, -2 ); // ud, k, mt, __class table, function
    lua_remove ( luaVM, -2 ); // ud, k, mt, function

    if ( lua_isfunction ( luaVM, -1 ) )
    { // Found the function, clean up and return
        lua_remove ( luaVM, -2 ); // ud, k, function
        return 1;
    }
    lua_pop ( luaVM, 1 ); // ud, k, mt

    // Function not found, look for property
    lua_pushstring ( luaVM, "__get" ); // ud, k, mt, "__get"
    lua_rawget ( luaVM, -2 ); // ud, k, mt, __get table

    if ( !lua_istable ( luaVM, -1 ) )
    {
        lua_pop ( luaVM, 1 ); // ud, k, mt
        goto searchparent;
    }

    lua_pushvalue ( luaVM, 2 ); // ud, k, mt, __get table, k
    lua_rawget ( luaVM, -2 ); // ud, k, mt, __get table, function
    lua_remove ( luaVM, -2 ); // ud, k, mt, function

    if ( lua_isfunction ( luaVM, -1 ) )
    { // Found the property,
        lua_remove ( luaVM, -2 ); // ud, k, function

        lua_pushvalue ( luaVM, 1 ); // ud, k, function, ud
        lua_call ( luaVM, 1, 1 ); // ud, k, result

        return 1;
    }
    lua_pop ( luaVM, 1 ); // ud, k, mt

searchparent:
    lua_pushstring ( luaVM, "__parent" ); // ud, k, mt, "__parent"
    lua_rawget ( luaVM, -2 ); // ud, k, mt, __parent table
    if ( lua_istable ( luaVM, -1 ) )
    {
        lua_pushstring ( luaVM, "__index" ); // ud, k, mt, __parent table, "__index"
        lua_rawget ( luaVM, -2 ); // ud, k, mt, __parent table, function
        if ( lua_isfunction ( luaVM, -1 ) )
        {
            lua_pushvalue ( luaVM, 1 ); // ud, k, mt, __parent table, function, ud
            lua_pushvalue ( luaVM, 2 ); // ud, k, mt, __parent table, function, ud, k

            lua_call ( luaVM, 2, 1 ); // ud, k, mt, __parent table, result

            lua_replace ( luaVM, -3 ); // ud, k, result, __parent table
            lua_pop ( luaVM, 1 ); // ud, k, result
            return 1;
        }
        lua_pop ( luaVM, 1 ); // ud, k, mt, __parent table
    }
    lua_pop ( luaVM, 2 ); // ud, k

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaClassDefs::NewIndex ( lua_State* luaVM )
{
    lua_pushvalue ( luaVM, lua_upvalueindex ( 1 ) ); // ud, k, v, mt

    lua_pushstring ( luaVM, "__set" ); // ud, k, v, mt, "__set"
    lua_rawget ( luaVM, -2 );  // ud, k, v, mt, __set table

    if ( !lua_istable ( luaVM, -1 ) )
    {
        lua_pop ( luaVM, 1 ); // ud, k, v, mt
        goto searchparent;
    }

    lua_pushvalue ( luaVM, 2 ); // ud, k, v, mt, __set table, k
    lua_rawget ( luaVM, -2 ); // ud, k, v, mt, __set table, function
    lua_remove ( luaVM, -2 ); // ud, k, v, mt, function

    if ( lua_isfunction ( luaVM, -1 ) )
    { // Found the property
        lua_pushvalue ( luaVM, 1 ); // ud, k, v, mt, function, ud
        lua_pushvalue ( luaVM, 3 ); // ud, k, v, mt, function, ud, v

        lua_call ( luaVM, 2, 0 ); // ud, k, v, mt

        lua_pop ( luaVM, 1 ); // ud, k, v

        return 0;
    }

    lua_pop ( luaVM, 1 ); // ud, k, v, mt

searchparent:
    lua_pushstring ( luaVM, "__parent" ); // ud, k, v, mt, "__parent"
    lua_rawget ( luaVM, -2 ); // ud, k, v, mt, __parent table
    if ( lua_istable ( luaVM, -1 ) )
    {
        lua_pushstring ( luaVM, "__newindex" ); // ud, k, v, mt, __parent table, "__newindex"
        lua_rawget ( luaVM, -2 ); // ud, k, v, mt, __parent table, function
        if ( lua_isfunction ( luaVM, -1 ) )
        {
            lua_pushvalue ( luaVM, 1 ); // ud, k, v, mt, __parent table, function, ud
            lua_pushvalue ( luaVM, 2 ); // ud, k, v, mt, __parent table, function, ud, k
            lua_pushvalue ( luaVM, 3 ); // ud, k, v, mt, __parent table, function, ud, k, v

            lua_call ( luaVM, 3, 0 );  // ud, k, v, mt, __parent table

            lua_pop ( luaVM, 2 ); // ud, k, v

            return 0;
        }
        lua_pop ( luaVM, 1 ); // ud, k, v, mt, __parent table
    }
    lua_pop ( luaVM, 2 ); // ud, k, v

    return 0;
}


int CLuaClassDefs::StaticNewIndex ( lua_State* luaVM )
{
    lua_pushvalue ( luaVM, lua_upvalueindex ( 1 ) ); // ud, k, v, mt

    lua_pushstring ( luaVM, "__set" ); // ud, k, v, mt, "__set"
    lua_rawget ( luaVM, -2 );  // ud, k, v, mt, __set table

    if ( !lua_istable ( luaVM, -1 ) )
    {
        lua_pop ( luaVM, 1 ); // ud, k, v, mt
        goto searchparent;
    }

    lua_pushvalue ( luaVM, 2 ); // ud, k, v, mt, __set table, k
    lua_rawget ( luaVM, -2 ); // ud, k, v, mt, __set table, function
    lua_remove ( luaVM, -2 ); // ud, k, v, mt, function

    if ( lua_isfunction ( luaVM, -1 ) )
    { // Found the property
        lua_pushvalue ( luaVM, 3 ); // ud, k, v, mt, function, v

        lua_call ( luaVM, 1, 0 ); // ud, k, v, mt

        lua_pop ( luaVM, 1 ); // ud, k, v

        return 0;
    }

    lua_pop ( luaVM, 1 ); // ud, k, v, mt

searchparent:
    lua_pushstring ( luaVM, "__parent" ); // ud, k, v, mt, "__parent"
    lua_rawget ( luaVM, -2 ); // ud, k, v, mt, __parent table
    if ( lua_istable ( luaVM, -1 ) )
    {
        lua_pushstring ( luaVM, "__newindex" ); // ud, k, v, mt, __parent table, "__newindex"
        lua_rawget ( luaVM, -2 ); // ud, k, v, mt, __parent table, function
        if ( lua_isfunction ( luaVM, -1 ) )
        {
            lua_pushvalue ( luaVM, 1 ); // ud, k, v, mt, __parent table, function, ud
            lua_pushvalue ( luaVM, 2 ); // ud, k, v, mt, __parent table, function, ud, k
            lua_pushvalue ( luaVM, 3 ); // ud, k, v, mt, __parent table, function, ud, k, v

            lua_call ( luaVM, 3, 0 );  // ud, k, v, mt, __parent table

            lua_pop ( luaVM, 2 ); // ud, k, v

            return 0;
        }
        lua_pop ( luaVM, 1 ); // ud, k, v, mt, __parent table
    }
    lua_pop ( luaVM, 2 ); // ud, k, v

    return 0;
}


int CLuaClassDefs::Call ( lua_State* luaVM )
{
    if ( !lua_istable ( luaVM, 1 ) )
        return 0;

    int stack = lua_gettop ( luaVM );

    lua_pushstring ( luaVM, "create" );
    lua_rawget ( luaVM, 1 );

    if ( lua_isfunction ( luaVM, -1 ) )
    {
        for ( int i = 2; i <= stack; i++ )
            lua_pushvalue ( luaVM, i );

        int args = stack - 1;

        lua_call ( luaVM, args, LUA_MULTRET );

        return lua_gettop ( luaVM ) - stack;
    }
    lua_pop ( luaVM, 1 );
    return 0;
}


int CLuaClassDefs::ReadOnly ( lua_State* luaVM )
{
    m_pScriptDebugging->LogWarning ( luaVM, "Property %s is read-only", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaClassDefs::WriteOnly ( lua_State* luaVM )
{
    m_pScriptDebugging->LogWarning ( luaVM, "Property %s is write-only", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

    return 0;
}

int CLuaClassDefs::ToString ( lua_State* luaVM )
{
    return 0;
}

const char* CLuaClassDefs::GetObjectClass ( void* pObject )
{
    if ( CElement* pElement = UserDataCast < CElement > ( ( CElement* ) NULL, pObject, NULL ) )
        return GetElementClass ( pElement );
    else if ( CResource* pResource = UserDataCast < CResource > ( ( CResource* ) NULL, pObject, NULL ) )
        return GetResourceClass ( pResource );
    else if ( CXMLNode* pNode = UserDataCast < CXMLNode > ( ( CXMLNode* ) NULL, pObject, NULL ) )
        return GetXmlNodeClass ( pNode );
    else if ( CLuaTimer* pTimer = UserDataCast < CLuaTimer > ( ( CLuaTimer* ) NULL, pObject, NULL ) )
        return GetTimerClass ( pTimer );
    return NULL;
}

const char* CLuaClassDefs::GetResourceClass ( CResource* pResource )
{
    return "Resource";
}

const char* CLuaClassDefs::GetTimerClass ( CLuaTimer* pTimer )
{
    return "Timer";
}

const char* CLuaClassDefs::GetXmlNodeClass ( CXMLNode* pXmlNode )
{
    return "XmlNode";
}

const char* CLuaClassDefs::GetACLClass ( CAccessControlList* pACL )
{
    return "ACL";
}

const char* CLuaClassDefs::GetACLGroupClass ( CAccessControlListGroup* pACLGroup )
{
    return "ACL-Group";
}

const char* CLuaClassDefs::GetAccountClass ( CAccount* pAccount )
{
    return "Account";
}

const char* CLuaClassDefs::GetTextDisplayClass ( CTextDisplay* pDisplay )
{
    return "Text-Display";
}


const char* CLuaClassDefs::GetTextItemClass ( CTextItem* pItem )
{
    return "Text-Item";
}

const char* CLuaClassDefs::GetBanClass ( CBan* pBan )
{
    return "Ban";
}

const char* CLuaClassDefs::GetQueryClass ( CDbJobData* pJobData )
{
    return "Query";
}

// absolutely ugly, need a better way
const char* CLuaClassDefs::GetElementClass ( CElement* pElement )
{
    assert ( pElement );
    switch ( pElement->GetType () )
    {
        case CElement::PLAYER: return "Player";
        case CElement::VEHICLE: return "Vehicle";
        case CElement::BLIP: return "Blip";
        case CElement::OBJECT: return "Object";
        case CElement::PICKUP: return "Pickup";
        case CElement::RADAR_AREA: return "RadarArea";
        case CElement::MARKER: return "Marker";
        case CElement::TEAM: return "Team";
        case CElement::PED: return "Ped";
        case CElement::COLSHAPE: return "ColShape";
        case CElement::SCRIPTFILE: return "File";
        case CElement::WATER: return "Water";
        case CElement::WEAPON: return "Weapon";
    }
    return "Element";
}