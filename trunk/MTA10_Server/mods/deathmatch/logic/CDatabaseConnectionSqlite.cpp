/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseConnectionSqlite.cpp
*  PURPOSE:     Sqlite connection handler
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDatabaseType.h"


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
                        CDatabaseConnectionSqlite   ( CDatabaseType* pManager, const SString& strPath, const SString& strOptions );
    virtual             ~CDatabaseConnectionSqlite  ( void );

    // CDatabaseConnection
    virtual bool            IsValid                 ( void );
    virtual const SString&  GetLastErrorMessage     ( void );
    virtual void            AddRef                  ( void );
    virtual void            Release                 ( void );
    virtual bool            Query                   ( const SString& strQuery, CRegistryResult& registryResult );

    // CDatabaseConnectionSqlite
    void                    SetLastErrorMessage     ( const SString& strMessage, const SString& strQuery );

    int                     m_iRefCount;
    CDatabaseType*          m_pManager;
    sqlite3*                m_handle;
    bool                    m_bOpened;
    SString                 m_strLastErrorMessage;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseConnection* NewDatabaseConnectionSqlite ( CDatabaseType* pManager, const SString& strPath, const SString& strOptions )
{
    return new CDatabaseConnectionSqlite ( pManager, strPath, strOptions );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::CDatabaseConnectionSqlite
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseConnectionSqlite::CDatabaseConnectionSqlite ( CDatabaseType* pManager, const SString& strPath, const SString& strOptions )
    : m_iRefCount ( 1 )
    , m_pManager ( pManager )
{
    MakeSureDirExists ( strPath );
    if ( sqlite3_open ( strPath, &m_handle ) )
    {
        SetLastErrorMessage ( "Could not open SQLite3 database", sqlite3_errmsg ( m_handle ) );
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
CDatabaseConnectionSqlite::~CDatabaseConnectionSqlite ( void )
{
    if ( m_bOpened )
        sqlite3_close ( m_handle );

    m_pManager->NotifyConnectionDeleted ( this );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::AddRef
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::AddRef ( void )
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
void CDatabaseConnectionSqlite::Release ( void )
{
    if ( --m_iRefCount > 0 )
        return;

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
bool CDatabaseConnectionSqlite::IsValid ( void )
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
const SString& CDatabaseConnectionSqlite::GetLastErrorMessage ( void )
{
    return m_strLastErrorMessage;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::SetLastErrorMessage
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionSqlite::SetLastErrorMessage ( const SString& strMessage, const SString& strQuery )
{
    m_strLastErrorMessage = strMessage;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionSqlite::Query
//
// Return false on error
// Return true with datum in registryResult on success
//
///////////////////////////////////////////////////////////////
bool CDatabaseConnectionSqlite::Query ( const SString& strQuery, CRegistryResult& registryResult )
{
    const char* szQuery = strQuery;
    CRegistryResult* pResult = &registryResult;

    // Prepare the query
    sqlite3_stmt* pStmt;
    if ( sqlite3_prepare ( m_handle, szQuery, strlen ( szQuery ) + 1, &pStmt, NULL ) != SQLITE_OK )
    {
        SetLastErrorMessage ( sqlite3_errmsg ( m_handle ), szQuery );
        return false;
    }

    // Get column names
    pResult->nColumns = sqlite3_column_count ( pStmt );
    pResult->ColNames.clear ();
    for ( int i = 0; i < pResult->nColumns; i++ )
    {
        pResult->ColNames.push_back ( sqlite3_column_name ( pStmt, i ) );
    }

    // Fetch the rows
    pResult->nRows = 0;
    pResult->Data.clear ();
    int status;
    while ( (status = sqlite3_step(pStmt)) == SQLITE_ROW )
    {
        pResult->Data.push_back ( vector < CRegistryResultCell > ( pResult->nColumns ) );
        vector < CRegistryResultCell > & row = *(pResult->Data.end () - 1);
        for ( int i = 0; i < pResult->nColumns; i++ )
        {
            CRegistryResultCell& cell = row[i];
            cell.nType = sqlite3_column_type ( pStmt, i );
            switch ( cell.nType )
            {
                case SQLITE_NULL:
                    break;
                case SQLITE_INTEGER:
                    cell.nVal = sqlite3_column_int ( pStmt, i );
                    break;
                case SQLITE_FLOAT:
                    cell.fVal = (float)sqlite3_column_double ( pStmt, i );
                    break;
                case SQLITE_BLOB:
                    cell.nLength = sqlite3_column_bytes ( pStmt, i );
                    if ( cell.nLength == 0 )
                    {
                        cell.pVal = NULL;
                    }
                    else
                    {
                        cell.pVal = new unsigned char [ cell.nLength ];
                        memcpy ( cell.pVal, sqlite3_column_blob ( pStmt, i ), cell.nLength );
                    }
                    break;
                default:
                    cell.nLength = sqlite3_column_bytes ( pStmt, i ) + 1;
                    cell.pVal = new unsigned char [ cell.nLength ];
                    memcpy ( cell.pVal, sqlite3_column_text ( pStmt, i ), cell.nLength );
                    break;
            }
        }
        pResult->nRows++;
    }

    // Did we leave the fetching loop because of an error?
    if ( status != SQLITE_DONE )
    {
        SetLastErrorMessage ( sqlite3_errmsg ( m_handle ), szQuery );
        sqlite3_finalize ( pStmt );
        return false;
    }

    // All done
    sqlite3_finalize ( pStmt );

    return true;
}


///////////////////////////////////////////////////////////////
//
// InsertQueryArgumentsSqlite
//
// Insert arguments and apply sqlite escapement
//
///////////////////////////////////////////////////////////////
SString InsertQueryArgumentsSqlite ( const SString& strQuery, CLuaArguments* pArgs )
{
    SString strParsedQuery;

    // Walk through the query and replace the variable placeholders with the actual variables
    unsigned int uiLen = strQuery.length ();
    unsigned int a = 0, type = 0;
    const char *szContent = NULL;
    char szBuffer[32] = {0};
    for ( unsigned int i = 0; i < uiLen; i++ )
    {
        if ( strQuery.at(i) == SQL_VARIABLE_PLACEHOLDER ) {
            // If the placeholder is found, replace it with the variable
            CLuaArgument *pArgument = (*pArgs)[a++];

            // Check the type of the argument and convert it to a string we can process
            if ( pArgument ) {
                type = pArgument->GetType ();
                if ( type == LUA_TBOOLEAN ) {
                    szContent = ( pArgument->GetBoolean() ) ? "true" : "false";
                } else if ( type == LUA_TNUMBER ) {
                    snprintf ( szBuffer, 31, "%f", pArgument->GetNumber () );
                    szContent = szBuffer;
                } else if ( type == LUA_TSTRING ) {
                    szContent = pArgument->GetString ().c_str ();

                    // If we have a string, add a quote at the beginning too
                    strParsedQuery += '\'';
                }
            }

            // Copy the string into the query, and escape the single quotes as well
            if ( szContent ) {
                for ( unsigned int k = 0; k < strlen ( szContent ); k++ ) {
                    if ( szContent[k] == '\'' )
                        strParsedQuery += '\'';
                    strParsedQuery += szContent[k];
                }
                // If we have a string, add a quote at the end too
                if ( type == LUA_TSTRING ) strParsedQuery += '\'';
            } else {
                // If we don't have any content, put just output 2 quotes to indicate an empty variable
                strParsedQuery += "\'\'";
            }

        } else {
            // If we found a normal character, copy it into the destination buffer
            strParsedQuery += strQuery[i];
        }
    }

    return strParsedQuery;
}
