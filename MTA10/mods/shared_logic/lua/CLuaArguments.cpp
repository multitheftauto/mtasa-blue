/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaArguments.cpp
*  PURPOSE:     Lua arguments manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CLuaArguments
#include "profiler/SharedUtil.Profiler.h"

using namespace std;

#ifndef VERIFY_ENTITY
#define VERIFY_ENTITY(entity) (CStaticFunctionDefinitions::GetRootElement()->IsMyChild(entity,true)&&!entity->IsBeingDeleted())
#endif
 
extern CClientGame* g_pClientGame;

CLuaArguments::CLuaArguments ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    ReadFromBitStream ( bitStream, pKnownTables );
}


CLuaArguments::CLuaArguments ( const CLuaArguments& Arguments, CFastHashMap < CLuaArguments*, CLuaArguments* > * pKnownTables )
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


// Slow if used with a constructor as it does a copy twice
const CLuaArguments& CLuaArguments::operator = ( const CLuaArguments& Arguments )
{
    CopyRecursive ( Arguments );

    // Return the given reference allowing for chaining
    return Arguments;
}


void CLuaArguments::CopyRecursive ( const CLuaArguments& Arguments, CFastHashMap < CLuaArguments*, CLuaArguments* > * pKnownTables )
{
    // Clear our previous list if any
    DeleteArguments ();

    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new CFastHashMap < CLuaArguments*, CLuaArguments* > ();
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

    CFastHashMap < const void*, CLuaArguments* > knownTables;

    // Start reading arguments until there are none left
    while ( lua_type ( luaVM, uiIndexBegin ) != LUA_TNONE )
    {
        // Create an argument, let it read out the argument and add it to our vector
        CLuaArgument* pArgument = new CLuaArgument ( luaVM, uiIndexBegin++, &knownTables );
        m_Arguments.push_back ( pArgument );

        knownTables.clear ();
    }
}

void CLuaArguments::ReadTable ( lua_State* luaVM, int iIndexBegin, CFastHashMap < const void*, CLuaArguments* > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new CFastHashMap < const void*, CLuaArguments* > ();
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

void CLuaArguments::PushAsTable ( lua_State* luaVM, CFastHashMap < CLuaArguments*, int > * pKnownTables )
{
    // Ensure there is enough space on the Lua stack
    LUA_CHECKSTACK ( luaVM, 4 );

    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new CFastHashMap < CLuaArguments*, int > ();
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


void CLuaArguments::PushArguments ( const CLuaArguments& Arguments )
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
    LUA_CHECKSTACK ( luaVM, 2 );
    int luaStackPointer = lua_gettop ( luaVM );
    lua_getref ( luaVM, iLuaFunction.ToInt () );

    // Push our arguments onto the stack
    PushArguments ( luaVM );

    // Call the function with our arguments
    pLuaMain->ResetInstructionCount ();

    int iret = pLuaMain->PCall ( luaVM, m_Arguments.size (), LUA_MULTRET, 0 );
    if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
    {
        SString strRes = ConformResourcePath ( lua_tostring( luaVM, -1 ) );
        
        // Split the error message
        vector <SString> vecSplit;
        strRes.Split ( ":", vecSplit );
        
        // If it consists of 3 parts
        if ( vecSplit.size ( ) >= 3 )
        {
            // Pass it to a special LogError function (because with normal Lua errors, the other one won't be able to get the file and line of the error)
            SString strFile = vecSplit[0];
            int     iLine   = atoi ( vecSplit[1].c_str ( ) );
            SString strMsg  = vecSplit[2].substr ( 1 );
            
            g_pClientGame->GetScriptDebugging()->LogError ( strFile, iLine, strMsg );
        }
        else
            g_pClientGame->GetScriptDebugging()->LogError ( luaVM, "%s", strRes.c_str () );

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
        
    CClientPerfStatLuaTiming::GetSingleton ()->UpdateLuaTiming ( pLuaMain, pLuaMain->GetFunctionTag ( iLuaFunction.ToInt() ), GetTimeUs() - startTime );
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

    int iret = pLuaMain->PCall ( luaVM, m_Arguments.size (), LUA_MULTRET, 0 );
    if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
    {
        std::string strRes = ConformResourcePath ( lua_tostring( luaVM, -1 ) );
        g_pClientGame->GetScriptDebugging()->LogError ( luaVM, "%s", strRes.c_str () );

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
        
    CClientPerfStatLuaTiming::GetSingleton ()->UpdateLuaTiming ( pLuaMain, szFunction, GetTimeUs() - startTime );
    return true;
}


CLuaArgument* CLuaArguments::PushNil ( void )
{
    CLuaArgument* pArgument = new CLuaArgument;
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushBoolean ( bool bBool )
{
    CLuaArgument* pArgument = new CLuaArgument ();
    pArgument->ReadBool ( bBool );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushNumber ( double dNumber )
{
    CLuaArgument* pArgument = new CLuaArgument ();
    pArgument->ReadNumber ( dNumber );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushString ( const std::string& strString )
{
    CLuaArgument* pArgument = new CLuaArgument ();
    pArgument->ReadString ( strString );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushResource ( CResource* pResource )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID ( pResource->GetScriptID () );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushElement ( CClientEntity* pElement )
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadElement ( pElement );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushArgument ( const CLuaArgument& Argument )
{
    CLuaArgument* pArgument = new CLuaArgument ( Argument );
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
            // TODO - Handle ref in KnownTables
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


bool CLuaArguments::WriteToBitStream ( NetBitStreamInterface& bitStream, CFastHashMap < CLuaArguments*, unsigned long > * pKnownTables ) const
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new CFastHashMap < CLuaArguments*, unsigned long > ();
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

json_object * CLuaArguments::WriteTableToJSONObject ( bool bSerialize, CFastHashMap < CLuaArguments*, unsigned long > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new CFastHashMap < CLuaArguments*, unsigned long > ();
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
    // Fast isJSON check: Check first non-white space character is '[' or '{'
    for ( const char* ptr = szJSON ; true ; )
    {
        char c = *ptr++;
        if ( c == '[' || c == '{' )
            break;
        if ( isspace( (uchar)c ) )
            continue;
        return false;
    }

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
		else if (json_object_get_type(object) == json_type_object)
        {
            std::vector < CLuaArguments* > knownTables;
            CLuaArgument * pArgument = new CLuaArgument();
            bool bSuccess = pArgument->ReadFromJSONObject(object, &knownTables);
            m_Arguments.push_back(pArgument); // value
            json_object_put(object);

            return bSuccess;
        }
        json_object_put ( object ); // dereference
    }
//    else
//        g_pClientGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
 //   else
//        g_pClientGame->GetScriptDebugging()->LogError ( "Could not parse HTTP POST request, ensure data uses JSON.");
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
                CLuaArgument* pArgument = new CLuaArgument ();
                pArgument->ReadString ( key );
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
//        g_pClientGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
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
                CLuaArgument* pArgument = new CLuaArgument ();
                pArgument->ReadNumber ( i + 1 ); // push the key
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
//        g_pClientGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    return false;
}
