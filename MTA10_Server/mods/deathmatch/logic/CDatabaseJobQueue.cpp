/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseJobQueue.cpp
*  PURPOSE:     Threaded job queue
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDatabaseType.h"
#include "CDatabaseJobQueue.h"
#include "SharedUtil.Thread.h"

SThreadCPUTimesStore g_DatabaseThreadCPUTimes;

typedef CFastList < CDbJobData* > CJobQueueType;

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl
//
//
///////////////////////////////////////////////////////////////
class CDatabaseJobQueueImpl : public CDatabaseJobQueue
{
public:
    ZERO_ON_NEW
                                CDatabaseJobQueueImpl       ( void );
    virtual                     ~CDatabaseJobQueueImpl      ( void );

    // Main thread functions
    virtual void                DoPulse                     ( void );
    virtual CDbJobData*         AddCommand                  ( EJobCommandType jobType, SConnectionHandle connectionHandle, const SString& strData );
    virtual bool                PollCommand                 ( CDbJobData* pJobData, uint uiTimeout );
    virtual bool                FreeCommand                 ( CDbJobData* pJobData );
    virtual CDbJobData*         FindCommandFromId           ( SDbJobId id );
    virtual void                IgnoreConnectionResults     ( SConnectionHandle connectionHandle );

protected:
    void                        StopThread                  ( void );
    CDbJobData*                 GetNewJobData               ( void );
    void                        UpdateDebugData             ( void );
    void                        IgnoreJobResults            ( CDbJobData* pJobData );

    // Other thread functions
    static void*                StaticThreadProc            ( void* pContext );
    void*                       ThreadProc                  ( void );
    void                        ProcessCommand              ( CDbJobData* pJobData );
    void                        ProcessConnect              ( CDbJobData* pJobData );
    void                        ProcessDisconnect           ( CDbJobData* pJobData );
    void                        ProcessQuery                ( CDbJobData* pJobData );
    void                        ProcessFlush                ( CDbJobData* pJobData );
    void                        ProcessSetLogLevel          ( CDbJobData* pJobData );
    uint                        MakeHandleForConnection     ( CDatabaseConnection* pConnection );
    CDatabaseConnection*        GetConnectionFromHandle     ( SConnectionHandle connectionHandle );
    void                        RemoveHandleForConnection   ( SConnectionHandle connectionHandle, CDatabaseConnection* pConnection );
    SConnectionHandle           GetNextConnectionHandle     ( void );
    void                        LogResult                   ( CDbJobData* pJobData );
    void                        LogString                   ( const SString& strText );

    // Main thread variables
    CThreadHandle*                      m_pServiceThreadHandle;
    std::map < SDbJobId, CDbJobData* >  m_ActiveJobHandles;
    std::set < CDbJobData* >            m_FinishedList;         // Result has been used, will be deleted next pulse
    uint                                m_uiJobCountWarnThresh;
    uint                                m_uiJobCount10sMin;
    CElapsedTime                        m_JobCountElpasedTime;
    std::set < SConnectionHandle >      m_PendingFlushMap;

    // Other thread variables
    SConnectionHandle                                       m_ConnectionHandleCounter;
    std::map < SConnectionHandle, CDatabaseConnection* >    m_HandleConnectionMap;
    std::map < SString, CDatabaseType* >                    m_DatabaseTypeMap;
    uint                                                    m_uiConnectionCountWarnThresh;
    EJobLogLevelType                                        m_LogLevel;
    SString                                                 m_strLogFilename;

