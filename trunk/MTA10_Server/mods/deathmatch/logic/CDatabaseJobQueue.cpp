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

namespace
{
    //
    // CDatabaseJobQueue helpers
    //

    typedef void *(*PFN_ThreadStart) (void *);

    // Wrap pthread
    class CThreadHandle
    {
        int res;
        pthread_t handle;
    public:
        CThreadHandle ( PFN_ThreadStart threadStart, void *arg )
        {
            res = pthread_create ( &handle, NULL, threadStart, arg );
        }

        static void AllowASyncCancel ( void )
        {
            pthread_setcancelstate ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
        }

        bool IsValid ( void ) const
        {
            return res == 0;
        }

        void Cancel ( void )
        {
            pthread_cancel ( handle );
        }
    };
}


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
    virtual bool                PollCommand                 ( CDbJobData* pJobData );
    virtual bool                FreeCommand                 ( CDbJobData* pJobData );
    virtual CDbJobData*         FindCommandFromId           ( SDbJobId id );
    virtual void                IgnoreConnectionResults     ( SConnectionHandle connectionHandle );

protected:
    void                        StopThread                  ( void );
    CDbJobData*                 GetNewJobData               ( void );
    void                        RemoveUnwantedResults       ( void );
    void                        IgnoreJobResults            ( CDbJobData* pJobData );

    // Other thread functions
    static void*                StaticThreadProc            ( void* pContext );
    void*                       ThreadProc                  ( void );
    void                        ProcessCommand              ( CDbJobData* pJobData );
    void                        ProcessConnect              ( CDbJobData* pJobData );
    void                        ProcessDisconnect           ( CDbJobData* pJobData );
    void                        ProcessQuery                ( CDbJobData* pJobData );
    uint                        MakeHandleForConnection     ( CDatabaseConnection* pConnection );
    CDatabaseConnection*        GetConnectionFromHandle     ( SConnectionHandle connectionHandle );
    void                        RemoveHandleForConnection   ( SConnectionHandle connectionHandle, CDatabaseConnection* pConnection );
    SConnectionHandle           GetNextConnectionHandle     ( void );

    // Main thread variables
    SDbJobId                            m_JobHandleCounter;
    CThreadHandle*                      m_pServiceThreadHandle;
    std::map < SDbJobId, CDbJobData* >  m_ActiveJobHandles;
    std::set < CDbJobData* >            m_IgnoreResultList;
    std::set < CDbJobData* >            m_FinishedList;         // Result has been used, will be deleted next pulse
    uint                                m_uiJobHighCount;

    // Other thread variables
    SConnectionHandle                                       m_ConnectionHandleCounter;
    std::map < SConnectionHandle, CDatabaseConnection* >    m_HandleConnectionMap;
    std::map < SString, CDatabaseType* >                    m_DatabaseTypeMap;
    uint                                                    m_uiConnectionHighCount;

    // Shared variables
    struct
    {
        bool                                        m_bTerminateThread;
        bool                                        m_bThreadTerminated;
        std::list < CDbJobData* >                   m_CommandQueue;
        std::list < CDbJobData* >                   m_ResultQueue;
        CCriticalSection                            m_CS;
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
    : m_uiJobHighCount ( 10 )
    , m_uiConnectionHighCount ( 10 )
{
    // Add known database types
    CDatabaseType* pDatabaseTypeSqlite = NewDatabaseTypeSqlite ();
    MapSet ( m_DatabaseTypeMap, pDatabaseTypeSqlite->GetDataSourceTag (), pDatabaseTypeSqlite );

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
    shared.m_bTerminateThread = true;

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
    do
    {
        m_JobHandleCounter++;
        m_JobHandleCounter &= 0x000FFFFF;
        m_JobHandleCounter |= 0x00100000;
        // TODO - check when all (1,048,575) ids are in use
    }
    while ( MapContains ( m_ActiveJobHandles, m_JobHandleCounter ) );

    // Keep track of number of job handles
    if ( m_ActiveJobHandles.size () > m_uiJobHighCount )
    {
        m_uiJobHighCount = m_ActiveJobHandles.size () * 2;
        CLogger::LogPrintf ( "Notice: There are now %d job handles\n", m_ActiveJobHandles.size () );
    }

    CDbJobData* pJobData = new CDbJobData ( m_JobHandleCounter );
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
    // Create command
    CDbJobData* pJobData = GetNewJobData ();
    pJobData->command.type = jobType;
    pJobData->command.connectionHandle = connectionHandle;
    pJobData->command.strData = strData;

    // Add to queue
    shared.m_CS.Lock ();
    pJobData->stage = EJobStage::COMMAND_QUEUE;
    shared.m_CommandQueue.push_back ( pJobData );
    shared.m_CS.Unlock ();

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
again:
    shared.m_CS.Lock ();

    // Delete finished
    for ( std::set < CDbJobData* >::iterator iter = m_FinishedList.begin () ; iter != m_FinishedList.end () ; )
    {
        CDbJobData* pJobData = *iter;
        m_FinishedList.erase ( iter++ );
        // Check not refed
        assert ( !ListContains ( shared.m_CommandQueue, pJobData ) );
        assert ( !ListContains ( shared.m_ResultQueue, pJobData ) );
        assert ( !MapContains ( m_IgnoreResultList, pJobData ) );
        assert ( !MapContains ( m_FinishedList, pJobData ) );

        assert ( MapContains ( m_ActiveJobHandles, pJobData->GetId () ) );
        MapRemove ( m_ActiveJobHandles, pJobData->GetId () );
        SAFE_DELETE( pJobData->pLuaCallback );
        SAFE_DELETE( pJobData );
    }

    // Remove ignored
    RemoveUnwantedResults ();

    // Do pending callbacks
    for ( std::list < CDbJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
    {
        CDbJobData* pJobData = *iter;
        if ( pJobData->pLuaCallback )
        {
            CLuaCallback* pLuaCallback = pJobData->pLuaCallback;
            pJobData->pLuaCallback = NULL;
            shared.m_CS.Unlock ();

            pLuaCallback->Call ();
            SAFE_DELETE( pLuaCallback );

            // Redo from the top ensure everything is consistant
            goto again;
        }
    }

    shared.m_CS.Unlock ();

}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::RemoveUnwantedResults
//
// Check result queue items for match with ignore list items.
// * Must be called from inside a locked section *
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::RemoveUnwantedResults ( void )
{
    if ( m_IgnoreResultList.empty () )
        return;

    for ( std::list < CDbJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; )
    {
        CDbJobData* pJobData = *iter;
        if ( MapContains ( m_IgnoreResultList, pJobData ) )
        {
            // Found result to ignore, remove from result and ignore lists, add to finished list
            iter = shared.m_ResultQueue.erase ( iter );
            MapRemove ( m_IgnoreResultList, pJobData );
            pJobData->stage = EJobStage::FINISHED;
            MapInsert ( m_FinishedList, pJobData );
        }
        else
            ++iter;
    }
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::PollCommand
//
// Find result for previous command
// Returns false if result not ready.
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueueImpl::PollCommand ( CDbJobData* pJobData )
{
    shared.m_CS.Lock ();

    // Remove ignored before checking
    RemoveUnwantedResults ();

    // Find result with the required job handle
    bool bFound = false;
    for ( std::list < CDbJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
    {
        if ( pJobData == *iter )
        {
            // Found result. Remove from the result queue and flag return value
            shared.m_ResultQueue.erase ( iter );
            pJobData->stage = EJobStage::FINISHED;
            MapInsert ( m_FinishedList, pJobData );
            bFound = true;
            break;
        }
    }
    shared.m_CS.Unlock ();

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

    if ( MapContains ( m_IgnoreResultList, pJobData ) )
        return false;       // Already ignoring query handle

    // if in command or result queue, then put in ignore result list
    bool bFound;
    shared.m_CS.Lock ();

    bFound = ListContains ( shared.m_CommandQueue, pJobData ) || ListContains ( shared.m_ResultQueue, pJobData );

    shared.m_CS.Unlock ();

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
    shared.m_CS.Lock ();

    // All active jobhandles will either be in m_CommandQueue or m_ResultQueue or m_FinishedList
    for ( std::list < CDbJobData* >::iterator iter = shared.m_CommandQueue.begin () ; iter != shared.m_CommandQueue.end () ; ++iter )
        if ( (*iter)->command.connectionHandle == connectionHandle )
            IgnoreJobResults ( *iter );

    for ( std::list < CDbJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
        if ( (*iter)->command.connectionHandle == connectionHandle )
            IgnoreJobResults ( *iter );

    shared.m_CS.Unlock ();
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
    // Callback won't be needed now
    SAFE_DELETE( pJobData->pLuaCallback );
    MapInsert ( m_IgnoreResultList, pJobData );
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
    while ( !shared.m_bTerminateThread )
    {
        // TODO: Use wait instead of sleep
        Sleep ( 1 );

        // Is there a waiting command?
        shared.m_CS.Lock ();
        if ( !shared.m_CommandQueue.empty () )
        {
            // Get next command
            CDbJobData* pJobData = shared.m_CommandQueue.front ();
            pJobData->stage = EJobStage::PROCCESSING;
            shared.m_CS.Unlock ();

            // Process command
            ProcessCommand ( pJobData );

            // Store result
            shared.m_CS.Lock ();
            // Check command has not been cancelled (this should not be possible)
            if ( !shared.m_CommandQueue.empty () && pJobData == shared.m_CommandQueue.front () )
            {
                // Remove command
                shared.m_CommandQueue.pop_front ();
                // Add result
                pJobData->stage = EJobStage::RESULT;
                shared.m_ResultQueue.push_back ( pJobData );
            }
        }
        shared.m_CS.Unlock ();
    }

    shared.m_bThreadTerminated = true;
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
        pJobData->result.strReason = pConnection->GetLastErrorMessage ();
        pConnection->Release ();
        return;
    }

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
        return;
    }

    // Set result
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
    if ( m_HandleConnectionMap.size () > m_uiConnectionHighCount )
    {
        m_uiConnectionHighCount = m_HandleConnectionMap.size () * 2;
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
