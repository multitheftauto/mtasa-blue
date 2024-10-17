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
#include "CLuaDatabaseDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"
#include "CPerfStatManager.h"
#include "lua/CLuaCallback.h"
#include "Utils.h"
#include <lua/CLuaShared.h>

void CLuaDatabaseDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"dbConnect", ArgumentParser<DbConnect>},
        {"dbExec", DbExec},
        {"dbQuery", DbQuery},
        {"dbFree", DbFree},
        {"dbPoll", DbPoll},
        {"dbPrepareString", DbPrepareString},

        {"executeSQLCreateTable", ExecuteSQLCreateTable},
        {"executeSQLDropTable", ExecuteSQLDropTable},
        {"executeSQLDelete", ExecuteSQLDelete},
        {"executeSQLSelect", ExecuteSQLSelect},
        {"executeSQLUpdate", ExecuteSQLUpdate},
        {"executeSQLInsert", ExecuteSQLInsert},
        {"executeSQLQuery", ExecuteSQLQuery},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaDatabaseDefs::AddClass(lua_State* luaVM)
{
    //////////////////////////
    //  Connection class
    //////////////////////////
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "dbConnect");
    lua_classfunction(luaVM, "exec", "dbExec");
    lua_classfunction(luaVM, "prepareString", "dbPrepareString");
    lua_classfunction(luaVM, "query", "dbQuery", OOP_DbQuery);

    lua_registerclass(luaVM, "Connection", "Element");
    //////////////////////////

    //////////////////////////
    //  QueryHandle class
    //////////////////////////
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "poll", "dbPoll");
    lua_classfunction(luaVM, "free", "dbFree");

    lua_registerclass(luaVM, "QueryHandle", NULL, false);
    //////////////////////////
}

