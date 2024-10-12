/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerBrowser.cpp
 *  PURPOSE:     In-game server browser user interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServerCache.h"

using namespace std;

extern CCore* g_pCore;

template <>
CServerBrowser* CSingleton<CServerBrowser>::m_pSingleton = NULL;

#define SB_SPAN 0.85f                  // How much % of the screen the server browser should fill
#define SB_NAVBAR_SIZE_Y 40            // Navbar button size
#define SB_BUTTON_SIZE_X 26
#define SB_BUTTON_SIZE_Y 26
#define SB_SPACER 10            // Spacer between searchbar and navbar
#define SB_SMALL_SPACER 5
#define SB_SEARCHBAR_COMBOBOX_SIZE_X 45            // Mow much the search type combobox occupies of searchbar
#define SB_SEARCHBAR_COMBOBOX_SIZE_Y 22
#define SB_PLAYERLIST_SIZE_X 200            // Width of players list [NB. adjusted for low resolutions in CServerBrowser::CreateTab]
#define SB_BACK_BUTTON_SIZE_Y 40            // Size of the back butt
#define COMBOBOX_ARROW_SIZE_X 23            // Fixed CEGUI size of the 'combobox' arrow
#define TAB_SIZE_Y 25                       // Fixed CEGUI size of the Tab in a tab panel

#define CONNECT_HISTORY_LIMIT 20

//
// Local helper
//
static CVector2D CalcScreenPosition(CGUIElement* pElement)
{
    CVector2D pos;
    for (; pElement; pElement = pElement->GetParent())
        pos += pElement->GetPosition();
    return pos;
}

CServerBrowser::CServerBrowser()
{
    CGUI* pManager = g_pCore->GetGUI();

    // Initialize
    m_ulLastUpdateTime = 0;
    m_bFirstTimeBrowseServer = true;
    m_bOptionsLoaded = false;
    m_PrevServerBrowserType = ServerBrowserTypes::INTERNET;

    m_bFocusTextEdit = false;
    m_uiShownQuickConnectHelpCount = 0;
    m_uiIsUsingTempTab = 0;
    m_BeforeTempServerBrowserType = ServerBrowserTypes::INTERNET;
    m_llLastGeneralHelpTime = 0;

    // Do some initial math
    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    bool      bCreateFrame = true;

    if (resolution.fY <= 600)            // Make our window bigger at small resolutions
    {
        m_WidgetSize = CVector2D(resolution.fX, resolution.fY);
        bCreateFrame = false;
    }
    else
        m_WidgetSize = CVector2D(resolution.fX * SB_SPAN, resolution.fY * SB_SPAN);

    // Limit size
    m_WidgetSize.fX = std::min(m_WidgetSize.fX, 1200.f);
    m_WidgetSize.fY = std::min(m_WidgetSize.fY, 1000.f);

    CVector2D widgetPosition((resolution.fX - m_WidgetSize.fX) / 2, (resolution.fY - m_WidgetSize.fY) / 2);

    if (bCreateFrame)
    {
        // Create the window
        m_pFrame = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, _("SERVER BROWSER")));
        m_pTopWindow = m_pFrame;
        m_pFrame->SetCloseButtonEnabled(true);
        m_pFrame->SetMovable(true);
        m_pFrame->SetPosition(widgetPosition - CVector2D(10, 20));
        m_pFrame->SetSize(m_WidgetSize + CVector2D(20, 30));
        m_pFrame->SetSizingEnabled(false);
        m_pFrame->SetAlwaysOnTop(true);
        m_pFrame->SetZOrderingEnabled(false);

        // Create the serverlist tab panel and some tabs
        m_pPanel = reinterpret_cast<CGUITabPanel*>(pManager->CreateTabPanel(m_pFrame));
        m_pPanel->SetPosition(CVector2D(10, 20));
        m_pPanel->SetSize(m_WidgetSize);
    }
    else
    {
        // No frame
        m_pFrame = NULL;

        // Create the serverlist tab panel and some tabs
        m_pPanel = reinterpret_cast<CGUITabPanel*>(pManager->CreateTabPanel());
        m_pTopWindow = m_pPanel;
        m_pPanel->SetPosition(widgetPosition);
        m_pPanel->SetSize(m_WidgetSize);
        m_pPanel->SetAlwaysOnTop(true);
        m_pPanel->SetZOrderingEnabled(false);
    }

    // Create locked icon
    m_pLockedIcon = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage());
    m_pLockedIcon->SetVisible(false);
    m_pLockedIcon->SetFrameEnabled(false);
    m_pLockedIcon->LoadFromFile("cgui\\images\\serverbrowser\\locked.png");

    // Create search filter types icon
    m_szSearchTypePath[SearchTypes::SERVERS] = "cgui\\images\\serverbrowser\\search-servers.png";
    m_szSearchTypePath[SearchTypes::PLAYERS] = "cgui\\images\\serverbrowser\\search-players.png";

    for (unsigned int i = 0; i != SearchTypes::MAX_SEARCH_TYPES; i++)
    {
        m_pSearchIcons[i] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage());
        m_pSearchIcons[i]->SetVisible(false);
        m_pSearchIcons[i]->SetFrameEnabled(false);
        m_pSearchIcons[i]->LoadFromFile(m_szSearchTypePath[i]);
    }

    // Create the tabs
    CreateTab(ServerBrowserTypes::INTERNET, _("Internet"));
    CreateTab(ServerBrowserTypes::LAN, _("Local"));
    CreateTab(ServerBrowserTypes::FAVOURITES, _("Favourites"));
    CreateTab(ServerBrowserTypes::RECENTLY_PLAYED, _("Recent"));

    // Load options
    LoadOptions(CCore::GetSingletonPtr()->GetConfig()->FindSubNode(CONFIG_NODE_SERVER_OPTIONS));

    // Save the active tab, needs to be done after at least one tab exists
    m_pPanel->SetSelectionHandler(GUI_CALLBACK(&CServerBrowser::OnTabChanged, this));

    // Attach some editbox handlers, also must be done after full creation
    for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
    {
        m_pEditAddress[i]->SetActivateHandler(GUI_CALLBACK(&CServerBrowser::OnAddressFocused, this));
        m_pEditAddress[i]->SetDeactivateHandler(GUI_CALLBACK(&CServerBrowser::OnAddressDefocused, this));
        m_pEditSearch[i]->SetActivateHandler(GUI_CALLBACK(&CServerBrowser::OnSearchFocused, this));
        m_pEditSearch[i]->SetDeactivateHandler(GUI_CALLBACK(&CServerBrowser::OnSearchDefocused, this));
    }

    // Simulate focusing to keep things tidy
    OnSearchFocused(m_pTopWindow);
    OnAddressFocused(m_pTopWindow);
    OnSearchDefocused(m_pTopWindow);
    OnAddressDefocused(m_pTopWindow);

    // Quick connect help
    CVector2D helpPos;
    helpPos.fX = resolution.fX * (512 / 1024.f);
    helpPos.fY = widgetPosition.fY + (115 - 58);

    m_pQuickConnectHelpWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, ""));
    m_pQuickConnectHelpWindow->SetMovable(false);
    m_pQuickConnectHelpWindow->SetPosition(helpPos);
    m_pQuickConnectHelpWindow->SetSize(CVector2D(320, 150));
    m_pQuickConnectHelpWindow->SetAlwaysOnTop(true);
    m_pQuickConnectHelpWindow->SetFrameEnabled(false);
    m_pQuickConnectHelpWindow->SetTitlebarEnabled(false);
    m_pQuickConnectHelpWindow->SetCloseButtonEnabled(false);
    m_pQuickConnectHelpWindow->SetVisible(false);

    // Quick connect help label
    {
        CGUILabel* pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pQuickConnectHelpWindow, ""));
        pLabel->SetPosition(CVector2D(5, 0));
        pLabel->SetSize(CVector2D(310, 150));
        pLabel->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        pLabel->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP);
        pLabel->SetProperty("BackgroundEnabled", "True");

        // Create second slightly smaller label so wrapped text looks better
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pQuickConnectHelpWindow, ""));
        pLabel->SetPosition(CVector2D(5 + 10, 0));
        pLabel->SetSize(CVector2D(310 - 10 * 2, 150));
        pLabel->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        pLabel->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP);

        SString strHelpMessage =
            _("FOR QUICK CONNECT:\n"
              "\n"
              "Type the address and port into the address bar.\n"
              "Or select a server from the history list and press 'Connect'");

        pLabel->SetText(strHelpMessage);
    }

    // General help
    CVector2D helpButtonSize = m_pButtonGeneralHelp[0]->GetSize();
    CVector2D helpButtonPos = CalcScreenPosition(m_pButtonGeneralHelp[0]) + CVector2D(0, 24);

    m_pGeneralHelpWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, _("HELP")));
    m_pGeneralHelpWindow->SetMovable(false);
    m_pGeneralHelpWindow->SetAlwaysOnTop(true);
    m_pGeneralHelpWindow->SetFrameEnabled(false);
    m_pGeneralHelpWindow->SetTitlebarEnabled(false);
    m_pGeneralHelpWindow->SetCloseButtonEnabled(false);
    m_pGeneralHelpWindow->SetVisible(false);
    m_pGeneralHelpWindow->SetDeactivateHandler(GUI_CALLBACK(&CServerBrowser::OnGeneralHelpDeactivate, this));

    float fMaxLeft = pManager->CGUI_GetMaxTextExtent("default-normal", _("Refresh"), _("Add Favorite"), _("Connect"), _("Server information"));
    float fMaxRight = pManager->CGUI_GetMaxTextExtent("default-normal", _("Search servers"), _("Search players"), _("Start search"));

    CVector2D generalHelpSize(80 + fMaxLeft + 80 + fMaxRight, 160);
    CVector2D generalHelpPos = helpButtonPos - generalHelpSize + CVector2D(helpButtonSize.fX, 0);
    m_pGeneralHelpWindow->SetPosition(generalHelpPos);
    m_pGeneralHelpWindow->SetSize(generalHelpSize);

    {
        const int iBase = 23;
        const int iGap = 29;

        struct
        {
            int         x, y, w, h;
            const char* szName;
        } iconInfoList[] = {
            {20, iBase + iGap * 0, 26, 26, "cgui\\images\\serverbrowser\\refresh.png"},
            {25, iBase + iGap * 1 + 5, 16, 16, "cgui\\images\\serverbrowser\\favorite.png"},
            {20, iBase + iGap * 2, 26, 26, "cgui\\images\\serverbrowser\\connect.png"},
            {20, iBase + iGap * 3, 26, 26, "cgui\\images\\serverbrowser\\info.png"},
            {80 + static_cast<int>(fMaxLeft), iBase + iGap * 0 + 5, 29, 16, "cgui\\images\\serverbrowser\\search-servers.png"},
            {80 + static_cast<int>(fMaxLeft), iBase + iGap * 1 + 5, 29, 16, "cgui\\images\\serverbrowser\\search-players.png"},
            {80 + static_cast<int>(fMaxLeft), iBase + iGap * 2 + 5, 16, 16, "cgui\\images\\serverbrowser\\search.png"},
        };

        for (uint i = 0; i < NUMELMS(iconInfoList); i++)
        {
            CGUIStaticImage* pIcon = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pGeneralHelpWindow));
            pIcon->SetPosition(CVector2D(iconInfoList[i].x, iconInfoList[i].y));
            pIcon->SetSize(CVector2D(iconInfoList[i].w, iconInfoList[i].h));
            pIcon->LoadFromFile(iconInfoList[i].szName);
            pIcon->SetZOrderingEnabled(false);
        }

        struct
        {
            int     x, y;
            SString strDesc;
        } helpInfoList[] = {
            {20, iBase + iGap * 0, _("Refresh")},
            {20, iBase + iGap * 1, _("Add Favorite")},
            {20, iBase + iGap * 2, _("Connect")},
            {20, iBase + iGap * 3, _("Server information")},
            {80 + static_cast<int>(fMaxLeft), iBase + iGap * 0, _("Search servers")},
            {80 + static_cast<int>(fMaxLeft), iBase + iGap * 1, _("Search players")},
            {80 + static_cast<int>(fMaxLeft), iBase + iGap * 2, _("Start search")},
        };

        for (uint i = 0; i < NUMELMS(helpInfoList); i++)
        {
            CGUILabel* pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pGeneralHelpWindow, ""));
            pLabel->SetText(SString("  -   %s", helpInfoList[i].strDesc.c_str()));
            pLabel->SetPosition(CVector2D(helpInfoList[i].x + 20, helpInfoList[i].y + 5));
            pLabel->AutoSize();
        }
    }
}

