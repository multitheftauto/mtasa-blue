/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerBrowser.h
 *  PURPOSE:     Header file for server browser class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CServerBrowser;

#pragma once

// Amount of server lists/tabs (ServerBrowserType)
#define SERVER_BROWSER_TYPE_COUNT 4

// Amount of search types
#define SERVER_BROWSER_SEARCH_TYPE_COUNT 2

// Server browser list update interval (in ms)
#define SERVER_BROWSER_UPDATE_INTERVAL 1000

#include "CMainMenu.h"
#include "CServerList.h"
#include "CSingleton.h"
#include "CServerInfo.h"
#include <ctime>
#include <cstddef>
#include <optional>
#include <string>

namespace ServerBrowserTypes
{
    enum ServerBrowserType
    {
        INTERNET = 0,
        LAN,
        FAVOURITES,
        RECENTLY_PLAYED
    };
}

using ServerBrowserTypes::ServerBrowserType;

namespace SearchTypes
{
    enum SearchType
    {
        SERVERS = 0,
        PLAYERS,
        MAX_SEARCH_TYPES,
    };
}

using SearchTypes::SearchType;

class CServerBrowser : public CSingleton<CServerBrowser>
{
    friend class CCore;

public:
    explicit CServerBrowser();
    ~CServerBrowser();

    void Update();

    void SetVisible(bool bVisible);
    bool IsVisible() const;

    bool ConnectToSelectedServer();

    static void CompleteConnect();

    CServerList* GetInternetList() { return &m_ServersInternet; };
    CServerList* GetFavouritesList() { return &m_ServersFavourites; };
    CServerList* GetRecentList() { return &m_ServersRecent; };
    CServerList* GetHistoryList() { return &m_ServersHistory; };

    void SaveRecentlyPlayedList();
    void SaveFavouritesList();

    void LoadOptions(CXMLNode* pNode);
    void SaveOptions(bool bForceSave = false);

    bool LoadServerList(CXMLNode* pNode, const std::string& strTagName, CServerList* pList);
    bool SaveServerList(CXMLNode* pNode, const std::string& strTagName, CServerList* pList, unsigned int iLimit = 0);

    void        SetServerPassword(const std::string& strHost, const std::string& strPassword);
    std::string GetServerPassword(const std::string& strHost);
    void        ClearServerPasswords();

    void SetStatusText(std::string_view strStatus);
    void SetAddressBarText(std::string_view strText);

    CServerListItem* FindSelectedServer(ServerBrowserType Type);
    CServerListItem* FindServerFromRow(ServerBrowserType Type, int iRow);
    CServerListItem* FindServer(const std::string& strHost, unsigned short usPort);
    unsigned short   FindServerHttpPort(const std::string& strHost, unsigned short usPort);
    void             UpdateRowIndexMembers(ServerBrowserType Type);
    void             UpdateSelectedServerPlayerList(ServerBrowserType Type);
    void             GetVisibleEndPointList(std::vector<SAddressPort>& outEndpointList);

    bool IsAddressBarAwaitingInput();
    void SetNextHistoryText(bool bDown);

    void OnQuickConnectButtonClick();
    void NotifyServerExists(in_addr Address, ushort usPort);

    void TabSkip(bool bBackwards);

    bool IsActive();

    void SetSelectedIndex(unsigned int uiIndex);

protected:
    bool OnMouseClick(CGUIMouseEventArgs Args);
    bool OnMouseDoubleClick(CGUIMouseEventArgs Args);

    void CreateHistoryList();
    bool CanBrowseVersion(const SString& strVersion);

    int m_iSelectedServer[SERVER_BROWSER_TYPE_COUNT];

    const char*      m_szSearchTypePath[SearchTypes::MAX_SEARCH_TYPES];
    CGUIStaticImage* m_pSearchIcons[SearchTypes::MAX_SEARCH_TYPES];

    CVector2D m_WidgetSize;

