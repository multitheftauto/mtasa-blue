/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseJobQueueManager.cpp
 *  PURPOSE:     Threaded job queue
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDatabaseJobQueue.h"
#include "CDatabaseJobQueueManager.h"
#include "CDatabaseManager.h"

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::~CDatabaseJobQueueManager
//
// Stop threads and delete everything
//
///////////////////////////////////////////////////////////////
CDatabaseJobQueueManager::~CDatabaseJobQueueManager()
{
    for (auto iter : m_QueueNameMap)
    {
        SAFE_DELETE(iter.second);
    }
    m_QueueNameMap.clear();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::AddCommand
//
// AddCommand to correct queue
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseJobQueueManager::AddCommand(EJobCommandType jobType, SConnectionHandle connectionHandle, const SString& strData)
{
    CDatabaseJobQueue* pJobQueue;
    if (jobType == EJobCommand::CONNECT)
    {
        connectionHandle = GetNextConnectionHandle();
        pJobQueue = GetQueueFromConnectCommand(strData);
    }
    else
    {
        pJobQueue = FindQueueFromConnection(connectionHandle);
        if (!pJobQueue)
        {
            return nullptr;
        }
    }
    return pJobQueue->AddCommand(jobType, connectionHandle, strData);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::DoPulse
//
// Check if any callback functions are due
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueManager::DoPulse()
{
    for (const auto iter : m_QueueNameMap)
    {
        iter.second->DoPulse();
    }
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::PollCommand
//
// Find result for previous command
// Returns false if result not ready or invalid
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueueManager::PollCommand(CDbJobData* pJobData, uint uiTimeout)
{
    if (!pJobData || !pJobData->GetQueue())
        return false;
    return pJobData->GetQueue()->PollCommand(pJobData, uiTimeout);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::FreeCommand
//
// Throw away result when this job is done
// Returns false if job data incorrect
//
///////////////////////////////////////////////////////////////
bool CDatabaseJobQueueManager::FreeCommand(CDbJobData* pJobData)
{
    if (!pJobData || !pJobData->GetQueue())
        return false;
    return pJobData->GetQueue()->FreeCommand(pJobData);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::FindCommandFromId
//
// Find job data with matching id
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseJobQueueManager::FindCommandFromId(SDbJobId id)
{
    for (const auto iter : m_QueueNameMap)
    {
        CDbJobData* pJobData = iter.second->FindCommandFromId(id);
        if (pJobData)
            return pJobData;
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::IgnoreConnectionResults
//
// Throw away results for all jobs using this connection
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueManager::IgnoreConnectionResults(SConnectionHandle connectionHandle)
{
    CDatabaseJobQueue* pJobQueue = FindQueueFromConnection(connectionHandle);
    if (!pJobQueue)
        return;

    pJobQueue->IgnoreConnectionResults(connectionHandle);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::SetLogLevel
//
// Update all queues with new log level
//
///////////////////////////////////////////////////////////////
void CDatabaseJobQueueManager::SetLogLevel(EJobLogLevelType logLevel, const SString& strLogFilename)
{
    CDbOptionsMap argMap;
    argMap.Set("name", strLogFilename);
    argMap.Set("level", logLevel);
    for (const auto iter : m_QueueNameMap)
    {
        CDbJobData* pJobData = iter.second->AddCommand(EJobCommand::SETLOGLEVEL, 0, argMap.ToString());
        FreeCommand(pJobData);
    }
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::FindQueueFromConnection
//
// Find existing queue or fail
//
///////////////////////////////////////////////////////////////
CDatabaseJobQueue* CDatabaseJobQueueManager::FindQueueFromConnection(SConnectionHandle connectionHandle)
{
    for (const auto iter : m_QueueNameMap)
    {
        if (iter.second->UsesConnection(connectionHandle))
            return iter.second;
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::GetQueueFromConnectCommand
//
// Find existing queue or create new one
// Can't fail
//
///////////////////////////////////////////////////////////////
CDatabaseJobQueue* CDatabaseJobQueueManager::GetQueueFromConnectCommand(const SString& strData)
{
    // Extract queue name from options
    std::vector<SString> parts;
    strData.Split("\1", parts);
    SString strQueueName;
    GetOption<CDbOptionsMap>(parts[4], "queue", strQueueName);

    // Find queue with name
    CDatabaseJobQueue* pQueue = MapFindRef(m_QueueNameMap, strQueueName);
    if (!pQueue)
    {
        // Add new queue
        pQueue = NewDatabaseJobQueue();
        MapSet(m_QueueNameMap, strQueueName, pQueue);
    }
    return pQueue;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager::GetNextConnectionHandle
//
// Return unused handle within correct range
//
///////////////////////////////////////////////////////////////
SConnectionHandle CDatabaseJobQueueManager::GetNextConnectionHandle()
{
    do
    {
        m_ConnectionHandleCounter++;
        m_ConnectionHandleCounter &= 0x000FFFFF;
        m_ConnectionHandleCounter |= 0x00200000;
        // TODO - check when all (1,048,575) ids are in use
    } while (FindQueueFromConnection(m_ConnectionHandleCounter));

    return m_ConnectionHandleCounter;
}
