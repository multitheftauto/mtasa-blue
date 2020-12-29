/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CScreenShot.h
 *  PURPOSE:     Header file for screen capture handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCore.h"
#include <time.h>

class CScreenShot
{
public:
    static SString PreScreenShot();
    static void    PostScreenShot(const SString& strFileName);
    static void    SetPath(const char* szPath);

    static SString GetScreenShotPath(int iNumber);
    static SString GetValidScreenshotFilename();
    static int     GetScreenShots();

    static void  BeginSave(const char* szFileName, void* pData, uint uiDataSize, uint uiWidth, uint uiHeight);
    static bool  IsSaving();
    static DWORD ThreadProc(LPVOID lpdwThreadParam);
};
