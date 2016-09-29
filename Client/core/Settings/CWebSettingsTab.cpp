#include <StdInc.h>
#include "CWebSettingsTab.h"

CWebSettingsTab::CWebSettingsTab(CGUITab* pTab) :
    m_pTab(pTab)
{
    m_bBrowserListsChanged = false;

    CGUI *pManager = g_pCore->GetGUI();
    /**
    * Webbrowser tab
    **/
    CVector2D vecTemp;
    m_pLabelBrowserGeneral = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("General")));
    m_pLabelBrowserGeneral->SetPosition(CVector2D(10.0f, 12.0f));
    m_pLabelBrowserGeneral->GetPosition(vecTemp);
    m_pLabelBrowserGeneral->AutoSize(NULL, 5.0f);
    m_pLabelBrowserGeneral->SetFont("default-bold-small");

    m_pCheckBoxRemoteBrowser = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Enable remote websites"), true));
    m_pCheckBoxRemoteBrowser->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pCheckBoxRemoteBrowser->GetPosition(vecTemp);
    m_pCheckBoxRemoteBrowser->AutoSize(NULL, 20.0f);

    m_pCheckBoxRemoteJavascript = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Enable Javascript on remote websites"), true));
    m_pCheckBoxRemoteJavascript->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 25.0f));
    m_pCheckBoxRemoteJavascript->GetPosition(vecTemp);
    m_pCheckBoxRemoteJavascript->AutoSize(NULL, 20.0f);

    m_pLabelBrowserCustomBlacklist = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Custom blacklist")));
    m_pLabelBrowserCustomBlacklist->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pLabelBrowserCustomBlacklist->GetPosition(vecTemp);
    m_pLabelBrowserCustomBlacklist->AutoSize(NULL, 5.0f);
    m_pLabelBrowserCustomBlacklist->SetFont("default-bold-small");

    m_pEditBrowserBlacklistAdd = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pTab.get()));
    m_pEditBrowserBlacklistAdd->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 25.0f));
    m_pEditBrowserBlacklistAdd->GetPosition(vecTemp);
    m_pEditBrowserBlacklistAdd->SetSize(CVector2D(191.0f, 22.0f));
    m_pEditBrowserBlacklistAdd->SetText(_("Enter a domain e.g. google.com"));

    m_pButtonBrowserBlacklistAdd = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Block")));
    m_pButtonBrowserBlacklistAdd->SetPosition(CVector2D(vecTemp.fX + m_pEditBrowserBlacklistAdd->GetSize().fX + 2.0f, vecTemp.fY));
    m_pButtonBrowserBlacklistAdd->SetSize(CVector2D(64.0f, 22.0f));

    m_pGridBrowserBlacklist = std::unique_ptr<CGUIGridList>(pManager->CreateGridList(m_pTab.get()));
    m_pGridBrowserBlacklist->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pGridBrowserBlacklist->GetPosition(vecTemp);
    m_pGridBrowserBlacklist->SetSize(CVector2D(256.0f, 150.0f));
    m_pGridBrowserBlacklist->AddColumn(_("Domain"), 0.9f);

    m_pButtonBrowserBlacklistRemove = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Remove domain")));
    m_pButtonBrowserBlacklistRemove->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + m_pGridBrowserBlacklist->GetSize().fY + 5.0f));
    m_pButtonBrowserBlacklistRemove->SetSize(CVector2D(140.0f, 22.0f));

    m_pLabelBrowserCustomBlacklist->GetPosition(vecTemp); // Reset vecTemp

    m_pLabelBrowserCustomWhitelist = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Custom whitelist")));
    m_pLabelBrowserCustomWhitelist->SetPosition(CVector2D(292.0f, vecTemp.fY));
    m_pLabelBrowserCustomWhitelist->GetPosition(vecTemp);
    m_pLabelBrowserCustomWhitelist->AutoSize(NULL, 5.0f);
    m_pLabelBrowserCustomWhitelist->SetFont("default-bold-small");

    m_pEditBrowserWhitelistAdd = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pTab.get()));
    m_pEditBrowserWhitelistAdd->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 25.0f));
    m_pEditBrowserWhitelistAdd->GetPosition(vecTemp);
    m_pEditBrowserWhitelistAdd->SetSize(CVector2D(191.0f, 22.0f));
    m_pEditBrowserWhitelistAdd->SetText(_("Enter a domain e.g. google.com"));

    m_pButtonBrowserWhitelistAdd = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Allow")));
    m_pButtonBrowserWhitelistAdd->SetPosition(CVector2D(vecTemp.fX + m_pEditBrowserWhitelistAdd->GetSize().fX + 2.0f, vecTemp.fY));
    m_pButtonBrowserWhitelistAdd->SetSize(CVector2D(64.0f, 22.0f));

    m_pGridBrowserWhitelist = std::unique_ptr<CGUIGridList>(pManager->CreateGridList(m_pTab.get()));
    m_pGridBrowserWhitelist->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pGridBrowserWhitelist->GetPosition(vecTemp);
    m_pGridBrowserWhitelist->SetSize(CVector2D(256.0f, 150.0f));
    m_pGridBrowserWhitelist->AddColumn(_("Domain"), 0.9f);

    m_pButtonBrowserWhitelistRemove = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Remove domain")));
    m_pButtonBrowserWhitelistRemove->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + m_pGridBrowserWhitelist->GetSize().fY + 5.0f));
    m_pButtonBrowserWhitelistRemove->SetSize(CVector2D(140.0f, 22.0f));

    m_pButtonBrowserBlacklistAdd->SetClickHandler(GUI_CALLBACK(&CWebSettingsTab::OnBrowserBlacklistAdd, this));
    m_pButtonBrowserBlacklistRemove->SetClickHandler(GUI_CALLBACK(&CWebSettingsTab::OnBrowserBlacklistRemove, this));
    m_pButtonBrowserWhitelistAdd->SetClickHandler(GUI_CALLBACK(&CWebSettingsTab::OnBrowserWhitelistAdd, this));
    m_pButtonBrowserWhitelistRemove->SetClickHandler(GUI_CALLBACK(&CWebSettingsTab::OnBrowserWhitelistRemove, this));
}

