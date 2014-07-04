/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CSequencer.h"

// Common command line keys
#define INSTALL_STAGE       "install_stage"
#define INSTALL_LOCATION    "install_loc"
#define SILENT_OPT          "silent_opt"

typedef CBadLang < class CInstallManager > CSequencerType;

class CInstallManager
{
public:
    void            InitSequencer                   ( void );
    SString         Continue                        ( const SString& strCommandLine );
    void            RestoreSequencerFromSnapshot    ( const SString& strText );
    SString         GetSequencerSnapshot            ( void );
    SString         GetLauncherPathFilename         ( void );

protected:
    SString         _ShowCrashFailDialog            ( void );
    SString         _CheckOnRestartCommand          ( void );
    SString         _MaybeSwitchToTempExe           ( void );
    SString         _SwitchBackFromTempExe          ( void );
    SString         _InstallFiles                   ( void );
    SString         _ChangeToAdmin                  ( void );
    SString         _ShowCopyFailDialog             ( void );
    SString         _ProcessLayoutChecks            ( void );
    SString         _ProcessAeroChecks              ( void );
    SString         _ChangeFromAdmin                ( void );
    SString         _InstallNewsItems               ( void );
    SString         _Quit                           ( void );

    CSequencerType*     m_pSequencer;
    SString             m_strAdminReason;
};

CInstallManager* GetInstallManager ( void );
