/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        dbconpg/CDatabaseConnectionPostgreSql.cpp
 *  PURPOSE:     PostgreSQL connection handler
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

//
// PostgreSQL support for dbConnect("postgresql", ...).
//
// It lives in its own library (like dbconmy for MySQL) so the server only loads libpq and its
// dependencies when a script actually connects to PostgreSQL. Results are converted into the same
// SQLITE_* typed cells the other backends produce, so dbPoll and the Lua side need no changes.
//
// Differences from the MySQL backend, and why:
// - No automatic transactions ("batch" option). In PostgreSQL any failed statement aborts the whole
//   open transaction, so batching like MySQL does would silently roll back every successful write
//   made earlier in the same batch. Each query commits on its own instead.
// - One statement per query unless multi_statements=1. PQexec would run several statements by
//   default; using PQexecParams (extended protocol) rejects them, which matches MySQL's default and
//   limits what an injected string can do.
// - No last insert id. PostgreSQL has no equivalent of mysql_insert_id; scripts use RETURNING.
// - Reconnecting (autoreconnect=1) happens before the next query, never by re-sending a query that
//   was already sent, so the first query after the server drops the connection fails. While the
//   server stays down, queries fail at once for a few seconds between attempts instead of each one
//   waiting for connect_timeout.
//

#include "StdInc.h"
using namespace std;
#include "../mods/deathmatch/logic/CRegistry.h"
#include "../mods/deathmatch/logic/CDatabaseType.h"
#include <libpq-fe.h>

// Built-in type OIDs. These are fixed in PostgreSQL's catalog (pg_type.dat) and never change,
// so hardcoding them avoids depending on the server headers (catalog/pg_type_d.h).
namespace PgTypeOid
{
    constexpr Oid BOOL = 16;
    constexpr Oid BYTEA = 17;
    constexpr Oid INT8 = 20;
    constexpr Oid INT2 = 21;
    constexpr Oid INT4 = 23;
    constexpr Oid OID = 26;
    constexpr Oid FLOAT4 = 700;
    constexpr Oid FLOAT8 = 701;
    constexpr Oid NUMERIC = 1700;
}  // namespace PgTypeOid

// Error code reported to scripts for every PostgreSQL failure. CDatabaseConnection only carries a
// numeric code, but PostgreSQL error codes (SQLSTATE) are 5 characters like "42P01", so the SQLSTATE
// goes at the start of the error message instead.
#define PG_ERROR_CODE 1

// After a failed reconnect, queries fail without trying again for this long (see EnsureConnected)
#define PG_RECONNECT_RETRY_DELAY_MS 5000

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql
//
//
///////////////////////////////////////////////////////////////
class CDatabaseConnectionPostgreSql : public CDatabaseConnection
{
public:
    ZERO_ON_NEW
    CDatabaseConnectionPostgreSql(CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword,
                                  const SString& strOptions);
    virtual ~CDatabaseConnectionPostgreSql();

    // CDatabaseConnection
    virtual bool           IsValid();
    virtual const SString& GetLastErrorMessage();
    virtual uint           GetLastErrorCode();
    virtual void           AddRef();
    virtual void           Release();
    virtual bool           Query(const SString& strQuery, CRegistryResult& registryResult);
    virtual void           Flush();
    virtual int            GetShareCount() { return m_iRefCount; }

    // CDatabaseConnectionPostgreSql
    void SetLastError(uint uiCode, const SString& strMessage);
    void SetLastErrorFromConnection();
    void SetLastErrorFromResult(const PGresult* res);
    bool EnsureConnected();
    bool QuerySingle(const SString& strQuery, CRegistryResultData* pResult);
    bool QueryMultiple(const SString& strQuery, CRegistryResultData* pResult);
    bool StoreResult(const PGresult* res, CRegistryResultData* pResult);
    int  ConvertToSqliteType(Oid type);

    int            m_iRefCount;
    CDatabaseType* m_pManager;
    PGconn*        m_handle;
    bool           m_bOpened;
    SString        m_strLastErrorMessage;
    uint           m_uiLastErrorCode;
    int            m_bAutomaticReconnect;
    int            m_bMultipleStatements;
    bool           m_bLastReconnectFailed;
    CTickCount     m_LastReconnectTime;
    SString        m_strLastReconnectError;
};

