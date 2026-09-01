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

struct SSoundDeviceInfo
{
    std::string strName;
    std::string strDriver;
    bool        bIsDefault;
};

class CClientBase
{
public:
    virtual int  ClientInitialize(const char* szArguments, CCoreInterface* pCoreInterface) = 0;
    virtual void ClientShutdown() = 0;

    virtual void PreFrameExecutionHandler() = 0;
    virtual void PreHUDRenderExecutionHandler(bool bDidUnminimize, bool bDidRecreateRenderTargets) = 0;
    virtual void PostFrameExecutionHandler() = 0;
    virtual void IdleHandler() = 0;
    virtual void RestreamModel(std::uint16_t model) = 0;

    virtual bool WebsiteRequestResultHandler(const std::unordered_set<SString>& newPages) = 0;

    virtual bool ProcessCommand(const char* commandName, size_t commandNameLength, const void* userdata, size_t userdataSize) = 0;

    virtual bool HandleException(CExceptionInformation* pExceptionInformation) = 0;
    virtual void GetPlayerNames(std::vector<SString>& vPlayerNames) = 0;

    virtual void OnWindowFocusChange(bool state) = 0;
    virtual void OnPossibleAudioDeviceChange() = 0;

    virtual unsigned int                  GetSoundOutputDeviceListRevision() = 0;
    virtual std::vector<SSoundDeviceInfo> GetSoundOutputDevices() = 0;
    virtual std::string                   GetSoundOutputDeviceDriver() = 0;
    virtual bool                          SetSoundOutputDevice(const std::string& strDriver) = 0;
};
