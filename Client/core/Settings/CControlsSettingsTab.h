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

class CControlsSettingsTab
{
public:
    CControlsSettingsTab(CGUITab* pTab);
    void                UpdateJoypadTab();
    void SaveData();
    void LoadData();
    void                ProcessJoypad(void);
    void                Update(void);
private:

    void                UpdateCaptureAxis();

    bool                OnJoypadTextChanged(CGUIElement* pElement);
    bool                OnAxisSelectClick(CGUIElement* pElement);
    bool                OnControlsDefaultClick(CGUIElement* pElement);


    bool                OnMouseSensitivityChanged(CGUIElement* pElement);
    bool                OnVerticalAimSensitivityChanged(CGUIElement* pElement);

    std::unique_ptr<CGUITab> m_pTab;

    std::unique_ptr<CGUILabel>                  m_pJoypadName;
    std::unique_ptr<CGUILabel>                  m_pJoypadUnderline;
    std::unique_ptr<CGUIEdit>                   m_pEditDeadzone;
    std::unique_ptr<CGUIEdit>                   m_pEditSaturation;
    std::vector < std::unique_ptr<CGUILabel> >  m_pJoypadLabels;
    std::vector < std::unique_ptr<CGUIButton> > m_pJoypadButtons;
    int                         m_JoypadSettingsRevision;
    bool                m_bCaptureAxis;

    DWORD               m_dwFrameCount;
    std::unique_ptr<CGUILabel>          m_pControlsMouseLabel;
    std::unique_ptr<CGUICheckBox>       m_pInvertMouse;
    std::unique_ptr<CGUICheckBox>       m_pSteerWithMouse;
    std::unique_ptr<CGUICheckBox>       m_pFlyWithMouse;
    std::unique_ptr<CGUILabel>          m_pLabelMouseSensitivity;
    std::unique_ptr<CGUIScrollBar>      m_pMouseSensitivity;
    std::unique_ptr<CGUILabel>          m_pLabelMouseSensitivityValue;
    std::unique_ptr<CGUILabel>          m_pLabelVerticalAimSensitivity;
    std::unique_ptr<CGUIScrollBar>      m_pVerticalAimSensitivity;
    std::unique_ptr<CGUILabel>          m_pLabelVerticalAimSensitivityValue;

    std::unique_ptr<CGUILabel>          m_pLabelLeft;
    std::unique_ptr<CGUILabel>          m_pLabelRight;
    std::unique_ptr<CGUIButton>         m_pControlsDefButton;

    std::unique_ptr<CGUILabel> m_pLabelDeadZone;
    std::unique_ptr<CGUILabel> m_pLabelSaturation;
    std::unique_ptr<CGUILabel> m_pLabelHelp;

    std::unique_ptr<CGUILabel>          m_pControlsJoypadLabel;
    std::unique_ptr<CGUIScrollPane>     m_pControlsInputTypePane;
    std::unique_ptr<CGUIRadioButton>    m_pStandardControls;
    std::unique_ptr<CGUIRadioButton>    m_pClassicControls;

};
