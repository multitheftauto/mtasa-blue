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
    m_uiActiveEditboxes = 0;
    m_bAutoCloseOnConnect = false;

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

    float fEditHeight = 29.0f;
    float fEditSpacer = 10.0f;

    float fMsgWidth = Max ( 400.f, m_pMessage->GetTextExtent () + 50.f );
    float fMsgHeight = Max < float > ( 3, uiNumLines ) * ( m_pMessage->GetFontHeight () + 1 );
    float fWinWidth = Max ( fMsgWidth, m_uiActiveButtons * ( 112 + 10.f ) );
    float fWinHeight = 50 + fMsgHeight + 50 + 30 + ( m_uiActiveEditboxes * ( fEditHeight + 2*fEditSpacer ) ) ;

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - fWinWidth / 2, resolution.fY / 2 - fWinHeight / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( fWinWidth, fWinHeight + 10  ), false );

    m_pMessage->SetPosition ( CVector2D ( fWinWidth / 2 - fMsgWidth / 2, fWinHeight / 2 - fMsgHeight / 2 - 10  ), false );
    m_pMessage->SetSize ( CVector2D ( fMsgWidth, fMsgHeight ), false );
    m_pMessage->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );

    // One editbox takes the entire width, position them like so
    for ( unsigned int i = 0 ; i < m_EditList.size () ; i++ )
    {
        if ( i < m_uiActiveButtons )
        {
            m_EditList[ i ]->SetPosition ( CVector2D ( 0, (fWinHeight / 2 - fMsgHeight / 2 - 10) + fMsgHeight + (i+1)*fEditSpacer + i*fEditHeight ), false );
            m_EditList[ i ]->SetSize ( CVector2D ( fWinWidth - 20, fEditHeight ), false );
        }
    }

    // Position the buttons evenly across the bottom
    float fSpaceBetween = ( fWinWidth - ( m_uiActiveButtons * 112 ) ) / ( m_uiActiveButtons + 1 ) + m_uiActiveEditboxes ? fEditSpacer : 0;

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
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->BringToFront ();
    g_pCore->RemoveMessageBox ();

    AddReportLog( 9100, SString( "QuestionBox::Show [%s] %s", m_pWindow->GetText().c_str(), *m_strMsg.Left( 200 ).Replace( "\n", "|" ) ) );
}


void CQuestionBox::Reset ( void )
{
    Hide ();
    m_uiLastButton = -1;
    m_bAutoCloseOnConnect = false;
    m_strMsg = "";
    SetTitle ( "" );
    SetMessage ( "" );
    SetCallback ( NULL );
    SetCallbackEdit ( NULL );
    m_uiActiveButtons = 0;
    for ( unsigned int i = 0 ; i < m_ButtonList.size () ; i++ )
        m_ButtonList[ i ]->SetVisible ( false );

    for ( unsigned int i = 0 ; i < m_EditList.size () ; i++ )
        m_EditList[ i ]->SetVisible ( false );
}


void CQuestionBox::SetTitle ( const SString& strTitle )
{
    m_pWindow->SetText ( strTitle );
}


void CQuestionBox::SetMessage ( const SString& strMsg )
{
    m_strMsg = strMsg;
    m_pMessage->SetText ( strMsg );
}

void CQuestionBox::AppendMessage ( const SString& strMsg )
{
    m_pMessage->SetText ( SString ( m_strMsg + strMsg ) );
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
        pButton->SetZOrderingEnabled ( false );
        m_ButtonList.push_back ( pButton );
    }

    m_ButtonList[ uiButton ]->SetText ( strText );
    m_ButtonList[ uiButton ]->SetVisible ( true );
}

CGUIEdit* CQuestionBox::SetEditbox ( unsigned int uiEditbox, const SString& strText )
{
    m_uiActiveEditboxes = Max ( m_uiActiveEditboxes, uiEditbox + 1 );
    while ( m_EditList.size () < m_uiActiveEditboxes )
    {
        CGUIEdit* pEdit = reinterpret_cast < CGUIEdit* > ( g_pCore->GetGUI ()->CreateEdit ( m_pWindow ) );
        pEdit->SetTextAcceptedHandler ( GUI_CALLBACK ( &CQuestionBox::OnButtonClick, this ) );
        pEdit->SetUserData ( reinterpret_cast < void* > ( m_EditList.size () ) );
        pEdit->SetVisible ( false );
        m_EditList.push_back ( pEdit );
    }

    m_EditList[ uiEditbox ]->SetText ( strText );
    m_EditList[ uiEditbox ]->SetVisible ( true );
    return m_EditList [ uiEditbox ];
}


void CQuestionBox::SetCallback ( pfnQuestionCallback callback, void* ptr )
{
    m_Callback = callback;
    m_CallbackParameter = ptr;
}

void CQuestionBox::SetCallbackEdit ( pfnQuestionEditCallback callback, void* ptr )
{
    m_CallbackEdit = callback;
    m_CallbackParameter = ptr;
}


// Call after the main message has been set
void CQuestionBox::SetOnLineHelpOption( const SString& strTroubleLink )
{
    SString strMessage = "\n\n";
    strMessage += _("Do you want to see some on-line help about this problem ?");
    AppendMessage( strMessage );
    SetButton( 0, _("No") );
    SetButton( 1, _("Yes") );
    SetCallback( CCore::ErrorMessageBoxCallBack, new SString( strTroubleLink ) );
}


unsigned int CQuestionBox::PollButtons ( void )
{
    if ( !m_pWindow->IsVisible () )
        return -1;
    return m_uiLastButton;
}


bool CQuestionBox::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}


bool CQuestionBox::OnButtonClick ( CGUIElement* pElement )
{
    // For detecting startup problems
    WatchDogUserDidInteractWithMenu();

    m_uiLastButton = reinterpret_cast < unsigned int > ( pElement->GetUserData () );
    if ( m_Callback)
        m_Callback ( m_CallbackParameter, m_uiLastButton );
    
    if ( m_CallbackEdit && !m_EditList.empty() ) //Just grab the first editbox for now
        m_CallbackEdit ( m_CallbackParameter, m_uiLastButton, m_EditList[0]->GetText() );
    return true;
}


void CQuestionBox::SetAutoCloseOnConnect ( bool bEnable )
{
    m_bAutoCloseOnConnect = bEnable;
}


void CQuestionBox::OnConnect ( void )
{
    if ( m_bAutoCloseOnConnect )
        Hide ();
}