    // Window widgets
    CGUIWindow*      m_pFrame;
    CGUIElement*     m_pTopWindow;
    CGUITabPanel*    m_pPanel;
    CGUIStaticImage* m_pLockedIcon;

    // Tab controls
    CGUITab* m_pTab[SERVER_BROWSER_TYPE_COUNT];

    CGUIGridList* m_pServerList[SERVER_BROWSER_TYPE_COUNT];
    int           m_pServerListRevision[SERVER_BROWSER_TYPE_COUNT];

    CGUILabel*    m_pServerPlayerListLabel[SERVER_BROWSER_TYPE_COUNT];
    CGUIGridList* m_pServerPlayerList[SERVER_BROWSER_TYPE_COUNT];

    CGUIEdit*        m_pEditServerSearch[SERVER_BROWSER_TYPE_COUNT];
    CGUIStaticImage* m_pServerSearchIcon[SERVER_BROWSER_TYPE_COUNT];

    CGUILabel*    m_pLabelInclude[SERVER_BROWSER_TYPE_COUNT];
    CGUICheckBox* m_pIncludeEmpty[SERVER_BROWSER_TYPE_COUNT];
    CGUICheckBox* m_pIncludeFull[SERVER_BROWSER_TYPE_COUNT];
    CGUICheckBox* m_pIncludeLocked[SERVER_BROWSER_TYPE_COUNT];
    CGUICheckBox* m_pIncludeOffline[SERVER_BROWSER_TYPE_COUNT];
    CGUICheckBox* m_pIncludeOtherVersions[SERVER_BROWSER_TYPE_COUNT];

    CGUIButton*      m_pButtonConnect[SERVER_BROWSER_TYPE_COUNT];
    CGUIStaticImage* m_pButtonConnectIcon[SERVER_BROWSER_TYPE_COUNT];
    CGUIButton*      m_pButtonRefresh[SERVER_BROWSER_TYPE_COUNT];
    CGUIStaticImage* m_pButtonRefreshIcon[SERVER_BROWSER_TYPE_COUNT];
    CGUIButton*      m_pButtonInfo[SERVER_BROWSER_TYPE_COUNT];
    CGUIStaticImage* m_pButtonInfoIcon[SERVER_BROWSER_TYPE_COUNT];
    CGUIButton*      m_pButtonFavourites[SERVER_BROWSER_TYPE_COUNT];

    CGUIEdit*        m_pEditAddress[SERVER_BROWSER_TYPE_COUNT];
    CGUILabel*       m_pLabelAddressDescription[SERVER_BROWSER_TYPE_COUNT];
    CGUIComboBox*    m_pComboAddressHistory[SERVER_BROWSER_TYPE_COUNT];
    CGUIStaticImage* m_pSearchTypeIcon[SERVER_BROWSER_TYPE_COUNT];
    CGUIStaticImage* m_pAddressFavoriteIcon[SERVER_BROWSER_TYPE_COUNT];
    CGUIStaticImage* m_pRemoveFromRecentIcon[SERVER_BROWSER_TYPE_COUNT];

    CGUIComboBox* m_pComboSearchType[SERVER_BROWSER_TYPE_COUNT];
    CGUIEdit*     m_pEditSearch[SERVER_BROWSER_TYPE_COUNT];
    CGUILabel*    m_pLabelSearchDescription[SERVER_BROWSER_TYPE_COUNT];

    CGUILabel* m_pLabelPassword[SERVER_BROWSER_TYPE_COUNT];
    CGUIEdit*  m_pEditPassword[SERVER_BROWSER_TYPE_COUNT];

    CGUILabel*  m_pServerListStatus[SERVER_BROWSER_TYPE_COUNT];
    CGUIButton* m_pButtonBack[SERVER_BROWSER_TYPE_COUNT];
    CGUIButton* m_pButtonGeneralHelp[SERVER_BROWSER_TYPE_COUNT];

