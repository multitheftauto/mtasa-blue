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
#include <windows.h>

#ifdef MTA_DEBUG
    #define CMODMANAGER_CLIENTDLL "client_d.dll"
#else
    #define CMODMANAGER_CLIENTDLL "client.dll"
#endif

class CModManager : public CModManagerInterface, public CSingleton<CModManager>
{
public:
    CModManager();
    ~CModManager();

    void RequestLoad(const char* szModName, const char* szArguments);
    void RequestLoadDefault(const char* szArguments);
    void RequestUnload();
    void ClearRequest();

    bool IsLoaded();

    CClientBase* Load(const char* szName, const char* szArguments);
    void         Unload();

    void DoPulsePreFrame();
    void DoPulsePreHUDRender(bool bDidUnminimize, bool bDidRecreateRenderTargets);
    void DoPulsePostFrame();

    CClientBase* GetCurrentMod();

    void RefreshMods();

    bool TriggerCommand(const char* commandName, size_t commandNameLength, const void* userdata, size_t userdataSize) const override;

private:
    void InitializeModList(const char* szModFolderPath);
    void Clear();

    void VerifyAndAddEntry(const char* szModFolderPath, const char* szName);

    std::map<std::string, std::string> m_ModDLLFiles;
    HMODULE                            m_hClientDLL;
    CClientBase*                       m_pClientBase;

    SString m_strDefaultModName;

    bool    m_bUnloadRequested;
    SString m_strRequestedMod;
    SString m_strRequestedModArguments;
};
