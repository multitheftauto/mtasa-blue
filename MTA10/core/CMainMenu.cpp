/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMainMenu.cpp
*  PURPOSE:     2D Main menu graphical user interface
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include "CNewsBrowser.h"

#define NATIVE_RES_X    1280.0f
#define NATIVE_RES_Y    1024.0f

#define NATIVE_BG_X     1280.0f
#define NATIVE_BG_Y     649.0f

#define NATIVE_LOGO_X     1058.0f
#define NATIVE_LOGO_Y     540.0f

#define CORE_MTA_MENUITEMS_START_X  0.168

#define CORE_MTA_BG_MAX_ALPHA       1.00f   //ACHTUNG: Set to 1 for now due to GTA main menu showing through (no delay inserted between Entering game... and loading screen)
#define CORE_MTA_BG_INGAME_ALPHA    0.90f
#define CORE_MTA_FADER              0.05f // 1/20
#define CORE_MTA_FADER_CREDITS      0.01f

#define CORE_MTA_HOVER_SCALE        1.0f
#define CORE_MTA_NORMAL_SCALE       0.6f
#define CORE_MTA_HOVER_ALPHA        1.0f
#define CORE_MTA_NORMAL_ALPHA       0.6f

#define CORE_MTA_HIDDEN_ALPHA       0.0f
#define CORE_MTA_DISABLED_ALPHA     0.4f
#define CORE_MTA_ENABLED_ALPHA      1.0f

#define CORE_MTA_ANIMATION_TIME     200
#define CORE_MTA_MOVE_ANIM_TIME     600

#define CORE_MTA_STATIC_BG          "cgui\\images\\background.png"
#define CORE_MTA_LOGO               "cgui\\images\\background_logo.png"
#define CORE_MTA_FILLER             "cgui\\images\\mta_filler.png"
#define CORE_MTA_VERSION            "cgui\\images\\version.png"
#define CORE_MTA_LATEST_NEWS        "cgui\\images\\latest_news.png"

static int WaitForMenu = 0;
static const SColor headlineColors [] = { SColorRGBA ( 233, 234, 106, 255 ), SColorRGBA ( 233/6*4, 234/6*4, 106/6*4, 255 ), SColorRGBA ( 233/7*3, 234/7*3, 106/7*3, 255 ) };

// Improve alignment with magical mystery values!
static const int BODGE_FACTOR_1 = -5;
static const int BODGE_FACTOR_2 = 5;
static const int BODGE_FACTOR_3 = -5;
static const int BODGE_FACTOR_4 = 5;
static const CVector2D BODGE_FACTOR_5 ( 0,-50 );
static const CVector2D BODGE_FACTOR_6 ( 0,100 );


