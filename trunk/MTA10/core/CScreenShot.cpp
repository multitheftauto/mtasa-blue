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

    bIsChatVisible = g_pCore->IsChatVisible ();
    bIsDebugVisible = g_pCore->IsDebugVisible ();

    // make the chat and debug windows invisible
    g_pCore->SetChatVisible ( false );
    g_pCore->SetDebugVisible ( false );

    SString strScreenShotName = GetValidScreenshotFilename();

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
    int iNumberOfFiles = 0;
    HANDLE hFind;
    WIN32_FIND_DATA fdFindData;
    //Create a search string
    SString strScreenShotName ( "%s\\mta-screen*.png", &szScreenShotPath[0] );
    // Find the first match
    hFind = FindFirstFile(strScreenShotName, &fdFindData); 
    // Check if the first match failed
    if ( hFind != INVALID_HANDLE_VALUE) {
        iNumberOfFiles++;
        //Loop through and count the files
        while (FindNextFile(hFind, &fdFindData)) { 
            //Keep going until we find the last file
            iNumberOfFiles++;
        }
    }
    //Close the file handle
    FindClose(hFind);
    return iNumberOfFiles;
}

SString CScreenShot::GetValidScreenshotFilename ( void )
{
    //Get the system time
    SYSTEMTIME sysTime;
    GetLocalTime( &sysTime );
    return SString("%s\\mta-screen %d-%02d-%02d %02d-%02d-%02d.png" ,&szScreenShotPath[0],sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
}

SString CScreenShot::GetScreenShotPath ( int iNumber )
{
    //Create a search string
    SString strScreenShotName ( "%s\\mta-screen*.png", &szScreenShotPath[0] );
    HANDLE hFind;
    SString strReturn = "";
    WIN32_FIND_DATA fdFindData;
    int i = 1;
    //Find the first match
    hFind = FindFirstFile(strScreenShotName, &fdFindData);
    //Check if the first match failed
    if ( hFind != INVALID_HANDLE_VALUE) {
        if (iNumber == 1) {
            //We wanted the first file
            strReturn = SString("%s\\%s",&szScreenShotPath[0],fdFindData.cFileName);
        }
        else
        {
            //Loop through and find all occurences of the file
            while (FindNextFile(hFind, &fdFindData)) { 
                //Keep going until we find the last file
                i++;
                if (iNumber == i) {
                    strReturn = SString("%s\\%s",&szScreenShotPath[0], fdFindData.cFileName);
                    break;
                }
            }
        }
    }
    FindClose(hFind); //Close the file handle
    // Get Return the file directory
    return strReturn;
}
