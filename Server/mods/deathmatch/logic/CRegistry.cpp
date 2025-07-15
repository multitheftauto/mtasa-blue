/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRegistry.cpp
 *  PURPOSE:     SQLite registry abstraction class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRegistry.h"
#include "CDatabaseType.h"
#include "CPerfStatModule.h"
#include "CGame.h"
#include "CMainConfig.h"
#include "Utils.h"

CRegistry::CRegistry(const std::string& strFileName)
{
    m_db = NULL;
    m_bOpened = false;
    m_bInAutomaticTransaction = false;
    m_llSuspendBatchingEndTime = 0;

    Load(strFileName);
}

CRegistry::~CRegistry()
{
    EndAutomaticTransaction();
    CPerfStatSqliteTiming::GetSingleton()->OnSqliteClose(this);
    CLogger::LogPrint("Closing SQLite3 database\n");
    if (m_bOpened)
        sqlite3_close(m_db);
}

bool CRegistry::SetLastErrorMessage(const std::string& strLastErrorMessage, const std::string& strQuery)
{
    m_strLastErrorMessage = strLastErrorMessage;
    m_strLastErrorQuery = strQuery;
    OutputDebugLine(SString("[Database] %s [Query was:%s]", *m_strLastErrorMessage, *m_strLastErrorQuery));
    return false;
}

// Disable automatic transactions for a length of time
void CRegistry::SuspendBatching(uint uiTicks)
{
    m_llSuspendBatchingEndTime = GetTickCount64_() + uiTicks;
    if (uiTicks)
        EndAutomaticTransaction();
}

void CRegistry::Load(const std::string& strFileName)
{
    m_bOpened = false;

    if (!strFileName.empty())
    {
        m_strFileName = strFileName;
        if (sqlite3_open(strFileName.c_str(), &m_db))
        {
            CLogger::ErrorPrintf("Could not open SQLite3 database! (%s)\n", sqlite3_errmsg(m_db));
        }
        else
        {
            m_bOpened = true;
            CPerfStatSqliteTiming::GetSingleton()->OnSqliteOpen(this, strFileName);
        }
    }
}

bool CRegistry::IntegrityCheck()
{
    // Check database integrity
    {
        CRegistryResult result;
        bool            bOk = Query(&result, "PRAGMA integrity_check");

        // Get result as a string
        SString strResult;
        if (result->nRows && result->nColumns)
        {
            CRegistryResultCell& cell = result->Data.front()[0];
            if (cell.nType == SQLITE_TEXT)
                strResult = std::string((const char*)cell.pVal, cell.nLength - 1);
        }

        // Process result
        if (!bOk || !strResult.BeginsWithI("ok"))
        {
            CLogger::ErrorPrintf("%s", *strResult);
            CLogger::ErrorPrintf("%s\n", GetLastError().c_str());
            CLogger::ErrorPrintf("Errors were encountered loading '%s' database\n", *ExtractFilename(PathConform(m_strFileName)));
            CLogger::ErrorPrintf("Maybe now is the perfect time to panic.\n");
            CLogger::ErrorPrintf("See - https://wiki.multitheftauto.com/wiki/fixdb\n");
            CLogger::ErrorPrintf("************************\n");
            return false;
        }
    }

    // Do compact if required
    if (g_pGame->GetConfig()->ShouldCompactInternalDatabases())
    {
        CLogger::LogPrintf("Compacting database '%s' ...\n", *ExtractFilename(PathConform(m_strFileName)));

        CRegistryResult result;
        bool            bOk = Query(&result, "VACUUM");

        // Get result as a string
        SString strResult;
        if (result->nRows && result->nColumns)
        {
            CRegistryResultCell& cell = result->Data.front()[0];
            if (cell.nType == SQLITE_TEXT)
                strResult = std::string((const char*)cell.pVal, cell.nLength - 1);
        }

        // Process result
        if (!bOk)
        {
            CLogger::ErrorPrintf("%s", *strResult);
            CLogger::ErrorPrintf("%s\n", GetLastError().c_str());
            CLogger::ErrorPrintf("Errors were encountered compacting '%s' database\n", *ExtractFilename(PathConform(m_strFileName)));
            CLogger::ErrorPrintf("Maybe now is the perfect time to panic.\n");
            CLogger::ErrorPrintf("See - https://wiki.multitheftauto.com/wiki/fixdb\n");
            CLogger::ErrorPrintf("************************\n");
            // Allow server to continue
        }
    }

    return true;
}

