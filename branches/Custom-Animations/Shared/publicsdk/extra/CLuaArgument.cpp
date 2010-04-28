/*********************************************************
*
*  Multi Theft Auto: San Andreas - Deathmatch
*
*  ml_base, External lua add-on module
*  
*  Copyright © 2003-2008 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 2002-2003 Rockstar North
*
*  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
*  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
*  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
*  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
*  PROVIDED WITH THIS PACKAGE.
*
*********************************************************/

#include "CLuaArgument.h"
#include <assert.h>
#include <cstring>

using namespace std;

CLuaArgument::CLuaArgument ( void )
{
    m_szString = NULL;
    m_iType = LUA_TNIL;
}


CLuaArgument::CLuaArgument ( bool bBool )
{
    m_szString = NULL;
    m_iType = LUA_TBOOLEAN;
    m_bBoolean = bBool;
}


CLuaArgument::CLuaArgument ( double dNumber )
{
    m_szString = NULL;
    m_iType = LUA_TNUMBER;
    m_Number = dNumber;
}


CLuaArgument::CLuaArgument ( const char* szString )
{
    assert ( szString );

    m_iType = LUA_TSTRING;
    m_szString = new char [strlen ( szString ) + 1];
    strcpy ( m_szString, szString );
}


CLuaArgument::CLuaArgument ( void* pUserData )
{
    m_szString = NULL;
    m_iType = LUA_TLIGHTUSERDATA;
    m_pLightUserData = pUserData;
}


CLuaArgument::CLuaArgument ( const CLuaArgument& Argument )
{
    // Initialize and call our = on the argument
    m_szString = NULL;
    operator= ( Argument );
}


CLuaArgument::CLuaArgument ( lua_State* luaVM, unsigned int uiArgument )
{
    // Read the argument out of the lua VM
    m_szString = NULL;
    Read ( luaVM, uiArgument );
}


CLuaArgument::~CLuaArgument ( void )
{
    // Eventually destroy our string
    if ( m_szString )
    {
        delete [] m_szString;
    }
}


const CLuaArgument& CLuaArgument::operator = ( const CLuaArgument& Argument )
{
    // Destroy our old string if neccessary
    if ( m_szString )
    {
        delete [] m_szString;
        m_szString = NULL;
    }

    // Set our variable equally to the copy class
    m_iType = Argument.m_iType;
    switch ( m_iType )
    {
        case LUA_TBOOLEAN:
        {
            m_bBoolean = Argument.m_bBoolean;
            break;
        }

        case LUA_TLIGHTUSERDATA:
        {
            m_pLightUserData = Argument.m_pLightUserData;
            break;
        }

        case LUA_TNUMBER:
        {
            m_Number = Argument.m_Number;
            break;
        }

        case LUA_TSTRING:
        {
            if ( Argument.m_szString )
            {
                m_szString = new char [strlen ( Argument.m_szString ) + 1];
                strcpy ( m_szString, Argument.m_szString );
            }
           
            break;
        }

        default: break;
    }

    // Return the given class allowing for chaining
    return Argument;
}


bool CLuaArgument::operator == ( const CLuaArgument& Argument )
{
    // If the types differ, they're not matching
    if ( Argument.m_iType != m_iType )
        return false;

    // Compare the variables depending on the type
    switch ( m_iType )
    {
        case LUA_TBOOLEAN:
        {
            return m_bBoolean == Argument.m_bBoolean;
        }

        case LUA_TLIGHTUSERDATA:
        {
            return m_pLightUserData == Argument.m_pLightUserData;
        }

        case LUA_TNUMBER:
        {
            return m_Number == Argument.m_Number;
        }

        case LUA_TSTRING:
        {
            if ( m_szString )
            {
                if ( Argument.m_szString )
                    return strcmp ( m_szString, Argument.m_szString ) == 0;
                else
                    return false;
            }
            else
            {
                return Argument.m_szString == NULL;
            }
        }
    }

    return true;
}


bool CLuaArgument::operator != ( const CLuaArgument& Argument )
{
    return !( operator == ( Argument ) );
}


void CLuaArgument::Read ( lua_State* luaVM, unsigned int uiArgument )
{
    // Eventually delete our previous string
    if ( m_szString )
    {
        delete [] m_szString;
        m_szString = NULL;
    }

    // Grab the argument type
    m_iType = lua_type ( luaVM, uiArgument );
    if ( m_iType != LUA_TNONE )
    {
        // Read out the content depending on the type
        switch ( m_iType )
        {
            case LUA_TNIL:
                break;

            case LUA_TBOOLEAN:
            {
                m_bBoolean = lua_toboolean ( luaVM, uiArgument ) ? true:false;
                break;
            }

            case LUA_TLIGHTUSERDATA:
            {
                m_pLightUserData = lua_touserdata ( luaVM, uiArgument );
                break;
            }

            case LUA_TNUMBER:
            {
                m_Number = lua_tonumber ( luaVM, uiArgument );
                break;
            }

            case LUA_TSTRING:
            {
                // Grab the lua string and its size
                const char* szLuaString = lua_tostring ( luaVM, uiArgument );
                size_t sizeLuaString = strlen ( szLuaString );

                // Allocate our buffer
                m_szString = new char [sizeLuaString + 1];
                strcpy ( m_szString, szLuaString );
                break;
            }

            default:
            {
                m_iType = LUA_TNONE;
                break;
            }
        }
    }
}


void CLuaArgument::Push ( lua_State* luaVM ) const
{
    // Got any type?
    if ( m_iType != LUA_TNONE )
    {
        // Push it depending on the type
        switch ( m_iType )
        {
            case LUA_TNIL:
            {
                lua_pushnil ( luaVM );
                break;
            }

            case LUA_TBOOLEAN:
            {
                lua_pushboolean ( luaVM, m_bBoolean );
                break;
            }

            case LUA_TLIGHTUSERDATA:
            {
                lua_pushlightuserdata ( luaVM, m_pLightUserData );
                break;
            }

            case LUA_TNUMBER:
            {
                lua_pushnumber ( luaVM, m_Number );
                break;
            }

            case LUA_TSTRING:
            {
                // Push the string if we got any
                if ( m_szString )
                {
                    lua_pushstring ( luaVM, m_szString );
                }
                else
                {
                    lua_pushstring ( luaVM, "" );
                }

                break;
            }
        }
    }
}
