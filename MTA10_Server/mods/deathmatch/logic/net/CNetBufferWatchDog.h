/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


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
    const static int HistoryLength = 60;
    struct SItem
    {
        EActionWhoType uiWho;
        EActionWhereType uiWhere;
        EActionWhatType uiWhat;
        uint uiWhen;
        int iExtra;
    };
    std::vector < SItem > itemList;
    uint position;
    CCriticalSection cs;

    CActionHistory ( void ) : position ( 0 ) {}
    void AddAction ( EActionWhoType uiWho, EActionWhereType uiWhere, EActionWhatType uiWhat, int iExtra = 0 )
    {
        cs.Lock ();
        if ( itemList.size () < HistoryLength )
        {
            position = itemList.size ();
            itemList.push_back ( SItem() );
        }
        else
        {
            position = ( position + 1 ) % HistoryLength;
        }
        SItem& item = itemList[position];
        item.uiWho = uiWho;
        item.uiWhere = uiWhere;
        item.uiWhat = uiWhat;
        item.uiWhen = GetTickCount32 ();
        item.iExtra = iExtra;
        cs.Unlock ();
    }

    bool GetLastItem ( SItem& outItem )
    {
        cs.Lock ();
        bool bResult = false;
        if ( position > 0 )
        {
            outItem = itemList[position];
            bResult = true;
        }
        cs.Unlock ();
        return bResult;
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

    void AddAction ( EActionWhereType uiWhere, EActionWhatType uiWhat, int iExtra = 0 )
    {
        if ( IsMainThread () )
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
    void Lock ( EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWhere, EActionWhat::PRE_LOCK );
        CComboMutex::Lock ();
        g_HistorySet.AddAction ( uiWhere, EActionWhat::POST_LOCK );
    }

    void Unlock ( EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWhere, EActionWhat::PRE_UNLOCK );
        CComboMutex::Unlock ();
        g_HistorySet.AddAction ( uiWhere, EActionWhat::POST_UNLOCK );
    }

    // unlock - wait for signal - lock
    // Returns ETIMEDOUT if timeout period expired
    int Wait ( uint uiTimeout, EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWhere, EActionWhat::PRE_WAIT, uiTimeout );
        int iResult = CComboMutex::Wait ( uiTimeout );
        g_HistorySet.AddAction ( uiWhere, EActionWhat::POST_WAIT );
        return iResult;
    }

    void Signal ( EActionWhereType uiWhere )
    {
        g_HistorySet.AddAction ( uiWhere, EActionWhat::SIGNAL );
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
                                            CNetBufferWatchDog              ( void );
                                            ~CNetBufferWatchDog             ( void );

    // CNetBufferWatchDog interface
    //virtual bool                            StartNetwork                    ( const char* szIP, unsigned short usServerPort, unsigned int uiAllowedPlayers );
    void                        RecordQueueSizes            ( uint uiFinishedList , uint uiOutCommandQueue, uint uiOutResultQueue, uint uiInResultQueue );

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
        uint                                        m_uiFinishedList;
        uint                                        m_uiOutCommandQueue;
        uint                                        m_uiOutResultQueue;
        uint                                        m_uiInResultQueue;
        CComboMutex                                 m_Mutex;
    } shared;
};









