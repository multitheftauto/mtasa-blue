/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CModManagerImpl.h
 *  PURPOSE:     Mod manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CModManagerImpl;

#pragma once

#include <core/CModManager.h>
#include <core/CServerBase.h>

#include "CDynamicLibrary.h"
#include "CServerImpl.h"

typedef CServerBase*(InitServer)();

class CModManagerImpl : public CModManager
{
public:
    CModManagerImpl(CServerImpl* pServer);
    virtual ~CModManagerImpl();

    void SetServerPath(const char* szServerPath) { m_strServerPath = szServerPath; };

    bool RequestLoad(const char* szModName);

    const char*     GetServerPath() { return m_strServerPath; };
    const char*     GetModPath() { return m_strModPath; };
    virtual SString GetAbsolutePath(const char* szRelative);

    bool         IsModLoaded();
    CServerBase* GetCurrentMod();

    bool Load(const char* szModName, int iArgumentCount, char* szArguments[]);
    void Unload(bool bKeyPressBeforeTerm = false);

    void HandleInput(const char* szCommand);
    void GetTag(char* szInfoTag, int iInfoTag);

    void DoPulse();

    bool IsReadyToAcceptConnections() const noexcept;

    bool IsFinished();

    bool             PendingWorkToDo();
    bool             GetSleepIntervals(int& iSleepBusyMs, int& iSleepIdleMs, int& iLogicFpsLimit);
    CDynamicLibrary& GetDynamicLibrary() { return m_Library; };

    void SetExitCode(int exitCode) override;
    int  GetExitCode() const override;

private:
    CServerImpl* m_pServer;

    CServerBase*    m_pBase;
    CDynamicLibrary m_Library;
    SString         m_strServerPath;            // Root path
    SString         m_strModPath;               // root/mods/deathmatch
};
