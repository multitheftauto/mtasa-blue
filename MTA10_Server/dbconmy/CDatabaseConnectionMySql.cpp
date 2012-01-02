/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseConnectionMySql.cpp
*  PURPOSE:     MySql connection handler
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
using namespace std;
#include "../mods/deathmatch/logic/CRegistry.h"
#include "../mods/deathmatch/logic/CDatabaseType.h"
#include <mysql.h>

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql
//
//
///////////////////////////////////////////////////////////////
class CDatabaseConnectionMySql : public CDatabaseConnection
{
public:
    ZERO_ON_NEW
                        CDatabaseConnectionMySql   ( CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions );
    virtual             ~CDatabaseConnectionMySql  ( void );

    // CDatabaseConnection
    virtual bool            IsValid                 ( void );
    virtual const SString&  GetLastErrorMessage     ( void );
    virtual uint            GetLastErrorCode        ( void );
    virtual uint            GetNumAffectedRows      ( void );
    virtual void            AddRef                  ( void );
    virtual void            Release                 ( void );
    virtual bool            Query                   ( const SString& strQuery, CRegistryResult& registryResult );
    virtual void            Flush                   ( void );
    virtual int             GetShareCount           ( void )        { return m_iRefCount; }

    // CDatabaseConnectionMySql
    void                    SetLastError            ( uint uiCode, const SString& strMessage );
    bool                    QueryInternal           ( const SString& strQuery, CRegistryResult& registryResult );
    void                    BeginAutomaticTransaction ( void );
    void                    EndAutomaticTransaction ( void );
    int                     ConvertToSqliteType     ( enum_field_types type );