CServerBrowser::~CServerBrowser()
{
    // Save options now and disable selection handler
    SaveOptions();
    m_pPanel->SetSelectionHandler(nullptr);

    // Delete the Tabs
    DeleteTab(ServerBrowserTypes::INTERNET);
    DeleteTab(ServerBrowserTypes::LAN);
    DeleteTab(ServerBrowserTypes::FAVOURITES);
    DeleteTab(ServerBrowserTypes::RECENTLY_PLAYED);

    // Unload the icon
    m_pLockedIcon->Clear();
    for (unsigned int i = 0; i != SearchTypes::MAX_SEARCH_TYPES; i++)
        m_pSearchIcons[i]->Clear();

    // Delete the GUI items
    delete m_pTopWindow;
}

void CServerBrowser::CreateTab(ServerBrowserType type, const char* szName)
{
    CGUI* pManager = g_pCore->GetGUI();

    m_pTab[type] = m_pPanel->CreateTab(szName);

    float fPlayerListSizeX = SB_PLAYERLIST_SIZE_X;
    float fSearchBarSizeX = pManager->GetTextExtent(_("Search players..."), "default-bold-small") + 90;

    float fConnectButtonWidth = 26 + pManager->GetTextExtent(_("Connect"), "default-bold-small") + 5;

    // Make our playerlist smaller, if it's a small panel - either 15%, or a max of 200px in size.
    fPlayerListSizeX = std::min<float>(m_WidgetSize.fX * 0.15, SB_PLAYERLIST_SIZE_X);

    // Formulate our navigation bar

    float fX = SB_SMALL_SPACER;
    float fY = (SB_NAVBAR_SIZE_Y - SB_BUTTON_SIZE_X) / 2;

    // Refresh button + icon
    m_pButtonRefresh[type] = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTab[type], ""));
    m_pButtonRefresh[type]->SetPosition(CVector2D(fX, fY), false);
    m_pButtonRefresh[type]->SetSize(CVector2D(SB_BUTTON_SIZE_X, SB_BUTTON_SIZE_Y), false);
    m_pButtonRefresh[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnRefreshClick, this));
    m_pButtonRefresh[type]->SetZOrderingEnabled(false);
    m_pButtonRefreshIcon[type] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pButtonRefresh[type]));
    m_pButtonRefreshIcon[type]->SetSize(CVector2D(1, 1), true);
    m_pButtonRefreshIcon[type]->LoadFromFile("cgui\\images\\serverbrowser\\refresh.png");
    m_pButtonRefreshIcon[type]->SetProperty("MousePassThroughEnabled", "True");
    m_pButtonRefreshIcon[type]->SetProperty("DistributeCapturedInputs", "True");

    // Address Bar + History Combo
    fX = fX + SB_BUTTON_SIZE_X + SB_SMALL_SPACER;
    // Work out our size by calculating from the end - minus the searchbox, combobox, spacing, info button, play button
    float fWidth = m_WidgetSize.fX - SB_SMALL_SPACER - fSearchBarSizeX - SB_SPACER - SB_BUTTON_SIZE_X - SB_SMALL_SPACER - fConnectButtonWidth -
                   SB_SMALL_SPACER - COMBOBOX_ARROW_SIZE_X - fX;
    m_pEditAddress[type] = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(m_pTab[type], ""));
    m_pEditAddress[type]->SetPosition(CVector2D(fX, fY), false);
    m_pEditAddress[type]->SetSize(CVector2D(fWidth, SB_BUTTON_SIZE_Y), false);
    m_pEditAddress[type]->SetTextAcceptedHandler(GUI_CALLBACK(&CServerBrowser::OnConnectClick, this));
    m_pEditAddress[type]->SetTextChangedHandler(GUI_CALLBACK(&CServerBrowser::OnAddressChanged, this));

    m_pLabelAddressDescription[type] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pEditAddress[type], "Enter an address [IP:Port]"));
    m_pLabelAddressDescription[type]->SetPosition(CVector2D(10, 5), false);
    m_pLabelAddressDescription[type]->SetTextColor(0, 0, 0);
    m_pLabelAddressDescription[type]->AutoSize(m_pLabelAddressDescription[type]->GetText().c_str());
    m_pLabelAddressDescription[type]->SetAlpha(0.6f);
    m_pLabelAddressDescription[type]->SetProperty("MousePassThroughEnabled", "True");
    m_pLabelAddressDescription[type]->SetProperty("DistributeCapturedInputs", "True");

    // Favourite icon
    m_pAddressFavoriteIcon[type] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pEditAddress[type]));
    m_pAddressFavoriteIcon[type]->SetPosition(CVector2D(fWidth - 20 - 8, (SB_BUTTON_SIZE_Y - 16) / 2), false);
    m_pAddressFavoriteIcon[type]->SetSize(CVector2D(16, 16), false);
    m_pAddressFavoriteIcon[type]->LoadFromFile("cgui\\images\\serverbrowser\\favorite.png");
    m_pAddressFavoriteIcon[type]->SetAlpha(0.3f);
    m_pAddressFavoriteIcon[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnFavouritesClick, this));

    // Remove recent played icon
    if (type == ServerBrowserTypes::RECENTLY_PLAYED)
    {
        m_pRemoveFromRecentIcon[type] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pEditAddress[type]));
        m_pRemoveFromRecentIcon[type]->SetPosition(CVector2D(fWidth - 40 - 8, (SB_BUTTON_SIZE_Y - 16) / 2), false);
        m_pRemoveFromRecentIcon[type]->SetSize(CVector2D(16, 16), false);
        m_pRemoveFromRecentIcon[type]->LoadFromFile("cgui\\images\\serverbrowser\\trashcan.png");
        m_pRemoveFromRecentIcon[type]->SetAlpha(0.3f);
        m_pRemoveFromRecentIcon[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnRemoveFromRecentClick, this));
    }

    // History Combo
    fWidth = fWidth + COMBOBOX_ARROW_SIZE_X;
    m_pComboAddressHistory[type] = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(m_pTab[type], ""));
    m_pComboAddressHistory[type]->SetPosition(CVector2D(fX - 1, fY + (SB_BUTTON_SIZE_Y - SB_SEARCHBAR_COMBOBOX_SIZE_Y) / 2), false);
    m_pComboAddressHistory[type]->SetSize(CVector2D(fWidth, 200), false);
    m_pComboAddressHistory[type]->SetReadOnly(true);
    m_pComboAddressHistory[type]->SetSelectionHandler(GUI_CALLBACK(&CServerBrowser::OnHistorySelected, this));
    m_pComboAddressHistory[type]->SetDropListRemoveHandler(GUI_CALLBACK(&CServerBrowser::OnHistoryDropListRemove, this));

    // Connect button + icon
    fX = fX + fWidth + SB_SMALL_SPACER;
    std::string strButtonText = _("Connect");
    strButtonText = "     " + strButtonText;
    m_pButtonConnect[type] = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTab[type], strButtonText.c_str()));
    m_pButtonConnect[type]->SetPosition(CVector2D(fX, fY), false);
    m_pButtonConnect[type]->SetSize(CVector2D(fConnectButtonWidth, SB_BUTTON_SIZE_Y), false);
    m_pButtonConnect[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnConnectClick, this));
    m_pButtonConnect[type]->SetFont("default-bold-small");
    m_pButtonConnect[type]->SetZOrderingEnabled(false);
    m_pButtonConnectIcon[type] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pButtonConnect[type]));
    m_pButtonConnectIcon[type]->SetSize(CVector2D(SB_BUTTON_SIZE_Y, SB_BUTTON_SIZE_Y), false);
    m_pButtonConnectIcon[type]->LoadFromFile("cgui\\images\\serverbrowser\\connect.png");
    m_pButtonConnectIcon[type]->SetProperty("MousePassThroughEnabled", "True");

    // Info button + icon
    fX = fX + fConnectButtonWidth + SB_SMALL_SPACER;
    m_pButtonInfo[type] = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTab[type], ""));
    m_pButtonInfo[type]->SetPosition(CVector2D(fX, fY), false);
    m_pButtonInfo[type]->SetSize(CVector2D(SB_BUTTON_SIZE_X, SB_BUTTON_SIZE_Y), false);
    m_pButtonInfo[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnInfoClick, this));
    m_pButtonInfo[type]->SetZOrderingEnabled(false);
    m_pButtonInfoIcon[type] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pButtonInfo[type]));
    m_pButtonInfoIcon[type]->SetSize(CVector2D(1, 1), true);
    m_pButtonInfoIcon[type]->LoadFromFile("cgui\\images\\serverbrowser\\info.png");
    m_pButtonInfoIcon[type]->SetProperty("MousePassThroughEnabled", "True");

    // Search bar + type combo
    fX = m_WidgetSize.fX - SB_SMALL_SPACER - fSearchBarSizeX;
    m_pComboSearchType[type] = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(m_pTab[type], ""));
    m_pComboSearchType[type]->SetPosition(CVector2D(fX, fY + (SB_BUTTON_SIZE_Y - SB_SEARCHBAR_COMBOBOX_SIZE_Y) / 2), false);
    m_pComboSearchType[type]->SetSize(CVector2D(SB_SEARCHBAR_COMBOBOX_SIZE_X, 80), false);

    for (unsigned int i = 0; i != SearchTypes::MAX_SEARCH_TYPES; i++)
        m_pComboSearchType[type]->AddItem(m_pSearchIcons[i]);

    m_pComboSearchType[type]->SetReadOnly(true);
    m_pComboSearchType[type]->SetSelectedItemByIndex(0);
    m_pComboSearchType[type]->SetSelectionHandler(GUI_CALLBACK(&CServerBrowser::OnSearchTypeSelected, this));
    m_pSearchTypeIcon[type] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pComboSearchType[type]));
    m_pSearchTypeIcon[type]->SetPosition(CVector2D(2, 4), false);
    m_pSearchTypeIcon[type]->SetSize(CVector2D(29, SB_SEARCHBAR_COMBOBOX_SIZE_Y - 6), false);
    m_pSearchTypeIcon[type]->SetProperty("MousePassThroughEnabled", "True");
    m_pSearchTypeIcon[type]->SetAlwaysOnTop(true);
    m_pSearchTypeIcon[type]->LoadFromFile(m_szSearchTypePath[SearchTypes::SERVERS]);

    fWidth = fSearchBarSizeX - SB_SEARCHBAR_COMBOBOX_SIZE_X;
    m_pEditSearch[type] = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(m_pTab[type], ""));
    m_pEditSearch[type]->SetPosition(CVector2D(fX + SB_SEARCHBAR_COMBOBOX_SIZE_X, fY + (SB_BUTTON_SIZE_Y - SB_SEARCHBAR_COMBOBOX_SIZE_Y) / 2), false);
    m_pEditSearch[type]->SetSize(CVector2D(fWidth, SB_SEARCHBAR_COMBOBOX_SIZE_Y), false);
    m_pEditSearch[type]->SetTextChangedHandler(GUI_CALLBACK(&CServerBrowser::OnFilterChanged, this));

    m_pLabelSearchDescription[type] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pEditSearch[type], _("Search servers...")));
    m_pLabelSearchDescription[type]->SetPosition(CVector2D(10, 3), false);
    m_pLabelSearchDescription[type]->SetTextColor(0, 0, 0);
    m_pLabelSearchDescription[type]->SetSize(CVector2D(1, 1), true);
    m_pLabelSearchDescription[type]->SetAlpha(0.6f);
    m_pLabelSearchDescription[type]->SetProperty("MousePassThroughEnabled", "True");
    m_pLabelSearchDescription[type]->SetProperty("DistributeCapturedInputs", "True");

    // Server search icon
    m_pServerSearchIcon[type] = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pEditSearch[type]));
    m_pServerSearchIcon[type]->SetPosition(CVector2D(fWidth - 18, (SB_SEARCHBAR_COMBOBOX_SIZE_Y - 16) / 2), false);
    m_pServerSearchIcon[type]->SetSize(CVector2D(16, 16), false);
    m_pServerSearchIcon[type]->LoadFromFile("cgui\\images\\serverbrowser\\search.png");
    m_pServerSearchIcon[type]->SetProperty("MousePassThroughEnabled", "True");
    m_pServerSearchIcon[type]->SetProperty("DistributeCapturedInputs", "True");

    // Create the serverlist
    fX = 5;
    fY = fY + SB_BUTTON_SIZE_Y + (SB_NAVBAR_SIZE_Y - SB_BUTTON_SIZE_X) / 2;
    fWidth = m_WidgetSize.fX - SB_SMALL_SPACER - fPlayerListSizeX - fX;
    float fHeight = m_WidgetSize.fY - SB_SMALL_SPACER - SB_BACK_BUTTON_SIZE_Y - fY - TAB_SIZE_Y;
    m_pServerList[type] = reinterpret_cast<CGUIGridList*>(pManager->CreateGridList(m_pTab[type]));
    m_pServerList[type]->SetPosition(CVector2D(fX, fY), false);
    m_pServerList[type]->SetSize(CVector2D(fWidth, fHeight), false);
    m_pServerList[type]->SetIgnoreTextSpacer(true);
    m_pServerList[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnClick, this));
    m_pServerListRevision[type] = 0;

    // Server List Columns
    m_hVersion[type] = m_pServerList[type]->AddColumn("", 0.2f);
    m_hLocked[type] = m_pServerList[type]->AddColumn("", 0.2f);
    m_hName[type] = m_pServerList[type]->AddColumn(_("Name"), 0.2f);
    m_hPlayers[type] = m_pServerList[type]->AddColumn(_("Players"), 0.2f);
    m_hPing[type] = m_pServerList[type]->AddColumn(_("Ping"), 0.2f);
    m_hGame[type] = m_pServerList[type]->AddColumn(_("Gamemode"), 0.2f);

    // NB. SetColumnWidth seems to start from 0
    m_pServerList[type]->SetColumnWidth(m_hVersion[type], 25, false);
    m_pServerList[type]->SetColumnWidth(m_hLocked[type], 16, false);
    m_pServerList[type]->SetColumnWidth(m_hPlayers[type], 70, false);
    m_pServerList[type]->SetColumnWidth(m_hPing[type], 35, false);

    // We give Name and Gamemode 65% and 35% of the remaining length respectively
    float fRemainingWidth = fWidth - 25 - 16 - 70 - 35 - 50;            // All the fixed sizes plus 50 for the scrollbar

    m_pServerList[type]->SetColumnWidth(m_hGame[type], fRemainingWidth * 0.35, false);
    m_pServerList[type]->SetColumnWidth(m_hName[type], fRemainingWidth * 0.65, false);

    // Server player list;
    fX = fX + fWidth;
    fHeight = m_WidgetSize.fY - SB_SMALL_SPACER - SB_BACK_BUTTON_SIZE_Y - fY - TAB_SIZE_Y;
    m_pServerPlayerList[type] = reinterpret_cast<CGUIGridList*>(pManager->CreateGridList(m_pTab[type]));
    m_pServerPlayerList[type]->SetPosition(CVector2D(fX, fY), false);
    m_pServerPlayerList[type]->SetSize(CVector2D(fPlayerListSizeX, fHeight), false);
    m_pServerPlayerList[type]->SetIgnoreTextSpacer(true);
    // Player List Columns
    m_hPlayerName[type] = m_pServerPlayerList[type]->AddColumn(_("Player list"), 0.75f);

    // Filters
    float fLineHeight = SB_BACK_BUTTON_SIZE_Y / 2;
    fX = SB_SMALL_SPACER;
    fY = m_WidgetSize.fY - SB_SMALL_SPACER / 2 - SB_BACK_BUTTON_SIZE_Y - TAB_SIZE_Y;

    // Include label
    m_pLabelInclude[type] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTab[type], _("Include:")));
    m_pLabelInclude[type]->SetPosition(CVector2D(fX, fY), false);
    m_pLabelInclude[type]->AutoSize(m_pLabelInclude[type]->GetText().c_str());

    // Include checkboxes
    fX = fX + pManager->GetTextExtent(m_pLabelInclude[type]->GetText().c_str()) + SB_SPACER;
    m_pIncludeEmpty[type] = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTab[type], _("Empty"), true));
    m_pIncludeEmpty[type]->SetPosition(CVector2D(fX, fY), false);
    m_pIncludeEmpty[type]->AutoSize(m_pIncludeEmpty[type]->GetText().c_str(), 20.0f);
    m_pIncludeEmpty[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnFilterChanged, this));

    fX = fX + 20.0f + pManager->GetTextExtent(m_pIncludeEmpty[type]->GetText().c_str()) + SB_SPACER;
    m_pIncludeFull[type] = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTab[type], _("Full"), true));
    m_pIncludeFull[type]->SetPosition(CVector2D(fX, fY), false);
    m_pIncludeFull[type]->AutoSize(m_pIncludeFull[type]->GetText().c_str(), 20.0f);
    m_pIncludeFull[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnFilterChanged, this));

    fX = fX + 20.0f + pManager->GetTextExtent(m_pIncludeFull[type]->GetText().c_str()) + SB_SPACER;
    m_pIncludeLocked[type] = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTab[type], _("Locked"), true));
    m_pIncludeLocked[type]->SetPosition(CVector2D(fX, fY), false);
    m_pIncludeLocked[type]->AutoSize(m_pIncludeLocked[type]->GetText().c_str(), 20.0f);
    m_pIncludeLocked[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnFilterChanged, this));