int CLuaDatabaseDefs::ExecuteSQLCreateTable(lua_State* luaVM)
{
    SString strTable;
    SString strDefinition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTable);
    argStream.ReadString(strDefinition);

    if (!argStream.HasErrors())
    {
        CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(luaVM);
        CStaticFunctionDefinitions::ExecuteSQLCreateTable(strTable, strDefinition);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLDropTable(lua_State* luaVM)
{
    SString strTable;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTable);

    if (!argStream.HasErrors())
    {
        CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(luaVM);
        CStaticFunctionDefinitions::ExecuteSQLDropTable(strTable);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLDelete(lua_State* luaVM)
{
    SString strTable;
    SString strDefinition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTable);
    argStream.ReadString(strDefinition);

    if (!argStream.HasErrors())
    {
        CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(luaVM);
        if (CStaticFunctionDefinitions::ExecuteSQLDelete(strTable, strDefinition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();
            m_pScriptDebugging->LogError(luaVM, "%s", strError.c_str());

            lua_pushstring(luaVM, strError);
            lua_pushboolean(luaVM, false);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLInsert(lua_State* luaVM)
{
    SString strTable;
    SString strDefinition;
    SString strColumns;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTable);
    argStream.ReadString(strDefinition);
    argStream.ReadString(strColumns, "");

    if (!argStream.HasErrors())
    {
        CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(luaVM);
        if (CStaticFunctionDefinitions::ExecuteSQLInsert(strTable, strDefinition, strColumns))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();
            m_pScriptDebugging->LogError(luaVM, "%s", strError.c_str());

            lua_pushstring(luaVM, strError);
            lua_pushboolean(luaVM, false);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

//
// Db function callbacks
//

// Handle directing callback for DbQuery
void CLuaDatabaseDefs::DbQueryCallback(CDbJobData* pJobData, void* pContext)
{
    CLuaCallback* pLuaCallback = (CLuaCallback*)pContext;
    if (pJobData->stage == EJobStage::RESULT)
    {
        if (pLuaCallback)
        {
            if (pLuaCallback->IsValid())
                pLuaCallback->Call();
            else
                g_pGame->GetDatabaseManager()->QueryFree(pJobData);
        }
    }
    g_pGame->GetLuaCallbackManager()->DestroyCallback(pLuaCallback);
}

// Handle callback for DbExec
void CLuaDatabaseDefs::DbExecCallback(CDbJobData* pJobData, void* pContext)
{
    assert(pContext == NULL);
    if (pJobData->stage >= EJobStage::RESULT && pJobData->result.status == EJobResult::FAIL)
    {
        if (!pJobData->result.bErrorSuppressed)
            m_pScriptDebugging->LogWarning(pJobData->m_LuaDebugInfo, "dbExec failed; (%d) %s", pJobData->result.uiErrorCode, *pJobData->result.strReason);
    }
}

// Handle callback for DbFree
void CLuaDatabaseDefs::DbFreeCallback(CDbJobData* pJobData, void* pContext)
{
    assert(pContext == NULL);
    if (pJobData->stage >= EJobStage::RESULT && pJobData->result.status == EJobResult::FAIL)
    {
        if (!pJobData->result.bErrorSuppressed)
            m_pScriptDebugging->LogWarning(pJobData->m_LuaDebugInfo, "dbFree failed; (%d) %s", pJobData->result.uiErrorCode, *pJobData->result.strReason);
    }
}

std::variant<CDatabaseConnectionElement*, bool> CLuaDatabaseDefs::DbConnect(lua_State* luaVM, std::string type, std::string host,
                                                                            std::optional<std::string> username, std::optional<std::string> password,
                                                                            std::optional<std::string> options, std::optional<CLuaFunctionRef> callback)
{
    if (!username.has_value())
        username = "";
    if (!password.has_value())
        password = "";
    if (!options.has_value())
        options = "";

    CResource* resource = &lua_getownerresource(luaVM);

    if (type == "sqlite" && !host.empty())
    {
        // If path starts with :/ then use global database directory
        if (!host.rfind(":/", 0))
        {
            host = host.substr(1);
            if (!IsValidFilePath(host.c_str()))
            {
                SString err("host path %s not valid", host.c_str());
                throw LuaFunctionError(err.c_str(), false);
            }

            host = PathJoin(g_pGame->GetConfig()->GetGlobalDatabasesPath(), host);
        }
        else
        {
            std::string absPath;

            // Parse path
            CResource* pathResource = resource;
            if (!CResourceManager::ParseResourcePathInput(host, pathResource, &absPath))
            {
                SString err("host path %s not found", host.c_str());
                throw LuaFunctionError(err.c_str(), false);
            }

            host = absPath;
            auto [status, err] = CheckCanModifyOtherResource(resource, pathResource);
            if (!status)
                throw LuaFunctionError(err.c_str(), false);
        }
    }
    
    if (type == "mysql")
        resource->SetUsingDbConnectMysql(true);

    // Add logging options
    bool    loggingEnabled;
    std::string logTag;
    std::string queueName;
    // Set default values if required
    GetOption<CDbOptionsMap>(*options, "log", loggingEnabled, 1);
    GetOption<CDbOptionsMap>(*options, "tag", logTag, "script");
    GetOption<CDbOptionsMap>(*options, "queue", queueName, (type == "mysql") ? host.c_str() : DB_SQLITE_QUEUE_NAME_DEFAULT);
    SetOption<CDbOptionsMap>(*options, "log", loggingEnabled);
    SetOption<CDbOptionsMap>(*options, "tag", logTag);
    SetOption<CDbOptionsMap>(*options, "queue", queueName);

    const auto CreateConnection = [](CResource* resource, const SConnectionHandle& handle)
        -> CDatabaseConnectionElement*
    {
        // Use an element to wrap the connection for auto disconnected when the resource stops
        // Don't set a parent because the element should not be accessible from other resources
        auto*          element = new CDatabaseConnectionElement(nullptr, handle);
        CElementGroup* group = resource->GetElementGroup();
        if (group)
            group->Add(element);
        return element;
    };

    if (callback.has_value())
    {
        const auto taskFunc = [type = type, host, username = username.value(), password = password.value(), options = options.value()]
        {
            return g_pGame->GetDatabaseManager()->Connect(
                type,
                host,
                username,
                password,
                options
            );
        };
        const auto readyFunc = [CreateConnection = CreateConnection, resource = resource, luaFunctionRef = callback.value()](const SConnectionHandle& handle)
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
            if (!luaMain)
                return;

            CLuaArguments arguments;

            if (handle == INVALID_DB_HANDLE)
            {
                auto lastError = g_pGame->GetDatabaseManager()->GetLastErrorMessage();
                m_pScriptDebugging->LogCustom(luaMain->GetVM(), lastError.c_str());
                arguments.PushBoolean(false);
            }

            arguments.PushElement(CreateConnection(resource, handle));
            arguments.Call(luaMain, luaFunctionRef);
        };

        CLuaShared::GetAsyncTaskScheduler()->PushTask(taskFunc, readyFunc);
        return true;
    }

    // Do connect
    SConnectionHandle connection = g_pGame->GetDatabaseManager()->Connect(
        type, host, *username, *password, *options
    );
    if (connection == INVALID_DB_HANDLE)
        throw LuaFunctionError(g_pGame->GetDatabaseManager()->GetLastErrorMessage().c_str(), false);

    return CreateConnection(resource, connection);
}

// This method has an OOP counterpart - don't forget to update the OOP code too!
int CLuaDatabaseDefs::DbQuery(lua_State* luaVM)
{
    //  handle dbQuery ( [ function callbackFunction, [ table callbackArguments, ] ] element connection, string query, ... )
    CLuaFunctionRef             iLuaFunction;
    CLuaArguments               callbackArgs;
    CDatabaseConnectionElement* pElement;
    SString                     strQuery;
    CLuaArguments               Args;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsFunction())
    {
        argStream.ReadFunction(iLuaFunction);
        if (argStream.NextIsTable())
        {
            argStream.ReadLuaArgumentsTable(callbackArgs);
        }
    }
    argStream.ReadUserData(pElement);
    argStream.ReadString(strQuery);
    argStream.ReadLuaArguments(Args);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager()->QueryStart(pElement->GetConnectionHandle(), strQuery, &Args);
        if (!pJobData)
        {
            if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed())
                m_pScriptDebugging->LogWarning(luaVM, "%s failed; %s", lua_tostring(luaVM, lua_upvalueindex(1)),
                                               *g_pGame->GetDatabaseManager()->GetLastErrorMessage());
            lua_pushboolean(luaVM, false);
            return 1;
        }
        // Make callback function if required
        if (VERIFY_FUNCTION(iLuaFunction))
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                CLuaArguments Arguments;
                Arguments.PushDbQuery(pJobData);
                Arguments.PushArguments(callbackArgs);
                pJobData->SetCallback(DbQueryCallback, g_pGame->GetLuaCallbackManager()->CreateCallback(pLuaMain, iLuaFunction, Arguments));
            }
        }
        // Add debug info incase query result does not get collected
        pJobData->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
        lua_pushquery(luaVM, pJobData);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::OOP_DbQuery(lua_State* luaVM)
{
    //  handle dbQuery ( [ function callbackFunction, [ table callbackArguments, ] ] element connection, string query, ... )
    CLuaFunctionRef             iLuaFunction;
    CLuaArguments               callbackArgs;
    CDatabaseConnectionElement* pElement;
    SString                     strQuery;
    CLuaArguments               Args;

    CScriptArgReader argStream(luaVM);

    argStream.ReadUserData(pElement);
    if (argStream.NextIsFunction())
    {
        argStream.ReadFunction(iLuaFunction);
        if (argStream.NextIsTable())
        {
            argStream.ReadLuaArgumentsTable(callbackArgs);
        }
    }
    argStream.ReadString(strQuery);
    argStream.ReadLuaArguments(Args);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager()->QueryStart(pElement->GetConnectionHandle(), strQuery, &Args);
        if (!pJobData)
        {
            if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed())
                m_pScriptDebugging->LogWarning(luaVM, "%s failed; %s", lua_tostring(luaVM, lua_upvalueindex(1)),
                                               *g_pGame->GetDatabaseManager()->GetLastErrorMessage());
            lua_pushboolean(luaVM, false);
            return 1;
        }
        // Make callback function if required
        if (VERIFY_FUNCTION(iLuaFunction))
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                CLuaArguments Arguments;
                Arguments.PushDbQuery(pJobData);
                Arguments.PushArguments(callbackArgs);
                pJobData->SetCallback(CLuaDatabaseDefs::DbQueryCallback, g_pGame->GetLuaCallbackManager()->CreateCallback(pLuaMain, iLuaFunction, Arguments));
            }
        }
        // Add debug info incase query result does not get collected
        pJobData->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
        lua_pushquery(luaVM, pJobData);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::DbExec(lua_State* luaVM)
{
    //  bool dbExec ( element connection, string query, ... )
    CDatabaseConnectionElement* pElement;
    SString                     strQuery;
    CLuaArguments               Args;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strQuery);
    argStream.ReadLuaArguments(Args);

    if (!argStream.HasErrors())
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager()->Exec(pElement->GetConnectionHandle(), strQuery, &Args);
        if (!pJobData)
        {
            if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed())
                m_pScriptDebugging->LogError(luaVM, "%s failed: %s", lua_tostring(luaVM, lua_upvalueindex(1)),
                                             *g_pGame->GetDatabaseManager()->GetLastErrorMessage());
            lua_pushboolean(luaVM, false);
            return 1;
        }
        // Add callback for tracking errors
        pJobData->SetCallback(DbExecCallback, NULL);
        pJobData->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::DbFree(lua_State* luaVM)
{
    //  bool dbFree ( handle query )
    CDbJobData* pJobData;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pJobData);

    if (!argStream.HasErrors())
    {
        // Add callback for tracking errors
        pJobData->SetCallback(DbFreeCallback, NULL);
        pJobData->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));

        bool bResult = g_pGame->GetDatabaseManager()->QueryFree(pJobData);
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

