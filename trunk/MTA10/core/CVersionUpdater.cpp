/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVersionUpdater.cpp
*  PURPOSE:     Version update check and message dialog class
*  DEVELOPERS:  You're only supposed to blow the bloody doors off.
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <utils/CMD5Hasher.h>

// Do a static page check first
#define VERSION_CHECKER_URL1 "http://vers-m3-kows.yomg.net/mta/%VERSION%/?v=%VERSION%&id=%ID%&st=%STATUS%&ty=%TYPE%&re=%REFER%"
#define VERSION_CHECKER_URL2 "http://vers-m3-kows.yomg.net/mta/?v=%VERSION%&id=%ID%&st=%STATUS%&ty=%TYPE%&re=%REFER%"

#define DATA_FILES_URL1 "http://vers-m3-kows.yomg.net/mtadf/%VERSION%/?v=%VERSION%&id=%ID%&st=%STATUS%"
#define DATA_FILES_URL2 "http://vers-m3-kows.yomg.net/mtadf/?v=%VERSION%&id=%ID%&st=%STATUS%"

#define REPORT_LOG_URL1 "http://vers-m3-kows.yomg.net/mtarep/?v=%VERSION%&id=%ID%&st=%STATUS%"


#define MAX_UPDATER_CHECK_ATTEMPTS      (3)
#define MAX_UPDATER_DOWNLOAD_ATTEMPTS   (10)
#define UPDATER_CHECK_TIMEOUT           (15000)
#define UPDATER_DOWNLOAD_TIMEOUT        (30000)

/*
Example of a response file stored on the server:
------------------------------------------------

status=update=
priority=notmandatory=
title=UPDATE AVAILABLE=
msg=There is an update for the version of MTA you are using.

DO YOU WANT TO UPDATE NOW?

=
yes=Yes=
no=No=
msg2=Press OK to install the update.

After the update is installed, restart MTA=
filename=mtasa-1.0.4-untested-28-20100509.exe=
filesize=2185798=
mirror=http://nightly.multitheftauto.com/mtasa-1.0.4-untested-28-20100509.exe=
mirror=http://nightly2.multitheftauto.com/mtasa-1.0.4-untested-28-20100509.exe=
mirror=http://nightly3.multitheftauto.com/mtasa-1.0.4-untested-28-20100509.exe=
md5=12977BDDCCCE7171898F999773823CD5=
end=ok
*/

//
// Note: Due to a bug, pre 1.0.4 1983 clients cannot download from http servers that send no data with the http header
//
// To help get round this:
//  * Use 'mirror2' for http servers that send no data with the http header (e.g. Apache/2.2.9)
//  * Use 'mirror' for http servers that send data with the http header (e.g. lighttpd, Apache/2)
//  * Always use 'mirror' at least once to prevent locking out older clients
//

//
// Added two new statuses for 1981: 'files' and 'silent'
//


class CVersionUpdater;

namespace
{

    enum
    {
        RES_FAIL,
        RES_OK,
        RES_POLL,
        RES_CANCEL,
    };

    enum
    {
        BUTTON_NONE = -1,
        BUTTON_0    = 0,
        BUTTON_1,
        BUTTON_2,
        BUTTON_3,
    };

    CQuestionBox& GetQuestionBox ( void )
    {
        return *CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
    }

    struct CCheckInfo
    {
                                CCheckInfo() : iCurrent ( -1 ), iRetryCount ( 0 ) {}
        int                     iCurrent;
        int                     iRetryCount;
        std::vector < SString > serverList;
    };

    struct CDownloadInfo
    {
                                CDownloadInfo() : iCurrent ( -1 ), iRetryCount ( 0 ), uiFilesize ( 0 ), uiBytesDownloaded ( 0 ) {}
        int                     iCurrent;
        int                     iRetryCount;
        SString                 strStatus;
        SString                 strTitle;
        SString                 strMsg;
        SString                 strMsg2;
        SString                 strYes;
        SString                 strNo;
        SString                 strPriority;
        SString                 strFilename;
        unsigned int            uiFilesize;
        std::vector < SString > serverList;
        SString                 strMD5;
        SString                 strEnd;
        unsigned int            uiBytesDownloaded;
        SString                 strSaveLocation;
    };

    struct CStringPair
    {
        SString strValue1;
        SString strValue2;
    };

    typedef void (CVersionUpdater::*PFNVOIDVOID) ( void );


    //
    // One update instruction
    //
    struct CInstruction
    {
        SString strLabel;
        SString strCondition;
        SString strGoto;
        PFNVOIDVOID pfnCmdFunc;

        CInstruction ( PFNVOIDVOID pfnCmdFunc )
        {
            this->pfnCmdFunc = pfnCmdFunc;
        }
        CInstruction ( const SString& strText )
        {
            this->pfnCmdFunc = NULL;
            std::vector < SString > parts;
            strText.Split ( " ", parts );
            if ( parts.size () == 1 )
                strLabel = parts[0].TrimEnd ( ":" );    // Label
            else
            if ( parts.size () == 4 )
            {
                strCondition = parts[1];                // Conditional goto
                strGoto = parts[3];
            }
        }
        bool IsLabel() const            { return strLabel.length () != 0; }
        bool IsConditionalGoto() const  { return strCondition.length () != 0; }
        bool IsFunction() const         { return pfnCmdFunc != NULL; }
    };


    //
    // Many update instructions
    //
    struct CProgram
    {
        unsigned int pc;
        std::vector < CInstruction >    instructions;

