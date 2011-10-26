/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseManager.cpp
*  PURPOSE:     Outside world interface for enjoying asynchronous database functionality
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDatabaseJobQueue.h"


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl
//
//
///////////////////////////////////////////////////////////////
class CDatabaseManagerImpl : public CDatabaseManager
{
public:
    ZERO_ON_NEW
                                    CDatabaseManagerImpl        ( void );
    virtual                         ~CDatabaseManagerImpl       ( void );

    // CDatabaseManager
    virtual SConnectionHandle       Connect                     ( const SString& strType, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions );
    virtual bool                    Disconnect                  ( SConnectionHandle hConnection );
    virtual SJobHandle              QueryStart                  ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs );
    virtual bool                    QueryPoll                   ( SQueryResult& result, SJobHandle hQuery, ulong ulTimeout );
    virtual bool                    QueryFree                   ( SJobHandle hQuery );
    virtual bool                    IsValidQuery                ( SJobHandle hQuery );
    virtual const SString&          GetLastErrorMessage         ( void )                    { return m_strLastErrorMessage; }

    // CDatabaseManagerImpl
    SString                         InsertQueryArguments        ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs );
    SString                         InsertQueryArgumentsSqlite  ( const SString& strQuery, CLuaArguments* pArgs );
    void                            ClearLastErrorMessage       ( void )                    { m_strLastErrorMessage.clear (); }
    void                            SetLastErrorMessage         ( const SString& strMsg )   { m_strLastErrorMessage = strMsg; }

    CDatabaseJobQueue*                          m_JobQueue;
    std::map < SConnectionHandle, SString >     m_ConnectionTypeMap;
    SString                                     m_strLastErrorMessage;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseManager* NewDatabaseManager ( void )
{
    return new CDatabaseManagerImpl ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::CDatabaseManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseManagerImpl::CDatabaseManagerImpl ( void )
{
    m_JobQueue = NewDatabaseJobQueue ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::CDatabaseManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseManagerImpl::~CDatabaseManagerImpl ( void )
{
    SAFE_DELETE ( m_JobQueue );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::DatabaseConnect
//
// strType is one of the supported database types i.e. "sqlite"
//
///////////////////////////////////////////////////////////////
uint CDatabaseManagerImpl::Connect ( const SString& strType, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
{
    ClearLastErrorMessage ();

    SString strCombo = strType + "\1" + strHost + "\1" + strUsername + "\1" + strPassword + "\1" + strOptions;

    // Start connect
    SJobHandle jobHandle = m_JobQueue->AddCommand ( JOBTYPE_CONNECT, 0, strCombo );

    // Complete connect
    SQueryResult result;
    while ( !m_JobQueue->PollCommand ( result, jobHandle ) )
    {
        Sleep ( 1 );
    }

    // Check for problems
    if ( result.status == STATUS_FAIL )
    {
        SetLastErrorMessage ( result.strReason );
        return INVALID_DB_HANDLE;
    }

    // Process result
    MapSet ( m_ConnectionTypeMap, result.connectionHandle, strType );
    return result.connectionHandle;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::Disconnect
//
//
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::Disconnect ( uint hConnection )
{
    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return false;
    }

    // Start disconnect
    SJobHandle jobHandle = m_JobQueue->AddCommand ( JOBTYPE_DISCONNECT, hConnection, "" );

    // Complete disconnect
    SQueryResult result;
    while ( !m_JobQueue->PollCommand ( result, jobHandle ) )
        Sleep ( 1 );

    // Check for problems
    if ( result.status == STATUS_FAIL )
    {
        SetLastErrorMessage ( result.strReason );
        return false;
    }

    // Remove connection refs
    MapRemove ( m_ConnectionTypeMap, result.connectionHandle );
    m_JobQueue->IgnoreConnectionResults ( result.connectionHandle );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryStart
//
//
//
///////////////////////////////////////////////////////////////
SJobHandle CDatabaseManagerImpl::QueryStart ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs )
{
    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return INVALID_DB_HANDLE;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = InsertQueryArguments ( hConnection, strQuery, pArgs );

    // Start query
    SJobHandle jobHandle = m_JobQueue->AddCommand ( JOBTYPE_QUERY, hConnection, strEscapedQuery );
    return jobHandle;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryPoll
//
// ulTimeout = 0   -  No wait if not ready
// ulTimeout > 0   -  Wait(ms) if not ready
// ulTimeout = -1  -  Wait 49.7 days if not ready
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryPoll ( SQueryResult& result, SJobHandle hQuery, ulong ulTimeout )
{
    ClearLastErrorMessage ();

    while ( true )
    {
        if ( m_JobQueue->PollCommand ( result, hQuery ) )
        {
            if ( result.status == STATUS_FAIL )
                SetLastErrorMessage ( result.strReason );
            return true;
        }

        if ( ulTimeout == 0 )
            break;

        ulTimeout--;
        Sleep ( 1 );
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryFree
//
//
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryFree ( SJobHandle hQuery )
{
    ClearLastErrorMessage ();
    return m_JobQueue->IgnoreCommandResult ( hQuery );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::IsValidQuery
//
//
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::IsValidQuery ( SJobHandle hQuery )
{
    return CDatabaseJobQueue::IsValidJobHandleRange ( hQuery );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::InsertQueryArguments
//
// Insert arguments and apply correct escapement for the connection type
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::InsertQueryArguments ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs )
{
    // Check for case of no arguments
    if ( !pArgs )
        return strQuery;

    // Determine connection type
    SString* pstrType = MapFind ( m_ConnectionTypeMap, hConnection );
    SString strType = pstrType ? *pstrType : "";

    if ( strType == "sqlite" )
        return InsertQueryArgumentsSqlite ( strQuery, pArgs );
    // Other types here

    // 'Helpful' error message
    CLogger::ErrorPrintf ( "DatabaseManager internal error #1" );
    return "";
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::InsertQueryArgumentsSqlite
//
// Insert arguments and apply sqlite escapement
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::InsertQueryArgumentsSqlite ( const SString& strQuery, CLuaArguments* pArgs )
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
