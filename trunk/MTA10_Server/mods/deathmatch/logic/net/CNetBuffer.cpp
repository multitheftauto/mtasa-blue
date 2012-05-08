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

uint g_uiThreadnetProcessorNumber = -1;

namespace
{
    // Used in StaticProcessPacket and elsewhere
    CNetServerBuffer*       ms_pNetServerBuffer = NULL;

    // Used for getting stats (non blocking)
    bool                    ms_bNetStatisticsLastSavedValid = false;
    NetStatistics           ms_NetStatisticsLastSaved;
    NetServerPlayerID       ms_NetStatisticsLastFor;
    bool                    ms_bBandwidthStatisticsLastSavedValid = false;
    SBandwidthStatistics    ms_BandwidthStatisticsLastSaved;
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::CNetServerBuffer
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetServerBuffer::CNetServerBuffer ( CSimPlayerManager* pSimPlayerManager )
{
    m_pSimPlayerManager = pSimPlayerManager;
    ms_pNetServerBuffer = this;
    m_pRealNetServer = g_pRealNetServer;

    // Begin the watchdog
    shared.m_pWatchDog = new CNetBufferWatchDog ( this, g_pGame->GetConfig ()->GetDebugFlag () & 1 );


    // Start the job queue processing thread
    m_pServiceThreadHandle = new CThreadHandle ( CNetServerBuffer::StaticThreadProc, this );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::~CNetServerBuffer
//
//
//
///////////////////////////////////////////////////////////////////////////
CNetServerBuffer::~CNetServerBuffer ( void )
{
    // Stop the job queue processing thread
    StopThread ();

    // Delete thread
    SAFE_DELETE ( m_pServiceThreadHandle );

    // Stop the debug watchdog
    SAFE_DELETE ( shared.m_pWatchDog );

    ms_pNetServerBuffer = NULL;
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetAutoPulseEnabled
//
// Enable pulsing automatically in the sync thread
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::SetAutoPulseEnabled ( bool bEnable )
{
    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::SetAutoPulseEnabled );
    shared.m_bAutoPulse = bEnable;
    shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::SetAutoPulseEnabled );
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::StopThread
//
// Stop the job queue processing thread
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::StopThread ( void )
{
    // Stop the job queue processing thread
    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::StopThread );
    shared.m_bTerminateThread = true;
    shared.m_Mutex.Signal ( EActionWho::MAIN, EActionWhere::StopThread );
    shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::StopThread );

    for ( uint i = 0 ; i < 5000 ; i += 15 )
    {
        if ( shared.m_bThreadTerminated )
            return;

        Sleep ( 15 );
    }

