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

class CVideoSettingsTab
{
public:
    CVideoSettingsTab(CGUITab* pTab);
    void                UpdateVideoTab(void);
    void                PopulateResolutionComboBox(void);
    void                UpdateFullScreenComboBoxEnabled(void);
    void SaveData();
    void LoadData();

    bool ChangeRequiresRestart() {
        return m_bRequiresRestart;
    }

private:
    bool                OnFieldOfViewChanged(CGUIElement* pElement);
    bool                OnDrawDistanceChanged(CGUIElement* pElement);
    bool                OnBrightnessChanged(CGUIElement* pElement);
    bool                OnAnisotropicChanged(CGUIElement* pElement);
    bool                OnFxQualityChanged(CGUIElement* pElement);
    bool                OnVolumetricShadowsClick(CGUIElement* pElement);
    bool                ShowUnsafeResolutionsClick(CGUIElement* pElement);
    bool                OnWindowedClick(CGUIElement* pElement);
    bool                OnVideoDefaultClick(CGUIElement* pElement);

    bool m_bRequiresRestart;
    int                 m_iMaxAnisotropic;
    bool                m_bShownVolumetricShadowsWarning;


    std::unique_ptr<CGUITab> m_pTab;

    std::unique_ptr<CGUILabel>          m_pFullscreenStyleLabel;
    std::unique_ptr<CGUIComboBox>       m_pFullscreenStyleCombo;
    std::unique_ptr<CGUILabel>          m_pVideoGeneralLabel;
    std::unique_ptr<CGUILabel>          m_pVideoResolutionLabel;
    std::unique_ptr<CGUIComboBox>       m_pComboResolution;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxMipMapping;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxWindowed;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxHudMatchAspectRatio;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxMinimize;
    std::unique_ptr<CGUIComboBox>       m_pComboFxQuality;
    std::unique_ptr<CGUILabel>          m_pFXQualityLabel;
    std::unique_ptr<CGUIComboBox>       m_pComboAspectRatio;
    std::unique_ptr<CGUILabel>          m_pAspectRatioLabel;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxVolumetricShadows;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxDeviceSelectionDialog;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxShowUnsafeResolutions;

    std::unique_ptr<CGUICheckBox>       m_pCheckBoxGrass;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxHeatHaze;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxTyreSmokeParticles;
    std::unique_ptr<CGUILabel>          m_pFieldOfViewLabel;
    std::unique_ptr<CGUIScrollBar>      m_pFieldOfView;
    std::unique_ptr<CGUILabel>          m_pFieldOfViewValueLabel;
    std::unique_ptr<CGUILabel>          m_pDrawDistanceLabel;
    std::unique_ptr<CGUIScrollBar>      m_pDrawDistance;
    std::unique_ptr<CGUILabel>          m_pDrawDistanceValueLabel;
    std::unique_ptr<CGUILabel>          m_pBrightnessLabel;
    std::unique_ptr<CGUIScrollBar>      m_pBrightness;
    std::unique_ptr<CGUILabel>          m_pBrightnessValueLabel;
    std::unique_ptr<CGUILabel>          m_pAnisotropicLabel;
    std::unique_ptr<CGUIScrollBar>      m_pAnisotropic;
    std::unique_ptr<CGUILabel>          m_pAnisotropicValueLabel;
    std::unique_ptr<CGUIComboBox>       m_pComboAntiAliasing;
    std::unique_ptr<CGUILabel>          m_pAntiAliasingLabel;
    std::unique_ptr<CGUIButton>         m_pVideoDefButton;
};
