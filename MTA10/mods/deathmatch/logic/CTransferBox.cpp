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

#define TRANSFERBOX_WIDTH		350
#define TRANSFERBOX_HEIGHT		58
#define TRANSFERBOX_ICONSIZE	20
#define TRANSFERBOX_DRAWXSTART  68
#define TRANSFERBOX_PROGRESSHEIGHT  28
#define TRANSFERBOX_YSTART		20
#define TRANSFERBOX_SPACER		11

CTransferBox::CTransferBox ( void )
{
	CVector2D ScreenSize = g_pCore->GetGUI ()->GetResolution ();
	float fFontHeight = g_pCore->GetGUI ()->GetDefaultFont ()->GetFontHeight ();

	// create the window
	m_pWindow = g_pCore->GetGUI ()->CreateWnd ();
	m_pWindow->SetText ( "" );
	m_pWindow->SetAlpha ( 0.7f );
	m_pWindow->SetVisible ( false );
	m_pWindow->SetAlwaysOnTop ( true );
	m_pWindow->SetCloseButtonEnabled ( false );
	m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetPosition ( CVector2D ( ScreenSize.fX*0.5f - TRANSFERBOX_WIDTH*0.5f, ScreenSize.fY*0.85f - TRANSFERBOX_HEIGHT*0.5f ) );
    m_pWindow->SetSize ( CVector2D ( TRANSFERBOX_WIDTH, TRANSFERBOX_HEIGHT ) );		// relative 0.35, 0.225

	// create the progress bar
	m_pProgress = g_pCore->GetGUI ()->CreateProgressBar ( m_pWindow );
	m_pProgress->SetPosition ( CVector2D ( 0, TRANSFERBOX_YSTART ) );
	m_pProgress->SetSize ( CVector2D ( TRANSFERBOX_WIDTH, TRANSFERBOX_HEIGHT - TRANSFERBOX_YSTART - TRANSFERBOX_SPACER ) );

	// create the icons
	for ( unsigned int i = 0; i < TRANSFERBOX_FRAMES; i++ ) {
		SString strIcon ( "cgui\\images\\transferset\\%u.png", i+1 );
		m_pIcon[i] = g_pCore->GetGUI ()->CreateStaticImage ( m_pProgress );
		m_pIcon[i]->SetFrameEnabled ( false );
		m_pIcon[i]->SetPosition ( CVector2D ( TRANSFERBOX_DRAWXSTART, ((TRANSFERBOX_PROGRESSHEIGHT)/2)  - (TRANSFERBOX_ICONSIZE/2) ) );
		m_pIcon[i]->SetSize ( CVector2D ( TRANSFERBOX_ICONSIZE, TRANSFERBOX_ICONSIZE ) );
		m_pIcon[i]->LoadFromFile ( strIcon );
		m_pIcon[i]->SetVisible ( false );
	}
	m_pIcon[0]->SetVisible ( true );

	// stats label
	m_pInfo = g_pCore->GetGUI ()->CreateLabel ( m_pProgress, "" );
    m_pInfo->SetPosition ( CVector2D ( TRANSFERBOX_DRAWXSTART + TRANSFERBOX_ICONSIZE + 4, 0 ) );
    m_pInfo->SetSize ( CVector2D ( TRANSFERBOX_WIDTH, TRANSFERBOX_PROGRESSHEIGHT ) );
    m_pInfo->SetTextColor ( 0, 0, 0 );
    m_pInfo->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );
    m_pInfo->SetText ( "Disconnect to cancel download" );

	// set animation counters
	m_uiVisible = 0;
	m_ulTime = 0;
    m_dTotalSize = 0;
}

CTransferBox::~CTransferBox ( void )
{
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

void CTransferBox::SetInfoSingleDownload ( const char* szFileName, double dDownloadSizeNow )
{
    // Convert to reasonable units
    SString strDownloadSizeNow   = GetDataUnit ( static_cast < unsigned int > ( dDownloadSizeNow ) );
    SString strDownloadSizeTotal = GetDataUnit ( static_cast < unsigned int > ( m_dTotalSize ) );

    SString strBuffer ( "Download Progress: %s of %s", strDownloadSizeNow.c_str (), strDownloadSizeTotal.c_str () );
	m_pWindow->SetText ( strBuffer );

    m_pProgress->SetProgress ( (dDownloadSizeNow / m_dTotalSize) );
}

void CTransferBox::SetInfoMultipleDownload ( double dDownloadSizeNow, double dDownloadSizeTotal, int iDownloadsRemaining, int iDownloadsTotal )
{
    SString strBuffer ( "Download Progress: %.2fMB of %.2fMB", (float) ( dDownloadSizeNow / 1048576.0 ), (float) ( dDownloadSizeTotal / 1048576.0 ) );
	m_pWindow->SetText ( strBuffer );

    m_pProgress->SetProgress ( (dDownloadSizeNow / dDownloadSizeTotal) );
}

void CTransferBox::DoPulse ( void )
{
	// animated icon mechanism
	if ( timeGetTime () > ( m_ulTime + TRANSFERBOX_DELAY ) ) {
		m_ulTime = timeGetTime ();
		m_pIcon[m_uiVisible]->SetVisible ( false );
		m_uiVisible = ( m_uiVisible + 1 ) % TRANSFERBOX_FRAMES;
		m_pIcon[m_uiVisible]->SetVisible ( true );
	}
}