#if MTA_DEBUG
    if (type != ServerBrowserTypes::LAN)
#else
    if (type != ServerBrowserTypes::INTERNET && type != ServerBrowserTypes::LAN)
#endif
    {
        fX = fX + 20.0f + pManager->GetTextExtent(m_pIncludeLocked[type]->GetText().c_str()) + SB_SPACER;
        m_pIncludeOffline[type] = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTab[type], _("Offline"), true));
        m_pIncludeOffline[type]->SetPosition(CVector2D(fX, fY), false);
        m_pIncludeOffline[type]->AutoSize(m_pIncludeOffline[type]->GetText().c_str(), 20.0f);
        m_pIncludeOffline[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnFilterChanged, this));

        fX = fX + 20.0f + pManager->GetTextExtent(m_pIncludeOffline[type]->GetText().c_str()) + SB_SPACER;
    }
    else
    {
        m_pIncludeOffline[type] = NULL;
        fX = fX + 20.0f + pManager->GetTextExtent(m_pIncludeLocked[type]->GetText().c_str()) + SB_SPACER;
    }

    m_pIncludeOtherVersions[type] = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTab[type], _("Other Versions"), false));
    m_pIncludeOtherVersions[type]->SetPosition(CVector2D(fX, fY), false);
    m_pIncludeOtherVersions[type]->AutoSize(m_pIncludeOtherVersions[type]->GetText().c_str(), 20.0f);
    m_pIncludeOtherVersions[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnFilterChanged, this));
    m_pIncludeOtherVersions[type]->SetSelected(true);

    // Status bar
    fX = 5;
    fY = fY + fLineHeight;

    m_pServerListStatus[type] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTab[type], ""));
    m_pServerListStatus[type]->SetPosition(CVector2D(fX, fY));
    m_pServerListStatus[type]->SetSize(CVector2D(m_WidgetSize.fX, fLineHeight), true);
    m_pServerListStatus[type]->MoveToBack();
    m_pServerListStatus[type]->SetZOrderingEnabled(false);
    m_pServerListStatus[type]->SetProperty("MousePassThroughEnabled", "True");
    m_pServerListStatus[type]->SetProperty("DistributeCapturedInputs", "True");

    // Back button
    fX = m_WidgetSize.fX - fPlayerListSizeX - SB_SMALL_SPACER;
    fY = m_WidgetSize.fY - SB_SMALL_SPACER - SB_BACK_BUTTON_SIZE_Y - TAB_SIZE_Y;
    m_pButtonBack[type] = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTab[type], _("Back")));
    m_pButtonBack[type]->SetPosition(CVector2D(fX, fY), false);
    m_pButtonBack[type]->SetSize(CVector2D(fPlayerListSizeX, SB_BACK_BUTTON_SIZE_Y), false);
    m_pButtonBack[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnBackClick, this));
    m_pButtonBack[type]->SetZOrderingEnabled(false);

    m_pButtonGeneralHelp[type] = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTab[type], _("Help")));
    m_pButtonGeneralHelp[type]->SetPosition(CVector2D(fX - fPlayerListSizeX - 10, fY), false);
    m_pButtonGeneralHelp[type]->SetSize(CVector2D(fPlayerListSizeX, SB_BACK_BUTTON_SIZE_Y), false);
    m_pButtonGeneralHelp[type]->SetClickHandler(GUI_CALLBACK(&CServerBrowser::OnGeneralHelpClick, this));
    m_pButtonGeneralHelp[type]->SetZOrderingEnabled(false);

    // Disable resizing of the first and second columns (Version & Locked)
    m_pServerList[type]->SetColumnSegmentSizingEnabled(0, false);
    m_pServerList[type]->SetColumnSegmentSizingEnabled(1, false);

    // Bring our combo box to front so it doesnt get stuck behind server list
    m_pServerList[type]->MoveToBack();
    m_pComboAddressHistory[type]->BringToFront();
    m_pEditAddress[type]->BringToFront();
    m_pComboAddressHistory[type]->SetZOrderingEnabled(false);
    m_pServerList[type]->SetZOrderingEnabled(false);

    // Attach some keyboard events to the serverlist.
    m_pServerList[type]->SetEnterKeyHandler(GUI_CALLBACK(&CServerBrowser::OnDoubleClick, this));
    m_pServerList[type]->SetDoubleClickHandler(GUI_CALLBACK(&CServerBrowser::OnDoubleClick, this));
    m_pServerList[type]->SetKeyDownHandler(GUI_CALLBACK_KEY(&CServerBrowser::OnServerListKeyDown, this));

    // If any of the include checkboxes overlap with the help/back buttons, we move them down - next to the status bar.
    CVector2D                  vecButtonPos = m_pButtonGeneralHelp[type]->GetPosition();
    float                      fMoveX = vecButtonPos.fX;
    std::vector<CGUICheckBox*> pCheckBox;
    pCheckBox.push_back(m_pIncludeOtherVersions[type]);
    pCheckBox.push_back(m_pIncludeOffline[type]);
    pCheckBox.push_back(m_pIncludeLocked[type]);
    for (std::vector<CGUICheckBox*>::iterator iter = pCheckBox.begin(); iter != pCheckBox.end(); ++iter)
        if ((*iter) != NULL && (((*iter)->GetPosition(false).fX + (*iter)->GetSize(false).fX) > vecButtonPos.fX))
            (*iter)->SetPosition(CVector2D(fMoveX += -SB_SPACER - (*iter)->GetSize(false).fX, m_pServerListStatus[type]->GetPosition().fY));
}

void CServerBrowser::DeleteTab(ServerBrowserType type)
{
    delete m_pButtonConnect[type];
    delete m_pButtonRefresh[type];

    delete m_pEditAddress[type];
    delete m_pComboSearchType[type];
    delete m_pSearchTypeIcon[type];
    delete m_pServerSearchIcon[type];
    delete m_pButtonInfoIcon[type];
    delete m_pButtonConnectIcon[type];

    // Remove handler to prevent unwanted events during delete
    m_pComboAddressHistory[type]->SetDropListRemoveHandler(NULL);
    delete m_pComboAddressHistory[type];

    delete m_pAddressFavoriteIcon[type];
    if (type == ServerBrowserTypes::RECENTLY_PLAYED)
        delete m_pRemoveFromRecentIcon[type];
    delete m_pEditSearch[type];
    delete m_pButtonRefreshIcon[type];

    delete m_pLabelInclude[type];
    delete m_pIncludeEmpty[type];
    delete m_pIncludeFull[type];
    delete m_pIncludeLocked[type];
    delete m_pIncludeOtherVersions[type];

    if (m_pIncludeOffline[type])
    {
        delete m_pIncludeOffline[type];
    }

    delete m_pServerPlayerList[type];

    delete m_pServerListStatus[type];

    delete m_pServerList[type];
    delete m_pTab[type];
}

ServerBrowserType CServerBrowser::GetCurrentServerBrowserTypeForSave()
{
    // If current tab is temporary, then save the one used befor it
    if (m_uiIsUsingTempTab)
        return m_BeforeTempServerBrowserType;

    return GetCurrentServerBrowserType();
}

