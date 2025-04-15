/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDebugView.cpp
 *  PURPOSE:     In-game debug view window implementation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;
extern CChat* g_pChat;

CDebugView::CDebugView(CGUI* pManager, const CVector2D& vecPosition) : CChat()
{
    CChat* pChat = g_pChat;
    g_pChat = this;

    m_pManager = pManager;

    CVector2D vecResolution = m_pManager->GetResolution();
    m_vecScale = CVector2D(vecResolution.fX / 800.0f, vecResolution.fY / 600.0f);
    m_vecBackgroundPosition = vecPosition * vecResolution;

    m_bUseCEGUI = false;
    m_ulChatLineLife = 0;
    m_ulChatLineFadeOut = 0;
    m_bCssStyleText = false;
    m_bCssStyleBackground = false;
    m_bVisible = false;
    m_bInputVisible = false;
    m_uiNumLines = 7;
    m_uiMostRecentLine = 0;
    m_uiScrollOffset = 0;
    m_fSmoothScroll = 0;
    m_fSmoothLastTimeSeconds = 0;
    m_fSmoothAllowAfter = 0;
    m_fSmoothScrollResetTime = 0;
    m_fSmoothRepeatTimer = 0;
    m_pFont = m_pManager->GetBoldFont();
    m_pDXFont = NULL;
    SetDxFont(g_pCore->GetGraphics()->GetFont());
    m_fNativeWidth = DEBUGVIEW_WIDTH;
    m_fRcpUsingDxFontScale = 1;
    m_bCanChangeWidth = false;
    m_iScrollingBack = 0;
    m_fCssStyleOverrideAlpha = 0.0f;
    m_fBackgroundAlpha = 0.0f;
    m_fInputBackgroundAlpha = 0.f;
    m_pCacheTexture = NULL;
    m_iCacheTextureRevision = 0;
    m_iReportCount = 0;
    m_Color = CColor(0, 0, 0, 100);
    m_TextColor = DEBUGVIEW_TEXT_COLOR;
    unsigned long ulBackgroundColor = COLOR_ARGB(m_Color.A, m_Color.R, m_Color.G, m_Color.B);

    m_pBackground = m_pManager->CreateStaticImage();
    m_pBackgroundTexture = m_pManager->CreateTexture();

    m_pBackgroundTexture->LoadFromMemory(&ulBackgroundColor, 1, 1);
    m_pBackground->LoadFromTexture(m_pBackgroundTexture);
    m_pBackground->MoveToBack();
    m_pBackground->SetPosition(m_vecBackgroundPosition);
    m_pBackground->SetSize(m_vecBackgroundSize);
    m_pBackground->SetEnabled(false);
    m_pBackground->SetVisible(false);

    m_pInput = NULL;
    m_pInputTexture = NULL;

    g_pChat = pChat;

    UpdateGUI();
}

void CDebugView::Draw(bool bUseCacheTexture, bool bAllowOutline)
{
    // Are we visible?
    if (!m_bVisible)
        return;

    // Make the chat use our data, dirty as you like!
    CChat* pChat = g_pChat;
    g_pChat = this;

    // Force the window on screen
    CVector2D vecPosition(0.23f, 0.985f);
    CVector2D vecResolution = m_pManager->GetResolution();
    float     height = m_uiNumLines * GetFontHeight(1) * m_vecScale.fY;
    m_vecBackgroundPosition = vecPosition * vecResolution - CVector2D(0, height);
    m_vecBackgroundPosition.fX = Round(m_vecBackgroundPosition.fX);
    m_vecBackgroundPosition.fY = Round(m_vecBackgroundPosition.fY);
    m_pBackground->SetPosition(m_vecBackgroundPosition);

    CChat::Draw(bUseCacheTexture, bAllowOutline);
    g_pChat = pChat;
}

void CDebugView::Output(const char* szText, bool bColorCoded)
{
    CChat* pChat = g_pChat;
    g_pChat = this;
    CChat::Output(szText, bColorCoded);
    g_pChat = pChat;
}
