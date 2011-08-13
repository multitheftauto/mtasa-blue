/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CNewsBrowser.cpp
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CNewsBrowser.h"

extern CCore* g_pCore;


////////////////////////////////////////////////////
//
//  CNewsBrowser::CNewsBrowser
//
//
//
////////////////////////////////////////////////////
CNewsBrowser::CNewsBrowser ( void )
{
    m_pWindow = NULL;
    m_pTabPanel = NULL;
    m_pButtonOK = NULL;
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::~CNewsBrowser
//
//
//
////////////////////////////////////////////////////
CNewsBrowser::~CNewsBrowser ( void )
{
    DestroyGUI ();
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::InitNewsItemList
//
//
//
////////////////////////////////////////////////////
void CNewsBrowser::InitNewsItemList ( void )
{
    m_NewsitemList.clear ();

    // Find all sub-directories in 'news' directory
    SString strAllNewsDir = PathJoin ( GetMTALocalAppDataPath (), "news" );
    std::vector < SString > directoryList = FindFiles ( strAllNewsDir + "\\*", false, true );
    std::sort ( directoryList.begin (), directoryList.end () );

    // Get news settings
    SString strOldestPost;
    uint uiMaxHistoryLength;
    GetVersionUpdater ()->GetNewsSettings ( strOldestPost, uiMaxHistoryLength );

    // Process each sub-directory
    for ( uint i = 0; i < directoryList.size () ; i++ )
    {
        SString strItemDir = directoryList[ directoryList.size () - 1 - i ];
        if ( strItemDir < strOldestPost )
            continue;   // Post too old
        if ( m_NewsitemList.size () >= uiMaxHistoryLength )
            continue;   // Post count too high

        SNewsItem newsItem;
        newsItem.strContentFullDir = PathJoin ( strAllNewsDir, strItemDir );

        // Get filenames from XML file
        CXMLFile* pFile = g_pCore->GetXML ()->CreateXML ( PathJoin ( newsItem.strContentFullDir, "files.xml" ) );
        if ( pFile )
        {
            pFile->Parse ();
            CXMLNode* pRoot = pFile->GetRootNode ();

            if ( pRoot )
            {
                // Headline text
                CXMLNode* pHeadline = pRoot->FindSubNode ( "headline", 0 );
                if ( pHeadline )
                    newsItem.strHeadline = pHeadline->GetTagContent ();

                // Date text
                CXMLNode* pDate = pRoot->FindSubNode ( "date", 0 );
                if ( pDate )
                    newsItem.strDate = pDate->GetTagContent ();
                else
                    newsItem.strHeadline.Split ( " - ", &newsItem.strHeadline, &newsItem.strDate, true );

                // Layout filename
                CXMLNode* pLayout = pRoot->FindSubNode ( "layout", 0 );
                if ( pLayout )
                    newsItem.strLayoutFilename = pLayout->GetTagContent ();

                // Imageset filenames
                CXMLNode* pImages = pRoot->FindSubNode ( "imagesetlist", 0 );
                if ( pImages )
                    for ( uint i = 0 ; i < pImages->GetSubNodeCount () ; i++ )
                        newsItem.imagesetFilenameList.push_back ( pImages->GetSubNode ( i )->GetTagContent () );
            }

            delete pFile;
        }

        // Add to news item list if valid
        if ( !newsItem.strHeadline.empty () && !newsItem.strLayoutFilename.empty () )
            m_NewsitemList.push_back ( newsItem );
    }
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::CreateHeadlines
//
//
//
////////////////////////////////////////////////////
void CNewsBrowser::CreateHeadlines ( void )
{
    InitNewsItemList ();
    bool bNewsUpdated = GetApplicationSettingInt ( "news-updated" ) == 1;

    uint i;
    // Process each news item
    for ( i = 0; i < m_NewsitemList.size () ; i++ )
        CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->SetNewsHeadline ( i, m_NewsitemList[i].strHeadline, m_NewsitemList[i].strDate, i < 1 && bNewsUpdated );

    // Clear unused slots
    for ( ; i < 3 ; i++ )
        CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->SetNewsHeadline ( i, "", "", false );
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::CreateGUI
//
//
//
////////////////////////////////////////////////////
void CNewsBrowser::CreateGUI ( void )
{
    CreateHeadlines ();
    CGUI *pManager = g_pCore->GetGUI ();

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "NEWS" ) );
    m_pWindow->SetCloseButtonEnabled ( true );
    m_pWindow->SetMovable ( true );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    float yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 640.0f / 2, resolution.fY / 2 - 480.0f / 2 + yoff ), false );
    m_pWindow->SetSize ( CVector2D ( 640.0f, 480.0f ) );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );

    // Create buttons
    //  OK button
    m_pButtonOK = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "OK" ) );
    m_pButtonOK->SetPosition ( CVector2D ( 560.0f - 60, 480.0f - 30 ) );
    m_pButtonOK->SetZOrderingEnabled ( false );

    // Set up the events
    m_pWindow->SetEnterKeyHandler ( GUI_CALLBACK ( &CNewsBrowser::OnOKButtonClick, this ) );
    m_pButtonOK->SetClickHandler ( GUI_CALLBACK ( &CNewsBrowser::OnOKButtonClick, this ) );

    // Create the tab panel and necessary tabs
    m_pTabPanel = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( m_pWindow ) );
    m_pTabPanel->SetPosition ( CVector2D ( 0, 20.0f ) );
    m_pTabPanel->SetSize ( CVector2D ( 640.0f, 480.0f - 60 ) );

    for ( uint i = 0; i < m_NewsitemList.size () ; i++ )
    {
        AddNewsTab ( m_NewsitemList[i] );
    }
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::DestroyGUI
//
//
//
////////////////////////////////////////////////////
void CNewsBrowser::DestroyGUI ( void )
{
    // Destroy
    for ( uint i = 0 ; i < m_TabList.size () ; i++ )
    {
        CGUITab* pTab = m_TabList[i];
        if ( pTab )
            delete pTab;
    }
    m_TabList.clear ();
    for ( uint i = 0 ; i < m_TabContentList.size () ; i++ )
    {
        CGUIWindow* pWindow = m_TabContentList[i];
        if ( pWindow )
        {
            CGUIElement* m_pScrollPane = pWindow->GetParent ();
            if ( m_pScrollPane )
            {
                delete m_pScrollPane;
            }
            delete pWindow;
        }
    }
    m_TabContentList.clear ();
    SAFE_DELETE ( m_pTabPanel );
    SAFE_DELETE ( m_pButtonOK );
    SAFE_DELETE ( m_pWindow );
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::AddNewsTab
//
//
//
////////////////////////////////////////////////////
void CNewsBrowser::AddNewsTab ( const SNewsItem& newsItem )
{
    CGUI *pManager = g_pCore->GetGUI ();

    CGUITab* pTab = m_pTabPanel->CreateTab ( "News" );
    m_TabList.push_back ( pTab );

    //Create everything under a scrollpane
    CGUIScrollPane* m_pScrollPane = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( pTab ) ); 
    m_pScrollPane->SetProperty ( "ContentPaneAutoSized", "True" );
    m_pScrollPane->SetPosition ( CVector2D ( 3, 3 ), 0 );
    m_pScrollPane->SetSize ( CVector2D ( 618.0f, 390.0f ) );
    m_pScrollPane->SetVerticalScrollStepSize ( 0.15f );
    m_pScrollPane->SetVerticalScrollBar ( true );

    // Switch cwd
    SString cwd = GetCurrentWorkingDirectory ();
    SetCurrentDirectory ( newsItem.strContentFullDir );

    // Load files
    CGUIWindow* pWindow = LoadLayoutAndImages ( m_pScrollPane, newsItem );
    m_TabContentList.push_back ( pWindow );

    // Set tab name from content window title
    if ( pWindow )
    {
        SString strTitle = pWindow->GetText ();
        if ( !strTitle.empty () )
           pTab->SetText ( strTitle );
    }

    // Restore cwd
    SetCurrentDirectory ( cwd );
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::LoadLayoutAndImages
//
//
//
////////////////////////////////////////////////////
CGUIWindow* CNewsBrowser::LoadLayoutAndImages ( CGUIElement* pParent, const SNewsItem& newsItem )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Make sure we have the layout filename
    if ( newsItem.strLayoutFilename.empty () )
    {
        AddReportLog ( 3302, SString ( "CNewsBrowser::LoadLayout: Problem loading %s", *newsItem.strContentFullDir ) );
        return NULL;
    }

    // Load any imagesets
    for ( uint i = 0 ; i < newsItem.imagesetFilenameList.size () ; i++ )
    {
        if ( !pManager->LoadImageset( newsItem.imagesetFilenameList[i] ) )
        {
            AddReportLog ( 3303, SString ( "CNewsBrowser::LoadLayout: Problem with LoadImageset [%s] %s", *newsItem.strContentFullDir, *newsItem.imagesetFilenameList[i] ) );
            return NULL;
        }
    }

    // Load layout
    CGUIWindow* pWindow = pManager->LoadLayout ( pParent, newsItem.strLayoutFilename );
    if ( !pWindow )
    {
        AddReportLog ( 3304, SString ( "CNewsBrowser::LoadLayout: Problem with LoadLayout [%s] %s", *newsItem.strContentFullDir, *newsItem.strLayoutFilename ) );
        return NULL;
    }
    return pWindow;
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::SwitchToTab
//
//
//
////////////////////////////////////////////////////
void CNewsBrowser::SwitchToTab ( int iIndex )
{
    if ( iIndex >= 0 && iIndex < (int)m_TabList.size () )
        m_pTabPanel->SetSelectedTab ( m_TabList[iIndex] );
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::SetVisible
//
//
//
////////////////////////////////////////////////////
void CNewsBrowser::SetVisible ( bool bVisible )
{
    if ( !bVisible && !m_pWindow )
        return;

    // HACK: Recreate GUI if left shift held (for editing)
    if ( ( GetAsyncKeyState ( VK_LSHIFT ) & 0x8000 ) != 0 )
    {
        DestroyGUI ();
        CreateGUI ();
    }

    if ( !m_pWindow )
        CreateGUI ();

    // Hide / show the form
    m_pWindow->SetVisible ( bVisible );

    if ( bVisible )
    {
        // Bring it all forward
        m_pWindow->Activate ();
        m_pWindow->SetAlwaysOnTop ( true );
        m_pWindow->BringToFront ();
        if ( GetApplicationSettingInt ( "news-updated" ) == 1 )
        {
            SetApplicationSettingInt ( "news-updated", 0 );
            CreateHeadlines ();
        }
    }
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::IsVisible
//
//
//
////////////////////////////////////////////////////
bool CNewsBrowser::IsVisible ( void )
{
    return m_pWindow && m_pWindow->IsVisible ();
}


////////////////////////////////////////////////////
//
//  CNewsBrowser::OnOKButtonClick
//
//
//
////////////////////////////////////////////////////
bool CNewsBrowser::OnOKButtonClick ( CGUIElement* pElement )
{
    // Close the window
    m_pWindow->SetVisible ( false );
    return true;
}
