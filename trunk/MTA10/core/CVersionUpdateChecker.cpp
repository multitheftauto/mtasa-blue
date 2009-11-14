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


CVersionUpdateChecker::CVersionUpdateChecker ( void )
    : m_iStage ( 0 )
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
            StartVersionCheck ();
            m_iStage = 1;
        }
        else
            m_iStage = 2;
    }

    if ( m_iStage == 1 )
    {
        // Attempt to get the HTTP data
        CHTTPBuffer buffer;
        if ( m_HTTP.GetData ( buffer ) )
        {
            m_iStage = 2;

            // Parse the data - Look for x.x.x or x.x
            int values[3] = { 0, 0, 0 };
            int iCount = 0;

            char* ptr = buffer.GetData ();
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
                }
            }
            else
            {
                CLogger::GetSingleton ().ErrorPrintf ( "Could not parse version check response." );
            }
        }
    }
}


void CVersionUpdateChecker::StartVersionCheck ( void )
{
    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

    // Make the URL
    SString strURL ( VERSION_CHECK_URL "&ver=%s&id=%s",
                     MTA_DM_VERSIONSTRING,
                     szSerial );

    // Perform the HTTP request
    if ( !m_HTTP.Get ( strURL ) )
    {
        CLogger::GetSingleton ().ErrorPrintf ( "Could not perform version check. (%s)", strURL.c_str () );
    }
}

