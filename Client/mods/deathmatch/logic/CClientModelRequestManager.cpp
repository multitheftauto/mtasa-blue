/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientModelRequestManager.cpp
 *  PURPOSE:     Entity model streaming manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <chrono>

using std::list;

// Number of background worker threads used to poll/re-request models that are still loading.
// Kept small: all engine calls are serialized behind m_ModelInfoMutex anyway, so more threads
// would just contend on that lock rather than do useful parallel work.
static constexpr unsigned int kNumModelWorkerThreads = 2;

// How long a worker sleeps between polls of an in-flight model request.
static constexpr int kWorkerPollIntervalMs = 20;

CClientModelRequestManager::CClientModelRequestManager()
{
    m_bDoingPulse = false;
    StartWorkers(kNumModelWorkerThreads);
}

CClientModelRequestManager::~CClientModelRequestManager()
{
    // Stop workers first so nothing is touching m_Requests/entries while we tear them down.
    StopWorkers();

    // Delete all our requests.
    list<SClientModelRequest*>::iterator iter;
    for (iter = m_Requests.begin(); iter != m_Requests.end(); iter++)
    {
        delete *iter;
    }

    m_Requests.clear();
}

void CClientModelRequestManager::StartWorkers(unsigned int uiNumThreads)
{
    m_bShutdownWorkers = false;
    m_WorkerThreads.reserve(uiNumThreads);
    for (unsigned int i = 0; i < uiNumThreads; ++i)
        m_WorkerThreads.emplace_back(&CClientModelRequestManager::WorkerLoop, this);
}

void CClientModelRequestManager::StopWorkers()
{
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_bShutdownWorkers = true;
    }
    m_Cv.notify_all();

    for (auto& thread : m_WorkerThreads)
    {
        if (thread.joinable())
            thread.join();
    }
    m_WorkerThreads.clear();
}

// Runs on a background thread. Pops entries off m_BackgroundQueue and polls/retries them until
// they're either loaded or cancelled, then hands them back to the main thread via
// bBackgroundProcessed. Never deletes an entry and never calls ModelRequestCallback/MakeCustomModel
// itself - that stays on the main thread inside DoPulse().
void CClientModelRequestManager::WorkerLoop()
{
    for (;;)
    {
        SClientModelRequest* pEntry = nullptr;

        // Wait for work or shutdown
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Cv.wait(lock, [this] { return m_bShutdownWorkers || !m_BackgroundQueue.empty(); });

            if (m_bShutdownWorkers)
                return;

            pEntry = m_BackgroundQueue.front();
            m_BackgroundQueue.pop();
        }

        // Process this entry until it's loaded or cancelled
        for (;;)
        {
            if (m_bShutdownWorkers || pEntry->bCancelled.load())
            {
                // Hand back to the main thread for cleanup. Don't touch the engine or the
                // entity - Cancel()/the destructor already own responsibility for that.
                pEntry->bBackgroundProcessed = true;
                break;
            }

            // Snapshot the fields we need under the lock, since Request() can reassign
            // pModel/reset requestTimer on the main thread while this entry is in flight.
            CModelInfo* pModel;
            bool        bShouldRetry;
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                pModel = pEntry->pModel;
                bShouldRetry = pEntry->requestTimer.Get() > 2000;
                if (bShouldRetry)
                    pEntry->requestTimer.Reset();
            }

            bool bLoaded;
            {
                std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                bLoaded = pModel->IsLoaded();
                if (!bLoaded && bShouldRetry)
                {
                    // Request it again. Don't add a reference, or we screw up the reference count.
                    if (g_pGame->IsASyncLoadingEnabled())
                        pModel->Request(NON_BLOCKING, "CClientModelRequestManager::WorkerLoop #1");
                    else
                        pModel->Request(BLOCKING, "CClientModelRequestManager::WorkerLoop #2");
                }
            }

            if (bLoaded)
            {
                pEntry->bBackgroundProcessed = true;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(kWorkerPollIntervalMs));
        }
    }
}

bool CClientModelRequestManager::IsLoaded(unsigned short usModelID)
{
    // Grab the model info
    CModelInfo* pInfo = g_pGame->GetModelInfo(usModelID);
    if (pInfo)
    {
        std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
        return pInfo->IsLoaded() ? true : false;
    }

    return false;
}

