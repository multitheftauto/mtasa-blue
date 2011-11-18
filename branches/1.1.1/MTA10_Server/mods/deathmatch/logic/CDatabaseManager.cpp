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
SString InsertQueryArgumentsSqlite ( const SString& strQuery, CLuaArguments* pArgs );
SString InsertQueryArgumentsMySql ( const SString& strQuery, CLuaArguments* pArgs );
SString InsertQueryArgumentsSqlite ( const char* szQuery, va_list vl );
SString InsertQueryArgumentsMySql ( const char* szQuery, va_list vl );


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
    virtual void                    DoPulse                     ( void );
    virtual SConnectionHandle       Connect                     ( const SString& strType, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions );
    virtual bool                    Disconnect                  ( SConnectionHandle hConnection );
    virtual CDbJobData*             Exec                        ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs );
    virtual CDbJobData*             Execf                       ( SConnectionHandle hConnection, const char* szQuery, ... );
    virtual CDbJobData*             QueryStart                  ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs );
    virtual CDbJobData*             QueryStartf                 ( SConnectionHandle hConnection, const char* szQuery, ... );
    virtual bool                    QueryPoll                   ( CDbJobData* pJobData, uint ulTimeout );
    virtual bool                    QueryFree                   ( CDbJobData* pJobData );
    virtual CDbJobData*             GetQueryFromId              ( SDbJobId id );
    virtual const SString&          GetLastErrorMessage         ( void )                    { return m_strLastErrorMessage; }
    virtual bool                    QueryWithResultf            ( SConnectionHandle hConnection, CRegistryResult* pResult, const char* szQuery, ... );
    virtual bool                    QueryWithCallbackf          ( SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const char* szQuery, ... );
    virtual void                    SetLogLevel                 ( EJobLogLevelType logLevel, const SString& strLogFilename );

    // CDatabaseManagerImpl
    SString                         InsertQueryArguments        ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs );
    SString                         InsertQueryArguments        ( SConnectionHandle hConnection, const char* szQuery, va_list vl );
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
    // Disconnect all active connections
    std::map < SConnectionHandle, SString > connectionTypeMapCopy = m_ConnectionTypeMap;
    for ( std::map < SConnectionHandle, SString >::iterator iter = connectionTypeMapCopy.begin () ; iter != connectionTypeMapCopy.end () ; iter++ )
            Disconnect ( iter->first );

    SAFE_DELETE ( m_JobQueue );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::DoPulse