        CProgram() : pc ( 0 ) {}
        bool IsValid () const
        {
            return ( pc >= 0 && pc < instructions.size () );
        }
        const CInstruction* GetNextInstruction()
        {
            if ( IsValid () )
                return &instructions[ pc++ ];
            return NULL;
        }
        void GotoLabel ( const SString& strLabel )
        {
            pc = FindLabel ( strLabel );
        }
        int FindLabel( const SString& strLabel ) const
        {
            for ( unsigned int i = 0 ; i < instructions.size () ; i++ )
                if ( instructions[i].IsLabel () && instructions[i].strLabel == strLabel )
                    return i;
            return -1;
        }
    };


    #define Push(F) \
        m_Stack.push_back ( &CVersionUpdater::F )

    #define ADDINST( expr ) \
        program.instructions.push_back ( CInstruction ( &CVersionUpdater::expr ) );
    #define ADDCOND( condition ) \
        program.instructions.push_back ( CInstruction ( condition ) );
    #define ADDLABL( label ) \
        program.instructions.push_back ( CInstruction ( label ) );
}


///////////////////////////////////////////////////////////////
//
//
// CVersionUpdater
//
//
///////////////////////////////////////////////////////////////
class CVersionUpdater : public CVersionUpdaterInterface
{
public:
    // CVersionUpdaterInterface interface
                        CVersionUpdater                     ( void );
    virtual             ~CVersionUpdater                    ( void );
    virtual void        DoPulse                             ( void );
    virtual void        InitiateUpdate                      ( const SString& strType, const SString& strHost );
    virtual bool        IsOptionalUpdateInfoRequired        ( const SString& strHost );
    virtual void        InitiateDataFilesFix                ( void );

    // CVersionUpdater functions

    // Command lists
    void                InitPrograms                        ( void );
    void                CheckPrograms                       ( void );
    void                RunProgram                          ( const SString& strProgramName );

    // Util
    bool                IsBusy                              ( void );
    void                SetCondition                        ( const SString& strType, const SString& strValue1, const SString& strValue2 = "" );
    bool                IsConditionTrue                     ( const SString& strCondition );
    void                ResetEverything                     ( void );

    // Commands
    void                _UseVersionCheckURLs                ( void );
    void                _UseDataFilesURLs                   ( void );
    void                _UseReportLogURLs                   ( void );
    void                _ActionReconnect                    ( void );
    void                _DialogHide                         ( void );
    void                _End                                ( void );
    void                _ExitGame                           ( void );
    void                _ResetCheckTimer                    ( void );
    void                _PollQuestionNoYes                  ( void );
    void                _PollAnyButton                      ( void );
    void                _DialogConnectingWait               ( void );
    void                _DialogChecking                     ( void );
    void                _DialogDownloading                  ( void );
    void                _DialogServerSaysUpdateQuestion     ( void );
    void                _DialogServerSaysRecommendQuestion  ( void );
    void                _DialogUpdateQuestion               ( void );
    void                _DialogUpdateQueryError             ( void );
    void                _DialogUpdateResult                 ( void );
    void                _QUpdateResult                      ( void );
    void                _DialogDataFilesQuestion            ( void );
    void                _DialogDataFilesQueryError          ( void );
    void                _DialogDataFilesResult              ( void );
    void                _StartFileQuery                     ( void );
    void                _PollFileQuery                      ( void );
    void                _StartFileDownload                  ( void );
    void                _PollFileDownload                   ( void );
    void                _StartSendPost                      ( void );
    void                _PollSendPost                       ( void );

    // Doers
    int                 DoSendQueryToNextServer             ( void );
    int                 DoPollQueryResponse                 ( void );
    int                 DoSendDownloadRequestToNextServer   ( void );
    int                 DoPollDownload                      ( void );
    int                 DoSendPostToNextServer              ( void );
    int                 DoPollPost                          ( void );

    std::vector < PFNVOIDVOID >         m_Stack;
    CDownloadInfo                       m_DownloadInfo;
    std::map < SString, CProgram >      m_ProgramMap;
    CProgram                            m_CurrentProgram;
    CCheckInfo                          m_QueryInfo;
    CHTTPClient                         m_HTTP;
    long long                           m_llTimeStart;
    std::map < SString, CStringPair >   m_ConditionMap;
    SString                             m_strServerSaysType;
    SString                             m_strServerSaysHost;

