/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaDatabaseDefs.cpp
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaDatabaseDefs::LoadFunctions ()
{
    CLuaCFunctions::AddFunction ( "dbConnect", DbConnect );
    CLuaCFunctions::AddFunction ( "dbExec", DbExec );
    CLuaCFunctions::AddFunction ( "dbQuery", DbQuery );
    CLuaCFunctions::AddFunction ( "dbFree", DbFree );
    CLuaCFunctions::AddFunction ( "dbPoll", DbPoll );
    CLuaCFunctions::AddFunction ( "dbPrepareString", DbPrepareString );

    CLuaCFunctions::AddFunction ( "executeSQLCreateTable", ExecuteSQLCreateTable );
    CLuaCFunctions::AddFunction ( "executeSQLDropTable", ExecuteSQLDropTable );
    CLuaCFunctions::AddFunction ( "executeSQLDelete", ExecuteSQLDelete );
    CLuaCFunctions::AddFunction ( "executeSQLSelect", ExecuteSQLSelect );
    CLuaCFunctions::AddFunction ( "executeSQLUpdate", ExecuteSQLUpdate );
    CLuaCFunctions::AddFunction ( "executeSQLInsert", ExecuteSQLInsert );
    CLuaCFunctions::AddFunction ( "executeSQLQuery", ExecuteSQLQuery );
}


void CLuaDatabaseDefs::AddClass ( lua_State* luaVM )
{
    //////////////////////////
    //  Connection class
    //////////////////////////
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "dbConnect" );
    lua_classfunction ( luaVM, "exec", "dbExec" );
    lua_classfunction ( luaVM, "prepareString", "dbPrepareString" );
    lua_classfunction ( luaVM, "query", "dbQuery", OOP_DbQuery );

    lua_registerclass ( luaVM, "Connection", "Element" );
    //////////////////////////



    //////////////////////////
    //  QueryHandle class
    //////////////////////////
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "poll", "dbPoll" );
    lua_classfunction ( luaVM, "free", "dbFree" );

    lua_registerclass ( luaVM, "QueryHandle", NULL, false );
    //////////////////////////
}


int CLuaDatabaseDefs::ExecuteSQLCreateTable ( lua_State* luaVM )
{
    SString strTable;
    SString strDefinition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTable );
    argStream.ReadString ( strDefinition );

    if ( !argStream.HasErrors () )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        CStaticFunctionDefinitions::ExecuteSQLCreateTable ( strTable, strDefinition );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::ExecuteSQLDropTable ( lua_State* luaVM )
{
    SString strTable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTable );

    if ( !argStream.HasErrors () )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        CStaticFunctionDefinitions::ExecuteSQLDropTable ( strTable );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLDelete ( lua_State* luaVM )
{
    SString strTable;
    SString strDefinition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTable );
    argStream.ReadString ( strDefinition );

    if ( !argStream.HasErrors () )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLDelete ( strTable, strDefinition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, "%s", strError.c_str () );

            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::ExecuteSQLInsert ( lua_State* luaVM )
{
    SString strTable;
    SString strDefinition;
    SString strColumns;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTable );
    argStream.ReadString ( strDefinition );
    argStream.ReadString ( strColumns, "" );

    if ( !argStream.HasErrors () )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLInsert ( strTable, strDefinition, strColumns ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, "%s", strError.c_str () );

            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


//
// Db function callbacks
//

// Handle directing callback for DbQuery
void CLuaDatabaseDefs::DbQueryCallback ( CDbJobData* pJobData, void* pContext )
{
    CLuaCallback* pLuaCallback = (CLuaCallback*) pContext;
    if ( pJobData->stage == EJobStage::RESULT )
    {
        if ( pLuaCallback )
            pLuaCallback->Call ();
    }
    g_pGame->GetLuaCallbackManager ()->DestroyCallback ( pLuaCallback );
}

// Handle callback for DbExec
void CLuaDatabaseDefs::DbExecCallback ( CDbJobData* pJobData, void* pContext )
{
    assert ( pContext == NULL );
    if ( pJobData->stage >= EJobStage::RESULT && pJobData->result.status == EJobResult::FAIL )
    {
        if ( !pJobData->result.bErrorSuppressed )
            m_pScriptDebugging->LogWarning ( pJobData->m_LuaDebugInfo, "dbExec failed; (%d) %s", pJobData->result.uiErrorCode, *pJobData->result.strReason );
    }
}

