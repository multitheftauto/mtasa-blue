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


CLuaArgument::CLuaArgument ( const CLuaArgument& Argument, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables )
{
    // Initialize and call our = on the argument
    m_pTableData = NULL;
    CopyRecursive ( Argument, pKnownTables );
}


CLuaArgument::CLuaArgument ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    m_pTableData = NULL;
    ReadFromBitStream ( bitStream, pKnownTables );
}


CLuaArgument::CLuaArgument ( lua_State* luaVM, int iArgument, std::map < const void*, CLuaArguments* > * pKnownTables )
{
    // Read the argument out of the lua VM
    m_pTableData = NULL;
    Read ( luaVM, iArgument, pKnownTables );

    // Store debug data for later retrieval
    m_iLine = 0;
    m_strFilename = "";
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        const char * szFilename = GetFilenameFromPath ( debugInfo.source );
        if ( szFilename ) m_strFilename = szFilename;
        m_iLine = debugInfo.currentline;
    }
}


CLuaArgument::~CLuaArgument ( void )
{
    // Eventually destroy our table
    DeleteTableData ();
}

void CLuaArgument::CopyRecursive ( const CLuaArgument& Argument, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables )
{
    // Clear the string
    m_strString = "";

    // Destroy our old tabledata if neccessary
    DeleteTableData ();

    // Copy over line and filename too
    m_strFilename = Argument.m_strFilename;
    m_iLine = Argument.m_iLine;

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
            if ( pKnownTables && pKnownTables->find ( Argument.m_pTableData ) != pKnownTables->end () )
            {
                m_pTableData = pKnownTables->find ( Argument.m_pTableData )->second;
                m_bWeakTableRef = true;
            }
            else
            {
                m_pTableData = new CLuaArguments ( *Argument.m_pTableData, pKnownTables );
                m_bWeakTableRef = false;
            }
            break;
        }

        case LUA_TSTRING:
        {
            m_strString = Argument.m_strString;
            break;
        }

        default: break;
    }
}

const CLuaArgument& CLuaArgument::operator = ( const CLuaArgument& Argument )
{
    CopyRecursive ( Argument );

    // Return the given class allowing for chaining
    return Argument;
}

bool CLuaArgument::operator == ( const CLuaArgument& Argument )
{
    std::set < CLuaArguments* > knownTables;
    return CompareRecursive ( Argument, &knownTables );
}


bool CLuaArgument::operator != ( const CLuaArgument& Argument )
{
    std::set < CLuaArguments* > knownTables;
    return !CompareRecursive ( Argument, &knownTables );
}

bool CLuaArgument::CompareRecursive ( const CLuaArgument& Argument, std::set < CLuaArguments* > * pKnownTables )
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
                if ( pKnownTables->find ( m_pTableData ) == pKnownTables->end () )
                {
                    pKnownTables->insert ( m_pTableData );
                    if ( *iter != *iterCompare )
                        return false;
                }
            
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