    int                     m_iRefCount;
    CDatabaseType*          m_pManager;
    MYSQL*                  m_handle;
    bool                    m_bOpened;
    SString                 m_strLastErrorMessage;
    uint                    m_uiLastErrorCode;
    uint                    m_uiNumAffectedRows;
    int                     m_bAutomaticReconnect;
    int                     m_bAutomaticTransactionsEnabled;
    bool                    m_bInAutomaticTransaction;
    CTickCount              m_AutomaticTransactionStartTime;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
MTAEXPORT CDatabaseConnection* NewDatabaseConnectionMySql ( CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
{
    return new CDatabaseConnectionMySql ( pManager, strHost, strUsername, strPassword, strOptions );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::CDatabaseConnectionMySql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseConnectionMySql::CDatabaseConnectionMySql ( CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
    : m_iRefCount ( 1 )
    , m_pManager ( pManager )
{
    // Parse options string
    CArgMap optionsMap ( "=", ";" );
    optionsMap.SetFromString ( strOptions );
    optionsMap.Get ( "autoreconnect", m_bAutomaticReconnect, 1 );
    optionsMap.Get ( "batch", m_bAutomaticTransactionsEnabled, 1 );

    SString strHostname;
    SString strDatabaseName;
    int iPort = 0;
    SString strUnixSocket;
    ulong ulClientFlags = 0;

    // Parse host string
    CArgMap argMap ( "=", ";" );
    argMap.SetFromString ( strHost );
    argMap.Get ( "dbname", strDatabaseName, "" );
    argMap.Get ( "host", strHostname, "localhost" );
    argMap.Get ( "port", iPort, 0 );
    argMap.Get ( "unix_socket", strUnixSocket, "" );

    m_handle = mysql_init ( NULL );
    if ( m_handle )
    {
        my_bool reconnect = m_bAutomaticReconnect;
        mysql_options ( m_handle, MYSQL_OPT_RECONNECT, &reconnect );

        if ( mysql_real_connect ( m_handle, strHostname, strUsername, strPassword, strDatabaseName, iPort, strUnixSocket, ulClientFlags ) )
            m_bOpened = true;
        else
        {
            SetLastError ( mysql_errno( m_handle ), mysql_error ( m_handle ) );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::CDatabaseConnectionMySql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseConnectionMySql::~CDatabaseConnectionMySql ( void )
{
    Flush ();

    if ( m_handle != NULL )
    {
        mysql_close ( m_handle );
        m_handle = 0;
        m_bOpened = false;
    }

    m_pManager->NotifyConnectionDeleted ( this );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::AddRef
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionMySql::AddRef ( void )
{
    m_iRefCount++;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::Release
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionMySql::Release ( void )
{
    if ( --m_iRefCount > 0 )
    {
        m_pManager->NotifyConnectionChanged ( this );
        return;
    }

    // Do disconnect
    delete this;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::IsValid
//
// Returns false if connection created all wrong
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionMySql::IsValid ( void )
{
    return m_bOpened;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::GetLastErrorMessage
//
// Only valid when IsValid() or Query() returns false
//
///////////////////////////////////////////////////////////////
const SString& CDatabaseConnectionMySql::GetLastErrorMessage ( void )
{
    return m_strLastErrorMessage;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::GetLastErrorCode
//
// Only valid when IsValid() or Query() returns false
//
///////////////////////////////////////////////////////////////
uint CDatabaseConnectionMySql::GetLastErrorCode ( void )
{
    return m_uiLastErrorCode;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::SetLastError
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionMySql::SetLastError ( uint uiCode, const SString& strMessage )
{
    m_uiLastErrorCode = uiCode;
    m_strLastErrorMessage = strMessage;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::GetNumAffectedRows
//
// Only valid when Query() returns true
//
///////////////////////////////////////////////////////////////
uint CDatabaseConnectionMySql::GetNumAffectedRows ( void )
{
    return m_uiNumAffectedRows;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::Query
//
//
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionMySql::Query ( const SString& strQuery, CRegistryResult& registryResult )
{
    BeginAutomaticTransaction ();
    return QueryInternal ( strQuery, registryResult );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::QueryInternal
//
// Return false on error
// Return true with datum in registryResult on success
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionMySql::QueryInternal ( const SString& strQuery, CRegistryResult& registryResult )
{
    CRegistryResult* pResult = &registryResult;

    int status = mysql_real_query ( m_handle, strQuery, static_cast < unsigned long > ( strQuery.length () ) );
    if ( status )
    {
        SetLastError ( mysql_errno( m_handle ), mysql_error ( m_handle ) );
        return false;
    }


    MYSQL_RES* res = mysql_store_result ( m_handle );

    m_uiNumAffectedRows = static_cast < uint > ( mysql_affected_rows ( m_handle ) );

    if ( !res )
    {
        // No result data
        registryResult = CRegistryResult ();
    }
    else
    {
        // Get column names
        pResult->nColumns = mysql_num_fields ( res );
        pResult->ColNames.clear ();
        for ( int i = 0; i < pResult->nColumns; i++ )
        {
            mysql_field_seek ( res, i );
            MYSQL_FIELD* field = mysql_fetch_field ( res );
            pResult->ColNames.push_back ( field->name );
        }

        // Fetch the rows
        pResult->nRows = 0;
        pResult->Data.clear ();
        MYSQL_ROW inRow;
        while ( ( inRow = mysql_fetch_row ( res ) ) )
        {
            ulong* inLengths = mysql_fetch_lengths ( res );

            pResult->Data.push_back ( vector < CRegistryResultCell > ( pResult->nColumns ) );
            vector < CRegistryResultCell > & outRow = *(pResult->Data.end () - 1);
            for ( int i = 0; i < pResult->nColumns; i++ )
            {
                CRegistryResultCell& cell = outRow[i];
                mysql_field_seek ( res, i );
                MYSQL_FIELD* field = mysql_fetch_field ( res );
                cell.nType = ConvertToSqliteType ( field->type );

                char* inData = inRow[i];
                ulong inLength = inLengths[i];

                if ( !inData )
                    cell.nType = SQLITE_NULL;

                switch ( cell.nType )
                {
                    case SQLITE_NULL:
                        break;
                    case SQLITE_INTEGER:
                        cell.nVal = atol ( inData );
                        break;
                    case SQLITE_FLOAT:
                        cell.fVal = (float)atof ( inData );
                        break;
                    case SQLITE_BLOB:
                        cell.nLength = inLength;
                        if ( cell.nLength == 0 )
                        {
                            cell.pVal = NULL;
                        }
                        else
                        {
                            cell.pVal = new unsigned char [ cell.nLength ];
                            memcpy ( cell.pVal, inData, cell.nLength );
                        }
                        break;
                    default:
                        cell.nLength = inLength + 1;
                        cell.pVal = new unsigned char [ cell.nLength ];
                        memcpy ( cell.pVal, inData, cell.nLength );
                        break;
                }
            }
            pResult->nRows++;
        }

        mysql_free_result ( res );
    }

    // Discard extra results from a called procedure
    while ( mysql_next_result ( m_handle ) == 0 )
        if ( MYSQL_RES* result = mysql_store_result ( m_handle ) )
            mysql_free_result ( result );

    return true;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::BeginAutomaticTransaction
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionMySql::BeginAutomaticTransaction ( void )
{
    if ( m_bInAutomaticTransaction )
    {
        // If it's been a little while since this transaction was started, consider renewing it
        if ( ( CTickCount::Now () - m_AutomaticTransactionStartTime ).ToLongLong () > 500 )
            EndAutomaticTransaction ();
    }
    if ( !m_bInAutomaticTransaction && m_bAutomaticTransactionsEnabled )
    {
        m_bInAutomaticTransaction = true;
        m_AutomaticTransactionStartTime = CTickCount::Now ();
        CRegistryResult dummy;
        QueryInternal ( "SET autocommit = 0", dummy );
    }
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::EndAutomaticTransaction
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionMySql::EndAutomaticTransaction ( void )
{
    if ( m_bInAutomaticTransaction )
    {
        m_bInAutomaticTransaction = false;
        CRegistryResult dummy;
        QueryInternal ( "SET autocommit = 1", dummy );
    }
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::Flush
//
// Flush caches
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionMySql::Flush ( void )
{
    EndAutomaticTransaction ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionMySql::ConvertToSqliteType
//
// Convert MYSQL_TYPE into SQLITE equivalent
//
///////////////////////////////////////////////////////////////
int CDatabaseConnectionMySql::ConvertToSqliteType ( enum_field_types type )
{
    switch ( type )
    {
        case MYSQL_TYPE_NULL:
            return SQLITE_NULL;

	    case MYSQL_TYPE_LONGLONG:
	    case MYSQL_TYPE_INT24:
	    case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_LONG:
            return SQLITE_INTEGER;

	    case MYSQL_TYPE_FLOAT:
        case MYSQL_TYPE_DOUBLE:
            return SQLITE_FLOAT;

        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
            return SQLITE_BLOB;

        default:
            return SQLITE_TEXT;
    };
}
