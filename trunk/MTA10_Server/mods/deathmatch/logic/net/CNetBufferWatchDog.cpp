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

CActionHistorySet g_HistorySet;

///////////////////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::CNetBufferWatchDog
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetBufferWatchDog::CNetBufferWatchDog ( CNetServerBuffer* pNetBuffer )
    : m_pNetBuffer ( pNetBuffer )
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
        shared.m_Mutex.Wait ( 1000 );
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

    uint uiFinishedList;
    uint uiOutCommandQueue;
    uint uiOutResultQueue;
    uint uiInResultQueue;
    m_pNetBuffer->GetQueueSizes ( uiFinishedList, uiOutCommandQueue, uiOutResultQueue, uiInResultQueue );

    CheckQueueSize ( 1000, uiFinishedList, "FinishedList", m_uiFinishedListHigh );
    CheckQueueSize ( 1000, uiOutCommandQueue, "OutCommandQueue", m_uiOutCommandQueueHigh );
    CheckQueueSize ( 1000, uiOutResultQueue, "OutResultQueue", m_uiOutResultQueueHigh );
    CheckQueueSize ( 1000, uiInResultQueue, "InResultQueue", m_uiInResultQueueHigh );
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
    if ( history.bChanged )
    {
        history.bChanged = false;
        history.uiLastChangedTime = GetTickCount32 ();
    }

    if ( !history.uiLastChangedTime )
        return;

    uint uiAge = GetTickCount32 () - history.uiLastChangedTime;
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
        CLogger::LogPrintf ( "INFO: %s thread last action age: %d ticks.\n"
                                    , szTag
                                    , uiAge
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
#ifdef MTA_DEBUG
    uint uiMinLevel = 5;
#else
    uint uiMinLevel = uiSizeLimit;
#endif

    uiHigh = Max < uint > ( uiMinLevel, uiHigh );

    bool bShowMessage = false;
    if ( uiSize > uiHigh )
    {
        uiHigh = uiSize + Max < uint > ( 10, uiSize / 2 );
        bShowMessage = true;
    }
    else
    if ( uiSize < uiHigh / 4 )
    {
        if ( uiHigh > uiMinLevel )
        {
            uiHigh = uiSize / 2;
            bShowMessage = true;
        }
    }

    uiHigh = Max < uint > ( uiMinLevel, uiHigh );

    if ( bShowMessage )
    {
        SString strMessage ( "INFO: %s queue size: %d.", szTag, uiSize );
        if ( uiSize > uiMinLevel )
            strMessage += SString ( " (Next warning will be outside %d to %d)", uiHigh / 4, uiHigh );
        CLogger::LogPrint ( strMessage + "\n" );
    }
}
