/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CServerBase.h
 *  PURPOSE:     Core server base interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CServerInterface.h"

class CServerBase
{
public:
    virtual void ServerInitialize(CServerInterface* pServer) noexcept = 0;
    virtual bool ServerStartup(int iArgumentCount, char* szArguments[]) noexcept = 0;
    virtual void ServerShutdown() noexcept = 0;

    virtual void DoPulse() noexcept = 0;
    virtual void HandleInput(const char* szCommand) noexcept = 0;
    virtual void GetTag(char* szInfoTag, int iInfoTag) const noexcept = 0;

    virtual bool IsFinished() const noexcept = 0;
    virtual bool PendingWorkToDo() noexcept = 0;
    virtual bool GetSleepIntervals(int& iSleepBusyMs, int& iSleepIdleMs, int& iLogicFpsLimit) const noexcept = 0;
};