bool CClientModelRequestManager::IsRequested(CModelInfo* pModelInfo)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    // Look through the list
    std::list<SClientModelRequest*>::iterator iter = m_Requests.begin();
    for (; iter != m_Requests.end(); iter++)
    {
        // Same model as this entry?
        if ((*iter)->pModel == pModelInfo && !(*iter)->bCancelled)
        {
            return true;
        }
    }

    // Not in request list
    return false;
}

bool CClientModelRequestManager::HasRequested(CClientEntity* pRequester)
{
    assert(pRequester);

    std::lock_guard<std::mutex> lock(m_Mutex);

    // Look through the list
    std::list<SClientModelRequest*>::iterator iter = m_Requests.begin();
    for (; iter != m_Requests.end(); iter++)
    {
        // Same requester as we check for? He has requested something.
        if ((*iter)->pEntity == pRequester && !(*iter)->bCancelled)
        {
            return true;
        }
    }

    // Not requested anything
    return false;
}

CModelInfo* CClientModelRequestManager::GetRequestedModelInfo(CClientEntity* pRequester)
{
    assert(pRequester);

    std::lock_guard<std::mutex> lock(m_Mutex);

    // Look through the list
    std::list<SClientModelRequest*>::iterator iter = m_Requests.begin();
    for (; iter != m_Requests.end(); iter++)
    {
        // Same requester as we check for? He has requested something.
        if ((*iter)->pEntity == pRequester && !(*iter)->bCancelled)
        {
            // Return the model info he requested
            return (*iter)->pModel;
        }
    }

    // Not requested anything
    return NULL;
}

bool CClientModelRequestManager::RequestBlocking(unsigned short usModelID, const char* szTag)
{
    // Grab the model info
    CModelInfo* pInfo = g_pGame->GetModelInfo(usModelID);
    if (pInfo)
    {
        std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
        pInfo->Request(BLOCKING, szTag);
        if (pInfo->IsLoaded())
        {
            pInfo->MakeCustomModel();
            return true;
        }
        OutputDebugLine(SString("[Models] RequestBlocking failed for id %d", usModelID));
    }

    // Bad model ID probably.
    return false;
}

bool CClientModelRequestManager::Request(unsigned short usModelID, CClientEntity* pRequester)
{
    assert(pRequester);
    SClientModelRequest* pEntry;

    // Grab the model info for that model
    CModelInfo* pInfo = g_pGame->GetModelInfo(usModelID);
    if (pInfo)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);

        // Has it already requested something?
        list<SClientModelRequest*>::iterator iter;
        if (GetRequestEntry(pRequester, iter))
        {
            // Get the entry
            pEntry = *iter;

            // The same model?
            if (pInfo == pEntry->pModel)
            {
                // He has to wait more for it
                return false;
            }
            else
            {
                // Remove the reference to the old model
                {
                    std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                    pEntry->pModel->RemoveRef();
                }

                // Is it loaded? Only safe to check/consume synchronously here if no worker
                // currently owns this entry (i.e. it already finished processing).
                bool bAlreadyLoaded = pEntry->bBackgroundProcessed && pInfo->IsLoaded();
                if (bAlreadyLoaded)
                {
                    // Delete it, remove it from the list and return true.
                    delete pEntry;
                    m_Requests.erase(iter);

                    std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                    pInfo->MakeCustomModel();
                    return true;
                }
                else
                {
                    // If not loaded (or a worker still owns this entry), replace the model
                    // we're going to load and hand it back to the background queue.
                    pEntry->pModel = pInfo;
                    pEntry->requestTimer.Reset();
                    pEntry->bBackgroundProcessed = false;

                    {
                        std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                        pInfo->ModelAddRef(NON_BLOCKING, "CClientModelRequestManager::Request");
                    }

                    m_BackgroundQueue.push(pEntry);
                    lock.unlock();
                    m_Cv.notify_one();

                    // He has to wait for it.
                    return false;
                }
            }
        }
        else
        {
            // Already loaded? Don't bother adding to the list.
            {
                std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                if (pInfo->IsLoaded())
                {
                    pInfo->MakeCustomModel();
                    return true;
                }

                // Request it
                pInfo->ModelAddRef(NON_BLOCKING, "CClientModelRequestManager::Request #2");
            }

            // Add him to the list over models we're waiting for.
            pEntry = new SClientModelRequest;
            pEntry->pModel = pInfo;
            pEntry->pEntity = pRequester;
            pEntry->requestTimer.SetMaxIncrement(500);
            pEntry->requestTimer.Reset();
            m_Requests.push_back(pEntry);

            // Hand it to the workers to poll/retry in the background.
            m_BackgroundQueue.push(pEntry);
            lock.unlock();
            m_Cv.notify_one();

            // Return false. Caller needs to wait.
            return false;
        }
    }

    // Error, model is bad. Caller should not do this.
    return false;
}