ServerBrowserType CServerBrowser::GetCurrentServerBrowserType()
{
    ServerBrowserType currentServerBrowserType;

    if (m_pPanel->IsTabSelected(m_pTab[ServerBrowserTypes::FAVOURITES]))
    {
        currentServerBrowserType = ServerBrowserTypes::FAVOURITES;
    }
    else if (m_pPanel->IsTabSelected(m_pTab[ServerBrowserTypes::RECENTLY_PLAYED]))
    {
        currentServerBrowserType = ServerBrowserTypes::RECENTLY_PLAYED;
    }
    else if (m_pPanel->IsTabSelected(m_pTab[ServerBrowserTypes::LAN]))
    {
        currentServerBrowserType = ServerBrowserTypes::LAN;
    }
    else
    {
        currentServerBrowserType = ServerBrowserTypes::INTERNET;
    }

    return currentServerBrowserType;
}

void CServerBrowser::Update()
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    CServerList*      pList = GetServerList(Type);

    if (m_bFocusTextEdit)
    {
        m_bFocusTextEdit = false;
        // Focus the text edit
        m_pEditAddress[Type]->Activate();
        m_pEditAddress[Type]->SetCaretAtEnd();
    }

    // Update the current server list class
    if (IsVisible())
    {
        pList->Pulse();
        m_ServersHistory.Pulse();
    }

    // If an update is needed, the serverbrowser is visible and it has gone some time since last update
    if ((pList->IsUpdated() || m_PrevServerBrowserType != Type) && m_ulLastUpdateTime < CClientTime::GetTime() - SERVER_BROWSER_UPDATE_INTERVAL)
    {
        // Update the GUI
        UpdateServerList(Type, Type == ServerBrowserTypes::RECENTLY_PLAYED);

        UpdateHistoryList();

        // Set the status string
        SetStatusText(pList->GetStatus());

        // Update last time updated
        m_ulLastUpdateTime = CClientTime::GetTime();

        // Update last viewed tab
        m_PrevServerBrowserType = Type;

        UpdateSelectedServerPlayerList(Type);
    }

    // Flash searchbox if needed
    if (m_FlashSearchBox[Type].uiCount)
    {
        if (m_FlashSearchBox[Type].uiNextTime < GetTickCount32())
        {
            m_pEditSearch[Type]->SetVisible((m_FlashSearchBox[Type].uiCount-- & (8 + 1)) ? true : false);
            m_FlashSearchBox[Type].uiNextTime = GetTickCount32() + 100;
        }
    }
}

