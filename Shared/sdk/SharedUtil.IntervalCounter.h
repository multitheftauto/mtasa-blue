/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.IntervalCounter.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

namespace SharedUtil
{
    //
    // Increments supplied pointer contents using threads and stuff
    //
    class CIntervalCounter : public CRefCountable
    {
    public:
        typedef uchar T;
        CIntervalCounter(uint uiMinIntervalMs, T* pCounter);

    private:
        virtual ~CIntervalCounter();

    public:
        void         StopThread();
        static void* StaticThreadProc(void* pContext);
        void*        ThreadProc();

    protected:
        // Main thread variables
        CThreadHandle* m_pServiceThreadHandle = nullptr;
        uint           m_uiMinIntervalMs = 0;

        // Sync thread variables
        T m_InternalCounter = 0;

        // Shared variables
        struct
        {
            T*          m_pCounter = 0;
            bool        m_bTerminateThread = false;
            bool        m_bThreadTerminated = false;
            CComboMutex m_Mutex;
        } shared;
    };

}            // namespace SharedUtil