    // Shared variables
    struct
    {
        bool                                        m_bTerminateThread;
        bool                                        m_bThreadTerminated;
        CJobQueueType                               m_CommandQueue;
        CJobQueueType                               m_ResultQueue;
        CComboMutex                                 m_Mutex;
    } shared;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseJobQueue* NewDatabaseJobQueue ( void )
{
    return new CDatabaseJobQueueImpl ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::CDatabaseJobQueueImpl
//
// Init known database types and start the job service thread
//
///////////////////////////////////////////////////////////////
CDatabaseJobQueueImpl::CDatabaseJobQueueImpl ( void )
    : m_uiJobCountWarnThresh ( 200 )
    , m_uiConnectionCountWarnThresh ( 20 )
{
    // Add known database types
    CDatabaseType* pDatabaseTypeSqlite = NewDatabaseTypeSqlite ();
    MapSet ( m_DatabaseTypeMap, pDatabaseTypeSqlite->GetDataSourceTag (), pDatabaseTypeSqlite );

    // Add known database types
    CDatabaseType* pDatabaseTypeMySql = NewDatabaseTypeMySql ();
    MapSet ( m_DatabaseTypeMap, pDatabaseTypeMySql->GetDataSourceTag (), pDatabaseTypeMySql );

    // Start the job queue processing thread
    m_pServiceThreadHandle = new CThreadHandle ( CDatabaseJobQueueImpl::StaticThreadProc, this );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::CDatabaseJobQueueImpl
//
// Stop threads and delete everything
//
///////////////////////////////////////////////////////////////
CDatabaseJobQueueImpl::~CDatabaseJobQueueImpl ( void )
{
    // Stop the job queue processing thread
    StopThread ();

    // Delete thread
    SAFE_DELETE ( m_pServiceThreadHandle );

    // Delete database types
    for ( std::map < SString, CDatabaseType* >::iterator iter = m_DatabaseTypeMap.begin () ; iter != m_DatabaseTypeMap.end () ; ++iter )
        SAFE_DELETE ( iter->second );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::StopThread
//
// Stop the job queue processing thread
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::StopThread ( void )
{
    // Stop the job queue processing thread
    shared.m_Mutex.Lock ();
    shared.m_bTerminateThread = true;
    shared.m_Mutex.Signal ();
    shared.m_Mutex.Unlock ();

    for ( uint i = 0 ; i < 5000 ; i += 15 )
    {
        if ( shared.m_bThreadTerminated )
            return;

        Sleep ( 15 );
    }

    // If thread not stopped, (async) cancel it
    m_pServiceThreadHandle->Cancel ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::GetNextJobHandle
//
// Return a new job data object
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseJobQueueImpl::GetNewJobData ( void )
{
    g_pStats->iDbJobDataCount++;
    CDbJobData* pJobData = new CDbJobData ();
    assert ( !MapContains ( m_ActiveJobHandles, pJobData->GetId () ) );
    MapSet ( m_ActiveJobHandles, pJobData->GetId (), pJobData );
    return pJobData;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::AddCommand
//
// AddCommand to queue
// Can't fail
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseJobQueueImpl::AddCommand ( EJobCommandType jobType, SConnectionHandle connectionHandle, const SString& strData )
{
    // Add connection handle to the flush todo list
    if ( jobType == EJobCommand::QUERY )
        MapInsert ( m_PendingFlushMap, connectionHandle );

    // Create command
    CDbJobData* pJobData = GetNewJobData ();
    pJobData->command.type = jobType;
    pJobData->command.connectionHandle = connectionHandle;
    pJobData->command.strData = strData;

    // Add to queue
    shared.m_Mutex.Lock ();
    pJobData->stage = EJobStage::COMMAND_QUEUE;
    shared.m_CommandQueue.push_back ( pJobData );
    shared.m_Mutex.Signal ();
    shared.m_Mutex.Unlock ();

    return pJobData;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::DoPulse
//
// Check if any callback functions are due
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::DoPulse ( void )
{
    // Check if any connection needs a flush
    while ( m_PendingFlushMap.size () )
    {
        SConnectionHandle connectionHandle = *m_PendingFlushMap.begin ();
        MapRemove ( m_PendingFlushMap, connectionHandle );
        CDbJobData* pJobData = AddCommand ( EJobCommand::FLUSH, connectionHandle, "" );
        FreeCommand ( pJobData );
    }

    shared.m_Mutex.Lock ();

again:
    // Delete finished
    for ( std::set < CDbJobData* >::iterator iter = m_FinishedList.begin () ; iter != m_FinishedList.end () ; )
    {
        CDbJobData* pJobData = *iter;
        m_FinishedList.erase ( iter++ );
        // Check not refed
        dassert ( !ListContains ( shared.m_CommandQueue, pJobData ) );
        dassert ( !ListContains ( shared.m_ResultQueue, pJobData ) );
        dassert ( !MapContains ( m_FinishedList, pJobData ) );

        dassert ( MapContains ( m_ActiveJobHandles, pJobData->GetId () ) );
        MapRemove ( m_ActiveJobHandles, pJobData->GetId () );
        dassert ( !pJobData->HasCallback () );
        SAFE_DELETE( pJobData );
        g_pStats->iDbJobDataCount--;
    }

    // Do pending callbacks
    for ( CJobQueueType::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; )
    {
        CDbJobData* pJobData = *iter;

        if ( pJobData->result.bIgnoreResult )
        {
            // Ignored results won't be collected, so move to finished list here
            iter = shared.m_ResultQueue.erase ( iter );
            pJobData->stage = EJobStage::FINISHED;
            MapInsert ( m_FinishedList, pJobData );
            // Still allow callback incase any cleanup is needed
        }
        else
            ++iter;

        if ( pJobData->HasCallback () )
        {
            shared.m_Mutex.Unlock ();
            pJobData->ProcessCallback ();              
            shared.m_Mutex.Lock ();

            // Redo from the top ensure everything is consistent
            goto again;
        }
    }

    shared.m_Mutex.Unlock ();

    UpdateDebugData ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::UpdateDebugData
//
// Update info relevant to debugging database jobs
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::UpdateDebugData ( void )
{
    // Update once every 10 seconds
    if ( m_JobCountElpasedTime.Get () < 10000 )
        return;

    shared.m_Mutex.Lock ();

    // Log to console if job count is creeping up
    m_uiJobCount10sMin = Min < uint > ( m_uiJobCount10sMin, m_ActiveJobHandles.size () );
    if ( m_uiJobCount10sMin > m_uiJobCountWarnThresh )
    {
        m_uiJobCountWarnThresh = m_uiJobCount10sMin * 2;
        CLogger::LogPrintf ( "Notice: %d database query handles active in the last 10 seconds\n", m_uiJobCount10sMin );
    }
    m_JobCountElpasedTime.Reset ();
    m_uiJobCount10sMin = m_ActiveJobHandles.size ();

    CTickCount timeNow = CTickCount::Now ( true );

    // Log old uncollected queries
    for ( CJobQueueType::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; iter++ )
    {
        CDbJobData* pJobData = *iter;
        if ( !pJobData->result.bLoggedWarning )
        {
            CTickCount age = timeNow - pJobData->result.timeReady;
            if ( age.ToLongLong () > 1000 * 60 * 5 )
            {
                g_pGame->GetScriptDebugging()->LogWarning( pJobData->m_LuaDebugInfo, "Database result uncollected after 5 minutes. [Query: %s]", *pJobData->command.strData );
                pJobData->result.bLoggedWarning = true;
                break;
            }
        }
    }

    shared.m_Mutex.Unlock ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::PollCommand
//
// Find result for previous command
// Returns false if result not ready.
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueueImpl::PollCommand ( CDbJobData* pJobData, uint uiTimeout )
{
    bool bFound = false;

    shared.m_Mutex.Lock ();
    while ( true )
    {
        // Should not be called for ignored results
        dassert ( !pJobData->result.bIgnoreResult );

        // Should not be called for collected results
        dassert ( pJobData->stage != EJobStage::FINISHED );

        // See if result has come in yet
        if ( ListContains( shared.m_ResultQueue, pJobData ) )
        {
            ListRemove( shared.m_ResultQueue, pJobData );

            pJobData->stage = EJobStage::FINISHED;
            MapInsert ( m_FinishedList, pJobData );

            // Do callback incase any cleanup is needed
            if ( pJobData->HasCallback () )
            {
                shared.m_Mutex.Unlock ();                 
                pJobData->ProcessCallback ();              
                shared.m_Mutex.Lock ();
            }

            bFound = true;
        }

        if ( bFound || uiTimeout == 0 )
        {
            shared.m_Mutex.Unlock ();
            break;
        }

        shared.m_Mutex.Wait ( uiTimeout );

        // If not infinite, break after next check
        if ( uiTimeout != (uint)-1 )
            uiTimeout = 0;
    }

    // Make sure if wait was infinite, we have a result
    assert ( uiTimeout != (uint)-1 || bFound );

    return bFound;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::FreeCommand
//
// Throw away result when this job is done
// Returns false if jobHandle not correct
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueueImpl::FreeCommand ( CDbJobData* pJobData )
{
    if ( !MapContains ( m_ActiveJobHandles, pJobData->GetId () ) )
    {
        CLogger::ErrorPrintf ( "FreeCommand: Serious problem #1 here\n" );
        return false;
    }

    if ( pJobData->result.bIgnoreResult )
        return false;       // Already ignoring query handle

    // if in command or result queue, then put in ignore result list
    bool bFound;
    shared.m_Mutex.Lock ();

    bFound = ListContains ( shared.m_CommandQueue, pJobData ) || ListContains ( shared.m_ResultQueue, pJobData );

    shared.m_Mutex.Unlock ();

    if ( !bFound )
    {
        // Must be in finished list
        if ( !MapContains ( m_FinishedList, pJobData ) )
            CLogger::ErrorPrintf ( "FreeCommand: Serious problem #2 here\n" );
        return false; 
    }

    IgnoreJobResults ( pJobData );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::FindCommandFromId
//
// Find job data with matching id
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseJobQueueImpl::FindCommandFromId ( SDbJobId id )
{
    return MapFindRef ( m_ActiveJobHandles, id );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::IgnoreConnectionResults
//
// Throw away results for all jobs using this connection
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::IgnoreConnectionResults ( SConnectionHandle connectionHandle )
{
    shared.m_Mutex.Lock ();

    // All active jobhandles will either be in m_CommandQueue or m_ResultQueue or m_FinishedList
    for ( CJobQueueType::iterator iter = shared.m_CommandQueue.begin () ; iter != shared.m_CommandQueue.end () ; ++iter )
        if ( (*iter)->command.connectionHandle == connectionHandle )
            IgnoreJobResults ( *iter );

    for ( CJobQueueType::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
        if ( (*iter)->command.connectionHandle == connectionHandle )
            IgnoreJobResults ( *iter );

    shared.m_Mutex.Unlock ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::IgnoreJobResult
//
// Throw away the result for this job
// * Assumed job data is in either m_CommandQueue or m_ResultQueue *
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::IgnoreJobResults ( CDbJobData* pJobData )
{
    dassert ( pJobData->stage <= EJobStage::RESULT );
    dassert ( !MapContains ( m_FinishedList, pJobData ) );
    pJobData->result.bIgnoreResult = true;
}


//
//
//
// Job servicing thread
//
//
//

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::StaticThreadProc
//
// static function
// Thread entry point
//
///////////////////////////////////////////////////////////////
void* CDatabaseJobQueueImpl::StaticThreadProc ( void* pContext )
{
    CThreadHandle::AllowASyncCancel ();
    return ((CDatabaseJobQueueImpl*)pContext)->ThreadProc ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ThreadProc
//
// Job service loop
//
///////////////////////////////////////////////////////////////
void* CDatabaseJobQueueImpl::ThreadProc ( void )
{
    shared.m_Mutex.Lock ();
    while ( !shared.m_bTerminateThread )
    {
        UpdateThreadCPUTimes( g_DatabaseThreadCPUTimes );

        // Is there a waiting command?
        if ( shared.m_CommandQueue.empty () )
        {
            shared.m_Mutex.Wait ( 100 );
        }
        else
        {
            // Get next command
            CDbJobData* pJobData = shared.m_CommandQueue.front ();
            pJobData->stage = EJobStage::PROCCESSING;
            shared.m_Mutex.Unlock ();

            // Process command
            ProcessCommand ( pJobData );

            // Store result
            shared.m_Mutex.Lock ();
            // Check command has not been cancelled (this should not be possible)
            if ( !shared.m_CommandQueue.empty () && pJobData == shared.m_CommandQueue.front () )
            {
                // Remove command
                shared.m_CommandQueue.pop_front ();
                // Add result
                pJobData->stage = EJobStage::RESULT;
                pJobData->result.timeReady = CTickCount::Now ( true );
                shared.m_ResultQueue.push_back ( pJobData );
            }
            shared.m_Mutex.Signal ();
        }
    }

    shared.m_bThreadTerminated = true;
    shared.m_Mutex.Unlock ();

    return NULL;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessCommand
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessCommand ( CDbJobData* pJobData )
{
    if ( pJobData->command.type == EJobCommand::CONNECT )
        ProcessConnect ( pJobData );
    else
    if ( pJobData->command.type == EJobCommand::DISCONNECT )
        ProcessDisconnect ( pJobData );
    else
    if ( pJobData->command.type == EJobCommand::QUERY )
        ProcessQuery ( pJobData );
    else
    if ( pJobData->command.type == EJobCommand::FLUSH )
        ProcessFlush ( pJobData );
    else
    if ( pJobData->command.type == EJobCommand::SETLOGLEVEL )
        ProcessSetLogLevel ( pJobData );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessConnect
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessConnect ( CDbJobData* pJobData )
{
    // Determine which type manager to use
    std::vector < SString > parts;
    pJobData->command.strData.Split ( "\1", parts );
    if ( parts.size () < 5 )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = "Internal Error (JobQueueServer:ProcessConnect #1)";
        return;
    }

    CDatabaseType* pTypeManager = MapFindRef ( m_DatabaseTypeMap, parts[0] );
    if ( !pTypeManager )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = "Not valid type";
        return;
    }

    // Get type manager to return a CDatabaseConnection*
    CDatabaseConnection* pConnection = pTypeManager->Connect ( parts[1], parts[2], parts[3], parts[4] );
    if ( !pConnection )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = "Could not connect";
        return;
    }

    if ( !pConnection->IsValid () )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = pConnection->GetLastErrorMessage ();
        pConnection->Release ();
        return;
    }

    // Extract some options
    GetOption < CDbOptionsMap > ( parts[4], "log", pConnection->m_bLoggingEnabled, 0 );
    GetOption < CDbOptionsMap > ( parts[4], "tag", pConnection->m_strLogTag );
    GetOption < CDbOptionsMap > ( parts[4], "suppress", ",", pConnection->m_SuppressedErrorCodes );

    // Only allow error codes to be suppress with mysql, as sqlite only has one error code
    if ( pTypeManager->GetDataSourceTag () != "mysql" )
        pConnection->m_SuppressedErrorCodes.clear ();

    // Get handle from CDatabaseConnection*
    SConnectionHandle connectionHandle = MakeHandleForConnection ( pConnection );

    // Set result
    pJobData->result.status = EJobResult::SUCCESS;
    pJobData->result.connectionHandle = connectionHandle;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessDisconnect
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessDisconnect ( CDbJobData* pJobData )
{
    // CDatabaseConnection* from handle
    CDatabaseConnection* pConnection = GetConnectionFromHandle ( pJobData->command.connectionHandle );
    if ( !pConnection )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = "Invalid connection";
        return;
    }

    // And disconnect
    RemoveHandleForConnection ( pJobData->command.connectionHandle, pConnection );
    pConnection->Release ();
    pConnection = NULL;

    // Set result
    pJobData->result.status = EJobResult::SUCCESS;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessQuery
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessQuery ( CDbJobData* pJobData )
{
    // CDatabaseConnection* from handle
    CDatabaseConnection* pConnection = GetConnectionFromHandle ( pJobData->command.connectionHandle );
    if ( !pConnection )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = "Invalid connection";
        return;
    }

    // And query
    if ( !pConnection->Query ( pJobData->command.strData, pJobData->result.registryResult ) )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = pConnection->GetLastErrorMessage ();
        pJobData->result.uiErrorCode = pConnection->GetLastErrorCode ();
        pJobData->result.bErrorSuppressed = MapContains ( pConnection->m_SuppressedErrorCodes, pConnection->GetLastErrorCode () );
    }
    else
    {
        pJobData->result.status = EJobResult::SUCCESS;
    }

    // And log if required
    LogResult ( pJobData );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessFlush ()
//
// Tell the connection to flush cached data
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessFlush ( CDbJobData* pJobData )
{
    CDatabaseConnection* pConnection = GetConnectionFromHandle ( pJobData->command.connectionHandle );
    if ( !pConnection )
    {
        pJobData->result.status = EJobResult::FAIL;
        pJobData->result.strReason = "Invalid connection";
        return;
    }

    // Do flush
    pConnection->Flush ();
    pJobData->result.status = EJobResult::SUCCESS;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessSetLogLevel ()
//
// Change logging level and filename
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessSetLogLevel ( CDbJobData* pJobData )
{
    GetOption < CDbOptionsMap > ( pJobData->command.strData, "name", m_strLogFilename );
    GetOption < CDbOptionsMap > ( pJobData->command.strData, "level", m_LogLevel );
    pJobData->result.status = EJobResult::SUCCESS;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::GetNextConnectionHandle
//
// Return handle within correct range
//
///////////////////////////////////////////////////////////////
SConnectionHandle CDatabaseJobQueueImpl::GetNextConnectionHandle ( void )
{
    do
    {
        m_ConnectionHandleCounter++;
        m_ConnectionHandleCounter &= 0x000FFFFF;
        m_ConnectionHandleCounter |= 0x00200000;
        // TODO - check when all (1,048,575) ids are in use
    }
    while ( MapContains ( m_HandleConnectionMap, m_ConnectionHandleCounter ) );

    // Keep track of number of database connections
    if ( m_HandleConnectionMap.size () > m_uiConnectionCountWarnThresh )
    {
        m_uiConnectionCountWarnThresh = m_HandleConnectionMap.size () * 2;
        CLogger::LogPrintf ( "Notice: There are now %d database connections\n", m_HandleConnectionMap.size () );
    }

    return m_ConnectionHandleCounter;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::MakeHandleForConnection
//
//
//
///////////////////////////////////////////////////////////////
SConnectionHandle CDatabaseJobQueueImpl::MakeHandleForConnection ( CDatabaseConnection* pConnection )
{
    SConnectionHandle connectionHandle = GetNextConnectionHandle ();
    MapSet ( m_HandleConnectionMap, connectionHandle, pConnection );
    return connectionHandle;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::GetConnectionFromHandle
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseJobQueueImpl::GetConnectionFromHandle ( SConnectionHandle connectionHandle )
{
    return MapFindRef ( m_HandleConnectionMap, connectionHandle );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::RemoveHandleForConnection
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::RemoveHandleForConnection ( SConnectionHandle connectionHandle, CDatabaseConnection* pConnection )
{
    if ( !MapContains ( m_HandleConnectionMap, connectionHandle ) )
        CLogger::ErrorPrintf ( "RemoveHandleForConnection: Serious problem here\n" );

    MapRemove ( m_HandleConnectionMap, connectionHandle );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::LogResult
//
// Log last job if connection has logging enabled
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::LogResult ( CDbJobData* pJobData )
{
    // Early out if logging switched off globally
    if ( m_LogLevel == EJobLogLevel::NONE )
        return;

    // Check logging status of connection
    CDatabaseConnection* pConnection = GetConnectionFromHandle ( pJobData->command.connectionHandle );
    if ( !pConnection || !pConnection->m_bLoggingEnabled )
        return;

    if ( pJobData->result.status == EJobResult::SUCCESS )
    {
        if ( m_LogLevel >= EJobLogLevel::ALL )
        {
            SString strLine ( "%s: [%s] SUCCESS: Affected rows:%d [Query:%s]\n"
                                    , *GetLocalTimeString ( true, true )
                                    , *pConnection->m_strLogTag
                                    , pJobData->result.registryResult->uiNumAffectedRows
                                    , *pJobData->command.strData
                                );
            LogString ( strLine );
        }
    }
    else
    {
        if ( m_LogLevel >= EJobLogLevel::ERRORS )
        {
            // Suppress requested errors unless logging is set to ALL
            if ( pJobData->result.bErrorSuppressed && m_LogLevel != EJobLogLevel::ALL )
                return;

            SString strLine ( "%s: [%s] FAIL: (%d) %s [Query:%s]\n"
                                    , *GetLocalTimeString ( true, true )
                                    , *pConnection->m_strLogTag
                                    , pJobData->result.uiErrorCode
                                    , *pJobData->result.strReason
                                    , *pJobData->command.strData
                                );
            LogString ( strLine );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::LogString
//
// Add string to log output
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::LogString ( const SString& strText )
{
    FileAppend ( m_strLogFilename, strText );
}