void CServerBrowser::SetVisible(bool bVisible)
{
    if (m_uiIsUsingTempTab)
    {
        // Stop using temp tab now
        m_pPanel->SetSelectedTab(m_pTab[GetCurrentServerBrowserTypeForSave()]);
        m_uiIsUsingTempTab = 0;
    }

    m_pTopWindow->SetZOrderingEnabled(true);
    m_pTopWindow->SetVisible(bVisible);
    m_pTopWindow->BringToFront();
    m_pTopWindow->SetZOrderingEnabled(false);

    // Are we making this window visible?
    if (bVisible)
    {
        bool bAutoRefresh = false;
        CVARS_GET("auto_refresh_browser", bAutoRefresh);

        // Start loading all servers (if needed).
        for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
        {
            // Don't refresh Internet unless it's activated or needed.
            if (i != ServerBrowserTypes::INTERNET || m_bFirstTimeBrowseServer || bAutoRefresh)
            {
                m_pServerListStatus[i]->SetText(_("Loading..."));
                m_iSelectedServer[i] = -1;
                GetServerList((ServerBrowserType)i)->Refresh();
            }
        }

        CreateHistoryList();

        if (m_bFirstTimeBrowseServer)
            m_bFirstTimeBrowseServer = false;

        // Set the first item as our starting address
        if (m_pComboAddressHistory[ServerBrowserTypes::INTERNET]->GetItemCount() > 0)
        {
            std::string strHistoryText = (const char*)m_pComboAddressHistory[ServerBrowserTypes::INTERNET]->GetItemByIndex(0)->GetData();
            SetAddressBarText("mtasa://" + strHistoryText);
        }

        // Focus the address bar for power users
        ServerBrowserType Type = GetCurrentServerBrowserType();
        m_pEditAddress[Type]->Activate();
        m_pEditAddress[Type]->SetCaretAtEnd();

        // Flash search box if it is not empty
        for (uint i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
        {
            m_FlashSearchBox[i].uiCount = m_pEditSearch[i]->GetText().empty() ? 0 : 15;
            m_FlashSearchBox[Type].uiNextTime = 0;
        }
    }
    else
    {
        // Hide information windows.
        m_pGeneralHelpWindow->SetVisible(false);
        m_pQuickConnectHelpWindow->SetVisible(false);
        CServerInfo::GetSingletonPtr()->Hide();

        for (uint i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
        {
            m_FlashSearchBox[i].uiCount = 0;
        }
    }
}

bool CServerBrowser::IsVisible()
{
    return m_pTopWindow && m_pTopWindow->IsVisible();
}

//
// Check if the client can see another server version in the browser
//
bool CServerBrowser::CanBrowseVersion(const SString& strVersion)
{
    // If in deny list, then no
    if (MapContains(m_blockedVersionMap, strVersion))
        return false;

    // If not in allow list then no (unless allow list is empty)
    if (!m_allowedVersionMap.empty() && !MapContains(m_allowedVersionMap, strVersion))
        return false;

    // otherwise yes
    return true;
}

void CServerBrowser::UpdateServerList(ServerBrowserType Type, bool bClearServerList)
{
    GetVersionUpdater()->GetBrowseVersionMaps(m_blockedVersionMap, m_allowedVersionMap);

#if MTA_DEBUG
    // Selecting 'Nightly update' in debug build will show other versions
    SString strUpdateBuildType;
    CVARS_GET("update_build_type", strUpdateBuildType);
    if (strUpdateBuildType == "2")
    {
        m_blockedVersionMap.clear();
        m_allowedVersionMap.clear();
    }
#endif

    // Setting this in coreconfig will show other versions
    bool bForceBrowseOtherVersions = false;
    CVARS_GET("force_browse_other_versions", bForceBrowseOtherVersions);
    if (bForceBrowseOtherVersions)
    {
        m_blockedVersionMap.clear();
        m_allowedVersionMap.clear();
    }

    // Save sort info
    uint          uiSortColumn;
    SortDirection sortDirection;
    m_pServerList[Type]->GetSort(uiSortColumn, sortDirection);

    // Disable sorting
    m_pServerList[Type]->Sort(uiSortColumn, SortDirections::None);

    // Get the appropriate server list
    CServerList* pList = GetServerList(Type);

    bool bGetListsCleared = pList->GetRevision() != m_pServerListRevision[Type] || bClearServerList;
    if (bGetListsCleared)
    {
        m_pServerListRevision[Type] = pList->GetRevision();

        // Clear current list
        m_pServerList[Type]->Clear();

        // Clear the player lists
        m_pServerPlayerList[Type]->Clear();
    }

    bool didUpdateRowIndices = false;

    // Loop the server list
    for (CServerListIterator it = pList->IteratorBegin(); it != pList->IteratorEnd(); it++)
    {
        CServerListItem* pServer = *it;

        if (bGetListsCleared)
            pServer->iRowIndex = -1;

        // Find info from server cache for favourites and recent
        if (Type == ServerBrowserType::FAVOURITES || Type == ServerBrowserType::RECENTLY_PLAYED)
            GetServerCache()->GetServerCachedInfo(pServer);

        // Add/update/remove the item to the list
        if (pServer->revisionInList[Type] != pServer->uiRevision || bClearServerList)
        {
            if (!didUpdateRowIndices)
            {
                UpdateRowIndexMembers(Type);
                didUpdateRowIndices = true;
            }

            pServer->revisionInList[Type] = pServer->uiRevision;
            AddServerToList(pServer, Type);
        }
    }
    bool              bIncludeOtherVersions = m_pIncludeOtherVersions[Type]->GetSelected();
    ServerBrowserType type = Type;

    if (bIncludeOtherVersions)
    {
        m_pServerList[type]->SetColumnWidth(1, 34, false);
    }
    else
    {
        m_pServerList[type]->SetColumnWidth(1, 0.03f, true);
    }

    // Re-enable sorting
    m_pServerList[Type]->Sort(uiSortColumn, sortDirection);

    m_pServerList[Type]->ForceUpdate();
    pList->SetUpdated(false);

    if (Type == ServerBrowserTypes::RECENTLY_PLAYED)
        m_pRemoveFromRecentIcon[Type]->SetAlpha(0.3f);
}

void CServerBrowser::CreateHistoryList()
{
    // Clear our combo boxes first
    for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
    {
        m_pComboAddressHistory[i]->Clear();
    }

    bool bEmpty = true;

    // Populate our history
    for (CServerListReverseIterator it = m_ServersHistory.ReverseIteratorBegin(); it != m_ServersHistory.ReverseIteratorEnd(); it++)
    {
        CServerListItem* pServer = *it;
        if (pServer->strEndpoint)
        {
            bEmpty = false;
            for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
            {
                m_pComboAddressHistory[i]->AddItem(("mtasa://" + pServer->strEndpoint).c_str())->SetData(pServer->strEndpoint.c_str());
            }
        }
    }

    // If we had no history, import it from our old quick connect
    if (bEmpty)
    {
        std::string strAddress;
        CVARS_GET("qc_host", strAddress);

        if (!strAddress.empty())
        {
            std::string strPort;
            CVARS_GET("qc_port", strPort);

            if (!strPort.empty())
            {
                in_addr Address;
                if (CServerListItem::Parse(strAddress.c_str(), Address))
                {
                    m_ServersHistory.AddUnique(Address, atoi(strPort.c_str()));
                    CreateHistoryList();            // Restart with our new list.
                    return;
                }
            }
        }
    }

    m_ServersHistory.Refresh();
}

void CServerBrowser::UpdateHistoryList()
{
    // Assume our type is 0, then update all fields when appropriate
    unsigned int Type = 0;

    // Look through our combo box and process each item
    CGUIComboBox* pServerList = m_pComboAddressHistory[Type];
    int           iRowCount = pServerList->GetItemCount();
    for (int i = 0; i < iRowCount; i++)
    {
        CGUIListItem* item = pServerList->GetItemByIndex(i);
        const char*   szAddress = (const char*)item->GetData();

        // Find our corresponding server item
        for (CServerListIterator it = m_ServersHistory.IteratorBegin(); it != m_ServersHistory.IteratorEnd(); it++)
        {
            CServerListItem* pServer = *it;
            if (pServer->strEndpoint == szAddress)
            {
                if (pServer->strEndpoint != pServer->strName)            // Do we have a real name for the server?
                {
                    for (unsigned int index = 0; index < SERVER_BROWSER_TYPE_COUNT; index++)
                    {
                        m_pComboAddressHistory[index]->SetItemText(i, ("mtasa://" + pServer->strEndpoint + " | " + pServer->strName).c_str());
                    }
                }
                break;
            }
        }
    }
}

void CServerBrowser::AddServerToList(CServerListItem* pServer, const ServerBrowserType Type)
{
    bool bIncludeEmpty = m_pIncludeEmpty[Type]->GetSelected();
    bool bIncludeFull = m_pIncludeFull[Type]->GetSelected();
    bool bIncludeLocked = m_pIncludeLocked[Type]->GetSelected();
    bool bIncludeOffline = m_pIncludeOffline[Type] && m_pIncludeOffline[Type]->GetSelected();
    bool bIncludeOtherVersions = m_pIncludeOtherVersions[Type]->GetSelected();
    bool bServerSearchFound = true;

    std::string strServerSearchText = m_pEditSearch[Type]->GetText();
    int         iCurrentSearchType = m_pComboSearchType[Type]->GetSelectedItemIndex();
    if (!strServerSearchText.empty())
    {
        if (iCurrentSearchType == SearchTypes::SERVERS)
        {
            // Search for the search text in the servername
            SString strServerName = pServer->strSearchableName;
            bServerSearchFound = strServerName.ContainsI(strServerSearchText);
        }
        else if (iCurrentSearchType == SearchTypes::PLAYERS)
        {
            bServerSearchFound = false;

            if (pServer->nPlayers > 0)
            {
                // Search for the search text in the names of the players in the server
                for (unsigned int i = 0; i < pServer->vecPlayers.size(); i++)
                {
                    SString strPlayerName = pServer->vecPlayers[i];

                    if (strPlayerName.ContainsI(strServerSearchText))
                    {
                        bServerSearchFound = true;
                        int k = m_pServerPlayerList[Type]->AddRow(true);
                        m_pServerPlayerList[Type]->SetItemText(k, m_hPlayerName[Type], strPlayerName.c_str(), false, false, true);
                    }
                }
            }
        }
    }

    //
    // Add or remove ?
    //

    bool bAddServer;

    bool bIsOtherVersion = (!pServer->strVersion.empty()) && (pServer->strVersion != MTA_DM_ASE_VERSION);
    bool bIsOffline = (pServer->bSkipped) || (pServer->MaybeWontRespond());
    bool bWasGoodNowFailing = (!bIsOffline) && (pServer->bMaybeOffline);
    bool bLowQuality = (pServer->GetDataQuality() <= SERVER_INFO_ASE_0);
    bool bIsEmpty = (pServer->nPlayers == 0) && (pServer->nMaxPlayers != 0);
    bool bIsFull = (pServer->nPlayers >= pServer->nMaxPlayers) && (pServer->nMaxPlayers != 0);
    bool bIsLocked = pServer->bPassworded;
    bool bIsBlockedVersion = bIsOtherVersion && !CanBrowseVersion(pServer->strVersion);
    bool bIsBlockedServer = (pServer->uiMasterServerSaysRestrictions & RESTRICTION_BLOCK_SERVER) != false;

    if ((!pServer->strVersion.empty() || bIsOffline) && (!bLowQuality || bIsOffline) && (!bIsEmpty || bIncludeEmpty) && (!bIsFull || bIncludeFull) &&
        (!bIsLocked || bIncludeLocked) && (!bIsOffline || bIncludeOffline || bWasGoodNowFailing) && (!bIsOtherVersion || bIncludeOtherVersions) &&
        (!bIsBlockedVersion) && (!bIsBlockedServer) && (bServerSearchFound))
    {
        bAddServer = true;
    }
    else
    {
        bAddServer = false;
    }

    if (!bAddServer)
    {
        //
        // Remove server from list
        //
        int iIndex = pServer->iRowIndex;
        if (iIndex != -1)
        {
            m_pServerList[Type]->RemoveRow(iIndex);
            pServer->iRowIndex = -1;
            UpdateRowIndexMembers(Type);
        }
    }
    else
    {
        //
        // Add/update server in list
        //

        // Get existing row or create a new row if not found
        int iIndex = pServer->iRowIndex;
        if (iIndex == -1)
        {
            iIndex = m_pServerList[Type]->AddRow(true);
            pServer->iRowIndex = iIndex;
        }

        const SString strVersion = !bIncludeOtherVersions ? "" : pServer->strVersion;
        const SString strVersionSortKey = pServer->strVersionSortKey + pServer->strTieBreakSortKey;

        const SString strVerified = pServer->isStatusVerified ? "" : "*";
        const SString strPlayers = pServer->nMaxPlayers == 0 ? "" : SString("%d / %d %s", pServer->nPlayers, pServer->nMaxPlayers, *strVerified);
        const SString strPlayersSortKey = SString("%04d-", pServer->nMaxPlayers ? pServer->nPlayers + 1 : 0) + pServer->strTieBreakSortKey;

        const SString strPing = pServer->nPing == 9999 ? "" : SString("%d", pServer->nPing);
        const SString strPingSortKey = SString("%04d-", pServer->nPing) + pServer->strTieBreakSortKey;

        // The row index could change at any point here if list sorting is enabled
        iIndex = m_pServerList[Type]->SetItemText(iIndex, m_hVersion[Type], strVersion, false, false, true, strVersionSortKey);
        iIndex = m_pServerList[Type]->SetItemText(iIndex, m_hName[Type], pServer->strName, false, false, true, pServer->strNameSortKey);
        iIndex = m_pServerList[Type]->SetItemText(iIndex, m_hGame[Type], pServer->strGameMode, false, false, true);
        iIndex = m_pServerList[Type]->SetItemText(iIndex, m_hPlayers[Type], strPlayers, false, false, true, strPlayersSortKey);
        iIndex = m_pServerList[Type]->SetItemText(iIndex, m_hPing[Type], strPing, false, false, true, strPingSortKey);

        // Locked icon
        m_pServerList[Type]->SetItemImage(iIndex, m_hLocked[Type], pServer->bPassworded ? m_pLockedIcon : NULL);

        // Data for later use
        m_pServerList[Type]->SetItemData(iIndex, DATA_PSERVER, (void*)pServer);

        // Colours
        SColor color = SColorRGBA(255, 255, 255, 255);

#if MTA_DEBUG
        if (pServer->uiCacheNoReplyCount)
            color.R /= 2;
        if (pServer->bMasterServerSaysNoResponse)
            color.G /= 2;
        if (pServer->uiMasterServerSaysRestrictions)
            color.B /= 4;
#endif
        if (bIsOtherVersion)
            color.B /= 2;
        if (pServer->bMaybeOffline)
            color.A = color.A / 3 * 2;
        if (pServer->bSkipped)
            color.A = color.A / 4 * 3;

        m_pServerList[Type]->SetItemColor(iIndex, m_hVersion[Type], color.R, color.G, color.B, color.A);
        m_pServerList[Type]->SetItemColor(iIndex, m_hLocked[Type], color.R, color.G, color.B, color.A);
        m_pServerList[Type]->SetItemColor(iIndex, m_hName[Type], color.R, color.G, color.B, color.A);
        m_pServerList[Type]->SetItemColor(iIndex, m_hPlayers[Type], color.R, color.G, color.B, color.A);
        m_pServerList[Type]->SetItemColor(iIndex, m_hPing[Type], color.R, color.G, color.B, color.A);
        m_pServerList[Type]->SetItemColor(iIndex, m_hGame[Type], color.R, color.G, color.B, color.A);

        if (!pServer->isStatusVerified)
        {
            SColor orange = SColorRGBA(230, 200, 180, color.A);
            m_pServerList[Type]->SetItemColor(iIndex, m_hPlayers[Type], orange.R, orange.G, orange.B, orange.A);
        }

        // If the index was modified from the original, then update all indexes because it means there was some sort
        if (pServer->iRowIndex != iIndex)
            UpdateRowIndexMembers(Type);
    }
}

bool CServerBrowser::RemoveSelectedServerFromRecentlyPlayedList()
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    CServerListItem*  pServer;
    CServerList*      pHistoryList;
    int               iSelectedItem;

    if (Type != ServerBrowserTypes::RECENTLY_PLAYED)
        return false;

    pServer = FindSelectedServer(Type);

    if (!pServer)
        return false;

    pHistoryList = GetRecentList();
    iSelectedItem = m_pServerList[Type]->GetSelectedItemRow();

    pHistoryList->Remove(pServer->Address, pServer->usGamePort);
    m_pServerList[Type]->RemoveRow(iSelectedItem);
    SaveRecentlyPlayedList();

    m_pRemoveFromRecentIcon[Type]->SetAlpha(0.3f);

    return true;
}

CServerList* CServerBrowser::GetServerList(ServerBrowserType Type)
{
    if (Type == ServerBrowserTypes::FAVOURITES)
        return &m_ServersFavourites;
    else if (Type == ServerBrowserTypes::INTERNET)
        return &m_ServersInternet;
    else if (Type == ServerBrowserTypes::LAN)
        return &m_ServersLAN;
    else if (Type == ServerBrowserTypes::RECENTLY_PLAYED)
        return &m_ServersRecent;

    assert(false);
    return NULL;
}

bool CServerBrowser::OnClick(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();

    if (pElement == m_pServerPlayerList[Type] && m_pServerPlayerList[Type]->GetSelectedCount() >= 1)
    {
        // Get the selected row of the player gridlist
        int         iSelectedIndex = m_pServerPlayerList[Type]->GetSelectedItemRow();
        std::string strSelectedPlayerName = m_pServerPlayerList[Type]->GetItemText(iSelectedIndex, m_hPlayerName[Type]);

        // Walk the server list looking for the player on a server
        CServerList*        pList = GetServerList(Type);
        CServerListIterator i, i_b = pList->IteratorBegin(), i_e = pList->IteratorEnd();
        for (i = i_b; i != i_e; i++)
        {
            CServerListItem* pServer = *i;

            for (unsigned int j = 0; j < pServer->vecPlayers.size(); j++)
            {
                std::string strPlayerName = pServer->vecPlayers[j].c_str();
                if (strPlayerName.compare(strSelectedPlayerName) == 0)
                {
                    // We found the server on which the player is
                    // Walk the server gridlist looking for the server host to get the row index
                    for (int k = 0; k < m_pServerList[Type]->GetRowCount(); k++)
                    {
                        if (pServer == ((CServerListItem*)m_pServerList[Type]->GetItemData(k, DATA_PSERVER)))
                        {
                            // We found the index, select it
                            m_pServerList[Type]->SetSelectedItem(k, DATA_PSERVER, true);

                            // save the selected server
                            m_iSelectedServer[Type] = iSelectedIndex;

                            return true;
                        }
                    }
                }
            }
        }
    }

    // If there is one item selected
    if (m_pServerList[Type]->GetSelectedCount() >= 1)
    {
        // Clear the player list
        m_pServerPlayerList[Type]->Clear();

        // Get the selected row
        int iSelectedIndex = m_pServerList[Type]->GetSelectedItemRow();

        CServerListItem* pServer = FindSelectedServer(Type);
        if (pServer)
        {
            // We found the server, add all the players
            for (unsigned int j = 0; j < pServer->vecPlayers.size(); j++)
            {
                int k = m_pServerPlayerList[Type]->AddRow();
                m_pServerPlayerList[Type]->SetItemText(k, m_hPlayerName[Type], pServer->vecPlayers[j].c_str());
            }

            if (pServer->nPlayers && !pServer->vecPlayers.size())
            {
                int k = m_pServerPlayerList[Type]->AddRow();
                m_pServerPlayerList[Type]->SetItemText(k, m_hPlayerName[Type], _("  ..loading.."));
            }

            SetAddressBarText("mtasa://" + pServer->strEndpoint);
            m_pLabelAddressDescription[Type]->SetVisible(false);
        }

        // save the selected server
        m_iSelectedServer[Type] = iSelectedIndex;

        if (Type == ServerBrowserTypes::RECENTLY_PLAYED)
            m_pRemoveFromRecentIcon[Type]->SetAlpha(1.0f);
    }
    else
    {
        if (Type == ServerBrowserTypes::RECENTLY_PLAYED)
            m_pRemoveFromRecentIcon[Type]->SetAlpha(0.3f);
    }
    return true;
}

bool CServerBrowser::OnDoubleClick(CGUIElement* pElement)
{
    return ConnectToSelectedServer();
}

bool CServerBrowser::OnConnectClick(CGUIElement* pElement)
{
    unsigned short usPort;
    std::string    strHost, strNick, strPassword;
    SString        strURI = m_pEditAddress[GetCurrentServerBrowserType()]->GetText();

    // Trim leading spaces from the URI
    strURI = strURI.TrimStart(" ");

    // Ensure we have something entered
    if (strURI.size() == 0 || strURI == "mtasa://")
    {
        CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC70"), _("No address specified!"), MB_BUTTON_OK | MB_ICON_INFO);
        return true;
    }

    // Ensure that the protocol is mtasa://
    size_t iProtocolEnd = strURI.find("://");
    if (iProtocolEnd == -1)
    {
        strURI = "mtasa://" + strURI;
        SetAddressBarText(strURI);
    }
    else if (strURI.substr(0, iProtocolEnd) != "mtasa")            // Is it the mtasa:// protocol?  Don't want noobs trying http etc
    {
        CCore::GetSingleton().ShowMessageBox(_("Unknown protocol") + _E("CC71"), _("Please use the mtasa:// protocol!"), MB_BUTTON_OK | MB_ICON_INFO);
        return true;
    }

    g_pCore->GetConnectParametersFromURI(strURI.c_str(), strHost, usPort, strNick, strPassword);

    // Valid nick?
    if (!CCore::GetSingleton().IsValidNick(strNick.c_str()))
    {
        CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC72"), _("Invalid nickname! Please go to Settings and set a new one!"),
                                             MB_BUTTON_OK | MB_ICON_INFO);
        return true;
    }

    // If our password is empty, try and grab a saved password
    if (strPassword.empty())
    {
        strPassword = GetServerPassword(strHost + ":" + SString("%u", usPort));
    }

    // Start the connect
    CCore::GetSingleton().GetConnectManager()->Connect(strHost.c_str(), usPort, strNick.c_str(), strPassword.c_str(), true);

    return true;
}

