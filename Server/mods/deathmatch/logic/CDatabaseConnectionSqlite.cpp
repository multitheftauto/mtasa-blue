/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseConnectionSqlite.cpp
 *  PURPOSE:     Sqlite connection handler
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDatabaseType.h"
#include "CDatabaseManager.h"
#include "CRegistry.h"
#include "CPerfStatModule.h"
#include "lua/CLuaArguments.h"

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite
//
//
///////////////////////////////////////////////////////////////
class CDatabaseConnectionSqlite : public CDatabaseConnection
{
public:
    ZERO_ON_NEW
    CDatabaseConnectionSqlite(CDatabaseType* pManager, const SString& strPath, const SString& strOptions);
    virtual ~CDatabaseConnectionSqlite();

    // CDatabaseConnection
    virtual bool           IsValid();
    virtual const SString& GetLastErrorMessage();
    virtual uint           GetLastErrorCode();
    virtual void           AddRef();
    virtual void           Release();
    virtual bool           Query(const SString& strQuery, CRegistryResult& registryResult);
    virtual void           Flush();
    virtual int            GetShareCount() { return m_iRefCount; }

    // CDatabaseConnectionSqlite
    void SetLastError(uint uiCode, const SString& strMessage);
    bool QueryInternal(const SString& strQuery, CRegistryResult& registryResult);
    void BeginAutomaticTransaction();
    void EndAutomaticTransaction();

    int            m_iRefCount;
    CDatabaseType* m_pManager;
    sqlite3*       m_handle;
    bool           m_bOpened;
    SString        m_strLastErrorMessage;
    uint           m_uiLastErrorCode;
    bool           m_bAutomaticTransactionsEnabled;
    bool           m_bInAutomaticTransaction;
    CTickCount     m_AutomaticTransactionStartTime;
    bool           m_bMultipleStatements;
};

