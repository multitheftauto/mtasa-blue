/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientModelRequestManager.h
 *  PURPOSE:     Entity model streaming manager class
 *
 *****************************************************************************/

class CClientModelRequestManager;

#pragma once

#include "CClientCommon.h"
#include "CClientEntity.h"
#include <list>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <atomic>

struct SClientModelRequest
{
    CModelInfo*       pModel;
    CClientEntity*    pEntity;
    CElapsedTime      requestTimer;
    std::atomic<bool> bBackgroundProcessed{false};  // Set by a worker thread once the model has finished
                                                    // loading (or the request was cancelled). Only the main
                                                    // thread (DoPulse) is allowed to delete/erase the entry,
                                                    // and only after this is true.
    std::atomic<bool> bCancelled{false};            // Set by Cancel(). Tells the worker to stop polling this
                                                    // entry as soon as possible.
};

class CClientModelRequestManager
{
    friend class CClientManager;

public:
    CClientModelRequestManager();
    ~CClientModelRequestManager();

    bool        IsLoaded(unsigned short usModelID);
    bool        IsRequested(CModelInfo* pModelInfo);
    bool        HasRequested(CClientEntity* pRequester);
    CModelInfo* GetRequestedModelInfo(CClientEntity* pRequester);

    bool RequestBlocking(unsigned short usModelID, const char* szTag);

    bool Request(unsigned short usModelID, CClientEntity* pRequester);
    void Cancel(CClientEntity* pRequester, bool bAllowQueue);

private:
    void DoPulse();
    bool GetRequestEntry(CClientEntity* pRequester, std::list<SClientModelRequest*>::iterator& iter);

    void StartWorkers(unsigned int uiNumThreads);
    void StopWorkers();
    void WorkerLoop();

    bool                            m_bDoingPulse;
    std::list<SClientModelRequest*> m_Requests;

    std::vector<std::thread>         m_WorkerThreads;
    std::queue<SClientModelRequest*> m_BackgroundQueue;

    std::mutex m_Mutex;

    std::mutex m_ModelInfoMutex;

    std::condition_variable m_Cv;
    bool                    m_bShutdownWorkers = false;
};
