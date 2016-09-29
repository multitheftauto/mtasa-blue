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

class CMultiplayerSettingsTab 
{
public:
    CMultiplayerSettingsTab(CGUITab* pTab, CSettings* pSettings);
    std::string GetNickname() { return m_pEditNick->GetText(); }

    void SaveData();
    void LoadData();
    bool ChangeRequiresRestart() {
        return m_bCustomizedSAFilesChanged;
    }
    void                RequestNewNickname(void);
private:
    bool                m_bShownAllowScreenUploadMessage;
    bool m_bCustomizedSAFilesChanged;

    bool                OnAllowScreenUploadClick(CGUIElement* pElement);
    bool                OnCustomizedSAFilesClick(CGUIElement* pElement);
    bool                OnMapAlphaChanged(CGUIElement* pElement);
    std::unique_ptr<CGUITab> m_pTab;
    std::unique_ptr<CGUILabel>          m_pLabelNick;
    std::unique_ptr<CGUIEdit>           m_pEditNick;
    std::unique_ptr<CGUICheckBox>       m_pSavePasswords;
    std::unique_ptr<CGUICheckBox>       m_pAutoRefreshBrowser;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxAllowScreenUpload;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxCustomizedSAFiles;
    std::unique_ptr<CGUILabel>          m_pMapRenderingLabel;
    std::unique_ptr<CGUILabel>          m_pMapAlphaLabel;
    std::unique_ptr<CGUIScrollBar>      m_pMapAlpha;
    std::unique_ptr<CGUILabel>          m_pMapAlphaValueLabel;

};
