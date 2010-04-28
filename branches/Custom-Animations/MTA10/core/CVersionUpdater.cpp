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

#define VERSION_CHECKER_URL1 "http://vers-m2-kows.yomg.net/mta/"

#define MAX_UPDATER_CHECK_ATTEMPTS      (3)
#define MAX_UPDATER_DOWNLOAD_ATTEMPTS   (10)
#define UPDATER_CHECK_TIMEOUT           (15000)
#define UPDATER_DOWNLOAD_TIMEOUT        (30000)


CVersionUpdater::CVersionUpdater ( void )
    : m_llTimeStart ( 0 )
{
    m_strStage = "Start";
    m_CheckInfo.serverList.push_back ( VERSION_CHECKER_URL1 );
}


void CVersionUpdater::DoPulse ( void )
{
    if ( m_strStage == "" )
        return;

    if ( m_strStage == "Start" )
    {
        unsigned int uiTimeNow = static_cast < unsigned int > ( time ( NULL ) );
        unsigned int uiLastCheckTime = 0;
        CVARS_GET ( "last_version_check2", uiLastCheckTime );

        // Only check once a day
        if ( uiTimeNow - uiLastCheckTime > 60 * 60 * 24 )
        {
            //CVARS_SET ( "last_version_check2", uiTimeNow );
            m_strStage = "NextCheck";
        }
        else
            m_strStage = "End";
    }


    if ( m_strStage == "NextCheck" )
    {
        StartVersionCheck ();
    }

    if ( m_strStage == "Checking" )
    {
        PollVersionCheck ();
    }

    if ( m_strStage == "StartUpdateQuestion" )
    {
        StartUpdateQuestion ();
    }

    if ( m_strStage == "PollUpdateQuestion" )
    {
        PollUpdateQuestion ();
    }

    if ( m_strStage == "NextMirror" )
    {
        StartDownload ();
    }

    if ( m_strStage == "Downloading" )
    {
        PollDownload ();
    }

    if ( m_strStage == "PollPostDownloadError" )
    {
        PollPostDownloadError ();
    }

    if ( m_strStage == "PollPostDownloadOk" )
    {
        PollPostDownloadOk ();
    }

    if ( m_strStage == "End" )
    {
        m_strStage = "";
    }
}


//
// Couple 'o shortcuts
//
void CVersionUpdater::ResetCheckTimer ( void )
{
    unsigned int uiTimeNow = static_cast < unsigned int > ( time ( NULL ) );
    CVARS_SET ( "last_version_check2", uiTimeNow );
}

bool CVersionUpdater::IsDownloadMandatory ( void )
{
    return m_DownloadInfo.strPriority == "mandatory";
}

CQuestionBox& CVersionUpdater::GetQuestionBox ( void )
{
    return *CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//
// Version check
//
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CVersionUpdater::StartVersionCheck ( void )
{
    if ( m_CheckInfo.iRetryCount++ > MAX_UPDATER_CHECK_ATTEMPTS || m_CheckInfo.serverList.size () == 0 )
    {
        m_strStage = "End";
        return;
    }

    m_llTimeStart = GetTickCount64_ ();

    // Get URL of next server in the list
    m_CheckInfo.iCurrent++;
    m_CheckInfo.iCurrent = m_CheckInfo.iCurrent % m_CheckInfo.serverList.size ();
    SString strServerURL = m_CheckInfo.serverList[ m_CheckInfo.iCurrent ];

    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

    // Make the query URL
    SString strQueryURL ( "%s?v=%s&id=%s", strServerURL.c_str (), MTA_DM_BUILDTAG_LONG, szSerial );

    // Perform the HTTP request
    m_strStage = "Checking";
    m_HTTP.Get ( strQueryURL );
}


void CVersionUpdater::PollVersionCheck ( void )
{
    CHTTPBuffer buffer;
    if ( !m_HTTP.GetData ( buffer ) )
    {
        // Not done yet
        // Give up if error or timeout
        unsigned int status = m_HTTP.GetStatus ();
        if ( ( status !=0 && status != 200 ) || GetTickCount64_ () - m_llTimeStart > UPDATER_CHECK_TIMEOUT )
            m_strStage = "NextCheck";
        return;
    }

    // Got something
    char* pData = buffer.GetData ();
    if ( pData )
    {
        // Try to parse download info
        m_DownloadInfo = CDownloadInfo ();

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
                if ( strPartList[0] == "sha1" )         m_DownloadInfo.strSHA1 = strPartList[1];
                if ( strPartList[0] == "end" )          m_DownloadInfo.strEnd = strPartList[1];
            }
        }

        // Check if full message gotten
        if ( m_DownloadInfo.strEnd == "ok" )
        {
            // Reset check timer if update is not mandatory
            if ( IsDownloadMandatory () == false )
                ResetCheckTimer ();

            // Finish here if there is no update available
            if ( m_DownloadInfo.strStatus != "update" )
            {
                m_strStage = "End";
                return;
            }

            // Ask user what to do
            m_strStage = "StartUpdateQuestion";
            return;
        }
    }

    m_strStage = "NextCheck";
    return;
}


void CVersionUpdater::StartUpdateQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( m_DownloadInfo.strTitle );
    GetQuestionBox ().SetMessage ( m_DownloadInfo.strMsg );
    GetQuestionBox ().SetButton ( 0, m_DownloadInfo.strNo );
    GetQuestionBox ().SetButton ( 1, m_DownloadInfo.strYes );
    GetQuestionBox ().Show ();
    m_strStage = "PollUpdateQuestion";
}