CMainMenu::CMainMenu ( CGUI* pManager )
{
    m_pNewsBrowser = new CNewsBrowser ();

    ulPreviousTick = GetTickCount32 ();
    m_pHoveredItem = NULL;
    m_iMoveStartPos = 0;

    // Initialize
    m_pManager = pManager;
    m_bIsVisible = false;
    m_bIsFullyVisible = false;
    m_bIsIngame = true;
//    m_bIsInSubWindow = false;
    m_bStarted = false;
    m_fFader = 0;
    m_ucFade = FADE_INVISIBLE;

    // Adjust window size to resolution
    CVector2D ScreenSize = m_pManager->GetResolution ();
    m_ScreenSize = ScreenSize;

    int iBackgroundX = 0; int iBackgroundY = 0;
    int iBackgroundSizeX = ScreenSize.fX; int iBackgroundSizeY;

    // First let's work out our x and y offsets
    if ( ScreenSize.fX > ScreenSize.fY ) //If the monitor is a normal landscape one
    {
		float iRatioSizeY = ScreenSize.fY / NATIVE_RES_Y;
		m_iMenuSizeX = NATIVE_RES_X * iRatioSizeY;
		m_iMenuSizeY = ScreenSize.fY;
		m_iXOff = (ScreenSize.fX - m_iMenuSizeX)*0.5f;
		m_iYOff = 0;

        float iRatioSizeX = ScreenSize.fX / NATIVE_RES_X;
        iBackgroundSizeX = ScreenSize.fX;
        iBackgroundSizeY = NATIVE_BG_Y * iRatioSizeX;
    }
    else //Otherwise our monitor is in a portrait resolution, so we cant fill the background by y
    {
		float iRatioSizeX = ScreenSize.fX / NATIVE_RES_X;
		m_iMenuSizeY = NATIVE_RES_Y * iRatioSizeX;
		m_iMenuSizeX = ScreenSize.fX;
		m_iXOff = 0;
		m_iYOff = (ScreenSize.fY - m_iMenuSizeY)*0.5f;

        iBackgroundY = m_iYOff;
        iBackgroundSizeX = m_iMenuSizeX;
        iBackgroundSizeY = NATIVE_BG_Y * iRatioSizeX;
    }
    // First create our filler black background image, which covers the whole screen
    m_pFiller = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pFiller->LoadFromFile ( CORE_MTA_FILLER );
    m_pFiller->SetVisible ( false );
    m_pFiller->MoveToBack ();
    m_pFiller->SetZOrderingEnabled ( false );
    m_pFiller->SetAlwaysOnTop ( true );
    m_pFiller->MoveToBack ();
    m_pFiller->SetSize(CVector2D(ScreenSize.fX,iBackgroundY),false);

    // Background image
    m_pBackground = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pBackground->LoadFromFile ( CORE_MTA_STATIC_BG );
    m_pBackground->SetProperty("InheritsAlpha", "False" );
    m_pBackground->SetPosition ( CVector2D(iBackgroundX,iBackgroundY), false);   
    m_pBackground->SetSize ( CVector2D(iBackgroundSizeX,iBackgroundSizeY), false);
    m_pBackground->SetZOrderingEnabled ( false );
    m_pBackground->SetAlwaysOnTop ( true );
    m_pBackground->MoveToBack ();
    m_pBackground->SetAlpha(0);
    m_pBackground->SetVisible ( false );

    m_pFiller2 = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pFiller2->LoadFromFile ( CORE_MTA_FILLER );
    m_pFiller2->SetVisible ( false );
    m_pFiller2->SetZOrderingEnabled ( false );
    m_pFiller2->SetAlwaysOnTop ( true );
    m_pFiller2->MoveToBack ();
    m_pFiller2->SetPosition(CVector2D(0,iBackgroundY + iBackgroundSizeY));
    m_pFiller2->SetSize(ScreenSize,false);

    m_pCanvas = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane () ); 
    m_pCanvas->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pCanvas->SetPosition ( CVector2D(m_iXOff,m_iYOff), false);
    m_pCanvas->SetSize ( CVector2D(m_iMenuSizeX,m_iMenuSizeY), false);
    m_pCanvas->SetZOrderingEnabled ( false );
    m_pCanvas->SetAlwaysOnTop ( true );
    m_pCanvas->MoveToBack ();
    m_pCanvas->SetVisible (false);

    // Create our MTA logo
    CVector2D logoSize = CVector2D( (NATIVE_LOGO_X/NATIVE_RES_X)*m_iMenuSizeX, (NATIVE_LOGO_Y/NATIVE_RES_Y)*m_iMenuSizeY );
    m_pLogo = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage ( m_pCanvas ) );
    m_pLogo->LoadFromFile ( CORE_MTA_LOGO );
    m_pLogo->SetProperty("InheritsAlpha", "False" );
    m_pLogo->SetSize ( logoSize, false);
    m_pLogo->SetPosition ( CVector2D( 0.5f*m_iMenuSizeX - logoSize.fX/2, 0.365f*m_iMenuSizeY - logoSize.fY/2 ), false );
    m_pLogo->SetZOrderingEnabled ( false );

    // Create the image showing the version number
    m_pVersion = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pVersion->LoadFromFile ( CORE_MTA_VERSION );
    m_pVersion->SetParent ( m_pCanvas );
    m_pVersion->SetPosition ( CVector2D(0.845f,0.528f), true);
    m_pVersion->SetSize ( CVector2D((32/NATIVE_RES_X)*m_iMenuSizeX,(32/NATIVE_RES_Y)*m_iMenuSizeY), false);
    m_pVersion->SetProperty("InheritsAlpha", "False" );

    m_pCommunityLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pFiller, "Not logged in" ) );
    m_pCommunityLabel->AutoSize ( "Not logged in" );
    m_pCommunityLabel->SetAlwaysOnTop ( true );
    m_pCommunityLabel->SetAlpha ( 0 ); // disabled, previous alpha was 0.7f
    m_pCommunityLabel->SetVisible ( false );
    m_pCommunityLabel->SetPosition ( CVector2D ( 40.0f, ScreenSize.fY - 20.0f ) );

    std::string strUsername;
    CCore::GetSingleton().GetCommunity()->GetUsername ( strUsername );
    if ( CCore::GetSingleton().GetCommunity()->IsLoggedIn() && !strUsername.empty() )
        ChangeCommunityState ( true, strUsername );

    float fBase = 0.613f;
    float fGap = 0.043f;
    // Our disconnect item is shown/hidden dynamically, so we store it seperately
    m_pDisconnect = CreateItem ( MENU_ITEM_DISCONNECT, "menu_disconnect.png",    CVector2D ( 0.168f, fBase + fGap * 0 ) );
    m_pDisconnect->image->SetVisible(false);

    // Create the menu items
    //Filepath, Relative position, absolute native size
    // And the font for the graphics is ?
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_QUICK_CONNECT,  "menu_quick_connect.png",    CVector2D ( 0.168f, fBase + fGap * 0 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_BROWSE_SERVERS, "menu_browse_servers.png",   CVector2D ( 0.168f, fBase + fGap * 1 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_HOST_GAME,      "menu_host_game.png",        CVector2D ( 0.168f, fBase + fGap * 2 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_MAP_EDITOR,     "menu_map_editor.png",       CVector2D ( 0.168f, fBase + fGap * 3 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_SETTINGS,       "menu_settings.png",         CVector2D ( 0.168f, fBase + fGap * 4 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_ABOUT,          "menu_about.png",            CVector2D ( 0.168f, fBase + fGap * 5 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_QUIT,           "menu_quit.png",             CVector2D ( 0.168f, fBase + fGap * 6 ) ) );

    // We store the position of the top item, and the second item.  These will be useful later
    float fFirstItemSize = m_menuItems.front()->image->GetSize(false).fY;
    float fSecondItemSize = m_menuItems[1]->image->GetSize(false).fY;

    m_iFirstItemCentre  = (m_menuItems.front()->image)->GetPosition().fY + fFirstItemSize*0.5f;
    m_iSecondItemCentre = (m_menuItems[1]->image)->GetPosition().fY + fSecondItemSize*0.5f;

    // Store some mouse over bounding box positions
    m_menuAX = (0.168f*m_iMenuSizeX) + m_iXOff;             //Left side of the items
    m_menuAY = m_iFirstItemCentre - fFirstItemSize*(CORE_MTA_HOVER_SCALE/CORE_MTA_NORMAL_SCALE)*0.5f;     //Top side of the items
	m_menuBX =  m_menuAX + ((390/NATIVE_RES_X)*m_iMenuSizeX); //Right side of the items. We add the longest picture (browse_servers)
	m_menuAY += BODGE_FACTOR_1;

    m_pMenuArea = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage(m_pCanvas) );
    m_pMenuArea->LoadFromFile ( CORE_MTA_FILLER );
    m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff) + BODGE_FACTOR_5 , false);
    m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY) + BODGE_FACTOR_6, false);
    m_pMenuArea->SetAlpha(0);
    m_pMenuArea->SetZOrderingEnabled(false);
    m_pMenuArea->SetClickHandler ( GUI_CALLBACK ( &CMainMenu::OnMenuClick, this ) );
    m_pMenuArea->SetMouseEnterHandler ( GUI_CALLBACK ( &CMainMenu::OnMenuEnter, this ) );
    m_pMenuArea->SetMouseLeaveHandler ( GUI_CALLBACK ( &CMainMenu::OnMenuExit, this ) );

    float fDrawSizeX = (365/NATIVE_RES_X)*m_iMenuSizeX; //Right aligned
    float fDrawSizeY = (52/NATIVE_RES_Y)*m_iMenuSizeY;
    float fDrawPosX = 0.83f*m_iMenuSizeX - fDrawSizeX;
    float fDrawPosY = 0.60f*m_iMenuSizeY;
    m_pLatestNews = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pLatestNews->LoadFromFile ( CORE_MTA_LATEST_NEWS );
    m_pLatestNews->SetParent ( m_pCanvas );
    m_pLatestNews->SetPosition ( CVector2D(fDrawPosX,fDrawPosY), false);
    m_pLatestNews->SetSize ( CVector2D(fDrawSizeX,fDrawSizeY), false);
    m_pLatestNews->SetProperty("InheritsAlpha", "False" );
    m_pLatestNews->SetVisible(false);

    // Create news item stuff
    fDrawPosX -= 25;
    fDrawPosY += fDrawSizeY - 8;
    for ( uint i = 0 ; i < CORE_MTA_NEWS_ITEMS ; i++ )
    {
        fDrawPosY += 20;
        // Create our shadow and item
        CGUILabel * pItemShadow = reinterpret_cast < CGUILabel* > ( m_pManager->CreateLabel ( m_pCanvas, " " ) );
        CGUILabel * pItem = reinterpret_cast < CGUILabel* > ( m_pManager->CreateLabel ( m_pCanvas, " " ) );

        pItem->SetFont ( "sans" );
        pItemShadow->SetFont ( "sans" );
        pItem->SetHorizontalAlign ( CGUI_ALIGN_RIGHT );
        pItemShadow->SetHorizontalAlign ( CGUI_ALIGN_RIGHT );

        pItem->SetSize( CVector2D (fDrawSizeX, 14), false );
        pItemShadow->SetSize( CVector2D (fDrawSizeX, 15), false );

        pItem->SetPosition( CVector2D (fDrawPosX, fDrawPosY), false );
        pItemShadow->SetPosition( CVector2D (fDrawPosX + 1, fDrawPosY + 1), false );

        pItemShadow->SetTextColor ( 112, 112, 112 );

        // Set the handlers
        pItem->SetClickHandler ( GUI_CALLBACK ( &CMainMenu::OnNewsButtonClick, this )  );

        // Store the item in the array
        m_pNewsItemLabels[i] = pItem;
        m_pNewsItemShadowLabels[i] = pItemShadow;

        // Create our date label
        fDrawPosY += 15;
        CGUILabel * pItemDate = reinterpret_cast < CGUILabel* > ( m_pManager->CreateLabel ( m_pCanvas, " " ) );

        pItemDate->SetFont ( "default-small" );
        pItemDate->SetHorizontalAlign ( CGUI_ALIGN_RIGHT );

        pItemDate->SetSize( CVector2D (fDrawSizeX, 13), false );
        pItemDate->SetPosition( CVector2D (fDrawPosX, fDrawPosY), false );

        m_pNewsItemDateLabels[i] = pItemDate;

        // Create 'NEW' sticker
        CGUILabel*& pLabel =  m_pNewsItemNEWLabels[i];
        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pCanvas, "NEW" ) );
        pLabel->SetFont ( "default-small" );
        pLabel->SetTextColor ( 255, 0, 0 );
        pLabel->AutoSize ( pLabel->GetText ().c_str () );
        pLabel->SetAlpha ( 0.7f );
        pLabel->SetVisible ( false );
    }

    m_pLogo->MoveToBack ();

    // Submenus
    m_QuickConnect.SetVisible ( false );
    m_ServerBrowser.SetVisible ( false );
    m_ServerInfo.Hide ( );
    m_Settings.SetVisible ( false );
    m_Credits.SetVisible ( false );
    m_pNewsBrowser->SetVisible ( false );

    // We're not ingame
    SetIsIngame ( false );

    // Store the pointer to the graphics subsystem
    m_pGraphics = CGraphics::GetSingletonPtr ();

    // Load the server lists
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    m_ServerBrowser.LoadServerList ( pConfig->FindSubNode ( CONFIG_NODE_SERVER_FAV ),
        CONFIG_FAVOURITE_LIST_TAG, m_ServerBrowser.GetFavouritesList () );
    m_ServerBrowser.LoadServerList ( pConfig->FindSubNode ( CONFIG_NODE_SERVER_REC ),
        CONFIG_RECENT_LIST_TAG, m_ServerBrowser.GetRecentList () );
    m_ServerBrowser.LoadServerList ( pConfig->FindSubNode ( CONFIG_NODE_SERVER_HISTORY ),
        CONFIG_HISTORY_LIST_TAG, m_ServerBrowser.GetHistoryList () );

    // Remove unused node
    if ( CXMLNode* pOldNode = pConfig->FindSubNode ( CONFIG_NODE_SERVER_INT ) )
        pConfig->DeleteSubNode ( pOldNode );
}