void CClientModelRequestManager::Cancel(CClientEntity* pEntity, bool bAllowQueue)
{
    assert(pEntity);

    std::lock_guard<std::mutex> lock(m_Mutex);

    // Check to ensure entity has not got its knickers in a twist
    if (ListContains(m_CancelQueue, pEntity))
        return;

    // Mark any matching, not-yet-finished entries as cancelled. We do NOT delete or erase them
    // here: a worker thread may currently own the entry. DoPulse() is the only place entries are
    // deleted, and only once bBackgroundProcessed is true, so it's always safe from there.
    for (auto* pEntry : m_Requests)
    {
        if (pEntry->pEntity == pEntity)
            pEntry->bCancelled = true;
    }

    m_Cv.notify_all();
}

void CClientModelRequestManager::DoPulse()
{
    std::unique_lock<std::mutex> lock(m_Mutex);

    // Any requests?
    if (m_Requests.size() > 0)
    {
        // We are now doing the pulse
        m_bDoingPulse = true;

        // Reap entries a worker has finished with (loaded or cancelled) and remove them from the list
        SClientModelRequest*                 pEntry;
        list<SClientModelRequest*>::iterator iter;
        for (iter = m_Requests.begin(); iter != m_Requests.end();)
        {
            pEntry = *iter;

            // Still being processed by a worker? Leave it alone.
            if (!pEntry->bBackgroundProcessed)
            {
                ++iter;
                continue;
            }

            if (pEntry->bCancelled)
            {
                // Cancelled: undo the reference we added and just drop it, no callback.
                CModelInfo* pModel = pEntry->pModel;

                delete pEntry;
                iter = m_Requests.erase(iter);

                std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                pModel->RemoveRef();

                continue;
            }

            // Copy then remove from the list because the request is complete and we don't want it
            // modified in Request()
            CModelInfo*    pModel = pEntry->pModel;
            CClientEntity* pEntity = pEntry->pEntity;

            delete pEntry;
            iter = m_Requests.erase(iter);

            // Engine work + the callback happen with the manager's own mutex unlocked, so that
            // ModelRequestCallback is free to call back into Request()/Cancel() without deadlocking.
            lock.unlock();

            {
                std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                // Make sure custom things are replaced
                pModel->MakeCustomModel();
            }

            // Create ped/object/vehicle using the loaded model (this can eventually trigger script events)
            pEntity->ModelRequestCallback(pModel);

            {
                // Unreference us from the model (callback should've added a reference!)
                std::lock_guard<std::mutex> engineLock(m_ModelInfoMutex);
                pModel->RemoveRef();
            }

            lock.lock();

            // The list may have changed while unlocked - restart the scan.
            iter = m_Requests.begin();
        }

        // No longer doing the pulse
        m_bDoingPulse = false;

        // Cancel what we've scheduled for cancel now if anything
        if (m_CancelQueue.size() > 0)
        {
            // Cancel every entity in our cancel list
            list<CClientEntity*> cancelQueueCopy = m_CancelQueue;
            m_CancelQueue.clear();

            lock.unlock();
            list<CClientEntity*>::iterator cancelIter = cancelQueueCopy.begin();
            for (; cancelIter != cancelQueueCopy.end(); ++cancelIter)
            {
                Cancel(*cancelIter, false);
            }
            lock.lock();
        }
    }
}

bool CClientModelRequestManager::GetRequestEntry(CClientEntity* pRequester, list<SClientModelRequest*>::iterator& iterOut)
{
    // Look through the list
    std::list<SClientModelRequest*>::iterator iter = m_Requests.begin();
    for (; iter != m_Requests.end(); iter++)
    {
        // Same requester as we check for? He has requested something.
        if ((*iter)->pEntity == pRequester)
        {
            // Pass out the iterator entry and return true
            iterOut = iter;
            return true;
        }
    }

    // Not requested anything
    return false;
}