    // If thread not stopped, (async) cancel it
    m_pServiceThreadHandle->Cancel ();
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::StartNetwork
//
// BLOCKING
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::StartNetwork ( const char* szIP, unsigned short usServerPort, unsigned int uiAllowedPlayers )
{
    SStartNetworkArgs* pArgs = new SStartNetworkArgs ( szIP, usServerPort, uiAllowedPlayers );
    AddCommandAndWait ( pArgs );
    return pArgs->result;
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::StopNetwork
//
// BLOCKING
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::StopNetwork ( void )
{
    SStopNetworkArgs* pArgs = new SStopNetworkArgs ();
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ResetNetwork
//
// BLOCKING
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ResetNetwork ( void )
{
    SResetNetworkArgs* pArgs = new SResetNetworkArgs ();
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::DoPulse
//
//
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::DoPulse ( void )
{
    // Schedule a net pulse
    SDoPulseArgs* pArgs = new SDoPulseArgs ();
    AddCommandAndFree ( pArgs );

    // Read incoming packets
    ProcessIncoming ();

    // See if it's time to check the thread 'fps'
    if ( m_TimeThreadFPSLastCalced.Get () > 1000 )
    {
        m_TimeThreadFPSLastCalced.Reset ();
        float fSyncFPS;
        shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::DoPulse );
        fSyncFPS = static_cast < float > ( shared.m_iThreadFrameCount );
        shared.m_iThreadFrameCount = 0;
        shared.m_iuGamePlayerCount = g_pGame->GetPlayerManager ()->Count (); // Also update player count here (for scaling buffer size checks)
        shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::DoPulse );

        // Compress high counts
        if ( fSyncFPS > 500 )
            fSyncFPS = pow ( fSyncFPS - 500, 0.6f ) + 500;

        // Clamp the max change
        float fChange = fSyncFPS - m_fSmoothThreadFPS;
        float fMaxChange = 50;
        fChange = Clamp ( -fMaxChange, fChange, fMaxChange );
        m_fSmoothThreadFPS = Lerp ( m_fSmoothThreadFPS, 0.4f, m_fSmoothThreadFPS + fChange );
        g_pGame->SetSyncFPS ( static_cast < int > ( m_fSmoothThreadFPS ) );
    }
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::RegisterPacketHandler
//
// BLOCKING. Called once at startup
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::RegisterPacketHandler ( PPACKETHANDLER pfnPacketHandler )
{
    m_pfnDMPacketHandler = pfnPacketHandler;

    // Replace handler if not NULL
    if ( pfnPacketHandler )
        pfnPacketHandler = CNetServerBuffer::StaticProcessPacket;

    SRegisterPacketHandlerArgs* pArgs = new SRegisterPacketHandlerArgs ( pfnPacketHandler );
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetNetworkStatistics
//
// Blocking on first call with new PlayerID.
// Subsequent calls with the same PlayerID are non blocking but return previous results
//
///////////////////////////////////////////////////////////////////////////
void GetNetworkStatisticsCallback ( CNetJobData* pJobData, void* pContext )
{
    NetStatistics* pStore = (NetStatistics*)pContext;
    if ( pJobData->stage == EJobStage::RESULT )
    {
        ms_pNetServerBuffer->PollCommand ( pJobData, -1 );
        ms_NetStatisticsLastSaved = *pStore;
    }
    delete pStore;
}

bool CNetServerBuffer::GetNetworkStatistics ( NetStatistics* pDest, const NetServerPlayerID& PlayerID )
{
    // Blocking read required?
    if ( !ms_bNetStatisticsLastSavedValid || ms_NetStatisticsLastFor != PlayerID )
    {
        // Do blocking read 
        SGetNetworkStatisticsArgs* pArgs = new SGetNetworkStatisticsArgs ( pDest, PlayerID );
        AddCommandAndWait ( pArgs );

        // Save results
        ms_NetStatisticsLastSaved = *pDest;
        ms_NetStatisticsLastFor = PlayerID;
        ms_bNetStatisticsLastSavedValid = true;
        return pArgs->result;
    }

    // Start a new async read,
    NetStatistics* pStore = new NetStatistics();
    SGetNetworkStatisticsArgs* pArgs = new SGetNetworkStatisticsArgs ( pStore, PlayerID );
    AddCommandAndCallback ( pArgs, GetNetworkStatisticsCallback, pStore );

    // but use results from previous
    *pDest = ms_NetStatisticsLastSaved;
    return true;
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetPacketStats
//
// Uses stats gathered here
//
///////////////////////////////////////////////////////////////////////////
const SPacketStat* CNetServerBuffer::GetPacketStats ( void )
{
    m_TimeSinceGetPacketStats.Reset ();
    return m_PacketStatList [ 0 ];
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetBandwidthStatistics
//
// Blocking on first call.
// Subsequent calls are non blocking but return previous results
//
///////////////////////////////////////////////////////////////////////////
void GetBandwidthStatisticsCallback ( CNetJobData* pJobData, void* pContext )
{
    SBandwidthStatistics* pStore = (SBandwidthStatistics*)pContext;
    if ( pJobData->stage == EJobStage::RESULT )
    {
        ms_pNetServerBuffer->PollCommand ( pJobData, -1 );
        ms_BandwidthStatisticsLastSaved = *pStore;
    }
    delete pStore;
}

bool CNetServerBuffer::GetBandwidthStatistics ( SBandwidthStatistics* pDest )
{
    // Blocking read required?
    if ( !ms_bBandwidthStatisticsLastSavedValid )
    {
        // Do blocking read 
        SGetBandwidthStatisticsArgs* pArgs = new SGetBandwidthStatisticsArgs ( pDest );
        AddCommandAndWait ( pArgs );

        // Save results
        ms_BandwidthStatisticsLastSaved = *pDest;
        ms_bBandwidthStatisticsLastSavedValid = true;
        return pArgs->result;
    }

    // Start a new async read,
    SBandwidthStatistics* pStore = new SBandwidthStatistics();
    SGetBandwidthStatisticsArgs* pArgs = new SGetBandwidthStatisticsArgs ( pStore );
    AddCommandAndCallback ( pArgs, GetBandwidthStatisticsCallback, pStore );

    // but use results from previous
    *pDest = ms_BandwidthStatisticsLastSaved;
    return true;
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AllocateNetServerBitStream
//
// Thread safe
//
///////////////////////////////////////////////////////////////////////////
NetBitStreamInterface* CNetServerBuffer::AllocateNetServerBitStream ( unsigned short usBitStreamVersion )
{
    return m_pRealNetServer->AllocateNetServerBitStream ( usBitStreamVersion );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::DeallocateNetServerBitStream
//
// Thread safe
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::DeallocateNetServerBitStream ( NetBitStreamInterface* bitStream )
{
    m_pRealNetServer->DeallocateNetServerBitStream ( bitStream );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SendPacket
//
// Non blocking
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::SendPacket ( unsigned char ucPacketID, const NetServerPlayerID& playerID, NetBitStreamInterface* bitStream, bool bBroadcast, NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability, ePacketOrdering packetOrdering  )
{
    AddPacketStat ( STATS_OUTGOING_TRAFFIC, ucPacketID, bitStream->GetNumberOfBitsUsed () / 8, 0 );

    bitStream->AddRef ();
    SSendPacketArgs* pArgs = new SSendPacketArgs ( ucPacketID, playerID, bitStream, bBroadcast, packetPriority, packetReliability, packetOrdering );
    AddCommandAndFree ( pArgs );
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
void CNetServerBuffer::GetPlayerIP ( const NetServerPlayerID& playerID, char strIP[22], unsigned short* usPort )
{
    SGetPlayerIPArgs* pArgs = new SGetPlayerIPArgs ( playerID, strIP, usPort );
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::Kick
//
// Non blocking
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::Kick ( const NetServerPlayerID &PlayerID )
{
    SKickArgs* pArgs = new SKickArgs (PlayerID);
    AddCommandAndFree ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetPassword
//
// BLOCKING
// (To make non blocking, szPassword will have to be stored in SSetPasswordArgs)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetPassword ( const char* szPassword )
{
    SSetPasswordArgs* pArgs = new SSetPasswordArgs (szPassword);
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetMaximumIncomingConnections
//
// Non blocking
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetMaximumIncomingConnections ( unsigned short numberAllowed )
{
    SSetMaximumIncomingConnectionsArgs* pArgs = new SSetMaximumIncomingConnectionsArgs (numberAllowed);
    AddCommandAndFree ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetHTTPDownloadManager
//
// Thread safe
//
///////////////////////////////////////////////////////////////////////////
CNetHTTPDownloadManagerInterface* CNetServerBuffer::GetHTTPDownloadManager ( void )
{
    return m_pRealNetServer->GetHTTPDownloadManager ();
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetClientBitStreamVersion
//
// Non-blocking. Called once per player
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetClientBitStreamVersion ( const NetServerPlayerID &PlayerID, unsigned short usBitStreamVersion )
{
    SSetClientBitStreamVersionArgs* pArgs = new SSetClientBitStreamVersionArgs ( PlayerID, usBitStreamVersion );
    AddCommandAndFree ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ClearClientBitStreamVersion
//
// Non-blocking. Called once per player
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ClearClientBitStreamVersion ( const NetServerPlayerID &PlayerID )
{
    SClearClientBitStreamVersionArgs* pArgs = new SClearClientBitStreamVersionArgs ( PlayerID );
    AddCommandAndFree ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetChecks
//
// BLOCKING. Called once at startup
// (To make non blocking, strings will have to be stored in SSetChecksArgs)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetChecks ( const char* szDisableComboACMap, const char* szDisableACMap, const char* szEnableSDMap, int iEnableClientChecks, bool bHideAC )
{
    SSetChecksArgs* pArgs = new SSetChecksArgs ( szDisableComboACMap, szDisableACMap, szEnableSDMap, iEnableClientChecks, bHideAC );
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetPendingPacketCount
//
// 
//
///////////////////////////////////////////////////////////////////////////
unsigned int CNetServerBuffer::GetPendingPacketCount ( void )
{
    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::GetPendingPacketCount );
    uint uiCount = shared.m_InResultQueue.size ();
    shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::GetPendingPacketCount );
    return uiCount;
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::InitServerId
//
// BLOCKING. Called once at startup
// (To make non blocking, szPath will have to be stored in SInitServerIdArgs)
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::InitServerId ( const char* szPath )
{
    SInitServerIdArgs* pArgs = new SInitServerIdArgs ( szPath );
    AddCommandAndWait ( pArgs );
    return pArgs->result;
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetEncryptionEnabled
//
// Non-blocking. Called once at startup, and when config changes
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetEncryptionEnabled ( bool bEncryptionEnabled )
{
    SSetEncryptionEnabledArgs* pArgs = new SSetEncryptionEnabledArgs ( bEncryptionEnabled );
    AddCommandAndFree ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ResendModPackets
//
// Non-blocking.
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ResendModPackets ( const NetServerPlayerID& playerID )
{
    SResendModPacketsArgs* pArgs = new SResendModPacketsArgs ( playerID );
    AddCommandAndFree ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetClientSerialAndVersion
//
// BLOCKING. Called once per player
// (To make non blocking, don't)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::GetClientSerialAndVersion ( const NetServerPlayerID& playerID, SFixedString < 32 >& strSerial, SFixedString < 32 >& strVersion )
{
    SGetClientSerialAndVersionArgs* pArgs = new SGetClientSerialAndVersionArgs ( playerID, strSerial, strVersion );
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::SetNetOptions
//
// BLOCKING. Called rarely
// (To make non blocking, don't)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetNetOptions ( const SNetOptions& options )
{
    SSetNetOptionsArgs* pArgs = new SSetNetOptionsArgs ( options );
    AddCommandAndWait ( pArgs );
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
CNetJobData* CNetServerBuffer::GetNewJobData ( void )
{
    CNetJobData* pJobData = new CNetJobData ();
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
CNetJobData* CNetServerBuffer::AddCommand ( SArgs* pArgs, bool bAutoFree )
{
    // Create command
    CNetJobData* pJobData = GetNewJobData ();
    pJobData->pArgs = pArgs;
    pJobData->bAutoFree = bAutoFree;

    // Add to queue
    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::AddCommand );
    pJobData->stage = EJobStage::COMMAND_QUEUE;
    shared.m_OutCommandQueue.push_back ( pJobData );
    shared.m_Mutex.Signal ( EActionWho::MAIN, EActionWhere::AddCommand );
    shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::AddCommand );

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
void CNetServerBuffer::AddCommandAndFree ( SArgs* pArgs )
{
    AddCommand ( pArgs, true );
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AddCommandAndWait
//
// AddCommand and wait for response
// Can't fail
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::AddCommandAndWait ( SArgs* pArgs )
{
    // Start command
    CNetJobData* pJobData = AddCommand ( pArgs, false );

    // Complete command
    PollCommand ( pJobData, -1 );
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AddCommandAndCallback
//
// AddCommand and set a callback for the result
// Can't fail
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::AddCommandAndCallback ( SArgs* pArgs, PFN_NETRESULT pfnNetResult, void* pContext )
{
    // Start command
    CNetJobData* pJobData = AddCommand ( pArgs, false );

    // Set callback
    pJobData->SetCallback ( pfnNetResult, pContext );
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::PollCommand
//
// Find result for previous command
// Returns false if result not ready.
//
///////////////////////////////////////////////////////////////
bool CNetServerBuffer::PollCommand ( CNetJobData* pJobData, uint uiTimeout )
{
    bool bFound = false;

    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::PollCommand );
    while ( true )
    {
        // Find result with the required job handle
        for ( std::list < CNetJobData* >::iterator iter = shared.m_OutResultQueue.begin () ; iter != shared.m_OutResultQueue.end () ; ++iter )
        {
            if ( pJobData == *iter )
            {
                // Found result. Remove from the result queue and flag return value
                shared.m_OutResultQueue.erase ( iter );
                pJobData->stage = EJobStage::FINISHED;
                MapInsert ( shared.m_FinishedList, pJobData );

                // Do callback incase any cleanup is needed
                if ( pJobData->HasCallback () )
                {
                    shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::PollCommand2 );                 
                    pJobData->ProcessCallback ();              
                    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::PollCommand2 );
                }

                bFound = true;
                break;
            }
        }

        if ( bFound || uiTimeout == 0 )
        {
            shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::PollCommand );
            break;
        }

        shared.m_Mutex.Wait ( uiTimeout, EActionWho::MAIN, EActionWhere::PollCommand );

        // If not infinite, break after next check
        if ( uiTimeout != (uint)-1 )
            uiTimeout = 0;
    }

    // Make sure if wait was infinite, we have a result
    assert ( uiTimeout != (uint)-1 || bFound );

    return bFound;
}


// Update info about one packet
void CNetServerBuffer::AddPacketStat ( CNetServer::ENetworkUsageDirection eDirection, uchar ucPacketID, int iPacketSize, TIMEUS elapsedTime )
{
    SPacketStat& stat = m_PacketStatList [ eDirection ] [ ucPacketID ];
    stat.iCount++;
    stat.iTotalBytes += iPacketSize;
    stat.totalTime += elapsedTime;
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessIncoming
//
// Called during pulse
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ProcessIncoming ( void )
{
    bool bTimePacketHandler = m_TimeSinceGetPacketStats.Get () < 10000;

    // Get incoming packets
    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::ProcessIncoming );
    std::list < SProcessPacketArgs* > inResultQueue = shared.m_InResultQueue;
    shared.m_InResultQueue.clear ();
    shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::ProcessIncoming );

    // Handle incoming packets
    for ( std::list < SProcessPacketArgs* >::iterator iter = inResultQueue.begin () ; iter != inResultQueue.end () ; ++iter )
    {
        SProcessPacketArgs* pArgs = *iter;

        // Stats
        const int iPacketSize = ( pArgs->BitStream->GetNumberOfUnreadBits () + 8 + 7 ) / 8;
        const TIMEUS startTime = bTimePacketHandler ? GetTimeUs () : 0;

        if ( m_pfnDMPacketHandler )
            m_pfnDMPacketHandler( pArgs->ucPacketID, pArgs->Socket, pArgs->BitStream, pArgs->pNetExtraInfo );

        // Stats
        const TIMEUS elapsedTime = ( bTimePacketHandler ? GetTimeUs () : 0 ) - startTime;
        AddPacketStat ( STATS_INCOMING_TRAFFIC, pArgs->ucPacketID, iPacketSize, elapsedTime );

        SAFE_RELEASE( pArgs->pNetExtraInfo );
        SAFE_RELEASE( pArgs->BitStream );
        SAFE_DELETE( pArgs );
    }

    shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::ProcessIncoming2 );

    // Delete finished
    for ( std::set < CNetJobData* >::iterator iter = shared.m_FinishedList.begin () ; iter != shared.m_FinishedList.end () ; )
    {
        CNetJobData* pJobData = *iter;
        shared.m_FinishedList.erase ( iter++ );
        // Check not refed
        dassert ( !ListContains ( shared.m_OutCommandQueue, pJobData ) );
        dassert ( !ListContains ( shared.m_OutResultQueue, pJobData ) );
        dassert ( !MapContains ( shared.m_FinishedList, pJobData ) );
        SAFE_DELETE( pJobData );
    }

again:
    // Do pending callbacks
    for ( std::list < CNetJobData* >::iterator iter = shared.m_OutResultQueue.begin () ; iter != shared.m_OutResultQueue.end () ; ++iter )
    {
        CNetJobData* pJobData = *iter;

        if ( pJobData->HasCallback () )
        {
            shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::ProcessIncoming3 );
            pJobData->ProcessCallback ();              
            shared.m_Mutex.Lock ( EActionWho::MAIN, EActionWhere::ProcessIncoming3 );

            // Redo from the top ensure everything is consistent
            goto again;
        }
    }

    shared.m_Mutex.Unlock ( EActionWho::MAIN, EActionWhere::ProcessIncoming2 );
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
void* CNetServerBuffer::StaticThreadProc ( void* pContext )
{
    SetCurrentThreadType ( EActionWho::SYNC );
    CThreadHandle::AllowASyncCancel ();
    return ((CNetServerBuffer*)pContext)->ThreadProc ();
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ThreadProc
//
// Job service loop
//
///////////////////////////////////////////////////////////////
void* CNetServerBuffer::ThreadProc ( void )
{
    shared.m_Mutex.Lock ( EActionWho::SYNC, EActionWhere::ThreadProc );
    while ( !shared.m_bTerminateThread )
    {
        shared.m_iThreadFrameCount++;

        if ( shared.m_bAutoPulse )
        {
            shared.m_Mutex.Unlock ( EActionWho::SYNC, EActionWhere::ThreadProc2 );
            m_pRealNetServer->DoPulse ();
            g_uiThreadnetProcessorNumber = _GetCurrentProcessorNumber ();
            shared.m_Mutex.Lock ( EActionWho::SYNC, EActionWhere::ThreadProc2 );
        }

        // Is there a waiting command?
        if ( shared.m_OutCommandQueue.empty () )
        {
            shared.m_Mutex.Wait ( 10, EActionWho::SYNC, EActionWhere::ThreadProc );
        }
        else
        {
            // Number of commands to do before checking if a pulse is required
            uint uiNumToDo = shared.m_OutCommandQueue.size ();

            while ( uiNumToDo-- && !shared.m_bTerminateThread )
            {
                // Get next command
                CNetJobData* pJobData = shared.m_OutCommandQueue.front ();
                pJobData->stage = EJobStage::PROCCESSING;
                shared.m_Mutex.Unlock ( EActionWho::SYNC, EActionWhere::ThreadProc3 );

                // Process command
                ProcessCommand ( pJobData );

                // Store result
                shared.m_Mutex.Lock ( EActionWho::SYNC, EActionWhere::ThreadProc3 );
                // Check command has not been cancelled (this should not be possible)
                assert ( pJobData == shared.m_OutCommandQueue.front () );

                // Remove command
                shared.m_OutCommandQueue.pop_front ();
                // Add result
                pJobData->stage = EJobStage::RESULT;
                if ( pJobData->bAutoFree )
                    SAFE_DELETE( pJobData )
                else
                    shared.m_OutResultQueue.push_back ( pJobData );

                shared.m_Mutex.Signal ( EActionWho::SYNC, EActionWhere::ThreadProc );
            }
        }
    }

    shared.m_bThreadTerminated = true;
    shared.m_Mutex.Unlock ( EActionWho::SYNC, EActionWhere::ThreadProc );

    return NULL;
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessCommand
//
// Call the correct function in m_pRealNetServer depending on the job type
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::ProcessCommand ( CNetJobData* pJobData )
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
#define CALLREALNET5(func,t1,n1,t2,n2,t3,n3,t4,n4,t5,n5)                  CALLPRE(func) m_pRealNetServer->func ( a.n1, a.n2, a.n3, a.n4, a.n5 ); CALLPOST

#define CALLREALNET0R(ret,func)                                           CALLPRE(func) a.result = m_pRealNetServer->func (); CALLPOST
#define CALLREALNET1R(ret,func,t1,n1)                                     CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1 ); CALLPOST
#define CALLREALNET2R(ret,func,t1,n1,t2,n2)                               CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1, a.n2 ); CALLPOST
#define CALLREALNET3R(ret,func,t1,n1,t2,n2,t3,n3)                         CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1, a.n2, a.n3 ); CALLPOST
#define CALLREALNET7R(ret,func,t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6,t7,n7) CALLPRE(func) a.result = m_pRealNetServer->func ( a.n1, a.n2, a.n3, a.n4, a.n5, a.n6, a.n7 ); CALLPOST

#define CALLPOST \
        break; \
    }


    switch ( pJobData->pArgs->type )
    {
        CALLREALNET3R( bool,                StartNetwork                    , const char*, szIP, unsigned short, usServerPort, unsigned int, uiAllowedPlayers )
        CALLREALNET0 (                      StopNetwork                     )
        CALLREALNET0 (                      ResetNetwork                    )
        CALLREALNET0 (                      DoPulse                         )
        CALLREALNET1 (                      RegisterPacketHandler           , PPACKETHANDLER, pfnPacketHandler )
        CALLREALNET2R( bool,                GetNetworkStatistics            , NetStatistics*, pDest, NetServerPlayerID&, PlayerID )
        CALLREALNET0R( const SPacketStat*,  GetPacketStats                  )
        CALLREALNET1R( bool,                GetBandwidthStatistics          , SBandwidthStatistics*, pDest )
        CALLREALNET7R( bool,                SendPacket                      , unsigned char, ucPacketID, const NetServerPlayerID&, playerID, NetBitStreamInterface*, bitStream, bool, bBroadcast, NetServerPacketPriority, packetPriority, NetServerPacketReliability, packetReliability, ePacketOrdering, packetOrdering )
        CALLREALNET3 (                      GetPlayerIP                     , const NetServerPlayerID&, playerID, char*, strIP, unsigned short*, usPort )
        CALLREALNET1 (                      Kick                            , const NetServerPlayerID &,PlayerID )
        CALLREALNET1 (                      SetPassword                     , const char*, szPassword )
        CALLREALNET1 (                      SetMaximumIncomingConnections   , unsigned short, numberAllowed )
        CALLREALNET2 (                      SetClientBitStreamVersion       , const NetServerPlayerID &,PlayerID, unsigned short, usBitStreamVersion )
        CALLREALNET1 (                      ClearClientBitStreamVersion     , const NetServerPlayerID &,PlayerID )
        CALLREALNET5 (                      SetChecks                       , const char*, szDisableComboACMap, const char*, szDisableACMap, const char*, szEnableSDMap, int, iEnableClientChecks, bool, bHideAC );
        CALLREALNET0R( unsigned int,        GetPendingPacketCount           )
        CALLREALNET1R( bool,                InitServerId                    , const char*, szPath )
        CALLREALNET1 (                      SetEncryptionEnabled            , bool, bEncryptionEnabled )
        CALLREALNET1 (                      ResendModPackets                , const NetServerPlayerID&, playerID )
        CALLREALNET3 (                      GetClientSerialAndVersion       , const NetServerPlayerID&, playerID, SFixedString < 32 >&, strSerial, SFixedString < 32 >&, strVersion )
        CALLREALNET1 (                      SetNetOptions                   , const SNetOptions&, options )

        default:
            // no args type match
            assert ( 0 );
    }

    // SendPacket extra
    if ( pJobData->pArgs->type == TYPE_SendPacket )
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
bool CNetServerBuffer::StaticProcessPacket ( unsigned char ucPacketID, const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream, SNetExtraInfo* pNetExtraInfo )
{
    ms_pNetServerBuffer->ProcessPacket ( ucPacketID, Socket, BitStream, pNetExtraInfo );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessPacket
//
// Handle data pushed from netmodule during its pulse
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::ProcessPacket ( unsigned char ucPacketID, const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream, SNetExtraInfo* pNetExtraInfo )
{

    if ( ucPacketID == PACKET_ID_PLAYER_PURESYNC )
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandlePlayerPureSync ( Socket, BitStream );

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer ();
    }
    else
    if ( ucPacketID == PACKET_ID_PLAYER_VEHICLE_PURESYNC )
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandleVehiclePureSync ( Socket, BitStream );

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer ();
    }
    else
    if ( ucPacketID == PACKET_ID_PLAYER_KEYSYNC )
    {
        // See about handling the packet relaying here
        m_pSimPlayerManager->HandleKeySync ( Socket, BitStream );

        // Reset bitstream pointer so game can also read the packet data
        BitStream->ResetReadPointer ();
    }

    if ( !CNetBufferWatchDog::CanReceivePacket ( ucPacketID ) )
       return;

    BitStream->AddRef ();
    if ( pNetExtraInfo )
        pNetExtraInfo->AddRef ();
    SProcessPacketArgs* pArgs = new SProcessPacketArgs ( ucPacketID, Socket, BitStream, pNetExtraInfo );

    // Store result
    shared.m_Mutex.Lock ( EActionWho::SYNC, EActionWhere::ProcessPacket );
    shared.m_InResultQueue.push_back ( pArgs );
    shared.m_Mutex.Unlock ( EActionWho::SYNC, EActionWhere::ProcessPacket );
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::GetQueueSizes
//
// Called from watchdog thread
//
///////////////////////////////////////////////////////////////
void CNetServerBuffer::GetQueueSizes ( uint& uiFinishedList, uint& uiOutCommandQueue, uint& uiOutResultQueue, uint& uiInResultQueue, uint& uiGamePlayerCount )
{
    shared.m_Mutex.Lock ( EActionWho::WATCHDOG, EActionWhere::GetQueueSizes );

    uiFinishedList = shared.m_FinishedList.size ();
    uiOutCommandQueue = shared.m_OutCommandQueue.size ();
    uiOutResultQueue = shared.m_OutResultQueue.size ();
    uiInResultQueue = shared.m_InResultQueue.size ();
    uiGamePlayerCount = shared.m_iuGamePlayerCount;

    shared.m_Mutex.Unlock ( EActionWho::WATCHDOG, EActionWhere::GetQueueSizes );
}


///////////////////////////////////////////////////////////////
//
// CNetJobData::SetCallback
//
// Set callback for a job data
// Returns false if callback could not be set
//
///////////////////////////////////////////////////////////////
bool CNetJobData::SetCallback ( PFN_NETRESULT pfnNetResult, void* pContext )
{
    if ( callback.bSet )
        return false;   // One has already been set

    if ( this->stage > EJobStage::RESULT )
        return false;   // Too late to set a callback now

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
bool CNetJobData::HasCallback ( void )
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
void CNetJobData::ProcessCallback ( void )
{
    assert ( HasCallback () );
    callback.bDone = true;
    callback.pfnNetResult( this, callback.pContext );
}
