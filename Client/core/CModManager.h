/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CModManager.h
 *  PURPOSE:     Header file for game mod manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CModManagerInterface.h>
#include <core/CClientBase.h>
#include "CSingleton.h"

using HISTANCE = struct HINSTANCE__*;
using HMODULE = HISTANCE;

class CModManager : public CModManagerInterface, public CSingleton<CModManager>
{
public:
    ~CModManager() { Unload(); }

    void RequestLoad(const char* arguments) override;
    void RequestUnload() override;

    bool         IsLoaded() const override { return m_library != nullptr; }
    CClientBase* GetClient() override { return m_client; }

    bool TriggerCommand(const char* commandName, size_t commandNameLength, const void* userdata, size_t userdataSize) const override;

public:
    void DoPulsePreFrame();
    void DoPulsePreHUDRender(bool bDidUnminimize, bool bDidRecreateRenderTargets);
    void DoPulsePostFrame();

    bool Load(const char* arguments);
    void Unload();

private:
    bool Start();
    bool TryStart();
    void Stop();
    void TryStop();

private:
    enum class State
    {
        Idle,
        PendingStop,
        PendingStart,
    };

    State        m_state{};
    HMODULE      m_library{};
    CClientBase* m_client{};
    std::string  m_arguments;
};
