/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Utils.h
*  PURPOSE:     Loading utilities
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __UTILS_H
#define __UTILS_H

extern HINSTANCE g_hInstance;
extern HANDLE g_hMutex;

enum ePathResult
{
    GAME_PATH_STEAM,
    GAME_PATH_UNICODE_CHARS,
    GAME_PATH_MISSING,
    GAME_PATH_OK,
};

// Loads the given dll into hProcess. Returns 0 on failure or the handle to the
// remote dll module on success.
HMODULE         RemoteLoadLibrary                   ( HANDLE hProcess, const char* szLibPath );

void            TerminateGTAIfRunning               ( void );
bool            IsGTARunning                        ( void );
void            TerminateOtherMTAIfRunning          ( void );
bool            IsOtherMTARunning                   ( void );

void            ShowSplash                          ( HINSTANCE hInstance );
void            HideSplash                          ( bool bOnlyDelay = false );

long            DisplayErrorMessageBox              ( const SString& strMessage, const SString& strTroubleType = "" );

void            SetMTASAPathSource                  ( bool bReadFromRegistry );
SString         GetMTASAPath                        ( bool bReadFromRegistry = true );
ePathResult     GetGamePath                         ( SString& strOutResult, bool bFindIfMissing = false );
SString         GetMTASAModuleFileName              ( void );

void            ShowProgressDialog                  ( HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel = false );
void            HideProgressDialog                  ( void );
bool            UpdateProgress                      ( int iPos, int iMax, const SString& strMsg = "" );

void            FindFilesRecursive                  ( const SString& strPathMatch, std::vector < SString >& outFileList, uint uiMaxDepth = 99 );
SString         GetOSVersion                        ( void );
SString         GetRealOSVersion                    ( void );
bool            IsVistaOrHigher                     ( void );
BOOL            IsUserAdmin                         ( void );

void            StartPseudoProgress                 ( HINSTANCE hInstance, const SString& strTitle, const SString& strMsg );
void            StopPseudoProgress                  ( void );

SString         ShowCrashedDialog                   ( HINSTANCE hInstance, const SString& strMessage );
void            HideCrashedDialog                   ( void );

void            ShowD3dDllDialog                    ( HINSTANCE hInstance, const SString& strPath );
void            HideD3dDllDialog                    ( void );

void            UpdateMTAVersionApplicationSetting  ( void );
bool            Is32bitProcess                      ( DWORD processID );

bool            CreateSingleInstanceMutex           ( void );
void            ReleaseSingleInstanceMutex          ( void );

SString         CheckOnRestartCommand               ( void );
void            CleanDownloadCache                  ( void );

HMODULE         GetLibraryHandle                    ( const SString& strFilename );
void            FreeLibraryHandle                   ( void );
uint            WaitForObject                       ( HANDLE hProcess, HANDLE hThread, DWORD dwMilliseconds, HANDLE hMutex );

void            MaybeShowCopySettingsDialog         ( void );

#undef CREATE_SUSPENDED
#define CREATE_SUSPENDED 5

#endif
