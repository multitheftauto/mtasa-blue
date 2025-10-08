/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"
#include "CGame.h"
#include "CMainConfig.h"

volatile bool CNetBufferWatchDog::ms_bBlockOutgoingSyncPackets = false;
volatile bool CNetBufferWatchDog::ms_bBlockIncomingSyncPackets = false;
volatile bool CNetBufferWatchDog::ms_bCriticalStopThreadNet = false;
volatile bool CNetBufferWatchDog::ms_bVerboseDebug = false;
uint          CNetBufferWatchDog::ms_uiFinishedListSize = 0;
uint          CNetBufferWatchDog::ms_uiOutCommandQueueSize = 0;
uint          CNetBufferWatchDog::ms_uiOutResultQueueSize = 0;
uint          CNetBufferWatchDog::ms_uiInResultQueueSize = 0;

///////////////////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::CNetBufferWatchDog
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetBufferWatchDog::CNetBufferWatchDog(CNetServerBuffer* pNetBuffer, bool bVerboseDebug) : m_pNetBuffer(pNetBuffer)
{
    // Reset some statics
    ms_bBlockOutgoingSyncPackets = false;
    ms_bBlockIncomingSyncPackets = false;
    ms_bCriticalStopThreadNet = false;
    ms_bVerboseDebug = bVerboseDebug;

    if (ms_bVerboseDebug)
        CLogger::LogPrintf("INFO: CNetBufferWatchDog started\n");

    // Start the watch dog thread
    m_pServiceThreadHandle = new CThreadHandle(CNetBufferWatchDog::StaticThreadProc, this);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::~CNetBufferWatchDog
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetBufferWatchDog::~CNetBufferWatchDog()
{
    if (ms_bVerboseDebug)
        CLogger::LogPrintf("INFO: CNetBufferWatchDog stopped\n");

    // Stop the watch dog thread
    StopThread();

    // Delete thread
    SAFE_DELETE(m_pServiceThreadHandle);
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::StopThread
//
// Stop the watch dog thread
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::StopThread()
{
    // Stop the watch dog thread
    shared.m_Mutex.Lock();
    shared.m_bTerminateThread = true;
    shared.m_Mutex.Signal();
    shared.m_Mutex.Unlock();

    for (uint i = 0; i < 5000; i += 15)
    {
        if (shared.m_bThreadTerminated)
            return;

        Sleep(15);
    }

    // If thread not stopped, (async) cancel it
    m_pServiceThreadHandle->Cancel();
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
void* CNetBufferWatchDog::StaticThreadProc(void* pContext)
{
    CThreadHandle::AllowASyncCancel();
    return ((CNetBufferWatchDog*)pContext)->ThreadProc();
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::ThreadProc
//
// Check thread loop
//
///////////////////////////////////////////////////////////////
void* CNetBufferWatchDog::ThreadProc()
{
    shared.m_Mutex.Lock();
    while (!shared.m_bTerminateThread)
    {
        DoChecks();
        shared.m_Mutex.Wait(1000);
    }

    shared.m_bThreadTerminated = true;
    shared.m_Mutex.Unlock();

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
void CNetBufferWatchDog::DoChecks()
{
    // Get queue sizes now
    uint uiFinishedList;
    uint uiOutCommandQueue;
    uint uiOutResultQueue;
    uint uiInResultQueue;
    m_pNetBuffer->GetQueueSizes(uiFinishedList, uiOutCommandQueue, uiOutResultQueue, uiInResultQueue, m_uiGamePlayerCount);

    // Update queue status
    UpdateQueueInfo(m_FinishedListQueueInfo, uiFinishedList, "[Network] FinishedList");
    UpdateQueueInfo(m_OutCommandQueueInfo, uiOutCommandQueue, "[Network] OutCommandQueue");
    UpdateQueueInfo(m_OutResultQueueInfo, uiOutResultQueue, "[Network] OutResultQueue");
    UpdateQueueInfo(m_InResultQueueInfo, uiInResultQueue, "[Network] InResultQueue");

    // Apply queue status
    if (m_OutCommandQueueInfo.status == EQueueStatus::STATUS_OK)
        AllowOutgoingSyncPackets();
    else if (m_OutCommandQueueInfo.status == EQueueStatus::SUSPEND_SYNC)
        BlockOutgoingSyncPackets();

    if (m_InResultQueueInfo.status == EQueueStatus::STATUS_OK)
        AllowIncomingSyncPackets();
    else if (m_InResultQueueInfo.status == EQueueStatus::SUSPEND_SYNC)
        BlockIncomingSyncPackets();

    // Copy sizes for stats only (Unsafe)
    ms_uiFinishedListSize = uiFinishedList;
    ms_uiOutCommandQueueSize = uiOutCommandQueue;
    ms_uiOutResultQueueSize = uiOutResultQueue;
    ms_uiInResultQueueSize = uiInResultQueue;
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::UpdateQueueInfo
//
// Generic queue info updater
//
// Thread:                  check
// Mutex should be locked:  yes
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::UpdateQueueInfo(CQueueInfo& queueInfo, int iQueueSize, const char* szTag)
{
    // Increase thresh levels when more players
    int       iPlayerCountFactor = Clamp<int>(1, 1 + m_uiGamePlayerCount / 100, 5);
    const int iThreshLevel1 = 100000 * iPlayerCountFactor;
    const int iThreshLevel2 = 200000 * iPlayerCountFactor;
    const int iThreshLevel3 = 300000 * iPlayerCountFactor;
    const int iThreshLevel4 = 400000 * iPlayerCountFactor;
    const int iThreshLevel5 = 500000 * iPlayerCountFactor;

    queueInfo.m_SizeHistory.AddPoint(iQueueSize);
    if (queueInfo.status == EQueueStatus::STATUS_OK)
    {
        //  if Queue > 200,000 for 5 seconds
        //      stop related queue until it is below 100,000
        if (queueInfo.m_SizeHistory.GetLowestPointSince(5) > iThreshLevel2)
        {
            CLogger::LogPrintf("%s > %d msgs. This is due to server overload or script freeze\n", szTag, iThreshLevel2);
            queueInfo.status = EQueueStatus::SUSPEND_SYNC;
        }
    }
    else if (queueInfo.status == EQueueStatus::SUSPEND_SYNC)
    {
        //  if Queue < 100,000 within the last 5 seconds
        //      resume related queue
        if (queueInfo.m_SizeHistory.GetLowestPointSince(5) < iThreshLevel1)
        {
            queueInfo.status = EQueueStatus::STATUS_OK;
        }

        //  if Queue > 300,000 for 30 seconds
        //      terminate threadnet
        if (queueInfo.m_SizeHistory.GetLowestPointSince(30) > iThreshLevel3)
        {
            CLogger::ErrorPrintf("%s > %d msgs for 30 seconds\n", szTag, iThreshLevel3);
            CLogger::ErrorPrintf("Something is wrong - Switching from threaded sync mode\n");
            queueInfo.status = EQueueStatus::STOP_THREAD_NET;
            ms_bCriticalStopThreadNet = true;
        }
    }
    else if (queueInfo.status == EQueueStatus::STOP_THREAD_NET)
    {
        //  if Queue > 400,000 for 60 seconds
        //      shutdown
        if (queueInfo.m_SizeHistory.GetLowestPointSince(60) > iThreshLevel4)
        {
            CLogger::ErrorPrintf("%s > %d msgs for 60 seconds\n", szTag, iThreshLevel4);
            CLogger::ErrorPrintf("Something is very wrong - Shutting down server\n");
            queueInfo.status = EQueueStatus::SHUTDOWN;
            g_pGame->SetIsFinished(true);
        }
    }
    else if (queueInfo.status == EQueueStatus::SHUTDOWN)
    {
        //  if Queue > 500,000 for 90 seconds
        //      terminate server
        if (queueInfo.m_SizeHistory.GetLowestPointSince(90) > iThreshLevel5)
        {
            CLogger::ErrorPrintf("%s > %d msgs for 90 seconds\n", szTag, iThreshLevel5);
            CLogger::ErrorPrintf("Something is badly wrong right here - Terminating server\n");
            queueInfo.status = EQueueStatus::TERMINATE;
#ifdef WIN32
            TerminateProcess(GetCurrentProcess(), 1);
#else
            exit(1);
#endif
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::BlockOutgoingSyncPackets
//
//
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::BlockOutgoingSyncPackets()
{
    if (!ms_bBlockOutgoingSyncPackets)
    {
        ms_bBlockOutgoingSyncPackets = true;
        CLogger::LogPrintf("Temporarily suspending outgoing sync packets\n");
    }
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::BlockOutgoingSyncPackets
//
//
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::AllowOutgoingSyncPackets()
{
    if (ms_bBlockOutgoingSyncPackets)
    {
        ms_bBlockOutgoingSyncPackets = false;
        CLogger::LogPrintf("Resuming outgoing sync packets\n");
    }
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::BlockOutgoingSyncPackets
//
//
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::BlockIncomingSyncPackets()
{
    if (!ms_bBlockIncomingSyncPackets)
    {
        ms_bBlockIncomingSyncPackets = true;
        CLogger::LogPrintf("Temporarily suspending incoming sync packets\n");
    }
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::BlockOutgoingSyncPackets
//
//
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::AllowIncomingSyncPackets()
{
    if (ms_bBlockIncomingSyncPackets)
    {
        ms_bBlockIncomingSyncPackets = false;
        CLogger::LogPrintf("Resuming incoming sync packets\n");
    }
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::DoPulse
//
// Static function
// Check for critical sitayshun
//
///////////////////////////////////////////////////////////////
void CNetBufferWatchDog::DoPulse()
{
    if (CSimControl::IsSimSystemEnabled())
    {
        if (CNetBufferWatchDog::ms_bCriticalStopThreadNet)
        {
            CNetBufferWatchDog::ms_bCriticalStopThreadNet = false;
            g_pGame->GetConfig()->SetSetting("threadnet", "0", false);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::IsUnreliableSyncPacket
//
// Static function
//
///////////////////////////////////////////////////////////////
bool CNetBufferWatchDog::IsUnreliableSyncPacket(uchar ucPacketID)
{
    switch (ucPacketID)
    {
        case PACKET_ID_PLAYER_KEYSYNC:
        case PACKET_ID_PLAYER_PURESYNC:
        case PACKET_ID_PLAYER_VEHICLE_PURESYNC:
        case PACKET_ID_LIGHTSYNC:
        case PACKET_ID_VEHICLE_RESYNC:
        case PACKET_ID_RETURN_SYNC:
        case PACKET_ID_OBJECT_SYNC:
        case PACKET_ID_UNOCCUPIED_VEHICLE_SYNC:
        case PACKET_ID_PED_SYNC:
        case PACKET_ID_CAMERA_SYNC:
            return true;
        default:
            return false;
    };
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::CanSendPacket
//
// Static function
//
///////////////////////////////////////////////////////////////
bool CNetBufferWatchDog::CanSendPacket(uchar ucPacketID)
{
    if (CSimControl::IsSimSystemEnabled())
        if (CNetBufferWatchDog::ms_bBlockOutgoingSyncPackets)
            return !IsUnreliableSyncPacket(ucPacketID);
    return true;
}

///////////////////////////////////////////////////////////////
//
// CNetBufferWatchDog::CanReceivePacket
//
// Static function
//
///////////////////////////////////////////////////////////////
bool CNetBufferWatchDog::CanReceivePacket(uchar ucPacketID)
{
    if (CSimControl::IsSimSystemEnabled())
        if (CNetBufferWatchDog::ms_bBlockIncomingSyncPackets)
            return !IsUnreliableSyncPacket(ucPacketID);
    return true;
}
