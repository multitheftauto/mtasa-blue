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
#include <libpng/png.h>

extern CCore* g_pCore;

static char szScreenShotPath[MAX_PATH] = {0};
static bool bIsChatVisible = false;
static bool bIsDebugVisible = false;

// Variables used for saving the screen shot file on a separate thread
static bool     ms_bIsSaving        = false;
static uint     ms_uiWidth          = 0;
static uint     ms_uiHeight         = 0;
static void*    ms_pData            = NULL;
static uint     ms_uiDataSize       = 0;
static bool     ms_bHideChatBox     = false;        // TODO - Make setting
static SString  ms_strFileName;


SString CScreenShot::PreScreenShot ()
{
    bIsChatVisible = g_pCore->IsChatVisible ();
    bIsDebugVisible = g_pCore->IsDebugVisible ();

    // make the chat and debug windows invisible
    if ( ms_bHideChatBox )
    {
        g_pCore->SetChatVisible ( false );
        g_pCore->SetDebugVisible ( false );
    }

    SString strScreenShotName = GetValidScreenshotFilename();

    return strScreenShotName;
}

void CScreenShot::PostScreenShot ( const SString& strFileName )
{
    // print a notice
    if ( !strFileName.empty () )
        g_pCore->GetConsole()->Printf ( _("Screenshot taken: '%s'"), *strFileName );

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
    return SString("%s\\mta-screen_%d-%02d-%02d_%02d-%02d-%02d.png" ,&szScreenShotPath[0],sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
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


// Callback for threaded save
// Static function
DWORD CScreenShot::ThreadProc ( LPVOID lpdwThreadParam )
{
    unsigned long ulScreenHeight = ms_uiHeight;
    unsigned long ulScreenWidth = ms_uiWidth;
    uint uiReqDataSize = ulScreenHeight * ulScreenWidth * 4;
    uint uiLinePitch = ulScreenWidth * 4;

    if ( uiReqDataSize != ms_uiDataSize )
    {
        ms_bIsSaving = false;
        return 0;
    }

    // Create the screen data buffer
    BYTE** ppScreenData = NULL;
    ppScreenData = new BYTE* [ ulScreenHeight ];
    for ( unsigned short y = 0; y < ulScreenHeight; y++ ) {
        ppScreenData[y] = new BYTE [ ulScreenWidth * 4 ];
    }

    // Copy the surface data into a row-based buffer for libpng
    #define BYTESPERPIXEL 4
    unsigned long ulLineWidth = ulScreenWidth * 4;
    for ( unsigned int i = 0; i < ulScreenHeight; i++ ) {
        memcpy ( ppScreenData[i], (BYTE*) ms_pData + i* uiLinePitch, ulLineWidth );
        for ( unsigned int j = 3; j < ulLineWidth; j += BYTESPERPIXEL ) {
            ppScreenData[i][j] = 0xFF;
        }
    }

    FILE *file = fopen (ms_strFileName, "wb");
        png_struct* png_ptr = png_create_write_struct ( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
        png_info* info_ptr = png_create_info_struct ( png_ptr );
        png_init_io ( png_ptr, file );
        png_set_filter ( png_ptr, 0, PNG_FILTER_NONE );
        png_set_compression_level ( png_ptr, 1 );
        png_set_IHDR ( png_ptr, info_ptr, ulScreenWidth, ulScreenHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
        png_set_rows ( png_ptr, info_ptr, ppScreenData );
        png_write_png ( png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_STRIP_ALPHA, NULL );
        png_write_end ( png_ptr, info_ptr );
        png_destroy_write_struct ( &png_ptr, &info_ptr );
    fclose(file);

    // Clean up the screen data buffer
    if ( ppScreenData ) {
        for ( unsigned short y = 0; y < ulScreenHeight; y++ ) {
            delete [] ppScreenData[y];
        }
        delete [] ppScreenData;
    }

    ms_bIsSaving = false;
    return 0;
}

// Static function
void CScreenShot::BeginSave ( const char *szFileName, void* pData, uint uiDataSize, uint uiWidth, uint uiHeight )
{
    if ( ms_bIsSaving )
        return;

    ms_strFileName = szFileName;
    ms_pData = pData;
    ms_uiDataSize = uiDataSize;
    ms_uiWidth = uiWidth;
    ms_uiHeight = uiHeight;

    HANDLE hThread = CreateThread ( NULL, 0, (LPTHREAD_START_ROUTINE)CScreenShot::ThreadProc, NULL, CREATE_SUSPENDED, NULL );
    if ( !hThread )
    {
        CCore::GetSingleton ().GetConsole ()->Printf ( "Could not create screenshot thread." );
    }
    else
    {
        ms_bIsSaving = true;
        SetThreadPriority ( hThread, THREAD_PRIORITY_LOWEST );
        ResumeThread ( hThread );
    }
}

// Static function
bool CScreenShot::IsSaving ( void )
{
    return ms_bIsSaving;
}
