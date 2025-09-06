/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerInfo.h
 *  PURPOSE:     Header file for server queue class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CServerInfo;

#pragma once

// Update interval for the full server (in milliseconds)
#define SERVER_UPDATE_INTERVAL 2500

// Dimensions for our window
#define INFO_WINDOW_DEFAULTWIDTH 370.0f
#define INFO_WINDOW_DEFAULTHEIGHT 400.0f

#define INFO_WINDOW_HSPACING 20
#define INFO_LABEL_VSPACING 0
#define INFO_WINDOW_VSPACING 10
#define INFO_BUTTON_HEIGHT 25
#define INFO_BUTTON_WIDTH 110

#include "CServerList.h"
#include "CSingleton.h"

namespace eWindowTypes
{
    enum eWindowType
    {
        SERVER_INFO_RAW = 0,
        SERVER_INFO_QUEUE,
        SERVER_INFO_PASSWORD,
    };
}

using eWindowTypes::eWindowType;

class CServerInfo : public CSingleton<CServerInfo>
{
    friend class CCore;

public:
    explicit CServerInfo();
    ~CServerInfo();

    bool IsVisible();
    void Hide();
    void Show(eWindowType WindowType);
    void Show(eWindowType WindowType, const char* szHost, unsigned short usPort, const char* szPassword, CServerListItem* pInitialServerListItem = NULL);

    void SetServerInformation(const char* szHost, unsigned short usPort, const char* szPassword, CServerListItem* pInitialServerListItem = NULL);

    void DoPulse();

protected:
    CGUIWindow* m_pWindow;
    CGUILabel*  m_pServerNameLabel;
    CGUILabel*  m_pServerAddressLabel;
    CGUILabel*  m_pGamemodeLabel;
    CGUILabel*  m_pMapLabel;
    CGUILabel*  m_pPlayersLabel;
    CGUILabel*  m_pPasswordedLabel;
    CGUILabel*  m_pLatencyLabel;

    CGUILabel* m_pServerNameLabelTitle;
    CGUILabel* m_pServerAddressLabelTitle;
    CGUILabel* m_pGamemodeLabelTitle;
    CGUILabel* m_pMapLabelTitle;
    CGUILabel* m_pPlayersLabelTitle;
    CGUILabel* m_pPasswordedLabelTitle;
    CGUILabel* m_pLatencyLabelTitle;

    CGUIGridList* m_pServerPlayerList;
    CGUIHandle    m_hPlayerName;

    CGUICheckBox* m_pCheckboxAutojoin;

    CGUILabel* m_pEnterPasswordLabel;
    CGUIEdit*  m_pEnterPasswordEdit;

    CGUIButton* m_pButtonJoinGame;
    CGUIButton* m_pButtonClose;

private:
    unsigned long   m_ulLastUpdateTime;
    CServerListItem m_Server;
    eWindowType     m_pCurrentWindowType;
    bool            m_bWaitingToActivatePassword;

    SString        m_strHost;
    unsigned short m_usPort;
    SString        m_strPassword;

    void Refresh();
    void Connect();
    void Reset();
    void ResetServerGUI(CServerListItem* pServer);

    bool OnCloseClicked(CGUIElement* pElement);
    bool OnJoinGameClicked(CGUIElement* pElement);
};
