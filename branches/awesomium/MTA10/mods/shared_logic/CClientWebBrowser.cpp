/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWebBrowser.cpp
*  PURPOSE:     ClientEntity webbrowser tab class
*
*****************************************************************************/

#include "StdInc.h"
#include "CClientWebBrowser.h"

CClientWebBrowser::CClientWebBrowser(CClientManager* pManager, ElementID ID, CWebBrowserItem* pWebBrowserItem, CWebViewInterface* pWebView)
    : ClassInit(this), CClientTexture(pManager, ID, pWebBrowserItem), m_pWebView(pWebView)
{
}

CClientWebBrowser::~CClientWebBrowser()
{
    Unlink ();
}

bool CClientWebBrowser::IsLoading()
{
    return m_pWebView->IsLoading();
}

bool CClientWebBrowser::LoadURL(const SString& strURL)
{
    return m_pWebView->LoadURL(strURL);
}

void CClientWebBrowser::GetTitle(SString& outPageTitle)
{
    m_pWebView->GetTitle(outPageTitle);
}

void CClientWebBrowser::GetURL(SString& outURL)
{
    m_pWebView->GetURL(outURL);
}

/*void CClientWebBrowser::GetScrollPosition(int& iScrollX, int& iScrollY)
{
    g_pCore->GetWebCore ()->GetScrollPosition(GetWebCore Item(), iScrollX, iScrollY);
}

void CClientWebBrowser::SetScrollPosition(int iScrollX, int iScrollY)
{
    g_pCore->GetWebCore ()->SetScrollPosition(GetWebCore Item(), iScrollX, iScrollY);
}*/
/*
void CClientWebBrowser::InjectMouseMove(int iPosX, int iPosY)
{
    g_pCore->GetWebCore ()->InjectMouseMove(GetWebCore Item(), iPosX, iPosY);
}

void CClientWebBrowser::InjectMouseDown(int mouseButton)
{
    g_pCore->GetWebCore ()->InjectMouseDown(GetWebCore Item(), mouseButton);
}

void CClientWebBrowser::InjectMouseUp(int mouseButton)
{
    g_pCore->GetWebCore ()->InjectMouseUp(GetWebCore Item(), mouseButton);
}

void CClientWebBrowser::InjectKeyboardEvent(const SString& strKey, bool bKeyDown, bool bCharacter)
{
    g_pCore->GetWebCore ()->InjectKeyboardEvent(GetWebCore Item(), strKey, bKeyDown, bCharacter);
}
*/