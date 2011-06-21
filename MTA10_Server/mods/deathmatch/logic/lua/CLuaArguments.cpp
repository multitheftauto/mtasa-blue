/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArguments.cpp
*  PURPOSE:     Lua argument manager class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Chris McArthur <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#ifndef WIN32
#include <clocale>
#endif

extern CGame* g_pGame;

#ifndef VERIFY_ELEMENT
#define VERIFY_ELEMENT(element) (g_pGame->GetMapManager()->GetRootElement ()->IsMyChild(element,true)&&!element->IsBeingDeleted())
#endif

CLuaArguments::CLuaArguments ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    ReadFromBitStream ( bitStream, pKnownTables );
}


CLuaArguments::CLuaArguments ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables )
{
    // Copy all the arguments
    CopyRecursive ( Arguments, pKnownTables );
}


CLuaArgument* CLuaArguments::operator [] ( const unsigned int uiPosition ) const
{
    if ( uiPosition < m_Arguments.size () )
        return m_Arguments.at ( uiPosition );
    return NULL;
}


const CLuaArguments& CLuaArguments::operator = ( const CLuaArguments& Arguments )
{
    CopyRecursive ( Arguments );

    // Return the given reference allowing for chaining
    return Arguments;
}


void CLuaArguments::CopyRecursive ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables )
{
    // Clear our previous list if any
    DeleteArguments ();

    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < CLuaArguments*, CLuaArguments* > ();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert ( std::make_pair ( (CLuaArguments *)&Arguments, (CLuaArguments *)this ) );

    // Copy all the arguments
    vector < CLuaArgument* > ::const_iterator iter = Arguments.m_Arguments.begin ();
    for ( ; iter != Arguments.m_Arguments.end (); iter++ )
    {
        CLuaArgument* pArgument = new CLuaArgument ( **iter, pKnownTables );
        m_Arguments.push_back ( pArgument );
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;
}

void CLuaArguments::ReadArguments ( lua_State* luaVM, signed int uiIndexBegin )
{
    // Delete the previous arguments if any
    DeleteArguments ();

    std::map < const void*, CLuaArguments* > knownTables;

    // Start reading arguments until there are none left
    while ( lua_type ( luaVM, uiIndexBegin ) != LUA_TNONE )
    {
        // Create an argument, let it read out the argument and add it to our vector
        CLuaArgument* pArgument = new CLuaArgument ( luaVM, uiIndexBegin++, &knownTables );
        m_Arguments.push_back ( pArgument );

        knownTables.clear ();
    }
}

void CLuaArguments::ReadTable ( lua_State* luaVM, int iIndexBegin, std::map < const void*, CLuaArguments* > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < const void*, CLuaArguments* > ();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert ( std::make_pair ( lua_topointer(luaVM, iIndexBegin), this ) );

    // Delete the previous arguments if any
    DeleteArguments ();

    LUA_CHECKSTACK ( luaVM, 2 );
    lua_pushnil(luaVM);  /* first key */
    if ( iIndexBegin < 0 )
        iIndexBegin--;

    while (lua_next(luaVM, iIndexBegin) != 0) {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        CLuaArgument* pArgument = new CLuaArgument ( luaVM, -2, pKnownTables );
        m_Arguments.push_back ( pArgument ); // push the key first

        pArgument = new CLuaArgument ( luaVM, -1, pKnownTables );
        m_Arguments.push_back ( pArgument ); // then the value
       
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(luaVM, 1);
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;
}

void CLuaArguments::ReadArgument ( lua_State* luaVM, int iIndex )
{
    CLuaArgument* pArgument = new CLuaArgument ( luaVM, iIndex );
    m_Arguments.push_back ( pArgument );
}

void CLuaArguments::PushArguments ( lua_State* luaVM ) const
{
    // Push all our arguments
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end (); iter++ )
    {
        (*iter)->Push ( luaVM );
    }
}

void CLuaArguments::PushAsTable ( lua_State* luaVM, std::map < CLuaArguments*, int > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < CLuaArguments*, int > ();
        bKnownTablesCreated = true;

		lua_newtable ( luaVM );
		// using registry to make it fail safe, else we'd have to carry
		// either lua top or current depth variable between calls
		lua_setfield ( luaVM, LUA_REGISTRYINDEX, "cache" );
    }

    lua_newtable ( luaVM );

	// push it onto the known tables
	int size = pKnownTables->size();
	lua_getfield ( luaVM, LUA_REGISTRYINDEX, "cache" );
	lua_pushnumber ( luaVM, ++size );
	lua_pushvalue ( luaVM, -3 );
	lua_settable ( luaVM, -3 );
	lua_pop ( luaVM, 1 );
    pKnownTables->insert ( std::make_pair ( (CLuaArguments *)this, size ) );

    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () && (iter+1) != m_Arguments.end (); iter ++ )
    {
        (*iter)->Push ( luaVM, pKnownTables ); // index
        iter++;
        (*iter)->Push ( luaVM, pKnownTables ); // value
        lua_settable ( luaVM, -3 );
    }

    if ( bKnownTablesCreated )
	{
		// clear the cache
		lua_pushnil ( luaVM );
		lua_setfield ( luaVM, LUA_REGISTRYINDEX, "cache" );
        delete pKnownTables;
	}
}


