/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CScreenShot.cpp
 *  PURPOSE:     Screen capturing
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <libpng/png.h>

extern CCore* g_pCore;

// If screenshot should be taken on current frame
static bool ms_bScreenShot = false;

static bool ms_bIsCameraShot = false;

// If screenshot should be taken before MTA draws GUI etc.
static bool ms_bBeforeGUI = false;

static SString ms_strScreenDirectoryPath;

// Last save time, seperated per given type
// (normal screenshot or camera weapon initiated)
static long long ms_lLastSaveTime[2] = {0, 0};

// Variables which are also used in save thread
static CBuffer ms_ScreenShotBuffer;
static SString ms_strScreenShotPath;
static bool    ms_bIsSaving = false;
static uint    ms_uiWidth = 0;
static uint    ms_uiHeight = 0;

// whether we want to actually save photo in documents folder
static bool savePhotoInDocuments = false;

void CScreenShot::InitiateScreenShot(bool bIsCameraShot)
{
    if (ms_bScreenShot || ms_bIsSaving || IsRateLimited(bIsCameraShot))
        return;

    ms_bScreenShot = true;
    ms_bIsCameraShot = bIsCameraShot;

    // Camera shots shouldn't have GUI
    ms_bBeforeGUI = bIsCameraShot;

    if (bIsCameraShot)
    {
        if (savePhotoInDocuments)
        {
            // Set the screenshot path to camera gallery path
            ms_strScreenDirectoryPath = PathJoin(GetSystemPersonalPath(), "GTA San Andreas User Files", "Gallery");
        }
    }
    else
    {
        // Set the screenshot path to this default library (screenshots shouldn't really be made outside mods)
        ms_strScreenDirectoryPath = CalcMTASAPath("screenshots");
    }
}

SString CScreenShot::GetScreenshotPath()
{
    // Get the system time
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    return SString("%s\\mta-screen_%d-%02d-%02d_%02d-%02d-%02d.png", *ms_strScreenDirectoryPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
                   sysTime.wMinute, sysTime.wSecond);
}

//
// Take a screenshot if needed.
// @ bool bBeforeGUI: whenever we try to capture screenshot before GUI gets drawn
//
void CScreenShot::CheckForScreenShot(bool bBeforeGUI)
{
    if (!ms_bScreenShot)
        return;

    if (ms_bBeforeGUI != bBeforeGUI)
        return;

    // Update last time of taken screenshot of given type
    ms_lLastSaveTime[ms_bIsCameraShot] = GetTickCount64_();

    if (ms_bIsCameraShot && !savePhotoInDocuments)
    {
        ClearBuffer();
        ms_bScreenShot = false;
        return;
    }

    ms_strScreenShotPath = GetScreenshotPath();
    ms_uiWidth = CDirect3DData::GetSingleton().GetViewportWidth();
    ms_uiHeight = CDirect3DData::GetSingleton().GetViewportHeight();

    // Try to get the screen data
    SString strError;
    if (CGraphics::GetSingleton().GetScreenGrabber()->GetBackBufferPixels(ms_uiWidth, ms_uiHeight, ms_ScreenShotBuffer, strError))
    {
        // Validate data size
        uint uiDataSize = ms_ScreenShotBuffer.GetSize();
        uint uiReqDataSize = ms_uiWidth * ms_uiHeight * 4;

        if (uiDataSize == uiReqDataSize)
        {
            // Start the save thread
            StartSaveThread();
        }
        else
        {
            g_pCore->GetConsole()->Printf(_("Screenshot got %d bytes, but expected %d"), uiDataSize, uiReqDataSize);
            ClearBuffer();
        }
    }
    else
    {
        g_pCore->GetConsole()->Print(_("Screenshot failed") + SString(" (%s)", *strError));
        ClearBuffer();
    }

    ms_bScreenShot = false;
}

// Callback for threaded save
DWORD CScreenShot::ThreadProc(LPVOID lpdwThreadParam)
{
    uint  uiLinePitch = ms_uiWidth * 4;
    void* pData = ms_ScreenShotBuffer.GetData();

    // Create the screen data buffer
    BYTE** ppScreenData = NULL;
    ppScreenData = new BYTE*[ms_uiHeight];
    for (unsigned short y = 0; y < ms_uiHeight; y++)
    {
        ppScreenData[y] = new BYTE[ms_uiWidth * 4];
    }

    // Copy the surface data into a row-based buffer for libpng
    #define BYTESPERPIXEL 4
    unsigned long ulLineWidth = ms_uiWidth * 4;
    for (unsigned int i = 0; i < ms_uiHeight; i++)
    {
        memcpy(ppScreenData[i], (BYTE*)pData + i * uiLinePitch, ulLineWidth);
        for (unsigned int j = 3; j < ulLineWidth; j += BYTESPERPIXEL)
        {
            ppScreenData[i][j] = 0xFF;
        }
    }

    MakeSureDirExists(ms_strScreenShotPath);
    FILE* file = File::Fopen(ms_strScreenShotPath, "wb");
    if (file)
    {
        png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_info*   info_ptr = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, file);
        png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
        png_set_compression_level(png_ptr, 1);
        png_set_IHDR(png_ptr, info_ptr, ms_uiWidth, ms_uiHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);
        png_set_rows(png_ptr, info_ptr, ppScreenData);
        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_STRIP_ALPHA, NULL);
        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(file);

        CCore::GetSingleton().GetConsole()->Printf(_("Screenshot taken: '%s'"), *ms_strScreenShotPath);
    }
    else
    {
        CCore::GetSingleton().GetConsole()->Printf("Could not create screenshot file '%s'", *ms_strScreenShotPath);
    }

    // Clean up the screen data buffer
    if (ppScreenData)
    {
        for (unsigned short y = 0; y < ms_uiHeight; y++)
        {
            delete[] ppScreenData[y];
        }
        delete[] ppScreenData;
    }

    ClearBuffer();
    ms_bIsSaving = false;
    return 0;
}

void CScreenShot::StartSaveThread()
{
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, CREATE_SUSPENDED, NULL);
    if (!hThread)
    {
        CCore::GetSingleton().GetConsole()->Printf("Could not create screenshot thread.");
        ClearBuffer();
    }
    else
    {
        ms_bIsSaving = true;
        SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
        ResumeThread(hThread);
    }
}

// Max one screenshot per second
bool CScreenShot::IsRateLimited(bool bIsCameraShot)
{
    return GetTickCount64_() - ms_lLastSaveTime[bIsCameraShot] < 1000;
}

void CScreenShot::ClearBuffer()
{
    ms_ScreenShotBuffer.Clear();
}

void CScreenShot::SetPhotoSavingInsideDocuments(bool savePhoto) noexcept
{
    savePhotoInDocuments = savePhoto;
}