    bool                                m_bCheckedTimeForPeriodicCheck;
    bool                                m_bSentReportLog;
    std::map < SString, int >           m_DoneOptionalMap;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CVersionUpdater* g_pVersionUpdater = NULL;

CVersionUpdaterInterface* GetVersionUpdater ()
{
    if ( !g_pVersionUpdater )
        g_pVersionUpdater = new CVersionUpdater ();
    return g_pVersionUpdater;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::CVersionUpdater
//
//
//
///////////////////////////////////////////////////////////////
CVersionUpdater::CVersionUpdater ( void )
{
    m_llTimeStart = 0;
    m_bCheckedTimeForPeriodicCheck = false;
    m_bSentReportLog = false;
    InitPrograms ();
    CheckPrograms ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::CVersionUpdater
//
//
//
///////////////////////////////////////////////////////////////
CVersionUpdater::~CVersionUpdater ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::DoPulse ( void )
{
    //
    // Check time for periodic check?
    //
    if ( !m_bCheckedTimeForPeriodicCheck && !IsBusy () )
    {
        m_bCheckedTimeForPeriodicCheck = true;

        unsigned int uiTimeNow = static_cast < unsigned int > ( time ( NULL ) );
        unsigned int uiLastCheckTime = 0;
        CVARS_GET ( "last_version_check", uiLastCheckTime );

        // Only check once a day
        if ( uiTimeNow - uiLastCheckTime > 60 * 60 * 24 || uiTimeNow < uiLastCheckTime )
        {
            RunProgram ( "PeriodicCheck" );
        }
    }

    //
    // Should send report on how new features are functioning?
    //
    if ( !m_bSentReportLog && !IsBusy () )
    {
        m_bSentReportLog = true;
        RunProgram ( "SendReportLog" );
    }

    //
    // Step through update sequence
    //
    if ( m_Stack.size () )
    {
        // Call top function on stack
        PFNVOIDVOID pfnCmdFunc = m_Stack.back ();
        m_Stack.pop_back ();
        ( this->*pfnCmdFunc ) ();
    }
    else
    {
        // If no top function on stack, process next instruction
        if ( const CInstruction* pInstruction = m_CurrentProgram.GetNextInstruction () )
        {
            if ( pInstruction->IsFunction () )
                m_Stack.push_back ( pInstruction->pfnCmdFunc );
            else
            if ( pInstruction->IsConditionalGoto () )
                if ( IsConditionTrue ( pInstruction->strCondition ) )
                    m_CurrentProgram.GotoLabel ( pInstruction->strGoto );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateUpdate
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateUpdate ( const SString& strType, const SString& strHost )
{
    if ( strType == "Mandatory" )
    {
        CCore::GetSingleton ().RemoveMessageBox ();
        RunProgram ( "ServerSaysUpdate" );
    }
    else
    if ( strType == "Optional" || strType == "Recommended" )
    {
        CCore::GetSingleton ().RemoveMessageBox ();
        MapSet ( m_DoneOptionalMap, strHost, 1 );
        RunProgram ( "ServerSaysRecommend" );
    }

    m_strServerSaysType = strType;
    m_strServerSaysHost = strHost;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::IsOptionalUpdateInfoRequired
//
//
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::IsOptionalUpdateInfoRequired ( const SString& strHost )
{
    // Should this server tell us about recommended updates?
    return !MapContains ( m_DoneOptionalMap, strHost );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateDataFilesFix
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateDataFilesFix ( void )
{
    RunProgram ( "ServerSaysDataFilesWrong" );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::ResetEverything
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::ResetEverything ()
{
    m_Stack.clear ();
    m_DownloadInfo = CDownloadInfo();
    m_CurrentProgram = CProgram();
    m_QueryInfo = CCheckInfo ();
    m_HTTP.Get ("");
    m_llTimeStart = 0;
    m_ConditionMap.clear ();
    m_strServerSaysType = "";
    m_strServerSaysHost = "";

    GetQuestionBox ().Reset ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::IsBusy
//
//
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::IsBusy ( void )
{
    return m_Stack.size () || ( m_CurrentProgram.IsValid () );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::SetCondition
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::SetCondition ( const SString& strType, const SString& strValue1, const SString& strValue2 )
{
    CStringPair pair;
    pair.strValue1 = strValue1.ToLower ();
    pair.strValue2 = strValue2.ToLower ();
    MapSet ( m_ConditionMap, strType.ToLower (), pair );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::IsConditionTrue
//
//
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::IsConditionTrue ( const SString& strCondition )
{
    if ( strCondition.length () == 0 )
        return true;

    bool bResult = false;

    SString strType, strTemp, strValue1, strValue2;
    strCondition.ToLower ().Split( ".", &strType, &strTemp );
    strTemp.Split( ".", &strValue1, &strValue2 );

    bool bInvert = strValue1[0] == '!';
    if ( bInvert )
        strValue1 = strValue1.substr ( 1 );

    if ( CStringPair *pPair = MapFind ( m_ConditionMap, strType ) )
    {
        bResult = ( !strValue1.size () || pPair->strValue1 == strValue1 )
               && ( !strValue2.size () || pPair->strValue2 == strValue2 );
    }

    if ( bInvert )
        bResult = !bResult;

    return bResult;
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
// Run'
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitPrograms
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitPrograms ()
{
    //
    // PeriodicCheck
    //
    {
        SString strProgramName = "PeriodicCheck";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseVersionCheckURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _StartFileQuery );                          // Fetch update info from update server
        ADDCOND ( "if QueryResponse.update goto dload");        // If update server says 'update' then goto dload:
        ADDCOND ( "if QueryResponse.files goto dload");         // If update server says 'files' then goto dload:
        ADDCOND ( "if QueryResponse.silent goto silentdload");  // If update server says 'silent' then goto silentdload:
        ADDCOND ( "if QueryResponse.noupdate goto noupdate");   // If update server says 'noupdate' then goto noupdate:
        ADDINST (   _End );

        ADDLABL ( "dload:" );
        ADDINST (   _DialogUpdateQuestion );                    // Show "Update available" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto end" );        // If user says 'No', then goto end:
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _StartFileDownload );                       // Fetch update binary from update mirror
        ADDINST (   _DialogUpdateResult );                      // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "silentdload:" );
        ADDINST (   _StartFileDownload );                       // Fetch update binary from update mirror
        ADDINST (   _QUpdateResult );                           // Maybe set OnRestartCommand
        ADDINST (   _End );

        ADDLABL ( "noupdate:" );
        ADDINST (   _ResetCheckTimer );                         // Wait 24hrs before checking again
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }

    //
    // ServerSaysUpdate
    //
    {
        SString strProgramName = "ServerSaysUpdate";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseVersionCheckURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _DialogServerSaysUpdateQuestion );          // Show "Server says update" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto end" );        // If user says 'No', then goto end:
        ADDINST (   _DialogChecking );                          // Show "Checking..." message
        ADDINST (   _StartFileQuery );                          // Fetch update info from update server
        ADDCOND ( "if QueryResponse.update goto dload");        // If update server says 'update' then goto dload:
        ADDCOND ( "if QueryResponse.files goto dload");         // If update server says 'files' then goto dload:
        ADDCOND ( "if QueryResponse.silent goto dload");        // If update server says 'silent' then goto dload:
        ADDINST (   _DialogUpdateQueryError );
        ADDINST (   _End );

        ADDLABL ( "dload:" );
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _StartFileDownload );                       // Fetch update binary from update mirror
        ADDINST (   _DialogUpdateResult );                      // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }

    //
    // ServerSaysRecommend
    //
    {
        SString strProgramName = "ServerSaysRecommend";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseVersionCheckURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _DialogConnectingWait );                    // Show "Please wait..." message
        ADDINST (   _StartFileQuery );                          // Fetch update info from update server
        ADDCOND ( "if QueryResponse.update goto dload");        // If update server says 'update' then goto dload:
        ADDCOND ( "if QueryResponse.files goto dload");         // If update server says 'files' then goto dload:
        ADDCOND ( "if QueryResponse.silent goto silentdload");  // If update server says 'silent' then goto silentdload:
        ADDINST (   _ActionReconnect );
        ADDINST (   _End );

        ADDLABL ( "dload:" );
        ADDINST (   _DialogServerSaysRecommendQuestion );       // Show "Server says update" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto reconnect" );  // If user says 'No', then goto reconnect:
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _StartFileDownload );                       // Fetch update binary from update mirror
        ADDINST (   _DialogUpdateResult );                      // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "silentdload:" );
        ADDINST (   _DialogHide );                              // Don't show downloading progress
        ADDINST (   _ActionReconnect );                         // Reconnect to game
        ADDINST (   _StartFileDownload );                       // Fetch update binary from update mirror
        ADDINST (   _QUpdateResult );                           // Maybe set OnRestartCommand
        ADDINST (   _End );

        ADDLABL ( "reconnect:" );
        ADDINST (   _ActionReconnect );
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }


    //
    // ServerSaysDataFilesWrong
    //
    {
        SString strProgramName = "ServerSaysDataFilesWrong";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseDataFilesURLs );                        // Use DATA_FILES_URL*
        ADDINST (   _DialogDataFilesQuestion );                 // Show "Data files wrong" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto end" );        // If user says 'No', then goto end:
        ADDINST (   _DialogChecking );                          // Show "Checking..." message
        ADDINST (   _StartFileQuery );                          // Fetch file info from server
        ADDCOND ( "if QueryResponse.!datafiles goto error1" );  // If server says 'No files' then goto error1:
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _StartFileDownload );                       // Fetch file binary from mirror
        ADDINST (   _DialogDataFilesResult );                   // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "error1:" );
        ADDINST (   _DialogUpdateQueryError );
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (     _End );
    }


    //
    // SendReportLog
    //
    {
        SString strProgramName = "SendReportLog";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseReportLogURLs );                        // Use REPORT_LOG_URL*
        ADDINST (   _StartSendPost );                           // Send data
        ADDLABL ( "end:" );
        ADDINST (     _End );
    }

}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::CheckPrograms
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::CheckPrograms ()
{
    // Check each program
    for ( std::map < SString, CProgram >::iterator it = m_ProgramMap.begin (); it != m_ProgramMap.end (); ++it )
    {
        CProgram program = it->second;

        // Check each instruction
        while ( const CInstruction* pInstruction = program.GetNextInstruction () )
        {
            assert ( pInstruction->IsFunction () || pInstruction->IsConditionalGoto () || pInstruction->IsLabel () );

            if ( pInstruction->IsConditionalGoto () )
                assert ( program.FindLabel ( pInstruction->strGoto ) != -1 );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::RunProgram
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::RunProgram ( const SString& strProgramName )
{
    ResetEverything ();
    CProgram* pProgram = MapFind ( m_ProgramMap, strProgramName );
    if ( pProgram )
    {
        m_CurrentProgram = *pProgram;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
// '_'
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseVersionCheckURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseVersionCheckURLs ( void )
{
    m_QueryInfo = CCheckInfo ();
    m_QueryInfo.serverList.push_back ( VERSION_CHECKER_URL1 );
    m_QueryInfo.serverList.push_back ( VERSION_CHECKER_URL2 );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseDataFilesURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseDataFilesURLs ( void )
{
    m_QueryInfo = CCheckInfo ();
    m_QueryInfo.serverList.push_back ( DATA_FILES_URL1 );
    m_QueryInfo.serverList.push_back ( DATA_FILES_URL2 );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseReportLogURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseReportLogURLs ( void )
{
    m_QueryInfo = CCheckInfo ();
    m_QueryInfo.serverList.push_back ( REPORT_LOG_URL1 );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ActionReconnect
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ActionReconnect ( void )
{
    CCommandFuncs::Reconnect ( NULL );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogHide
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogHide()
{
    GetQuestionBox ().Reset ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_End
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_End ( void )
{
    ResetEverything ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ExitGame
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ExitGame ( void )
{
    // Exit game
    CCore::GetSingleton ().Quit ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetCheckTimer ( void )
{
    unsigned int uiTimeNow = static_cast < unsigned int > ( time ( NULL ) );
    CVARS_SET ( "last_version_check", uiTimeNow );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollAnyButton
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollAnyButton ( void )
{
    // Wait for button press before continuing
    if ( GetQuestionBox ().PollButtons () == -1 )
        Push ( _PollAnyButton );
    else
        GetQuestionBox ().Reset ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollQuestionNoYes
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollQuestionNoYes ( void )
{
    switch ( GetQuestionBox ().PollButtons () )
    {
        case BUTTON_NONE:
            Push ( _PollQuestionNoYes );
            break;

        case BUTTON_0:
            SetCondition ( "QuestionResponse", "No" );
            GetQuestionBox ().Reset ();
            break;

        case BUTTON_1:
            SetCondition ( "QuestionResponse", "Yes" );
            GetQuestionBox ().Reset ();
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogConnectingWait
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogConnectingWait ( void )
{
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "Connecting" );
    GetQuestionBox ().SetMessage ( "Please wait..." );
    GetQuestionBox ().SetButton ( 0, "Cancel" );
    GetQuestionBox ().Show ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogChecking
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogChecking ( void )
{
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "CHECKING" );
    GetQuestionBox ().SetMessage ( "Please wait..." );
    GetQuestionBox ().SetButton ( 0, "Cancel" );
    GetQuestionBox ().Show ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDownloading
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDownloading ( void )
{
    // Display dialog
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "DOWNLOADING" );
    GetQuestionBox ().SetMessage ( "waiting..." );
    GetQuestionBox ().SetButton ( 0, "Cancel" );
    GetQuestionBox ().Show ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogServerSaysUpdateQuestion
//
// ServerSaysUpdateQuestion
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogServerSaysUpdateQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "MANDATORY UPDATE" );
    GetQuestionBox ().SetMessage ( "To join this server, you must update MTA.\n\n Do you want to update now ?" );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogServerSaysRecommendQuestion
//
// ServerSaysRecommendQuestion
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogServerSaysRecommendQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "OPTIONAL UPDATE" );
    GetQuestionBox ().SetMessage ( "Server says an update is recommended, but not essential.\n\n Do you want to update now ?" );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateQuestion
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( m_DownloadInfo.strTitle );
    GetQuestionBox ().SetMessage ( m_DownloadInfo.strMsg );
    GetQuestionBox ().SetButton ( 0, m_DownloadInfo.strNo );
    GetQuestionBox ().SetButton ( 1, m_DownloadInfo.strYes );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateQueryError
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateQueryError ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "UPDATE CHECK" );
    GetQuestionBox ().SetMessage ( "Update not currently avalable.\n\nPlease check www.mtasa.com" );
    GetQuestionBox ().SetButton ( 0, "OK" );
    GetQuestionBox ().Show ();
    Push ( _PollAnyButton );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateResult(void)
{
    if ( IsConditionTrue ( "Download.Fail" ) )
    {
        // Handle failure
        if ( IsConditionTrue ( "Download.Fail.Saving" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR SAVING" );
            GetQuestionBox ().SetMessage ( "Unable to create the file." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        if ( IsConditionTrue ( "Download.Fail.Checksum" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "The downloaded file appears to be incorrect." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "For some reason." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
    }
    else
    if ( IsConditionTrue ( "Download.Ok" ) )
    {
        // Handle success
        // Exit game after button press
        GetQuestionBox ().Reset ();
        GetQuestionBox ().SetTitle ( "DOWNLOAD COMPLETE" );
        GetQuestionBox ().SetMessage ( m_DownloadInfo.strMsg2 );
        GetQuestionBox ().SetButton ( 0, "OK" );
        GetQuestionBox ().Show ();

        if ( m_DownloadInfo.strStatus == "update" )
        {
            // 'update' - Stand alone installer
            SetOnQuitCommand ( "open", m_DownloadInfo.strSaveLocation );
            SetOnRestartCommand ( "" );
            Push ( _ExitGame );
        }
        else
        if ( m_DownloadInfo.strStatus == "files" || m_DownloadInfo.strStatus == "silent" )
        {
            // 'files'/'silent' - Self extracting archive 
            SetOnQuitCommand ( "restart" );
            SetOnRestartCommand ( "files", m_DownloadInfo.strSaveLocation );
            Push ( _ExitGame );
        }
        else
        {
            GetQuestionBox ().SetMessage ( m_DownloadInfo.strStatus + " - Unknown problem in _DialogUpdateResult" );
        }

        Push ( _PollAnyButton );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_QUpdateResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_QUpdateResult ( void )
{
    if ( IsConditionTrue ( "Download.Ok" ) )
    {
        // Handle success
        if ( m_DownloadInfo.strStatus == "silent" )
        {
            // 'silent' - Self extracting archive 
            SetOnRestartCommand ( "silent", m_DownloadInfo.strSaveLocation );
        }
#if MTA_DEBUG
        else
            MessageBox ( NULL, "_QUpdateResult m_DownloadInfo.strStatus != 'silent'", m_DownloadInfo.strStatus, MB_OK );
#endif
    }
#if MTA_DEBUG
    else
        MessageBox ( NULL, "_QUpdateResult Download.!Ok", "Error", MB_OK );
#endif
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDataFilesQuestion
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDataFilesQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "ERROR" );
    GetQuestionBox ().SetMessage ( "San Andreas data files have been modified\n\n\nDo you want to automatically fix this problem?" );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDataFilesQueryError
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDataFilesQueryError ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "DATA FILES" );
    GetQuestionBox ().SetMessage ( "Data files not currently avalable.\n\nPlease check www.mtasa.com" );
    GetQuestionBox ().SetButton ( 0, "OK" );
    GetQuestionBox ().Show ();
    Push ( _PollAnyButton );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDataFilesResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDataFilesResult ( void )
{
    // Handle failure
    if ( IsConditionTrue ( "Download.Fail" ) )
    {
        if ( IsConditionTrue ( "Download.Fail.Saving" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR SAVING" );
            GetQuestionBox ().SetMessage ( "Unable to create the file." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        if ( IsConditionTrue ( "Download.Fail.Checksum" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "The downloaded file appears to be incorrect." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "For some reason." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
    }
    else
    if ( IsConditionTrue ( "Download.Ok" ) )
    {
        // Handle success
        SetOnQuitCommand ( "open", m_DownloadInfo.strSaveLocation );

        // Exit game after button press
        GetQuestionBox ().Reset ();
        GetQuestionBox ().SetTitle ( "DOWNLOAD COMPLETE" );
        GetQuestionBox ().SetMessage ( m_DownloadInfo.strMsg2 );
        GetQuestionBox ().SetButton ( 0, "OK" );
        GetQuestionBox ().Show ();
        Push ( _ExitGame );
        Push ( _PollAnyButton );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_StartFileQuery
//
// VersionCheck
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_StartFileQuery(void)
{
    switch ( DoSendQueryToNextServer () )
    {
        case RES_FAIL:
            // Can't find any(more) servers to send a query to
            // Drop back to previous stack level
            break;

        case RES_OK:
            // Query sent ok, now wait for response
            Push ( _PollFileQuery );
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollFileQuery
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollFileQuery ( void )
{
    switch ( DoPollQueryResponse () )
    {
        case RES_FAIL:
            // Connection to current server failed, try next server
            Push ( _StartFileQuery );
            break;

        case RES_OK:
            // Got a valid response
            // Drop back to previous stack level for processing
            break;

        case RES_POLL:
            // Waiting...
            // Check if cancel button pressed
            if ( GetQuestionBox ().PollButtons () == -1 )
                Push ( _PollFileQuery );
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_StartFileDownload
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_StartFileDownload ( void )
{
    switch ( DoSendDownloadRequestToNextServer () )
    {
        case RES_FAIL:
            // Can't find any(more) servers to send a query to
            // Drop back to previous stack level
            break;

        case RES_OK:
            // Query sent ok, now wait for response
            Push ( _PollFileDownload );
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollFileDownload
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollFileDownload ( void )
{
    switch ( DoPollDownload () )
    {
        case RES_FAIL:
            // Connection to current server failed, try next server
            Push ( _StartFileDownload );
            break;

        case RES_OK:
            // Got a valid response
            // Drop back to previous stack level for processing
            break;

        case RES_POLL:
            // Waiting...

            if ( GetQuestionBox ().IsVisible () )
            {
                // Handle progress/cancel if visible
                if ( GetQuestionBox ().PollButtons () == 0 )
                {
                    m_HTTP.Get ("");
                    GetQuestionBox ().Reset ();
                    return;
                }
                GetQuestionBox ().SetMessage ( SString ( "%3d %% completed", m_DownloadInfo.uiBytesDownloaded * 100 / Max < unsigned int > ( 1, m_DownloadInfo.uiFilesize ) ) );
            }

            Push ( _PollFileDownload );
            break;

        default:
            assert ( 0 );
    }
}



///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_StartSendPost
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_StartSendPost ( void )
{
    switch ( DoSendPostToNextServer () )
    {
        case RES_FAIL:
            // Can't find any(more) servers to send a query to
            // Drop back to previous stack level
            break;

        case RES_OK:
            // Query sent ok, now wait for response
            Push ( _PollSendPost );
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollSendPost
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollSendPost ( void )
{
    switch ( DoPollPost () )
    {
        case RES_FAIL:
            // Connection to current server failed, try next server
            Push ( _StartSendPost );
            break;

        case RES_OK:
            // Got a valid response
            // Drop back to previous stack level for processing
            break;

        case RES_POLL:

            // Waiting...
            Push ( _PollSendPost );
            break;

        default:
            assert ( 0 );
    }
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
// 'Do'
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoSendQueryToNextServer
//
// RES_FAIL - Can't find a valid server to query
// RES_OK   - Query sent
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoSendQueryToNextServer ( void )
{
    if ( ++m_QueryInfo.iRetryCount > MAX_UPDATER_CHECK_ATTEMPTS || m_QueryInfo.serverList.size () == 0 )
    {
        return RES_FAIL;
    }

    m_llTimeStart = GetTickCount64_ ();

    // Get URL of next server in the list
    m_QueryInfo.iCurrent++;
    m_QueryInfo.iCurrent = m_QueryInfo.iCurrent % m_QueryInfo.serverList.size ();
    SString strServerURL = m_QueryInfo.serverList[ m_QueryInfo.iCurrent ];

    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );
    char szStatus [ 128 ];
    CCore::GetSingleton ().GetNetwork ()->GetStatus ( szStatus, sizeof ( szStatus ) );

    // Compose version string
    unsigned short usNetRev = CCore::GetSingleton ().GetNetwork ()->GetNetRev ();
    SString strPlayerVersion ( "%d.%d.%d-%s.%05d.%d"
                                ,MTASA_VERSION_MAJOR
                                ,MTASA_VERSION_MINOR
                                ,MTASA_VERSION_MAINTENANCE
                                ,MTA_DM_BUILDTYPE
                                ,MTASA_VERSION_BUILD
                                ,usNetRev
                                );

    // Make the query URL
    SString strQueryURL = strServerURL;
    strQueryURL = strQueryURL.Replace ( "%VERSION%", strPlayerVersion );
    strQueryURL = strQueryURL.Replace ( "%ID%", szSerial );
    strQueryURL = strQueryURL.Replace ( "%STATUS%", szStatus );
    strQueryURL = strQueryURL.Replace ( "%TYPE%", m_strServerSaysType );
    strQueryURL = strQueryURL.Replace ( "%REFER%", m_strServerSaysHost );

    // Perform the HTTP request
    m_HTTP.Get ( strQueryURL );
    return RES_OK;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoPollQueryResponse
//
// RES_FAIL  - Query failed
// RES_OK    - Got valid response
// RES_POLL  - Processing
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoPollQueryResponse ( void )
{
    CHTTPBuffer buffer;
    if ( !m_HTTP.GetData ( buffer ) )
    {
        // Not done yet
        // Give up if error or timeout
        unsigned int status = m_HTTP.GetStatus ();
        if ( ( status !=0 && status != 200 ) || GetTickCount64_ () - m_llTimeStart > UPDATER_CHECK_TIMEOUT )
        {
            // If 404 error, remove this server from the list
            if ( status == 404 && m_QueryInfo.serverList.size () )
                ListRemove( m_QueryInfo.serverList, m_QueryInfo.serverList[ m_QueryInfo.iCurrent % m_QueryInfo.serverList.size () ] );

            return RES_FAIL;
        }
        return RES_POLL;
    }

    // Got something
    char* pData = buffer.GetData ();
    if ( pData )
    {
        // Try to parse download info
        m_DownloadInfo = CDownloadInfo();

        // Make all line endings '\n'
        SString strInput = SString ( std::string( pData ) ).ConformLineEndings();

        // Break into lines
        std::vector < SString > strLineList;
        strInput.Split ( "=\n", strLineList );
 
        // Parse each line
        std::vector < SString > ::iterator iter = strLineList.begin ();
        for ( ; iter != strLineList.end () ; iter++ )
        {
            std::vector < SString > strPartList;
            (*iter).Split ( "=", strPartList, 2 );
            if ( strPartList.size () == 2 )
            {
                if ( strPartList[0] == "status" )       m_DownloadInfo.strStatus = strPartList[1];
                if ( strPartList[0] == "title" )        m_DownloadInfo.strTitle = strPartList[1];
                if ( strPartList[0] == "msg" )          m_DownloadInfo.strMsg = strPartList[1];
                if ( strPartList[0] == "msg2" )         m_DownloadInfo.strMsg2 = strPartList[1];
                if ( strPartList[0] == "yes" )          m_DownloadInfo.strYes = strPartList[1];
                if ( strPartList[0] == "no" )           m_DownloadInfo.strNo = strPartList[1];
                if ( strPartList[0] == "priority" )     m_DownloadInfo.strPriority = strPartList[1];
                if ( strPartList[0] == "filename" )     m_DownloadInfo.strFilename = strPartList[1];
                if ( strPartList[0] == "filesize" )     m_DownloadInfo.uiFilesize = atoi ( strPartList[1] );
                if ( strPartList[0] == "mirror" )       m_DownloadInfo.serverList.push_back ( strPartList[1] );
                if ( strPartList[0] == "mirror2" )      m_DownloadInfo.serverList.push_back ( strPartList[1] );
                if ( strPartList[0] == "md5" )          m_DownloadInfo.strMD5 = strPartList[1];
                if ( strPartList[0] == "end" )          m_DownloadInfo.strEnd = strPartList[1];
            }
        }

        // Check if full message gotten
        if ( m_DownloadInfo.strEnd == "ok" )
        {
            SetCondition ( "QueryResponse", m_DownloadInfo.strStatus );
            return RES_OK;
        }
    }

    return RES_FAIL;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoSendDownloadRequestToNextServer
//
// RES_FAIL - Can't find a valid server to query
// RES_OK   - Query sent
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoSendDownloadRequestToNextServer ( void )
{
    if ( ++m_DownloadInfo.iRetryCount > MAX_UPDATER_DOWNLOAD_ATTEMPTS || m_DownloadInfo.serverList.size () == 0 )
    {
        return RES_FAIL;
    }

    m_llTimeStart = GetTickCount64_ ();

    // Get URL of next server in the list
    m_DownloadInfo.iCurrent++;
    m_DownloadInfo.iCurrent = m_DownloadInfo.iCurrent % m_DownloadInfo.serverList.size ();
    SString strURL = m_DownloadInfo.serverList[ m_DownloadInfo.iCurrent ];

    // Perform the HTTP request
    m_HTTP.Get ( strURL );
    return RES_OK;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoPollDownload
//
// RES_FAIL   - Download failed
// RES_OK     - Got valid file
// RES_POLL   - Processing
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoPollDownload ( void )
{
    SetCondition ( "Download", "" );

    // Update progress
    unsigned int uiBytesDownloaded = m_HTTP.PeekNextDataSize ();
    if ( m_DownloadInfo.uiBytesDownloaded != uiBytesDownloaded )
    {
        m_llTimeStart = GetTickCount64_ ();
        m_DownloadInfo.uiBytesDownloaded = uiBytesDownloaded;
    }

    // Are we done yet?
    CHTTPBuffer buffer;
    if ( !m_HTTP.GetData ( buffer ) )
    {
        unsigned int status = m_HTTP.GetStatus ();
        // Give up if error or timeout
        if ( ( status !=0 && status != 200 ) || GetTickCount64_ () - m_llTimeStart > UPDATER_DOWNLOAD_TIMEOUT )
        {
            // If 404 error, remove this server from the list
            if ( status == 404 && m_DownloadInfo.serverList.size () )
                ListRemove( m_DownloadInfo.serverList, m_DownloadInfo.serverList[ m_DownloadInfo.iCurrent % m_DownloadInfo.serverList.size () ] );

            SetCondition ( "Download", "Fail" );
            AddReportLog ( SString ( "DoPollDownload: Regular fail for %s (status:%u  time:%u)", m_DownloadInfo.strFilename.c_str(), status, GetTickCount64_ () - m_llTimeStart ) );
            return RES_FAIL;
        }
        return RES_POLL;
    }

    // Got something
    char* pData = buffer.GetData ();
    if ( pData )
    {
        unsigned int uiSize = buffer.GetSize ();

        // Check MD5
        if ( m_DownloadInfo.strMD5 != "none" )
        {
            // Hash data
            MD5 md5Result;
            CMD5Hasher().Calculate ( pData, uiSize, md5Result );
            char szMD5[33];
            CMD5Hasher::ConvertToHex ( md5Result, szMD5 );

            if ( m_DownloadInfo.strMD5 != szMD5 )
            {
                // If MD5 error, remove this server from the list
                if ( m_DownloadInfo.serverList.size () )
                    ListRemove( m_DownloadInfo.serverList, m_DownloadInfo.serverList[ m_DownloadInfo.iCurrent % m_DownloadInfo.serverList.size () ] );
                SetCondition ( "Download", "Fail", "Checksum" );
                AddReportLog ( SString ( "DoPollDownload: Checksum wrong for %s (%s %s)", m_DownloadInfo.strFilename.c_str(), m_DownloadInfo.strMD5.c_str(), szMD5 ) );
                return RES_FAIL;
            }
        }

        // Save file
        SString strPathFilename = CalcMTASAPath ( std::string ( "\\mta\\upcache\\" ) + m_DownloadInfo.strFilename );
        MakeSureDirExists ( strPathFilename );
        if ( !FileSave ( strPathFilename, pData, uiSize ) )
        {
            if ( m_DownloadInfo.serverList.size () )
                ListRemove( m_DownloadInfo.serverList, m_DownloadInfo.serverList[ m_DownloadInfo.iCurrent % m_DownloadInfo.serverList.size () ] );
            SetCondition ( "Download", "Fail", "Saving" );
            AddReportLog ( SString ( "DoPollDownload: Unable to save the file %s (size %d)", strPathFilename.c_str(), uiSize ) );
            return RES_FAIL;
        }

        m_DownloadInfo.strSaveLocation = strPathFilename;
        SetCondition ( "Download", "Ok" );
        AddReportLog ( SString ( "DoPollDownload: Downloaded %s", m_DownloadInfo.strFilename.c_str() ) );
        return RES_OK;
    }

    SetCondition ( "Download", "Fail" );
    AddReportLog ( SString ( "DoPollDownload: Fail for %s", m_DownloadInfo.strFilename.c_str() ) );
    return RES_FAIL;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoSendPostToNextServer
//
// RES_FAIL - Can't find a valid server to query
// RES_OK   - Query sent
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoSendPostToNextServer ( void )
{
    if ( ++m_QueryInfo.iRetryCount > 1 || m_QueryInfo.serverList.size () == 0 )
    {
        return RES_FAIL;
    }

    m_llTimeStart = GetTickCount64_ ();

    // Get URL of next server in the list
    m_QueryInfo.iCurrent++;
    m_QueryInfo.iCurrent = m_QueryInfo.iCurrent % m_QueryInfo.serverList.size ();
    SString strServerURL = m_QueryInfo.serverList[ m_QueryInfo.iCurrent ];

    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );
    char szStatus [ 128 ];
    CCore::GetSingleton ().GetNetwork ()->GetStatus ( szStatus, sizeof ( szStatus ) );

    // Compose version string
    unsigned short usNetRev = CCore::GetSingleton ().GetNetwork ()->GetNetRev ();
    SString strPlayerVersion ( "%d.%d.%d-%s.%05d.%d"
                                ,MTASA_VERSION_MAJOR
                                ,MTASA_VERSION_MINOR
                                ,MTASA_VERSION_MAINTENANCE
                                ,MTA_DM_BUILDTYPE
                                ,MTASA_VERSION_BUILD
                                ,usNetRev
                                );

    // Make the query URL
    SString strQueryURL = strServerURL;
    strQueryURL = strQueryURL.Replace ( "%VERSION%", strPlayerVersion );
    strQueryURL = strQueryURL.Replace ( "%ID%", szSerial );
    strQueryURL = strQueryURL.Replace ( "%STATUS%", szStatus );

    //
    // Get post contents
    //
    SString strReportFilename = CalcMTASAPath ( std::string ( "\\mta\\report.log" )  );
    std::vector < char > buffer;
    FileLoad ( strReportFilename, buffer );

    //
    // Send data. Don't bother checking if it was received, as it's not important.
    //
    if ( buffer.size () > 10 )
    {
        buffer.push_back ( 0 );
        SString strContent = &buffer[0];
        if ( strContent.length () > 5000 )
            strContent = strContent.substr ( strContent.length () - 5000 );

        CNetHTTPDownloadManagerInterface * downloadManager = CCore::GetSingleton ().GetNetwork ()->GetHTTPDownloadManager ();
        downloadManager->QueueFile ( strQueryURL, NULL, 0, &strContent.at ( 0 ) );
        if ( !downloadManager->IsDownloading () )
            downloadManager->StartDownloadingQueuedFiles ();

        FileSave ( strReportFilename, NULL, 0 );
    }

    return RES_OK;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoPollPost
//
// RES_FAIL   - Post failed
// RES_OK     - Post ok
// RES_POLL   - Processing
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoPollPost ( void )
{
    CNetHTTPDownloadManagerInterface* pHTTP = CCore::GetSingleton ().GetNetwork ()->GetHTTPDownloadManager ();
    if ( pHTTP )
    {
        if ( !pHTTP->ProcessQueuedFiles () )
        {
            return RES_POLL;
        }
    }
    return RES_OK;
}
