/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

class CCompressorJobQueueImpl : public CCompressorJobQueue
{
public:
    ZERO_ON_NEW
    CCompressorJobQueueImpl(void);
    virtual                     ~CCompressorJobQueueImpl(void);

    // Main thread functions
    virtual void                DoPulse(void);
    virtual CCompressJobData*   AddCommand(uint uiSizeX, uint uiSizeY, uint uiQuality, uint uiTimeSpentInQueue, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, const CBuffer& buffer);
    virtual bool                PollCommand(CCompressJobData* pJobData, uint uiTimeout);
    virtual bool                FreeCommand(CCompressJobData* pJobData);

protected:
    void                        StopThread(void);
    void                        RemoveUnwantedResults(void);
    void                        IgnoreJobResults(CCompressJobData* pJobData);

    // Other thread functions
    static void*                StaticThreadProc(void* pContext);
    void*                       ThreadProc(void);
    void                        ProcessCommand(CCompressJobData* pJobData);
    void                        ProcessCompress(uint uiSizeX, uint uiSizeY, uint uiQuality, const CBuffer& inBuffer, CBuffer& outBuffer);

    // Main thread variables
    CThreadHandle                           m_ServiceThreadHandle;
    std::set < CCompressJobData* >          m_IgnoreResultList;
    std::set < CCompressJobData* >          m_FinishedList;         // Result has been used, will be deleted next pulse

                                                                    // Other thread variables
                                                                    // -none-

                                                                    // Shared variables
    struct
    {
        bool                                m_bTerminateThread;
        bool                                m_bThreadTerminated;
        std::list < CCompressJobData* >     m_CommandQueue;
        std::list < CCompressJobData* >     m_ResultQueue;
        CComboMutex                         m_Mutex;
    } shared;
};
