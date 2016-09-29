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

class CAdvancedSettingsTab
{
public:
    CAdvancedSettingsTab(CGUITab* pTab);
    void SaveData();
    void LoadData();
private:
    bool                OnUpdateButtonClick(CGUIElement* pElement);
    bool                OnCachePathShowButtonClick(CGUIElement* pElement);
    bool                OnShowAdvancedSettingDescription(CGUIElement* pElement);
    bool                OnHideAdvancedSettingDescription(CGUIElement* pElement);

    std::unique_ptr<CGUITab> m_pTab;

    std::unique_ptr<CGUILabel>          m_pAdvancedSettingDescriptionLabel;
    std::unique_ptr<CGUILabel>          m_pPriorityLabel;
    std::unique_ptr<CGUIComboBox>       m_pPriorityCombo;
    std::unique_ptr<CGUILabel>          m_pFastClothesLabel;
    std::unique_ptr<CGUIComboBox>       m_pFastClothesCombo;
    std::unique_ptr<CGUILabel>          m_pBrowserSpeedLabel;
    std::unique_ptr<CGUIComboBox>       m_pBrowserSpeedCombo;
    std::unique_ptr<CGUILabel>          m_pSingleDownloadLabel;
    std::unique_ptr<CGUIComboBox>       m_pSingleDownloadCombo;
    std::unique_ptr<CGUILabel>          m_pPacketTagLabel;
    std::unique_ptr<CGUIComboBox>       m_pPacketTagCombo;
    std::unique_ptr<CGUILabel>          m_pProgressAnimationLabel;
    std::unique_ptr<CGUIComboBox>       m_pProgressAnimationCombo;
    std::unique_ptr<CGUILabel>          m_pDebugSettingLabel;
    std::unique_ptr<CGUIComboBox>       m_pDebugSettingCombo;
    std::unique_ptr<CGUILabel>          m_pWin8Label;
    std::unique_ptr<CGUICheckBox>       m_pWin8ColorCheckBox;
    std::unique_ptr<CGUICheckBox>       m_pWin8MouseCheckBox;
    std::unique_ptr<CGUILabel>          m_pUpdateBuildTypeLabel;
    std::unique_ptr<CGUIComboBox>       m_pUpdateBuildTypeCombo;
    std::unique_ptr<CGUILabel>          m_pUpdateAutoInstallLabel;
    std::unique_ptr<CGUIComboBox>       m_pUpdateAutoInstallCombo;
    std::unique_ptr<CGUIButton>         m_pButtonUpdate;
    std::unique_ptr<CGUILabel>          m_pAdvancedMiscLabel;
    std::unique_ptr<CGUILabel>          m_pAdvancedUpdaterLabel;
    std::unique_ptr<CGUILabel>          m_pCachePathLabel;
    std::unique_ptr<CGUILabel>          m_pCachePathValue;
    std::unique_ptr<CGUIButton>         m_pCachePathShowButton;
    std::unique_ptr<CGUILabel>          m_pStreamingMemoryLabel;
    std::unique_ptr<CGUIScrollBar>      m_pStreamingMemory;
    std::unique_ptr<CGUILabel>          m_pStreamingMemoryMinLabel;
    std::unique_ptr<CGUILabel>          m_pStreamingMemoryMaxLabel;
    std::unique_ptr<CGUILabel>          m_pStreamingMemoryLabelInfo;

};
