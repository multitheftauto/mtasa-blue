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

#include "StdInc.h"
#include "net/SyncStructures.h"

#define ARGUMENT_TYPE_INT       9
#define ARGUMENT_TYPE_FLOAT     10

#ifndef VERIFY_ENTITY
#define VERIFY_ENTITY(entity) (CStaticFunctionDefinitions::GetRootElement()->IsMyChild(entity,true)&&!entity->IsBeingDeleted())
#endif

#ifndef VERIFY_RESOURCE
#define VERIFY_RESOURCE(resource) (g_pClientGame->GetResourceManager()->Exists(resource))
#endif

extern CClientGame* g_pClientGame;

using namespace std;

// Prevent the warning issued when doing unsigned short -> void*
#pragma warning(disable:4312)



CLuaArgument::CLuaArgument ( void )
{
    m_iType = LUA_TNIL;
    m_pTableData = NULL;
    m_pUserData = NULL;
}


CLuaArgument::CLuaArgument ( const CLuaArgument& Argument, CFastHashMap < CLuaArguments*, CLuaArguments* > * pKnownTables )
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


CLuaArgument::CLuaArgument ( lua_State* luaVM, int iArgument, CFastHashMap < const void*, CLuaArguments* > * pKnownTables )
{
    // Read the argument out of the lua VM
    m_pTableData = NULL;
    Read ( luaVM, iArgument, pKnownTables );
}


CLuaArgument::~CLuaArgument ( void )
{
    // Eventually destroy our table
    DeleteTableData ();
}