///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
MTAEXPORT CDatabaseConnection* NewDatabaseConnectionPostgreSql(CDatabaseType* pManager, const SString& strHost, const SString& strUsername,
                                                               const SString& strPassword, const SString& strOptions)
{
    return new CDatabaseConnectionPostgreSql(pManager, strHost, strUsername, strPassword, strOptions);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::CDatabaseConnectionPostgreSql
//
// strHost is like "dbname=testdb;host=127.0.0.1;port=5432;sslmode=require"
//
///////////////////////////////////////////////////////////////
CDatabaseConnectionPostgreSql::CDatabaseConnectionPostgreSql(CDatabaseType* pManager, const SString& strHost, const SString& strUsername,
                                                             const SString& strPassword, const SString& strOptions)
    : m_iRefCount(1), m_pManager(pManager)
{
    int iUseSSL;

    // Parse options string
    CArgMap optionsMap("=", ";");
    optionsMap.SetFromString(strOptions);
    optionsMap.Get("autoreconnect", m_bAutomaticReconnect, 1);
    optionsMap.Get("multi_statements", m_bMultipleStatements, 0);
    optionsMap.Get("use_ssl", iUseSSL, 0);

    // Parse host string. Only these keys are passed to libpq, so a script can't set arbitrary
    // connection parameters (e.g. client_encoding, which the query escaping relies on).
    SString strDatabaseName;
    SString strHostname;
    SString strPort;
    SString strSSLMode;
    SString strConnectTimeout;
    SString strApplicationName;

    CArgMap argMap("=", ";");
    argMap.SetFromString(strHost);
    argMap.Get("dbname", strDatabaseName, "");
    argMap.Get("host", strHostname, "localhost");
    argMap.Get("port", strPort, "");
    // "prefer" is libpq's own default: use TLS when the server offers it. use_ssl=1 makes it mandatory,
    // matching the meaning of use_ssl for MySQL connections.
    argMap.Get("sslmode", strSSLMode, iUseSSL ? "require" : "prefer");
    // libpq waits forever by default. Connecting blocks this connection's job queue, so give up after a while.
    argMap.Get("connect_timeout", strConnectTimeout, "10");
    argMap.Get("application_name", strApplicationName, "MTA:SA Server");

    std::vector<const char*> keywords;
    std::vector<const char*> values;
    auto                     AddParam = [&](const char* szKeyword, const SString& strValue)
    {
        if (!strValue.empty())
        {
            keywords.push_back(szKeyword);
            values.push_back(strValue.c_str());
        }
    };
    AddParam("host", strHostname);
    AddParam("port", strPort);
    AddParam("dbname", strDatabaseName);
    AddParam("user", strUsername);
    AddParam("password", strPassword);
    AddParam("sslmode", strSSLMode);
    AddParam("connect_timeout", strConnectTimeout);
    AddParam("application_name", strApplicationName);
    // MTA strings are UTF-8. InsertQueryArgumentsPostgreSql's escaping is only safe for encodings
    // where a quote or backslash byte can't be part of a multibyte character, which UTF-8 guarantees.
    // Pushed directly: AddParam keeps c_str() pointers, so passing it a literal would store a pointer
    // into a temporary SString that is gone before PQconnectdbParams reads it.
    keywords.push_back("client_encoding");
    values.push_back("UTF8");
    keywords.push_back(nullptr);
    values.push_back(nullptr);

    // expand_dbname=0: dbname is a plain name. Otherwise a value containing '=' would be parsed as a
    // full connection string and could override the other parameters.
    m_handle = PQconnectdbParams(keywords.data(), values.data(), 0);
    if (m_handle && PQstatus(m_handle) == CONNECTION_OK)
        m_bOpened = true;
    else
        SetLastErrorFromConnection();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::~CDatabaseConnectionPostgreSql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseConnectionPostgreSql::~CDatabaseConnectionPostgreSql()
{
    Flush();

    if (m_handle)
    {
        PQfinish(m_handle);
        m_handle = nullptr;
        m_bOpened = false;
    }

    m_pManager->NotifyConnectionDeleted(this);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::AddRef
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionPostgreSql::AddRef()
{
    m_iRefCount++;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::Release
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionPostgreSql::Release()
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
// CDatabaseConnectionPostgreSql::IsValid
//
// Returns false if connection created all wrong
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionPostgreSql::IsValid()
{
    return m_bOpened;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::GetLastErrorMessage
//
// Only valid when IsValid() or Query() returns false
//
///////////////////////////////////////////////////////////////
const SString& CDatabaseConnectionPostgreSql::GetLastErrorMessage()
{
    return m_strLastErrorMessage;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::GetLastErrorCode
//
// Only valid when IsValid() or Query() returns false
//
///////////////////////////////////////////////////////////////
uint CDatabaseConnectionPostgreSql::GetLastErrorCode()
{
    return m_uiLastErrorCode;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::SetLastError
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionPostgreSql::SetLastError(uint uiCode, const SString& strMessage)
{
    m_uiLastErrorCode = uiCode;
    // libpq messages end with a newline, which would leave a blank line in the server log
    m_strLastErrorMessage = strMessage.TrimEnd("\n").TrimEnd("\r");
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::SetLastErrorFromConnection
//
// For errors with no result, like failing to connect or losing the connection
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionPostgreSql::SetLastErrorFromConnection()
{
    // PQconnectdbParams only returns null when out of memory
    SetLastError(PG_ERROR_CODE, m_handle ? PQerrorMessage(m_handle) : "Out of memory");
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::SetLastErrorFromResult
//
// Error message is "[SQLSTATE] message", e.g. "[42P01] ERROR:  relation "foo" does not exist"
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionPostgreSql::SetLastErrorFromResult(const PGresult* res)
{
    if (!res)
    {
        SetLastErrorFromConnection();
        return;
    }

    const char* szSQLState = PQresultErrorField(res, PG_DIAG_SQLSTATE);
    const char* szMessage = PQresultErrorMessage(res);
    if (szSQLState)
        SetLastError(PG_ERROR_CODE, SString("[%s] %s", szSQLState, szMessage));
    else
        SetLastError(PG_ERROR_CODE, szMessage);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::EnsureConnected
//
// Reconnect before a query if the connection was lost (server restart, network drop...).
// This only runs before sending, so a query that was already sent is never sent twice.
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionPostgreSql::EnsureConnected()
{
    if (PQstatus(m_handle) == CONNECTION_OK)
        return true;

    if (m_bAutomaticReconnect)
    {
        // While the database is down, each reconnect attempt can take up to connect_timeout. Retrying
        // for every queued query would make the queue (and the memory it holds) grow without limit, so
        // fail queries straight away for a few seconds after a failed attempt.
        if (m_bLastReconnectFailed && (CTickCount::Now() - m_LastReconnectTime).ToLongLong() < PG_RECONNECT_RETRY_DELAY_MS)
        {
            SetLastError(PG_ERROR_CODE, m_strLastReconnectError);
            return false;
        }

        PQreset(m_handle);
        m_LastReconnectTime = CTickCount::Now();
        m_bLastReconnectFailed = (PQstatus(m_handle) != CONNECTION_OK);
        if (!m_bLastReconnectFailed)
            return true;

        SetLastErrorFromConnection();
        m_strLastReconnectError = m_strLastErrorMessage;
        return false;
    }

    SetLastErrorFromConnection();
    return false;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::Query
//
// Return false on error
// Return true with datum in registryResult on success
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionPostgreSql::Query(const SString& strQuery, CRegistryResult& registryResult)
{
    // libpq takes the query as a C string, so everything after a zero byte would be silently dropped.
    // That could turn "DELETE FROM ?? WHERE id=?" into "DELETE FROM t" if a script passed "t\0" for ??.
    // PostgreSQL text can't hold zero bytes anyway, so refuse the whole query.
    if (strQuery.find('\0') != SString::npos)
    {
        SetLastError(PG_ERROR_CODE, "Query contains a zero byte (\\0), which PostgreSQL text does not support");
        return false;
    }

    if (!EnsureConnected())
        return false;

    CRegistryResultData* pResult = registryResult->GetThis();
    if (m_bMultipleStatements)
        return QueryMultiple(strQuery, pResult);
    return QuerySingle(strQuery, pResult);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::QuerySingle
//
// PQexecParams with no parameters uses the extended query protocol, which refuses more than one
// statement ("cannot insert multiple commands into a prepared statement").
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionPostgreSql::QuerySingle(const SString& strQuery, CRegistryResultData* pResult)
{
    PGresult* res = PQexecParams(m_handle, strQuery, 0, nullptr, nullptr, nullptr, nullptr, 0);
    bool      bOk = StoreResult(res, pResult);
    if (!bOk)
        SetLastErrorFromResult(res);
    PQclear(res);
    return bOk;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::QueryMultiple
//
// multi_statements=1: send the whole string with the simple query protocol and return one result per
// statement, chained through pNextResult like the MySQL backend does.
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionPostgreSql::QueryMultiple(const SString& strQuery, CRegistryResultData* pResult)
{
    if (!PQsendQuery(m_handle, strQuery))
    {
        SetLastErrorFromConnection();
        return false;
    }

    bool bOk = true;
    bool bFirst = true;
    // Results must be read until PQgetResult returns null, even after an error, or the connection
    // stays busy and the next query fails
    while (PGresult* res = PQgetResult(m_handle))
    {
        if (bOk)
        {
            if (!bFirst)
            {
                pResult->pNextResult = new CRegistryResultData();
                pResult = pResult->pNextResult;
            }
            bFirst = false;

            if (!StoreResult(res, pResult))
            {
                SetLastErrorFromResult(res);
                bOk = false;
            }
        }
        PQclear(res);
    }

    return bOk;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::StoreResult
//
// Copy one PostgreSQL result into pResult. Returns false if the result is an error.
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionPostgreSql::StoreResult(const PGresult* res, CRegistryResultData* pResult)
{
    if (!res)
        return false;

    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK && status != PGRES_EMPTY_QUERY)
        return false;

    // Rows changed by INSERT/UPDATE/DELETE, or rows returned by SELECT. Empty for other commands.
    pResult->uiNumAffectedRows = static_cast<uint>(std::strtoul(PQcmdTuples(const_cast<PGresult*>(res)), nullptr, 10));
    // PostgreSQL has no last insert id. Scripts get it with "INSERT ... RETURNING id".
    pResult->ullLastInsertId = 0;

    if (status != PGRES_TUPLES_OK)
        return true;

    // Get column names
    pResult->nColumns = PQnfields(res);
    pResult->ColNames.clear();
    for (int i = 0; i < pResult->nColumns; i++)
        pResult->ColNames.push_back(PQfname(res, i));

    // Fetch the rows
    const int nRows = PQntuples(res);
    pResult->nRows = 0;
    pResult->Data.clear();
    for (int iRow = 0; iRow < nRows; iRow++)
    {
        pResult->Data.push_back(vector<CRegistryResultCell>(pResult->nColumns));
        vector<CRegistryResultCell>& outRow = pResult->Data.back();
        for (int i = 0; i < pResult->nColumns; i++)
        {
            CRegistryResultCell& cell = outRow[i];

            if (PQgetisnull(res, iRow, i))
            {
                cell.nType = SQLITE_NULL;
                continue;
            }

            const Oid   type = PQftype(res, i);
            const char* inData = PQgetvalue(res, iRow, i);
            const int   inLength = PQgetlength(res, iRow, i);
            cell.nType = ConvertToSqliteType(type);

            switch (cell.nType)
            {
                case SQLITE_INTEGER:
                    // Booleans come back as "t"/"f". Return 1/0 like MySQL does for BOOL (TINYINT(1)).
                    if (type == PgTypeOid::BOOL)
                        cell.nVal = (inData[0] == 't') ? 1 : 0;
                    else
                        cell.nVal = std::atoll(inData);
                    break;
                case SQLITE_FLOAT:
                    cell.fVal = (float)atof(inData);
                    break;
                case SQLITE_BLOB:
                {
                    // bytea comes back as text like "\x0102ff"; decode it to the raw bytes
                    size_t         uiDecodedLength = 0;
                    unsigned char* pDecoded = PQunescapeBytea(reinterpret_cast<const unsigned char*>(inData), &uiDecodedLength);
                    cell.nLength = pDecoded ? static_cast<int>(uiDecodedLength) : 0;
                    if (cell.nLength == 0)
                    {
                        cell.pVal = nullptr;
                    }
                    else
                    {
                        cell.pVal = new unsigned char[cell.nLength];
                        memcpy(cell.pVal, pDecoded, cell.nLength);
                    }
                    if (pDecoded)
                        PQfreemem(pDecoded);
                    break;
                }
                default:
                    // Text, including the zero terminator (see CRegistryResultCell::nLength)
                    cell.nLength = inLength + 1;
                    cell.pVal = new unsigned char[cell.nLength];
                    memcpy(cell.pVal, inData, cell.nLength);
                    break;
            }
        }
        pResult->nRows++;
    }

    return true;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::Flush
//
// Flush caches. Nothing is cached: there are no automatic transactions (see top of file).
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionPostgreSql::Flush()
{
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionPostgreSql::ConvertToSqliteType
//
// Convert a PostgreSQL type OID into the SQLITE equivalent. Anything not listed (text, varchar,
// timestamp, json, uuid...) is returned as text, which is how PostgreSQL sends it.
//
///////////////////////////////////////////////////////////////
int CDatabaseConnectionPostgreSql::ConvertToSqliteType(Oid type)
{
    switch (type)
    {
        case PgTypeOid::BOOL:
        case PgTypeOid::INT2:
        case PgTypeOid::INT4:
        case PgTypeOid::INT8:
        case PgTypeOid::OID:
            return SQLITE_INTEGER;

        case PgTypeOid::FLOAT4:
        case PgTypeOid::FLOAT8:
        case PgTypeOid::NUMERIC:
            return SQLITE_FLOAT;

        case PgTypeOid::BYTEA:
            return SQLITE_BLOB;

        default:
            return SQLITE_TEXT;
    }
}
