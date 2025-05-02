/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CLocalServer.h
 *  PURPOSE:     Header for local server class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CClientGUIManager.h>
#include <string>

class CLocalServer
{
public:
    CLocalServer(const char* szConfig);
    ~CLocalServer();

protected:
    bool OnStartButtonClick(CGUIElement* pElement);
    bool OnCancelButtonClick(CGUIElement* pElement);
    bool OnAddButtonClick(CGUIElement* pElement);
    bool OnDelButtonClick(CGUIElement* pElement);

    // Server config
    bool Load();
    bool Save();

    void StoreConfigValue(const char* szNode, const char* szValue);

    void HandleResource(const char* szResource);
    void GetResourceNameList(std::vector<SString>& outResourceNameList, const SString& strResourceDirectoryPath);

    std::string GetConfigName() { return m_strConfig; };

private:
    std::string m_strConfig;
    CXMLFile*   m_pConfig;

    CGUI* m_pGUI;

    CGUIWindow* m_pWindow;

    CGUITabPanel* m_pTabs;

    CGUITab* m_pTabGeneral;
    CGUITab* m_pTabResources;

    CGUILabel* m_pLabelName;
    CGUILabel* m_pLabelPass;
    CGUILabel* m_pLabelPlayers;
    CGUILabel* m_pLabelBroadcast;

    CGUIEdit* m_pEditName;
    CGUIEdit* m_pEditPass;
    CGUIEdit* m_pEditPlayers;

    CGUICheckBox* m_pBroadcastLan;
    CGUICheckBox* m_pBroadcastInternet;

    CGUIGridList* m_pResourcesAll;
    CGUIGridList* m_pResourcesCur;

    CGUIHandle m_hResourcesAll;
    CGUIHandle m_hResourcesCur;

    CGUIButton* m_pResourceAdd;
    CGUIButton* m_pResourceDel;

    CGUIButton* m_pButtonStart;
    CGUIButton* m_pButtonCancel;
};
