/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArgument.cpp
*  PURPOSE:     Lua argument handler class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

#ifndef VERIFY_ELEMENT
#define VERIFY_ELEMENT(element) (g_pGame->GetMapManager()->GetRootElement ()->IsMyChild(element,true)&&!element->IsBeingDeleted())
#endif

#ifndef VERIFY_RESOURCE
#define VERIFY_RESOURCE(resource) (g_pGame->GetResourceManager()->Exists(resource))
#endif

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

using namespace std;

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


CLuaArgument::CLuaArgument ( CElement* pElement )
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


void CLuaArgument::Read ( bool bBool )
{
    m_strString = "";
    DeleteTableData ();
    m_iType = LUA_TBOOLEAN;
    m_bBoolean = bBool;
}

void CLuaArgument::Read ( CLuaArguments * table )
{
    m_strString = "";
    DeleteTableData ();
    m_pTableData = new CLuaArguments ( *table );
    m_iType = LUA_TTABLE;
}

void CLuaArgument::Read ( double dNumber )
{
    m_strString = "";
    DeleteTableData ();
    m_iType = LUA_TNUMBER;
    m_Number = dNumber;
}


void CLuaArgument::Read ( const char* szString )
{
    assert ( szString );

    m_iType = LUA_TSTRING;
    DeleteTableData ();
    m_strString = szString;
}


void CLuaArgument::Read ( CElement* pElement )
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


void CLuaArgument::ReadUserData ( void* pUserData )
{
    m_strString = "";
    DeleteTableData ();
    m_iType = LUA_TLIGHTUSERDATA;
    m_pLightUserData = pUserData;
}


CElement* CLuaArgument::GetElement ( void ) const
{
    ElementID ID = TO_ELEMENTID ( m_pLightUserData );
    return CElementIDs::GetElement ( ID );
}


bool CLuaArgument::GetAsString ( char * szBuffer, unsigned int uiLength )
{
    assert ( szBuffer );
    assert ( uiLength );
    switch ( m_iType )
    {
        case LUA_TNIL:
            strncpy ( szBuffer, "nil", uiLength );
            break;
        case LUA_TBOOLEAN:
            if ( m_bBoolean ) strncpy ( szBuffer, "true", uiLength );
            else strncpy ( szBuffer, "false", uiLength );
            break;
        case LUA_TTABLE:
            return false;
            break;
        case LUA_TSTRING:
            strncpy ( szBuffer, m_strString.c_str (), uiLength );
            break;
        case LUA_TLIGHTUSERDATA:
            return false;
            break;
        case LUA_TNUMBER:
            _snprintf ( szBuffer, uiLength, "%d", ( int ) m_Number );
            break;
        default: return false; break;
    }
    return true;
}


bool CLuaArgument::ReadFromBitStream ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    DeleteTableData ();
    SLuaTypeSync type;

    // Read out the type
	if ( bitStream.Read ( &type ) )
	{
        // Depending on what type...
		switch ( type.data.ucType )
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
				bool bValue;
				if ( bitStream.ReadBit ( bValue ) )
					Read(bValue);
				break;
			}

            // Number type
			case LUA_TNUMBER:
			{
                bool bIsFloatingPoint;
                if ( bitStream.ReadBit ( bIsFloatingPoint ) && bIsFloatingPoint )
                {
                    float fNum;
                    if ( bitStream.Read ( fNum ) )
                        Read ( (double) fNum );
                }
                else
                {
                    long lNum;
                    if ( bitStream.ReadCompressed ( lNum ) )
                        Read ( (double) lNum );
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

            // Table reference
            case LUA_TTABLEREF:
            {
                unsigned long ulTableRef;
                if ( bitStream.ReadCompressed ( ulTableRef ) )
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
				if ( bitStream.ReadCompressed ( usLength ) && usLength )
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

            // Element type?
			case LUA_TLIGHTUSERDATA:
			{
				ElementID ElementID;
				if ( bitStream.ReadCompressed ( ElementID ) )
				{
					CElement * element = CElementIDs::GetElement ( ElementID );
					Read ( element );
				}
				break;
			}
		}
    }
    return true;
}