CMainMenu::~CMainMenu ( void )
{
    // Destroy GUI items
    delete m_pBackground;   
    delete m_pCanvas;  
    delete m_pFiller;
    delete m_pFiller2;
    delete m_pLogo;
    delete m_pLatestNews;
    delete m_pVersion;
    delete m_pMenuArea;

    // Destroy community label
    delete m_pCommunityLabel;

    // Destroy the menu items. Note: The disconnect item isn't always in the
    // list of menu items (it's only in there when we're in game). This means we
    // don't delete it when we iterate the list and delete it separately - the
    // menu item itself still exists even when it's no in the list of menu
    // items. Perhaps there should be a separate list of loaded items really.
    for ( std::deque<sMenuItem*>::iterator it = m_menuItems.begin(); it != m_menuItems.end(); ++it )
    {   
        if ( (*it) != m_pDisconnect )
        {
            delete (*it)->image;
            delete (*it);
        }   
    }

    delete m_pDisconnect->image;
    delete m_pDisconnect;
}

void CMainMenu::SetMenuVerticalPosition ( int iPosY )
{
	if ( m_pHoveredItem )
    {
        m_unhoveredItems.insert ( m_pHoveredItem );
        m_pHoveredItem = NULL;
    }

    float fFirstItemSize = m_menuItems.front()->image->GetSize(false).fY;
    int iMoveY = iPosY - m_menuItems.front()->image->GetPosition(false).fY - fFirstItemSize*0.5f;
    
    std::deque<sMenuItem*>::iterator it = m_menuItems.begin();
    for ( it; it != m_menuItems.end(); it++ )
    {
        CVector2D vOrigPos = (*it)->image->GetPosition(false);
        (*it)->drawPositionY = (*it)->drawPositionY + iMoveY;
        (*it)->image->SetPosition( CVector2D(vOrigPos.fX,vOrigPos.fY + iMoveY), false);
    }
	
    m_menuAY = m_menuAY + iMoveY;
	m_menuBY = m_menuBY + iMoveY;
    m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff) + BODGE_FACTOR_5 , false);
    m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY) + BODGE_FACTOR_6, false);
}

