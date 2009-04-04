/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CScreenShot.cpp
*  PURPOSE:     Screen capture file handling
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

static char szScreenShotPath[MAX_PATH] = {0};
static bool bIsChatVisible = false;
static bool bIsDebugVisible = false;

SString CScreenShot::PreScreenShot ()
{
    int iNumberOfFiles = 1;

	bIsChatVisible = g_pCore->IsChatVisible ();
	bIsDebugVisible = g_pCore->IsDebugVisible ();

	// make the chat and debug windows invisible
	g_pCore->SetChatVisible ( false );
	g_pCore->SetDebugVisible ( false );

    SString strScreenShotName ( "%s\\mta-screen%04d.png", &szScreenShotPath[0], iNumberOfFiles );

    OFSTRUCT ReOpenBuff;
    HFILE hFile = OpenFile ( strScreenShotName, &ReOpenBuff, OF_CANCEL );    

    while ( hFile != HFILE_ERROR )
    {
        CloseHandle( (HANDLE)hFile );
        iNumberOfFiles++;
        strScreenShotName = SString ( "%s\\mta-screen%04d.png", &szScreenShotPath[0], iNumberOfFiles );
        hFile = OpenFile ( strScreenShotName, &ReOpenBuff, OF_CANCEL );
    }

    return strScreenShotName;
}

void CScreenShot::PostScreenShot ( const char *szFileName )
{
	// print a notice
    g_pCore->GetConsole()->Printf ( "Screenshot taken: '%s'", szFileName );

	// make the chat and debug windows visible again
	g_pCore->SetChatVisible ( bIsChatVisible );
	g_pCore->SetDebugVisible ( bIsDebugVisible );
}

void CScreenShot::SetPath ( const char *szPath )
{
	strncpy ( szScreenShotPath, szPath, MAX_PATH - 1 );

    // make sure the directory exists
    mkdir ( szPath );
}

int CScreenShot::GetScreenShots ( void )
{
    int iNumberOfFiles = 1;

	bIsChatVisible = g_pCore->IsChatVisible ();
	bIsDebugVisible = g_pCore->IsDebugVisible ();

	// make the chat and debug windows invisible
	g_pCore->SetChatVisible ( false );
	g_pCore->SetDebugVisible ( false );

    SString strScreenShotName ( "%s\\mta-screen%04d.png", &szScreenShotPath[0], iNumberOfFiles );

    OFSTRUCT ReOpenBuff;
    HFILE hFile = OpenFile ( strScreenShotName, &ReOpenBuff, OF_CANCEL );    

    while ( hFile != HFILE_ERROR )
    {
        CloseHandle( (HANDLE)hFile );
        iNumberOfFiles++;
        strScreenShotName.Format ( "%s\\mta-screen%04d.png", &szScreenShotPath[0], iNumberOfFiles );
        hFile = OpenFile ( strScreenShotName, &ReOpenBuff, OF_CANCEL );
	}

	return iNumberOfFiles;
}

SString CScreenShot::GetScreenShotPath ( int iNumber )
{
	// Get a random number
	return SString ( "%s\\mta-screen%04d.png", &szScreenShotPath[0], iNumber );
}