void CVersionUpdater::PollUpdateQuestion ( void )
{
    unsigned int uiButton = GetQuestionBox ().PollButtons ();

    if ( uiButton == -1 )
        return;

    GetQuestionBox ().Reset ();

    if ( uiButton == 0 )
    {
        // No
        if ( IsDownloadMandatory () )
        {
            // If mandantory, 'no' quits the game
            CCore::GetSingleton ().Quit ();
        }
        else
        {
            // If optional, just end the updater
            m_strStage = "End";
        }
    }
    else
    {
        // Yes
        m_strStage = "NextMirror";
        // Start first download attempt at a random mirror
        m_DownloadInfo.iCurrent = ( rand () / 11 ) % 100;
    }
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//
// Download
//
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CVersionUpdater::StartDownload ( void )
{
    if ( m_DownloadInfo.iRetryCount++ > MAX_UPDATER_DOWNLOAD_ATTEMPTS || m_DownloadInfo.serverList.size () == 0 )
    {
        m_strStage = "End";
        return;
    }

    m_llTimeStart = GetTickCount64_ ();

    // Get URL of next server in the list
    m_DownloadInfo.iCurrent++;
    m_DownloadInfo.iCurrent = m_DownloadInfo.iCurrent % m_DownloadInfo.serverList.size ();
    SString strURL = m_DownloadInfo.serverList[ m_DownloadInfo.iCurrent ];

    // Perform the HTTP request
    m_strStage = "Downloading";
    m_HTTP.Get ( strURL );

    // Display dialog
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "DOWNLOADING" );
    GetQuestionBox ().SetMessage ( "waiting..." );
    GetQuestionBox ().SetButton ( 0, "Cancel" );
    GetQuestionBox ().Show ();
}


void CVersionUpdater::PollDownload ( void )
{
    // Poll cancel button
    if ( GetQuestionBox ().PollButtons () == 0 )
    {
        m_HTTP.Get ("");
        GetQuestionBox ().Reset ();
        m_strStage = "StartUpdateQuestion";
        return;
    }

    // Update progress
    unsigned int uiBytesDownloaded = m_HTTP.PeekNextDataSize ();
    if ( m_DownloadInfo.uiBytesDownloaded != uiBytesDownloaded )
    {
        m_llTimeStart = GetTickCount64_ ();
        m_DownloadInfo.uiBytesDownloaded = uiBytesDownloaded;
        GetQuestionBox ().SetMessage ( SString ( "%3d %% completed", m_DownloadInfo.uiBytesDownloaded * 100 / Max < unsigned int > ( 1, m_DownloadInfo.uiFilesize ) ) );
    }

    // Are we done yet?
    CHTTPBuffer buffer;
    if ( !m_HTTP.GetData ( buffer ) )
    {
        unsigned int status = m_HTTP.GetStatus ();
        // Give up if error or timeout
        if ( ( status !=0 && status != 200 ) || GetTickCount64_ () - m_llTimeStart > UPDATER_DOWNLOAD_TIMEOUT )
            m_strStage = "NextMirror";
        return;
    }

    // Got something
    GetQuestionBox ().Reset ();

    char* pData = buffer.GetData ();
    if ( pData )
    {

        // Check SHA1
        unsigned int uiSize = buffer.GetSize ();
        bool bHashOk = true;
        // CheckDatumHash( pData, uiSize, m_DownloadInfo.strSHA1 );
        if ( !bHashOk )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "The file had an accident on the infomation superhighway" );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            m_strStage = "PollPostDownloadError";
            return;
        }

        // Save file somewhere
        // Make a list of possible places to save the file
        std::vector < SString > saveLocationList;
        char szTempPath[ MAX_PATH ] = "";
        GetTempPath( MAX_PATH - 30, szTempPath );
        saveLocationList.push_back ( std::string( szTempPath ) + m_DownloadInfo.strFilename );
        saveLocationList.push_back ( CalcMTASAPath ( std::string ( "\\mta\\" ) + m_DownloadInfo.strFilename ) );

        // Try each place
        SString strPathFilename;
        for ( std::vector < SString > ::iterator iter = saveLocationList.begin () ; iter != saveLocationList.end () ; ++iter )
        {
            bool bSaveOk = false;
            FILE* fh = fopen ( *iter, "wb" );
            if ( fh )
            {
                bSaveOk = ( fwrite ( pData, 1, uiSize, fh ) == uiSize );
                fclose ( fh );
            }
            if ( bSaveOk )
            {
                strPathFilename = *iter;
                break;
            }
        }

        if ( strPathFilename.length () == 0 )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR SAVING" );
            GetQuestionBox ().SetMessage ( "The file wants to stay in ram" );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            m_strStage = "PollPostDownloadError";
            return;
        }

        // Set it to autorun when the game exits
        SetOnQuitCommand ( "open", strPathFilename );

        // Exit game after button press
        GetQuestionBox ().Reset ();
        GetQuestionBox ().SetTitle ( "DOWNLOAD COMPLETE" );
        GetQuestionBox ().SetMessage ( m_DownloadInfo.strMsg2 );
        GetQuestionBox ().SetButton ( 0, "OK" );
        GetQuestionBox ().Show ();
        m_strStage = "PollPostDownloadOk";
        return;
    }
    m_strStage = "NextMirror";
}


void CVersionUpdater::PollPostDownloadError ( void )
{
    // Wait for button press before continuing
    if ( GetQuestionBox ().PollButtons () != -1 )
    {
        m_strStage = "StartUpdateQuestion";
    }
}

void CVersionUpdater::PollPostDownloadOk ( void )
{
    // Wait for button press before continuing
    if ( GetQuestionBox ().PollButtons () != -1 )
    {
        // Exit game
        m_strStage = "End";
        CCore::GetSingleton ().Quit ();
    }
}