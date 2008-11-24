/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDebugView.cpp
*  PURPOSE:     In-game debug view window implementation
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;
extern CChat* g_pChat;

CDebugView::CDebugView ( CGUI * pManager, CVector2D & vecPosition ) : CChat ()
{
    CChat * pChat = g_pChat;
    g_pChat = this;

    m_pManager = pManager;    

    CVector2D vecResolution = m_pManager->GetResolution ();
    m_vecScale = CVector2D ( vecResolution.fX / 800.0f, vecResolution.fY / 600.0f );
    vecPosition.fX *= vecResolution.fX;
    vecPosition.fY *= vecResolution.fY;

    m_bUseCEGUI = false;
    m_ulChatLineLife = 0;
    m_ulChatLineFadeOut = 0;
    m_bCssStyleText = false;
    m_bCssStyleBackground = false;
    m_szCommand = NULL;
    m_bVisible = false;
    m_bInputVisible = false;
    m_uiNumLines = 7;
    m_pFont = m_pManager->GetBoldFont ();
    m_pDXFont = g_pCore->GetGraphics ()->GetFont ();
    m_fWidth = 1.0f;
    m_vecBackgroundPosition = vecPosition;
    m_vecBackgroundSize = CVector2D ( ( 576.0f * m_fWidth ) * m_vecScale.fX, ( CChat::GetFontHeight ( m_vecScale.fY ) ) * ( float ) ( ( float ) m_uiNumLines + 0.5f ) );
    m_Color = CColor ( 0, 0, 0, 100 );
    m_TextColor = CColor ( 235, 221, 178, 255 );
    unsigned long ulBackgroundColor = COLOR_ARGB ( m_Color.A, m_Color.R, m_Color.G, m_Color.B );

    m_pBackground = m_pManager->CreateStaticImage ();
    m_pBackgroundTexture = m_pManager->CreateTexture ();

    m_pBackgroundTexture->LoadFromMemory ( &ulBackgroundColor, 1, 1 );
    m_pBackground->LoadFromTexture ( m_pBackgroundTexture );
    m_pBackground->MoveToBack ();
    m_pBackground->SetPosition ( m_vecBackgroundPosition );
    m_pBackground->SetSize ( m_vecBackgroundSize );
    m_pBackground->SetEnabled ( false );
    m_pBackground->SetVisible ( false );

    m_pInput = NULL;
    m_pInputTexture = NULL;
    m_pInputLine = NULL;
    m_szInputText = NULL;
    m_szCommand = NULL;   

    g_pChat = pChat;
}


void CDebugView::Draw ( void )
{
    // Make the chat use our data, dirty as you like!
    CChat * pChat = g_pChat;
    g_pChat = this;
    CChat::Draw ();
    g_pChat = pChat;
}


void CDebugView::Output ( char* szText, bool bColorCoded )
{
    CChat * pChat = g_pChat;
    g_pChat = this;
    CChat::Output ( szText, bColorCoded );
    g_pChat = pChat;
}

void CDebugView::Outputf ( bool bColorCoded, char* szText, ... )
{
    char szBuffer [ 1024 ];
	va_list ap;
	va_start ( ap, szText );
	_vsnprintf ( szBuffer, 1024, szText, ap );
	va_end ( ap );

    CChat * pChat = g_pChat;
    g_pChat = this;
    CChat::Output ( szBuffer, bColorCoded );
    g_pChat = pChat;
}