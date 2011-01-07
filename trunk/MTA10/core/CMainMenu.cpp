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
#define CORE_MTA_FILLER             "cgui\\images\\mta_filler.png"
#define CORE_MTA_VERSION            "cgui\\images\\version.png"
#define CORE_MTA_LATEST_NEWS        "cgui\\images\\latest_news.png"

static short WaitForMenu = 0;
static const SColor headlineColors [] = { SColorRGBA ( 233, 234, 106, 255 ), SColorRGBA ( 233/6*4, 234/6*4, 106/6*4, 255 ), SColorRGBA ( 233/7*3, 234/7*3, 106/7*3, 255 ) };

CMainMenu::CMainMenu ( CGUI* pManager )
{
    m_pNewsBrowser = new CNewsBrowser ();

    ulPreviousTick = GetTickCount ();
    m_pHoveredItem = NULL;
    m_iMoveStartPos = 0;

    // Initialize
    m_pManager = pManager;
    m_bIsVisible = false;
    m_bIsIngame = true;
//    m_bIsInSubWindow = false;
    m_fFader = 0;
    m_ucFade = FADE_INVISIBLE;

    // Adjust window size to resolution
    CVector2D ScreenSize = m_pManager->GetResolution ();
    m_ScreenSize = ScreenSize;

    // First let's work out our x and y offsets
    if ( ScreenSize.fX > ScreenSize.fY ) //If the monitor is a normal landscape one
    {
		float iRatioSizeY = ScreenSize.fY / NATIVE_RES_Y;
		m_iMenuSizeX = NATIVE_RES_X * iRatioSizeY;
		m_iMenuSizeY = ScreenSize.fY;
		m_iXOff = (ScreenSize.fX - m_iMenuSizeX)*0.5f;
		m_iYOff = 0;
    }
    else //Otherwise our monitor is in a portrait resolution, so we cant fill the background by y
    {
		float iRatioSizeX = ScreenSize.fX / NATIVE_RES_X;
		m_iMenuSizeY = NATIVE_RES_Y * iRatioSizeX;
		m_iMenuSizeX = ScreenSize.fX;
		m_iXOff = 0;
		m_iYOff = (ScreenSize.fY - m_iMenuSizeY)*0.5f;
    }
    // First create our filler black background image, which covers the whole screen
    m_pFiller = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pFiller->LoadFromFile ( CORE_MTA_FILLER );
    m_pFiller->SetVisible ( false );
    m_pFiller->MoveToBack ();
    m_pFiller->SetZOrderingEnabled ( false );
    m_pFiller->SetAlwaysOnTop ( true );
    m_pFiller->MoveToBack ();
    m_pFiller->SetSize(CVector2D(m_iXOff,ScreenSize.fY),false);

    // Background image
    m_pBackground = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pBackground->LoadFromFile ( CORE_MTA_STATIC_BG );
    //m_pBackground->SetParent ( m_pFiller );
    m_pBackground->SetProperty("InheritsAlpha", "False" );
    m_pBackground->SetPosition ( CVector2D(m_iXOff,m_iYOff), false);   
    m_pBackground->SetSize ( CVector2D(m_iMenuSizeX,m_iMenuSizeY), false);
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
    m_pFiller2->SetPosition(CVector2D(m_iXOff+m_iMenuSizeX,0));
    m_pFiller2->SetSize(ScreenSize,false);

    // Create the image showing the version number
    m_pVersion = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
    m_pVersion->LoadFromFile ( CORE_MTA_VERSION );
    m_pVersion->SetParent ( m_pBackground );
    m_pVersion->SetPosition ( CVector2D(0.845f,0.528f), true);
    m_pVersion->SetSize ( CVector2D((32/NATIVE_RES_X)*m_iMenuSizeX,(32/NATIVE_RES_Y)*m_iMenuSizeY), false);
    m_pVersion->SetProperty("InheritsAlpha", "False" );

    m_pCommunityLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pFiller, "Not logged in" ) );
    m_pCommunityLabel->AutoSize ( "Not logged in" );
    m_pCommunityLabel->SetAlwaysOnTop ( true );
    m_pCommunityLabel->SetAlpha ( 0.7f );
    m_pCommunityLabel->SetVisible ( false );
    m_pCommunityLabel->SetPosition ( CVector2D ( 40.0f, ScreenSize.fY - 20.0f ) );

    std::string strUsername;
    CCore::GetSingleton().GetCommunity()->GetUsername ( strUsername );
    if ( CCore::GetSingleton().GetCommunity()->IsLoggedIn() && !strUsername.empty() )
        ChangeCommunityState ( true, strUsername );

    // Our disconnect item is shown/hidden dynamically, so we store it seperately
    m_pDisconnect = CreateItem ( MENU_ITEM_DISCONNECT, "cgui\\images\\menu_disconnect.png",    CVector2D ( 0.168f, 0.615f ),    CVector2D ( 278, 34 ) );
    m_pDisconnect->image->SetVisible(false);

    // Create the menu items
    //Filepath, Relative position, absolute native size
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_QUICK_CONNECT,  "cgui\\images\\menu_quick_connect.png",    CVector2D ( 0.168f, 0.615f ),    CVector2D ( 358, 34 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_BROWSE_SERVERS, "cgui\\images\\menu_browse_servers.png",   CVector2D ( 0.168f, 0.662f ),    CVector2D ( 390, 34 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_HOST_GAME,      "cgui\\images\\menu_host_game.png",        CVector2D ( 0.168f, 0.709f ),    CVector2D ( 251, 34 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_MAP_EDITOR,     "cgui\\images\\menu_map_editor.png",       CVector2D ( 0.168f, 0.756f ),    CVector2D ( 261, 34 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_SETTINGS,       "cgui\\images\\menu_settings.png",         CVector2D ( 0.168f, 0.803f ),    CVector2D ( 207, 34 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_ABOUT,          "cgui\\images\\menu_about.png",            CVector2D ( 0.168f, 0.850f ),    CVector2D ( 150, 34 ) ) );
    m_menuItems.push_back ( CreateItem ( MENU_ITEM_QUIT,           "cgui\\images\\menu_quit.png",             CVector2D ( 0.168f, 0.897f ),    CVector2D ( 102, 34 ) ) );

    // We store the position of the top item, and the second item.  These will be useful later
    m_iFirstItemTop  = (m_menuItems.front()->image)->GetPosition().fY;
    m_iSecondItemTop = (m_menuItems[1]->image)->GetPosition().fY;
    
    // Store some mosue over bounding box positions
    m_menuAX = (0.168f*m_iMenuSizeX) + m_iXOff;             //Left side of the items
    m_menuAY = m_iFirstItemTop;                             //Top side of the items
	m_menuBX =  m_menuAX + ((390/NATIVE_RES_X)*m_iMenuSizeX); //Right side of the items. We add the longest picture (browse_servers)

    m_pMenuArea = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage(m_pBackground) );
    m_pMenuArea->LoadFromFile ( CORE_MTA_FILLER );
    m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff), false);
    m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY), false);
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
    m_pLatestNews->SetParent ( m_pBackground );
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
        CGUILabel * pItemShadow = reinterpret_cast < CGUILabel* > ( m_pManager->CreateLabel ( m_pBackground, " " ) );
        CGUILabel * pItem = reinterpret_cast < CGUILabel* > ( m_pManager->CreateLabel ( m_pBackground, " " ) );

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
        CGUILabel * pItemDate = reinterpret_cast < CGUILabel* > ( m_pManager->CreateLabel ( m_pBackground, " " ) );

        pItemDate->SetFont ( "default-small" );
        pItemDate->SetHorizontalAlign ( CGUI_ALIGN_RIGHT );

        pItemDate->SetSize( CVector2D (fDrawSizeX, 13), false );
        pItemDate->SetPosition( CVector2D (fDrawPosX, fDrawPosY), false );

        m_pNewsItemDateLabels[i] = pItemDate;

        // Create 'NEW' sticker
        CGUILabel*& pLabel =  m_pNewsItemNEWLabels[i];
        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pBackground, "NEW" ) );
        pLabel->SetFont ( "default-small" );
        pLabel->SetTextColor ( 255, 0, 0 );
        pLabel->AutoSize ( pLabel->GetText ().c_str () );
        pLabel->SetAlpha ( 0.7f );
        pLabel->SetVisible ( false );
    }

    // Submenu's
    m_QuickConnect.SetVisible ( false );
    m_ServerBrowser.SetVisible ( false );
    m_ServerQueue.SetVisible ( false );
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

    // Remove unused node
    if ( CXMLNode* pOldNode = pConfig->FindSubNode ( CONFIG_NODE_SERVER_INT ) )
        pConfig->DeleteSubNode ( pOldNode );

}