void CServerBrowser::NotifyServerExists(in_addr Address, ushort usPort)
{
    // If the connect button was pressed, and the server exists, add it to the history
    CServerList* pHistoryList = GetHistoryList();
    pHistoryList->Remove(Address, usPort);
    pHistoryList->AddUnique(Address, usPort);
    while (pHistoryList->GetServerCount() > 11)
    {
        CServerListItem* pLast = *pHistoryList->IteratorBegin();
        pHistoryList->Remove(pLast->Address, pLast->usGamePort);
    }
    CreateHistoryList();
    SaveRecentlyPlayedList();
}

void CServerBrowser::CompleteConnect()
{
    CServerBrowser* pBrowser = CServerBrowser::GetSingletonPtr();
    pBrowser->ConnectToSelectedServer();
}

bool CServerBrowser::ConnectToSelectedServer()
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    m_pServerPlayerList[Type]->Clear();

    // If there is one item selected
    if (CServerListItem* pServer = FindSelectedServer(Type))
    {
        if (pServer->bSerials)
        {
            return true;
        }
        // Get the nick from the config
        std::string strNick;
        CVARS_GET("nick", strNick);

        // Valid nick?
        if (!CCore::GetSingleton().IsValidNick(strNick.c_str()))
        {
            CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC73"), _("Invalid nickname! Please go to Settings and set a new one!"),
                                                 MB_BUTTON_OK | MB_ICON_INFO);
            return true;
        }

        // Password buffer
        SString strPassword = "";
        if (pServer->bPassworded)            // The server is passworded, let's try and grab a saved password
        {
            strPassword = GetServerPassword(pServer->GetEndpoint().c_str());

            if (strPassword.empty())            // No password could be found, popup password entry.
            {
                CServerInfo::GetSingletonPtr()->Show(eWindowTypes::SERVER_INFO_PASSWORD, pServer->strHost.c_str(), pServer->usGamePort, "", pServer);
                return true;
            }
        }

        // Start the connect
        CCore::GetSingleton().GetConnectManager()->Connect(pServer->strHost.c_str(), pServer->usGamePort, strNick.c_str(), strPassword);
    }
    else
    {
        CCore::GetSingleton().ShowMessageBox(_("Information") + _E("CC74"), _("You have to select a server to connect to."), MB_BUTTON_OK | MB_ICON_INFO);
    }
    return false;
}

bool CServerBrowser::OnRefreshClick(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();

    GetServerList(Type)->Refresh();

    return true;
}

bool CServerBrowser::OnInfoClick(CGUIElement* pElement)
{
    unsigned short usPort;
    std::string    strHost, strNick, strPassword;
    SString        strURI = m_pEditAddress[GetCurrentServerBrowserType()]->GetText();

    // Trim leading spaces from the URI
    strURI = strURI.TrimStart(" ");

    // Ensure we have something entered
    if (strURI.size() == 0 || strURI == "mtasa://")
    {
        CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC75"), _("No address specified!"), MB_BUTTON_OK | MB_ICON_INFO);
        return true;
    }

    g_pCore->GetConnectParametersFromURI(strURI.c_str(), strHost, usPort, strNick, strPassword);

    CServerInfo::GetSingletonPtr()->Show(eWindowTypes::SERVER_INFO_RAW, strHost.c_str(), usPort, strPassword.c_str());
    return true;
}

bool CServerBrowser::OnFavouritesClick(CGUIElement* pElement)
{
    unsigned short usPort;
    std::string    strHost, strNick, strPassword;
    std::string    strURI = m_pEditAddress[GetCurrentServerBrowserType()]->GetText();
    g_pCore->GetConnectParametersFromURI(strURI.c_str(), strHost, usPort, strNick, strPassword);

    // If there are more than 0 items selected in the browser
    if (strHost.size() > 0 && usPort)
    {
        in_addr Address;

        CServerListItem::Parse(strHost.c_str(), Address);

        // Do we have this entry already?  If so, remove it
        if (m_ServersFavourites.Remove(Address, usPort))
        {
            SaveFavouritesList();
            UpdateServerList(ServerBrowserTypes::FAVOURITES, true);
            for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
            {
                m_pAddressFavoriteIcon[i]->SetAlpha(0.3f);
            }
            return true;
        }

        if (m_ServersFavourites.AddUnique(Address, usPort))
        {
            SaveFavouritesList();
            UpdateServerList(ServerBrowserTypes::FAVOURITES, true);
            for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
            {
                m_pAddressFavoriteIcon[i]->SetAlpha(1.0f);
            }
        }
    }
    return true;
}

bool CServerBrowser::OnRemoveFromRecentClick(CGUIElement* pElement)
{
    return RemoveSelectedServerFromRecentlyPlayedList();
}

bool CServerBrowser::OnAddressChanged(CGUIElement* pElement)
{
    unsigned short    usPort;
    std::string       strHost, strNick, strPassword;
    ServerBrowserType Type = GetCurrentServerBrowserType();
    std::string       strURI = m_pEditAddress[Type]->GetText();
    g_pCore->GetConnectParametersFromURI(strURI.c_str(), strHost, usPort, strNick, strPassword);

    // Adjust our other address bars to be consistent
    for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
    {
        if ((i != Type) && (strURI != m_pEditAddress[i]->GetText()))
            m_pEditAddress[i]->SetText(strURI.c_str());
    }

    // If this address exists in favourites, change our favourites icon
    CServerListIterator i, i_b = m_ServersFavourites.IteratorBegin(), i_e = m_ServersFavourites.IteratorEnd();
    for (CServerListIterator i = i_b; i != i_e; i++)
    {
        CServerListItem* pServer = *i;
        if (pServer->strHost == strHost && pServer->usGamePort == usPort)
        {
            for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
            {
                m_pAddressFavoriteIcon[i]->SetAlpha(1.0f);
            }
            return true;
        }
    }
    for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
    {
        m_pAddressFavoriteIcon[i]->SetAlpha(0.3f);
    }
    return true;
}

bool CServerBrowser::OnHistorySelected(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    std::string       strHistoryText = (const char*)m_pComboAddressHistory[Type]->GetSelectedItem()->GetData();
    SetAddressBarText("mtasa://" + strHistoryText);
    return true;
}

bool CServerBrowser::OnHistoryDropListRemove(CGUIElement* pElementx)
{
    // Hide quick connect help if visible
    m_pQuickConnectHelpWindow->SetVisible(false);

    // Grab our cursor position
    tagPOINT cursor;
    GetCursorPos(&cursor);

    HWND hookedWindow = CCore::GetSingleton().GetHookedWindow();

    tagPOINT windowPos = {0};
    ClientToScreen(hookedWindow, &windowPos);

    CVector2D vecResolution = CCore::GetSingleton().GetGUI()->GetResolution();
    cursor.x -= windowPos.x;
    cursor.y -= windowPos.y;

    // Get size and screen position of edit control
    CGUIEdit* pEdit = m_pEditAddress[0];
    CVector2D size = pEdit->GetSize();
    CVector2D pos = CalcScreenPosition(pEdit);

    pos.fY += 24;

    // See if cursor is over edit control
    if (cursor.x > pos.fX)
        if (cursor.x < pos.fX + size.fX)
            if (cursor.y > pos.fY)
                if (cursor.y < pos.fY + size.fY)
                    m_bFocusTextEdit = true;            // Focus the text edit next frame

    return true;
}

bool CServerBrowser::OnSearchTypeSelected(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    int               iCurrentSearchType = m_pComboSearchType[Type]->GetSelectedItemIndex();

    m_pSearchTypeIcon[Type]->LoadFromFile(m_szSearchTypePath[iCurrentSearchType]);

    OnSearchDefocused(pElement);

    // Don't bother doing anything if the search bar is empty
    if (m_pEditSearch[Type]->GetText().empty())
        return true;

    UpdateServerList(GetCurrentServerBrowserType(), true);
    return true;
}

bool CServerBrowser::OnBackClick(CGUIElement* pElement)
{
    CMainMenu* pMainMenu = CLocalGUI::GetSingleton().GetMainMenu();

    m_pTopWindow->SetVisible(false);
    pMainMenu->m_bIsInSubWindow = false;

    SaveOptions();

    return true;
}

bool CServerBrowser::OnGeneralHelpClick(CGUIElement* pElement)
{
    if (!m_pGeneralHelpWindow->IsVisible())
    {
        if (GetTickCount64_() - m_llLastGeneralHelpTime > 500)
        {
            CVector2D helpButtonSize = m_pButtonGeneralHelp[0]->GetSize();
            CVector2D helpButtonPos = CalcScreenPosition(m_pButtonGeneralHelp[0]) + CVector2D(0, 24);
            CVector2D generalHelpSize = m_pGeneralHelpWindow->GetSize();
            CVector2D generalHelpPos = helpButtonPos - generalHelpSize + CVector2D(helpButtonSize.fX, 0);
            m_pGeneralHelpWindow->SetPosition(generalHelpPos);
            m_pGeneralHelpWindow->SetVisible(true);
            m_pGeneralHelpWindow->BringToFront();
        }
    }
    else
    {
        m_pGeneralHelpWindow->SetVisible(false);
    }
    return true;
}

bool CServerBrowser::OnMouseClick(CGUIMouseEventArgs Args)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();

    if (Args.pWindow == m_pServerList[ServerBrowserTypes::INTERNET])
    {
        OnClick(m_pServerList[ServerBrowserTypes::INTERNET]);
        return true;
    }
    else if (Args.pWindow == m_pServerList[ServerBrowserTypes::LAN])
    {
        OnClick(m_pServerList[ServerBrowserTypes::LAN]);
        return true;
    }
    else if (Args.pWindow == m_pServerList[ServerBrowserTypes::FAVOURITES])
    {
        OnClick(m_pServerList[ServerBrowserTypes::FAVOURITES]);
        return true;
    }
    else if (Args.pWindow == m_pServerList[ServerBrowserTypes::RECENTLY_PLAYED])
    {
        OnClick(m_pServerList[ServerBrowserTypes::RECENTLY_PLAYED]);
        return true;
    }
    else if (Args.pWindow == m_pServerPlayerList[Type] && m_pComboSearchType[Type]->GetSelectedItemIndex() == SearchTypes::PLAYERS &&
             !m_pEditSearch[Type]->GetText().empty())
    {
        OnClick(m_pServerPlayerList[Type]);
        return true;
    }

    return false;
}

bool CServerBrowser::OnMouseDoubleClick(CGUIMouseEventArgs Args)
{
    if (Args.pWindow == m_pServerList[ServerBrowserTypes::INTERNET])
    {
        OnDoubleClick(m_pServerList[ServerBrowserTypes::INTERNET]);
        return true;
    }
    if (Args.pWindow == m_pServerList[ServerBrowserTypes::LAN])
    {
        OnDoubleClick(m_pServerList[ServerBrowserTypes::LAN]);
        return true;
    }
    else if (Args.pWindow == m_pServerList[ServerBrowserTypes::FAVOURITES])
    {
        OnDoubleClick(m_pServerList[ServerBrowserTypes::FAVOURITES]);
        return true;
    }
    else if (Args.pWindow == m_pServerList[ServerBrowserTypes::RECENTLY_PLAYED])
    {
        OnDoubleClick(m_pServerList[ServerBrowserTypes::RECENTLY_PLAYED]);
        return true;
    }

    return false;
}

bool CServerBrowser::OnFilterChanged(CGUIElement* pElement)
{
    UpdateServerList(GetCurrentServerBrowserType(), true);
    // SaveOptions ( );  Slow

    return true;
}

bool CServerBrowser::OnTabChanged(CGUIElement* pElement)
{
    // Decrement is temp tab counter
    if (m_uiIsUsingTempTab)
        m_uiIsUsingTempTab--;

    SaveOptions();

    OnSearchFocused(pElement);
    OnAddressFocused(pElement);
    OnSearchDefocused(pElement);
    OnAddressDefocused(pElement);
    return true;
}

bool CServerBrowser::OnSearchFocused(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    m_pLabelSearchDescription[Type]->SetVisible(false);
    return true;
}

bool CServerBrowser::OnAddressFocused(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    m_pLabelAddressDescription[Type]->SetVisible(false);
    return true;
}

