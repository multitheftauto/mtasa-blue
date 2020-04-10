/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        loader/Dialogs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

void ShowSplash(HINSTANCE hInstance);
void HideSplash();

void ShowProgressDialog(HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel = false);
void HideProgressDialog();
bool UpdateProgress(int iPos, int iMax, const SString& strMsg = "");

void StartPseudoProgress(HINSTANCE hInstance, const SString& strTitle, const SString& strMsg);
void StopPseudoProgress();

SString ShowCrashedDialog(HINSTANCE hInstance, const SString& strMessage);
void    HideCrashedDialog();

void ShowD3dDllDialog(HINSTANCE hInstance, const SString& strPath);
void HideD3dDllDialog();

void ShowOptimusDialog(HINSTANCE hInstance);
void HideOptimusDialog();

void ShowNoAvDialog(HINSTANCE hInstance, bool bEnableScaremongering);
void HideNoAvDialog();

#ifdef MTA_DEBUG
void TestDialogs();
#endif