void CLuaArguments::PushArguments ( CLuaArguments& Arguments )
{
    vector < CLuaArgument* > ::const_iterator iter = Arguments.IterBegin ();
    for ( ; iter != Arguments.IterEnd (); iter++ )
    {
        CLuaArgument* pArgument = new CLuaArgument ( **iter );
        m_Arguments.push_back ( pArgument );
    }
}


bool CLuaArguments::Call ( CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments * returnValues ) const
{
    assert ( pLuaMain );
    TIMEUS startTime = GetTimeUs ();

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine ();
    assert ( luaVM );
    LUA_CHECKSTACK ( luaVM, 1 );
    int luaStackPointer = lua_gettop ( luaVM );
    lua_getref ( luaVM, iLuaFunction.ToInt () );

    // Push our arguments onto the stack
    PushArguments ( luaVM );

    // Call the function with our arguments
    pLuaMain->ResetInstructionCount ();

    int iret = lua_pcall ( luaVM, m_Arguments.size (), LUA_MULTRET, 0 );
    if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
    {
        SString strRes = ConformResourcePath ( lua_tostring( luaVM, -1 ) );
        
        vector <SString> vecSplit;
        strRes.Split ( ":", vecSplit );
        
        if ( vecSplit.size ( ) >= 3 )
        {
            SString strFile = vecSplit[0];
            int     iLine   = atoi ( vecSplit[1].c_str ( ) );
            SString strMsg  = vecSplit[2].substr ( 1 );
            
            g_pGame->GetScriptDebugging()->LogError ( strFile, iLine, strMsg );
        }
        else
            g_pGame->GetScriptDebugging()->LogError ( luaVM, "%s", strRes.c_str () );

        // cleanup the stack
        while ( lua_gettop ( luaVM ) - luaStackPointer > 0 )
            lua_pop ( luaVM, 1 );

       return false; // the function call failed
    }
    else
    {
        int iReturns = lua_gettop ( luaVM ) - luaStackPointer;

        if ( returnValues != NULL )
        {
            for ( int i = - iReturns; i <= -1; i++ )
            {
                returnValues->ReadArgument ( luaVM, i );
            }
        }

        // cleanup the stack
        while ( lua_gettop ( luaVM ) - luaStackPointer > 0 )
            lua_pop ( luaVM, 1 );
    }

    CPerfStatLuaTiming::GetSingleton ()->UpdateLuaTiming ( pLuaMain, pLuaMain->GetFunctionTag ( iLuaFunction.m_iFunction ), GetTimeUs() - startTime );
    return true;
}