bool CWebSettingsTab::OnBrowserBlacklistAdd(CGUIElement* pElement)
{
    SString strDomain = m_pEditBrowserBlacklistAdd->GetText();
    if (!strDomain.empty())
    {
        bool bExists = false;
        for (int i = 0; i < m_pGridBrowserBlacklist->GetRowCount(); ++i)
        {
            if (m_pGridBrowserBlacklist->GetItemText(i, 1) == strDomain)
            {
                bExists = true;
                break;
            }
        }

        if (!bExists)
        {
            m_pGridBrowserBlacklist->SetItemText(m_pGridBrowserBlacklist->AddRow(), 1, strDomain);
            m_bBrowserListsChanged = true;
        }
    }
    return true;
}

bool CWebSettingsTab::OnBrowserBlacklistRemove(CGUIElement* pElement)
{
    int iSelectedRow = m_pGridBrowserBlacklist->GetSelectedItemRow();
    if (iSelectedRow > -1)
    {
        m_pGridBrowserBlacklist->RemoveRow(iSelectedRow);
        m_bBrowserListsChanged = true;
    }

    return true;
}

bool CWebSettingsTab::OnBrowserWhitelistAdd(CGUIElement* pElement)
{
    SString strDomain = m_pEditBrowserWhitelistAdd->GetText();
    if (!strDomain.empty())
    {
        bool bExists = false;
        for (int i = 0; i < m_pGridBrowserWhitelist->GetRowCount(); ++i)
        {
            if (m_pGridBrowserWhitelist->GetItemText(i, 1) == strDomain)
            {
                bExists = true;
                break;
            }
        }

        if (!bExists)
        {
            m_pGridBrowserWhitelist->SetItemText(m_pGridBrowserWhitelist->AddRow(), 1, strDomain);
            m_bBrowserListsChanged = true;
        }
    }
    return true;
}

bool CWebSettingsTab::OnBrowserWhitelistRemove(CGUIElement* pElement)
{
    int iSelectedRow = m_pGridBrowserWhitelist->GetSelectedItemRow();
    if (iSelectedRow > -1)
    {
        m_pGridBrowserWhitelist->RemoveRow(iSelectedRow);
        m_bBrowserListsChanged = true;
    }

    return true;
}

bool CWebSettingsTab::ChangeRequiresDisconnect()
{
    return m_bBrowserListsChanged;
}

void CWebSettingsTab::SaveData(void)
{
    // Webbrowser settings
    bool bOldRemoteWebsites, bOldRemoteJavascript;
    CVARS_GET("browser_remote_websites", bOldRemoteWebsites);
    CVARS_GET("browser_remote_javascript", bOldRemoteJavascript);

    bool bBrowserSettingChanged = false;
    if (bOldRemoteWebsites != m_pCheckBoxRemoteBrowser->GetSelected()
        || bOldRemoteJavascript != m_pCheckBoxRemoteJavascript->GetSelected())
    {
        bBrowserSettingChanged = true;
        CVARS_SET("browser_remote_websites", m_pCheckBoxRemoteBrowser->GetSelected());
        CVARS_SET("browser_remote_javascript", m_pCheckBoxRemoteJavascript->GetSelected());
    }

    auto pWebCore = CCore::GetSingleton().GetWebCore();
    if (pWebCore)
    {
        std::vector<SString> customBlacklist;
        for (int i = 0; i < m_pGridBrowserBlacklist->GetRowCount(); ++i)
        {
            customBlacklist.push_back(m_pGridBrowserBlacklist->GetItemText(i, 1));
        }
        pWebCore->WriteCustomList("customblacklist", customBlacklist);

        std::vector<SString> customWhitelist;
        for (int i = 0; i < m_pGridBrowserWhitelist->GetRowCount(); ++i)
        {
            customWhitelist.push_back(m_pGridBrowserWhitelist->GetItemText(i, 1));
        }
        pWebCore->WriteCustomList("customwhitelist", customWhitelist);

        if (m_bBrowserListsChanged)
            bBrowserSettingChanged = true;
    }

}

void CWebSettingsTab::LoadData()
{
    // Browser
    bool bVar;
    CVARS_GET("browser_remote_websites", bVar); m_pCheckBoxRemoteBrowser->SetSelected(bVar);
    CVARS_GET("browser_remote_javascript", bVar); m_pCheckBoxRemoteJavascript->SetSelected(bVar);

    m_pGridBrowserBlacklist->Clear();
    m_pGridBrowserWhitelist->Clear();
    m_bBrowserListsChanged = false;

    auto pWebCore = CCore::GetSingleton().GetWebCore();
    if (pWebCore)
    {
        std::vector<std::pair<SString, bool>> customBlacklist;
        pWebCore->GetFilterEntriesByType(customBlacklist, eWebFilterType::WEBFILTER_USER);
        for (std::vector<std::pair<SString, bool>>::iterator iter = customBlacklist.begin(); iter != customBlacklist.end(); ++iter)
        {
            if (iter->second == false)
                m_pGridBrowserBlacklist->SetItemText(m_pGridBrowserBlacklist->AddRow(), 1, iter->first);
            else
                m_pGridBrowserWhitelist->SetItemText(m_pGridBrowserWhitelist->AddRow(), 1, iter->first);
        }
    }
}