bool CRegistry::Update(const std::string& strTable, const std::string& strSet, const std::string& strWhere)
{
    std::string strQuery = "UPDATE " + strTable + " SET " + strSet;
    if (!strWhere.empty())
        strQuery += " WHERE " + strWhere;

    return Exec(strQuery);
}

void CRegistry::CreateTable(const std::string& strTable, const std::string& strDefinition, bool bSilent)
{
    if (!bSilent)
        CLogger::LogPrintf("Creating new DB table %s\n", strTable.c_str());
    Exec("CREATE TABLE IF NOT EXISTS " + strTable + " ( " + strDefinition + " )");
}

void CRegistry::DropTable(const std::string& strTable)
{
    CLogger::LogPrintf("Dropping DB table %s\n", strTable.c_str());
    Exec("DROP TABLE " + strTable);
}

bool CRegistry::Insert(const std::string& strTable, const std::string& strValues, const std::string& strColumns)
{
    std::string strQuery;

    if (strColumns.empty())
        strQuery = "INSERT INTO " + strTable + " VALUES ( " + strValues + " )";
    else
        strQuery = "INSERT INTO " + strTable + " ( " + strColumns + " ) VALUES ( " + strValues + " )";

    return Exec(strQuery);
}

bool CRegistry::Delete(const std::string& strTable, const std::string& strWhere)
{
    return Exec("DELETE FROM " + strTable + " WHERE " + strWhere);
}

bool CRegistry::Exec(const std::string& strQuery)
{
    if (m_bOpened == false)
    {
        SetLastErrorMessage("SQLite3 was not opened, cannot perform query!", strQuery);
        return false;
    }

    BeginAutomaticTransaction();
    return ExecInternal(strQuery.c_str());
}

bool CRegistry::ExecInternal(const char* szQuery)
{
    TIMEUS startTime = GetTimeUs();
    char*  szErrorMsg = NULL;

    if (sqlite3_exec(m_db, szQuery, NULL, NULL, &szErrorMsg) != SQLITE_OK)
    {
        SetLastErrorMessage(szErrorMsg, szQuery);
        sqlite3_free(szErrorMsg);
        return false;
    }

    CPerfStatSqliteTiming::GetSingleton()->UpdateSqliteTiming(this, szQuery, GetTimeUs() - startTime);
    return true;
}