bool CServerBrowser::OnSearchDefocused(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    std::string       strSearchText = m_pEditSearch[Type]->GetText();
    if (strSearchText == "")
    {
        m_pLabelSearchDescription[Type]->SetVisible(true);
        if (m_pComboSearchType[Type]->GetSelectedItemIndex() == SearchTypes::SERVERS)
            m_pLabelSearchDescription[Type]->SetText(_("Search servers..."));
        else if (m_pComboSearchType[Type]->GetSelectedItemIndex() == SearchTypes::PLAYERS)
            m_pLabelSearchDescription[Type]->SetText(_("Search players..."));
    }
    return true;
}

bool CServerBrowser::OnAddressDefocused(CGUIElement* pElement)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    std::string       strAddressText = m_pEditAddress[Type]->GetText();
    if (strAddressText.empty())
        m_pLabelAddressDescription[Type]->SetVisible(true);

    return true;
}

bool CServerBrowser::OnGeneralHelpDeactivate(CGUIElement* pElement)
{
    m_pGeneralHelpWindow->SetVisible(false);
    m_llLastGeneralHelpTime = GetTickCount64_();
    return true;
}

bool CServerBrowser::LoadServerList(CXMLNode* pNode, const std::string& strTagName, CServerList* pList)
{
    CXMLNode* pSubNode = NULL;
    in_addr   Address;
    int       iPort;

    if (!pNode)
        return false;

    // Loop through all subnodes looking for relevant nodes
    unsigned int uiCount = pNode->GetSubNodeCount();
    for (unsigned int i = 0; i < uiCount; i++)
    {
        pSubNode = pNode->GetSubNode(i);
        if (pSubNode && pSubNode->GetTagName().compare(strTagName) == 0)
        {
            // This node is relevant, so get the attributes we need and add it to the list
            CXMLAttribute* pHostAttribute = pSubNode->GetAttributes().Find("host");
            CXMLAttribute* pPortAttribute = pSubNode->GetAttributes().Find("port");
            if (pHostAttribute && pPortAttribute)
            {
                if (CServerListItem::Parse(pHostAttribute->GetValue().c_str(), Address))
                {
                    iPort = atoi(pPortAttribute->GetValue().c_str());
                    if (iPort > 0)
                        pList->AddUnique(Address, iPort);
                }
            }
        }
    }
    pList->SetUpdated(true);
    return true;
}

void CServerBrowser::SaveRecentlyPlayedList()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr()->GetConfig();
    CXMLNode* pRecent = pConfig->FindSubNode(CONFIG_NODE_SERVER_REC);
    if (!pRecent)
        pRecent = pConfig->CreateSubNode(CONFIG_NODE_SERVER_REC);
    SaveServerList(pRecent, CONFIG_RECENT_LIST_TAG, GetRecentList());

    // Save address history
    CXMLNode* pHistory = pConfig->FindSubNode(CONFIG_NODE_SERVER_HISTORY);
    if (!pHistory)
        pHistory = pConfig->CreateSubNode(CONFIG_NODE_SERVER_HISTORY);
    SaveServerList(pHistory, CONFIG_HISTORY_LIST_TAG, GetHistoryList(), CONNECT_HISTORY_LIMIT);
}

void CServerBrowser::SaveFavouritesList()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr()->GetConfig();
    CXMLNode* pFavourites = pConfig->FindSubNode(CONFIG_NODE_SERVER_FAV);
    if (!pFavourites)
        pFavourites = pConfig->CreateSubNode(CONFIG_NODE_SERVER_FAV);
    SaveServerList(pFavourites, CONFIG_FAVOURITE_LIST_TAG, GetFavouritesList());
}

bool CServerBrowser::SaveServerList(CXMLNode* pNode, const std::string& strTagName, CServerList* pList, unsigned int iLimit)
{
    if (!pNode)
        return false;

    // Start by clearing out all previous nodes
    pNode->DeleteAllSubNodes();

    // Iterate through the list, adding any items to our node
    unsigned int        iProcessed = 0;
    CServerListIterator i, i_b = pList->IteratorBegin(), i_e = pList->IteratorEnd();
    for (CServerListIterator i = i_b; i != i_e; i++)
    {
        if (iLimit && iProcessed == iLimit)
            break;
        CServerListItem* pServer = *i;

        // Add the item to the node
        CXMLNode* pSubNode = pNode->CreateSubNode(strTagName.c_str());
        if (pSubNode)
        {
            CXMLAttribute* pHostAttribute = pSubNode->GetAttributes().Create("host");
            std::string    strHost = pServer->strHost;
            if (!pServer->strHostName.empty())
                strHost = pServer->strHostName;
            pHostAttribute->SetValue(strHost.c_str());

            CXMLAttribute* pPortAttribute = pSubNode->GetAttributes().Create("port");
            pPortAttribute->SetValue(pServer->usGamePort);
        }
        ++iProcessed;
    }
    return true;
}

void CServerBrowser::LoadOptions(CXMLNode* pNode)
{
    if (!pNode)
    {
        // Node does not exist so allow saving
        m_bOptionsLoaded = true;
        return;
    }

    // loop through all subnodes
    unsigned int uiCount = pNode->GetSubNodeCount();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        CXMLNode* pSubNode = pNode->GetSubNode(ui);
        if (pSubNode && pSubNode->GetTagName().compare("list") == 0)
        {
            CXMLAttribute* pListID = pSubNode->GetAttributes().Find("id");
            if (pListID)
            {
                // Check for a valid list ID
                int i = atoi(pListID->GetValue().c_str());
                if (i >= 0 && i < SERVER_BROWSER_TYPE_COUNT)
                {
                    // load all checkbox options
                    CXMLAttribute* pIncludeEmpty = pSubNode->GetAttributes().Find("include_empty");
                    if (pIncludeEmpty)
                        m_pIncludeEmpty[i]->SetSelected(pIncludeEmpty->GetValue().compare("1") == 0);

                    CXMLAttribute* pIncludeFull = pSubNode->GetAttributes().Find("include_full");
                    if (pIncludeFull)
                        m_pIncludeFull[i]->SetSelected(pIncludeFull->GetValue().compare("1") == 0);

                    CXMLAttribute* pIncludeLocked = pSubNode->GetAttributes().Find("include_locked");
                    if (pIncludeLocked)
                        m_pIncludeLocked[i]->SetSelected(pIncludeLocked->GetValue().compare("1") == 0);

                    // CXMLAttribute* pIncludeOtherVersions = pSubNode->GetAttributes ( ).Find ( "include_other_versions" );
                    // if ( pIncludeOtherVersions )
                    //    m_pIncludeOtherVersions[ i ]->SetSelected ( pIncludeOtherVersions->GetValue ( ).compare ( "1" ) == 0 );

                    // load 'include offline' if the checkbox exists
                    if (m_pIncludeOffline[i])
                    {
                        CXMLAttribute* pIncludeOffline = pSubNode->GetAttributes().Find("include_offline");
                        if (pIncludeOffline)
                            m_pIncludeOffline[i]->SetSelected(pIncludeOffline->GetValue().compare("1") == 0);
                    }

                    CXMLAttribute* pDisabled = pSubNode->GetAttributes().Find("disabled");
                    if (pDisabled)
                        m_pTab[i]->SetEnabled(pDisabled->GetValue().compare("1") == 0);

                    // restore the active tab
                    CXMLAttribute* pActiveTab = pSubNode->GetAttributes().Find("active");
                    if (pActiveTab && pActiveTab->GetValue().compare("1") == 0)
                        m_pPanel->SetSelectedTab(m_pTab[i]);

                    // restore the search field contents
                    std::string strSearch = pSubNode->GetTagContent();
                    if (strSearch.length() > 0)
                        m_pEditSearch[i]->SetText(strSearch.c_str());
                }
            }
        }
    }
    m_bOptionsLoaded = true;
}

void CServerBrowser::SaveOptions()
{
    // Check to make sure if the options were loaded yet, if not the 'changed' events might screw up
    if (!m_bOptionsLoaded)
        return;

    CXMLNode* pConfig = CCore::GetSingletonPtr()->GetConfig();
    CXMLNode* pOptions = pConfig->FindSubNode(CONFIG_NODE_SERVER_OPTIONS);
    if (!pOptions)
    {
        pOptions = pConfig->CreateSubNode(CONFIG_NODE_SERVER_OPTIONS);
    }
    else
    {
        // start with a clean node
        pOptions->DeleteAllSubNodes();
    }

    int iCurrentType = GetCurrentServerBrowserTypeForSave();

    // Save the options for all four lists
    for (unsigned int ui = 0; ui < SERVER_BROWSER_TYPE_COUNT; ui++)
    {
        CXMLNode* pSubNode = pOptions->CreateSubNode("list");
        if (pSubNode)
        {
            // ID of the list to save
            CXMLAttribute* pListID = pSubNode->GetAttributes().Create("id");
            pListID->SetValue(ui);

            // Checkboxes
            CXMLAttribute* pIncludeEmpty = pSubNode->GetAttributes().Create("include_empty");
            pIncludeEmpty->SetValue(m_pIncludeEmpty[ui]->GetSelected());

            CXMLAttribute* pIncludeFull = pSubNode->GetAttributes().Create("include_full");
            pIncludeFull->SetValue(m_pIncludeFull[ui]->GetSelected());

            CXMLAttribute* pIncludeLocked = pSubNode->GetAttributes().Create("include_locked");
            pIncludeLocked->SetValue(m_pIncludeLocked[ui]->GetSelected());

            // CXMLAttribute* pIncludeOtherVersions = pSubNode->GetAttributes ( ).Create ( "include_other_versions" );
            // pIncludeOtherVersions->SetValue ( m_pIncludeOtherVersions [ ui ]->GetSelected ( ) );

            // Only recently played & favorites have 'Include offline'
            if (m_pIncludeOffline[ui])
            {
                CXMLAttribute* pIncludeOffline = pSubNode->GetAttributes().Create("include_offline");
                pIncludeOffline->SetValue(m_pIncludeOffline[ui]->GetSelected());
            }

            if (!m_pTab[ui]->IsEnabled())
            {
                CXMLAttribute* pIncludeOffline = pSubNode->GetAttributes().Create("disabled");
                pIncludeOffline->SetValue(!m_pTab[ui]->IsEnabled());
            }

            // Save the active Tab
            if (iCurrentType == ui)
            {
                CXMLAttribute* pActive = pSubNode->GetAttributes().Create("active");
                pActive->SetValue(1);
            }

            // save the search box content
            std::string strSearch = m_pEditSearch[ui]->GetText();
            if (strSearch.length() > 0)
            {
                pSubNode->SetTagContent(strSearch.c_str());
            }
        }
    }
    g_pCore->SaveConfig();
}

void CServerBrowser::SetServerPassword(const std::string& strHost, const std::string& strPassword)
{
    bool bSavedPasswords;
    CVARS_GET("save_server_passwords", bSavedPasswords);
    if (!bSavedPasswords)
        return;

    CXMLNode* pConfig = CCore::GetSingletonPtr()->GetConfig();
    CXMLNode* pServerPasswords = pConfig->FindSubNode(CONFIG_NODE_SERVER_SAVED);
    if (!pServerPasswords)
    {
        pServerPasswords = pConfig->CreateSubNode(CONFIG_NODE_SERVER_SAVED);
    }
    // Check if the server password already exists
    for (unsigned int i = 0; i < pServerPasswords->GetSubNodeCount(); i++)
    {
        CXMLAttributes* pAttributes = &(pServerPasswords->GetSubNode(i)->GetAttributes());
        if (pAttributes->Find("host"))
        {
            if (CXMLAttribute* pHost = pAttributes->Find("host"))
            {
                const std::string& strXMLHost = pHost->GetValue();
                if (strXMLHost == strHost)
                {
                    CXMLAttribute* pPassword = pAttributes->Create("password");
                    pPassword->SetValue(strPassword.c_str());
                    return;
                }
            }
        }
    }

    // Otherwise create the node from scratch
    CXMLNode*      pNode = pServerPasswords->CreateSubNode("server");
    CXMLAttribute* pHostAttribute = pNode->GetAttributes().Create("host");
    pHostAttribute->SetValue(strHost.c_str());
    CXMLAttribute* pPasswordAttribute = pNode->GetAttributes().Create("password");
    pPasswordAttribute->SetValue(strPassword.c_str());
}

