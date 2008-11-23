/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaArgument.cpp
*  PURPOSE:     Lua argument class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

#define ARGUMENT_TYPE_INT		9
#define ARGUMENT_TYPE_FLOAT		10

#ifndef VERIFY_ENTITY
#define VERIFY_ENTITY(entity) (CStaticFunctionDefinitions::GetRootElement()->IsMyChild(entity,true)&&!entity->IsBeingDeleted())
#endif

extern CClientGame* g_pClientGame;

using namespace std;

// Prevent the warning issued when doing unsigned short -> void*
#pragma warning(disable:4312)

CLuaArgument::CLuaArgument ( void )
{
    m_iType = LUA_TNIL;
    m_pTableData = NULL;
    m_pLightUserData = NULL;
}


CLuaArgument::CLuaArgument ( bool bBool )
{
    m_pTableData = NULL;
    Read ( bBool );
}


CLuaArgument::CLuaArgument ( double dNumber )
{
    m_pTableData = NULL;
    Read ( dNumber );
}


CLuaArgument::CLuaArgument ( const char* szString )
{
    m_pTableData = NULL;
    Read ( szString );
}


CLuaArgument::CLuaArgument ( void* pUserData )
{
    m_pTableData = NULL;
    Read ( pUserData );
}


CLuaArgument::CLuaArgument ( CClientEntity* pElement )
{
    m_pTableData = NULL;
    Read ( pElement );
}


CLuaArgument::CLuaArgument ( const CLuaArgument& Argument )
{
    // Initialize and call our = on the argument
    m_pTableData = NULL;
    operator= ( Argument );
}


CLuaArgument::CLuaArgument ( lua_State* luaVM, signed int uiArgument, unsigned int depth )
{
    // Read the argument out of the lua VM
    m_pTableData = NULL;
    Read ( luaVM, uiArgument, depth );

    // Store debug data for later retrieval
    m_iLine = 0;
    m_strFilename = "";
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        const char * szTemp = GetFilenameFromPath ( debugInfo.source );
        if ( szTemp )
            m_strFilename = szTemp;

        m_iLine = debugInfo.currentline;
    }
}


CLuaArgument::~CLuaArgument ( void )
{
    // Eventually destroy our tabledata
    if ( m_pTableData )
        delete m_pTableData;
}


