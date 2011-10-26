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

    // Used by other thread
    struct SCommandQueueItem
    {
        EJobType    jobType;
        SJobHandle  jobHandle;
        SConnectionHandle connectionHandle;
        SString     strData;
    };

    // Used by other thread
    struct SResultQueueItem
    {
        EResultStatus   status;
        SString         strReason;
        SJobHandle      jobHandle;
        SConnectionHandle connectionHandle;
        CRegistryResult registryResult;
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
    virtual SJobHandle          AddCommand                  ( EJobType jobType, SConnectionHandle connectionHandle, const SString& strData );
    virtual bool                PollCommand                 ( SQueryResult& queryResult, SJobHandle jobHandle );
    virtual bool                IgnoreCommandResult         ( SJobHandle jobHandle );
    virtual void                IgnoreConnectionResults     ( SConnectionHandle connectionHandle );

protected:
    void                        StopThread                  ( void );
    SJobHandle                  GetNextJobHandle            ( void );
    void                        UnuseJobHandle              ( SJobHandle jobHandle );

    // Other thread functions
    static void*                StaticThreadProc            ( void* pContext );
    void*                       ThreadProc                  ( void );
    void                        ProcessCommand              ( SResultQueueItem& result, const SCommandQueueItem& command );
    void                        ProcessConnect              ( SResultQueueItem& result, const SCommandQueueItem& command );
    void                        ProcessDisconnect           ( SResultQueueItem& result, const SCommandQueueItem& command );
    void                        ProcessQuery                ( SResultQueueItem& result, const SCommandQueueItem& command );
    uint                        MakeHandleForConnection     ( CDatabaseConnection* pConnection );
    CDatabaseConnection*        GetConnectionFromHandle     ( SConnectionHandle connectionHandle );
    void                        RemoveHandleForConnection   ( SConnectionHandle connectionHandle, CDatabaseConnection* pConnection );
    SConnectionHandle           GetNextConnectionHandle     ( void );

    // Main thread variables
    SJobHandle                      m_JobHandleCounter;
    CThreadHandle*                  m_pServiceThreadHandle;
    std::set < SJobHandle >         m_ActiveJobHandles;
    std::set < SJobHandle >         m_IgnoreResultList;
    uint                            m_uiJobHighCount;

    // Other thread variables
    SConnectionHandle                                       m_ConnectionHandleCounter;
    std::map < SConnectionHandle, CDatabaseConnection* >    m_HandleConnectionMap;
    std::map < SString, CDatabaseType* >                    m_DatabaseTypeMap;
    uint                                                    m_uiConnectionHighCount;

    // Shared variables
    bool                                        m_bTerminateThread;
    bool                                        m_bThreadTerminated;
    std::list < SCommandQueueItem >             m_CommandQueue;
    std::list < SResultQueueItem >              m_ResultQueue;
    CCriticalSection                            m_CS;
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
    this->m_bTerminateThread = true;

    for ( uint i = 0 ; i < 5000 ; i += 15 )
    {
        if ( this->m_bThreadTerminated )
            return;

        Sleep ( 15 );
    }

    // If thread not stopped, (async) cancel it
    m_pServiceThreadHandle->Cancel ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueue::IsValidJobHandleRange
//
// Static function
// Returns false if job handle can never be correct
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueue::IsValidJobHandleRange ( SJobHandle jobHandle )
{
    return jobHandle >= 0x00100000 && jobHandle <= 0x001FFFFF;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::GetNextJobHandle
//
// Return next free handle within correct range
//
///////////////////////////////////////////////////////////////
SJobHandle CDatabaseJobQueueImpl::GetNextJobHandle ( void )
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

    SJobHandle jobHandle = m_JobHandleCounter;
    MapInsert ( m_ActiveJobHandles, jobHandle );
    return jobHandle;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::UnuseJobHandle
//
// When finished with
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::UnuseJobHandle ( SJobHandle jobHandle )
{
    MapRemove ( m_ActiveJobHandles, jobHandle );
    MapRemove ( m_IgnoreResultList, jobHandle );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::AddCommand
//
// AddCommand to queue
// Can't fail
//
///////////////////////////////////////////////////////////////
SJobHandle CDatabaseJobQueueImpl::AddCommand ( EJobType jobType, SConnectionHandle connectionHandle, const SString& strData )
{
    SJobHandle jobHandle = GetNextJobHandle ();

    // Create command
    SCommandQueueItem command;
    command.jobHandle = jobHandle;
    command.jobType = jobType;
    command.connectionHandle = connectionHandle;
    command.strData = strData;

    // Add to queue
    this->m_CS.Lock ();
    this->m_CommandQueue.push_back ( command );
    this->m_CS.Unlock ();

    return jobHandle;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::PollCommand
//
// Find result for previous command
// Returns false if result not ready.
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueueImpl::PollCommand ( SQueryResult& queryResult, SJobHandle jobHandle )
{
    // Set result defaults
    queryResult.connectionHandle = INVALID_DB_HANDLE;
    queryResult.status = STATUS_FAIL;

    // Check job handle is valid
    if ( !MapContains ( m_ActiveJobHandles, jobHandle ) )
    {
        queryResult.strReason = "Invalid query handle";
        return true;
    }

    this->m_CS.Lock ();

    // Take the opportunity to do a little housekeeping and remove unwanted results
    if ( !m_IgnoreResultList.empty () )
    {
        for ( std::list < SResultQueueItem >::iterator iter = this->m_ResultQueue.begin () ; iter != this->m_ResultQueue.end () ; )
        {
            if ( MapContains ( m_IgnoreResultList, iter->jobHandle ) )
            {
                // Found result. Remove from the queue
                UnuseJobHandle ( iter->jobHandle );
                iter = this->m_ResultQueue.erase ( iter );
            }
            else
                ++iter;
        }
    }

    // Now find result with the required job handle
    bool bFound = false;
    for ( std::list < SResultQueueItem >::iterator iter = this->m_ResultQueue.begin () ; iter != this->m_ResultQueue.end () ; ++iter )
    {
        if ( iter->jobHandle == jobHandle )
        {
            // Found result. Copy and remove from the queue
            UnuseJobHandle ( jobHandle );
            queryResult.status = iter->status;
            queryResult.strReason = iter->strReason;
            queryResult.connectionHandle = iter->connectionHandle;
            queryResult.registryResult = iter->registryResult;
            this->m_ResultQueue.erase ( iter );
            bFound = true;
            break;
        }
    }
    this->m_CS.Unlock ();

    return bFound;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::IgnoreCommandResult
//
// Throw away result when this job is done
// Returns false if jobHandle not correct
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueueImpl::IgnoreCommandResult ( SJobHandle jobHandle )
{
    if ( !MapContains ( m_ActiveJobHandles, jobHandle ) )
        return false;       // Invalid query handle

    if ( MapContains ( m_IgnoreResultList, jobHandle ) )
        return false;       // Already ignoring query handle

    MapInsert ( m_IgnoreResultList, jobHandle );
    return true;
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
    this->m_CS.Lock ();

    // All active jobhandles will either be in m_CommandQueue or m_ResultQueue
    for ( std::list < SCommandQueueItem >::iterator iter = this->m_CommandQueue.begin () ; iter != this->m_CommandQueue.end () ; ++iter )
        if ( iter->connectionHandle == connectionHandle )
            MapInsert ( m_IgnoreResultList, iter->jobHandle );

    for ( std::list < SResultQueueItem >::iterator iter = this->m_ResultQueue.begin () ; iter != this->m_ResultQueue.end () ; ++iter )
        if ( iter->connectionHandle == connectionHandle )
            MapInsert ( m_IgnoreResultList, iter->jobHandle );

    this->m_CS.Unlock ();
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
    while ( !m_bTerminateThread )
    {
        // TODO: Use wait instead of sleep
        Sleep ( 1 );

        // Is there a waiting command?
        m_CS.Lock ();
        if ( !m_CommandQueue.empty () )
        {
            // Get next command
            SCommandQueueItem command = m_CommandQueue.front ();
            m_CS.Unlock ();

            // Process command
            SResultQueueItem result;
            ProcessCommand ( result, command );

            // Store result
            m_CS.Lock ();
            // Check command has not been cancelled
            if ( !m_CommandQueue.empty () && command.jobHandle == m_CommandQueue.front ().jobHandle )
            {
                // Remove command
                m_CommandQueue.pop_front ();
                // Add result
                m_ResultQueue.push_back ( result );
            }
        }
        m_CS.Unlock ();
    }

    m_bThreadTerminated = true;
    return NULL;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessCommand
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessCommand ( SResultQueueItem& result, const SCommandQueueItem& command )
{
    if ( command.jobType == JOBTYPE_CONNECT )
        ProcessConnect ( result, command );
    else
    if ( command.jobType == JOBTYPE_DISCONNECT )
        ProcessDisconnect ( result, command );
    else
    if ( command.jobType == JOBTYPE_QUERY )
        ProcessQuery ( result, command );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessConnect
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessConnect ( SResultQueueItem& result, const SCommandQueueItem& command )
{
    // Set default result
    result.jobHandle = command.jobHandle;
    result.connectionHandle = INVALID_DB_HANDLE;
    result.status = STATUS_FAIL;

    // Determine which type manager to use
    std::vector < SString > parts;
    command.strData.Split ( "\1", parts );
    if ( parts.size () < 5 )
    {
        result.strReason = "Internal Error (JobQueueServer:ProcessConnect #1)";
        return;
    }

    CDatabaseType* pTypeManager = MapFindRef ( m_DatabaseTypeMap, parts[0] );
    if ( !pTypeManager )
    {
        result.strReason = "Not valid type";
        return;
    }

    // Get type manager to return a CDatabaseConnection*
    CDatabaseConnection* pConnection = pTypeManager->Connect ( parts[1], parts[2], parts[3], parts[4] );
    if ( !pConnection )
    {
        result.strReason = "Could not connect";
        return;
    }

    if ( !pConnection->IsValid () )
    {
        result.strReason = pConnection->GetLastErrorMessage ();
        pConnection->Release ();
        return;
    }

    // Get handle from CDatabaseConnection*
    SConnectionHandle connectionHandle = MakeHandleForConnection ( pConnection );

    // Set result
    result.status = STATUS_SUCCESS;
    result.connectionHandle = connectionHandle;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessDisconnect
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessDisconnect ( SResultQueueItem& result, const SCommandQueueItem& command )
{
    // Set default result
    result.jobHandle = command.jobHandle;
    result.connectionHandle = INVALID_DB_HANDLE;
    result.status = STATUS_FAIL;

    // CDatabaseConnection* from handle
    CDatabaseConnection* pConnection = GetConnectionFromHandle ( command.connectionHandle );
    if ( !pConnection )
    {
        result.strReason = "Invalid connection";
        return;
    }

    // And disconnect
    RemoveHandleForConnection ( command.connectionHandle, pConnection );
    pConnection->Release ();
    pConnection = NULL;

    // Set result
    result.status = STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueImpl::ProcessQuery
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueImpl::ProcessQuery ( SResultQueueItem& result, const SCommandQueueItem& command )
{
    // Set default result
    result.jobHandle = command.jobHandle;
    result.connectionHandle = INVALID_DB_HANDLE;
    result.status = STATUS_FAIL;

    // CDatabaseConnection* from handle
    CDatabaseConnection* pConnection = GetConnectionFromHandle ( command.connectionHandle );
    if ( !pConnection )
    {
        result.strReason = "Invalid connection";
        return;
    }

    // And query
    if ( !pConnection->Query ( command.strData, result.registryResult ) )
    {
        result.strReason = "Query failed";
        return;
    }

    // Set result
    result.status = STATUS_SUCCESS;
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
    assert ( MapContains ( m_HandleConnectionMap, connectionHandle ) );
    MapRemove ( m_HandleConnectionMap, connectionHandle );
}