void CMainMenu::SetMenuUnhovered () //Dehighlight all our items
{
    if ( m_bIsIngame )  //CEGUI hack
    {
        float fAlpha = m_pDisconnect->image->GetAlpha();
        m_pDisconnect->image->SetAlpha(0.35f);
        m_pDisconnect->image->SetAlpha(fAlpha);
        SetItemHoverProgress ( m_pDisconnect, 0, false );
    }
    m_pHoveredItem = NULL;
    std::deque<sMenuItem*>::iterator it = m_menuItems.begin();
    for ( it; it != m_menuItems.end(); it++ )
    {
        SetItemHoverProgress ( (*it), 0, false );
    }
}

void CMainMenu::Update ( void )
{
    if ( g_pCore->GetDiagnosticDebug () == EDiagnosticDebug::JOYSTICK_0000 )
    {
        m_pFiller->SetVisible(false);
        m_pFiller2->SetVisible(false);
        m_pBackground->SetVisible(false);
        m_bHideGame = false;
    }

    if ( m_bFrameDelay )
    {
        m_bFrameDelay = false;
        return;
    }

    // Get the game interface and the system state
    CGame* pGame = CCore::GetSingleton ().GetGame ();
    eSystemState SystemState = pGame->GetSystemState ();

    m_Credits.Update ();
    m_Settings.Update ();

    unsigned long ulCurrentTick = GetTickCount32();
    unsigned long ulTimePassed = ulCurrentTick - ulPreviousTick;

    if ( m_bHideGame )
        m_pGraphics->DrawRectangle(0,0,m_ScreenSize.fX,m_ScreenSize.fY,0xFF000000);

    if ( m_bIsIngame )  //CEGUI hack
    {
        float fAlpha = m_pDisconnect->image->GetAlpha();
        m_pDisconnect->image->SetAlpha(0.35f);
        m_pDisconnect->image->SetAlpha(fAlpha);
    }

    if ( m_bIsFullyVisible )
    {
        // Grab our cursor position
        tagPOINT cursor;
        GetCursorPos ( &cursor );
        
        HWND hookedWindow = CCore::GetSingleton().GetHookedWindow();

        tagPOINT windowPos = { 0 };
        ClientToScreen( hookedWindow, &windowPos );

        CVector2D vecResolution = CCore::GetSingleton ().GetGUI ()->GetResolution ();
        cursor.x -= windowPos.x;
        cursor.y -= windowPos.y;
        if ( cursor.x < 0 )
            cursor.x = 0;
        else if ( cursor.x > ( long ) vecResolution.fX )
            cursor.x = ( long ) vecResolution.fX;
        if ( cursor.y < 0 )
            cursor.y = 0;
        else if ( cursor.y > ( long ) vecResolution.fY )
            cursor.y = ( long ) vecResolution.fY;

        // If we're within our highlight bounding box
        if ( m_bMouseOverMenu && ( cursor.x > m_menuAX ) && ( cursor.y > m_menuAY + BODGE_FACTOR_3 ) && ( cursor.x < m_menuBX ) && ( cursor.y < m_menuBY + BODGE_FACTOR_4 ) )
        {
            float fHoveredIndex = ((cursor.y-m_menuAY) / (float)(m_menuBY-m_menuAY) * m_menuItems.size());
            fHoveredIndex = Clamp <float> ( 0, fHoveredIndex, m_menuItems.size()-1 );
            sMenuItem* pItem = m_menuItems[(int)floor(fHoveredIndex)];
            int iSizeX = (pItem->nativeSizeX/NATIVE_RES_X)*m_iMenuSizeX;
            
            if (cursor.x < (iSizeX + m_menuAX) )
            {
                if ( ( m_pHoveredItem ) && ( m_pHoveredItem != pItem ) )
                {
                    m_unhoveredItems.insert(m_pHoveredItem);
                    m_pHoveredItem = pItem;
                }
                else
                {
                    m_pHoveredItem = NULL;
                }
                m_pHoveredItem = pItem;
            }
            else if ( m_pHoveredItem )
            {         
                m_unhoveredItems.insert(m_pHoveredItem);
                m_pHoveredItem = NULL;
            }
            
            if ( m_pHoveredItem )
            {
                float fProgress = (m_pHoveredItem->image->GetAlpha()-CORE_MTA_NORMAL_ALPHA)/(CORE_MTA_HOVER_ALPHA - CORE_MTA_NORMAL_ALPHA);
			    // Let's work out what the target progress should be by working out the time passed
			    fProgress = ((float)ulTimePassed/CORE_MTA_ANIMATION_TIME)*(CORE_MTA_HOVER_ALPHA-CORE_MTA_NORMAL_ALPHA) + fProgress;
                MapRemove ( m_unhoveredItems, m_pHoveredItem );
                SetItemHoverProgress ( m_pHoveredItem, fProgress, true );
            }

        }
        else if ( m_pHoveredItem )
        {
            m_unhoveredItems.insert(m_pHoveredItem);
            m_pHoveredItem = NULL;
        }

        // Let's unhover our recently un-moused over items
        std::set<sMenuItem*>::iterator it = m_unhoveredItems.begin();
        while (it != m_unhoveredItems.end())
        {
            float fProgress = ((*it)->image->GetAlpha()-CORE_MTA_NORMAL_ALPHA)/(CORE_MTA_HOVER_ALPHA - CORE_MTA_NORMAL_ALPHA);
		    // Let's work out what the target progress should be by working out the time passed
            // Min of 0.5 progress fixes occasional graphical glitchekal
		    fProgress = fProgress - Min ( 0.5f, ((float)ulTimePassed/CORE_MTA_ANIMATION_TIME)*(CORE_MTA_HOVER_ALPHA-CORE_MTA_NORMAL_ALPHA) );
            if ( SetItemHoverProgress ( (*it), fProgress, false ) )
            {
                std::set<sMenuItem*>::iterator itToErase = it++;
                m_unhoveredItems.erase(itToErase);
            }
            else
                it++;
        }
    }

    if ( m_iMoveStartPos )
    {
        float fTickDifference = ulCurrentTick - m_ulMoveStartTick;
        float fMoveTime = (fTickDifference/CORE_MTA_MOVE_ANIM_TIME);
        fMoveTime = Clamp <float> ( 0, fMoveTime, 1 );
        // Use OutQuad easing to smoothen the movement
        fMoveTime = -fMoveTime*(fMoveTime-2);

        SetMenuVerticalPosition ( fMoveTime*(m_iMoveTargetPos - m_iMoveStartPos) + m_iMoveStartPos );
        m_pDisconnect->image->SetAlpha ( m_bIsIngame ? fMoveTime*CORE_MTA_NORMAL_ALPHA : (1-fMoveTime)*CORE_MTA_NORMAL_ALPHA );

        if ( fMoveTime == 1 )
        {
            m_iMoveStartPos = 0;
            if ( !m_bIsIngame )
                m_pDisconnect->image->SetVisible(false);
            else
            {
                m_menuItems.push_front(m_pDisconnect);

                m_pDisconnect->image->SetVisible(true);

                float fTopItemSize = m_pDisconnect->image->GetSize(false).fY;
                float fTopItemCentre = m_pDisconnect->image->GetPosition(false).fY + fTopItemSize*0.5f;
                m_menuAY = fTopItemCentre - fTopItemSize*(CORE_MTA_HOVER_SCALE/CORE_MTA_NORMAL_SCALE)*0.5f;     //Top side of the items
        		m_menuAY += BODGE_FACTOR_1;

                m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff) + BODGE_FACTOR_5 , false);
                m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY) + BODGE_FACTOR_6, false);
            }
        }
    }

    // Fade in
    if ( m_ucFade == FADE_IN ) {

        // Increment the fader (use the other define if we're fading to the credits)
        m_fFader += CORE_MTA_FADER;

        float fFadeTarget = m_bIsIngame ? CORE_MTA_BG_INGAME_ALPHA : CORE_MTA_BG_MAX_ALPHA;

        m_pFiller->SetAlpha ( Clamp <float> ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pFiller2->SetAlpha ( Clamp <float> ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pCanvas->SetAlpha ( Clamp <float> ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pBackground->SetAlpha ( Clamp <float> ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );

        if ( m_fFader > 0.0f )
        {
            m_bIsVisible = true;     // Make cursor appear faster
        }

        // If the fade is complete
        if ( m_fFader >= fFadeTarget ) {
            m_ucFade = FADE_VISIBLE;
            m_bIsVisible = true;
            m_bIsFullyVisible = true;
        }
    }
    // Fade out
    else if ( m_ucFade == FADE_OUT ) {
        m_fFader -= CORE_MTA_FADER;

        m_pFiller->SetAlpha ( Clamp ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pFiller2->SetAlpha ( Clamp ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pCanvas->SetAlpha ( Clamp ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pBackground->SetAlpha ( Clamp ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );

        if ( m_fFader < 1.0f )
            m_bIsVisible = false;    // Make cursor disappear faster

        // If the fade is complete
        if ( m_fFader <= 0 ) {
            m_bIsFullyVisible = false;
            m_ucFade = FADE_INVISIBLE;
            m_bIsVisible = false;

            // Turn the widgets invisible
            m_pFiller->SetVisible ( false );
            m_pFiller2->SetVisible ( false );
            m_pCanvas->SetVisible(false);
            m_pBackground->SetVisible(false);
        }
    }

    // Force the mainmenu on if we're at GTA's mainmenu or not ingame
    if ( ( SystemState == 7 || SystemState == 9 ) && !m_bIsIngame )
    {
        // Cope with early finish
        if ( pGame->HasCreditScreenFadedOut () )
            WaitForMenu = Max ( WaitForMenu, 250 );

        // Fade up
        if ( WaitForMenu >= 250 )
        {
            m_bIsVisible = true;
            m_bStarted = true;
        }

        // Create headlines while the screen is still black
        if ( WaitForMenu == 250 )
            m_pNewsBrowser->CreateHeadlines ();

        // Start updater after fade up is complete
        if ( WaitForMenu == 275 )
            GetVersionUpdater ()->EnableChecking ( true );

        if ( WaitForMenu < 300 )
            WaitForMenu++;
    }

    // If we're visible
    if ( m_bIsVisible && SystemState != 8 )
    {
        // If we're at the game's mainmenu, or ingame when m_bIsIngame is true show the background
        if ( SystemState == 7 ||                    // GS_FRONTEND
             SystemState == 9 && !m_bIsIngame )     // GS_PLAYING_GAME
        {
            if ( m_ucFade == FADE_INVISIBLE )
                Show ( false );
        }
        else
        {
            if ( m_ucFade == FADE_INVISIBLE )
                Show ( true );
        }
    }
    else
    {
        if ( m_ucFade == FADE_VISIBLE )
            Hide ();
    }


    ulPreviousTick = GetTickCount32();

    // Call subdialog pulses
    m_ServerBrowser.Update ();
    m_ServerInfo.DoPulse ();
}


void CMainMenu::Show ( bool bOverlay )
{
    SetVisible ( true, bOverlay );
}


void CMainMenu::Hide ( void )
{
    SetVisible ( false );
}

// When escape key pressed and not connected, hide these windows
void CMainMenu::OnEscapePressedOffLine ( void )
{
    m_ServerBrowser.SetVisible ( false );
    m_Credits.SetVisible ( false );
    m_pNewsBrowser->SetVisible ( false );
}

void CMainMenu::SetVisible ( bool bVisible, bool bOverlay, bool bFrameDelay )
{
    CMultiplayer* pMultiplayer = CCore::GetSingleton ().GetMultiplayer ();
    pMultiplayer->DisablePadHandler ( bVisible );

    if ( ( m_ucFade == FADE_VISIBLE || m_ucFade == FADE_IN ) && bVisible == false  ) {
        m_ucFade = FADE_OUT;
    } else if ( ( m_ucFade == FADE_INVISIBLE || m_ucFade == FADE_OUT ) && bVisible == true ) {
        m_ucFade = FADE_IN;
    }

    // If we're hiding, hide any subwindows we might've had (prevent escaping hiding mousecursor issue)
    if ( !bVisible )
    {
        m_bFrameDelay = bFrameDelay;
        SetMenuUnhovered ();
        m_QuickConnect.SetVisible ( false );
        m_ServerBrowser.SetVisible ( false );
        m_Settings.SetVisible ( false );
        m_Credits.SetVisible ( false );
        m_pCommunityLabel->SetVisible ( false );
        m_pNewsBrowser->SetVisible ( false );

//        m_bIsInSubWindow = false;
    } else {
        m_bFrameDelay = bFrameDelay;
        SetMenuUnhovered ();
        m_pFiller->SetVisible ( true );
        m_pFiller2->SetVisible ( true );
        m_pCanvas->SetVisible( true );
        m_pBackground->SetVisible ( true );
        m_pCommunityLabel->SetVisible ( true );
    }

    m_bHideGame = !bOverlay;
}


bool CMainMenu::IsVisible ( void )
{
    return m_bIsVisible;
}


void CMainMenu::SetIsIngame ( bool bIsIngame )
{
    // Save some performance by making sure we don't do all this if the new state == old
    if ( m_bIsIngame != bIsIngame )
    {
        m_bIsIngame = bIsIngame;
        m_Settings.SetIsModLoaded ( bIsIngame );

        m_ulMoveStartTick = GetTickCount32();
        if ( bIsIngame )
        {
            m_iMoveTargetPos = m_iSecondItemCentre;
        }
        else
        {
            if ( m_menuItems.front() == m_pDisconnect )
                m_menuItems.pop_front();
            
            float fTopItemSize = m_menuItems.front()->image->GetSize(false).fY;
            float fTopItemCentre = m_menuItems.front()->image->GetPosition(false).fY + fTopItemSize*0.5f;
            m_menuAY = fTopItemCentre - fTopItemSize*(CORE_MTA_HOVER_SCALE/CORE_MTA_NORMAL_SCALE)*0.5f; 
        	m_menuAY += BODGE_FACTOR_1;

            m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff) + BODGE_FACTOR_5 , false);
            m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY) + BODGE_FACTOR_6, false);
            m_iMoveTargetPos = m_iFirstItemCentre;
        }
        m_iMoveStartPos = m_menuAY;
    }
}


bool CMainMenu::GetIsIngame ( void )
{
    return m_bIsIngame;
}

bool CMainMenu::OnMenuEnter ( CGUIElement* pElement )
{
    m_bMouseOverMenu = true;
    return true;
}

bool CMainMenu::OnMenuExit ( CGUIElement* pElement )
{
    m_bMouseOverMenu = false;
    return true;
}

bool CMainMenu::OnMenuClick ( CGUIElement* pElement )
{
    // Handle all our clicks to the menu from here
    if ( m_pHoveredItem )
    {
        // Possible disconnect question for user
        if ( g_pCore->IsConnected() )
        {
            switch (m_pHoveredItem->menuType)
            {
                case MENU_ITEM_HOST_GAME:
                case MENU_ITEM_MAP_EDITOR:
                    AskUserIfHeWantsToDisconnect( m_pHoveredItem->menuType );
                    return true;
                default: break;
            }
        }

        switch (m_pHoveredItem->menuType)
        {
            case MENU_ITEM_DISCONNECT:      OnDisconnectButtonClick   (pElement);    break;
            case MENU_ITEM_QUICK_CONNECT:   OnQuickConnectButtonClick (pElement);    break;
            case MENU_ITEM_BROWSE_SERVERS:  OnBrowseServersButtonClick(pElement);    break;
            case MENU_ITEM_HOST_GAME:       OnHostGameButtonClick     ();            break;
            case MENU_ITEM_MAP_EDITOR:      OnEditorButtonClick       ();            break;
            case MENU_ITEM_SETTINGS:        OnSettingsButtonClick     (pElement);    break;
            case MENU_ITEM_ABOUT:           OnAboutButtonClick        (pElement);    break;
            case MENU_ITEM_QUIT:            OnQuitButtonClick         (pElement);    break;
            default: break;
        }
    }
    return true;
}

bool CMainMenu::OnQuickConnectButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    m_ServerBrowser.SetVisible ( true );
    m_ServerBrowser.OnQuickConnectButtonClick ();
/*
//    if ( !m_bIsInSubWindow )
    {
        m_QuickConnect.SetVisible ( true );
//        m_bIsInSubWindow = true;
    }
*/
    return true;
}


