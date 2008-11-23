/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CLocalServer.h
*  PURPOSE:     Header for local server class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLOCALSERVER_H
#define __CLOCALSERVER_H

#include <CClientGUIManager.h>
#include "../../../core/CXMLConfig.h"
#include <string>

class CLocalServer
{
public:
                                CLocalServer        ( const char* szConfig );
                                ~CLocalServer       ( void );

	bool			            OnStartButtonClick	( CGUIElement* pElement );
	bool			            OnCancelButtonClick	( CGUIElement* pElement );
    bool                        OnAddButtonClick    ( CGUIElement* pElement );
    bool                        OnDelButtonClick    ( CGUIElement* pElement );

    // Server config
    bool                        Load                ( void );
    bool                        Save                ( void );

    void                        StoreConfigValue    ( const char* szNode, const char* szValue );

    void                        HandleResource      ( char* szResource );

private:

    string                      m_strConfig;
    CXMLFile*                   m_pConfig;

    char                        m_szServerPath [MAX_PATH];
    char                        m_szConfigPath [MAX_PATH];
    char                        m_szResourceDirectoryPath [MAX_PATH];
    char                        m_szResourceCachePath [MAX_PATH];

    CGUI*						m_pGUI;

    CGUIWindow*                 m_pWindow;

    CGUITabPanel*               m_pTabs;

    CGUITab*                    m_pTabGeneral;
    CGUITab*                    m_pTabResources;

    CGUILabel*                  m_pLabelName;
    CGUILabel*                  m_pLabelPass;
    CGUILabel*                  m_pLabelPlayers;
    CGUILabel*                  m_pLabelBroadcast;

    CGUIEdit*                   m_pEditName;
    CGUIEdit*                   m_pEditPass;
    CGUIEdit*                   m_pEditPlayers;

    CGUICheckBox*               m_pBroadcastLan;
    CGUICheckBox*               m_pBroadcastInternet;

    CGUIGridList*               m_pResourcesAll;
    CGUIGridList*               m_pResourcesCur;

    CGUIHandle                  m_hResourcesAll;
    CGUIHandle                  m_hResourcesCur;

    CGUIButton*                 m_pResourceAdd;
    CGUIButton*                 m_pResourceDel;

    CGUIButton*                 m_pButtonStart;
    CGUIButton*                 m_pButtonCancel;

};

#endif