std::string CServerBrowser::GetServerPassword(const std::string& strHost)
{
    // Password getting enabled?
    bool bSavedPasswords;
    CVARS_GET("save_server_passwords", bSavedPasswords);
    if (!bSavedPasswords)
        return "";

    CXMLNode* pConfig = CCore::GetSingletonPtr()->GetConfig();
    CXMLNode* pServerPasswords = pConfig->FindSubNode(CONFIG_NODE_SERVER_SAVED);
    if (!pServerPasswords)
    {
        pServerPasswords = pConfig->CreateSubNode(CONFIG_NODE_SERVER_SAVED);
    }
    // Check if the server password already exists
    for (unsigned int i = 0; i < pServerPasswords->GetSubNodeCount(); i++)
    {
        CXMLAttributes* pAttributes = &(pServerPasswords->GetSubNode(i)->GetAttributes());
        if (pAttributes->Find("host"))
        {
            if (CXMLAttribute* pHost = pAttributes->Find("host"))
            {
                const std::string& strXMLHost = pHost->GetValue();
                if (pHost->GetValue() == strHost)
                {
                    CXMLAttribute*     pPassword = pAttributes->Create("password");
                    const std::string& strPassword = pPassword->GetValue();
                    return strPassword;
                }
            }
        }
    }

    // If the server is the one from old quick connect, try importing the password from that
    std::string strQCEndpoint;
    CVARS_GET("qc_host", strQCEndpoint);

    std::string strTemp;
    CVARS_GET("qc_port", strTemp);

    strQCEndpoint = strQCEndpoint + ":" + strTemp;
    if (strQCEndpoint == strHost)
    {
        CVARS_GET("qc_password", strTemp);
        return strTemp;
    }

    return "";
}

void CServerBrowser::ClearServerPasswords()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr()->GetConfig();
    CXMLNode* pServerPasswords = pConfig->FindSubNode(CONFIG_NODE_SERVER_SAVED);
    if (pServerPasswords)
    {
        pServerPasswords->DeleteAllSubNodes();
        pConfig->DeleteSubNode(pServerPasswords);
    }
}

/////////////////////////////////////////////////////////////////
//
// CServerBrowser::FindSelectedServer
//
//
//
/////////////////////////////////////////////////////////////////
CServerListItem* CServerBrowser::FindSelectedServer(ServerBrowserType Type)
{
    if (m_pServerList[Type]->GetSelectedCount() >= 1)
    {
        return FindServerFromRow(Type, m_pServerList[Type]->GetSelectedItemRow());
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////
//
// CServerBrowser::FindServerFromRow
//
//
//
/////////////////////////////////////////////////////////////////
CServerListItem* CServerBrowser::FindServerFromRow(ServerBrowserType Type, int iRow)
{
    CServerList*     pList = GetServerList(Type);
    CServerListItem* pSelectedServer = (CServerListItem*)m_pServerList[Type]->GetItemData(iRow, DATA_PSERVER);
    if (CServerListItem::StaticIsValid(pSelectedServer))
        return pSelectedServer;
    return NULL;
}

/////////////////////////////////////////////////////////////////
//
// CServerBrowser::FindServer
//
// Finds a server at any point in the active list
//
/////////////////////////////////////////////////////////////////
CServerListItem* CServerBrowser::FindServer(const std::string& strHost, unsigned short usPort)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    CServerList*      pList = GetServerList(Type);

    CServerListIterator i, i_b = pList->IteratorBegin(), i_e = pList->IteratorEnd();

    for (CServerListIterator i = i_b; i != i_e; i++)
    {
        CServerListItem* pServer = *i;
        if (pServer->strHost == strHost && pServer->usGamePort == usPort)
            return pServer;
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////
//
// CServerBrowser::FindServerHttpPort
//
// Finds a server http port. Returns 0 if not found.
//
/////////////////////////////////////////////////////////////////
unsigned short CServerBrowser::FindServerHttpPort(const std::string& strHost, unsigned short usPort)
{
    CServerList*        pList = GetServerList(ServerBrowserTypes::INTERNET);
    CServerListIterator i, i_b = pList->IteratorBegin(), i_e = pList->IteratorEnd();
    for (CServerListIterator i = i_b; i != i_e; i++)
    {
        CServerListItem* pServer = *i;
        if (pServer->strHost == strHost && pServer->usGamePort == usPort)
            return pServer->m_usHttpPort;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////
//
// CServerBrowser::UpdateRowIndexMembers
//
// Update row index property of each CServerListItem on the server list
//
/////////////////////////////////////////////////////////////////
void CServerBrowser::UpdateRowIndexMembers(ServerBrowserType Type)
{
    CGUIGridList* pServerList = m_pServerList[Type];
    int           iRowCount = pServerList->GetRowCount();

    for (int iRowIndex = 0; iRowIndex < iRowCount; iRowIndex++)
    {
        CServerListItem* pServer = (CServerListItem*)pServerList->GetItemData(iRowIndex, DATA_PSERVER);
        pServer->iRowIndex = iRowIndex;
    }
}

/////////////////////////////////////////////////////////////////
//
// CServerBrowser::UpdateSelectedServerPlayerList
//
// Update GUI player list if select server gets query response
//
/////////////////////////////////////////////////////////////////
void CServerBrowser::UpdateSelectedServerPlayerList(ServerBrowserType Type)
{
    // If there is one item selected
    if (m_pServerList[Type]->GetSelectedCount() >= 1)
    {
        // Get the selected row
        int iSelectedIndex = m_pServerList[Type]->GetSelectedItemRow();

        // Get number of players as defined in the gridlist
        int iNumPlayers = atoi(m_pServerList[Type]->GetItemText(iSelectedIndex, m_hPlayers[Type]));

        // Get number of rows in the gui player list
        int iNumPlayerRows = m_pServerPlayerList[Type]->GetRowCount();

        // If number of rows in player list is less than number of players in server item,
        if (iNumPlayers > iNumPlayerRows || iNumPlayerRows == 1 && iNumPlayers == 1)
        {
            // find server item
            CServerListItem* pServer = FindSelectedServer(Type);

            if (pServer && pServer->vecPlayers.size() > 0)
            {
                bool bUpdatePlayerList = false;
                if (iNumPlayerRows == 1 && pServer->vecPlayers.size() == 1)
                {
                    SString strPlayerName = m_pServerPlayerList[Type]->GetItemText(0, m_hPlayerName[Type]);
                    if (strPlayerName == _("  ..loading.."))
                        bUpdatePlayerList = true;
                }
                if ((int)pServer->vecPlayers.size() > iNumPlayerRows || bUpdatePlayerList)
                {
                    m_pServerPlayerList[Type]->Clear();

                    // Add all the players
                    for (unsigned int j = 0; j < pServer->vecPlayers.size(); j++)
                    {
                        int k = m_pServerPlayerList[Type]->AddRow();
                        m_pServerPlayerList[Type]->SetItemText(k, m_hPlayerName[Type], pServer->vecPlayers[j].c_str());
                    }
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////
//
// CServerBrowser::GetVisibleEndPointList
//
// Get list servers that are visible in the GUI
//
/////////////////////////////////////////////////////////////////
void CServerBrowser::GetVisibleEndPointList(std::vector<SAddressPort>& outEndpointList)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();

    int iFirst, iLast;
    m_pServerList[Type]->GetVisibleRowRange(iFirst, iLast);
    for (int i = iFirst; i >= 0 && i <= iLast; i++)
    {
        if (CServerListItem* pServer = (CServerListItem*)m_pServerList[Type]->GetItemData(i, DATA_PSERVER))
        {
            if (CServerListItem::StaticIsValid(pServer))
                outEndpointList.push_back(SAddressPort(pServer->Address, pServer->usGamePort));
        }
    }
}

void CServerBrowser::SetStatusText(std::string strStatus)
{
    for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
    {
        m_pServerListStatus[i]->SetText(strStatus.c_str());
    }
}

void CServerBrowser::SetAddressBarText(std::string strText)
{
    for (unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
    {
        m_pEditAddress[i]->SetText(strText.c_str());
    }
}

bool CServerBrowser::IsAddressBarAwaitingInput()
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    return m_pEditAddress[Type]->IsActive();
}

void CServerBrowser::SetNextHistoryText(bool bDown)
{
    // Let's find our current position in the list
    ServerBrowserType Type = GetCurrentServerBrowserType();
    CGUIComboBox*     pServerList = m_pComboAddressHistory[Type];
    int               iRowCount = pServerList->GetItemCount();
    for (int i = 0; i < iRowCount; i++)
    {
        CGUIListItem* item = pServerList->GetItemByIndex(i);
        const char*   szAddress = (const char*)item->GetData();
        if (("mtasa://" + std::string(szAddress)) == m_pEditAddress[Type]->GetText())
        {
            // We've found our row
            i = bDown ? i + 1 : i - 1;

            // Ensure we're in range
            i = Clamp<int>(0, i, pServerList->GetItemCount() - 1);
            SetAddressBarText(std::string("mtasa://") + (const char*)pServerList->GetItemByIndex(i)->GetData());
            m_pEditAddress[Type]->SetCaretAtEnd();
            return;
        }
    }

    // Otherwise, let's start at the beginning if we're heading down
    if (bDown && pServerList->GetItemCount() > 0)
    {
        SetAddressBarText(std::string("mtasa://") + (const char*)pServerList->GetItemByIndex(0)->GetData());
    }
}

void CServerBrowser::OnQuickConnectButtonClick()
{
    // Show help text
    if (m_uiShownQuickConnectHelpCount < 1)
    {
        m_pQuickConnectHelpWindow->SetVisible(true);
        m_pQuickConnectHelpWindow->BringToFront();
        m_uiShownQuickConnectHelpCount++;
    }

    // Switch to LAN tab, but don't save it as selected
    if (!m_uiIsUsingTempTab)
        m_BeforeTempServerBrowserType = GetCurrentServerBrowserType();
    m_uiIsUsingTempTab = 2;
    m_pPanel->SetSelectedTab(m_pTab[ServerBrowserTypes::LAN]);

    // Show history
    m_pComboAddressHistory[ServerBrowserTypes::LAN]->ShowDropList();
}

bool CServerBrowser::OnServerListChangeRow(CGUIKeyEventArgs Args)
{
    ServerBrowserType Type = GetCurrentServerBrowserType();
    int               iSelectedItem = m_pServerList[Type]->GetSelectedItemRow();
    int               iMax = m_pServerList[Type]->GetRowCount();

    switch (Args.scancode)
    {
        case DIK_UPARROW:
        {
            if (iSelectedItem > 0)
            {
                m_pServerList[Type]->SetSelectedItem(iSelectedItem - 1, 1, true);
                OnClick(m_pServerPlayerList[Type]);            // hacky
            }
            break;
        }
        case DIK_DOWNARROW:
        {
            if (iSelectedItem < (iMax - 1))
            {
                m_pServerList[Type]->SetSelectedItem(iSelectedItem + 1, 1, true);
                OnClick(m_pServerPlayerList[Type]);            // hacky
            }
            break;
        }
        default:
            break;
    }

    return true;
}

bool CServerBrowser::OnServerListKeyDown(CGUIKeyEventArgs Args)
{
    switch (Args.scancode)
    {
        case DIK_UPARROW:
        case DIK_DOWNARROW:
        {
            OnServerListChangeRow(Args);
            break;
        }
        // Remove selected server from recent list when pressing on backspace
        case DIK_BACK:
        case DIK_DELETE:
        {
            RemoveSelectedServerFromRecentlyPlayedList();
            break;
        }
        default:
            break;
    }

    return true;
}

void CServerBrowser::SetSelectedIndex(unsigned int uiIndex)
{
    unsigned int uiTabCount = m_pPanel->GetTabCount();

    if (uiIndex < uiTabCount)
    {
        m_pPanel->SetSelectedIndex(uiIndex);
    }
}

void CServerBrowser::TabSkip(bool bBackwards)
{
    unsigned int uiTabCount = m_pPanel->GetTabCount();

    if (bBackwards)
    {
        unsigned int uiIndex = m_pPanel->GetSelectedIndex() - 1;

        if (m_pPanel->GetSelectedIndex() == 0)
        {
            uiIndex = uiTabCount - 1;
        }

        SetSelectedIndex(uiIndex);
    }
    else
    {
        unsigned int uiIndex = m_pPanel->GetSelectedIndex() + 1;
        unsigned int uiNewIndex = uiIndex % uiTabCount;

        SetSelectedIndex(uiNewIndex);
    }
}

bool CServerBrowser::IsActive()
{
    return (m_pFrame && m_pFrame->IsActive()) || (m_pPanel && m_pPanel->IsActive());
}