void CLuaArgument::CopyRecursive ( const CLuaArgument& Argument, CFastHashMap < CLuaArguments*, CLuaArguments* > * pKnownTables )
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

        case LUA_TUSERDATA:
        {
            m_pUserData = Argument.m_pUserData;
            break;
        }

        case LUA_TNUMBER:
        {
            m_Number = Argument.m_Number;
            break;
        }

        case LUA_TTABLE:
        {
            if ( pKnownTables && ( m_pTableData = MapFindRef ( *pKnownTables, Argument.m_pTableData ) ) )
            {
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

        case LUA_TUSERDATA:
        {
            return m_pUserData == Argument.m_pUserData;
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


void CLuaArgument::Read ( lua_State* luaVM, int iArgument, CFastHashMap < const void*, CLuaArguments* > * pKnownTables )
{
    // Store debug data for later retrieval
    m_iLine = 0;
    m_strFilename = "";
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        m_strFilename = debugInfo.source;
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
                if ( pKnownTables && ( m_pTableData = MapFindRef ( *pKnownTables, lua_topointer ( luaVM, iArgument ) ) ) )
                {
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

            case LUA_TUSERDATA:
            {
                m_pUserData = lua_touserdata ( luaVM, iArgument );
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


void CLuaArgument::ReadBool ( bool bBool )
{
    m_strString = "";
    m_iType = LUA_TBOOLEAN;
    DeleteTableData ();
    m_bBoolean = bBool;
}


void CLuaArgument::ReadNumber ( double dNumber )
{
    m_strString = "";
    m_iType = LUA_TNUMBER;
    DeleteTableData ();
    m_Number = dNumber;
}


void CLuaArgument::ReadString ( const std::string& strString )
{
    m_iType = LUA_TSTRING;
    DeleteTableData ();
    m_strString = strString;
}


void CLuaArgument::ReadScriptID ( uint uiScriptID )
{
    m_strString = "";
    DeleteTableData ();
    m_iType = LUA_TUSERDATA;
    m_pUserData = reinterpret_cast < void* > ( uiScriptID );
}


void CLuaArgument::ReadElement ( CClientEntity* pElement )
{
    m_strString = "";
    DeleteTableData ();
    if ( pElement )
    {   
        m_iType = LUA_TUSERDATA;
        m_pUserData = (void*) reinterpret_cast<unsigned int *>(pElement->GetID ().Value());
    }
    else
        m_iType = LUA_TNIL;
}


void CLuaArgument::ReadElementID ( ElementID ID )
{
    m_strString = "";
    DeleteTableData ();
    m_iType = LUA_TUSERDATA;
    m_pUserData = (void*) reinterpret_cast<unsigned int *>(ID.Value());
}


CClientEntity* CLuaArgument::GetElement ( void ) const
{
    ElementID ID = TO_ELEMENTID ( m_pUserData );
    return CElementIDs::GetElement ( ID );
}


void CLuaArgument::Push ( lua_State* luaVM, CFastHashMap < CLuaArguments*, int > * pKnownTables ) const
{
    // Got any type?
    if ( m_iType != LUA_TNONE )
    {
        // Make sure the stack has enough room
        LUA_CHECKSTACK ( luaVM, 1 );

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

            case LUA_TUSERDATA:
            {
                lua_pushuserdata ( luaVM, "Element", m_pUserData );
                break;
            }

            case LUA_TNUMBER:
            {
                lua_pushnumber ( luaVM, m_Number );
                break;
            }

            case LUA_TTABLE:
            {
                int* pTableId;
                if ( pKnownTables && ( pTableId = MapFind ( *pKnownTables, m_pTableData ) ) )
                {
					lua_getfield ( luaVM, LUA_REGISTRYINDEX, "cache" );
					lua_pushnumber ( luaVM, *pTableId );
					lua_gettable ( luaVM, -2 );
					lua_remove ( luaVM, -2 );
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
                    ReadBool ( bValue );
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
                        ReadNumber ( fNum );
                }
                else
                {
                    long lNum;
                    if ( bitStream.ReadCompressed ( lNum ) )
                        ReadNumber ( lNum );
                }
                break;
            }

            // Table type
            case LUA_TTABLE:
            {
                m_pTableData = new CLuaArguments ( bitStream, pKnownTables );
                m_bWeakTableRef = false;
                m_iType = LUA_TTABLE;
                m_pTableData->ValidateTableKeys ();
                break;
            }

            // Table reference (self-referencing tables)
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
                        ReadString ( std::string ( szValue, usLength ) );
                    }

                    // Delete the buffer
                    delete [] szValue;
                }
                else
                    ReadString ( "" );

                break;
            }

            // Long string type
            case LUA_TSTRING_LONG:
            {
                // Read out the string length
                uint uiLength;
                if ( bitStream.ReadCompressed ( uiLength ) && uiLength )
                {
                    bitStream.AlignReadToByteBoundary ();

                    // Allocate a buffer and read the string into it
                    char* szValue = new char [ uiLength + 1 ];
                    assert ( szValue );
                    if ( bitStream.Read ( szValue, uiLength ) )
                    {
                        // Put it into us
                        ReadString ( std::string ( szValue, uiLength ) );
                    }

                    // Delete the buffer
                    delete [] szValue;
                }
                else
                    ReadString ( "" );

                break;
            }

            // Element type
            case LUA_TLIGHTUSERDATA:
            case LUA_TUSERDATA:
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


bool CLuaArgument::WriteToBitStream ( NetBitStreamInterface& bitStream, CFastHashMap < CLuaArguments*, unsigned long > * pKnownTables ) const
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
            ulong* pTableId;
            if ( pKnownTables && ( pTableId = MapFind ( *pKnownTables, m_pTableData ) ) )
            {
                // Self-referencing table
                type.data.ucType = LUA_TTABLEREF;
                bitStream.Write ( &type );
                bitStream.WriteCompressed ( *pTableId );
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
            size_t sizeTemp = m_strString.length ();
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
                    bitStream.Write ( szTemp, usLength );
                }
            }
            else
            {
                // This is a long string argument
                type.data.ucType = LUA_TSTRING_LONG;
                bitStream.Write ( &type );

                // Write its length
                uint uiLength = sizeTemp;
                bitStream.WriteCompressed ( uiLength );

                // Write the content too if it's not empty
                if ( uiLength > 0 )
                {
                    bitStream.AlignWriteToByteBoundary ();
                    bitStream.Write ( szTemp, uiLength );
                }
            }
            break;
        }

        // Element packet
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
        {
            // Got a valid element to send?
            CClientEntity* pElement = GetElement ();
            if ( pElement )
            {
                // Clientside element?
                if ( !pElement->IsLocalEntity () )
                {
                    type.data.ucType = LUA_TLIGHTUSERDATA;
                    bitStream.Write ( &type );
                    bitStream.Write ( pElement->GetID () );
                }
                else
                {
                    // Write a nil though so other side won't get out of sync
                    type.data.ucType = LUA_TNIL;
                    bitStream.Write ( &type );
                    return false;
                }
            }
            else
            {
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


void CLuaArgument::LogUnableToPacketize ( const char* szMessage ) const
{
    if ( m_strFilename.length () > 0 )
    {
        g_pClientGame->GetScriptDebugging ()->LogWarning ( NULL, "%s:%d: %s\n", ConformResourcePath ( m_strFilename.c_str () ).c_str (), m_iLine, szMessage );
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


json_object * CLuaArgument::WriteToJSONObject ( bool bSerialize, CFastHashMap < CLuaArguments*, unsigned long > * pKnownTables )
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
            ulong* pTableId;
            if ( pKnownTables && ( pTableId = MapFind ( *pKnownTables, m_pTableData ) ) )
            {
                // Self-referencing table
                char szTableID[10];
                snprintf ( szTableID, sizeof(szTableID), "^T^%lu", *pTableId );
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
            const char* szTemp = GetString ();
            unsigned short usLength = static_cast < unsigned short > ( strlen ( szTemp ) );
            if ( strlen ( szTemp ) == usLength )
            {
                return json_object_new_string_len ( (char *)szTemp, usLength );
            }
            else
            {
                g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert argument list to JSON. Invalid string specified, limit is 65535 characters." );
            }
            break;
        }
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
        {
            CClientEntity* pElement = GetElement ();
            CResource* pResource = g_pClientGame->GetResourceManager ()->GetResourceFromScriptID ( reinterpret_cast < unsigned long > ( GetUserData () ) );

            // Elements are dynamic, so storing them is potentially unsafe
            if ( pElement && bSerialize )
            {
                char szElementID[10] = {0};
                snprintf ( szElementID, 9, "^E^%d", (int)pElement->GetID().Value() );
                return json_object_new_string ( szElementID );
            }
            else if ( VERIFY_RESOURCE(pResource) )
            {
                char szElementID[MAX_RESOURCE_NAME_LENGTH+4] = {0};
                snprintf ( szElementID, MAX_RESOURCE_NAME_LENGTH+3, "^R^%s", pResource->GetName()/*.c_str ()*/ );
                return json_object_new_string ( szElementID );
            }
            else
            {
                if ( pElement )     // eg toJSON() with valid element
                    g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert userdata argument to JSON, elements not allowed for this function." );
                else
                if ( !bSerialize )  // eg toJSON() with invalid element
                    g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert userdata argument to JSON, only valid resources can be included for this function." );
                else
                    g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert userdata argument to JSON, only valid elements or resources can be included." );
                return NULL;
            }
            break;
        }
        default:
        {
            g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert argument list to JSON, unsupported data type. Use Table, Nil, String, Number, Boolean, Resource or Element." );
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
            snprintf ( szBuffer, length, "0" );
            return szBuffer;
        }
        case LUA_TBOOLEAN:
        {
            if ( GetBoolean () )
                snprintf ( szBuffer, length, "true" );
            else
                snprintf ( szBuffer, length, "false" );
            return szBuffer;
        }
        case LUA_TTABLE:
        {
            g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Cannot convert table to string (do not use tables as keys in tables if you want to send them over http/JSON)." );
            return NULL;
        }
        case LUA_TNUMBER:
        {
            float fNum = static_cast < float > ( GetNumber () );
            int iNum = static_cast < int > ( GetNumber () );
            if ( iNum == fNum )
            {
                snprintf ( szBuffer, length, "%d", iNum );
                return szBuffer;
            }
            else
            {
                snprintf ( szBuffer, length, "%f", fNum );
                return szBuffer;
            }
            break;
        }
        case LUA_TSTRING:
        {
            const char* szTemp = GetString ();
            unsigned short usLength = static_cast < unsigned short > ( strlen ( szTemp ) );
            if ( strlen ( szTemp ) == usLength )
            {
                snprintf ( szBuffer, length, "%s", szTemp );
                return szBuffer;
            }
            else
            {
                g_pClientGame->GetScriptDebugging()->LogError ( NULL, "String is too long. Limit is 65535 characters." );
            }
            break;
        }
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
        {
            CClientEntity* pElement = GetElement ();
            CResource* pResource = reinterpret_cast < CResource* > ( GetUserData() );
            if ( pElement )
            {
                snprintf ( szBuffer, length, "#E#%d", (int)pElement->GetID().Value() );
                return szBuffer;
            }
            else if ( VERIFY_RESOURCE(pResource) )
            {
                snprintf ( szBuffer, length, "#R#%s", pResource->GetName()/*.c_str ()*/ );
                return szBuffer;
            }
            else
            {
                g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert element to string, only valid elements can be sent." );
                return NULL;
            }
            break;
        }
        default:
        {
            g_pClientGame->GetScriptDebugging()->LogError ( NULL, "Couldn't convert argument to string, unsupported data type. Use String, Number, Boolean or Element." );
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
                    ReadBool ( true );
                else
                    ReadBool ( false );
                break;
            case json_type_double:
                ReadNumber ( json_object_get_double ( object ) );
                break;
            case json_type_int:
                ReadNumber ( json_object_get_int ( object ) );
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
                            CClientEntity * element = NULL;
                            if ( id != INT_MAX && id != INT_MIN && id != 0 )
                                element = CElementIDs::GetElement(id);
                            if ( element )
                            {
                                ReadElement ( element );
                            }
                            else 
                            {
                                // Appears sometimes when a player quits
                                //g_pClientGame->GetScriptDebugging()->LogError ( NULL, SString ( "Invalid element specified in JSON string '%s'.", szString ) );
                                m_iType = LUA_TNIL;
                            }
                            break;
                        }
                        case 'R': // resource
                        {
                            CResource * resource = g_pClientGame->GetResourceManager()->GetResource(szString+3);
                            if ( resource )
                            {
                                ReadScriptID ( resource->GetScriptID () );
                            }
                            else 
                            {
                                g_pClientGame->GetScriptDebugging()->LogError ( NULL, SString ( "Invalid resource specified in JSON string '%s'.", szString ) );
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
                                g_pClientGame->GetScriptDebugging()->LogError ( NULL, SString ( "Invalid table reference specified in JSON string '%s'.", szString ) );
                                m_iType = LUA_TNIL;
                            }
                            break;
                        }
                    }
                }
                else
                    ReadString ( szString );
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