// Plop single set of registry results into a lua table
void PushRegistryResultTable(lua_State* luaVM, const CRegistryResultData* Result)
{
    lua_newtable(luaVM);
    int i = 0;
    for (CRegistryResultIterator iter = Result->begin(); iter != Result->end(); ++iter, ++i)
    {
        const CRegistryResultRow& row = *iter;
        lua_pushnumber(luaVM, i + 1);
        lua_newtable(luaVM);
        for (int j = 0; j < Result->nColumns; j++)
        {
            const CRegistryResultCell& cell = row[j];

            // Push the column name
            lua_pushlstring(luaVM, Result->ColNames[j].c_str(), Result->ColNames[j].size());
            switch (cell.nType)            // push the value with the right type
            {
                case SQLITE_INTEGER:
                    lua_pushnumber(luaVM, static_cast<double>(cell.nVal));
                    break;
                case SQLITE_FLOAT:
                    lua_pushnumber(luaVM, cell.fVal);
                    break;
                case SQLITE_BLOB:
                    lua_pushlstring(luaVM, (const char*)cell.pVal, cell.nLength);
                    break;
                case SQLITE_TEXT:
                    lua_pushlstring(luaVM, (const char*)cell.pVal, cell.nLength - 1);
                    break;
                default:
                    lua_pushboolean(luaVM, false);
            }
            lua_settable(luaVM, -3);
        }
        lua_settable(luaVM, -3);
    }
}