const CLuaArgument& CLuaArgument::operator = ( const CLuaArgument& Argument )
{
    // Clear the string
    m_strString = "";

    // Destroy our old tabledata if neccessary
    if ( m_pTableData )
    {
        delete m_pTableData;
        m_pTableData = NULL;
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

        case LUA_TTABLE:
        {
            m_pTableData = new CLuaArguments(*Argument.m_pTableData);
            break;
        }

        case LUA_TSTRING:
        {
            m_strString = Argument.m_strString;
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

        case LUA_TTABLE:
        {
            if ( m_pTableData->Count () != Argument.m_pTableData->Count () )
                return false;

            vector < CLuaArgument * > ::const_iterator iter = m_pTableData->IterBegin ();
            vector < CLuaArgument * > ::const_iterator iterCompare = Argument.m_pTableData->IterBegin ();
            while ( iter != m_pTableData->IterEnd () && iterCompare != Argument.m_pTableData->IterEnd () )
            {
                if ( *iter != *iterCompare )
                    return false;
            
                iter++;
                iterCompare++;
            }
            return true;
        }

        case LUA_TSTRING:
        {
            return m_strString == Argument.m_strString;
        }
    }

    return true;
}


bool CLuaArgument::operator != ( const CLuaArgument& Argument )
{
    return !( operator == ( Argument ) );
}


void CLuaArgument::Read ( lua_State* luaVM, signed int uiArgument, unsigned int depth )
{    
    // Store debug data for later retrieval
    m_iLine = 0;
    m_strFilename = "";
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        const char * szTemp = GetFilenameFromPath ( debugInfo.source );
        if ( szTemp )
            m_strFilename = szTemp;

        m_iLine = debugInfo.currentline;
    }

    // Clear the string
    m_strString = "";

    // TODO: Fix memoryleak
    m_pTableData = NULL;

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

            case LUA_TTABLE:
            {
                m_pTableData = new CLuaArguments();
                if ( uiArgument < 0 )
                    uiArgument--;
                if ( depth < 12 ) 
                    m_pTableData->ReadTable ( luaVM, uiArgument, depth + 1 );
                else {
                    g_pClientGame->GetScriptDebugging()->LogWarning ( NULL, "Truncated table at depth 12. You may have a self-referencing table." );
                    m_iType = LUA_TNIL;
                }
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
                size_t sizeLuaString = lua_strlen ( luaVM, uiArgument );

                // Set the string
                m_strString.assign ( szLuaString, sizeLuaString );
                break;
            }

            case LUA_TFUNCTION:
            {
                // TODO: add function reading (has to work inside tables too)
                m_iType = LUA_TNIL;
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


void CLuaArgument::Read ( bool bBool )
{
    m_strString = "";
    m_iType = LUA_TBOOLEAN;
    m_pTableData = NULL;
    m_bBoolean = bBool;
}


void CLuaArgument::Read ( double dNumber )
{
    m_strString = "";
    m_iType = LUA_TNUMBER;
    m_pTableData = NULL;
    m_Number = dNumber;
}


void CLuaArgument::Read ( const char* szString )
{
    assert ( szString );

    m_iType = LUA_TSTRING;
    m_pTableData = NULL;
    m_strString = szString ? szString : "";
}


void CLuaArgument::Read ( void* pUserData )
{
    m_strString = "";
    m_pTableData = NULL;
    m_iType = LUA_TLIGHTUSERDATA;
    m_pLightUserData = pUserData;
}


void CLuaArgument::Read ( CClientEntity* pElement )
{
    if ( pElement )
    {
        m_strString = "";
        m_pTableData = NULL;
        m_iType = LUA_TLIGHTUSERDATA;
        m_pLightUserData = (void*) pElement->GetID ();
    }
    else
        m_iType = LUA_TNIL;
}


void CLuaArgument::ReadElementID ( ElementID ID )
{
    m_strString = "";
    m_pTableData = NULL;
    m_iType = LUA_TLIGHTUSERDATA;
    m_pLightUserData = (void*) ID;
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

            case LUA_TTABLE:
            {
                m_pTableData->PushAsTable ( luaVM );
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
                lua_pushlstring ( luaVM, m_strString.c_str (), m_strString.length () );
                break;
            }
        }
    }
}


CClientEntity* CLuaArgument::GetElement ( void ) const
{
    ElementID ID = TO_ELEMENTID ( m_pLightUserData );
    return CElementIDs::GetElement ( ID );
}


bool CLuaArgument::ReadFromBitStream ( NetBitStreamInterface& bitStream )
{
    // Read out the type
    unsigned char cType = 0;
	if ( bitStream.Read ( cType ) )
	{
        // Depending on what type...
		switch ( cType )
		{
            // Nil type
            case LUA_TNIL:
            {
                m_iType = LUA_TNIL;
                break;
            }

            // Boolean type
			case LUA_TBOOLEAN:
			{
				unsigned char ucValue;
				if ( bitStream.Read ( ucValue ) )
					Read ((ucValue==1));
				break;
			}

            // Table type
            case LUA_TTABLE:
            {
                m_pTableData = new CLuaArguments(bitStream);
                m_iType = LUA_TTABLE;
                break;
            }

            // String type
			case LUA_TSTRING:
			{
                // Read the string length
				unsigned short usLength;
				if ( bitStream.Read ( usLength ) && usLength )
				{
                    // Allocate a buffer and read the string into it
                    char* szValue = new char [ usLength + 1 ];
                    if ( bitStream.Read ( szValue, usLength ) )
                    {
                        // Put it into us
                        szValue [ usLength ] = 0;
						Read ( szValue );
                    }

                    // Delete the temp buffer
                    delete [] szValue;
				}
				else
					Read ( "" );

				break;
			}

            // Element type
			case LUA_TLIGHTUSERDATA:
			{
                // Read out the elemnt ID
				ElementID ElementID;
				if ( bitStream.Read ( ElementID ) )
				{
                    ReadElementID ( ElementID );
				}
				break;
			}

            // ??
			case ARGUMENT_TYPE_INT:
			{
				int iNum;
				if ( bitStream.Read ( iNum ) )
				{
					Read ( ( double ) iNum );
				}
				break;
			}

            // ??
			case ARGUMENT_TYPE_FLOAT:
			{
				float fNum;
				if ( bitStream.Read ( fNum ) )
				{
					Read ( fNum );
				}
				break;
			}
		}
	}
    return true;
}