// Handle callback for DbFree
void CLuaDatabaseDefs::DbFreeCallback ( CDbJobData* pJobData, void* pContext )
{
    assert ( pContext == NULL );
    if ( pJobData->stage >= EJobStage::RESULT && pJobData->result.status == EJobResult::FAIL )
    {
        if ( !pJobData->result.bErrorSuppressed )
            m_pScriptDebugging->LogWarning ( pJobData->m_LuaDebugInfo, "dbExec failed; (%d) %s", pJobData->result.uiErrorCode, *pJobData->result.strReason );
    }
}

int CLuaDatabaseDefs::DbConnect ( lua_State* luaVM )
{
    //  element dbConnect ( string type, string host, string username, string password, string options )
    SString strType; SString strHost; SString strUsername; SString strPassword; SString strOptions;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strType );
    argStream.ReadString ( strHost );
    argStream.ReadString ( strUsername, "" );
    argStream.ReadString ( strPassword, "" );
    argStream.ReadString ( strOptions, "" );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pThisResource = pLuaMain->GetResource ();

            // If type is sqlite, and has a host, try to resolve path
            if ( strType == "sqlite" && !strHost.empty () )
            {
                // If path starts with :/ then use global database directory
                if ( strHost.BeginsWith ( ":/" ) )
                {
                    strHost = strHost.SubStr ( 1 );
                    if ( !IsValidFilePath ( strHost ) )
                    {
                        m_pScriptDebugging->LogError ( luaVM, "%s failed; host path not valid", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                        lua_pushboolean ( luaVM, false );
                        return 1;
                    }
                    strHost = PathJoin ( g_pGame->GetConfig ()->GetGlobalDatabasesPath (), strHost );
                }
                else
                {
                    std::string strAbsPath;

                    // Parse path
                    CResource* pPathResource = pThisResource;
                    if ( CResourceManager::ParseResourcePathInput ( strHost, pPathResource, &strAbsPath, NULL ) )
                    {
                        if ( pPathResource == pThisResource ||
                            m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                "ModifyOtherObjects",
                                CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                false ) )
                        {
                            strHost = strAbsPath;
                        }
                        else
                        {
                            m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pPathResource->GetName ().c_str () );
                            lua_pushboolean ( luaVM, false );
                            return 1;
                        }
                    }
                    else
                    {
                        m_pScriptDebugging->LogError ( luaVM, "%s failed; host path %s not found", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *strHost );
                        lua_pushboolean ( luaVM, false );
                        return 1;
                    }
                }
            }
            // Add logging options
            bool bLoggingEnabled;
            SString strLogTag;
            // Read value of 'log' and 'tag' if already set, otherwise use default
            GetOption < CDbOptionsMap > ( strOptions, "log", bLoggingEnabled, 1 );
            GetOption < CDbOptionsMap > ( strOptions, "tag", strLogTag, "script" );
            SetOption < CDbOptionsMap > ( strOptions, "log", bLoggingEnabled );
            SetOption < CDbOptionsMap > ( strOptions, "tag", strLogTag );
            // Do connect
            SConnectionHandle connection = g_pGame->GetDatabaseManager ()->Connect ( strType, strHost, strUsername, strPassword, strOptions );
            if ( connection == INVALID_DB_HANDLE )
            {
                m_pScriptDebugging->LogError ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Use an element to wrap the connection for auto disconnected when the resource stops
            CDatabaseConnectionElement* pElement = new CDatabaseConnectionElement ( NULL, connection );
            CElementGroup * pGroup = pThisResource->GetElementGroup ();
            if ( pGroup )
            {
                pGroup->Add ( pElement );
            }

            lua_pushelement ( luaVM, pElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

// This method has an OOP counterpart - don't forget to update the OOP code too!
int CLuaDatabaseDefs::DbQuery ( lua_State* luaVM )
{
    //  handle dbQuery ( [ function callbackFunction, [ table callbackArguments, ] ] element connection, string query, ... )
    CLuaFunctionRef iLuaFunction; CLuaArguments callbackArgs; CDatabaseConnectionElement* pElement; SString strQuery; CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsFunction () )
    {
        argStream.ReadFunction ( iLuaFunction );
        if ( argStream.NextIsTable () )
        {
            lua_pushnil ( luaVM );      // Loop through our table, beginning at the first key
            while ( lua_next ( luaVM, argStream.m_iIndex ) != 0 )
            {
                callbackArgs.ReadArgument ( luaVM, -1 );    // Ignore the index at -2, and just read the value
                lua_pop ( luaVM, 1 );                       // Remove the item and keep the key for the next iteration
            }
            argStream.m_iIndex++;
        }
    }
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strQuery );
    argStream.ReadLuaArguments ( Args );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager ()->QueryStart ( pElement->GetConnectionHandle (), strQuery, &Args );
        if ( !pJobData )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogWarning ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Make callback function if required
        if ( VERIFY_FUNCTION ( iLuaFunction ) )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CLuaArguments Arguments;
                Arguments.PushDbQuery ( pJobData );
                Arguments.PushArguments ( callbackArgs );
                pJobData->SetCallback ( DbQueryCallback, g_pGame->GetLuaCallbackManager ()->CreateCallback ( pLuaMain, iLuaFunction, Arguments ) );
            }
        }
        // Add debug info incase query result does not get collected
        pJobData->SetLuaDebugInfo ( g_pGame->GetScriptDebugging ()->GetLuaDebugInfo ( luaVM ) );
        lua_pushquery ( luaVM, pJobData );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::OOP_DbQuery ( lua_State* luaVM )
{
    //  handle dbQuery ( [ function callbackFunction, [ table callbackArguments, ] ] element connection, string query, ... )
    CLuaFunctionRef iLuaFunction; CLuaArguments callbackArgs; CDatabaseConnectionElement* pElement; SString strQuery; CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pElement );
    if ( argStream.NextIsFunction () )
    {
        argStream.ReadFunction ( iLuaFunction );
        if ( argStream.NextIsTable () )
        {
            lua_pushnil ( luaVM );      // Loop through our table, beginning at the first key
            while ( lua_next ( luaVM, argStream.m_iIndex ) != 0 )
            {
                callbackArgs.ReadArgument ( luaVM, -1 );    // Ignore the index at -2, and just read the value
                lua_pop ( luaVM, 1 );                       // Remove the item and keep the key for the next iteration
            }
            argStream.m_iIndex++;
        }
    }
    argStream.ReadString ( strQuery );
    argStream.ReadLuaArguments ( Args );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager ()->QueryStart ( pElement->GetConnectionHandle (), strQuery, &Args );
        if ( !pJobData )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogWarning ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Make callback function if required
        if ( VERIFY_FUNCTION ( iLuaFunction ) )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CLuaArguments Arguments;
                Arguments.PushDbQuery ( pJobData );
                Arguments.PushArguments ( callbackArgs );
                pJobData->SetCallback ( CLuaDatabaseDefs::DbQueryCallback, g_pGame->GetLuaCallbackManager ()->CreateCallback ( pLuaMain, iLuaFunction, Arguments ) );
            }
        }
        // Add debug info incase query result does not get collected
        pJobData->SetLuaDebugInfo ( g_pGame->GetScriptDebugging ()->GetLuaDebugInfo ( luaVM ) );
        lua_pushquery ( luaVM, pJobData );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::DbExec ( lua_State* luaVM )
{
    //  bool dbExec ( element connection, string query, ... )
    CDatabaseConnectionElement* pElement; SString strQuery; CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strQuery );
    argStream.ReadLuaArguments ( Args );

    if ( !argStream.HasErrors () )
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager ()->Exec ( pElement->GetConnectionHandle (), strQuery, &Args );
        if ( !pJobData )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogError ( luaVM, "%s failed: %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Add callback for tracking errors
        pJobData->SetCallback ( DbExecCallback, NULL );
        pJobData->SetLuaDebugInfo ( g_pGame->GetScriptDebugging ()->GetLuaDebugInfo ( luaVM ) );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::DbFree ( lua_State* luaVM )
{
    //  bool dbFree ( handle query )
    CDbJobData* pJobData;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pJobData );

    if ( !argStream.HasErrors () )
    {
        // Add callback for tracking errors
        pJobData->SetCallback ( DbFreeCallback, NULL );
        pJobData->SetLuaDebugInfo ( g_pGame->GetScriptDebugging ()->GetLuaDebugInfo ( luaVM ) );

        bool bResult = g_pGame->GetDatabaseManager ()->QueryFree ( pJobData );
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


// Plop single set of registry results into a lua table
void PushRegistryResultTable( lua_State* luaVM, const CRegistryResultData* Result )
{
    lua_newtable ( luaVM );
    int i = 0;
    for ( CRegistryResultIterator iter = Result->begin (); iter != Result->end (); ++iter, ++i )
    {
        const CRegistryResultRow& row = *iter;
        lua_pushnumber ( luaVM, i + 1 );
        lua_newtable ( luaVM );
        for ( int j = 0; j < Result->nColumns; j++ )
        {
            const CRegistryResultCell& cell = row[j];

            // Push the column name
            lua_pushlstring ( luaVM, Result->ColNames[j].c_str (), Result->ColNames[j].size () );
            switch ( cell.nType )                           // push the value with the right type
            {
            case SQLITE_INTEGER:
                lua_pushnumber ( luaVM, static_cast <double> ( cell.nVal ) );
                break;
            case SQLITE_FLOAT:
                lua_pushnumber ( luaVM, cell.fVal );
                break;
            case SQLITE_BLOB:
                lua_pushlstring ( luaVM, (const char *) cell.pVal, cell.nLength );
                break;
            case SQLITE_TEXT:
                lua_pushlstring ( luaVM, (const char *) cell.pVal, cell.nLength - 1 );
                break;
            default:
                lua_pushboolean ( luaVM, false );
            }
            lua_settable ( luaVM, -3 );
        }
        lua_settable ( luaVM, -3 );
    }
}


int CLuaDatabaseDefs::DbPoll ( lua_State* luaVM )
{
    //  table dbPoll ( handle query, int timeout )
    CDbJobData* pJobData; int iTimeout; bool bMultipleResults;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pJobData );
    argStream.ReadNumber ( iTimeout );
    argStream.ReadBool ( bMultipleResults, false );

    if ( !argStream.HasErrors () )
    {
        // Extra input validation
        if ( pJobData->stage > EJobStage::RESULT )
            argStream.SetCustomError ( "Previous dbPoll already returned result" );
        if ( pJobData->result.bIgnoreResult )
            argStream.SetCustomError ( "Cannot call dbPoll after dbFree" );
    }

    if ( !argStream.HasErrors () )
    {
        if ( !g_pGame->GetDatabaseManager ()->QueryPoll ( pJobData, iTimeout ) )
        {
            // Not ready yet
            lua_pushnil ( luaVM );
            return 1;
        }

        if ( pJobData->result.status == EJobResult::FAIL )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogWarning ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            lua_pushnumber ( luaVM, pJobData->result.uiErrorCode );
            lua_pushstring ( luaVM, pJobData->result.strReason );
            return 3;
        }

        const CRegistryResultData* Result = pJobData->result.registryResult->GetThis();

        if ( !bMultipleResults )
        {
            // Single result (from first statement)
            PushRegistryResultTable( luaVM, Result );
            lua_pushnumber ( luaVM, Result->uiNumAffectedRows );
            lua_pushnumber ( luaVM, static_cast <double> ( Result->ullLastInsertId ) );
            return 3;
        }
        else
        {
            // One or more results (from multiple statements)
            lua_newtable ( luaVM );
            for ( int i = 0 ; Result ; Result = Result->pNextResult, i++ )
            {
                lua_pushnumber ( luaVM, i + 1 );
                lua_newtable ( luaVM );
                {
                    lua_pushnumber ( luaVM, 1 );        // [1] - table of result rows
                    PushRegistryResultTable( luaVM, Result );
                    lua_settable ( luaVM, -3 );

                    lua_pushnumber ( luaVM, 2 );        // [2] - NumAffectedRows
                    lua_pushnumber ( luaVM, Result->uiNumAffectedRows );
                    lua_settable ( luaVM, -3 );

                    lua_pushnumber ( luaVM, 3 );        // [3] - LastInsertId
                    lua_pushnumber ( luaVM, static_cast <double> ( Result->ullLastInsertId ) );
                    lua_settable ( luaVM, -3 );
                }
                lua_settable ( luaVM, -3 );
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::DbPrepareString ( lua_State* luaVM )
{
    //  string dbPrepareString ( element connection, string query, ... )
    CDatabaseConnectionElement* pElement; SString strQuery; CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strQuery );
    argStream.ReadLuaArguments ( Args );

    if ( !argStream.HasErrors () )
    {
        SString strResult = g_pGame->GetDatabaseManager ()->PrepareString ( pElement->GetConnectionHandle (), strQuery, &Args );
        SString strError = g_pGame->GetDatabaseManager ()->GetLastErrorMessage ();
        if ( !strResult.empty () || strError.empty () )
        {
            lua_pushstring ( luaVM, strResult );
            return 1;
        }
        if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
            argStream.SetCustomError ( strError );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::ExecuteSQLQuery ( lua_State* luaVM )
{
    SString strQuery;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strQuery );


    if ( !argStream.HasErrors () )
    {
        CLuaArguments Args;
        CRegistryResult Result;

        Args.ReadArguments ( luaVM, 2 );

        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLQuery ( strQuery, &Args, &Result ) ) {
            lua_newtable ( luaVM );
            int i = 0;
            for ( CRegistryResultIterator iter = Result->begin (); iter != Result->end (); ++iter, ++i )
            {
                const CRegistryResultRow& row = *iter;
                //for ( int i = 0; i < Result.nRows; i++ ) {
                lua_newtable ( luaVM );                             // new table
                lua_pushnumber ( luaVM, i + 1 );                      // row index number (starting at 1, not 0)
                lua_pushvalue ( luaVM, -2 );                        // value
                lua_settable ( luaVM, -4 );                         // refer to the top level table
                for ( int j = 0; j < Result->nColumns; j++ )
                {
                    const CRegistryResultCell& cell = row[j];
                    if ( cell.nType == SQLITE_NULL )
                        continue;

                    // Push the column name
                    lua_pushlstring ( luaVM, Result->ColNames[j].c_str (), Result->ColNames[j].size () );
                    switch ( cell.nType )                           // push the value with the right type
                    {
                    case SQLITE_INTEGER:
                        lua_pushnumber ( luaVM, static_cast <double> ( cell.nVal ) );
                        break;
                    case SQLITE_FLOAT:
                        lua_pushnumber ( luaVM, cell.fVal );
                        break;
                    case SQLITE_BLOB:
                        lua_pushlstring ( luaVM, (const char *) cell.pVal, cell.nLength );
                        break;
                    case SQLITE_TEXT:
                        lua_pushlstring ( luaVM, (const char *) cell.pVal, cell.nLength - 1 );
                        break;
                    default:
                        lua_pushnil ( luaVM );
                    }
                    lua_settable ( luaVM, -3 );
                }
                lua_pop ( luaVM, 1 );                               // pop the inner table
            }
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, "%s", strError.c_str () );

            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLSelect ( lua_State* luaVM )
{
    SString strTable;
    SString strColumns;
    SString strWhere;
    SString strError;
    unsigned int uiLimit;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTable );
    argStream.ReadString ( strColumns );
    argStream.ReadString ( strWhere, "" );
    argStream.ReadNumber ( uiLimit, 0 );

    if ( !argStream.HasErrors () )
    {
        CRegistryResult Result;
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLSelect ( strTable, strColumns, strWhere, uiLimit, &Result ) )
        {
            lua_newtable ( luaVM );
            int i = 0;
            for ( CRegistryResultIterator iter = Result->begin (); iter != Result->end (); ++iter, ++i )
            {
                const CRegistryResultRow& row = *iter;
                //            for ( int i = 0; i < Result.nRows; i++ ) {
                lua_newtable ( luaVM );                             // new table
                lua_pushnumber ( luaVM, i + 1 );                      // row index number (starting at 1, not 0)
                lua_pushvalue ( luaVM, -2 );                        // value
                lua_settable ( luaVM, -4 );                         // refer to the top level table
                for ( int j = 0; j < Result->nColumns; j++ )
                {
                    const CRegistryResultCell& cell = row[j];
                    if ( cell.nType == SQLITE_NULL )
                        continue;

                    // Push the column name
                    lua_pushlstring ( luaVM, Result->ColNames[j].c_str (), Result->ColNames[j].size () );
                    switch ( cell.nType )                           // push the value with the right type
                    {
                    case SQLITE_INTEGER:
                        lua_pushnumber ( luaVM, static_cast <double> ( cell.nVal ) );
                        break;
                    case SQLITE_FLOAT:
                        lua_pushnumber ( luaVM, cell.fVal );
                        break;
                    case SQLITE_BLOB:
                        lua_pushlstring ( luaVM, (const char *) cell.pVal, cell.nLength );
                        break;
                    case SQLITE_TEXT:
                        lua_pushlstring ( luaVM, (const char *) cell.pVal, cell.nLength - 1 );
                        break;
                    default:
                        lua_pushnil ( luaVM );
                    }
                    lua_settable ( luaVM, -3 );
                }
                lua_pop ( luaVM, 1 );                               // pop the inner table
            }
            return 1;
        }
        else
        {
            strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, "%s", strError.c_str () );

            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaDatabaseDefs::ExecuteSQLUpdate ( lua_State* luaVM )
{
    SString strTable;
    SString strSet;
    SString strWhere;
    SString strError;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTable );
    argStream.ReadString ( strSet );
    argStream.ReadString ( strWhere, "" );

    if ( !argStream.HasErrors () )
    {

        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLUpdate ( strTable, strSet, strWhere ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else {
            strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, "%s", strError.c_str () );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    lua_pushstring ( luaVM, strError.c_str () );
    return 2;
}
