/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

class CWebSettingsTab
{
public:
    CWebSettingsTab(CGUITab* pTab);
    void SaveData();
    void LoadData();

private:
    bool                OnBrowserBlacklistAdd(CGUIElement* pElement);
    bool                OnBrowserBlacklistRemove(CGUIElement* pElement);
    bool                OnBrowserWhitelistAdd(CGUIElement* pElement);
    bool                OnBrowserWhitelistRemove(CGUIElement* pElement);

    bool                m_bBrowserListsChanged;
    std::unique_ptr<CGUITab> m_pTab;

    std::unique_ptr<CGUILabel>          m_pLabelBrowserGeneral;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxRemoteBrowser;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxRemoteJavascript;
    std::unique_ptr<CGUILabel>          m_pLabelBrowserCustomBlacklist;
    std::unique_ptr<CGUIEdit>           m_pEditBrowserBlacklistAdd;
    std::unique_ptr<CGUIButton>         m_pButtonBrowserBlacklistAdd;
    std::unique_ptr<CGUIGridList>       m_pGridBrowserBlacklist;
    std::unique_ptr<CGUIButton>         m_pButtonBrowserBlacklistRemove;
    std::unique_ptr<CGUILabel>          m_pLabelBrowserCustomWhitelist;
    std::unique_ptr<CGUIEdit>           m_pEditBrowserWhitelistAdd;
    std::unique_ptr<CGUIButton>         m_pButtonBrowserWhitelistAdd;
    std::unique_ptr<CGUIGridList>       m_pGridBrowserWhitelist;
    std::unique_ptr<CGUIButton>         m_pButtonBrowserWhitelistRemove;
public:
    bool ChangeRequiresDisconnect();
};
