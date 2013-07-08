/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTransferBox.cpp
*  PURPOSE:     Transfer box GUI
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define TRANSFERBOX_HEIGHT      58
#define TRANSFERBOX_ICONSIZE    20
#define TRANSFERBOX_PROGRESSHEIGHT  28
#define TRANSFERBOX_YSTART      20
#define TRANSFERBOX_SPACER      11

CTransferBox::CTransferBox ( void )
{
    CGUI* pGUI = g_pCore->GetGUI ();
    // Create our text for each transfer type
    m_strTransferText[Type::NORMAL] = _("Download Progress:");
    m_strTransferText[Type::PACKET] = _("Map download progress:");

    // Find our largest piece of text, so we can size accordingly
    float fTransferBoxWidth = 0;
    for (int i=0; i<Type::MAX_TYPES; i++)
        fTransferBoxWidth = Max < float > ( fTransferBoxWidth, pGUI->GetTextExtent ( m_strTransferText[i] + " " + SString(_("%s of %s"),"999.99 kB","999.99 kB"), "default-bold-small" ) );
    fTransferBoxWidth = Max < float > ( fTransferBoxWidth, pGUI->GetTextExtent ( _("Disconnect to cancel download"), "default-normal" ) );

    // Add some padding to our text for the size of the window
    fTransferBoxWidth += 80;
    
    // Begin creating our GUI
    CVector2D ScreenSize = pGUI->GetResolution ();
    float fFontHeight = pGUI->GetDefaultFont ()->GetFontHeight ();

    // create the window
    m_pWindow = pGUI->CreateWnd ();
    m_pWindow->SetText ( "" );
    m_pWindow->SetAlpha ( 0.7f );
    m_pWindow->SetVisible ( false );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->SetCloseButtonEnabled ( false );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetPosition ( CVector2D ( ScreenSize.fX*0.5f - fTransferBoxWidth*0.5f, ScreenSize.fY*0.85f - TRANSFERBOX_HEIGHT*0.5f ) );
    m_pWindow->SetSize ( CVector2D ( fTransferBoxWidth, TRANSFERBOX_HEIGHT ) );     // relative 0.35, 0.225

    // create the progress bar
    m_pProgress = pGUI->CreateProgressBar ( m_pWindow );
    m_pProgress->SetPosition ( CVector2D ( 0, TRANSFERBOX_YSTART ) );
    m_pProgress->SetSize ( CVector2D ( fTransferBoxWidth, TRANSFERBOX_HEIGHT - TRANSFERBOX_YSTART - TRANSFERBOX_SPACER ) );

    // stats label
    m_pInfo = pGUI->CreateLabel ( m_pProgress, _("Disconnect to cancel download") );
    float fTempX = ( m_pProgress->GetSize().fX - pGUI->GetTextExtent(m_pInfo->GetText().c_str()) - TRANSFERBOX_ICONSIZE - 4 )*0.5f;
    m_pInfo->SetPosition ( CVector2D ( fTempX + TRANSFERBOX_ICONSIZE + 4, 0 ) );
    m_pInfo->SetSize ( CVector2D ( fTransferBoxWidth, TRANSFERBOX_PROGRESSHEIGHT ) );
    m_pInfo->SetTextColor ( 0, 0, 0 );
    m_pInfo->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

    // create the icons
    for ( unsigned int i = 0; i < TRANSFERBOX_FRAMES; i++ ) {
        SString strIcon ( "cgui\\images\\transferset\\%u.png", i+1 );
        m_pIcon[i] = pGUI->CreateStaticImage ( m_pProgress );
        m_pIcon[i]->SetFrameEnabled ( false );
        m_pIcon[i]->SetPosition ( CVector2D ( fTempX, ((TRANSFERBOX_PROGRESSHEIGHT)/2)  - (TRANSFERBOX_ICONSIZE/2) ) );
        m_pIcon[i]->SetSize ( CVector2D ( TRANSFERBOX_ICONSIZE, TRANSFERBOX_ICONSIZE ) );
        m_pIcon[i]->LoadFromFile ( strIcon );
        m_pIcon[i]->SetVisible ( false );
    }
    m_pIcon[0]->SetVisible ( true );

    // set animation counters
    m_uiVisible = 0;
    m_dTotalSize = 0;
}

CTransferBox::~CTransferBox ( void )
{
    for ( unsigned int i = 0; i < TRANSFERBOX_FRAMES; i++ ) {
        delete m_pIcon[i];
    }

    if ( m_pWindow != NULL )
        delete m_pWindow;
    if ( m_pInfo != NULL )
        delete m_pInfo;
    if ( m_pProgress != NULL )
        delete m_pProgress;
}

void CTransferBox::Show ( void )
{
    m_pWindow->SetVisible ( true );
    g_pCore->GetGUI ()->SetTransferBoxVisible ( true );
}

void CTransferBox::Hide ( void )
{
    m_pWindow->SetVisible ( false );
    g_pCore->GetGUI ()->SetTransferBoxVisible ( false );

    m_dTotalSize = 0;
}

void CTransferBox::SetInfo ( double dDownloadSizeNow, CTransferBox::Type eTransferType )
{
    // Convert to reasonable units
    SString strDownloadSizeNow   = GetDataUnit ( static_cast < unsigned long long > ( dDownloadSizeNow ) );
    SString strDownloadSizeTotal = GetDataUnit ( static_cast < unsigned long long > ( m_dTotalSize ) );

    SString strBuffer = m_strTransferText[eTransferType] + " " +
        SString(_("%s of %s"),strDownloadSizeNow.c_str (), strDownloadSizeTotal.c_str ());     // TRANSLATORS: This represents the download progress. E.g. "500 kB of 800 kB"
    m_pWindow->SetText ( strBuffer );

    m_pProgress->SetProgress ( static_cast < float > (dDownloadSizeNow / m_dTotalSize) );
}


void CTransferBox::DoPulse ( void )
{
    // animated icon mechanism
    if ( m_AnimTimer.Get () > TRANSFERBOX_DELAY )
    {
        m_AnimTimer.Reset ();
        m_pIcon[m_uiVisible]->SetVisible ( false );
        m_uiVisible = ( m_uiVisible + 1 ) % TRANSFERBOX_FRAMES;
        m_pIcon[m_uiVisible]->SetVisible ( true );
    }
}