bool CLuaArguments::CallGlobal ( CLuaMain* pLuaMain, const char* szFunction, CLuaArguments * returnValues ) const
{
    assert ( pLuaMain );
    assert ( szFunction );
    TIMEUS startTime = GetTimeUs ();

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine ();
    assert ( luaVM );
    LUA_CHECKSTACK ( luaVM, 1 );
    int luaStackPointer = lua_gettop ( luaVM );
    lua_pushstring ( luaVM, szFunction );
    lua_gettable ( luaVM, LUA_GLOBALSINDEX );

    // Push our arguments onto the stack
    PushArguments ( luaVM );

    // Call the function with our arguments
    pLuaMain->ResetInstructionCount ();
    int iret = 0;
    try {
        iret = lua_pcall ( luaVM, m_Arguments.size (), LUA_MULTRET, 0 );
    }
    catch ( ... )
    {
        return false;
    }
    if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
    {
        std::string strRes = ConformResourcePath ( lua_tostring( luaVM, -1 ) );
        g_pGame->GetScriptDebugging()->LogError ( luaVM, "%s", strRes.c_str () );

        // cleanup the stack
        while ( lua_gettop ( luaVM ) - luaStackPointer > 0 )
            lua_pop ( luaVM, 1 );

        return false; // the function call failed
    }
    else
    {
        int iReturns = lua_gettop ( luaVM ) - luaStackPointer;

        if ( returnValues != NULL )
        {
            for ( int i = - iReturns; i <= -1; i++ )
            {
                returnValues->ReadArgument ( luaVM, i );
            }
        }

        // cleanup the stack
        while ( lua_gettop ( luaVM ) - luaStackPointer > 0 )
            lua_pop ( luaVM, 1 );
    }
        
    CPerfStatLuaTiming::GetSingleton ()->UpdateLuaTiming ( pLuaMain, szFunction, GetTimeUs() - startTime );
    return true;
}


vector < char * > * CLuaArguments::WriteToCharVector ( vector < char * > * values )
{
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () ; iter++ )
    {
        switch ( (*iter)->GetType() )
        {
        case LUA_TNUMBER:
            {
                char * szValue = new char [ 20 ];
                itoa ( ( int ) (*iter)->GetNumber(), szValue, 10 );
                values->push_back ( szValue );
                break;
            }
        case LUA_TSTRING:
            {
                const char * szString = (*iter)->GetString().c_str ();
                char * szValue = new char [ strlen ( szString ) + 1 ];
                strcpy ( szValue, szString );
                values->push_back ( szValue );
                break;
            }
        case LUA_TBOOLEAN:
            {
                char * szValue = new char [ 6 ];
                if ( (*iter)->GetBoolean() )
                    values->push_back ( strcpy ( szValue, "true" ) );
                else
                    values->push_back ( strcpy ( szValue, "false" ) );
                break;
            }
        case LUA_TLIGHTUSERDATA:
            {
                char * szValue = new char [10];
                memset(szValue,0,10);
                CElement* pElement = (*iter)->GetElement ();
                if ( VERIFY_ELEMENT(pElement) )
                {
                    snprintf ( szValue, 9, "E#%d", (int)pElement->GetID().Value() );
                }
                else
                {
                    g_pGame->GetScriptDebugging()->LogError ( NULL, "Couldn't serialize argument list, invalid element specified. Passing empty string instead." );
                }
                values->push_back ( szValue );
            }        
        default:
            {
                char * szEmpty = new char [ 1 ];
                szEmpty[0] = '\0';
                values->push_back ( szEmpty );
            }
        }
    }
    return values;
}



CLuaArgument* CLuaArguments::PushNil ( void )
{
    CLuaArgument* pArgument = new CLuaArgument;
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushBoolean ( bool bBool )
{
    CLuaArgument* pArgument = new CLuaArgument ( bBool );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushTable ( CLuaArguments * table )
{
    CLuaArgument* pArgument = new CLuaArgument (  );
    pArgument->Read(table);
    m_Arguments.push_back ( pArgument );
    return pArgument;
}

CLuaArgument* CLuaArguments::PushNumber ( double dNumber )
{
    CLuaArgument* pArgument = new CLuaArgument ( dNumber );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}

CLuaArgument* CLuaArguments::PushArgument ( const CLuaArgument & argument )
{
    CLuaArgument* pArgument = new CLuaArgument (argument); // create a copy
    m_Arguments.push_back ( pArgument );
    return pArgument;
}

CLuaArgument* CLuaArguments::PushString ( const std::string& strString )
{
    CLuaArgument* pArgument = new CLuaArgument ( strString );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushUserData ( void* pUserData )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pUserData );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushElement ( CElement* pElement )
{
    CLuaArgument* pArgument = new CLuaArgument ( pElement );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushACL ( CAccessControlList* pACL )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pACL );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushACLGroup ( CAccessControlListGroup* pACLGroup )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pACLGroup );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushAccount ( CAccount* pAccount )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pAccount );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushResource ( CResource* pResource )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pResource );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushTextDisplay ( CTextDisplay* pTextDisplay )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pTextDisplay );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushTextItem ( CTextItem* pTextItem )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pTextItem );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushTimer ( CLuaTimer* pLuaTimer )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadUserData ( pLuaTimer );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


