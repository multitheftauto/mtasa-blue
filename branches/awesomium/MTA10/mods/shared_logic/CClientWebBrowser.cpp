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

CClientWebBrowser::CClientWebBrowser ( CClientManager* pManager, ElementID ID, CWebBrowserItem* pWebBrowserItem, CWebViewInterface* pWebView )
    : ClassInit ( this ), CClientTexture ( pManager, ID, pWebBrowserItem ), m_pWebView ( pWebView )
{
}

CClientWebBrowser::~CClientWebBrowser ()
{
    // We can't release the memory here since we deal with an interface
    m_pWebView = NULL;

    // Unlink from tree
    Unlink ();
}

bool CClientWebBrowser::IsLoading ()
{
    return m_pWebView->IsLoading ();
}

bool CClientWebBrowser::LoadURL ( const SString& strURL, bool bFilterEnabled )
{
    return m_pWebView->LoadURL ( strURL, bFilterEnabled );
}

void CClientWebBrowser::GetTitle ( SString& outPageTitle )
{
    m_pWebView->GetTitle ( outPageTitle );
}

void CClientWebBrowser::GetURL ( SString& outURL )
{
    m_pWebView->GetURL ( outURL );
}

void CClientWebBrowser::SetRenderingPaused ( bool bPaused )
{
    m_pWebView->SetRenderingPaused ( bPaused );
}

bool CClientWebBrowser::ExecuteJavascript ( const SString& strJavascriptCode )
{
    // Don't allow javascript code execution on remote websites
    if ( !m_pWebView->IsLocal () )
        return false;

    m_pWebView->ExecuteJavascript ( strJavascriptCode );
    return true;
}

void CClientWebBrowser::InjectMouseMove ( int iPosX, int iPosY )
{
    m_pWebView->InjectMouseMove ( iPosX, iPosY );
}

void CClientWebBrowser::InjectMouseDown ( int mouseButton )
{
    m_pWebView->InjectMouseDown ( mouseButton );
}

void CClientWebBrowser::InjectMouseUp ( int mouseButton )
{
    m_pWebView->InjectMouseUp ( mouseButton );
}

void CClientWebBrowser::InjectMouseWheel ( int iScrollVert, int iScrollHorz )
{
    m_pWebView->InjectMouseWheel ( iScrollVert, iScrollHorz );
}

void CClientWebBrowser::InjectKeyboardEvent ( const SString& strKey, bool bKeyDown, bool bCharacter )
{
    m_pWebView->InjectKeyboardEvent ( strKey, bKeyDown, bCharacter );
}

bool CClientWebBrowser::IsLocal ()
{
    return m_pWebView->IsLocal ();
}

void CClientWebBrowser::SetTempURL ( const SString& strTempURL )
{
    m_pWebView->SetTempURL ( strTempURL );
}

bool CClientWebBrowser::SetAudioVolume ( float fVolume )
{
    return m_pWebView->SetAudioVolume ( fVolume );
}