CMainMenu::~CMainMenu ( void )
{
    // Destroy GUI items
    delete m_pBackground;
    delete m_pFiller;
    delete m_pLatestNews;
    delete m_pVersion;

    // Destroy community label
    delete m_pCommunityLabel;

    while ( m_menuItems.begin() != m_menuItems.end() )
    {
        std::deque<sMenuItem*>::iterator it = m_menuItems.begin();
        if ( (*it) != m_pDisconnect )
        {
            sMenuItem* item = (*it);
            delete item;
            m_menuItems.erase(it);
        }
        else
        {
            m_menuItems.erase(it);
        }
    }

    delete m_pDisconnect;
}

void CMainMenu::SetMenuVerticalPosition ( int iPosY )
{
	if ( m_pHoveredItem )
    {
        m_unhoveredItems.insert ( m_pHoveredItem );
        m_pHoveredItem = NULL;
    }

    int iMoveY = iPosY - m_menuAY;
    
    std::deque<sMenuItem*>::iterator it = m_menuItems.begin();
    for ( it; it != m_menuItems.end(); it++ )
    {
        CVector2D vOrigPos = (*it)->image->GetPosition(false);
        (*it)->drawPositionY = (*it)->drawPositionY + iMoveY;
        (*it)->image->SetPosition( CVector2D(vOrigPos.fX,vOrigPos.fY + iMoveY), false);
    }
	
    m_menuAY = iPosY;
	m_menuBY = m_menuBY + iMoveY;
    m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff), false);
    m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY), false);
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
    // Get the game interface and the system state
    CGame* pGame = CCore::GetSingleton ().GetGame ();
    eSystemState SystemState = pGame->GetSystemState ();

    m_Credits.Update ();
    m_Settings.Update ();

    unsigned long ulCurrentTick = GetTickCount();
    unsigned long ulTimePassed = ulCurrentTick - ulPreviousTick;

    if ( m_bHideGame )
        m_pGraphics->DrawRectangle(0,0,m_ScreenSize.fX,m_ScreenSize.fY,0xFF000000);

    if ( m_bIsIngame )  //CEGUI hack
    {
        float fAlpha = m_pDisconnect->image->GetAlpha();
        m_pDisconnect->image->SetAlpha(0.35f);
        m_pDisconnect->image->SetAlpha(fAlpha);
    }

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
    if ( m_bMouseOverMenu && ( cursor.x > m_menuAX ) && ( cursor.y > m_menuAY ) && ( cursor.x < m_menuBX ) && ( cursor.y < m_menuBY ) )
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
		fProgress = fProgress - ((float)ulTimePassed/CORE_MTA_ANIMATION_TIME)*(CORE_MTA_HOVER_ALPHA-CORE_MTA_NORMAL_ALPHA);
        if ( SetItemHoverProgress ( (*it), fProgress, false ) )
        {
            std::set<sMenuItem*>::iterator itToErase = it;
            it--;
            m_unhoveredItems.erase(itToErase);
        }
        else
            it++;
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
                m_menuAY =  m_pDisconnect->image->GetPosition(false).fY;
                m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff), false);
                m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY), false);
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
        m_pBackground->SetAlpha ( Clamp <float> ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );

        if ( m_fFader > 0.0f )
        {
            m_bIsVisible = true;     // Make cursor appear faster
        }

        // If the fade is complete
        if ( m_fFader >= fFadeTarget ) {
            m_ucFade = FADE_VISIBLE;
            m_bIsVisible = true;

        }
    }
    // Fade out
    else if ( m_ucFade == FADE_OUT ) {
        m_fFader -= CORE_MTA_FADER;

        m_pFiller->SetAlpha ( Clamp ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pFiller2->SetAlpha ( Clamp ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
        m_pBackground->SetAlpha ( Clamp ( 0.f, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );

        if ( m_fFader < 1.0f )
            m_bIsVisible = false;    // Make cursor disappear faster

        // If the fade is complete
        if ( m_fFader <= 0 ) {
            m_ucFade = FADE_INVISIBLE;
            m_bIsVisible = false;

            // Turn the widgets invisible
            m_pFiller->SetVisible ( false );
            m_pFiller2->SetVisible ( false );
            m_pBackground->SetVisible(false);
        }
    }

    // Force the mainmenu on if we're at GTA's mainmenu or not ingame
    if ( ( SystemState == 7 || SystemState == 9 ) && !m_bIsIngame )
    {
        // it takes 250 frames for the menu to be shown, we seem to update this twice a frame
        if ( WaitForMenu >= 250 ) {
            if ( !m_bStarted )
            {
                m_pNewsBrowser->CreateHeadlines ();
                GetVersionUpdater ()->EnableChecking ( true );
            }
            m_bIsVisible = true;
            m_bStarted = true;
        } else
            WaitForMenu++;
    }

    // If we're visible
    if ( m_bIsVisible && SystemState != 8 )
    {
        // If we're at the game's mainmenu, or ingame when m_bIsIngame is true show the background
        if ( SystemState == 7 ||                    // GS_FRONTEND
             SystemState == 9 && !m_bIsIngame )     // GS_INGAME
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


    ulPreviousTick = GetTickCount();

    // Call subdialog pulses
    m_ServerBrowser.Update ();
    m_ServerQueue.DoPulse ();
}


void CMainMenu::Show ( bool bOverlay )
{
    SetVisible ( true, bOverlay );
}


void CMainMenu::Hide ( void )
{
    SetVisible ( false );
}


void CMainMenu::SetVisible ( bool bVisible, bool bOverlay )
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
        SetMenuUnhovered ();
        m_QuickConnect.SetVisible ( false );
        m_ServerBrowser.SetVisible ( false );
        m_Settings.SetVisible ( false );
        m_Credits.SetVisible ( false );
        m_pCommunityLabel->SetVisible ( false );
        m_pNewsBrowser->SetVisible ( false );

//        m_bIsInSubWindow = false;
    } else {
        m_pFiller->SetVisible ( true );
        m_pFiller2->SetVisible ( true );
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

        m_ulMoveStartTick = GetTickCount();
        if ( bIsIngame )
        {
            m_pDisconnect->image->SetVisible(true);
            m_iMoveTargetPos = m_iSecondItemTop;
        }
        else
        {
            if ( m_menuItems.front() == m_pDisconnect )
                m_menuItems.pop_front();
            
            m_menuAY = m_menuItems.front()->image->GetPosition().fY;
            m_pMenuArea->SetPosition ( CVector2D(m_menuAX-m_iXOff,m_menuAY-m_iYOff), false);
            m_pMenuArea->SetSize ( CVector2D(m_menuBX-m_menuAX,m_menuBY-m_menuAY), false);
            m_iMoveTargetPos = m_iFirstItemTop;
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
        switch (m_pHoveredItem->menuType)
        {
            case MENU_ITEM_DISCONNECT:      OnDisconnectButtonClick   (pElement);    break;
            case MENU_ITEM_QUICK_CONNECT:   OnQuickConnectButtonClick (pElement);    break;
            case MENU_ITEM_BROWSE_SERVERS:  OnBrowseServersButtonClick(pElement);    break;
            case MENU_ITEM_HOST_GAME:       OnHostGameButtonClick     (pElement);    break;
            case MENU_ITEM_MAP_EDITOR:      OnEditorButtonClick       (pElement);    break;
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

//    if ( !m_bIsInSubWindow )
    {
        m_QuickConnect.SetVisible ( true );
//        m_bIsInSubWindow = true;
    }

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

void CMainMenu::ShowServerQueue ( void )
{
    m_ServerQueue.SetVisible ( true );
}

void CMainMenu::HideServerQueue ( void )
{
    m_ServerQueue.SetVisible ( false );
}


bool CMainMenu::OnDisconnectButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    // Send "disconnect" command to the command handler
    CCommands::GetSingleton ().Execute ( "disconnect", "" );

    return true;
}


bool CMainMenu::OnHostGameButtonClick ( CGUIElement* pElement )
{
    // Return if we haven't faded in yet
    if ( m_ucFade != FADE_VISIBLE ) return false;

    // Load deathmatch, but with local play
    CModManager::GetSingleton ().RequestLoad ( "deathmatch", "local" );

    return true;
}


bool CMainMenu::OnEditorButtonClick ( CGUIElement* pElement )
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


sMenuItem* CMainMenu::CreateItem ( unsigned char menuType, const char* szFilePath, CVector2D vecRelPosition, CVector2D vecNativeSize )
{ 
    // Make our positions absolute
	int iPosX = vecRelPosition.fX*m_iMenuSizeX;
    int iPosY = vecRelPosition.fY*m_iMenuSizeY;

    // Make our sizes relative to the size of menu, but in absolute coordinates
	int iSizeX = (vecNativeSize.fX/NATIVE_RES_X)*m_iMenuSizeX;
    int iSizeY = (vecNativeSize.fY/NATIVE_RES_Y)*m_iMenuSizeY;
	
    // Mark our bounding box's bottom value.
	m_menuBY = (iPosY + (iSizeY*CORE_MTA_HOVER_SCALE)/2) + m_iYOff;
	
    // Reduced their size down to unhovered size.
	iSizeX = iSizeX*CORE_MTA_NORMAL_SCALE;
    iSizeY = iSizeY*CORE_MTA_NORMAL_SCALE;
	// Grab our draw position from which we enlarge from
    iPosY = iPosY - (iSizeY/2);

    CGUIStaticImage* pImage = reinterpret_cast < CGUIStaticImage* > ( m_pManager->CreateStaticImage () );
    pImage->LoadFromFile ( szFilePath );
    pImage->SetParent ( m_pBackground );
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


void CMainMenu::SetNewsHeadline ( int iIndex, const SString& strContent, bool bIsNew )
{
    if ( iIndex < 0 || iIndex > 2 )
        return;

    m_pLatestNews->SetVisible(true);

    // Headline
    CGUILabel* pItem = m_pNewsItemLabels[ iIndex ];
    CGUILabel* pItemShadow = m_pNewsItemShadowLabels[ iIndex ];
    SColor color = headlineColors[ iIndex ];
    pItem->SetTextColor ( color.R, color.G, color.B );
    pItem->SetText ( strContent );
    pItemShadow->SetText ( strContent );

    // Set our Date labels
    CGUILabel* pItemDate = m_pNewsItemDateLabels[ iIndex ];
    pItemDate->SetText ( "2012-12-21" );

    // 'NEW' sticker
    CGUILabel* pNewLabel = m_pNewsItemNEWLabels[ iIndex ];
    pNewLabel->SetVisible ( bIsNew );
    pNewLabel->SetPosition ( CVector2D ( pItem->GetPosition ().fX + 4, pItem->GetPosition ().fY - 4 ) );
}
