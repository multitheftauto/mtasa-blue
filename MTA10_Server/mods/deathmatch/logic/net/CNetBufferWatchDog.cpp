/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"

namespace
{
    // Used in StaticProcessPacket and elsewhere
    CNetBufferWatchDog*       ms_pNetServerBuffer = NULL;
}

CActionHistorySet g_HistorySet;

///////////////////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::CNetBufferWatchDog
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetBufferWatchDog::CNetBufferWatchDog ( void )
{
    CLogger::LogPrintf ( "INFO: CNetBufferWatchDog started\n" );
    // Start the job queue processing thread
    m_pServiceThreadHandle = new CThreadHandle ( CNetBufferWatchDog::StaticThreadProc, this );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::~CNetBufferWatchDog
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetBufferWatchDog::~CNetBufferWatchDog ( void )
{
    CLogger::LogPrintf ( "INFO: CNetBufferWatchDog stopped\n" );

    // Stop the job queue processing thread
    StopThread ();

    // Delete thread
    SAFE_DELETE ( m_pServiceThreadHandle );
}


///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::StopThread
//
// Stop the job queue processing thread
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::StopThread ( void )
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


//
//
//
// Check thread
//
//
//

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::StaticThreadProc
//
// static function
// Thread entry point
//
///////////////////////////////////////////////////////////////
void* CNetBufferWatchDog::StaticThreadProc ( void* pContext )
{
    CThreadHandle::AllowASyncCancel ();
    return ((CNetBufferWatchDog*)pContext)->ThreadProc ();
}


///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::ThreadProc
//
// Check thread loop
//
///////////////////////////////////////////////////////////////
void* CNetBufferWatchDog::ThreadProc ( void )
{
    shared.m_Mutex.Lock ();
    while ( !shared.m_bTerminateThread )
    {
        DoChecks ();
        shared.m_Mutex.Wait ( 100 );
    }

    shared.m_bThreadTerminated = true;
    shared.m_Mutex.Unlock ();

    return NULL;
}


///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::DoChecks
//
// Thread:                  check
// Mutex should be locked:  yes
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::DoChecks ( void )
{
    // Check when main thread last updated anything
    CheckActionHistory ( g_HistorySet.main, "Main", m_uiMainAgeHigh );
    CheckActionHistory ( g_HistorySet.sync, "Sync", m_uiSyncAgeHigh );

    CheckQueueSize ( 100, shared.m_uiFinishedList, "FinishedList", m_uiFinishedListHigh );
    CheckQueueSize ( 100, shared.m_uiOutCommandQueue, "OutCommandQueue", m_uiOutCommandQueueHigh );
    CheckQueueSize ( 100, shared.m_uiOutResultQueue, "OutResultQueue", m_uiOutResultQueueHigh );
    CheckQueueSize ( 100, shared.m_uiInResultQueue, "InResultQueue", m_uiInResultQueueHigh );
}


///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::CheckActionHistory
//
// Thread:                  check
// Mutex should be locked:  yes
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::CheckActionHistory ( CActionHistory& history, const char* szTag, uint& uiHigh )
{
    CActionHistory::SItem lastItem;
    if ( !history.GetLastItem ( lastItem ) )
        return;

    uint uiAge = GetTickCount32 () - lastItem.uiWhen;
    if ( uiAge < 1000 * 5 )
        uiHigh = 10;

    bool bShowMessage = false;
    if ( uiAge > 1000 * uiHigh )
    {
        uiHigh = uiHigh + 10;
        bShowMessage = true;
    }

    if ( bShowMessage )
    {
        CLogger::LogPrintf ( "INFO: %s thread last action age: %d ticks. (who:%d where:%d what:%d extra:%d)\n"
                                    , szTag
                                    , uiAge
                                    , lastItem.uiWho
                                    , lastItem.uiWhere
                                    , lastItem.uiWhat
                                    , lastItem.iExtra
                                );
    }
}


///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::CheckQueueSize
//
// Thread:                  check
// Mutex should be locked:  yes
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::CheckQueueSize ( uint uiSizeLimit, uint uiSize, const char* szTag, uint& uiHigh )
{
    uiHigh = Max < uint > ( 100, uiHigh );

    bool bShowMessage = false;
    if ( uiSize > uiHigh )
    {
        uiHigh = uiSize + Max < uint > ( 10, uiSize / 2 );
        bShowMessage = true;
    }

    if ( bShowMessage )
    {
        CLogger::LogPrintf ( "INFO: %s queue size: %d. (Next warning will be at %d)\n"
                                    , szTag
                                    , uiSize
                                    , uiHigh
                                );
    }
}


///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::RecordQueueSizes
//
// Thread:                  any
// Mutex should be locked:  no
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::RecordQueueSizes ( uint uiFinishedList , uint uiOutCommandQueue, uint uiOutResultQueue, uint uiInResultQueue )
{
    shared.m_Mutex.Lock ();
    shared.m_uiFinishedList = uiFinishedList;
    shared.m_uiOutCommandQueue = uiOutCommandQueue;
    shared.m_uiOutResultQueue = uiOutResultQueue;
    shared.m_uiInResultQueue = uiInResultQueue;
    shared.m_Mutex.Unlock ();
}