void CLuaArguments::DeleteArguments ( void )
{
    // Delete each item
    vector < CLuaArgument* > ::iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the vector
    m_Arguments.clear ();
}


void CLuaArguments::ValidateTableKeys ( void )
{
    // Iterate over m_Arguments as pairs
    // If first is LUA_TNIL, then remove pair
    vector < CLuaArgument* > ::iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () ; )
    {
        // Check first in pair
        if ( (*iter)->GetType () == LUA_TNIL )
        {
            // Remove pair
            delete *iter;
            iter = m_Arguments.erase ( iter );
            if ( iter != m_Arguments.end () )
            {
                delete *iter;
                iter = m_Arguments.erase ( iter );
            }
            // Check if end
            if ( iter == m_Arguments.end () )
                break;
        }
        else
        {
            // Skip second in pair
            iter++;
            // Check if end
            if ( iter == m_Arguments.end () )
                break;

            iter++;
        }
    }
}


bool CLuaArguments::ReadFromBitStream ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::vector < CLuaArguments* > ();
        bKnownTablesCreated = true;
    }

    unsigned short usNumArgs;
    if ( bitStream.ReadCompressed ( usNumArgs ) )
    {
        pKnownTables->push_back ( this );
        for ( unsigned short us = 0 ; us < usNumArgs ; us++ )
        {
            CLuaArgument* pArgument = new CLuaArgument ( bitStream, pKnownTables );
            m_Arguments.push_back ( pArgument );
        }
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;

    return true;
}


bool CLuaArguments::WriteToBitStream ( NetBitStreamInterface& bitStream, std::map < CLuaArguments*, unsigned long > * pKnownTables ) const
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < CLuaArguments*, unsigned long > ();
        bKnownTablesCreated = true;
    }

    bool bSuccess = true;
    pKnownTables->insert ( make_pair ( (CLuaArguments *)this, pKnownTables->size () ) );
    bitStream.WriteCompressed ( static_cast < unsigned short > ( m_Arguments.size () ) );
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () ; iter++ )
    {
        CLuaArgument* pArgument = *iter;
        if ( !pArgument->WriteToBitStream ( bitStream, pKnownTables ) )
        {
            bSuccess = false;
        }
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;

    return bSuccess;
}

bool CLuaArguments::WriteToJSONString ( std::string& strJSON, bool bSerialize )
{
    json_object * my_array = WriteToJSONArray ( bSerialize );
    if ( my_array )
    {
        strJSON = json_object_get_string ( my_array );
        json_object_put ( my_array ); // dereference - causes a crash, is actually commented out in the example too
        return true;
    }
    return false;
}

json_object * CLuaArguments::WriteToJSONArray ( bool bSerialize )
{
    json_object * my_array = json_object_new_array();
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () ; iter++ )
    {
        CLuaArgument* pArgument = *iter;
        json_object * object = pArgument->WriteToJSONObject ( bSerialize );
        if ( object )
        {
            json_object_array_add(my_array, object);
        }
        else
        {
            break;
        }
    }
    return my_array;
}

