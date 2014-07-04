/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseTypeMySql.cpp
*  PURPOSE:     MySql connection maker
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDatabaseType.h"
#include "CDynamicLibrary.h"

#ifdef WIN32
    #ifdef MTA_DEBUG
        #define LIB_DBCONMY "dbconmy_d.dll"
    #else
        #define LIB_DBCONMY "dbconmy.dll"
    #endif
#else
    #define LIB_DBCONMY "./dbconmy.so"
#endif


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql
//
//
///////////////////////////////////////////////////////////////
class CDatabaseTypeMySql : public CDatabaseType
{
public:
    ZERO_ON_NEW
                                    CDatabaseTypeMySql         ( void );
    virtual                         ~CDatabaseTypeMySql        ( void );

    // CDatabaseType
    virtual SString                 GetDataSourceTag            ( void );
    virtual CDatabaseConnection*    Connect                     ( const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strDriverOptions );
    virtual void                    NotifyConnectionDeleted     ( CDatabaseConnection* pConnection );
    virtual void                    NotifyConnectionChanged     ( CDatabaseConnection* pConnection );

    // CDatabaseTypeMySql
    CDatabaseConnection*            CallNewDatabaseConnectionMySql ( CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions );
    void                            UpdateStats                 ( void );

    CDynamicLibrary                                 m_DbconmyLib;
    NewDatabaseConnectionMySql_t*                   m_pfnNewDatabaseConnection;
    std::map < SString, CDatabaseConnection* >      m_SharedConnectionMap;
    std::set < CDatabaseConnection* >               m_AllConnectionMap;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseType* NewDatabaseTypeMySql ( void )
{
    return new CDatabaseTypeMySql ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::CDatabaseTypeMySql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeMySql::CDatabaseTypeMySql ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::CDatabaseTypeMySql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeMySql::~CDatabaseTypeMySql ( void )
{
    assert ( m_SharedConnectionMap.empty () );
    assert ( m_AllConnectionMap.empty () );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::GetDataSourceTag
//
// Return database type as a string
//
///////////////////////////////////////////////////////////////
SString CDatabaseTypeMySql::GetDataSourceTag ( void )
{
    return "mysql";
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::NotifyConnectionDeleted
//
// Remove connection from internal lists
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeMySql::NotifyConnectionDeleted ( CDatabaseConnection* pConnection )
{
    g_pStats->iDbConnectionCount--;
    assert ( MapContains ( m_AllConnectionMap, pConnection ) );
    MapRemove ( m_AllConnectionMap, pConnection );
    MapRemoveByValue ( m_SharedConnectionMap, pConnection );

    UpdateStats ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::NotifyConnectionChanged
//
// Update things that need to know
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeMySql::NotifyConnectionChanged ( CDatabaseConnection* pConnection )
{
    UpdateStats ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::Connect
//
// strHost is like "dbname=testdb;host=127.0.0.1;port=3303"
// strUsername is username
// strPassword is password
// strOptions contains key=value pairs separated by semicolons
//
//      Options are:
//          share=1     // Share this connection with anything else (defaults to share=1)
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseTypeMySql::Connect ( const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
{
    // Parse options
    bool bShareConnection = false;
    GetOption < CDbOptionsMap > ( strOptions, "share", bShareConnection, false );

    CDatabaseConnection* pConnection = NULL;

    // Can we share a connection?
    if ( !bShareConnection )
    {
        // No sharing so create a new connection
        pConnection = CallNewDatabaseConnectionMySql ( this, strHost, strUsername, strPassword, strOptions );
        if ( pConnection )
            pConnection->m_strOtherTag = strHost + "%" + strUsername + "%" + strOptions;
    }
    else
    {
        // Yes sharing, so make a key
        SString strShareKey = strHost + "%" + strUsername + "%" + strOptions;

        // Look for a match
        pConnection = MapFindRef ( m_SharedConnectionMap, strShareKey );
        if ( !pConnection )
        {
            // No match, so create a new connection
            pConnection = CallNewDatabaseConnectionMySql ( this, strHost, strUsername, strPassword, strOptions );
            if ( pConnection )
                MapSet ( m_SharedConnectionMap, strShareKey, pConnection );
        }
        else
        {
            // Yes match, so add a ref to existing connection
            pConnection->AddRef ();
        }
    }

    if ( pConnection )
        MapInsert ( m_AllConnectionMap, pConnection );

    UpdateStats ();

    return pConnection;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::CallNewDatabaseConnectionMySql
//
// Load, find and call NewDatabaseConnectionMySql
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseTypeMySql::CallNewDatabaseConnectionMySql ( CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
{
    if ( !m_DbconmyLib.IsLoaded () )
    {
        SString strServerPath = g_pServerInterface->GetModManager ()->GetServerPath ();
        m_DbconmyLib.Load ( PathJoin ( strServerPath, SERVER_BIN_PATH_MOD, LIB_DBCONMY ) );
        m_pfnNewDatabaseConnection = reinterpret_cast < NewDatabaseConnectionMySql_t* > ( (long long)(m_DbconmyLib.GetProcedureAddress ( "NewDatabaseConnectionMySql" )) );      
    }

    if ( !m_pfnNewDatabaseConnection )
        return NULL;

    CDatabaseConnection* pConnection = m_pfnNewDatabaseConnection ( pManager, strHost, strUsername, strPassword, strOptions );

    if ( pConnection )
        g_pStats->iDbConnectionCount++;

    return pConnection;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::UpdateStats
//
// Tracking connections
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeMySql::UpdateStats ( void )
{
    // Remove all lines with this key
    CPerfStatDebugTable::GetSingleton ()->RemoveLines ( "dbcon mysql*" );

    int iIndex = 0;
    std::set < CDatabaseConnection* > unsharedConnectionMap = m_AllConnectionMap;

    // Add shared info
    for ( std::map < SString, CDatabaseConnection* >::iterator iter = m_SharedConnectionMap.begin () ; iter != m_SharedConnectionMap.end () ; ++iter )
    {
        const SString& strShareKey = iter->first;
        CDatabaseConnection* pConnection = iter->second;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton ()->UpdateLine ( *SString ( "dbcon mysql %d", iIndex++ ), 0
                                                          ,"Database connection: mysql (shared)"
                                                          ,*strShareKey
                                                          ,*SString ( "Share count: %d", pConnection->GetShareCount () )
                                                          , NULL );

        // Update unshared map for the second part
        MapRemove ( unsharedConnectionMap, pConnection );
    }

    // Add unshared info
    for ( std::set < CDatabaseConnection* >::iterator iter = unsharedConnectionMap.begin () ; iter != unsharedConnectionMap.end () ; ++iter )
    {
        CDatabaseConnection* pConnection = *iter;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton ()->UpdateLine ( *SString ( "dbcon mysql %d", iIndex++ ), 0
                                                          ,"Database connection: mysql (unshared)"
                                                          ,*pConnection->m_strOtherTag
                                                          ,*SString ( "Refs: %d", pConnection->GetShareCount () )
                                                          , NULL );
    }
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
                double dNumber = pArgument->GetNumber ();
                if ( dNumber == floor ( dNumber ) )
                    strParsedQuery += SString ( "%" PRId64, (long long)dNumber );
                else
                    strParsedQuery += SString ( "%f", dNumber );
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
    for ( unsigned int i = 0; szQuery[i] != '\0'; i++ )
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