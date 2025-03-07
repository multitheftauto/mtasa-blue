/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CServer.h
 *  PURPOSE:     Header for server class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CServer final
{
public:
    ~CServer() { Stop(); }

    bool Start(const char* configFileName);

    void Stop(bool graceful = true);

    void Pulse();

    bool IsRunning() const noexcept { return m_isRunning; }

    bool IsReady() const noexcept { return m_isRunning && m_isAcceptingConnections; }

    void SetPassword(const char* password) { m_password = password; }

    const std::string& GetPassword() const noexcept { return m_password; }

public:
    bool PrintServerOutputToConsole();

private:
    bool        m_isRunning{};
    bool        m_isAcceptingConnections{};
    HANDLE      m_job{};
    HANDLE      m_readyEvent{};
    HANDLE      m_process{};
    DWORD       m_processId{};
    HANDLE      m_stdout{};
    HANDLE      m_stdin{};
    std::string m_password;
};
