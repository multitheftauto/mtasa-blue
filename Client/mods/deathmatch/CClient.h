/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CClient.h
 *  PURPOSE:     Header file for Client class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CClientBase.h>

class CClient : public CClientBase
{
public:
    int  ClientInitialize(const char* szArguments, CCoreInterface* pCore);
    void ClientShutdown();

    void PreFrameExecutionHandler();
    void PreHUDRenderExecutionHandler(bool bDidUnminimize, bool bDidRecreateRenderTargets);
    void PostFrameExecutionHandler();
    void IdleHandler();
    void RestreamModel(unsigned short usModel);

    bool WebsiteRequestResultHandler(const std::unordered_set<SString>& newPages);

    bool ProcessCommand(const char* commandName, size_t commandNameLength, const void* userdata, size_t userdataSize) override;

    bool HandleException(CExceptionInformation* pExceptionInformation);
    void GetPlayerNames(std::vector<SString>& vPlayerNames);

    void OnWindowFocusChange(bool state) override;

private:
    struct InitializeArguments
    {
        std::string nickname;
        std::string password;
    };

    static InitializeArguments ExtractInitializeArguments(const char* arguments);
};