bool CLuaArgument::WriteToBitStream ( NetBitStreamInterface& bitStream, std::map < CLuaArguments*, unsigned long > * pKnownTables ) const
{
    SLuaTypeSync type;

    switch ( GetType () )
    {
        // Nil type
        case LUA_TNIL:
        {
            type.data.ucType = LUA_TNIL;
            bitStream.Write ( &type );
            break;
        }

        // Boolean type
        case LUA_TBOOLEAN:
        {
            type.data.ucType = LUA_TBOOLEAN;
			bitStream.Write ( &type );

            // Write the boolean to it
            bitStream.WriteBit ( GetBoolean () );
            break;
        }

        // Table argument
        case LUA_TTABLE:
        {
            if ( pKnownTables && pKnownTables->find ( m_pTableData ) != pKnownTables->end () )
            {
                // Self-referencing table
                type.data.ucType = LUA_TTABLEREF;
                bitStream.Write ( &type );
                bitStream.WriteCompressed ( pKnownTables->find ( m_pTableData )->second );
            }
            else
            {
                type.data.ucType = LUA_TTABLE;
                bitStream.Write ( &type );

                // Write the subtable to the bitstream
                m_pTableData->WriteToBitStream ( bitStream, pKnownTables );
            }
            break;
        }

        // Number argument?
        case LUA_TNUMBER:
        {
            type.data.ucType = LUA_TNUMBER;
            bitStream.Write ( &type );
            float fNumber = static_cast < float > ( GetNumber () );
            long lNumber = static_cast < long > ( fNumber );
            float fNumberInteger = static_cast < float > ( lNumber );

            // Check if the number is an integer and can fit a long datatype
            if ( fabs ( fNumber ) > fabs ( fNumberInteger + 1 ) ||
                 fabs ( fNumber - fNumberInteger ) >= FLOAT_EPSILON )
            {
                bitStream.WriteBit ( true );
                bitStream.Write ( fNumber );
            }
            else
            {
                bitStream.WriteBit ( false );
                bitStream.WriteCompressed ( lNumber );
            }
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
                type.data.ucType = LUA_TSTRING;
			    bitStream.Write ( &type );

                // Write its length
				bitStream.WriteCompressed ( usLength );

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
                type.data.ucType = LUA_TNIL;
                bitStream.Write ( &type );
                return false;
			}
            break;
        }

        // Element argument
        case LUA_TLIGHTUSERDATA:
        {
            // Grab the element from this userdata pointer. Valid and has a synced element ID?
			CElement* pElement = GetElement ();
			if ( pElement && pElement->GetID () != INVALID_ELEMENT_ID )
			{
                // Write its ID
                type.data.ucType = LUA_TLIGHTUSERDATA;
				bitStream.Write ( &type );
				bitStream.WriteCompressed ( static_cast < ElementID > ( pElement->GetID () ) );
			}
			else
			{
                // Jax: this just spams the script debugger, it's not really neccesary
                // LogUnableToPacketize ( "Couldn't packetize argument list, invalid element specified." );

                // Write a nil though so other side won't get out of sync
                type.data.ucType = LUA_TNIL;
                bitStream.Write ( &type );
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
            type.data.ucType = LUA_TNIL;
            bitStream.Write ( &type );
			return false;
		}
    }

    // Success
    return true;
}

