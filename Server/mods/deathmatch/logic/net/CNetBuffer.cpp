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
#include "packets/CPlayerDisconnectedPacket.h"

SThreadCPUTimesStore g_SyncThreadCPUTimes;
uint                 g_uiNetSentByteCounter = 0;

namespace
{
    // Used in StaticProcessPacket and elsewhere
    CNetServerBuffer* ms_pNetServerBuffer = NULL;

    // Used for getting stats (non blocking)
    bool                 ms_bNetStatisticsLastSavedValid = false;
    NetStatistics        ms_NetStatisticsLastSaved;
    NetServerPlayerID    ms_NetStatisticsLastFor;
    bool                 ms_bNetStatisticsLastResult;
    bool                 ms_bBandwidthStatisticsLastSavedValid = false;
    SBandwidthStatistics ms_BandwidthStatisticsLastSaved;
    bool                 ms_bPingStatusLastSavedValid = false;
    SFixedString<32>     ms_PingStatusLastSaved;
    bool                 ms_bNetRouteLastSavedValid = false;
    SFixedString<32>     ms_NetRouteLastSaved;

    // Sync thread stats
    uint   ms_StatsResetCounters;
    int    ms_StatsLoopCount;
    TIMEUS ms_StatsRecvTimeTotalUs;
    TIMEUS ms_StatsRecvTimeMaxUs;
    TIMEUS ms_StatsSendTimeTotalUs;
    TIMEUS ms_StatsSendTimeMaxUs;
    uint   ms_StatsRecvMsgsTotal;
    uint   ms_StatsRecvMsgsMax;
    uint   ms_StatsSendCmdsTotal;
    uint   ms_StatsSendCmdsMax;
    uint   ms_StatsRecvNumMessages;
    uint   ms_StatsSendNumCommands;
    TIMEUS ms_StatsTimePoint1;
    TIMEUS ms_StatsTimePoint2;
    TIMEUS ms_StatsTimePoint3;

    // Sync thread stats helper functions
    void UpdateStatsPreDoPulse()
    {
        ms_StatsTimePoint1 = GetTimeUs();
        ms_StatsRecvNumMessages = 0;
    }

    void UpdateStatsPreCommands(int uiNumCommands)
    {
        ms_StatsTimePoint2 = GetTimeUs();
        ms_StatsTimePoint3 = ms_StatsTimePoint2;
        ms_StatsSendNumCommands = uiNumCommands;
    }