json_object * CLuaArguments::WriteTableToJSONObject ( bool bSerialize, std::map < CLuaArguments*, unsigned long > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < CLuaArguments*, unsigned long > ();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert ( std::make_pair ( this, pKnownTables->size () ) );

    bool bIsArray = true;
    unsigned int iArrayPos = 1; // lua arrays are 1 based
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () ; iter+=2 )
    {
        CLuaArgument* pArgument = *iter;
        if ( pArgument->GetType() == LUA_TNUMBER )
        {
            double num = pArgument->GetNumber();
            unsigned int iNum = static_cast < unsigned int > ( num );
            if ( num == iNum )
            {
                if ( iArrayPos != iNum ) // check if the value matches its index in the table
                {
                    bIsArray = false;
                    break;
                }
            }
            else
            {
                bIsArray = false;
                break;
            }
        }
        else
        {
            bIsArray = false;
            break;
        }
        iArrayPos++;
    }
    
    if ( bIsArray )
    {
        json_object * my_array = json_object_new_array();
        vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
        for ( ; iter != m_Arguments.end () ; iter++ ) 
        {
            iter++; // skip the key values
            CLuaArgument* pArgument = *iter;
            json_object * object = pArgument->WriteToJSONObject ( bSerialize, pKnownTables );
            if ( object )
            {
                json_object_array_add(my_array, object);
            }
            else
            {
                break;
            }
        }
        if ( bKnownTablesCreated )
            delete pKnownTables;
        return my_array;
    }
    else
    {
        json_object * my_object = json_object_new_object();
        iter = m_Arguments.begin ();
        for ( ; iter != m_Arguments.end () ; iter++ )
        {
            char szKey[255];
            szKey[0] = '\0';
            CLuaArgument* pArgument = *iter;
            if ( !pArgument->WriteToString(szKey, 255) ) // index
                break;
            iter++;
            pArgument = *iter;
            json_object * object = pArgument->WriteToJSONObject ( bSerialize, pKnownTables ); // value

            if ( object )
            {
                json_object_object_add(my_object, szKey, object);
            }
            else
            {            
                break;
            }
        }
        if ( bKnownTablesCreated )
            delete pKnownTables;
        return my_object;
    }
}


bool CLuaArguments::ReadFromJSONString ( const char* szJSON )
{
    json_object* object = json_tokener_parse ( szJSON );
    if ( !is_error(object) )
    {
        if ( json_object_get_type ( object ) == json_type_array )
        {
            bool bSuccess = true;

            std::vector < CLuaArguments* > knownTables;
            
            for(int i=0; i < json_object_array_length(object); i++) 
            {
                json_object *arrayObject = json_object_array_get_idx(object, i);
                CLuaArgument * pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject ( arrayObject, &knownTables );
                m_Arguments.push_back ( pArgument ); // then the value
                if ( !bSuccess )
                    break;
            }
            json_object_put ( object ); // dereference
            return bSuccess;
        }
        json_object_put ( object ); // dereference
    }
//    else
//        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
 //   else
//        g_pGame->GetScriptDebugging()->LogError ( "Could not parse HTTP POST request, ensure data uses JSON.");
    return false;
}

bool CLuaArguments::ReadFromJSONObject ( json_object * object, std::vector < CLuaArguments* > * pKnownTables )
{
    if ( !is_error(object) )
    {
        if ( json_object_get_type ( object ) == json_type_object )
        {
            bool bKnownTablesCreated = false;
            if ( !pKnownTables )
            {
                pKnownTables = new std::vector < CLuaArguments* > ();
                bKnownTablesCreated = true;
            }

            pKnownTables->push_back ( this );

            bool bSuccess = true;
            json_object_object_foreach(object, key, val) 
            {
                CLuaArgument* pArgument = new CLuaArgument ( std::string ( key ) );
                m_Arguments.push_back ( pArgument ); // push the key first
                pArgument = new CLuaArgument ( );
                bSuccess = pArgument->ReadFromJSONObject ( val, pKnownTables ); // then the value
                m_Arguments.push_back ( pArgument );
                if ( !bSuccess )
                    break;
            }

            if ( bKnownTablesCreated )
                delete pKnownTables;
            return bSuccess;
        }
    }
 //   else
//        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    return false;
}

bool CLuaArguments::ReadFromJSONArray ( json_object * object, std::vector < CLuaArguments* > * pKnownTables )
{
    if ( !is_error(object) )
    {
        if ( json_object_get_type ( object ) == json_type_array )
        {
            bool bKnownTablesCreated = false;
            if ( !pKnownTables )
            {
                pKnownTables = new std::vector < CLuaArguments* > ();
                bKnownTablesCreated = true;
            }

            pKnownTables->push_back ( this );

            bool bSuccess = true;
            for(int i=0; i < json_object_array_length(object); i++) 
            {
                json_object *arrayObject = json_object_array_get_idx(object, i);
                CLuaArgument* pArgument = new CLuaArgument ((double)i+1); // push the key
                m_Arguments.push_back ( pArgument );

                pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject ( arrayObject, pKnownTables );
                m_Arguments.push_back ( pArgument ); // then the valoue
                if ( !bSuccess )
                    break;
            }

            if ( bKnownTablesCreated )
                delete pKnownTables;
            return bSuccess;
        }
    }
//    else
//        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    return false;
}
