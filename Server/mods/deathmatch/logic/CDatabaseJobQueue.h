/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseJobQueue.h
 *  PURPOSE:     Threaded job queue
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CDatabaseManager.h"

///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueue
//
//      JobQueue -> DatabaseType -> DatabaseConnection
//
///////////////////////////////////////////////////////////////
class CDatabaseJobQueue
{
public:
    virtual ~CDatabaseJobQueue() {}

    virtual void        DoPulse() = 0;
    virtual CDbJobData* AddCommand(EJobCommandType jobType, SConnectionHandle connectionHandle, const SString& strData) = 0;
    virtual bool        PollCommand(CDbJobData* pJobData, uint uiTimeout) = 0;
    virtual bool        FreeCommand(CDbJobData* pJobData) = 0;
    virtual CDbJobData* FindCommandFromId(SDbJobId id) = 0;
    virtual void        IgnoreConnectionResults(SConnectionHandle connectionHandle) = 0;
    virtual bool        UsesConnection(SConnectionHandle connectionHandle) = 0;
};

CDatabaseJobQueue* NewDatabaseJobQueue();
