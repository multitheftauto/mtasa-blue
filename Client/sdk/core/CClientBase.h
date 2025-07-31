/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CClientBase.h
 *  PURPOSE:     Client base interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCoreInterface.h"
#include "CExceptionInformation.h"

class CClientBase
{
public:
    virtual int  ClientInitialize(const char* szArguments, CCoreInterface* pCoreInterface) = 0;
    virtual void ClientShutdown() = 0;

    virtual void PreFrameExecutionHandler() = 0;
    virtual void PreHUDRenderExecutionHandler(bool bDidUnminimize, bool bDidRecreateRenderTargets) = 0;
    virtual void PostFrameExecutionHandler() = 0;
    virtual void IdleHandler() = 0;
    virtual void RestreamModel(unsigned short usModel) = 0;

    virtual bool WebsiteRequestResultHandler(const std::unordered_set<SString>& newPages) = 0;

    virtual bool ProcessCommand(const char* commandName, size_t commandNameLength, const void* userdata, size_t userdataSize) = 0;

    virtual bool HandleException(CExceptionInformation* pExceptionInformation) = 0;
    virtual void GetPlayerNames(std::vector<SString>& vPlayerNames) = 0;

    virtual void OnWindowFocusChange(bool state) = 0;
};
