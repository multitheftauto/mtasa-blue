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

#define TRANSFERBOX_WIDTH		300
#define TRANSFERBOX_HEIGHT		185
#define TRANSFERBOX_ICONSIZE	42
#define TRANSFERBOX_YSTART		35
#define TRANSFERBOX_SPACER		5

CTransferBox::CTransferBox ( void )
{
    char szIcon[MAX_PATH] = { '\0' };
	CVector2D ScreenSize = g_pCore->GetGUI ()->GetResolution ();
	float fFontHeight = g_pCore->GetGUI ()->GetDefaultFont ()->GetFontHeight ();

	// create the window
	m_pWindow = g_pCore->GetGUI ()->CreateWnd ();
	m_pWindow->SetText ( "DOWNLOADING RESOURCES" );
	m_pWindow->SetVisible ( false );
	m_pWindow->SetAlwaysOnTop ( true );
	m_pWindow->SetCloseButtonEnabled ( false );
	m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetPosition ( CVector2D ( ScreenSize.fX*0.5f - TRANSFERBOX_WIDTH*0.5f, ScreenSize.fY*0.5f - TRANSFERBOX_HEIGHT*0.5f ) );
    m_pWindow->SetSize ( CVector2D ( TRANSFERBOX_WIDTH, TRANSFERBOX_HEIGHT ) );		// relative 0.35, 0.225

	// create the icons
	for ( unsigned int i = 0; i < TRANSFERBOX_FRAMES; i++ ) {
		_snprintf ( szIcon, MAX_PATH, "cgui\\images\\transferset\\%u.png", i+1 );
		szIcon[MAX_PATH-1] = '\0';
		m_pIcon[i] = g_pCore->GetGUI ()->CreateStaticImage ( m_pWindow );
		m_pIcon[i]->SetFrameEnabled ( false );
		m_pIcon[i]->SetPosition ( CVector2D ( 20, TRANSFERBOX_YSTART ) );
		m_pIcon[i]->SetSize ( CVector2D ( TRANSFERBOX_ICONSIZE, TRANSFERBOX_ICONSIZE ) );
		m_pIcon[i]->LoadFromFile ( szIcon );
		m_pIcon[i]->SetVisible ( false );
	}
	m_pIcon[0]->SetVisible ( true );

	// description label
	m_pL1 = g_pCore->GetGUI ()->CreateLabel ( m_pWindow, "Currently downloading:" );
    m_pL1->SetPosition ( CVector2D ( 72, TRANSFERBOX_YSTART ) );
    m_pL1->SetSize ( CVector2D ( 208, fFontHeight ) );
    //m_pL1->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );

	// create the files label
    m_pFiles = g_pCore->GetGUI ()->CreateLabel ( m_pWindow, "N/A" );
    m_pFiles->SetPosition ( CVector2D ( 72, TRANSFERBOX_YSTART + fFontHeight + TRANSFERBOX_SPACER ) );
    m_pFiles->SetSize ( CVector2D ( 208, fFontHeight + 3 ) );
    m_pFiles->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
    m_pFiles->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );
	m_pFiles->SetTextColor ( 40, 110, 10 );

	// number-of-files label
	m_pNum = g_pCore->GetGUI ()->CreateLabel ( m_pWindow, "" );
    m_pNum->SetPosition ( CVector2D ( 20, TRANSFERBOX_YSTART + TRANSFERBOX_ICONSIZE + TRANSFERBOX_SPACER ) );
    m_pNum->SetSize ( CVector2D ( TRANSFERBOX_WIDTH - 40 , fFontHeight ) );

	// stats label
	m_pStats = g_pCore->GetGUI ()->CreateLabel ( m_pWindow, "" );
    m_pStats->SetPosition ( CVector2D ( 20, TRANSFERBOX_YSTART + TRANSFERBOX_ICONSIZE + TRANSFERBOX_SPACER + fFontHeight + TRANSFERBOX_SPACER ) );
    m_pStats->SetSize ( CVector2D ( TRANSFERBOX_WIDTH - 40, fFontHeight ) );

	// create the progress bar
	m_pProgress = g_pCore->GetGUI ()->CreateProgressBar ( m_pWindow );
	m_pProgress->SetPosition ( CVector2D ( 20, TRANSFERBOX_YSTART + TRANSFERBOX_ICONSIZE + TRANSFERBOX_SPACER + fFontHeight * 2 + TRANSFERBOX_SPACER * 2 + TRANSFERBOX_SPACER ) );
	m_pProgress->SetSize ( CVector2D ( TRANSFERBOX_WIDTH - 40, 16 ) );

    m_pButtonCancel = g_pCore->GetGUI ()->CreateButton ( m_pWindow, "Cancel" );
    m_pButtonCancel->SetPosition ( CVector2D ( (TRANSFERBOX_WIDTH / 2) - 50, TRANSFERBOX_HEIGHT - TRANSFERBOX_SPACER - 25 ) );
    m_pButtonCancel->SetSize ( CVector2D ( 100, 25 ) );

    m_pButtonCancel->SetOnClickHandler ( GUI_CALLBACK ( CTransferBox::OnCancelClick, this ) );

	// set animation counters
	m_uiVisible = 0;
	m_ulTime = 0;
    m_dTotalSize = 0;
}

CTransferBox::~CTransferBox ( void )
{
    if ( m_pButtonCancel != NULL )
        delete m_pButtonCancel;
	if ( m_pWindow != NULL )
		delete m_pWindow;
	if ( m_pFiles != NULL )
		delete m_pFiles;
	if ( m_pStats != NULL )
		delete m_pStats;
	if ( m_pNum != NULL )
		delete m_pNum;
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
    char szBuffer[64] = {0};

    m_pFiles->SetText ( szFileName );

    // Convert to reasonable units
    char szDownloadSizeNow [64];
    char szDownloadSizeTotal [64];
    GetDataUnit ( static_cast < unsigned int > ( dDownloadSizeNow ), szDownloadSizeNow );
    GetDataUnit ( static_cast < unsigned int > ( m_dTotalSize ), szDownloadSizeTotal );

    _snprintf ( szBuffer, 64, "Download Progress: %s of %s", szDownloadSizeNow, szDownloadSizeTotal );
	szBuffer[63] = '\0';
	m_pNum->SetText ( szBuffer );

    m_pProgress->SetProgress ( (dDownloadSizeNow / m_dTotalSize) );
}

void CTransferBox::SetInfoMultipleDownload ( double dDownloadSizeNow, double dDownloadSizeTotal, int iDownloadsRemaining, int iDownloadsTotal )
{
    char szBuffer[64] = {0};

	_snprintf ( szBuffer, 64, "Files Remaining: %d of %d", iDownloadsRemaining, iDownloadsTotal );
	szBuffer[63] = '\0';
	m_pNum->SetText ( szBuffer );

    _snprintf ( szBuffer, 64, "Download Progress: %.2fMB of %.2fMB", (float) ( dDownloadSizeNow / 1048576.0 ), (float) ( dDownloadSizeTotal / 1048576.0 ) );
	szBuffer[63] = '\0';
	m_pStats->SetText ( szBuffer );

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

bool CTransferBox::OnCancelClick ( CGUIElement* pElement )
{
    g_pCore->GetModManager ()->RequestUnload ();
    //g_pCore->Quit ();

    return true;
}