    void UpdateStatsFinish()
    {
        if (ms_StatsSendNumCommands != 0)
            ms_StatsTimePoint3 = GetTimeUs();

        static CElapsedTime lastStatsResetTime;
        static uint         uiLastResetValue = 10;
        // Do reset if requested from other thread, or it's been a little while
        if (ms_StatsResetCounters != uiLastResetValue || lastStatsResetTime.Get() > 20000)
        {
            lastStatsResetTime.Reset();
            uiLastResetValue = ms_StatsResetCounters;
            ms_StatsLoopCount = 0;
            ms_StatsRecvTimeTotalUs = 0;
            ms_StatsRecvTimeMaxUs = 0;
            ms_StatsRecvMsgsTotal = 0;
            ms_StatsRecvMsgsMax = 0;
            ms_StatsSendTimeTotalUs = 0;
            ms_StatsSendTimeMaxUs = 0;
            ms_StatsSendCmdsTotal = 0;
            ms_StatsSendCmdsMax = 0;
        }

        ms_StatsLoopCount++;

        TIMEUS llRecvTimeUs = ms_StatsTimePoint2 - ms_StatsTimePoint1;
        ms_StatsRecvTimeTotalUs += llRecvTimeUs;
        ms_StatsRecvTimeMaxUs = std::max(ms_StatsRecvTimeMaxUs, llRecvTimeUs);

        TIMEUS llSendTimeUs = ms_StatsTimePoint3 - ms_StatsTimePoint2;
        ms_StatsSendTimeTotalUs += llSendTimeUs;
        ms_StatsSendTimeMaxUs = std::max(ms_StatsSendTimeMaxUs, llSendTimeUs);

        ms_StatsRecvMsgsTotal += ms_StatsRecvNumMessages;
        ms_StatsRecvMsgsMax = std::max(ms_StatsRecvMsgsMax, ms_StatsRecvNumMessages);

        ms_StatsSendCmdsTotal += ms_StatsSendNumCommands;
        ms_StatsSendCmdsMax = std::max(ms_StatsSendCmdsMax, ms_StatsSendNumCommands);
    }
}            // namespace

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::CNetServerBuffer
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetServerBuffer::CNetServerBuffer(CSimPlayerManager* pSimPlayerManager)
{
    m_pSimPlayerManager = pSimPlayerManager;
    ms_pNetServerBuffer = this;
    m_pRealNetServer = g_pRealNetServer;

    // Begin the watchdog
    shared.m_pWatchDog = new CNetBufferWatchDog(this, false);

    // Start the job queue processing thread
    m_pServiceThreadHandle = new CThreadHandle(CNetServerBuffer::StaticThreadProc, this);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::~CNetServerBuffer
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetServerBuffer::~CNetServerBuffer()
{
    // Stop the job queue processing thread
    StopThread();

    // Delete thread
    SAFE_DELETE(m_pServiceThreadHandle);

    // Stop the debug watchdog
    SAFE_DELETE(shared.m_pWatchDog);

    ms_pNetServerBuffer = NULL;
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetAutoPulseEnabled
//
// Enable pulsing automatically in the sync thread
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::SetAutoPulseEnabled(bool bEnable)
{
    shared.m_Mutex.Lock();
    shared.m_bAutoPulse = bEnable;
    shared.m_Mutex.Unlock();
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::StopThread
//
// Stop the job queue processing thread
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::StopThread()
{
    // Stop the job queue processing thread
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

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::StartNetwork
//
// BLOCKING
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::StartNetwork(const char* szIP, unsigned short usServerPort, unsigned int uiAllowedPlayers, const char* szServerName)
{
    SStartNetworkArgs* pArgs = new SStartNetworkArgs(szIP, usServerPort, uiAllowedPlayers, szServerName);
    AddCommandAndWait(pArgs);
    return pArgs->result;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::StopNetwork
//
// BLOCKING
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::StopNetwork()
{
    SStopNetworkArgs* pArgs = new SStopNetworkArgs();
    AddCommandAndWait(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::DoPulse
//
//
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::DoPulse()
{
    // Schedule a net pulse
    SDoPulseArgs* pArgs = new SDoPulseArgs();
    AddCommandAndFree(pArgs);

    // Read incoming packets
    ProcessIncoming();

    // See if it's time to check the thread 'fps'
    if (m_TimeThreadFPSLastCalced.Get() > 1000)
    {
        m_TimeThreadFPSLastCalced.Reset();
        float fSyncFPS;
        shared.m_Mutex.Lock();
        fSyncFPS = static_cast<float>(shared.m_iThreadFrameCount);
        shared.m_iThreadFrameCount = 0;
        shared.m_iuGamePlayerCount = g_pGame->GetPlayerManager()->Count();            // Also update player count here (for scaling buffer size checks)
        shared.m_Mutex.Unlock();

        // Compress high counts
        if (fSyncFPS > 500)
            fSyncFPS = pow(fSyncFPS - 500, 0.6f) + 500;

        // Clamp the max change
        float fChange = fSyncFPS - m_fSmoothThreadFPS;
        float fMaxChange = 50;
        fChange = Clamp(-fMaxChange, fChange, fMaxChange);
        m_fSmoothThreadFPS = Lerp(m_fSmoothThreadFPS, 0.4f, m_fSmoothThreadFPS + fChange);
        g_pGame->SetSyncFPS(static_cast<int>(m_fSmoothThreadFPS));
    }
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::RegisterPacketHandler
//
// BLOCKING. Called once at startup
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::RegisterPacketHandler(PPACKETHANDLER pfnPacketHandler)
{
    m_pfnDMPacketHandler = pfnPacketHandler;

    // Replace handler if not NULL
    if (pfnPacketHandler)
        pfnPacketHandler = CNetServerBuffer::StaticProcessPacket;

    SRegisterPacketHandlerArgs* pArgs = new SRegisterPacketHandlerArgs(pfnPacketHandler);
    AddCommandAndWait(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetNetworkStatistics
//
// Blocking on first call with new PlayerID.
// Subsequent calls with the same PlayerID are non blocking but return previous results
//
///////////////////////////////////////////////////////////////////////////
void GetNetworkStatisticsCallback(CNetJobData* pJobData, void* pContext)
{
    NetStatistics* pStore = (NetStatistics*)pContext;
    if (pJobData->stage == EJobStage::RESULT)
    {
        ms_pNetServerBuffer->PollCommand(pJobData, -1);
        ms_NetStatisticsLastSaved = *pStore;
    }
    delete pStore;
}

bool CNetServerBuffer::GetNetworkStatistics(NetStatistics* pDest, const NetServerPlayerID& PlayerID)
{
    // Blocking read required?
    if (!ms_bNetStatisticsLastSavedValid || ms_NetStatisticsLastFor != PlayerID)
    {
        // Do blocking read
        SGetNetworkStatisticsArgs* pArgs = new SGetNetworkStatisticsArgs(pDest, PlayerID);
        AddCommandAndWait(pArgs);

        // Save results
        ms_NetStatisticsLastSaved = *pDest;
        ms_NetStatisticsLastFor = PlayerID;
        ms_bNetStatisticsLastResult = pArgs->result;
        ms_bNetStatisticsLastSavedValid = true;
        return ms_bNetStatisticsLastResult;
    }

    // Start a new async read,
    NetStatistics*             pStore = new NetStatistics();
    SGetNetworkStatisticsArgs* pArgs = new SGetNetworkStatisticsArgs(pStore, PlayerID);
    AddCommandAndCallback(pArgs, GetNetworkStatisticsCallback, pStore);

    // but use results from previous
    *pDest = ms_NetStatisticsLastSaved;
    return ms_bNetStatisticsLastResult;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetPacketStats
//
// Uses stats gathered here
//
///////////////////////////////////////////////////////////////////////////
const SPacketStat* CNetServerBuffer::GetPacketStats()
{
    m_TimeSinceGetPacketStats.Reset();
    return m_PacketStatList[0];
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetBandwidthStatistics
//
// Blocking on first call.
// Subsequent calls are non blocking but return previous results
//
///////////////////////////////////////////////////////////////////////////
void GetBandwidthStatisticsCallback(CNetJobData* pJobData, void* pContext)
{
    SBandwidthStatistics* pStore = (SBandwidthStatistics*)pContext;
    if (pJobData->stage == EJobStage::RESULT)
    {
        ms_pNetServerBuffer->PollCommand(pJobData, -1);
        ms_BandwidthStatisticsLastSaved = *pStore;
    }
    delete pStore;
}

bool CNetServerBuffer::GetBandwidthStatistics(SBandwidthStatistics* pDest)
{
    // Blocking read required?
    if (!ms_bBandwidthStatisticsLastSavedValid)
    {
        // Do blocking read
        SGetBandwidthStatisticsArgs* pArgs = new SGetBandwidthStatisticsArgs(pDest);
        AddCommandAndWait(pArgs);

        // Save results
        ms_BandwidthStatisticsLastSaved = *pDest;
        ms_bBandwidthStatisticsLastSavedValid = true;
        return pArgs->result;
    }

    // Start a new async read,
    SBandwidthStatistics*        pStore = new SBandwidthStatistics();
    SGetBandwidthStatisticsArgs* pArgs = new SGetBandwidthStatisticsArgs(pStore);
    AddCommandAndCallback(pArgs, GetBandwidthStatisticsCallback, pStore);

    // but use results from previous
    *pDest = ms_BandwidthStatisticsLastSaved;
    return true;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetNetPerformanceStatistics
//
// Thread safe, but numbers could be wrong
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::GetNetPerformanceStatistics(SNetPerformanceStatistics* pDest, bool bResetCounters)
{
    return m_pRealNetServer->GetNetPerformanceStatistics(pDest, bResetCounters);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetSyncThreadStatistics
//
// Thread safe, but numbers could be wrong
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::GetSyncThreadStatistics(SSyncThreadStatistics* pDest, bool bResetCounters)
{
    if (!pDest)
        return false;

    int iNumActiveRemoteSystems = std::max(1U, g_pGame->GetPlayerManager()->Count());
    int iLoopCount = std::max(1, ms_StatsLoopCount);
    pDest->uiRecvTimeAvgUs = (uint)(ms_StatsRecvTimeTotalUs / iLoopCount);
    pDest->uiSendTimeAvgUs = (uint)(ms_StatsSendTimeTotalUs / iLoopCount);

    pDest->uiRecvTimeMaxUs = (uint)(ms_StatsRecvTimeMaxUs);
    pDest->uiSendTimeMaxUs = (uint)(ms_StatsSendTimeMaxUs);

    pDest->fRecvMsgsAvg = ms_StatsRecvMsgsTotal / (float)iNumActiveRemoteSystems / (float)iLoopCount;
    pDest->uiRecvMsgsMax = ms_StatsRecvMsgsMax / iNumActiveRemoteSystems;
    pDest->fSendCmdsAvg = ms_StatsSendCmdsTotal / (float)iNumActiveRemoteSystems / (float)iLoopCount;
    pDest->uiSendCmdsMax = ms_StatsSendCmdsMax / iNumActiveRemoteSystems;

    if (bResetCounters)
        ms_StatsResetCounters++;

    return true;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetPingStatus
//
// Blocking on first call.
// Subsequent calls are non blocking but return previous results
//
///////////////////////////////////////////////////////////////////////////
void GetPingStatusCallback(CNetJobData* pJobData, void* pContext)
{
    SFixedString<32>* pStore = (SFixedString<32>*)pContext;
    if (pJobData->stage == EJobStage::RESULT)
    {
        ms_pNetServerBuffer->PollCommand(pJobData, -1);
        ms_PingStatusLastSaved = *pStore;
    }
    delete pStore;
}

void CNetServerBuffer::GetPingStatus(SFixedString<32>* pstrStatus)
{
    // Blocking read required?
    if (!ms_bPingStatusLastSavedValid)
    {
        // Do blocking read
        SGetPingStatusArgs* pArgs = new SGetPingStatusArgs(pstrStatus);
        AddCommandAndWait(pArgs);

        // Save results
        ms_PingStatusLastSaved = *pstrStatus;
        ms_bPingStatusLastSavedValid = true;
        return;
    }

    // Start a new async read,
    SFixedString<32>*   pStore = new SFixedString<32>();
    SGetPingStatusArgs* pArgs = new SGetPingStatusArgs(pStore);
    AddCommandAndCallback(pArgs, GetPingStatusCallback, pStore);

    // but use results from previous
    *pstrStatus = ms_PingStatusLastSaved;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AllocateNetServerBitStream
//
// Thread safe
//
///////////////////////////////////////////////////////////////////////////
NetBitStreamInterface* CNetServerBuffer::AllocateNetServerBitStream(unsigned short usBitStreamVersion, const void* pData, uint uiDataSize, bool bCopyData)
{
    return m_pRealNetServer->AllocateNetServerBitStream(usBitStreamVersion, pData, uiDataSize, bCopyData);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::DeallocateNetServerBitStream
//
// Thread safe
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::DeallocateNetServerBitStream(NetBitStreamInterface* bitStream)
{
    m_pRealNetServer->DeallocateNetServerBitStream(bitStream);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SendPacket
//
// Non blocking
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::SendPacket(unsigned char ucPacketID, const NetServerPlayerID& playerID, NetBitStreamInterface* bitStream, bool bBroadcast,
                                  NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability, ePacketOrdering packetOrdering)
{
    AddPacketStat(STATS_OUTGOING_TRAFFIC, ucPacketID, bitStream->GetNumberOfBitsUsed() / 8, 0);

    bitStream->AddRef();
    SSendPacketArgs* pArgs = new SSendPacketArgs(ucPacketID, playerID, bitStream, bBroadcast, packetPriority, packetReliability, packetOrdering);
    AddCommandAndFree(pArgs);
    return true;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetPlayerIP
//
// BLOCKING. Called once per player
// (To make non blocking, don't)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::GetPlayerIP(const NetServerPlayerID& playerID, char strIP[22], unsigned short* usPort)
{
    SGetPlayerIPArgs* pArgs = new SGetPlayerIPArgs(playerID, strIP, usPort);
    AddCommandAndWait(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::Kick
//
// Non blocking
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::Kick(const NetServerPlayerID& PlayerID)
{
    SKickArgs* pArgs = new SKickArgs(PlayerID);
    AddCommandAndFree(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetPassword
//
// BLOCKING
// (To make non blocking, szPassword will have to be stored in SSetPasswordArgs)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetPassword(const char* szPassword)
{
    SSetPasswordArgs* pArgs = new SSetPasswordArgs(szPassword);
    AddCommandAndWait(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetMaximumIncomingConnections
//
// Non blocking
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetMaximumIncomingConnections(unsigned short numberAllowed)
{
    SSetMaximumIncomingConnectionsArgs* pArgs = new SSetMaximumIncomingConnectionsArgs(numberAllowed);
    AddCommandAndFree(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetHTTPDownloadManager
//
// Thread safe
//
///////////////////////////////////////////////////////////////////////////
CNetHTTPDownloadManagerInterface* CNetServerBuffer::GetHTTPDownloadManager(EDownloadModeType iMode)
{
    return m_pRealNetServer->GetHTTPDownloadManager(iMode);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetClientBitStreamVersion
//
// Non-blocking. Called once per player
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetClientBitStreamVersion(const NetServerPlayerID& PlayerID, unsigned short usBitStreamVersion)
{
    SSetClientBitStreamVersionArgs* pArgs = new SSetClientBitStreamVersionArgs(PlayerID, usBitStreamVersion);
    AddCommandAndFree(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ClearClientBitStreamVersion
//
// Non-blocking. Called once per player
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ClearClientBitStreamVersion(const NetServerPlayerID& PlayerID)
{
    SClearClientBitStreamVersionArgs* pArgs = new SClearClientBitStreamVersionArgs(PlayerID);
    AddCommandAndFree(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetChecks
//
// BLOCKING. Called once at startup
// (To make non blocking, strings will have to be stored in SSetChecksArgs)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetChecks(const char* szDisableComboACMap, const char* szDisableACMap, const char* szEnableSDMap, int iEnableClientChecks, bool bHideAC,
                                 const char* szImgMods)
{
    SSetChecksArgs* pArgs = new SSetChecksArgs(szDisableComboACMap, szDisableACMap, szEnableSDMap, iEnableClientChecks, bHideAC, szImgMods);
    AddCommandAndWait(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetPendingPacketCount
//
//
//
///////////////////////////////////////////////////////////////////////////
unsigned int CNetServerBuffer::GetPendingPacketCount()
{
    shared.m_Mutex.Lock();
    uint uiCount = shared.m_InResultQueue.size();
    shared.m_Mutex.Unlock();
    return uiCount;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetNetRoute
//
// Blocking on first call.
// Subsequent calls are non blocking but return previous results
//
///////////////////////////////////////////////////////////////////////////
void GetNetRouteCallback(CNetJobData* pJobData, void* pContext)
{
    SFixedString<32>* pStore = (SFixedString<32>*)pContext;
    if (pJobData->stage == EJobStage::RESULT)
    {
        ms_pNetServerBuffer->PollCommand(pJobData, -1);
        ms_NetRouteLastSaved = *pStore;
    }
    delete pStore;
}

void CNetServerBuffer::GetNetRoute(SFixedString<32>* pstrRoute)
{
    // Blocking read required?
    if (!ms_bNetRouteLastSavedValid)
    {
        // Do blocking read
        SGetNetRouteArgs* pArgs = new SGetNetRouteArgs(pstrRoute);
        AddCommandAndWait(pArgs);

        // Save results
        ms_NetRouteLastSaved = *pstrRoute;
        ms_bNetRouteLastSavedValid = true;
        return;
    }

    // Start a new async read,
    SFixedString<32>* pStore = new SFixedString<32>();
    SGetNetRouteArgs* pArgs = new SGetNetRouteArgs(pStore);
    AddCommandAndCallback(pArgs, GetNetRouteCallback, pStore);

    // but use results from previous
    *pstrRoute = ms_NetRouteLastSaved;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::InitServerId
//
// BLOCKING. Called once at startup
// (To make non blocking, szPath will have to be stored in SInitServerIdArgs)
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::InitServerId(const char* szPath)
{
    SInitServerIdArgs* pArgs = new SInitServerIdArgs(szPath);
    AddCommandAndWait(pArgs);
    return pArgs->result;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ResendModPackets
//
// Non-blocking.
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ResendModPackets(const NetServerPlayerID& playerID)
{
    SResendModPacketsArgs* pArgs = new SResendModPacketsArgs(playerID);
    AddCommandAndFree(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ResendACPackets
//
// Non-blocking.
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ResendACPackets(const NetServerPlayerID& playerID)
{
    SResendACPacketsArgs* pArgs = new SResendACPacketsArgs(playerID);
    AddCommandAndFree(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetClientSerialAndVersion
//
// BLOCKING. Called once per player
// (To make non blocking, don't)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::GetClientSerialAndVersion(const NetServerPlayerID& playerID, SFixedString<32>& strSerial, SFixedString<64>& strExtra,
                                                 SFixedString<32>& strVersion)
{
    SGetClientSerialAndVersionArgs* pArgs = new SGetClientSerialAndVersionArgs(playerID, strSerial, strExtra, strVersion);
    AddCommandAndWait(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetNetOptions
//
// BLOCKING. Called rarely
// (To make non blocking, don't)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetNetOptions(const SNetOptions& options)
{
    SSetNetOptionsArgs* pArgs = new SSetNetOptionsArgs(options);
    AddCommandAndWait(pArgs);
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GenerateRandomData
//
// Thread safe
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::GenerateRandomData(void* pOutData, uint uiLength)
{
    m_pRealNetServer->GenerateRandomData(pOutData, uiLength);
}

//
//
//
// Job handling
//
//
//

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetNewJobData
//
// Return a new job data object
//
///////////////////////////////////////////////////////////////
CNetJobData* CNetServerBuffer::GetNewJobData()
{
    CNetJobData* pJobData = new CNetJobData();
    return pJobData;
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AddCommand
//
// AddCommand to queue
// Can't fail
//
///////////////////////////////////////////////////////////////
CNetJobData* CNetServerBuffer::AddCommand(SArgs* pArgs, bool bAutoFree)
{
    // Create command
    CNetJobData* pJobData = GetNewJobData();
    pJobData->pArgs = pArgs;
    pJobData->bAutoFree = bAutoFree;

    // Add to queue
    shared.m_Mutex.Lock();
    pJobData->stage = EJobStage::COMMAND_QUEUE;
    shared.m_OutCommandQueue.push_back(pJobData);
    shared.m_Mutex.Signal();
    shared.m_Mutex.Unlock();

    return bAutoFree ? NULL : pJobData;
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AddCommandAndFree
//
// AddCommand to queue
// Can't fail
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::AddCommandAndFree(SArgs* pArgs)
{
    AddCommand(pArgs, true);
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AddCommandAndWait
//
// AddCommand and wait for response
// Can't fail
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::AddCommandAndWait(SArgs* pArgs)
{
    // Start command
    CNetJobData* pJobData = AddCommand(pArgs, false);

    // Complete command
    PollCommand(pJobData, -1);
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AddCommandAndCallback
//
// AddCommand and set a callback for the result
// Can't fail
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::AddCommandAndCallback(SArgs* pArgs, PFN_NETRESULT pfnNetResult, void* pContext)
{
    // Start command
    CNetJobData* pJobData = AddCommand(pArgs, false);

    // Set callback
    pJobData->SetCallback(pfnNetResult, pContext);
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::PollCommand
//
// Find result for previous command
// Returns false if result not ready.
//
///////////////////////////////////////////////////////////////
bool CNetServerBuffer::PollCommand(CNetJobData* pJobData, uint uiTimeout)
{
    bool bFound = false;

    shared.m_Mutex.Lock();
    while (true)
    {
        // Find result with the required job handle
        for (std::list<CNetJobData*>::iterator iter = shared.m_OutResultQueue.begin(); iter != shared.m_OutResultQueue.end(); ++iter)
        {
            if (pJobData == *iter)
            {
                // Found result. Remove from the result queue and flag return value
                shared.m_OutResultQueue.erase(iter);
                pJobData->stage = EJobStage::FINISHED;
                MapInsert(shared.m_FinishedList, pJobData);

                // Do callback incase any cleanup is needed
                if (pJobData->HasCallback())
                {
                    shared.m_Mutex.Unlock();
                    pJobData->ProcessCallback();
                    shared.m_Mutex.Lock();
                }

                bFound = true;
                break;
            }
        }

        if (bFound || uiTimeout == 0)
        {
            shared.m_Mutex.Unlock();
            break;
        }

        shared.m_Mutex.Wait(uiTimeout);

        // If not infinite, break after next check
        if (uiTimeout != (uint)-1)
            uiTimeout = 0;
    }

    // Make sure if wait was infinite, we have a result
    assert(uiTimeout != (uint)-1 || bFound);

    return bFound;
}

// Update info about one packet
void CNetServerBuffer::AddPacketStat(CNetServer::ENetworkUsageDirection eDirection, uchar ucPacketID, int iPacketSize, TIMEUS elapsedTime)
{
    SPacketStat& stat = m_PacketStatList[eDirection][ucPacketID];
    stat.iCount++;
    stat.iTotalBytes += iPacketSize;
    stat.totalTime += elapsedTime;
    g_uiNetSentByteCounter += iPacketSize;
}

///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessIncoming
//
// Called during pulse
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ProcessIncoming()
{
    bool bTimePacketHandler = m_TimeSinceGetPacketStats.Get() < 10000;

    // Get incoming packets
    shared.m_Mutex.Lock();
    std::list<SProcessPacketArgs*> inResultQueue = shared.m_InResultQueue;
    shared.m_InResultQueue.clear();
    shared.m_Mutex.Unlock();

    // Handle incoming packets
    for (std::list<SProcessPacketArgs*>::iterator iter = inResultQueue.begin(); iter != inResultQueue.end(); ++iter)
    {
        SProcessPacketArgs* pArgs = *iter;

        // Stats
        const int    iPacketSize = (pArgs->BitStream->GetNumberOfUnreadBits() + 8 + 7) / 8;
        const TIMEUS startTime = bTimePacketHandler ? GetTimeUs() : 0;

        if (m_pfnDMPacketHandler)
            m_pfnDMPacketHandler(pArgs->ucPacketID, pArgs->Socket, pArgs->BitStream, pArgs->pNetExtraInfo);

        // Stats
        const TIMEUS elapsedTime = (bTimePacketHandler ? GetTimeUs() : 0) - startTime;
        AddPacketStat(STATS_INCOMING_TRAFFIC, pArgs->ucPacketID, iPacketSize, elapsedTime);

        SAFE_RELEASE(pArgs->pNetExtraInfo);
        SAFE_RELEASE(pArgs->BitStream);
        SAFE_DELETE(pArgs);
    }

    shared.m_Mutex.Lock();

    // Delete finished
    for (std::set<CNetJobData*>::iterator iter = shared.m_FinishedList.begin(); iter != shared.m_FinishedList.end();)
    {
        CNetJobData* pJobData = *iter;
        shared.m_FinishedList.erase(iter++);
        // Check not refed
        dassert(!ListContains(shared.m_OutCommandQueue, pJobData));
        dassert(!ListContains(shared.m_OutResultQueue, pJobData));
        dassert(!MapContains(shared.m_FinishedList, pJobData));
        SAFE_DELETE(pJobData);
    }

again:
    // Do pending callbacks
    for (std::list<CNetJobData*>::iterator iter = shared.m_OutResultQueue.begin(); iter != shared.m_OutResultQueue.end(); ++iter)
    {
        CNetJobData* pJobData = *iter;

        if (pJobData->HasCallback())
        {
            shared.m_Mutex.Unlock();
            pJobData->ProcessCallback();
            shared.m_Mutex.Lock();

            // Redo from the top ensure everything is consistent
            goto again;
        }
    }

    shared.m_Mutex.Unlock();
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
// CNetServerBuffer::StaticThreadProc
//
// static function
// Thread entry point
//
///////////////////////////////////////////////////////////////
void* CNetServerBuffer::StaticThreadProc(void* pContext)
{
    CThreadHandle::AllowASyncCancel();
    return ((CNetServerBuffer*)pContext)->ThreadProc();
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ThreadProc
//
// Job service loop
//
///////////////////////////////////////////////////////////////
void* CNetServerBuffer::ThreadProc()
{
    shared.m_Mutex.Lock();
    while (!shared.m_bTerminateThread)
    {
        shared.m_iThreadFrameCount++;
        UpdateStatsPreDoPulse();

        if (shared.m_bAutoPulse)
        {
            shared.m_Mutex.Unlock();
            m_pRealNetServer->DoPulse();
            UpdateThreadCPUTimes(g_SyncThreadCPUTimes);
            shared.m_Mutex.Lock();
        }

        UpdateStatsPreCommands(shared.m_OutCommandQueue.size());

        // Is there a waiting command?
        if (shared.m_OutCommandQueue.empty())
        {
            shared.m_Mutex.Wait(10);
        }
        else
        {
            // Number of commands to do before checking if a pulse is required
            uint uiNumToDo = shared.m_OutCommandQueue.size();

            while (uiNumToDo-- && !shared.m_bTerminateThread)
            {
                // Get next command
                CNetJobData* pJobData = shared.m_OutCommandQueue.front();
                pJobData->stage = EJobStage::PROCCESSING;
                shared.m_Mutex.Unlock();

                // Process command
                ProcessCommand(pJobData);

                // Store result
                shared.m_Mutex.Lock();
                // Check command has not been cancelled (this should not be possible)
                assert(pJobData == shared.m_OutCommandQueue.front());

                // Remove command
                shared.m_OutCommandQueue.pop_front();
                // Add result
                pJobData->stage = EJobStage::RESULT;
                if (pJobData->bAutoFree)
                    SAFE_DELETE(pJobData)
                else
                    shared.m_OutResultQueue.push_back(pJobData);

                shared.m_Mutex.Signal();
            }
        }

        UpdateStatsFinish();
    }

    shared.m_bThreadTerminated = true;
    shared.m_Mutex.Unlock();

    return NULL;
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessCommand
//
// Call the correct function in m_pRealNetServer depending on the job type
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::ProcessCommand(CNetJobData* pJobData)
{
// Macros of doom
#define CALLPRE0(func) \
    case TYPE_##func: \
    {

#define CALLPRE(func) \
    case TYPE_##func: \
    { \
        S##func##Args& a = *(S##func##Args*)pJobData->pArgs;

#define CALLREALNET0(func)                                                CALLPRE0(func) m_pRealNetServer->func (); CALLPOST
#define CALLREALNET1(func,t1,n1)                                          CALLPRE(func) m_pRealNetServer->func ( a.n1 ); CALLPOST
#define CALLREALNET2(func,t1,n1,t2,n2)                                    CALLPRE(func) m_pRealNetServer->func ( a.n1, a.n2 ); CALLPOST
#define CALLREALNET3(func,t1,n1,t2,n2,t3,n3)                              CALLPRE(func) m_pRealNetServer->func ( a.n1, a.n2, a.n3 ); CALLPOST
#define CALLREALNET4(func,t1,n1,t2,n2,t3,n3,t4,n4)                        CALLPRE(func) m_pRealNetServer->func ( a.n1, a.n2, a.n3, a.n4 ); CALLPOST
#define CALLREALNET5(func,t1,n1,t2,n2,t3,n3,t4,n4,t5,n5)                  CALLPRE(func) m_pRealNetServer->func ( a.n1, a.n2, a.n3, a.n4, a.n5 ); CALLPOST
#define CALLREALNET6(func,t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6)            CALLPRE(func) m_pRealNetServer->func ( a.n1, a.n2, a.n3, a.n4, a.n5, a.n6 ); CALLPOST

#define CALLREALNET0R(ret,func)                                           CALLPRE(func) a.result = m_pRealNetServer->func (); CALLPOST
#define CALLREALNET1R(ret,func,t1,n1)                                     CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1 ); CALLPOST
#define CALLREALNET2R(ret,func,t1,n1,t2,n2)                               CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1, a.n2 ); CALLPOST
#define CALLREALNET3R(ret,func,t1,n1,t2,n2,t3,n3)                         CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1, a.n2, a.n3 ); CALLPOST
#define CALLREALNET4R(ret,func,t1,n1,t2,n2,t3,n3,t4,n4)                   CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1, a.n2, a.n3, a.n4 ); CALLPOST
#define CALLREALNET7R(ret,func,t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6,t7,n7) CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1, a.n2, a.n3, a.n4, a.n5, a.n6, a.n7 ); CALLPOST

#define CALLPOST \
        break; \
    }

    switch (pJobData->pArgs->type)
    {
        CALLREALNET4R(bool, StartNetwork, const char*, szIP, unsigned short, usServerPort, unsigned int, uiAllowedPlayers, const char*, szServerName)
        CALLREALNET0(StopNetwork)
        CALLREALNET0(DoPulse)
        CALLREALNET1(RegisterPacketHandler, PPACKETHANDLER, pfnPacketHandler)
        CALLREALNET2R(bool, GetNetworkStatistics, NetStatistics*, pDest, NetServerPlayerID&, PlayerID)
        CALLREALNET0R(const SPacketStat*, GetPacketStats)
        CALLREALNET1R(bool, GetBandwidthStatistics, SBandwidthStatistics*, pDest)
        CALLREALNET2R(bool, GetNetPerformanceStatistics, SNetPerformanceStatistics*, pDest, bool, bResetCounters)
        CALLREALNET2R(bool, GetSyncThreadStatistics, SSyncThreadStatistics*, pDest, bool, bResetCounters)
        CALLREALNET1(GetPingStatus, SFixedString<32>*, pstrStatus)
        CALLREALNET7R(bool, SendPacket, unsigned char, ucPacketID, const NetServerPlayerID&, playerID, NetBitStreamInterface*, bitStream, bool, bBroadcast,
                      NetServerPacketPriority, packetPriority, NetServerPacketReliability, packetReliability, ePacketOrdering, packetOrdering)
        CALLREALNET3(GetPlayerIP, const NetServerPlayerID&, playerID, char*, strIP, unsigned short*, usPort)
        CALLREALNET1(Kick, const NetServerPlayerID&, PlayerID)
        CALLREALNET1(SetPassword, const char*, szPassword)
        CALLREALNET1(SetMaximumIncomingConnections, unsigned short, numberAllowed)
        CALLREALNET2(SetClientBitStreamVersion, const NetServerPlayerID&, PlayerID, unsigned short, usBitStreamVersion)
        CALLREALNET1(ClearClientBitStreamVersion, const NetServerPlayerID&, PlayerID)
        CALLREALNET6(SetChecks, const char*, szDisableComboACMap, const char*, szDisableACMap, const char*, szEnableSDMap, int, iEnableClientChecks, bool,
                     bHideAC, const char*, szImgMods);
        CALLREALNET0R(unsigned int, GetPendingPacketCount)
        CALLREALNET1(GetNetRoute, SFixedString<32>*, pstrRoute)
        CALLREALNET1R(bool, InitServerId, const char*, szPath)
        CALLREALNET1(ResendModPackets, const NetServerPlayerID&, playerID)
        CALLREALNET1(ResendACPackets, const NetServerPlayerID&, playerID)
        CALLREALNET4(GetClientSerialAndVersion, const NetServerPlayerID&, playerID, SFixedString<32>&, strSerial, SFixedString<64>&, strExtra,
                     SFixedString<32>&, strVersion)
        CALLREALNET1(SetNetOptions, const SNetOptions&, options)
        CALLREALNET2(GenerateRandomData, void*, pOutData, uint, uiLength)

        default:
            // no args type match
            assert(0);
    }

    // SendPacket extra
    if (pJobData->pArgs->type == TYPE_SendPacket)
    {
        SSendPacketArgs& a = *(SSendPacketArgs*)pJobData->pArgs;
        SAFE_RELEASE(a.bitStream);
    }
}

//
//
// Pushed packets
//
//

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::StaticProcessPacket
//
// static function
// Handle data pushed from netmodule during its pulse
//
///////////////////////////////////////////////////////////////
bool CNetServerBuffer::StaticProcessPacket(unsigned char ucPacketID, const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream,
                                           SNetExtraInfo* pNetExtraInfo)
{
    ms_pNetServerBuffer->ProcessPacket(ucPacketID, Socket, BitStream, pNetExtraInfo);
    return true;
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessPacket
//
// Handle data pushed from netmodule during its pulse
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::ProcessPacket(unsigned char ucPacketID, const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream, SNetExtraInfo* pNetExtraInfo)
{
    if (ucPacketID == PACKET_ID_PLAYER_PURESYNC)
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandlePlayerPureSync(Socket, BitStream);

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer();
    }
    else if (ucPacketID == PACKET_ID_PLAYER_VEHICLE_PURESYNC)
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandleVehiclePureSync(Socket, BitStream);

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer();
    }
    else if (ucPacketID == PACKET_ID_PLAYER_KEYSYNC)
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandleKeySync(Socket, BitStream);

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer();
    }
    else if (ucPacketID == PACKET_ID_PLAYER_BULLETSYNC)
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandleBulletSync(Socket, BitStream);

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer();
    }
    else if (ucPacketID == PACKET_ID_PED_TASK)
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandlePedTaskPacket(Socket, BitStream);

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer();
    }
    else if (ucPacketID == PACKET_ID_SERVER_DISCONNECTED)
    {
        // Handle request from net module to immediately send disconnected packet
        uchar   m_eType;
        SString strReason;
        BitStream->Read(m_eType);
        BitStream->ReadString(strReason);
        NetBitStreamInterface*    pSendBitStream = g_pRealNetServer->AllocateNetServerBitStream(BitStream->Version());
        CPlayerDisconnectedPacket packet((CPlayerDisconnectedPacket::ePlayerDisconnectType)m_eType, strReason);
        packet.Write(*pSendBitStream);
        g_pRealNetServer->SendPacket(packet.GetPacketID(), Socket, pSendBitStream, false, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
        g_pRealNetServer->DeallocateNetServerBitStream(pSendBitStream);
        return;
    }

    ms_StatsRecvNumMessages++;

    if (!CNetBufferWatchDog::CanReceivePacket(ucPacketID))
        return;

    BitStream->AddRef();
    if (pNetExtraInfo)
        pNetExtraInfo->AddRef();
    SProcessPacketArgs* pArgs = new SProcessPacketArgs(ucPacketID, Socket, BitStream, pNetExtraInfo);

    // Store result
    shared.m_Mutex.Lock();
    shared.m_InResultQueue.push_back(pArgs);
    shared.m_Mutex.Unlock();
}

///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetQueueSizes
//
// Called from watchdog thread
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::GetQueueSizes(uint& uiFinishedList, uint& uiOutCommandQueue, uint& uiOutResultQueue, uint& uiInResultQueue, uint& uiGamePlayerCount)
{
    shared.m_Mutex.Lock();

    uiFinishedList = shared.m_FinishedList.size();
    uiOutCommandQueue = shared.m_OutCommandQueue.size();
    uiOutResultQueue = shared.m_OutResultQueue.size();
    uiInResultQueue = shared.m_InResultQueue.size();
    uiGamePlayerCount = shared.m_iuGamePlayerCount;

    shared.m_Mutex.Unlock();
}

///////////////////////////////////////////////////////////////
//
// CNetJobData::SetCallback
//
// Set callback for a job data
// Returns false if callback could not be set
//
///////////////////////////////////////////////////////////////
bool CNetJobData::SetCallback(PFN_NETRESULT pfnNetResult, void* pContext)
{
    if (callback.bSet)
        return false;            // One has already been set

    if (this->stage > EJobStage::RESULT)
        return false;            // Too late to set a callback now

    // Set new
    callback.pfnNetResult = pfnNetResult;
    callback.pContext = pContext;
    callback.bSet = true;
    return true;
}

///////////////////////////////////////////////////////////////
//
// CNetJobData::HasCallback
//
// Returns true if callback has been set and has not been called yet
//
///////////////////////////////////////////////////////////////
bool CNetJobData::HasCallback()
{
    return callback.bSet && !callback.bDone;
}

///////////////////////////////////////////////////////////////
//
// CNetJobData::ProcessCallback
//
// Do callback
//
///////////////////////////////////////////////////////////////
void CNetJobData::ProcessCallback()
{
    assert(HasCallback());
    callback.bDone = true;
    callback.pfnNetResult(this, callback.pContext);
}