bool CRegistry::QueryInternal(const char* szQuery, CRegistryResult* ppResult)
{
    TIMEUS startTime = GetTimeUs();

    // Prepare the query
    sqlite3_stmt* pStmt;
    if (sqlite3_prepare(m_db, szQuery, strlen(szQuery) + 1, &pStmt, NULL) != SQLITE_OK)
    {
        SetLastErrorMessage(sqlite3_errmsg(m_db), szQuery);
        return false;
    }

    CRegistryResult& pResult = *ppResult;
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
        pResult->Data.push_back(vector<CRegistryResultCell>(pResult->nColumns));
        vector<CRegistryResultCell>& row = pResult->Data.back();
        for (int i = 0; i < pResult->nColumns; i++)
        {
            CRegistryResultCell& cell = row[i];
            cell.nType = sqlite3_column_type(pStmt, i);
            switch (cell.nType)
            {
                case SQLITE_NULL:
                    break;
                case SQLITE_INTEGER:
                    cell.nVal = sqlite3_column_int(pStmt, i);
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
        SetLastErrorMessage(sqlite3_errmsg(m_db), szQuery);
        sqlite3_finalize(pStmt);
        return false;
    }

    // All done
    sqlite3_finalize(pStmt);

    CPerfStatSqliteTiming::GetSingleton()->UpdateSqliteTiming(this, szQuery, GetTimeUs() - startTime);
    return true;
}

bool CRegistry::Query(const std::string& strQuery, CLuaArguments* pArgs, CRegistryResult* pResult)
{
    std::string strParsedQuery = "";

    if (m_bOpened == false)
    {
        SetLastErrorMessage("SQLite3 was not opened, cannot perform query!", strQuery);
        return false;
    }

    // Walk through the query and replace the variable placeholders with the actual variables
    unsigned int uiLen = strQuery.length();
    unsigned int a = 0, type = 0;
    const char*  szContent = NULL;
    char         szBuffer[32] = {0};
    for (unsigned int i = 0; i < uiLen; i++)
    {
        if (strQuery.at(i) == SQL_VARIABLE_PLACEHOLDER)
        {
            // If the placeholder is found, replace it with the variable
            CLuaArgument* pArgument = (*pArgs)[a++];

            // Check the type of the argument and convert it to a string we can process
            if (pArgument)
            {
                type = pArgument->GetType();
                if (type == LUA_TBOOLEAN)
                {
                    szContent = (pArgument->GetBoolean()) ? "true" : "false";
                }
                else if (type == LUA_TNUMBER)
                {
                    snprintf(szBuffer, 31, "%f", pArgument->GetNumber());
                    szContent = szBuffer;
                }
                else if (type == LUA_TSTRING)
                {
                    szContent = pArgument->GetString().c_str();

                    // If we have a string, add a quote at the beginning too
                    strParsedQuery += '\'';
                }
            }

            // Copy the string into the query, and escape the single quotes as well
            if (szContent)
            {
                for (unsigned int k = 0; szContent[k] != '\0'; k++)
                {
                    if (szContent[k] == '\'')
                        strParsedQuery += '\'';
                    strParsedQuery += szContent[k];
                }
                // If we have a string, add a quote at the end too
                if (type == LUA_TSTRING)
                    strParsedQuery += '\'';
            }
            else
            {
                // If we don't have any content, put just output 2 quotes to indicate an empty variable
                strParsedQuery += "\'\'";
            }
        }
        else
        {
            // If we found a normal character, copy it into the destination buffer
            strParsedQuery += strQuery[i];
        }
    }

    // Catch BEGIN/END/COMMIT TRANSACTION and ignore
    SString strTest = SString(strParsedQuery).ToUpper();
    if (strTest.find("TRANSACTION") != std::string::npos)
    {
        strTest = strTest.Replace("\t", " ").Replace("  ", " ", true).TrimStart(" ").TrimEnd(" ");
        if (strTest.find("BEGIN") == 0 || strTest.find("END") == 0 || strTest.find("COMMIT") == 0)
        {
            return true;
        }
    }

    BeginAutomaticTransaction();
    return QueryInternal(strParsedQuery.c_str(), pResult);
}

bool CRegistry::Select(const std::string& strColumns, const std::string& strTable, const std::string& strWhere, unsigned int uiLimit, CRegistryResult* pResult)
{
    std::string strQuery = "SELECT " + strColumns + " FROM " + strTable;
    if (!strWhere.empty())
        strQuery += " WHERE " + strWhere;
    if (uiLimit > 0)
        strQuery += " LIMIT " + std::to_string(uiLimit);

    if (m_bOpened == false)
    {
        SetLastErrorMessage("SQLite3 was not opened, cannot get value!", strQuery);
        return false;
    }

    // Execute the query
    return QueryInternal(strQuery.c_str(), pResult);
}

void CRegistry::BeginAutomaticTransaction()
{
    if (!m_bInAutomaticTransaction)
    {
        if (m_llSuspendBatchingEndTime)
        {
            if (m_llSuspendBatchingEndTime > GetTickCount64_())
                return;
            m_llSuspendBatchingEndTime = 0;
        }

        m_bInAutomaticTransaction = true;
        CRegistryResult dummy;
        QueryInternal("BEGIN TRANSACTION", &dummy);
    }
}

void CRegistry::EndAutomaticTransaction()
{
    if (m_bInAutomaticTransaction)
    {
        m_bInAutomaticTransaction = false;
        CRegistryResult dummy;
        QueryInternal("END TRANSACTION", &dummy);
    }
}

bool CRegistry::Query(const char* szQuery, ...)
{
    CRegistryResult dummy;
    va_list         vl;
    va_start(vl, szQuery);
    return Query(&dummy, szQuery, vl);
}

bool CRegistry::Query(CRegistryResult* pResult, const char* szQuery, ...)
{
    va_list vl;
    va_start(vl, szQuery);
    return Query(pResult, szQuery, vl);
}

bool CRegistry::Query(CRegistryResult* pResult, const char* szQuery, va_list vl)
{
    // Clear result
    if (pResult)
        *pResult = CRegistryResult();

    if (m_bOpened == false)
    {
        SetLastErrorMessage("SQLite3 was not opened, cannot perform query!", szQuery);
        return false;
    }

    SString strParsedQuery;
    for (unsigned int i = 0; szQuery[i] != '\0'; i++)
    {
        if (szQuery[i] != SQL_VARIABLE_PLACEHOLDER)
        {
            strParsedQuery += szQuery[i];
        }
        else
        {
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
                    strParsedQuery += SString("'%s'", SQLEscape(szValue, true, false).c_str());
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
    // VACUUM query does not work with transactions
    if (strParsedQuery.BeginsWithI("VACUUM"))
        EndAutomaticTransaction();
    else
        BeginAutomaticTransaction();
    return QueryInternal(strParsedQuery.c_str(), pResult);
}
