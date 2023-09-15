/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Dialogs.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>

void ShowSplash(HINSTANCE hInstance);
void HideSplash();

void ShowProgressDialog(HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel = false);
void HideProgressDialog();
bool UpdateProgress(int iPos, int iMax, const SString& strMsg = "");

void StartPseudoProgress(HINSTANCE hInstance, const SString& strTitle, const SString& strMsg);
void StopPseudoProgress();

SString ShowCrashedDialog(HINSTANCE hInstance, const SString& strMessage);
void    HideCrashedDialog();

struct GraphicsLibrary
{
    SString stem;            // filename without the final extension
    SString absoluteFilePath;
    SString md5Hash;
    SString appLastHash;
    SString appDontRemind;

    explicit GraphicsLibrary(const char* libraryStemName) : stem(std::string(libraryStemName)) {}
};

void ShowGraphicsDllDialog(HINSTANCE hInstance, const std::vector<GraphicsLibrary>& offenders);
void HideGraphicsDllDialog();

void ShowOptimusDialog(HINSTANCE hInstance);
void HideOptimusDialog();

void ShowNoAvDialog(HINSTANCE hInstance, bool bEnableScaremongering);
void HideNoAvDialog();

#ifdef MTA_DEBUG
void TestDialogs();
#endif