bool CLuaArgument::WriteToBitStream ( NetBitStreamInterface& bitStream )
{
    // Depending on our type
    switch ( GetType () )
    {
        // Nil type
        case LUA_TNIL:
        {
            bitStream.Write ( (unsigned char)LUA_TNIL );
            break;
        }

        // Boolean type
        case LUA_TBOOLEAN:
        {
			bitStream.Write ( (unsigned char)LUA_TBOOLEAN );
            bitStream.Write ( static_cast < unsigned char > ( ( GetBoolean () ) ? 1 : 0 ) );
            break;
        }

        // Table type
        case LUA_TTABLE:
        {
            bitStream.Write ( (unsigned char)LUA_TTABLE );
            m_pTableData->WriteToBitStream(bitStream);
            break;
        }

        // Number type
        case LUA_TNUMBER:
        {
			float fNum = (float)GetNumber();
			int iNum = (int)GetNumber();
			if ( iNum == fNum )
			{
				// we can use the int, the number was a whole number
				bitStream.Write ( (unsigned char)ARGUMENT_TYPE_INT );
				bitStream.Write ( iNum );
			}
			else
			{
				bitStream.Write ( (unsigned char)ARGUMENT_TYPE_FLOAT );
				bitStream.Write ( fNum );
			}
            break;
        }

        // String type
        case LUA_TSTRING:
        {
            // This is a string
			bitStream.Write ( (unsigned char)LUA_TSTRING );

            // Grab the string and its length
            const char* szTemp = GetString ();
            size_t sizeTemp = strlen ( szTemp );

            // Is this string sendable?
            unsigned short usLength = static_cast < unsigned short > ( sizeTemp );
			if ( sizeTemp == usLength )
			{
                // Write its length
				bitStream.Write ( usLength );

                // Non-zero length?
                if ( usLength > 0 )
                {
				    bitStream.Write ( const_cast < char* > ( szTemp ), usLength );
                }
			}
			else
			{
                // Too long to packetize
				LogUnableToPacketize ( "Couldn't packetize argument list. Invalid string specified, limit is 65535 characters." );

                // Write a nil though so other side won't get out of sync
                bitStream.Write ( (unsigned char) LUA_TNIL );
                return false;
			}
            break;
        }

        // Element packet
        case LUA_TLIGHTUSERDATA:
        {
            // Got a valid element to send?
            CClientEntity* pElement = GetElement ();
			if ( pElement )
			{
                // Clientside element?
                if ( !pElement->IsLocalEntity () )
                {
				    bitStream.Write ( (unsigned char)LUA_TLIGHTUSERDATA );
				    bitStream.Write ( pElement->GetID () );
                }
                else
                {
                    // Write a nil though so other side won't get out of sync
                    bitStream.Write ( (unsigned char) LUA_TNIL );
                    return false;
                }
			}
			else
			{
                // Write a nil though so other side won't get out of sync
                bitStream.Write ( (unsigned char) LUA_TNIL );
				return false;
			}
            break;
        }

        // Unknown
		default:
		{
            // Unpacketizable.
			LogUnableToPacketize ( "Couldn't packetize argument list, unknown type specified." );

            // Write a nil though so other side won't get out of sync
            bitStream.Write ( (unsigned char) LUA_TNIL );
			return false;
		}
    }

    // Success
	return true;
}


void CLuaArgument::LogUnableToPacketize ( const char* szMessage ) const
{
    if ( m_strFilename.length () > 0 )
    {
        g_pClientGame->GetScriptDebugging ()->LogWarning ( NULL, "%s: %s - Line: %d\n", m_strFilename.c_str (), szMessage, m_iLine );
    }
    else
    {
        g_pClientGame->GetScriptDebugging ()->LogWarning ( NULL, "Unknown: %s\n", szMessage );
    }
}