bool CMainMenu::OnResumeButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    SetVisible ( false );
    return true;
}


bool CMainMenu::OnBrowseServersButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

//    if ( !m_bIsInSubWindow )
    {
        m_ServerBrowser.SetVisible ( true );
//        m_bIsInSubWindow = true;
    }

    return true;
}

void CMainMenu::HideServerInfo ( void )
{
    m_ServerInfo.Hide ( );
}


bool CMainMenu::OnDisconnectButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    // Send "disconnect" command to the command handler
    CCommands::GetSingleton ().Execute ( "disconnect", "" );

    return true;
}


bool CMainMenu::OnHostGameButtonClick ( void )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    // Load deathmatch, but with local play
    CModManager::GetSingleton ().RequestLoad ( "deathmatch", "local" );

    return true;
}


bool CMainMenu::OnEditorButtonClick ( void )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    // Load deathmatch, but with local play
    CModManager::GetSingleton ().RequestLoad ( "deathmatch", "editor" );

    return true;
}


bool CMainMenu::OnSettingsButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

//    if ( !m_bIsInSubWindow )
    {
        m_Settings.SetVisible ( true );

        // Jax: not sure why this was commented, need it here if our main-menu is set to AlwaysOnTop
//        m_bIsInSubWindow = true;
    }  

    return true;
}