    // Server list columns
    CGUIHandle m_hVersion[SERVER_BROWSER_TYPE_COUNT];
    CGUIHandle m_hLocked[SERVER_BROWSER_TYPE_COUNT];
    CGUIHandle m_hName[SERVER_BROWSER_TYPE_COUNT];
    CGUIHandle m_hPing[SERVER_BROWSER_TYPE_COUNT];
    CGUIHandle m_hPlayers[SERVER_BROWSER_TYPE_COUNT];
    CGUIHandle m_hHost[SERVER_BROWSER_TYPE_COUNT];
    CGUIHandle m_hGame[SERVER_BROWSER_TYPE_COUNT];
    CGUIHandle m_hMap[SERVER_BROWSER_TYPE_COUNT];

    // Player list columns
    CGUIHandle m_hPlayerName[SERVER_BROWSER_TYPE_COUNT];

    // Flash searchbox
    struct
    {
        uint uiCount;
        uint uiNextTime;
    } m_FlashSearchBox[SERVER_BROWSER_TYPE_COUNT];

private:
    struct SFilterState;
    struct SListRefreshState;
    struct SSortState;

    enum
    {
        DATA_PSERVER = 1,  // Column which stores server pointer
    };

    void CreateTab(ServerBrowserType type, const char* szName);
    void DeleteTab(ServerBrowserType type);

    void                       UpdateServerList(ServerBrowserType Type, bool bClearServerList = false);
    void                       UpdateHistoryList();
    [[nodiscard]] CServerList* GetServerList(ServerBrowserType Type);
    void                       AddServerToList(CServerListItem* pServer, ServerBrowserType Type, const SFilterState& filterState);
    bool                       RemoveSelectedServerFromRecentlyPlayedList();
    void                       UpdateFilteredPlayerCountStatus(ServerBrowserType Type, CServerList* pDataList = nullptr);

    bool OnClick(CGUIElement* pElement);
    bool OnDoubleClick(CGUIElement* pElement);
    bool OnConnectClick(CGUIElement* pElement);
    bool OnRefreshClick(CGUIElement* pElement);
    bool OnInfoClick(CGUIElement* pElement);
    bool OnFavouritesClick(CGUIElement* pElement);
    bool OnRemoveFromRecentClick(CGUIElement* pElement);
    bool OnBackClick(CGUIElement* pElement);
    bool OnGeneralHelpClick(CGUIElement* pElement);
    bool OnGeneralHelpDeactivate(CGUIElement* pElement);
    bool OnAddressChanged(CGUIElement* pElement);
    bool OnFilterChanged(CGUIElement* pElement);
    bool OnTabChanged(CGUIElement* pElement);
    bool OnHistorySelected(CGUIElement* pElement);
    bool OnHistoryDropListRemove(CGUIElement* pElement);

    bool OnSearchTypeSelected(CGUIElement* pElement);

    bool OnSearchFocused(CGUIElement* pElement);
    bool OnSearchDefocused(CGUIElement* pElement);
    bool OnAddressFocused(CGUIElement* pElement);
    bool OnAddressDefocused(CGUIElement* pElement);

    bool OnServerListChangeRow(CGUIKeyEventArgs Args);
    bool OnServerListKeyDown(CGUIKeyEventArgs Args);
    bool OnServerListSortChanged(CGUIElement* pElement);

    ServerBrowserType GetCurrentServerBrowserType();
    ServerBrowserType GetCurrentServerBrowserTypeForSave();

    void EnsureRefreshFor(ServerBrowserType type, bool bAutoRefreshEnabled, bool bForceOnFirstView);
    void StartRefresh(ServerBrowserType type);

