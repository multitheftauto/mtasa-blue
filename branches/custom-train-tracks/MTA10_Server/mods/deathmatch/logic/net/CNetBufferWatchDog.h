/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CNetServerBuffer;

//
// Info about a queue
//
namespace EQueueStatus
{
    enum EQueueStatusType
    {
        STATUS_OK,
        SUSPEND_SYNC,
        STOP_THREAD_NET,
        SHUTDOWN,
        TERMINATE,
    };
}
using EQueueStatus::EQueueStatusType;

// 30 slot history of sizes
class CSizeHistory
{
    std::vector < int > dataList;
public:
    void AddPoint ( int iPoint )
    {
        dataList.push_back ( iPoint );
        if ( dataList.size () > 30 )
            dataList.erase ( dataList.begin () );
    }

    int GetLowestPointSince ( int iAge )
    {
        if ( iAge >= (int)dataList.size () )
            return 0;
        int iLowest = INT_MAX;
        for ( int i = dataList.size () - 1 ; i >= 0 && iAge > 0 ; i--, iAge-- )
            iLowest = Min ( iLowest, dataList[i] );
        return iLowest;
    }
};

// Queue status and size history
class CQueueInfo
{
public:
    EQueueStatusType status;
    CSizeHistory m_SizeHistory;
};


//
// Monitor queues and locks for trouble
//
class CNetBufferWatchDog
{
public:
    ZERO_ON_NEW
                                            CNetBufferWatchDog              ( CNetServerBuffer* pNetBuffer, bool bVerboseDebug );
                                            ~CNetBufferWatchDog             ( void );

    // Main thread functions
    void                        StopThread                  ( void );

    // Check thread functions
    static void*                StaticThreadProc            ( void* pContext );
    void*                       ThreadProc                  ( void );
    void                        DoChecks                    ( void );
    void                        UpdateQueueInfo             ( CQueueInfo& queueInfo, int iQueueSize, const char* szTag );
    void                        BlockOutgoingSyncPackets    ( void );
    void                        AllowOutgoingSyncPackets    ( void );
    void                        BlockIncomingSyncPackets    ( void );
    void                        AllowIncomingSyncPackets    ( void );

    // Main thread variables
    CThreadHandle*              m_pServiceThreadHandle;

    // Check thread variables
    CNetServerBuffer*           m_pNetBuffer;
    uint                        m_uiFinishedListHigh;
    uint                        m_uiOutCommandQueueHigh;
    uint                        m_uiOutResultQueueHigh;
    uint                        m_uiInResultQueueHigh;
    CQueueInfo                  m_FinishedListQueueInfo;
    CQueueInfo                  m_OutCommandQueueInfo;
    CQueueInfo                  m_OutResultQueueInfo;
    CQueueInfo                  m_InResultQueueInfo;
    uint                        m_uiGamePlayerCount;

    // Shared variables
    struct
    {
        bool                                        m_bTerminateThread;
        bool                                        m_bThreadTerminated;
        CComboMutex                                 m_Mutex;
    } shared;


    // Static globals
    static volatile bool ms_bBlockOutgoingSyncPackets;
    static volatile bool ms_bBlockIncomingSyncPackets;
    static volatile bool ms_bCriticalStopThreadNet;
    static volatile bool ms_bVerboseDebug;
    static uint ms_uiFinishedListSize;
    static uint ms_uiOutCommandQueueSize;
    static uint ms_uiOutResultQueueSize;
    static uint ms_uiInResultQueueSize;

    // Static functions
    static void     DoPulse                 ( void );
    static bool     IsUnreliableSyncPacket  ( uchar ucPacketID );
    static bool     CanSendPacket           ( uchar ucPacketID );
    static bool     CanReceivePacket        ( uchar ucPacketID );
};









