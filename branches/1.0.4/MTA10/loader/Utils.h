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

// Loads the given dll into hProcess. Returns 0 on failure or the handle to the
// remote dll module on success.
HMODULE RemoteLoadLibrary           ( HANDLE hProcess, const char* szLibPath );

bool    TerminateGTAIfRunning       ( void );

void    ShowSplash                  ( HINSTANCE hInstance );
void    HideSplash                  ( bool bOnlyDelay = false );

long    DisplayErrorMessageBox      ( const SString& strMessage );

SString ReadRegistryStringValue     ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, int* iResult = NULL );
void    WriteRegistryStringValue    ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, const char* szBuffer );

void    SetMTASAPathSource          ( bool bReadFromRegistry );
SString GetMTASAPath                ( void );
int     GetGamePath                 ( SString& strOutResult );

void    ShowProgressDialog          ( HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel = false );
void    HideProgressDialog          ( void );
bool    UpdateProgress              ( int iPos, int iMax, const SString& strMsg = "" );

void    FindFilesRecursive          ( const SString& strPath, std::vector < SString >& outFileList );
bool    CheckPermissions            ( const std::string& strPath, unsigned int uiMaxTimeMs );
void    FixPermissions              ( const std::string& strPath );
bool    IsVistaOrHigher             ( void );

bool    ShellExecuteBlocking        ( const SString& strAction, const SString& strFile, const SString& strParameters = "", const SString& strDirectory = "" );
bool    ShellExecuteNonBlocking     ( const SString& strAction, const SString& strFile, const SString& strParameters = "", const SString& strDirectory = "" );

SString ConformPath                 ( const SString& strPath );
SString PathJoin                    ( const SString& A, const SString& B );
bool    DelTree                     ( const SString& strPath, const SString& strInsideHere );
bool    MkDir                       ( const SString& strPath );

bool    FileCopy                    ( const SString& strSrc, const SString& strDest );
SString GetCurrentDir               ( void );

void    StartPseudoProgress           ( HINSTANCE hInstance, const SString& strTitle, const SString& strMsg );
void    StopPseudoProgress            ( void );

#endif
