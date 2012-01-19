/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/core/CCompressorJobQueue.cpp
*  PURPOSE:     Threaded job queue
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CCompressorJobQueue.h"
#include "SharedUtil.Thread.h"


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl
//
//
///////////////////////////////////////////////////////////////
class CCompressorJobQueueImpl : public CCompressorJobQueue
{
public:
    ZERO_ON_NEW
                                CCompressorJobQueueImpl       ( void );
    virtual                     ~CCompressorJobQueueImpl      ( void );

    // Main thread functions
    virtual void                DoPulse                     ( void );
    virtual CCompressJobData*   AddCommand                  ( uint uiSizeX, uint uiSizeY, uint uiQuality, uint uiTimeSpentInQueue, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, const CBuffer& buffer );
    virtual bool                PollCommand                 ( CCompressJobData* pJobData, uint uiTimeout );
    virtual bool                FreeCommand                 ( CCompressJobData* pJobData );

protected:
    void                        StopThread                  ( void );
    void                        RemoveUnwantedResults       ( void );
    void                        IgnoreJobResults            ( CCompressJobData* pJobData );

    // Other thread functions
    static void*                StaticThreadProc            ( void* pContext );
    void*                       ThreadProc                  ( void );
    void                        ProcessCommand              ( CCompressJobData* pJobData );
    void                        ProcessCompress             ( uint uiSizeX, uint uiSizeY, uint uiQuality, const CBuffer& inBuffer, CBuffer& outBuffer );

    // Main thread variables
    CThreadHandle*                          m_pServiceThreadHandle;
    std::set < CCompressJobData* >          m_IgnoreResultList;
    std::set < CCompressJobData* >          m_FinishedList;         // Result has been used, will be deleted next pulse

    // Other thread variables
    // -none-

