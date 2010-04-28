/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVersionUpdateChecker.cpp
*  PURPOSE:     Version update check and message dialog class
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define VERSION_CHECK_URL "http://wiki.multitheftauto.com/wiki/Current_Version&action=render"
#define VERSION_CHECK2_URL "http://vers-m1.kows.yomg.net/mta/"


CVersionUpdateChecker::CVersionUpdateChecker ( void )
    : m_iStage ( 0 )
    , m_llTimeStart ( 0 )
{
}


void CVersionUpdateChecker::DoPulse ( void )
{
    if ( m_iStage == 0 )
    {
        unsigned int uiTimeNow = static_cast < unsigned int > ( time ( NULL ) );
        unsigned int uiLastCheckTime = 0;
        CVARS_GET ( "last_version_check", uiLastCheckTime );

        // Only check once a day
        if ( uiTimeNow - uiLastCheckTime > 60 * 60 * 24 )
        {
            CVARS_SET ( "last_version_check", uiTimeNow );
            StartVersionCheck2 ();
            m_iStage = 1;
        }
        else
            m_iStage = -1;
    }

    if ( m_iStage == 1 )
    {
        PollVersionCheck2 ();
    }

    if ( m_iStage == 2 )
    {
        StartVersionCheck ();
        m_iStage = 3;
    }

    if ( m_iStage == 3 )
    {
        PollVersionCheck ();
    }
}



void CVersionUpdateChecker::StartVersionCheck ( void )
{
    m_llTimeStart = GetTickCount64_ ();

    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

    // Make the URL
    SString strURL ( VERSION_CHECK_URL "&ver=%s&id=%s",
                     MTA_DM_BUILDTAG_LONG,
                     szSerial );

    // Perform the HTTP request
    m_HTTP.Get ( strURL );
}


void CVersionUpdateChecker::PollVersionCheck ( void )
{
    CHTTPBuffer buffer;
    if ( !m_HTTP.GetData ( buffer ) )
    {
        // No reply yet
        // Give up after 5 seconds
        if ( GetTickCount64_ () - m_llTimeStart > 5000 )
            m_iStage++;
        return;
    }

    // Got something
    m_iStage++;
    char* ptr = buffer.GetData ();


    // Parse the data - Look for x.x.x or x.x
    int values[3] = { 0, 0, 0 };
    int iCount = 0;
    do
    {
        ptr = strpbrk( ptr, "0123456789" );
        if ( ptr )
            iCount = sscanf ( ptr, "%d.%d.%d.", &values[0], &values[1], &values[2] );
    }
    while ( ptr++ && iCount < 2 );

    if ( iCount >= 2 )
    {
        // Make a version number of the lastest avalible version
        unsigned short usRemoteVersion = ( values[0] << 8 ) + ( values[1] << 4 ) + values[2];

        // Make a version number of the version here. (Retard if this version is not a release version)
        unsigned short usLocalVersion = strcmp ( MTA_DM_BUILDTYPE, "Release" ) == 0 ? MTA_DM_VERSION : MTA_DM_VERSION - 1;

        if ( usRemoteVersion > usLocalVersion )
        {
            CCore::GetSingleton ().ShowMessageBox ( "Good News", "A new version of MTA is available!\n\nPlease download the latest version from www.mtasa.com", MB_BUTTON_OK | MB_ICON_INFO );
            m_iStage = -1;  // Success. Update message. End check.
        }
        else
        {
            m_iStage = -1;  // Success. No update. End check.
        }
    }
}


void CVersionUpdateChecker::StartVersionCheck2 ( void )
{
    m_llTimeStart = GetTickCount64_ ();

    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

    // Make the URL
    SString strURL ( VERSION_CHECK2_URL "%s/?id=%s",
                     MTA_DM_BUILDTAG_LONG,
                     szSerial );

    // Perform the HTTP request
    m_HTTP.Get ( strURL );
}


void CVersionUpdateChecker::PollVersionCheck2 ( void )
{
    CHTTPBuffer buffer;
    if ( !m_HTTP.GetData ( buffer ) )
    {
        // No reply yet
        // Give up after 5 seconds
        if ( m_HTTP.GetStatus () !=0 || GetTickCount64_ () - m_llTimeStart > 5000 )
            m_iStage++;
        return;
    }

    // Got something
    m_iStage++;
    char* ptr = buffer.GetData ();

    // Look for: msg=title=text=
    if ( ptr )
    {
        std::vector < SString > strPartList;
        SString ( std::string( ptr ) ).Split ( "=", strPartList );
        if ( strPartList.size () > 0 )
        {
            if ( strPartList[0] == "nomsg" )
            {
                m_iStage = -1;  // Success. No update. End check.
            }
            else
            if ( strPartList.size () >= 3 )
            {
                if ( strPartList[0] == "msg" )
                {
                    CCore::GetSingleton ().ShowMessageBox ( strPartList[1], strPartList[2], MB_BUTTON_OK | MB_ICON_INFO );
                    m_iStage = -1;  // Success. Update message. End check.
               }
            }
        }
    }
}

