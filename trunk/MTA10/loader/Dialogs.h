/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Dialogs.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

void            ShowSplash                          ( HINSTANCE hInstance );
void            HideSplash                          ( void );

void            ShowProgressDialog                  ( HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel = false );
void            HideProgressDialog                  ( void );
bool            UpdateProgress                      ( int iPos, int iMax, const SString& strMsg = "" );

void            StartPseudoProgress                 ( HINSTANCE hInstance, const SString& strTitle, const SString& strMsg );
void            StopPseudoProgress                  ( void );

SString         ShowCrashedDialog                   ( HINSTANCE hInstance, const SString& strMessage );
void            HideCrashedDialog                   ( void );

void            ShowD3dDllDialog                    ( HINSTANCE hInstance, const SString& strPath );
void            HideD3dDllDialog                    ( void );

void            ShowOptimusDialog                   ( HINSTANCE hInstance );
void            HideOptimusDialog                   ( void );

void            ShowNoAvDialog                      ( HINSTANCE hInstance, bool bWSCNotMonitoring );
void            HideNoAvDialog                      ( void );

#ifdef MTA_DEBUG
void            TestDialogs                         ( void );
#endif