int CLuaDatabaseDefs::DbPoll(lua_State* luaVM)
{
    //  table dbPoll ( handle query, int timeout )
    CDbJobData* pJobData;
    int         iTimeout;
    bool        bMultipleResults;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pJobData);
    argStream.ReadNumber(iTimeout);
    argStream.ReadBool(bMultipleResults, false);

    if (!argStream.HasErrors())
    {
        // Extra input validation
        if (pJobData->stage > EJobStage::RESULT)
            argStream.SetCustomError("Previous dbPoll already returned result");
        if (pJobData->result.bIgnoreResult)
            argStream.SetCustomError("Cannot call dbPoll after dbFree");
    }

    if (!argStream.HasErrors())
    {
        if (!g_pGame->GetDatabaseManager()->QueryPoll(pJobData, iTimeout))
        {
            // Not ready yet
            lua_pushnil(luaVM);
            return 1;
        }

        if (pJobData->result.status == EJobResult::FAIL)
        {
            if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed())
                m_pScriptDebugging->LogWarning(luaVM, "%s failed; %s", lua_tostring(luaVM, lua_upvalueindex(1)),
                                               *g_pGame->GetDatabaseManager()->GetLastErrorMessage());
            lua_pushboolean(luaVM, false);
            lua_pushnumber(luaVM, pJobData->result.uiErrorCode);
            lua_pushstring(luaVM, pJobData->result.strReason);
            return 3;
        }

        const CRegistryResultData* Result = pJobData->result.registryResult->GetThis();

        if (!bMultipleResults)
        {
            // Single result (from first statement)
            PushRegistryResultTable(luaVM, Result);
            lua_pushnumber(luaVM, Result->uiNumAffectedRows);
            lua_pushnumber(luaVM, static_cast<double>(Result->ullLastInsertId));
            return 3;
        }
        else
        {
            // One or more results (from multiple statements)
            lua_newtable(luaVM);
            for (int i = 0; Result; Result = Result->pNextResult, i++)
            {
                lua_pushnumber(luaVM, i + 1);
                lua_newtable(luaVM);
                {
                    lua_pushnumber(luaVM, 1);            // [1] - table of result rows
                    PushRegistryResultTable(luaVM, Result);
                    lua_settable(luaVM, -3);

                    lua_pushnumber(luaVM, 2);            // [2] - NumAffectedRows
                    lua_pushnumber(luaVM, Result->uiNumAffectedRows);
                    lua_settable(luaVM, -3);

                    lua_pushnumber(luaVM, 3);            // [3] - LastInsertId
                    lua_pushnumber(luaVM, static_cast<double>(Result->ullLastInsertId));
                    lua_settable(luaVM, -3);
                }
                lua_settable(luaVM, -3);
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::DbPrepareString(lua_State* luaVM)
{
    //  string dbPrepareString ( element connection, string query, ... )
    CDatabaseConnectionElement* pElement;
    SString                     strQuery;
    CLuaArguments               Args;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strQuery);
    argStream.ReadLuaArguments(Args);

    if (!argStream.HasErrors())
    {
        SString strResult = g_pGame->GetDatabaseManager()->PrepareString(pElement->GetConnectionHandle(), strQuery, &Args);
        SString strError = g_pGame->GetDatabaseManager()->GetLastErrorMessage();
        if (!strResult.empty() || strError.empty())
        {
            lua_pushstring(luaVM, strResult);
            return 1;
        }
        if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed())
            argStream.SetCustomError(strError);
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLQuery(lua_State* luaVM)
{
    SString strQuery;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strQuery);

    if (!argStream.HasErrors())
    {
        CLuaArguments   Args;
        CRegistryResult Result;

        Args.ReadArguments(luaVM, 2);

        CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(luaVM);
        if (CStaticFunctionDefinitions::ExecuteSQLQuery(strQuery, &Args, &Result))
        {
            lua_newtable(luaVM);
            int i = 0;
            for (CRegistryResultIterator iter = Result->begin(); iter != Result->end(); ++iter, ++i)
            {
                const CRegistryResultRow& row = *iter;
                // for ( int i = 0; i < Result.nRows; i++ ) {
                lua_newtable(luaVM);                     // new table
                lua_pushnumber(luaVM, i + 1);            // row index number (starting at 1, not 0)
                lua_pushvalue(luaVM, -2);                // value
                lua_settable(luaVM, -4);                 // refer to the top level table
                for (int j = 0; j < Result->nColumns; j++)
                {
                    const CRegistryResultCell& cell = row[j];
                    if (cell.nType == SQLITE_NULL)
                        continue;

                    // Push the column name
                    lua_pushlstring(luaVM, Result->ColNames[j].c_str(), Result->ColNames[j].size());
                    switch (cell.nType)            // push the value with the right type
                    {
                        case SQLITE_INTEGER:
                            lua_pushnumber(luaVM, static_cast<double>(cell.nVal));
                            break;
                        case SQLITE_FLOAT:
                            lua_pushnumber(luaVM, cell.fVal);
                            break;
                        case SQLITE_BLOB:
                            lua_pushlstring(luaVM, (const char*)cell.pVal, cell.nLength);
                            break;
                        case SQLITE_TEXT:
                            lua_pushlstring(luaVM, (const char*)cell.pVal, cell.nLength - 1);
                            break;
                        default:
                            lua_pushnil(luaVM);
                    }
                    lua_settable(luaVM, -3);
                }
                lua_pop(luaVM, 1);            // pop the inner table
            }
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();
            m_pScriptDebugging->LogError(luaVM, "%s", strError.c_str());

            lua_pushstring(luaVM, strError);
            lua_pushboolean(luaVM, false);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLSelect(lua_State* luaVM)
{
    SString      strTable;
    SString      strColumns;
    SString      strWhere;
    SString      strError;
    unsigned int uiLimit;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTable);
    argStream.ReadString(strColumns);
    argStream.ReadString(strWhere, "");
    argStream.ReadNumber(uiLimit, 0);

    if (!argStream.HasErrors())
    {
        CRegistryResult Result;
        CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(luaVM);
        if (CStaticFunctionDefinitions::ExecuteSQLSelect(strTable, strColumns, strWhere, uiLimit, &Result))
        {
            lua_newtable(luaVM);
            int i = 0;
            for (CRegistryResultIterator iter = Result->begin(); iter != Result->end(); ++iter, ++i)
            {
                const CRegistryResultRow& row = *iter;
                //            for ( int i = 0; i < Result.nRows; i++ ) {
                lua_newtable(luaVM);                     // new table
                lua_pushnumber(luaVM, i + 1);            // row index number (starting at 1, not 0)
                lua_pushvalue(luaVM, -2);                // value
                lua_settable(luaVM, -4);                 // refer to the top level table
                for (int j = 0; j < Result->nColumns; j++)
                {
                    const CRegistryResultCell& cell = row[j];
                    if (cell.nType == SQLITE_NULL)
                        continue;

                    // Push the column name
                    lua_pushlstring(luaVM, Result->ColNames[j].c_str(), Result->ColNames[j].size());
                    switch (cell.nType)            // push the value with the right type
                    {
                        case SQLITE_INTEGER:
                            lua_pushnumber(luaVM, static_cast<double>(cell.nVal));
                            break;
                        case SQLITE_FLOAT:
                            lua_pushnumber(luaVM, cell.fVal);
                            break;
                        case SQLITE_BLOB:
                            lua_pushlstring(luaVM, (const char*)cell.pVal, cell.nLength);
                            break;
                        case SQLITE_TEXT:
                            lua_pushlstring(luaVM, (const char*)cell.pVal, cell.nLength - 1);
                            break;
                        default:
                            lua_pushnil(luaVM);
                    }
                    lua_settable(luaVM, -3);
                }
                lua_pop(luaVM, 1);            // pop the inner table
            }
            return 1;
        }
        else
        {
            strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();
            m_pScriptDebugging->LogError(luaVM, "%s", strError.c_str());

            lua_pushstring(luaVM, strError);
            lua_pushboolean(luaVM, false);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDatabaseDefs::ExecuteSQLUpdate(lua_State* luaVM)
{
    SString strTable;
    SString strSet;
    SString strWhere;
    SString strError;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTable);
    argStream.ReadString(strSet);
    argStream.ReadString(strWhere, "");

    if (!argStream.HasErrors())
    {
        CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(luaVM);
        if (CStaticFunctionDefinitions::ExecuteSQLUpdate(strTable, strSet, strWhere))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
        {
            strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();
            m_pScriptDebugging->LogError(luaVM, "%s", strError.c_str());
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    lua_pushstring(luaVM, strError.c_str());
    return 2;
}