void CLuaArgument::Read ( lua_State* luaVM, int iArgument, std::map < const void*, CLuaArguments* > * pKnownTables )
{
    // Store debug data for later retrieval
    m_iLine = 0;
    m_strFilename = "";
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        const char * szFilename = GetFilenameFromPath ( debugInfo.source );
        if ( szFilename ) m_strFilename = szFilename;
        m_iLine = debugInfo.currentline;
    }

    // Eventually delete our previous string
    m_strString = "";

    DeleteTableData ();

    // Grab the argument type
    m_iType = lua_type ( luaVM, iArgument );
    if ( m_iType != LUA_TNONE )
    {
        // Read out the content depending on the type
        switch ( m_iType )
        {
            case LUA_TNIL:
                break;

            case LUA_TBOOLEAN:
            {
                m_bBoolean = lua_toboolean ( luaVM, iArgument ) ? true : false;
                break;
            }

            case LUA_TTABLE:
            {
                const void* pTable = lua_topointer ( luaVM, iArgument );
                if ( pKnownTables && pKnownTables->find ( pTable ) != pKnownTables->end () )
                {
                    m_pTableData = pKnownTables->find ( pTable )->second;
                    m_bWeakTableRef = true;
                }
                else
                {
                    m_pTableData = new CLuaArguments ();
                    m_pTableData->ReadTable ( luaVM, iArgument, pKnownTables );
                    m_bWeakTableRef = false;
                }
                break;
            }

            case LUA_TLIGHTUSERDATA:
            {
                m_pLightUserData = lua_touserdata ( luaVM, iArgument );
                break;
            }

            case LUA_TNUMBER:
            {
                m_Number = lua_tonumber ( luaVM, iArgument );
                break;
            }

            case LUA_TSTRING:
            {
                // Grab the lua string and its size
                const char* szLuaString = lua_tostring ( luaVM, iArgument );
                size_t sizeLuaString = lua_strlen ( luaVM, iArgument );

                // Set our string
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
    DeleteTableData ();
    m_bBoolean = bBool;
}


void CLuaArgument::Read ( double dNumber )
{
    m_strString = "";
    m_iType = LUA_TNUMBER;
    DeleteTableData ();
    m_Number = dNumber;
}


void CLuaArgument::Read ( const char* szString )
{
    assert ( szString );

    m_iType = LUA_TSTRING;
    DeleteTableData ();
    m_strString = szString ? szString : "";
}


void CLuaArgument::Read ( void* pUserData )
{
    m_strString = "";
    DeleteTableData ();
    m_iType = LUA_TLIGHTUSERDATA;
    m_pLightUserData = pUserData;
}


void CLuaArgument::Read ( CClientEntity* pElement )
{
    m_strString = "";
    DeleteTableData ();
    if ( pElement )
    {   
        m_iType = LUA_TLIGHTUSERDATA;
        m_pLightUserData = (void*) pElement->GetID ();
    }
    else
        m_iType = LUA_TNIL;
}


void CLuaArgument::ReadElementID ( ElementID ID )
{
    m_strString = "";
    DeleteTableData ();
    m_iType = LUA_TLIGHTUSERDATA;
    m_pLightUserData = (void*) ID;
}


CClientEntity* CLuaArgument::GetElement ( void ) const
{
    ElementID ID = TO_ELEMENTID ( m_pLightUserData );
    return CElementIDs::GetElement ( ID );
}


void CLuaArgument::Push ( lua_State* luaVM, std::map < CLuaArguments*, int > * pKnownTables ) const
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

            case LUA_TTABLE:
            {
                if ( pKnownTables && pKnownTables->find ( m_pTableData ) != pKnownTables->end () )
                {
                    lua_pushvalue ( luaVM, pKnownTables->find ( m_pTableData )->second );
                }
                else
                {
                    m_pTableData->PushAsTable ( luaVM, pKnownTables );
                }
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


bool CLuaArgument::ReadFromBitStream ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    DeleteTableData ();

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
					Read((ucValue==1));
				break;
			}

            // Number type
			case LUA_TNUMBER:
			{
				float fNum;
				if ( bitStream.Read ( fNum ) )
				{
					Read ( ( double ) fNum );
				}
				break;
			}

            // Table type
            case LUA_TTABLE:
            {
                m_pTableData = new CLuaArguments ( bitStream, pKnownTables );
                m_bWeakTableRef = false;
                m_iType = LUA_TTABLE;
                break;
            }

            // Table reference (self-referencing tables)
            case LUA_TTABLEREF:
            {
                unsigned long ulTableRef;
                if ( bitStream.Read ( ulTableRef ) )
                {
                    if ( pKnownTables && ulTableRef < pKnownTables->size () )
                    {
                        m_pTableData = pKnownTables->at ( ulTableRef );
                        m_bWeakTableRef = true;
                        m_iType = LUA_TTABLE;
                    }
                }
                break;
            }

            // String type
			case LUA_TSTRING:
			{
                // Read out the string length
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

                    // Delete the buffer
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
		}
    }
    return true;
}


bool CLuaArgument::WriteToBitStream ( NetBitStreamInterface& bitStream, std::map < CLuaArguments*, unsigned long > * pKnownTables ) const
{
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

            // Write the boolean to it
            bitStream.Write ( static_cast < unsigned char > ( GetBoolean () ? 1 : 0 ) );
            break;
        }

        // Table argument
        case LUA_TTABLE:
        {
            if ( pKnownTables && pKnownTables->find ( m_pTableData ) != pKnownTables->end () )
            {
                // Self-referencing table
                bitStream.Write ( (unsigned char)LUA_TTABLEREF );
                bitStream.Write ( pKnownTables->find ( m_pTableData )->second );
            }
            else
            {
                bitStream.Write ( (unsigned char)LUA_TTABLE );

                // Write the subtable to the bitstream
                m_pTableData->WriteToBitStream ( bitStream, pKnownTables );
            }
            break;
        }

        // Number argument?
        case LUA_TNUMBER:
        {
            bitStream.Write ( (unsigned char)LUA_TNUMBER );
			bitStream.Write ( static_cast < float > ( GetNumber () ) );
            break;
        }

        // String argument
        case LUA_TSTRING:
        {           
            // Grab the string and its length. Is it short enough to be sendable?
            const char* szTemp = m_strString.c_str ();
            size_t sizeTemp = strlen ( szTemp );
            unsigned short usLength = static_cast < unsigned short > ( sizeTemp );
			if ( sizeTemp == usLength )
			{
                // This is a string argument
			    bitStream.Write ( ( unsigned char ) LUA_TSTRING );

                // Write its length
				bitStream.Write ( usLength );

                // Write the content too if it's not empty
                if ( usLength > 0 )
                {
				    bitStream.Write ( const_cast < char* > ( szTemp ), usLength );
                }
			}
			else
			{
                // Too long string
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

        // Unpacketizable type.
		default:
		{
            // Unpacketizable
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

void CLuaArgument::DeleteTableData ( )
{
    if ( m_pTableData )
    {
        if ( !m_bWeakTableRef )
            delete m_pTableData;
        m_pTableData = NULL;
    }
}