json_object * CLuaArgument::WriteToJSONObject ( bool bSerialize, std::map < CLuaArguments*, unsigned long > * pKnownTables )
{
    switch ( GetType () )
    {
        case LUA_TNIL:
        {
            return json_object_new_int(0);
        }
        case LUA_TBOOLEAN:
        {
            return json_object_new_boolean(GetBoolean ());
        }
        case LUA_TTABLE:
        {
            if ( pKnownTables && pKnownTables->find ( m_pTableData ) != pKnownTables->end () )
            {
                char szTableID[10];
                _snprintf ( szTableID, sizeof(szTableID), "^T^%d", pKnownTables->find ( m_pTableData )->second );
                return json_object_new_string ( szTableID );
            }
            else
            {
                return m_pTableData->WriteTableToJSONObject ( bSerialize, pKnownTables );
            }
        }
        case LUA_TNUMBER:
        {
			float fNum = static_cast < float > ( GetNumber () );
			int iNum = static_cast < int > ( GetNumber () );
			if ( iNum == fNum )
			{
                return json_object_new_int(iNum);
			}
			else
			{
				return json_object_new_double(fNum);
			}
            break;
        }
        case LUA_TSTRING:
        {
            const char* szTemp = GetString ().c_str ();
            unsigned short usLength = static_cast < unsigned short > ( strlen ( szTemp ) );
			if ( strlen ( szTemp ) == usLength )
			{
				return json_object_new_string_len ( (char *)szTemp, usLength );
			}
			else
			{
				g_pGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert argument list to JSON. Invalid string specified, limit is 65535 characters." );
			}
            break;
        }
        case LUA_TLIGHTUSERDATA:
        {
			CElement* pElement = GetElement ();
            CResource* pResource = reinterpret_cast < CResource* > ( GetLightUserData() );
			
			// Elements are dynamic, so storing them is potentially unsafe
			if ( pElement && bSerialize )
			{
				char szElementID[10] = {0};
                _snprintf ( szElementID, 9, "^E^%d", pElement->GetID() );
				return json_object_new_string ( szElementID );
			}
            else if ( VERIFY_RESOURCE(pResource) )
            {
				char szElementID[MAX_RESOURCE_NAME_LENGTH+4] = {0};
                _snprintf ( szElementID, MAX_RESOURCE_NAME_LENGTH+3, "^R^%s", pResource->GetName().c_str () );
				return json_object_new_string ( szElementID );
            }
			else
			{
				g_pGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert argument list to JSON, only valid elements can be sent." );
				return NULL;
			}
            break;
        }
		default:
		{
			g_pGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert argument list to JSON, unsupported data type. Use Table, Nil, String, Number, Boolean, Resource or Element." );
			return NULL;
		}
    }
    return NULL;
}

char * CLuaArgument::WriteToString ( char * szBuffer, int length )
{
    switch ( GetType () )
    {
        case LUA_TNIL:
        {
            _snprintf ( szBuffer, length, "0" );
            return szBuffer;
        }
        case LUA_TBOOLEAN:
        {
            if ( GetBoolean () )
                _snprintf ( szBuffer, length, "true" );
            else
                _snprintf ( szBuffer, length, "false" );
            return szBuffer;
        }
        case LUA_TTABLE:
        {
            g_pGame->GetScriptDebugging()->LogError ( NULL, "Cannot convert table to string (do not use tables as keys in tables if you want to send them over http/JSON)." );
            return NULL;
        }
        case LUA_TNUMBER:
        {
			float fNum = static_cast < float > ( GetNumber () );
			int iNum = static_cast < int > ( GetNumber () );
			if ( iNum == fNum )
			{
                _snprintf ( szBuffer, length, "%d", iNum );
                return szBuffer;
			}
			else
			{
                _snprintf ( szBuffer, length, "%f", fNum );
                return szBuffer;
			}
            break;
        }
        case LUA_TSTRING:
        {
            const char* szTemp = GetString ().c_str ();
            unsigned short usLength = static_cast < unsigned short > ( strlen ( szTemp ) );
			if ( strlen ( szTemp ) == usLength )
			{
				_snprintf ( szBuffer, length, "%s", szTemp );
                return szBuffer;
			}
			else
			{
				g_pGame->GetScriptDebugging()->LogError ( NULL, "String is too long. Limit is 65535 characters." );
			}
            break;
        }
        case LUA_TLIGHTUSERDATA:
        {
			CElement* pElement = GetElement ();
            CResource* pResource = reinterpret_cast < CResource* > ( GetLightUserData() );
			if ( pElement )
			{
                _snprintf ( szBuffer, length, "#E#%d", pElement->GetID() );
				return szBuffer;
			}
			else if ( VERIFY_RESOURCE(pResource) )
			{
                _snprintf ( szBuffer, length, "#R#%d", pResource->GetName().c_str () );
				return szBuffer;
			}
			else
			{
				g_pGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert element to string, only valid elements can be sent." );
				return NULL;
			}
            break;
        }
		default:
		{
			g_pGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert argument to string, unsupported data type. Use String, Number, Boolean or Element." );
			return NULL;
		}
    }
    return NULL;
}