    // Shared variables
    struct
    {
        bool                                m_bTerminateThread;
        bool                                m_bThreadTerminated;
        std::list < CCompressJobData* >     m_CommandQueue;
        std::list < CCompressJobData* >     m_ResultQueue;
        CComboMutex                         m_Mutex;
    } shared;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CCompressorJobQueue* NewCompressorJobQueue ( void )
{
    return new CCompressorJobQueueImpl ();
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::CCompressorJobQueueImpl
//
// Init known database types and start the job service thread
//
///////////////////////////////////////////////////////////////
CCompressorJobQueueImpl::CCompressorJobQueueImpl ( void )
{
    // Start the job queue processing thread
    m_pServiceThreadHandle = new CThreadHandle ( CCompressorJobQueueImpl::StaticThreadProc, this );
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::CCompressorJobQueueImpl
//
// Stop threads and delete everything
//
///////////////////////////////////////////////////////////////
CCompressorJobQueueImpl::~CCompressorJobQueueImpl ( void )
{
    // Stop the job queue processing thread
    StopThread ();

    // Delete thread
    SAFE_DELETE ( m_pServiceThreadHandle );
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::StopThread
//
// Stop the job queue processing thread
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::StopThread ( void )
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
// CCompressorJobQueueImpl::AddCommand
//
// AddCommand to queue
// Can't fail
//
///////////////////////////////////////////////////////////////
CCompressJobData* CCompressorJobQueueImpl::AddCommand ( uint uiSizeX, uint uiSizeY, uint uiQuality, uint uiTimeSpentInQueue, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, const CBuffer& buffer )
{
    // Create command
    CCompressJobData* pJobData = new CCompressJobData ();
    pJobData->command.uiSizeX = uiSizeX;
    pJobData->command.uiSizeY = uiSizeY;
    pJobData->command.uiQuality = uiQuality;
    pJobData->command.buffer = buffer;

    pJobData->SetCallback ( pfnScreenShotCallback, uiTimeSpentInQueue );

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
// CCompressorJobQueueImpl::DoPulse
//
// Check if any callback functions are due
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::DoPulse ( void )
{
    shared.m_Mutex.Lock ();

again:
    // Delete finished
    for ( std::set < CCompressJobData* >::iterator iter = m_FinishedList.begin () ; iter != m_FinishedList.end () ; )
    {
        CCompressJobData* pJobData = *iter;
        m_FinishedList.erase ( iter++ );
        // Check not refed
        assert ( !ListContains ( shared.m_CommandQueue, pJobData ) );
        assert ( !ListContains ( shared.m_ResultQueue, pJobData ) );
        assert ( !MapContains ( m_IgnoreResultList, pJobData ) );
        assert ( !MapContains ( m_FinishedList, pJobData ) );

        assert ( !pJobData->HasCallback () );
        SAFE_DELETE( pJobData );
    }

    // Remove ignored
    RemoveUnwantedResults ();

    // Do pending callbacks
    for ( std::list < CCompressJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
    {
        CCompressJobData* pJobData = *iter;

        if ( pJobData->HasCallback () )
        {
            shared.m_Mutex.Unlock ();
            pJobData->ProcessCallback ();
            shared.m_Mutex.Lock ();

            // Redo from the top to ensure everything is consistent
            goto again;
        }
    }

    shared.m_Mutex.Unlock ();
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::RemoveUnwantedResults
//
// Check result queue items for match with ignore list items.
// * Must be called from inside a locked section *
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::RemoveUnwantedResults ( void )
{
    if ( m_IgnoreResultList.empty () )
        return;

again:
    for ( std::list < CCompressJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; )
    {
        CCompressJobData* pJobData = *iter;
        if ( MapContains ( m_IgnoreResultList, pJobData ) )
        {
            // Found result to ignore, remove from result and ignore lists, add to finished list
            iter = shared.m_ResultQueue.erase ( iter );
            MapRemove ( m_IgnoreResultList, pJobData );
            pJobData->stage = EJobStage::FINISHED;
            MapInsert ( m_FinishedList, pJobData );

            // Do callback incase any cleanup is needed
            if ( pJobData->HasCallback () )
            {
                shared.m_Mutex.Unlock ();
                pJobData->ProcessCallback ();
                shared.m_Mutex.Lock ();
                goto again;
            }
        }
        else
            ++iter;
    }
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::PollCommand
//
// Find result for previous command
// Returns false if result not ready.
//
///////////////////////////////////////////////////////////////
bool CCompressorJobQueueImpl::PollCommand ( CCompressJobData* pJobData, uint uiTimeout )
{
    bool bFound = false;

    shared.m_Mutex.Lock ();
    while ( true )
    {
        // Remove ignored before checking
        RemoveUnwantedResults ();

        // Find result with the required job handle
        for ( std::list < CCompressJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
        {
            if ( pJobData == *iter )
            {
                // Found result. Remove from the result queue and flag return value
                shared.m_ResultQueue.erase ( iter );
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
                break;
            }
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
// CCompressorJobQueueImpl::FreeCommand
//
// Throw away result when this job is done
// Returns false if jobHandle not correct
//
///////////////////////////////////////////////////////////////
bool CCompressorJobQueueImpl::FreeCommand ( CCompressJobData* pJobData )
{
    if ( MapContains ( m_IgnoreResultList, pJobData ) )
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
            OutputDebugLine ( "FreeCommand: Serious problem #2 here\n" );
        return false; 
    }

    IgnoreJobResults ( pJobData );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::IgnoreJobResult
//
// Throw away the result for this job
// * Assumed job data is in either m_CommandQueue or m_ResultQueue *
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::IgnoreJobResults ( CCompressJobData* pJobData )
{
    dassert ( pJobData->stage <= EJobStage::RESULT );
    dassert ( !MapContains ( m_FinishedList, pJobData ) );
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
// CCompressorJobQueueImpl::StaticThreadProc
//
// static function
// Thread entry point
//
///////////////////////////////////////////////////////////////
void* CCompressorJobQueueImpl::StaticThreadProc ( void* pContext )
{
    CThreadHandle::AllowASyncCancel ();
    return ((CCompressorJobQueueImpl*)pContext)->ThreadProc ();
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::ThreadProc
//
// Job service loop
//
///////////////////////////////////////////////////////////////
void* CCompressorJobQueueImpl::ThreadProc ( void )
{
    shared.m_Mutex.Lock ();
    while ( !shared.m_bTerminateThread )
    {
        // Is there a waiting command?
        if ( shared.m_CommandQueue.empty () )
        {
            shared.m_Mutex.Wait ( -1 );
        }
        else
        {
            // Get next command
            CCompressJobData* pJobData = shared.m_CommandQueue.front ();
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
// CCompressorJobQueueImpl::ProcessCommand
//
//
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::ProcessCommand ( CCompressJobData* pJobData )
{
    uint uiSizeX = pJobData->command.uiSizeX;
    uint uiSizeY = pJobData->command.uiSizeY;
    uint uiQuality = pJobData->command.uiQuality;
    const CBuffer& inBuffer = pJobData->command.buffer;
    CBuffer& outBuffer = pJobData->result.buffer;

    // JPEG compress here
    JpegEncode ( uiSizeX, uiSizeY, uiQuality, inBuffer, outBuffer );

    pJobData->result.status = EJobResult::SUCCESS;
}




//
//
//  CCompressJobData
//
//
//


///////////////////////////////////////////////////////////////
//
// CCompressJobData::SetCallback
//
// Set callback for a job data
// Returns false if callback could not be set
//
///////////////////////////////////////////////////////////////
bool CCompressJobData::SetCallback ( PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, uint uiTimeSpentInQueue )
{
    if ( callback.bSet )
        return false;   // One has already been set

    if ( this->stage > EJobStage::RESULT )
        return false;   // Too late to set a callback now

    // Set new
    callback.uiTimeSpentInQueue = uiTimeSpentInQueue;
    callback.pfnScreenShotCallback = pfnScreenShotCallback;
    callback.bSet = true;
    return true;
}


///////////////////////////////////////////////////////////////
//
// CCompressJobData::HasCallback
//
// Returns true if callback has been set and has not been called yet
//
///////////////////////////////////////////////////////////////
bool CCompressJobData::HasCallback ( void )
{
    return callback.bSet && !callback.bDone;
}


///////////////////////////////////////////////////////////////
//
// CCompressJobData::ProcessCallback
//
// Do callback
//
///////////////////////////////////////////////////////////////
void CCompressJobData::ProcessCallback ( void )
{
    assert ( HasCallback () );
    callback.bDone = true;
    callback.pfnScreenShotCallback( result.buffer, callback.uiTimeSpentInQueue );
}