bool CMainMenu::OnAboutButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    // Determine if we're ingame or if the background is set to static
    // If so, show the old credits dialog
    m_Credits.SetVisible ( true );

    return true;
}


bool CMainMenu::OnQuitButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    // Send "exit" command to the command handler
    CCommands::GetSingleton ().Execute ( "exit", "" );

    return true;
}


bool CMainMenu::OnNewsButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    int iIndex = 0;
    if ( pElement )
    {
        // Calc index from color
        CGUIColor color = dynamic_cast < CGUILabel* > ( pElement )->GetTextColor ();
        for ( uint i = 0 ; i < NUMELMS ( headlineColors ) ; i++ )
            if ( color.R == headlineColors[i].R )
                iIndex = i;
    }

    m_pNewsBrowser->SetVisible ( true );
    m_pNewsBrowser->SwitchToTab ( iIndex );

    return true;
}


sMenuItem* CMainMenu::CreateItem ( unsigned char menuType, const char* szFilename, CVector2D vecRelPosition )
{ 
    CGUIStaticImage* pImage = reinterpret_cast < CGUIStaticImage* > ( m_pManager->CreateStaticImage () );

    if ( g_pCore->GetLocalization()->IsLocalized() )
    {
        if ( !pImage->LoadFromFile ( PathJoin(g_pCore->GetLocalization()->GetLanguageDirectory(),szFilename) ) )
            pImage->LoadFromFile ( PathJoin("cgui/images",szFilename) );
    }
    else
        pImage->LoadFromFile ( PathJoin("cgui/images",szFilename) );
        

    // Make our positions absolute
	int iPosX = vecRelPosition.fX*m_iMenuSizeX;
    int iPosY = vecRelPosition.fY*m_iMenuSizeY;

    // Make our sizes relative to the size of menu, but in absolute coordinates
    CVector2D vecNativeSize;
    pImage->GetNativeSize(vecNativeSize);
	int iSizeX = (vecNativeSize.fX/NATIVE_RES_X)*m_iMenuSizeX;
    int iSizeY = (vecNativeSize.fY/NATIVE_RES_Y)*m_iMenuSizeY;
	
    // Mark our bounding box's bottom value.
	m_menuBY = (iPosY + (iSizeY*CORE_MTA_HOVER_SCALE)/2) + m_iYOff;
	m_menuBY += BODGE_FACTOR_2;

    // Reduced their size down to unhovered size.
	iSizeX = iSizeX*CORE_MTA_NORMAL_SCALE;
    iSizeY = iSizeY*CORE_MTA_NORMAL_SCALE;
	// Grab our draw position from which we enlarge from
    iPosY = iPosY - (iSizeY/2);

    pImage->SetParent ( m_pCanvas );
    pImage->SetPosition ( CVector2D(iPosX,iPosY), false);
    pImage->SetSize ( CVector2D(iSizeX,iSizeY), false);
    pImage->SetProperty("InheritsAlpha", "False" );
    pImage->SetAlpha ( CORE_MTA_NORMAL_ALPHA );

    sMenuItem* s = new sMenuItem();
    s->menuType = menuType;
    s->drawPositionX = vecRelPosition.fX*m_iMenuSizeX;
    s->drawPositionY = vecRelPosition.fY*m_iMenuSizeY;
    s->nativeSizeX = vecNativeSize.fX;
    s->nativeSizeY = vecNativeSize.fY;
    s->image = pImage;
    return s;
}

