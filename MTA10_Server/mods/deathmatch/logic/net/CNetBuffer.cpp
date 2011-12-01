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
    CNetServerBuffer*       ms_pNetServerBuffer = NULL;

    // Used for getting stats (non blocking)
    bool                    ms_bNetStatisticsLastSavedValid = false;
    NetStatistics           ms_NetStatisticsLastSaved;
    NetServerPlayerID       ms_NetStatisticsLastFor;
    const SPacketStat*      ms_PacketStatsLastSaved = NULL;
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
    shared.m_Mutex.Lock ();
    shared.m_bAutoPulse = bEnable;
    shared.m_Mutex.Unlock ();
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

bool CNetServerBuffer::GetNetworkStatistics ( NetStatistics* pDest, NetServerPlayerID& PlayerID )
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
// Blocking on first call, bit dodgy after that
//
///////////////////////////////////////////////////////////////////////////
const SPacketStat* CNetServerBuffer::GetPacketStats ( void )
{
    if ( !ms_PacketStatsLastSaved )
    {
        SGetPacketStatsArgs* pArgs = new SGetPacketStatsArgs ();
        AddCommandAndWait ( pArgs );
        ms_PacketStatsLastSaved = pArgs->result;
    }
    return ms_PacketStatsLastSaved;
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
bool CNetServerBuffer::SendPacket ( unsigned char ucPacketID, NetServerPlayerID& playerID, NetBitStreamInterface* bitStream, bool bBroadcast, NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability, NetServerPacketOrdering packetOrdering  )
{
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
void CNetServerBuffer::GetPlayerIP ( NetServerPlayerID& playerID, char strIP[22], unsigned short* usPort )
{
    SGetPlayerIPArgs* pArgs = new SGetPlayerIPArgs ( playerID, strIP, usPort );
    AddCommandAndWait ( pArgs );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::AddBan
//
// Non blocking. Handled by Raknet and is apparently thread safe
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::AddBan ( const char* szIP )
{
    m_pRealNetServer->AddBan ( szIP );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::RemoveBan
//
// Non blocking. Handled by Raknet and is apparently thread safe
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::RemoveBan ( const char* szIP )
{
    m_pRealNetServer->RemoveBan ( szIP );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::IsBanned
//
// Non blocking. Handled by Raknet and is apparently thread safe
//
///////////////////////////////////////////////////////////////////////////
bool CNetServerBuffer::IsBanned ( const char* szIP )
{
    return m_pRealNetServer->IsBanned ( szIP );
}


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::Kick
//
// Non blocking
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::Kick ( NetServerPlayerID &PlayerID )
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
// (To make non blocking, maps will have to be stored in SSetChecksArgs)
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::SetChecks ( const std::set < SString >& disableComboACMap, const std::set < SString >& disableACMap, const std::set < SString >& enableSDMap, int iEnableClientChecks, bool bHideAC )
{
    SSetChecksArgs* pArgs = new SSetChecksArgs ( disableComboACMap, disableACMap, enableSDMap, iEnableClientChecks, bHideAC );
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
    shared.m_Mutex.Lock ();
    uint uiCount = shared.m_InResultQueue.size ();
    shared.m_Mutex.Unlock ();
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
void CNetServerBuffer::ResendModPackets ( NetServerPlayerID& playerID )
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
void CNetServerBuffer::GetClientSerialAndVersion ( NetServerPlayerID& playerID, CStaticString < 32 >& strSerial, CStaticString < 32 >& strVersion )
{
    SGetClientSerialAndVersionArgs* pArgs = new SGetClientSerialAndVersionArgs ( playerID, strSerial, strVersion );
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
    shared.m_Mutex.Lock ();
    pJobData->stage = EJobStage::COMMAND_QUEUE;
    shared.m_OutCommandQueue.push_back ( pJobData );
    shared.m_Mutex.Signal ();
    shared.m_Mutex.Unlock ();

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

    shared.m_Mutex.Lock ();
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


///////////////////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessIncoming
//
// Called during pulse
//
///////////////////////////////////////////////////////////////////////////
void CNetServerBuffer::ProcessIncoming ( void )
{
    // Get incoming packets
    shared.m_Mutex.Lock ();
    std::list < SProcessPacketArgs* > inResultQueue = shared.m_InResultQueue;
    shared.m_InResultQueue.clear ();
    shared.m_Mutex.Unlock ();

    // Handle incoming packets
    for ( std::list < SProcessPacketArgs* >::iterator iter = inResultQueue.begin () ; iter != inResultQueue.end () ; ++iter )
    {
        SProcessPacketArgs* pArgs = *iter;

        if ( m_pfnDMPacketHandler )
            m_pfnDMPacketHandler( pArgs->ucPacketID, pArgs->Socket, pArgs->BitStream, pArgs->pNetExtraInfo );

        SAFE_RELEASE( pArgs->pNetExtraInfo );
        SAFE_RELEASE( pArgs->BitStream );
        SAFE_DELETE( pArgs );
    }

    shared.m_Mutex.Lock ();

    // Delete finished
    for ( std::set < CNetJobData* >::iterator iter = m_FinishedList.begin () ; iter != m_FinishedList.end () ; )
    {
        CNetJobData* pJobData = *iter;
        m_FinishedList.erase ( iter++ );
        // Check not refed
        assert ( !ListContains ( shared.m_OutCommandQueue, pJobData ) );
        assert ( !ListContains ( shared.m_OutResultQueue, pJobData ) );
        assert ( !MapContains ( m_FinishedList, pJobData ) );
        SAFE_DELETE( pJobData );
    }

again:
    // Do pending callbacks
    for ( std::list < CNetJobData* >::iterator iter = shared.m_OutResultQueue.begin () ; iter != shared.m_OutResultQueue.end () ; ++iter )
    {
        CNetJobData* pJobData = *iter;

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
    shared.m_Mutex.Lock ();
    while ( !shared.m_bTerminateThread )
    {
        // Check if time to do a net pulse
        if ( ( CTickCount::Now () - m_LastPulseTime ).ToLongLong () > 20 )
        {
            if ( shared.m_bAutoPulse )
            {
                shared.m_Mutex.Unlock ();
                m_pRealNetServer->DoPulse ();
                shared.m_Mutex.Lock ();
            }
            m_LastPulseTime = CTickCount::Now ();
        }

        // Is there a waiting command?
        if ( shared.m_OutCommandQueue.empty () )
        {
            shared.m_Mutex.Wait ( 10 );
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
                shared.m_Mutex.Unlock ();

                // Process command
                ProcessCommand ( pJobData );

                // Store result
                shared.m_Mutex.Lock ();
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

                shared.m_Mutex.Signal ();
            }
        }
    }

    shared.m_bThreadTerminated = true;
    shared.m_Mutex.Unlock ();

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
        CALLREALNET7R( bool,                SendPacket                      , unsigned char, ucPacketID, NetServerPlayerID&, playerID, NetBitStreamInterface*, bitStream, bool, bBroadcast, NetServerPacketPriority, packetPriority, NetServerPacketReliability, packetReliability, NetServerPacketOrdering, packetOrdering )
        CALLREALNET3 (                      GetPlayerIP                     , NetServerPlayerID&, playerID, char*, strIP, unsigned short*, usPort )
        CALLREALNET1 (                      Kick                            , NetServerPlayerID &,PlayerID )
        CALLREALNET1 (                      SetPassword                     , const char*, szPassword )
        CALLREALNET1 (                      SetMaximumIncomingConnections   , unsigned short, numberAllowed )
        CALLREALNET2 (                      SetClientBitStreamVersion       , const NetServerPlayerID &,PlayerID, unsigned short, usBitStreamVersion )
        CALLREALNET1 (                      ClearClientBitStreamVersion     , const NetServerPlayerID &,PlayerID )
        CALLREALNET5 (                      SetChecks                       , const std::set < SString >&, disableComboACMap, const std::set < SString >&, disableACMap, const std::set < SString >&, enableSDMap, int, iEnableClientChecks, bool, bHideAC )
        CALLREALNET0R( unsigned int,        GetPendingPacketCount           )
        CALLREALNET1R( bool,                InitServerId                    , const char*, szPath )
        CALLREALNET1 (                      SetEncryptionEnabled            , bool, bEncryptionEnabled )
        CALLREALNET1 (                      ResendModPackets                , NetServerPlayerID&, playerID )
        CALLREALNET3 (                      GetClientSerialAndVersion       , NetServerPlayerIDRef, playerID, CStaticString < 32 >&, strSerial, CStaticString < 32 >&, strVersion )

        default:
            // no packet type match
            assert ( 0 );
    }

    // SendPacket extra
    if ( pJobData->pArgs->type == TYPE_SendPacket )
    {
        SSendPacketArgs& a = *(SSendPacketArgs*)pJobData->pArgs;
        SAFE_RELEASE(a.bitStream);
    }

    // DoPulse extra
    if ( pJobData->pArgs->type == TYPE_DoPulse )
    {
        m_LastPulseTime = CTickCount::Now ();
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
bool CNetServerBuffer::StaticProcessPacket ( unsigned char ucPacketID, NetServerPlayerID& Socket, NetBitStreamInterface* BitStream, SNetExtraInfo* pNetExtraInfo )
{
    return ms_pNetServerBuffer->ProcessPacket ( ucPacketID, Socket, BitStream, pNetExtraInfo );
}


///////////////////////////////////////////////////////////////
//
// CNetServerBuffer::ProcessPacket
//
// Handle data pushed from netmodule during its pulse
//
///////////////////////////////////////////////////////////////
bool CNetServerBuffer::ProcessPacket ( unsigned char ucPacketID, NetServerPlayerID& Socket, NetBitStreamInterface* BitStream, SNetExtraInfo* pNetExtraInfo )
{
    BitStream->AddRef ();
    if ( pNetExtraInfo )
        pNetExtraInfo->AddRef ();
    SProcessPacketArgs* pArgs = new SProcessPacketArgs ( ucPacketID, Socket, BitStream, pNetExtraInfo );

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

    // Store result
    shared.m_Mutex.Lock ();
    shared.m_InResultQueue.push_back ( pArgs );
    shared.m_Mutex.Unlock ();

    return true;
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
