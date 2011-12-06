/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CNetServerBuffer;

namespace EActionWho
{
    enum EActionWhoType
    {
        MAIN,
        SYNC,
    };
}
using EActionWho::EActionWhoType;


namespace EActionWhere
{
    enum EActionWhereType
    {
        AddCommand,
        PollCommand,
        PollCommand2,
        SetAutoPulseEnabled,
        StopThread,
        GetPendingPacketCount,
        ProcessIncoming,
        ProcessIncoming2,
        ProcessIncoming3,
        ThreadProc,
        ThreadProc2,
        ThreadProc3,
        ProcessPacket,
        GetQueueSizes,
    };
}
using EActionWhere::EActionWhereType;


namespace EActionWhat
{
    enum EActionWhatType
    {
        PRE_LOCK,
        POST_LOCK,
        PRE_UNLOCK,
        POST_UNLOCK,
        PRE_WAIT,
        POST_WAIT,
        SIGNAL,
    };
}
using EActionWhat::EActionWhatType;


//
// History for a thread
//
class CActionHistory
{
public:
    volatile bool bChanged;
    uint uiLastChangedTime;

    CActionHistory ( void ) : bChanged ( 0 ), uiLastChangedTime ( 0 ) {}
    void AddAction ( EActionWhoType uiWho, EActionWhereType uiWhere, EActionWhatType uiWhat, int iExtra = 0 )
    {
        bChanged = true;
    }
};


//
// History for two threads
//
class CActionHistorySet
{
public:
    CActionHistory main;
    CActionHistory sync;

    void AddAction ( EActionWhoType uiWho, EActionWhereType uiWhere, EActionWhatType uiWhat, int iExtra = 0 )
    {
        dassert ( IsMainThread () == ( uiWho == EActionWho::MAIN ) );

        if ( uiWho == EActionWho::MAIN )
            main.AddAction ( EActionWho::MAIN, uiWhere, uiWhat, iExtra );
        else
            sync.AddAction ( EActionWho::SYNC, uiWhere, uiWhat, iExtra );
    }
};
extern CActionHistorySet      g_HistorySet;

//
// Debug mutex
//
class CDebugComboMutex : public CComboMutex
{
public:
    void Lock ( EActionWhoType uiWho, EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWho, uiWhere, EActionWhat::PRE_LOCK );
        CComboMutex::Lock ();
        g_HistorySet.AddAction ( uiWho, uiWhere, EActionWhat::POST_LOCK );
    }

    void Unlock ( EActionWhoType uiWho, EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWho, uiWhere, EActionWhat::PRE_UNLOCK );
        CComboMutex::Unlock ();
        g_HistorySet.AddAction ( uiWho, uiWhere, EActionWhat::POST_UNLOCK );
    }

    // unlock - wait for signal - lock
    // Returns ETIMEDOUT if timeout period expired
    int Wait ( uint uiTimeout, EActionWhoType uiWho, EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWho, uiWhere, EActionWhat::PRE_WAIT, uiTimeout );
        int iResult = CComboMutex::Wait ( uiTimeout );
        g_HistorySet.AddAction ( uiWho, uiWhere, EActionWhat::POST_WAIT );
        return iResult;
    }

    void Signal ( EActionWhoType uiWho, EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWho, uiWhere, EActionWhat::SIGNAL );
        CComboMutex::Signal ();
    }
};


//
// Monitor queues and locks for trouble
//
class CNetBufferWatchDog
{
public:
    ZERO_ON_NEW
                                            CNetBufferWatchDog              ( CNetServerBuffer* pNetBuffer );
                                            ~CNetBufferWatchDog             ( void );

    // Main thread functions
    void                        StopThread                  ( void );

    // Check thread functions
    static void*                StaticThreadProc            ( void* pContext );
    void*                       ThreadProc                  ( void );
    void                        DoChecks                    ( void );
    void                        CheckActionHistory          ( CActionHistory& history, const char* szTag, uint& uiDoneMessage );
    void                        CheckQueueSize              ( uint uiSizeLimit, uint uiSize, const char* szTag, uint& uiHigh );

    // Main thread variables
    CThreadHandle*              m_pServiceThreadHandle;

    // Check thread variables
    CNetServerBuffer*           m_pNetBuffer;
    uint                        m_uiMainAgeHigh;
    uint                        m_uiSyncAgeHigh;
    uint                        m_uiFinishedListHigh;
    uint                        m_uiOutCommandQueueHigh;
    uint                        m_uiOutResultQueueHigh;
    uint                        m_uiInResultQueueHigh;

    // Shared variables
    struct
    {
        bool                                        m_bTerminateThread;
        bool                                        m_bThreadTerminated;
        CComboMutex                                 m_Mutex;
    } shared;
};









