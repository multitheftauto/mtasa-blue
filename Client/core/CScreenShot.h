/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CScreenShot.h
*  PURPOSE:     Header file for screen capture handling class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCORESCREENSHOT_H
#define __CCORESCREENSHOT_H

#include "CCore.h"
#include <time.h>

class CScreenShot
{
public:
    static SString                  PreScreenShot               ( void );
    static void                     PostScreenShot              ( const SString& strFileName );
    static void                     SetPath                     ( const char *szPath );

    static SString                  GetScreenShotPath           ( int iNumber );
    static SString                  GetValidScreenshotFilename  ( void );
    static int                      GetScreenShots              ( void );

    static void                     BeginSave                   ( const char *szFileName, void* pData, uint uiDataSize, uint uiWidth, uint uiHeight );
    static bool                     IsSaving                    ( void );
    static DWORD                    ThreadProc                  ( LPVOID lpdwThreadParam );
};

#endif
