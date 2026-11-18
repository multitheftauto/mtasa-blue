/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseTypePostgreSql.cpp
 *  PURPOSE:     PostgreSQL connection maker
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

//
// dbConnect("postgresql", ...) support. Works like the MySQL type (CDatabaseTypeMySql.cpp): the
// connection code lives in a separate library (dbconpg), loaded the first time a script connects,
// and connections can be shared with the "share" option.
//
// Unlike MySQL, the main thread never waits on the database server (see CDatabaseManager.cpp):
// - destroyElement on a connection returns at once; queued queries still run before it closes.
// - With the "async_connect=1" option, dbConnect returns at once and connects in the background.
//   A failed connect is logged, and queries on that connection fail with the connect error.
// Queries already ran on a worker thread (one per host) for every database type.
//

#include "StdInc.h"
#include "CDatabaseType.h"
#include "CDatabaseManager.h"
#include "CDynamicLibrary.h"
#include "CPerfStatManager.h"
#include <sqlite/sqlite3.h>

#define LIB_DBCONPG "dbconpg" MTA_LIB_SUFFIX MTA_LIB_EXTENSION

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql
//
//
///////////////////////////////////////////////////////////////
class CDatabaseTypePostgreSql : public CDatabaseType
{
public:
    ZERO_ON_NEW
    CDatabaseTypePostgreSql();
    virtual ~CDatabaseTypePostgreSql();

    // CDatabaseType
    virtual SString              GetDataSourceTag();
    virtual CDatabaseConnection* Connect(const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strDriverOptions);
    virtual void                 NotifyConnectionDeleted(CDatabaseConnection* pConnection);
    virtual void                 NotifyConnectionChanged(CDatabaseConnection* pConnection);

    // CDatabaseTypePostgreSql
    CDatabaseConnection* CallNewDatabaseConnectionPostgreSql(CDatabaseType* pManager, const SString& strHost, const SString& strUsername,
                                                             const SString& strPassword, const SString& strOptions);
    void                 UpdateStats();

    CDynamicLibrary                         m_DbconpgLib;
    NewDatabaseConnectionPostgreSql_t*      m_pfnNewDatabaseConnection;
    std::map<SString, CDatabaseConnection*> m_SharedConnectionMap;
    std::set<CDatabaseConnection*>          m_AllConnectionMap;
    SString                                 m_strStatsKeyHead;
};