bool CMainMenu::SetItemHoverProgress ( sMenuItem* pItem, float fProgress, bool bHovering )
{
		fProgress = Clamp <float> ( 0, fProgress, 1 );

        // Use OutQuad equation for easing, or OutQuad for unhovering
        fProgress = bHovering ? -fProgress*(fProgress-2) : fProgress*fProgress;
        
        // Work out the target scale
		float fTargetScale = (CORE_MTA_HOVER_SCALE-CORE_MTA_NORMAL_SCALE)*(fProgress) + CORE_MTA_NORMAL_SCALE;
		
		// Work out our current progress based upon the alpha value now
		pItem->image->SetAlpha ( (CORE_MTA_HOVER_ALPHA-CORE_MTA_NORMAL_ALPHA)*(fProgress) + CORE_MTA_NORMAL_ALPHA );
		
        int iSizeX = (pItem->nativeSizeX/NATIVE_RES_X)*m_iMenuSizeX*fTargetScale;
        int iSizeY = (pItem->nativeSizeY/NATIVE_RES_Y)*m_iMenuSizeY*fTargetScale;

        // Aligned to the left horizontally, aligned to the centre vertically
        int iPosX = pItem->drawPositionX;
        int iPosY = (pItem->drawPositionY) - (iSizeY*0.5);
		
        pItem->image->SetPosition ( CVector2D(iPosX, iPosY), false );
        pItem->image->SetSize ( CVector2D(iSizeX, iSizeY), false );    

        //Return whether the hovering has maxed out
        return bHovering ? (fProgress == 1) : (fProgress == 0);
}