    void                      RequestConfigSave(bool bForceImmediate);
    void                      ProcessPendingConfigSave();
    void                      RequestFilterRefresh(ServerBrowserType type, bool bImmediate);
    void                      BeginServerListRefresh(ServerBrowserType type, bool bClearServerList, bool bForceRestart);
    bool                      ProcessServerListRefreshBatch(ServerBrowserType type, std::size_t uiMaxSteps);
    void                      ProcessPendingListRefreshes();
    void                      SuspendServerLists();
    void                      UpdateSortState(ServerBrowserType type);
    void                      ApplyListSort(ServerBrowserType type, unsigned int uiColumn, SortDirection direction, bool bUpdateStoredState = false);
    [[nodiscard]] bool        IsListRefreshInProgress(ServerBrowserType type) const;
    [[nodiscard]] std::size_t GetListRefreshBatchSize() const;
    [[nodiscard]] std::size_t GetHiddenListRefreshBatchSize() const;
    [[nodiscard]] bool        ShouldUseHiddenRefreshAcceleration() const;

    struct SFilterState
    {
        std::string searchText;
        int         searchType;
        bool        includeEmpty;
        bool        includeFull;
        bool        includeLocked;
        bool        includeOffline;
        bool        includeOtherVersions;
    };

    [[nodiscard]] SFilterState CaptureFilterState(ServerBrowserType type);
    [[nodiscard]] bool         FilterStateChanged(ServerBrowserType type, const SFilterState& newState) const;

    struct SListRefreshState
    {
        bool                        bActive = false;
        bool                        bClearServerList = false;
        bool                        bNeedsListClear = false;
        bool                        bDidUpdateRowIndices = false;
        uint                        uiSortColumn = 0;
        SortDirection               sortDirection = SortDirections::None;
        CServerList*                pList = nullptr;
        CServerListIterator         iterator;
        CServerListIterator         endIterator;
        std::optional<SFilterState> filterSnapshot;
    };

    struct SSortState
    {
        unsigned int  uiColumn = 0;
        SortDirection direction = SortDirections::None;
    };

    CServerListInternet m_ServersInternet;
    CServerListLAN      m_ServersLAN;
    CServerList         m_ServersFavourites;
    CServerList         m_ServersRecent;
    CServerList         m_ServersHistory;

    unsigned long     m_ulLastUpdateTime;
    bool              m_bFirstTimeBrowseServer;
    bool              m_bOptionsLoaded;
    ServerBrowserType m_PrevServerBrowserType;

    std::map<SString, int> m_blockedVersionMap;
    std::map<SString, int> m_allowedVersionMap;

    CGUIWindow* m_pQuickConnectHelpWindow;
    bool        m_bFocusTextEdit;

    uint              m_uiShownQuickConnectHelpCount;
    uint              m_uiIsUsingTempTab;
    ServerBrowserType m_BeforeTempServerBrowserType;
    CGUIWindow*       m_pGeneralHelpWindow;
    long long         m_llLastGeneralHelpTime;

    bool          m_bInitialRefreshDone[SERVER_BROWSER_TYPE_COUNT];
    bool          m_bPendingRefresh[SERVER_BROWSER_TYPE_COUNT];
    bool          m_bHistoryListDirty;
    bool          m_bPendingFilterUpdate[SERVER_BROWSER_TYPE_COUNT];
    unsigned long m_ulNextFilterUpdateTime[SERVER_BROWSER_TYPE_COUNT];
    SFilterState  m_FilterState[SERVER_BROWSER_TYPE_COUNT];
    SSortState    m_SortState[SERVER_BROWSER_TYPE_COUNT];

    bool              m_bPendingConfigSave;
    unsigned long     m_ulNextConfigSaveTime;
    SListRefreshState m_ListRefreshState[SERVER_BROWSER_TYPE_COUNT];
    unsigned long     m_ulNextListLayoutTime[SERVER_BROWSER_TYPE_COUNT] = {};
    unsigned long     m_ulHiddenRefreshBoostEnableTime = 0;
    int               m_iIgnoreSortCallbacks = 0;

    // Player list retry tracking - retry queries for servers that haven't responded with player data
    CServerListItem*          m_pLastSelectedServerForPlayerList[SERVER_BROWSER_TYPE_COUNT] = {};
    std::chrono::milliseconds m_msLastPlayerListQueryRetryTime[SERVER_BROWSER_TYPE_COUNT] = {};
};
