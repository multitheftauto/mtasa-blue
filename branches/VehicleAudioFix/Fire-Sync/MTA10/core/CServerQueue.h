/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerQueue.h
*  PURPOSE:     Header file for server queue class
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerQueue;

#ifndef __CSERVERQUEUE_H
#define __CSERVERQUEUE_H

// Update interval for the full server (in milliseconds)
#define SERVER_UPDATE_INTERVAL		2500

// Dimensions for our window
#define QUEUE_WINDOW_DEFAULTWIDTH    370.0f
#define QUEUE_WINDOW_DEFAULTHEIGHT   345.0f

#define QUEUE_WINDOW_HSPACING 20
#define QUEUE_LABEL_VSPACING 0
#define QUEUE_WINDOW_VSPACING 10
#define QUEUE_BUTTON_HEIGHT 25
#define QUEUE_BUTTON_WIDTH 110

#include "CServerList.h"
#include "CSingleton.h"

class CServerQueue : public CSingleton < CServerQueue >
{
    friend class CCore;

public:
    explicit            CServerQueue                ( void );
                        ~CServerQueue               ( void );

    void                SetVisible                  ( bool bVisible );
    bool			    IsVisible                   ( void );

    void                SetServerInformation        ( const char* szHost, unsigned short usPort, const char* szPassword );

    void                DoPulse                     ( void );

protected:
    CGUIWindow*         m_pWindow;
    CGUILabel*          m_pServerNameLabel;
    CGUILabel*          m_pServerIPLabel;
    CGUILabel*          m_pGamemodeLabel;
    CGUILabel*          m_pMapLabel;
    CGUILabel*          m_pPlayersLabel;
    CGUILabel*          m_pPasswordedLabel;
    CGUILabel*          m_pLatencyLabel;

    CGUILabel*          m_pServerNameLabelTitle;
    CGUILabel*          m_pServerIPLabelTitle;
    CGUILabel*          m_pGamemodeLabelTitle;
    CGUILabel*          m_pMapLabelTitle;
    CGUILabel*          m_pPlayersLabelTitle;
    CGUILabel*          m_pPasswordedLabelTitle;
    CGUILabel*          m_pLatencyLabelTitle;

    CGUIGridList*       m_pServerPlayerList;
    CGUIHandle          m_hPlayerName;

    CGUICheckBox*       m_pCheckboxAutojoin;

    CGUIButton*         m_pButtonJoinGame;
    CGUIButton*         m_pButtonClose;

private:
    unsigned long       m_ulLastUpdateTime;
    CServerListItem     m_Server;

    bool                m_bRequiresUpdate;

    const char*         m_szHost;
    unsigned short      m_usPort;
    const char*         m_szPassword;

    void                Refresh                     ( void );
    void                Connect                     ( void );
    void                Reset                       ( void );

    bool                OnCloseClicked              ( CGUIElement* pElement );
    bool                OnJoinGameClicked           ( CGUIElement* pElement );
};

#endif
