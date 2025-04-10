/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseJobQueueManager.h
 *  PURPOSE:     Threaded job queue
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueueManager
//
//
///////////////////////////////////////////////////////////////
class CDatabaseJobQueueManager
{
public:
    ZERO_ON_NEW
    ~CDatabaseJobQueueManager();
    void        DoPulse();
    CDbJobData* AddCommand(EJobCommandType jobType, SConnectionHandle connectionHandle, const SString& strData);
    bool        PollCommand(CDbJobData* pJobData, uint uiTimeout);
    bool        FreeCommand(CDbJobData* pJobData);
    CDbJobData* FindCommandFromId(SDbJobId id);
    void        IgnoreConnectionResults(SConnectionHandle connectionHandle);
    void        SetLogLevel(EJobLogLevelType logLevel, const SString& strLogFilename);

protected:
    CDatabaseJobQueue* GetQueueFromConnectCommand(const SString& strData);
    CDatabaseJobQueue* FindQueueFromConnection(SConnectionHandle connectionHandle);
    SConnectionHandle  GetNextConnectionHandle();

    std::map<SString, CDatabaseJobQueue*> m_QueueNameMap;
    SConnectionHandle                     m_ConnectionHandleCounter;
};
