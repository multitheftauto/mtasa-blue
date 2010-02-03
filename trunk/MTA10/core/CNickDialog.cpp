/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFavouritesAddByIP.h
*  PURPOSE:     Adds a nick dialog on connect for new users or people with
                invalid nicknames
*  DEVELOPERS:  Mr.Hankey <mrhankey88@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CNickDialog::CNickDialog ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    // Create the window
    m_pNickWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "" ) );
    m_pNickWindow->SetCloseButtonEnabled ( false );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pNickWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 280.0f / 2, resolution.fY / 2 - 105.0f / 2  ), false );
    m_pNickWindow->SetSize ( CVector2D ( 280.0f, 105.0f ), false );
    m_pNickWindow->SetSizingEnabled ( false );
	m_pNickWindow->SetAlwaysOnTop ( true );

    // Create the controls
    float fButtonWidth = (280.0f - 30.0f) / 2;

    //  Nick label
    m_pLabelNick = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pNickWindow, "Please enter a valid nickname:" ) );
    m_pLabelNick->SetPosition ( CVector2D ( 12.0f, 24.0f ), false );
    m_pLabelNick->AutoSize ( "Please enter a valid nickname:" );

    //  Nick edit
    m_pEditNick = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pNickWindow ) );
    m_pEditNick->SetPosition ( CVector2D ( 12.0f, 44.0f ), false );
    m_pEditNick->SetSize ( CVector2D ( 154.0f, 24.0f ), false );
    m_pEditNick->SetMaxLength ( 28 );

    //  OK button
    m_pButtonOK = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pNickWindow, "OK" ) );
    m_pButtonOK->SetPosition ( CVector2D ( 12.0f, 80.0f ), false );
    m_pButtonOK->SetSize ( CVector2D ( fButtonWidth, 20.0f ), false );

    //  Cancel button
    m_pButtonBack = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pNickWindow, "Back" ) );
    m_pButtonBack->SetPosition ( CVector2D ( 12.0f + fButtonWidth + 6.0f, 80.0f ), false );
    m_pButtonBack->SetSize ( CVector2D ( fButtonWidth, 20.0f ), false );

    // Register button events
    m_pButtonOK->SetClickHandler ( GUI_CALLBACK ( &CNickDialog::OnButtonOKClick, this ) );
    m_pButtonBack->SetClickHandler ( GUI_CALLBACK ( &CNickDialog::OnButtonBackClick, this ) );

    SetVisible ( false );

}

CNickDialog::~CNickDialog ( void )
{
    delete m_pButtonBack;
    delete m_pButtonOK;
    delete m_pEditNick;
    delete m_pLabelNick;
    delete m_pNickWindow;
}

void CNickDialog::SetVisible ( bool bVisible )
{
    m_pNickWindow->SetVisible ( bVisible );
    if ( bVisible )
        m_pNickWindow->BringToFront ();
}

bool CNickDialog::IsVisible ( void )
{
    return m_pNickWindow->IsVisible ();
}

void CNickDialog::Reset ( void )
{
    m_pEditNick->SetText ("");
}

bool CNickDialog::OnButtonOKClick ( CGUIElement* pElement )
{
    std::string strNick = m_pEditNick->GetText();

    // Invalid nickname?
    if ( !g_pCore->GetSingleton ().IsValidNick ( strNick.c_str () ) )
    {
		CCore::GetSingleton ().ShowMessageBox ( "Error", "Your nickname contains invalid characters!", MB_BUTTON_OK | MB_ICON_INFO );
        return false;
    }

    CVARS_SET ( "nick", strNick );
    Reset ();
    SetVisible ( false );
    return g_pCore->GetSingleton ().GetLocalGUI()->GetMainMenu ()->GetServerBrowser ()->ConnectToSelectedServer();
}

bool CNickDialog::OnButtonBackClick ( CGUIElement* pElement )
{
    Reset ();
    SetVisible ( false );
    return true;
}