void CMainMenu::ChangeCommunityState ( bool bIn, const std::string& strUsername )
{
    if ( bIn )
    {
        SString strText ( "Logged in as: %s", strUsername.c_str () );

        m_pCommunityLabel->SetText ( strText );
        m_pCommunityLabel->AutoSize ( strText );
        return;
    }
    m_pCommunityLabel->SetText ( "Not logged in" );
    m_pCommunityLabel->AutoSize ( "Not logged in" );
}


void CMainMenu::SetNewsHeadline ( int iIndex, const SString& strHeadline, const SString& strDate, bool bIsNew )
{
    if ( iIndex < 0 || iIndex > 2 )
        return;

    m_pLatestNews->SetVisible(true);

    // Headline
    CGUILabel* pItem = m_pNewsItemLabels[ iIndex ];
    CGUILabel* pItemShadow = m_pNewsItemShadowLabels[ iIndex ];
    SColor color = headlineColors[ iIndex ];
    pItem->SetTextColor ( color.R, color.G, color.B );
    pItem->SetText ( strHeadline );
    pItemShadow->SetText ( strHeadline );

    // Switch font if it's too big
    if ( pItem->GetSize(false).fX < pItem->GetTextExtent() )
    {
        const char* szFontName = "default-bold-small";
        for ( char i=0; i < CORE_MTA_NEWS_ITEMS; i++ )
        {
            // Try default-bold-small first, if that's too big use default-small
            m_pNewsItemLabels[ i ]->SetFont ( szFontName );
            m_pNewsItemShadowLabels[ i ]->SetFont ( szFontName );
            if ( strcmp(szFontName,"default-small") && ( m_pNewsItemLabels[ i ]->GetSize(false).fX < m_pNewsItemLabels[ i ]->GetTextExtent() ) )
            {
                szFontName = "default-small";
                i = -1;
            }
        }
    }

    // Set our Date labels
    CGUILabel* pItemDate = m_pNewsItemDateLabels[ iIndex ];
    pItemDate->SetText ( strDate );

    // 'NEW' sticker
    CGUILabel* pNewLabel = m_pNewsItemNEWLabels[ iIndex ];
    pNewLabel->SetVisible ( bIsNew );
    pNewLabel->SetPosition ( CVector2D ( pItem->GetPosition ().fX + 4, pItem->GetPosition ().fY - 4 ) );
}


/////////////////////////////////////////////////////////////
//
// CMainMenu::AskUserIfHeWantsToDisconnect
//
// Ask user if he wants to disconnect
//
/////////////////////////////////////////////////////////////
void CMainMenu::AskUserIfHeWantsToDisconnect ( uchar menuType )
{
    SStringX strMessage ( _("This will disconnect you from the current server."
                            "\n\nAre you sure you want to disconnect?" ) );
    CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
    pQuestionBox->Reset ();
    pQuestionBox->SetTitle ( _("DISCONNECT WARNING") );
    pQuestionBox->SetMessage ( strMessage );
    pQuestionBox->SetButton ( 0, _("No") );
    pQuestionBox->SetButton ( 1, _("Yes") );
    pQuestionBox->SetCallback ( StaticWantsToDisconnectCallBack, (void*)menuType );
    pQuestionBox->Show ();
}


/////////////////////////////////////////////////////////////
//
// CMainMenu::StaticWantsToDisconnectCallBack
//
// Callback from disconnect question
//
/////////////////////////////////////////////////////////////
void CMainMenu::StaticWantsToDisconnectCallBack( void* pData, uint uiButton )
{
    CLocalGUI::GetSingleton ().GetMainMenu ()->WantsToDisconnectCallBack( pData, uiButton );
}


/////////////////////////////////////////////////////////////
//
// CMainMenu::WantsToDisconnectCallBack
//
// Callback from disconnect question.
// If user says it's ok, continue with menu item selection
//
/////////////////////////////////////////////////////////////
void CMainMenu::WantsToDisconnectCallBack( void* pData, uint uiButton )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();

    if ( uiButton == 1 )
    {
        uchar menuType = (uchar)pData;
        switch( menuType )
        {
            case MENU_ITEM_HOST_GAME:       OnHostGameButtonClick ();    break;
            case MENU_ITEM_MAP_EDITOR:      OnEditorButtonClick   ();    break;
            default: break;
        }
    }
}
