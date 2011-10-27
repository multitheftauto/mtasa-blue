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
    virtual CDbJobData*             QueryStart                  ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs );
    virtual bool                    QueryPoll                   ( CDbJobData* pJobData, ulong ulTimeout );
    virtual bool                    QueryFree                   ( CDbJobData* pJobData );
    virtual CDbJobData*             GetQueryFromId              ( SDbJobId id );
    virtual const SString&          GetLastErrorMessage         ( void )                    { return m_strLastErrorMessage; }

    // CDatabaseManagerImpl
    SString                         InsertQueryArguments        ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs );
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
    while ( !m_JobQueue->PollCommand ( pJobData ) )
    {
        Sleep ( 1 );
    }

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
    while ( !m_JobQueue->PollCommand ( pJobData ) )
        Sleep ( 1 );

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
// CDatabaseManagerImpl::QueryStart
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::QueryStart ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs )
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
    CDbJobData* jobHandle = m_JobQueue->AddCommand ( EJobCommand::QUERY, hConnection, strEscapedQuery );
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
bool CDatabaseManagerImpl::QueryPoll ( CDbJobData* pJobData, ulong ulTimeout )
{
    ClearLastErrorMessage ();

    while ( true )
    {
        if ( m_JobQueue->PollCommand ( pJobData ) )
        {
            if ( pJobData->result.status == EJobResult::FAIL )
                SetLastErrorMessage ( pJobData->result.strReason );
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
