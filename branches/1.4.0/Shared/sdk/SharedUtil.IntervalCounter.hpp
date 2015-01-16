/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.IntervalCounter.hpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{

    ///////////////////////////////////////////////////////////////
    //
    // CIntervalCounter
    //
    // Increments supplied pointer contents using threads and stuff
    // uiMinIntervalMs is the minimum time between incremenets. Actual interval could be slightly longer.
    //
    ///////////////////////////////////////////////////////////////
    CIntervalCounter::CIntervalCounter( uint uiMinIntervalMs, T* pCounter )
    {
        m_uiMinIntervalMs = uiMinIntervalMs;
        shared.m_pCounter = pCounter;
        m_pServiceThreadHandle = new CThreadHandle( CIntervalCounter::StaticThreadProc, this );
    }

    CIntervalCounter::~CIntervalCounter( void )
    {
        // Stop and delete processing thread
        StopThread ();
        SAFE_DELETE ( m_pServiceThreadHandle );
    }

    ///////////////////////////////////////////////////////////////
    //
    // CLuaManager::StopThread
    //
    // Stop the processing thread
    //
    ///////////////////////////////////////////////////////////////
    void CIntervalCounter::StopThread ( void )
    {
        // Stop the processing thread
        shared.m_Mutex.Lock ();
        shared.m_bTerminateThread = true;
        shared.m_Mutex.Signal ();
        shared.m_Mutex.Unlock ();

        for ( uint i = 0 ; i < m_uiMinIntervalMs * 2 ; i += 15 )
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
    // CIntervalCounter::StaticThreadProc
    //
    // static function
    // Thread entry point
    //
    ///////////////////////////////////////////////////////////////
    void* CIntervalCounter::StaticThreadProc ( void* pContext )
    {
        CThreadHandle::AllowASyncCancel ();
        return ((CIntervalCounter*)pContext)->ThreadProc ();
    }


    ///////////////////////////////////////////////////////////////
    //
    // CIntervalCounter::ThreadProc
    //
    // Thread loop
    //
    ///////////////////////////////////////////////////////////////
    void* CIntervalCounter::ThreadProc ( void )
    {
        shared.m_Mutex.Lock ();
        while ( !shared.m_bTerminateThread )
        {
            m_InternalCounter++;
            *shared.m_pCounter = m_InternalCounter;
            shared.m_Mutex.Wait( m_uiMinIntervalMs );
        }
        shared.m_bThreadTerminated = true;
        shared.m_Mutex.Unlock ();
        return NULL;
    }


    ///////////////////////////////////////////////////////////////
    //
    // Setup for CElapsedTimeApprox using CIntervalCounter
    //
    //
    //
    ///////////////////////////////////////////////////////////////
    static CRefCountable*       g_pIntervalCounter = NULL;
    static uchar                g_ucCounterValue = 0;

    void CElapsedTimeApprox::StaticInitialize( CElapsedTimeApprox* pTimer )
    {
        dassert( !pTimer->m_bInitialized );
        pTimer->m_bInitialized = true;

        if ( !g_pIntervalCounter )
            g_pIntervalCounter = new CIntervalCounter( 100, &g_ucCounterValue );
        else
            g_pIntervalCounter->AddRef();

        pTimer->m_pucCounterValue = &g_ucCounterValue;
        pTimer->m_ppIntervalCounter = &g_pIntervalCounter;
        pTimer->Reset();
    }

}
