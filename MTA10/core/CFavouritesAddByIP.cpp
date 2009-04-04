/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFavouritesAddByIP.h
*  PURPOSE:     Header file for add to favourites by IP dialog
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CFavouritesAddByIP::CFavouritesAddByIP ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "Add to favourites" ) );
    m_pWindow->SetCloseButtonEnabled ( false );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 280.0f / 2, resolution.fY / 2 - 90.0f / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( 280.0f, 90.0f ), false );
    m_pWindow->SetSizingEnabled ( false );
	m_pWindow->SetAlwaysOnTop ( true );

    // Create the controls
    float fPosX = 0.0f;
    float fButtonWidth = (280.0f - 30.0f) / 2;

    //  Host label
    m_pLabelHost = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Host:" ) );
    m_pLabelHost->SetPosition ( CVector2D ( fPosX + 12.0f, 28.0f ), false );
    m_pLabelHost->AutoSize ( "Host:" );
    fPosX += 12.0f + m_pLabelHost->GetSize ().fX;

    //  Host edit
    m_pEditHost = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditHost->SetPosition ( CVector2D ( fPosX + 5.0f, 24.0f ), false );
    m_pEditHost->SetSize ( CVector2D ( 154.0f, 24.0f ), false );
    m_pEditHost->SetMaxLength ( 128 );          // Just to prevent entering a huge hostname size.. no-one has a hostname over 128 chars i believe
    fPosX += 5.0f + 154.0f;

    //  Port label
    m_pLabelPort = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, ":" ) );
    m_pLabelPort->SetPosition ( CVector2D ( fPosX + 3.0f, 28.0f ), false );
    m_pLabelPort->AutoSize ( ":" );
    fPosX += 3.0f + m_pLabelPort->GetSize ().fX;

    //  Port edit
    m_pEditPort = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditPort->SetPosition ( CVector2D ( fPosX + 3.0f, 24.0f ), false );
    m_pEditPort->SetSize ( CVector2D ( 59.0f, 24.0f ), false );
    m_pEditPort->SetMaxLength ( 5 );
    fPosX += 3.0f + 59.0f;

    //  Add button
    m_pButtonAdd = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Add" ) );
    m_pButtonAdd->SetPosition ( CVector2D ( 12.0f, 60.0f ), false );
    m_pButtonAdd->SetSize ( CVector2D ( fButtonWidth, 20.0f ), false );

    //  Cancel button
    m_pButtonBack = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Back" ) );
    m_pButtonBack->SetPosition ( CVector2D ( 12.0f + fButtonWidth + 6.0f, 60.0f ), false );
    m_pButtonBack->SetSize ( CVector2D ( fButtonWidth, 20.0f ), false );

    // Register button events
    m_pButtonBack->SetOnClickHandler ( GUI_CALLBACK ( &CFavouritesAddByIP::OnButtonBackClick, this ) );
}

CFavouritesAddByIP::~CFavouritesAddByIP ( void )
{
    delete m_pButtonBack;
    delete m_pButtonAdd;
    delete m_pEditPort;
    delete m_pLabelPort;
    delete m_pEditHost;
    delete m_pLabelHost;
    delete m_pWindow;
}

void CFavouritesAddByIP::SetVisible ( bool bVisible )
{
    m_pWindow->SetVisible ( bVisible );
    if ( bVisible )
        m_pWindow->BringToFront ();
}

bool CFavouritesAddByIP::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}

void CFavouritesAddByIP::Reset ( void )
{
    m_pEditHost->SetText ("");
    m_pEditPort->SetText ("");
}

void CFavouritesAddByIP::SetCallback ( const GUI_CALLBACK & Callback )
{
    m_pButtonAdd->SetOnClickHandler ( Callback );
}

void CFavouritesAddByIP::GetHost ( std::string &strHost )
{
    strHost = m_pEditHost->GetText ();
}

unsigned short CFavouritesAddByIP::GetPort ( void )
{
    unsigned short usPort = static_cast < unsigned short > ( atoi ( m_pEditPort->GetText ().c_str() ) );
    return usPort;
}

bool CFavouritesAddByIP::OnButtonBackClick ( CGUIElement* pElement )
{
    Reset ();
    SetVisible ( false );
    return true;
}
