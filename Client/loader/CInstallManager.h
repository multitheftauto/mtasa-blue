/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

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
    SString MaybeRenameExe(const SString& strGTAPath);

protected:
    SString _ShowCrashFailDialog();
    SString _CheckOnRestartCommand();
    SString _MaybeSwitchToTempExe();
    SString _SwitchBackFromTempExe();
    SString _InstallFiles();
    SString _ChangeToAdmin();
    SString _ShowCopyFailDialog();
    SString _ProcessGtaVersionCheck();
    SString _ProcessLayoutChecks();
    SString _ProcessLangFileChecks();
    SString _ProcessExePatchChecks();
    SString _ProcessServiceChecks();
    SString _ProcessAppCompatChecks();
    SString _ChangeFromAdmin();
    SString _InstallNewsItems();
    SString _Quit();

    CSequencerType* m_pSequencer;
    SString         m_strAdminReason;
};

CInstallManager* GetInstallManager();
