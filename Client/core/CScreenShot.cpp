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
#include <game/CHud.h>
#include <libpng/png.h>

extern CCore* g_pCore;

static CBuffer ms_ScreenShotBuffer;
static char    ms_szScreenShotPath[MAX_PATH] = {0};

static long long ms_lLastSaveTime = 0;
static SString ms_strFileName;

// If screenshot should be taken on the current frame
static bool    ms_bScreenShot = false;

// If GUI should be hidden on the current frame
static bool    ms_bHideGUIForScreenShot = false;

// If HUD was disabled on screenshot initiation
static bool    ms_bScreenShotHUDWasDisabled = false;

// Variables used for saving the screenshot file on a separate thread
static bool    ms_bIsSaving = false;
static uint    ms_uiWidth = 0;
static uint    ms_uiHeight = 0;
static void*   ms_pData = NULL;
static uint    ms_uiDataSize = 0;

void CScreenShot::InitiateScreenShot(bool bCameraShot)
{
    if (IsSaving() || IsRateLimited())
        return;

    ms_bScreenShot = true;
    ms_bHideGUIForScreenShot = bCameraShot;
    ms_bScreenShotHUDWasDisabled = g_pCore->GetGame()->GetHud()->IsDisabled();

    if (bCameraShot)
    {
        g_pCore->GetGame()->GetHud()->Disable(true);

        // Set the screenshot path to camera gallery path
        SString strGalleryPath = PathJoin(GetSystemPersonalPath(), "GTA San Andreas User Files", "Gallery");
        SetPath(strGalleryPath.c_str());
    }
    else
    {
        // Set the screenshot path to this default library (screenshots shouldn't really be made outside mods)
        SString strScreenShotPath = CalcMTASAPath("screenshots");
        SetPath(strScreenShotPath.c_str());
    }
}

SString CScreenShot::PreScreenShot()
{
    ms_lLastSaveTime = GetTickCount64_();
    return GetValidScreenshotFilename();
}

void CScreenShot::PostScreenShot(const SString& strFileName)
{
    // print a notice
    if (!strFileName.empty())
        g_pCore->GetConsole()->Printf(_("Screenshot taken: '%s'"), *strFileName);

    g_pCore->GetGame()->GetHud()->Disable(ms_bScreenShotHUDWasDisabled);
    ms_bScreenShot = false;
    ms_bHideGUIForScreenShot = false;
}

void CScreenShot::SetPath(const char* szPath)
{
    strncpy(ms_szScreenShotPath, szPath, MAX_PATH - 1);

    // make sure the directory exists
    File::Mkdir(szPath);
}

SString CScreenShot::GetValidScreenshotFilename()
{
    // Get the system time
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    return SString("%s\\mta-screen_%d-%02d-%02d_%02d-%02d-%02d.png", &ms_szScreenShotPath[0], sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
                   sysTime.wMinute, sysTime.wSecond);
}

// Take a screenshot if needed
void CScreenShot::CheckForScreenShot()
{
    if (!ms_bScreenShot)
        return;

    SString strFileName = PreScreenShot();
    uint    uiWidth = CDirect3DData::GetSingleton().GetViewportWidth();
    uint    uiHeight = CDirect3DData::GetSingleton().GetViewportHeight();

    // Try to get the screen data
    SString strError;
    if (CGraphics::GetSingleton().GetScreenGrabber()->GetBackBufferPixels(uiWidth, uiHeight, ms_ScreenShotBuffer, strError))
    {
        // Validate data size
        uint uiDataSize = ms_ScreenShotBuffer.GetSize();
        uint uiReqDataSize = uiWidth * uiHeight * 4;

        if (uiDataSize == uiReqDataSize)
        {
            // Start the save thread
            BeginSave(strFileName, ms_ScreenShotBuffer.GetData(), uiDataSize, uiWidth, uiHeight);
        }
        else
        {
            g_pCore->GetConsole()->Printf(_("Screenshot got %d bytes, but expected %d"), uiDataSize, uiReqDataSize);
            strFileName = "";
        }
    }
    else
    {
        g_pCore->GetConsole()->Print(_("Screenshot failed") + SString(" (%s)", *strError));
        strFileName = "";
    }

    PostScreenShot(strFileName);
}

// Callback for threaded save
// Static function
DWORD CScreenShot::ThreadProc(LPVOID lpdwThreadParam)
{
    unsigned long ulScreenHeight = ms_uiHeight;
    unsigned long ulScreenWidth = ms_uiWidth;
    uint          uiReqDataSize = ulScreenHeight * ulScreenWidth * 4;
    uint          uiLinePitch = ulScreenWidth * 4;

    if (uiReqDataSize != ms_uiDataSize)
    {
        ms_bIsSaving = false;
        return 0;
    }

    // Create the screen data buffer
    BYTE** ppScreenData = NULL;
    ppScreenData = new BYTE*[ulScreenHeight];
    for (unsigned short y = 0; y < ulScreenHeight; y++)
    {
        ppScreenData[y] = new BYTE[ulScreenWidth * 4];
    }

    // Copy the surface data into a row-based buffer for libpng
    #define BYTESPERPIXEL 4
    unsigned long ulLineWidth = ulScreenWidth * 4;
    for (unsigned int i = 0; i < ulScreenHeight; i++)
    {
        memcpy(ppScreenData[i], (BYTE*)ms_pData + i * uiLinePitch, ulLineWidth);
        for (unsigned int j = 3; j < ulLineWidth; j += BYTESPERPIXEL)
        {
            ppScreenData[i][j] = 0xFF;
        }
    }

    MakeSureDirExists(ms_strFileName);
    FILE* file = File::Fopen(ms_strFileName, "wb");
    if (file)
    {
        png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_info*   info_ptr = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, file);
        png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
        png_set_compression_level(png_ptr, 1);
        png_set_IHDR(png_ptr, info_ptr, ulScreenWidth, ulScreenHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);
        png_set_rows(png_ptr, info_ptr, ppScreenData);
        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_STRIP_ALPHA, NULL);
        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(file);
    }
    else
    {
        CCore::GetSingleton().GetConsole()->Printf("Could not create screenshot file '%s'", *ms_strFileName);
    }

    // Clean up the screen data buffer
    if (ppScreenData)
    {
        for (unsigned short y = 0; y < ulScreenHeight; y++)
        {
            delete[] ppScreenData[y];
        }
        delete[] ppScreenData;
    }

    ms_bIsSaving = false;
    return 0;
}

// Static function
void CScreenShot::BeginSave(const char* szFileName, void* pData, uint uiDataSize, uint uiWidth, uint uiHeight)
{
    if (ms_bIsSaving)
        return;

    ms_strFileName = szFileName;
    ms_pData = pData;
    ms_uiDataSize = uiDataSize;
    ms_uiWidth = uiWidth;
    ms_uiHeight = uiHeight;

    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, CREATE_SUSPENDED, NULL);
    if (!hThread)
    {
        CCore::GetSingleton().GetConsole()->Printf("Could not create screenshot thread.");
    }
    else
    {
        ms_bIsSaving = true;
        SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
        ResumeThread(hThread);
    }
}

bool CScreenShot::ShouldGUIBeHidden()
{
    return ms_bHideGUIForScreenShot;
}

bool CScreenShot::IsSaving()
{
    return ms_bIsSaving;
}

bool CScreenShot::IsRateLimited()
{
    return GetTickCount64_() - ms_lLastSaveTime < 1000;
}
