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
#include "CDatabaseType.h"
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

    // CDatabaseConnectionMySql
    void                    SetLastError            ( uint uiCode, const SString& strMessage );
    int                     ConvertToSqliteType     ( enum_field_types type );

    int                     m_iRefCount;
    CDatabaseType*          m_pManager;
    MYSQL*                  m_handle;
    bool                    m_bOpened;
    SString                 m_strLastErrorMessage;
    uint                    m_uiLastErrorCode;
    uint                    m_uiNumAffectedRows;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseConnection* NewDatabaseConnectionMySql ( CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
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
        my_bool reconnect = false;
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
        return;

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
// Return false on error
// Return true with datum in registryResult on success
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionMySql::Query ( const SString& strQuery, CRegistryResult& registryResult )
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
        return true;
    }

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

    return true;
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


///////////////////////////////////////////////////////////////
//
// MySqlEscape
//
// Apply MySql escapement to a string
//
///////////////////////////////////////////////////////////////
static void MySqlEscape ( SString& strOutput, const char* szContent, uint uiLength )
{
    for ( uint i = 0 ; i < uiLength ; i++ )
    {
        const char c = szContent[i];
        if ( c == '\x00' || c == '\n' || c == '\r' || c == '\\' || c == '\'' || c == '\"' || c == '\x1a' )
            strOutput += '\\';
        strOutput += c;
    }
}


///////////////////////////////////////////////////////////////
//
// InsertQueryArgumentsMySql
//
// Insert arguments and apply MySql escapement
//
///////////////////////////////////////////////////////////////
SString InsertQueryArgumentsMySql ( const SString& strQuery, CLuaArguments* pArgs )
{
    SString strParsedQuery;

    // Walk through the query and replace the variable placeholders with the actual variables
    unsigned int uiLen = strQuery.length ();
    unsigned int a = 0;
    for ( unsigned int i = 0 ; i < uiLen ; i++ )
    {
        if ( strQuery[i] != SQL_VARIABLE_PLACEHOLDER )
        {
            // If we found a normal character, copy it into the destination buffer
            strParsedQuery += strQuery[i];
        }
        else
        {
            // Use ?? for unquoted strings
            bool bUnquotedStrings = strQuery[i+1] == SQL_VARIABLE_PLACEHOLDER;
            if ( bUnquotedStrings )
                i++;

            // If the placeholder is found, replace it with the variable
            CLuaArgument* pArgument = (*pArgs)[a++];

            // Check the type of the argument and convert it to a string we can process
            uint type = pArgument ? pArgument->GetType () : LUA_TNONE;
            if ( type == LUA_TBOOLEAN )
            {
                strParsedQuery += ( pArgument->GetBoolean() ) ? "1" : "0";
            }
            else
            if ( type == LUA_TNUMBER )
            {
                strParsedQuery += SString ( "%f", pArgument->GetNumber () );
            }
            else
            if ( type == LUA_TSTRING )
            {
                // Copy the string into the query, and escape \x00, \n, \r, \, ', " and \x1a
                if ( !bUnquotedStrings ) strParsedQuery += '\'';
                MySqlEscape ( strParsedQuery, pArgument->GetString ().c_str (), pArgument->GetString ().length () );
                if ( !bUnquotedStrings ) strParsedQuery += '\'';
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
// InsertQueryArgumentsMySql
//
// Insert arguments and apply MySql escapement
//
///////////////////////////////////////////////////////////////
SString InsertQueryArgumentsMySql ( const char* szQuery, va_list vl )
{
    SString strParsedQuery;
    for ( unsigned int i = 0 ; i < strlen ( szQuery ) ; i++ )
    {
        if ( szQuery[i] != SQL_VARIABLE_PLACEHOLDER )
        {
            strParsedQuery += szQuery[i];
        }
        else
        {
            // Use ?? for unquoted strings
            bool bUnquotedStrings = szQuery[i+1] == SQL_VARIABLE_PLACEHOLDER;
            if ( bUnquotedStrings )
                i++;

            switch ( va_arg( vl, int ) )
            {
                case SQLITE_INTEGER:
                {
                    int iValue = va_arg( vl, int );
                    strParsedQuery += SString ( "%d", iValue );
                }
                break;

                case SQLITE_FLOAT:
                {
                    double fValue = va_arg( vl, double );
                    strParsedQuery += SString ( "%f", fValue );
                }
                break;

                case SQLITE_TEXT:
                {
                    const char* szValue = va_arg( vl, const char* );
                    assert ( szValue );
                    if ( !bUnquotedStrings ) strParsedQuery += '\'';
                    MySqlEscape ( strParsedQuery, szValue, strlen ( szValue ) );
                    if ( !bUnquotedStrings ) strParsedQuery += '\'';
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
                    assert ( 0 );
                    break;
            }
        }
    }
    va_end ( vl );
    return strParsedQuery;
}
