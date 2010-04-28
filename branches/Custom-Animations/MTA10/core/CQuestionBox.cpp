/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CQuestionBox.cpp
*  PURPOSE:     Quick connect window
*  DEVELOPERS:  Plectrum Handyworthingtonshire
* 
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CQuestionBox::CQuestionBox ( void )
{
    m_pWindow = NULL;
    m_pMessage = NULL;
    m_uiLastButton = -1;
    m_Callback = NULL;
    m_CallbackParameter = 0;
    m_uiActiveButtons = 0;

    CGUI *pManager = g_pCore->GetGUI ();

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "" ) );
    m_pWindow->SetCloseButtonEnabled ( false );
    m_pWindow->SetVisible ( false );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );

    // Message label
    m_pMessage = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
}


CQuestionBox::~CQuestionBox ( void )
{
    for ( unsigned int i = 0 ; i < m_ButtonList.size () ; i++ )
        delete m_ButtonList[ i ];
    delete m_pMessage;
    delete m_pWindow;
}


void CQuestionBox::Hide ( void )
{
    m_pWindow->SetVisible ( false );
}


void CQuestionBox::Show ( void )
{
    // Layout - Calc how many lines of text
    SString strMsg = m_pMessage->GetText ();
    unsigned int uiNumLines = std::count( strMsg.begin(), strMsg.end(), '\n' ) + 1;

    float fMsgWidth = Max ( 400.f, m_pMessage->GetTextExtent () + 50.f );
    float fMsgHeight = Max < float > ( 3, uiNumLines ) * m_pMessage->GetFontHeight ();
    float fWinWidth = Max ( fMsgWidth, m_uiActiveButtons * ( 112 + 10.f ) );
    float fWinHeight = 50 + fMsgHeight + 50 + 30;

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - fWinWidth / 2, resolution.fY / 2 - fWinHeight / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( fWinWidth, fWinHeight + 10  ), false );

    m_pMessage->SetPosition ( CVector2D ( fWinWidth / 2 - fMsgWidth / 2, fWinHeight / 2 - fMsgHeight / 2 - 10  ), false );
    m_pMessage->SetSize ( CVector2D ( fMsgWidth, fMsgHeight ), false );
    m_pMessage->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );

    // Position the buttons evenly across the bottom
    float fSpaceBetween = ( fWinWidth - ( m_uiActiveButtons * 112 ) ) / ( m_uiActiveButtons + 1 );

    for ( unsigned int i = 0 ; i < m_ButtonList.size () ; i++ )
    {
        if ( i < m_uiActiveButtons )
        {
            float fPosX = fWinWidth - ( ( i + 1 ) * fSpaceBetween + i * 112 ) - 112;
            m_ButtonList[ i ]->SetPosition ( CVector2D ( fPosX, fWinHeight - 35  ), false );
            m_ButtonList[ i ]->SetSize ( CVector2D ( 112, 24 ), false );
        }
    }

    m_pWindow->SetVisible ( true );
    m_pWindow->Activate ();
}


void CQuestionBox::Reset ( void )
{
    Hide ();
    m_uiLastButton = -1;
    SetTitle ( "" );
    SetMessage ( "" );
    SetCallback ( NULL );
    m_uiActiveButtons = 0;
    for ( unsigned int i = 0 ; i < m_ButtonList.size () ; i++ )
        m_ButtonList[ i ]->SetVisible ( false );
}


void CQuestionBox::SetTitle ( const SString& strTitle )
{
    m_pWindow->SetText ( strTitle );
}


void CQuestionBox::SetMessage ( const SString& strMsg )
{
    m_pMessage->SetText ( strMsg );
}


void CQuestionBox::SetButton ( unsigned int uiButton, const SString& strText )
{
    m_uiActiveButtons = Max ( m_uiActiveButtons, uiButton + 1 );
    while ( m_ButtonList.size () < m_uiActiveButtons )
    {
        CGUIButton* pButton = reinterpret_cast < CGUIButton* > ( g_pCore->GetGUI ()->CreateButton ( m_pWindow, "" ) );
        pButton->SetClickHandler ( GUI_CALLBACK ( &CQuestionBox::OnButtonClick, this ) );
        pButton->SetUserData ( reinterpret_cast < void* > ( m_ButtonList.size () ) );
        pButton->SetVisible ( false );
        m_ButtonList.push_back ( pButton );
    }

    m_ButtonList[ uiButton ]->SetText ( strText );
    m_ButtonList[ uiButton ]->SetVisible ( true );
}


void CQuestionBox::SetCallback ( pfnQuestionCallback callback, void* ptr )
{
    m_Callback = callback;
    m_CallbackParameter = ptr;
}


unsigned int CQuestionBox::PollButtons ( void )
{
    return m_uiLastButton;
}


bool CQuestionBox::OnButtonClick ( CGUIElement* pElement )
{
    m_uiLastButton = reinterpret_cast < unsigned int > ( pElement->GetUserData () );
    if ( m_Callback)
        m_Callback ( m_CallbackParameter, m_uiLastButton );
    return true;
}