///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseConnection* NewDatabaseConnectionSqlite(CDatabaseType* pManager, const SString& strPath, const SString& strOptions)
{
    return new CDatabaseConnectionSqlite(pManager, strPath, strOptions);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::CDatabaseConnectionSqlite
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseConnectionSqlite::CDatabaseConnectionSqlite(CDatabaseType* pManager, const SString& strPath, const SString& strOptions)
    : m_iRefCount(1), m_pManager(pManager)
{
    g_pStats->iDbConnectionCount++;

    // Parse options string
    GetOption<CDbOptionsMap>(strOptions, "batch", m_bAutomaticTransactionsEnabled, 1);
    GetOption<CDbOptionsMap>(strOptions, "multi_statements", m_bMultipleStatements, 0);

    MakeSureDirExists(strPath);
    if (sqlite3_open(strPath, &m_handle))
    {
        SetLastError(sqlite3_errcode(m_handle), sqlite3_errmsg(m_handle));
    }
    else
    {
        m_bOpened = true;
    }
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::CDatabaseConnectionSqlite
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseConnectionSqlite::~CDatabaseConnectionSqlite()
{
    Flush();

    if (m_bOpened)
        sqlite3_close(m_handle);

    m_pManager->NotifyConnectionDeleted(this);
    g_pStats->iDbConnectionCount--;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::AddRef
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::AddRef()
{
    m_iRefCount++;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::Release
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::Release()
{
    if (--m_iRefCount > 0)
    {
        m_pManager->NotifyConnectionChanged(this);
        return;
    }

    // Do disconnect
    delete this;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::IsValid
//
// Returns false if connection created all wrong
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionSqlite::IsValid()
{
    return m_bOpened;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::GetLastErrorMessage
//
// Only valid when IsValid() or Query() returns false
//
///////////////////////////////////////////////////////////////
const SString& CDatabaseConnectionSqlite::GetLastErrorMessage()
{
    return m_strLastErrorMessage;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::GetLastErrorMessage
//
// Only valid when IsValid() or Query() returns false
//
///////////////////////////////////////////////////////////////
uint CDatabaseConnectionSqlite::GetLastErrorCode()
{
    return m_uiLastErrorCode;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::SetLastError
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::SetLastError(uint uiCode, const SString& strMessage)
{
    m_uiLastErrorCode = uiCode;
    m_strLastErrorMessage = strMessage;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::Query
//
//
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionSqlite::Query(const SString& strQuery, CRegistryResult& registryResult)
{
    // VACUUM query does not work with transactions
    if (strQuery.BeginsWithI("VACUUM"))
        EndAutomaticTransaction();
    else
        BeginAutomaticTransaction();
    return QueryInternal(strQuery, registryResult);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::QueryInternal
//
// Return false on error
// Return true with datum in registryResult on success
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionSqlite::QueryInternal(const SString& strQuery, CRegistryResult& registryResult)
{
    const char*          szQuery = strQuery;
    CRegistryResultData* pResult = registryResult->GetThis();

    while (true)
    {
        // Prepare the query
        sqlite3_stmt* pStmt;
        if (sqlite3_prepare(m_handle, szQuery, strlen(szQuery) + 1, &pStmt, &szQuery) != SQLITE_OK)
        {
            SetLastError(sqlite3_errcode(m_handle), sqlite3_errmsg(m_handle));
            return false;
        }

        // Get column names
        pResult->nColumns = sqlite3_column_count(pStmt);
        pResult->ColNames.clear();
        for (int i = 0; i < pResult->nColumns; i++)
        {
            pResult->ColNames.push_back(sqlite3_column_name(pStmt, i));
        }

        // Fetch the rows
        pResult->nRows = 0;
        pResult->Data.clear();
        int status;
        while ((status = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            pResult->Data.push_back(std::vector<CRegistryResultCell>(pResult->nColumns));
            std::vector<CRegistryResultCell>& row = pResult->Data.back();
            for (int i = 0; i < pResult->nColumns; i++)
            {
                CRegistryResultCell& cell = row[i];
                cell.nType = sqlite3_column_type(pStmt, i);
                switch (cell.nType)
                {
                    case SQLITE_NULL:
                        break;
                    case SQLITE_INTEGER:
                        cell.nVal = sqlite3_column_int64(pStmt, i);
                        break;
                    case SQLITE_FLOAT:
                        cell.fVal = (float)sqlite3_column_double(pStmt, i);
                        break;
                    case SQLITE_BLOB:
                        cell.nLength = sqlite3_column_bytes(pStmt, i);
                        if (cell.nLength == 0)
                        {
                            cell.pVal = NULL;
                        }
                        else
                        {
                            cell.pVal = new unsigned char[cell.nLength];
                            memcpy(cell.pVal, sqlite3_column_blob(pStmt, i), cell.nLength);
                        }
                        break;
                    default:
                        cell.nLength = sqlite3_column_bytes(pStmt, i) + 1;
                        cell.pVal = new unsigned char[cell.nLength];
                        memcpy(cell.pVal, sqlite3_column_text(pStmt, i), cell.nLength);
                        break;
                }
            }
            pResult->nRows++;
        }

        // Did we leave the fetching loop because of an error?
        if (status != SQLITE_DONE)
        {
            SetLastError(sqlite3_errcode(m_handle), sqlite3_errmsg(m_handle));
            sqlite3_finalize(pStmt);
            return false;
        }

        // All done
        sqlite3_finalize(pStmt);

        // Number of affects rows/num of rows like MySql
        pResult->uiNumAffectedRows = pResult->nRows ? pResult->nRows : sqlite3_changes(m_handle);

        // Last insert id
        pResult->ullLastInsertId = sqlite3_last_insert_rowid(m_handle);

        // See if should process next statement
        if (!m_bMultipleStatements || !szQuery || strlen(szQuery) < 2)
            break;

        pResult->pNextResult = new CRegistryResultData();
        pResult = pResult->pNextResult;
    }

    return true;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::BeginAutomaticTransaction
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::BeginAutomaticTransaction()
{
    if (m_bInAutomaticTransaction)
    {
        // If it's been a little while since this transaction was started, consider renewing it
        if ((CTickCount::Now() - m_AutomaticTransactionStartTime).ToLongLong() > 1500)
            EndAutomaticTransaction();
    }
    if (!m_bInAutomaticTransaction && m_bAutomaticTransactionsEnabled)
    {
        m_bInAutomaticTransaction = true;
        m_AutomaticTransactionStartTime = CTickCount::Now();
        CRegistryResult dummy;
        QueryInternal("BEGIN TRANSACTION", dummy);
    }
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::EndAutomaticTransaction
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::EndAutomaticTransaction()
{
    if (m_bInAutomaticTransaction)
    {
        m_bInAutomaticTransaction = false;
        CRegistryResult dummy;
        QueryInternal("END TRANSACTION", dummy);
    }
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::Flush
//
// Flush caches
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::Flush()
{
    EndAutomaticTransaction();
}

///////////////////////////////////////////////////////////////
//
// SqliteEscape
//
// Apply Sqlite escapement to a string
//
///////////////////////////////////////////////////////////////
static void SqliteEscape(SString& strOutput, const char* szContent, uint uiLength)
{
    for (uint i = 0; i < uiLength; i++)
    {
        const char c = szContent[i];
        if (c == '\'')
            strOutput += '\'';
        strOutput += c;
    }
}

///////////////////////////////////////////////////////////////
//
// InsertQueryArgumentsSqlite
//
// Insert arguments and apply Sqlite escapement
//
///////////////////////////////////////////////////////////////
SString InsertQueryArgumentsSqlite(const SString& strQuery, CLuaArguments* pArgs)
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
                strParsedQuery += (pArgument->GetBoolean()) ? "1" : "0";
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
                // Copy the string into the query, and escape '
                if (!bUnquotedStrings)
                    strParsedQuery += '\'';
                SqliteEscape(strParsedQuery, pArgument->GetString().c_str(), pArgument->GetString().length());
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
// InsertQueryArgumentsSqlite
//
// Insert arguments and apply Sqlite escapement
//
///////////////////////////////////////////////////////////////
SString InsertQueryArgumentsSqlite(const char* szQuery, va_list vl)
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
                        strParsedQuery += '\'';
                    SqliteEscape(strParsedQuery, szValue, strlen(szValue));
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