///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseType* NewDatabaseTypePostgreSql()
{
    return new CDatabaseTypePostgreSql();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::CDatabaseTypePostgreSql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypePostgreSql::CDatabaseTypePostgreSql()
{
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::~CDatabaseTypePostgreSql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypePostgreSql::~CDatabaseTypePostgreSql()
{
    assert(m_SharedConnectionMap.empty());
    assert(m_AllConnectionMap.empty());
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::GetDataSourceTag
//
// Return database type as a string
//
///////////////////////////////////////////////////////////////
SString CDatabaseTypePostgreSql::GetDataSourceTag()
{
    return "postgresql";
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::NotifyConnectionDeleted
//
// Remove connection from internal lists
//
///////////////////////////////////////////////////////////////
void CDatabaseTypePostgreSql::NotifyConnectionDeleted(CDatabaseConnection* pConnection)
{
    g_pStats->iDbConnectionCount--;
    assert(MapContains(m_AllConnectionMap, pConnection));
    MapRemove(m_AllConnectionMap, pConnection);
    MapRemoveByValue(m_SharedConnectionMap, pConnection);

    UpdateStats();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::NotifyConnectionChanged
//
// Update things that need to know
//
///////////////////////////////////////////////////////////////
void CDatabaseTypePostgreSql::NotifyConnectionChanged(CDatabaseConnection* pConnection)
{
    UpdateStats();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::Connect
//
// strHost is like "dbname=testdb;host=127.0.0.1;port=5432"
// strUsername is username
// strPassword is password
// strOptions contains key=value pairs separated by semicolons
//
//      Options are:
//          share=1     // Share this connection with anything else (defaults to share=0)
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseTypePostgreSql::Connect(const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions)
{
    // Parse options
    bool bShareConnection = false;
    GetOption<CDbOptionsMap>(strOptions, "share", bShareConnection, false);

    CDatabaseConnection* pConnection = NULL;

    // Can we share a connection?
    if (!bShareConnection)
    {
        // No sharing so create a new connection
        pConnection = CallNewDatabaseConnectionPostgreSql(this, strHost, strUsername, strPassword, strOptions);
        if (pConnection)
            pConnection->m_strOtherTag = strHost + "%" + strUsername + "%" + strOptions;
    }
    else
    {
        // Yes sharing, so make a key
        SString strShareKey = strHost + "%" + strUsername + "%" + strOptions;

        // Look for a match
        pConnection = MapFindRef(m_SharedConnectionMap, strShareKey);
        if (!pConnection)
        {
            // No match, so create a new connection
            pConnection = CallNewDatabaseConnectionPostgreSql(this, strHost, strUsername, strPassword, strOptions);
            if (pConnection)
                MapSet(m_SharedConnectionMap, strShareKey, pConnection);
        }
        else
        {
            // Yes match, so add a ref to existing connection
            pConnection->AddRef();
        }
    }

    if (pConnection)
        MapInsert(m_AllConnectionMap, pConnection);

    // For stats
    SString strQueueName;
    GetOption<CDbOptionsMap>(strOptions, "queue", strQueueName);
    m_strStatsKeyHead = SString("dbcon postgresql [%s] ", *strQueueName);
    UpdateStats();

    return pConnection;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::CallNewDatabaseConnectionPostgreSql
//
// Load, find and call NewDatabaseConnectionPostgreSql.
// If the library or one of its DLLs is missing, CDynamicLibrary::Load logs why to the server log.
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseTypePostgreSql::CallNewDatabaseConnectionPostgreSql(CDatabaseType* pManager, const SString& strHost, const SString& strUsername,
                                                                                  const SString& strPassword, const SString& strOptions)
{
    if (!m_DbconpgLib.IsLoaded())
    {
        SString strServerPath = g_pServerInterface->GetModManager()->GetServerPath();
        m_DbconpgLib.Load(PathJoin(strServerPath, SERVER_BIN_PATH_MOD, LIB_DBCONPG));
        {
            const auto procAddr = m_DbconpgLib.GetProcedureAddress("NewDatabaseConnectionPostgreSql");
            static_assert(sizeof(m_pfnNewDatabaseConnection) == sizeof(procAddr), "Unexpected function pointer size");
            std::memcpy(&m_pfnNewDatabaseConnection, &procAddr, sizeof(m_pfnNewDatabaseConnection));
        }
    }

    if (!m_pfnNewDatabaseConnection)
        return NULL;

    CDatabaseConnection* pConnection = m_pfnNewDatabaseConnection(pManager, strHost, strUsername, strPassword, strOptions);

    if (pConnection)
        g_pStats->iDbConnectionCount++;

    return pConnection;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypePostgreSql::UpdateStats
//
// Tracking connections
//
///////////////////////////////////////////////////////////////
void CDatabaseTypePostgreSql::UpdateStats()
{
    // Remove all lines with this key
    CPerfStatDebugTable::GetSingleton()->RemoveLines(m_strStatsKeyHead + "*");

    int                            iIndex = 0;
    std::set<CDatabaseConnection*> unsharedConnectionMap = m_AllConnectionMap;

    // Add shared info
    for (std::map<SString, CDatabaseConnection*>::iterator iter = m_SharedConnectionMap.begin(); iter != m_SharedConnectionMap.end(); ++iter)
    {
        const SString&       strShareKey = iter->first;
        CDatabaseConnection* pConnection = iter->second;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton()->UpdateLine(m_strStatsKeyHead + SString("%d", iIndex++), 0, "Database connection: postgresql (shared)",
                                                        *strShareKey, *SString("Share count: %d", pConnection->GetShareCount()), NULL);

        // Update unshared map for the second part
        MapRemove(unsharedConnectionMap, pConnection);
    }

    // Add unshared info
    for (std::set<CDatabaseConnection*>::iterator iter = unsharedConnectionMap.begin(); iter != unsharedConnectionMap.end(); ++iter)
    {
        CDatabaseConnection* pConnection = *iter;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton()->UpdateLine(m_strStatsKeyHead + SString("%d", iIndex++), 0, "Database connection: postgresql (unshared)",
                                                        *pConnection->m_strOtherTag, *SString("Refs: %d", pConnection->GetShareCount()), NULL);
    }
}

///////////////////////////////////////////////////////////////
//
// PostgreSqlEscape
//
// Escape a string for use inside E'...'
//
// InsertQueryArguments has no connection, so PQescapeStringConn can't be used here. E'...' strings
// treat backslash as an escape character whatever the server's standard_conforming_strings setting
// is, so escaping both backslash and quote is always correct. This relies on the connection using
// UTF-8 (dbconpg forces client_encoding=UTF8), where these bytes can't appear inside a multibyte
// character.
//
// Zero bytes are copied as they are. dbconpg refuses any query that contains one, because libpq
// would cut the query short at that point.
//
///////////////////////////////////////////////////////////////
static void PostgreSqlEscape(SString& strOutput, const char* szContent, uint uiLength)
{
    for (uint i = 0; i < uiLength; i++)
    {
        const char c = szContent[i];
        if (c == '\\')
            strOutput += "\\\\";
        else if (c == '\'')
            strOutput += "''";
        else
            strOutput += c;
    }
}

///////////////////////////////////////////////////////////////
//
// InsertQueryArgumentsPostgreSql
//
// Insert arguments and apply PostgreSQL escapement.
//
// Same placeholders as MySQL: ? is a quoted value, ?? is inserted without quotes (for table and
// column names, ASC/DESC...). Booleans become '1'/'0': a quoted literal PostgreSQL converts to the
// column's type, so they work for both boolean and integer columns.
//
///////////////////////////////////////////////////////////////
SString InsertQueryArgumentsPostgreSql(const SString& strQuery, CLuaArguments* pArgs)
{
    SString strParsedQuery;

    // Walk through the query and replace the variable placeholders with the actual variables
    unsigned int uiLen = strQuery.length();
    unsigned int a = 0;
    for (unsigned int i = 0; i < uiLen; i++)
    {
        if (strQuery[i] != SQL_VARIABLE_PLACEHOLDER)
        {
            // If we found a normal character, copy it into the destination buffer
            strParsedQuery += strQuery[i];
        }
        else
        {
            // Use ?? for unquoted strings
            bool bUnquotedStrings = strQuery[i + 1] == SQL_VARIABLE_PLACEHOLDER;
            if (bUnquotedStrings)
                i++;

            // If the placeholder is found, replace it with the variable
            CLuaArgument* pArgument = (*pArgs)[a++];

            // Check the type of the argument and convert it to a string we can process
            uint type = pArgument ? pArgument->GetType() : LUA_TNONE;
            if (type == LUA_TBOOLEAN)
            {
                strParsedQuery += (pArgument->GetBoolean()) ? "'1'" : "'0'";
            }
            else if (type == LUA_TNUMBER)
            {
                double dNumber = pArgument->GetNumber();
                if (dNumber == floor(dNumber))
                    strParsedQuery += SString("%lld", (long long)dNumber);
                else
                    strParsedQuery += SString("%f", dNumber);
            }
            else if (type == LUA_TSTRING)
            {
                if (!bUnquotedStrings)
                    strParsedQuery += "E'";
                PostgreSqlEscape(strParsedQuery, pArgument->GetString().c_str(), pArgument->GetString().length());
                if (!bUnquotedStrings)
                    strParsedQuery += '\'';
            }
            else if (type == LUA_TNIL)
            {
                // Nil becomes NULL
                strParsedQuery += "NULL";
            }
            else
            {
                // If we don't have any content, put just output 2 quotes to indicate an empty variable
                strParsedQuery += "\'\'";
            }
        }
    }

    return strParsedQuery;
}

///////////////////////////////////////////////////////////////
//
// InsertQueryArgumentsPostgreSql
//
// Insert arguments and apply PostgreSQL escapement
//
///////////////////////////////////////////////////////////////
SString InsertQueryArgumentsPostgreSql(const char* szQuery, va_list vl)
{
    SString strParsedQuery;
    for (unsigned int i = 0; szQuery[i] != '\0'; i++)
    {
        if (szQuery[i] != SQL_VARIABLE_PLACEHOLDER)
        {
            strParsedQuery += szQuery[i];
        }
        else
        {
            // Use ?? for unquoted strings
            bool bUnquotedStrings = szQuery[i + 1] == SQL_VARIABLE_PLACEHOLDER;
            if (bUnquotedStrings)
                i++;

            switch (va_arg(vl, int))
            {
                case SQLITE_INTEGER:
                {
                    int iValue = va_arg(vl, int);
                    strParsedQuery += SString("%d", iValue);
                }
                break;

                case SQLITE_INTEGER64:
                {
                    long long int llValue = va_arg(vl, long long int);
                    strParsedQuery += SString("%lld", llValue);
                }
                break;

                case SQLITE_FLOAT:
                {
                    double fValue = va_arg(vl, double);
                    strParsedQuery += SString("%f", fValue);
                }
                break;

                case SQLITE_TEXT:
                {
                    const char* szValue = va_arg(vl, const char*);
                    assert(szValue);
                    if (!bUnquotedStrings)
                        strParsedQuery += "E'";
                    PostgreSqlEscape(strParsedQuery, szValue, strlen(szValue));
                    if (!bUnquotedStrings)
                        strParsedQuery += '\'';
                }
                break;

                case SQLITE_BLOB:
                {
                    strParsedQuery += "CANT_DO_BLOBS_M8";
                }
                break;

                case SQLITE_NULL:
                {
                    strParsedQuery += "NULL";
                }
                break;

                default:
                    // someone passed a value without specifying its type
                    assert(0);
                    break;
            }
        }
    }
    va_end(vl);
    return strParsedQuery;
}
