/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/CInstallManager.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CSequencer.h"

// Common command line keys
#define INSTALL_STAGE       "install_stage"
#define INSTALL_LOCATION    "install_loc"
#define HIDE_PROGRESS       "hide_prog"

typedef CBadLang<class CInstallManager> CSequencerType;

class CInstallManager
{
public:
    void    SetMTASAPathSource(const SString& strCommandLineIn);
    void    InitSequencer();
    SString Continue(/*const SString& strCommandLine*/);
    void    RestoreSequencerFromSnapshot(const SString& strText);
    SString GetSequencerSnapshot();
    SString GetLauncherPathFilename();
    bool    UpdateOptimusSymbolExport();

protected:
    SString _ShowCrashFailDialog();
    SString _CheckOnRestartCommand();
    SString _MaybeSwitchToTempExe();
    SString _SwitchBackFromTempExe();
    SString _InstallFiles();
    SString _ChangeToAdmin();
    SString _ShowCopyFailDialog();
    SString _PrepareLaunchLocation();
    SString _ProcessGtaPatchCheck();
    SString _ProcessGtaDllCheck();
    SString _ProcessGtaVersionCheck();
    SString _ProcessLayoutChecks();
    SString _ProcessLangFileChecks();
    SString _ProcessServiceChecks();
    SString _ProcessAppCompatChecks();
    SString _ChangeFromAdmin();
    SString _InstallNewsItems();
    SString _Quit();

    CSequencerType* m_pSequencer;
    SString         m_strAdminReason;
};

CInstallManager* GetInstallManager();