//
// Check if any callback functions are due
//
///////////////////////////////////////////////////////////////
void CDatabaseManagerImpl::DoPulse ( void )
{
    m_JobQueue->DoPulse ();
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
    CDbJobData* pJobData = m_JobQueue->AddCommand ( EJobCommand::CONNECT, 0, strCombo );

    // Complete connect
    m_JobQueue->PollCommand ( pJobData, -1 );

    // Check for problems
    if ( pJobData->result.status == EJobResult::FAIL )
    {
        SetLastErrorMessage ( pJobData->result.strReason );
        return INVALID_DB_HANDLE;
    }

    // Process result
    MapSet ( m_ConnectionTypeMap, pJobData->result.connectionHandle, strType );
    return pJobData->result.connectionHandle;
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
    CDbJobData* pJobData = m_JobQueue->AddCommand ( EJobCommand::DISCONNECT, hConnection, "" );

    // Complete disconnect
    m_JobQueue->PollCommand ( pJobData, -1 );

    // Check for problems
    if ( pJobData->result.status == EJobResult::FAIL )
    {
        SetLastErrorMessage ( pJobData->result.strReason );
        return false;
    }

    // Remove connection refs
    MapRemove ( m_ConnectionTypeMap, hConnection );
    m_JobQueue->IgnoreConnectionResults ( hConnection );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::Exec
//
// Start a query and ignore the result
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::Exec ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs )
{
    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = InsertQueryArguments ( hConnection, strQuery, pArgs );

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand ( EJobCommand::QUERY, hConnection, strEscapedQuery );

    // Ignore result
    m_JobQueue->FreeCommand ( pJobData );
    return pJobData;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::Execf
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::Execf ( SConnectionHandle hConnection, const char* szQuery, ... )
{
    va_list vl;
    va_start ( vl, szQuery );

    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = InsertQueryArguments ( hConnection, szQuery, vl );

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand ( EJobCommand::QUERY, hConnection, strEscapedQuery );

    // Ignore result
    m_JobQueue->FreeCommand ( pJobData );
    return pJobData;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryStart
//
// Start a query
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::QueryStart ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs )
{
    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = InsertQueryArguments ( hConnection, strQuery, pArgs );

    // Start query
    return m_JobQueue->AddCommand ( EJobCommand::QUERY, hConnection, strEscapedQuery );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryStartf
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::QueryStartf ( SConnectionHandle hConnection, const char* szQuery, ... )
{
    va_list vl;
    va_start ( vl, szQuery );

    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = InsertQueryArguments ( hConnection, szQuery, vl );

    // Start query
    return m_JobQueue->AddCommand ( EJobCommand::QUERY, hConnection, strEscapedQuery );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryWithResultf
//
// Start a query and wait for the result
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryWithResultf ( SConnectionHandle hConnection, CRegistryResult* pResult, const char* szQuery, ... )
{
    va_list vl;
    va_start ( vl, szQuery );

    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return false;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = InsertQueryArguments ( hConnection, szQuery, vl );

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand ( EJobCommand::QUERY, hConnection, strEscapedQuery );

    // Wait for result
    QueryPoll ( pJobData, -1 );

    // Process result
    if ( pJobData->result.status == EJobResult::FAIL )
    {
        if ( pResult )
            *pResult = CRegistryResult ();
        return false;
    }
    else
    {
        if ( pResult )
            *pResult = pJobData->result.registryResult;
        return true;
    }
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryWithCallbackf
//
// Start a query and direct the result through a callback
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryWithCallbackf ( SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const char* szQuery, ... )
{
    va_list vl;
    va_start ( vl, szQuery );

    ClearLastErrorMessage ();

    // Check connection
    if ( !MapContains ( m_ConnectionTypeMap, hConnection ) )
    {
        SetLastErrorMessage ( "Invalid connection" );
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = InsertQueryArguments ( hConnection, szQuery, vl );

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand ( EJobCommand::QUERY, hConnection, strEscapedQuery );

    // Set callback vars
    pJobData->SetCallback ( pfnDbResult, pCallbackContext );

    return true;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryPoll
//
// ulTimeout = 0   -  No wait if not ready
// ulTimeout > 0   -  Wait(ms) if not ready
// ulTimeout = -1  -  Wait infinity+1 if not ready
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryPoll ( CDbJobData* pJobData, uint ulTimeout )
{
    ClearLastErrorMessage ();

    if ( m_JobQueue->PollCommand ( pJobData, ulTimeout ) )
    {
        if ( pJobData->result.status == EJobResult::FAIL )
            SetLastErrorMessage ( pJobData->result.strReason );
        return true;
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
bool CDatabaseManagerImpl::QueryFree ( CDbJobData* pJobData )
{
    ClearLastErrorMessage ();
    return m_JobQueue->FreeCommand ( pJobData );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::GetQueryFromId
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::GetQueryFromId ( SDbJobId id )
{
    return m_JobQueue->FindCommandFromId ( id );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::SetLogLevel
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseManagerImpl::SetLogLevel ( EJobLogLevelType logLevel, const SString& strLogFilename )
{
    CDbOptionsMap argMap;
    argMap.Set ( "name", strLogFilename );
    argMap.Set ( "level", logLevel );
    CDbJobData* pJobData = m_JobQueue->AddCommand ( EJobCommand::SETLOGLEVEL, NULL, argMap.ToString () );
    m_JobQueue->FreeCommand ( pJobData );
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
    else
    if ( strType == "mysql" )
        return InsertQueryArgumentsMySql ( strQuery, pArgs );

    // 'Helpful' error message
    CLogger::ErrorPrintf ( "DatabaseManager internal error #1" );
    return "";
}


///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::InsertQueryArguments
//
// Insert arguments and apply correct escapement for the connection type
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::InsertQueryArguments ( SConnectionHandle hConnection, const char* szQuery, va_list vl )
{
    // Determine connection type
    SString* pstrType = MapFind ( m_ConnectionTypeMap, hConnection );
    SString strType = pstrType ? *pstrType : "";

    if ( strType == "sqlite" )
        return InsertQueryArgumentsSqlite ( szQuery, vl );
    else
    if ( strType == "mysql" )
        return InsertQueryArgumentsMySql ( szQuery, vl );

    // 'Helpful' error message
    CLogger::ErrorPrintf ( "DatabaseManager internal error #2" );
    return "";
}


///////////////////////////////////////////////////////////////
//
// CDbJobData::SetCallback
//
// Set callback for a job data
// Returns false if callback could not be set
//
///////////////////////////////////////////////////////////////
bool CDbJobData::SetCallback ( PFN_DBRESULT pfnDbResult, void* pContext )
{
    if ( callback.bSet )
        return false;   // One has already been set

    if ( this->stage > EJobStage::RESULT )
        return false;   // Too late to set a callback now

    // Set new
    callback.pfnDbResult = pfnDbResult;
    callback.pContext = pContext;
    callback.bSet = true;
    return true;
}


///////////////////////////////////////////////////////////////
//
// CDbJobData::HasCallback
//
// Returns true if callback has been set and has not been called yet
//
///////////////////////////////////////////////////////////////
bool CDbJobData::HasCallback ( void )
{
    return callback.bSet && !callback.bDone;
}


///////////////////////////////////////////////////////////////
//
// CDbJobData::ProcessCallback
//
// Do callback
//
///////////////////////////////////////////////////////////////
void CDbJobData::ProcessCallback ( void )
{
    assert ( HasCallback () );
    callback.bDone = true;
    callback.pfnDbResult( this, callback.pContext );
}