bool CLuaArgument::ReadFromJSONObject ( json_object* object, std::vector < CLuaArguments* > * pKnownTables )
{
    if ( !is_error(object) )
    {
        DeleteTableData ();

        if ( !object )
            m_iType = LUA_TNIL; 
        else
        {
            switch ( json_object_get_type ( object ) )
            {
            case json_type_null:
                m_iType = LUA_TNIL;
                break;
            case json_type_boolean:
                if ( json_object_get_boolean ( object ) == TRUE )
                    Read(true);
                else
                    Read(false);
                break;
            case json_type_double:
                Read(json_object_get_double ( object ));
                break;
            case json_type_int:
                Read((double)json_object_get_int ( object ));
                break;
            case json_type_object:
                m_pTableData = new CLuaArguments();
                m_pTableData->ReadFromJSONObject ( object, pKnownTables );
                m_bWeakTableRef = false;
                m_iType = LUA_TTABLE;
                break;
            case json_type_array:
                m_pTableData = new CLuaArguments();
                m_pTableData->ReadFromJSONArray ( object, pKnownTables );
                m_bWeakTableRef = false;
                m_iType = LUA_TTABLE;
                break;
            case json_type_string:
                {
                char * szString = json_object_get_string ( object );
                if ( strlen(szString) > 3 && szString[0] == '^' && szString[2] == '^' && szString[1] != '^' )
                {
                    switch ( szString[1] )
                    {
                        case 'E': // element
                        {
                            int id = atoi ( szString + 3 );
                            CElement * element = NULL;
                            if ( id != INT_MAX && id != INT_MIN && id != 0 )
                                element = CElementIDs::GetElement(id);
                            if ( element )
                            {
                                Read ( element );
                            }
                            else 
                            {
                                g_pGame->GetScriptDebugging()->LogError ( NULL, "Invalid element specified." );
                                m_iType = LUA_TNIL;
                            }
                            break;
                        }
                        case 'R': // resource
                        {
                            CResource * resource = g_pGame->GetResourceManager()->GetResource(szString+3);
                            if ( resource )
                            {
                                ReadUserData ((void *)resource);
                            }
                            else 
                            {
                                g_pGame->GetScriptDebugging()->LogError ( NULL, "Invalid resource specified." );
                                m_iType = LUA_TNIL;
                            }
                            break;
                        }
                        case 'T':   // Table reference
                        {
                            unsigned long ulTableID = static_cast < unsigned long > ( atol ( szString + 3 ) );
                            if ( pKnownTables && ulTableID >= 0 && ulTableID < pKnownTables->size () )
                            {
                                m_pTableData = pKnownTables->at ( ulTableID );
                                m_bWeakTableRef = true;
                                m_iType = LUA_TTABLE;
                            }
                            else
                            {
                                g_pGame->GetScriptDebugging()->LogError ( NULL, "Invalid table reference specified." );
                                m_iType = LUA_TNIL;
                            }
                            break;
                        }
                    }
                }
                else
                    Read(szString);
                break;
                }
            default:
                return false;
            }
        }
        return true;
    }
    return false;
}


void CLuaArgument::LogUnableToPacketize ( const char* szMessage ) const
{
    if ( m_strFilename.length () > 0 )
    {
        g_pGame->GetScriptDebugging ()->LogWarning ( NULL, "%s: %s - Line: %d\n", m_strFilename.c_str (), szMessage, m_iLine );
    }
    else
    {
        g_pGame->GetScriptDebugging ()->LogWarning ( NULL, "Unknown: %s\n", szMessage );
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