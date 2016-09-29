#include <StdInc.h>
#include "CControlsSettingsTab.h"

CControlsSettingsTab::CControlsSettingsTab(CGUITab* pTab) :
    m_pTab(pTab)
{
    CGUI *pManager = g_pCore->GetGUI();
    m_dwFrameCount = 0;
    /**
    *  Controls tab
    **/
    float fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
        _("Mouse sensitivity:"),
        _("Vertical aim sensitivity:")
    );
    auto vecTemp = CVector2D(0, 13);
    CVector2D vecSize;
    //Mouse Options
    m_pControlsMouseLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Mouse options")));
    m_pControlsMouseLabel->SetPosition(CVector2D(vecTemp.fX + 11, vecTemp.fY));
    m_pControlsMouseLabel->AutoSize(NULL, 5.0f);
    m_pControlsMouseLabel->SetFont("default-bold-small");
    vecTemp.fY += 18;

    vecTemp.fX = 16;
    m_pInvertMouse = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Invert mouse vertically"), true));
    m_pInvertMouse->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pInvertMouse->AutoSize(NULL, 20.0f);

    m_pSteerWithMouse = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Steer with mouse"), true));
    m_pSteerWithMouse->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 16.0f));
    m_pSteerWithMouse->AutoSize(NULL, 20.0f);

    m_pFlyWithMouse = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Fly with mouse"), true));
    m_pFlyWithMouse->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pFlyWithMouse->AutoSize(NULL, 20.0f);

    // MouseSensitivity
    vecTemp.fY += 52.0f;
    m_pLabelMouseSensitivity = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Mouse sensitivity:")));
    m_pLabelMouseSensitivity->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pLabelMouseSensitivity->AutoSize();

    m_pMouseSensitivity = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pMouseSensitivity->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pMouseSensitivity->GetPosition(vecTemp);
    m_pMouseSensitivity->SetSize(CVector2D(160.0f, 20.0f));
    m_pMouseSensitivity->GetSize(vecSize);
    m_pMouseSensitivity->SetProperty("StepSize", "0.01");

    m_pLabelMouseSensitivityValue = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pLabelMouseSensitivityValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelMouseSensitivityValue->AutoSize("100%");
    vecTemp.fX = 16;
    vecTemp.fY += 26.f;

    // VerticalAimSensitivity
    m_pLabelVerticalAimSensitivity = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Vertical aim sensitivity:")));
    m_pLabelVerticalAimSensitivity->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pLabelVerticalAimSensitivity->AutoSize(m_pLabelVerticalAimSensitivity->GetText().c_str());

    m_pVerticalAimSensitivity = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pVerticalAimSensitivity->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pVerticalAimSensitivity->GetPosition(vecTemp);
    m_pVerticalAimSensitivity->SetSize(CVector2D(160.0f, 20.0f));
    m_pVerticalAimSensitivity->GetSize(vecSize);
    m_pVerticalAimSensitivity->SetProperty("StepSize", "0.01");

    m_pLabelVerticalAimSensitivityValue = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pLabelVerticalAimSensitivityValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelVerticalAimSensitivityValue->AutoSize("100%");
    vecTemp.fY += 30.f;

    vecTemp.fX = 16;
    //Joypad options
    m_pControlsJoypadLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Joypad options")));
    m_pControlsJoypadLabel->SetPosition(CVector2D(11, vecTemp.fY));
    m_pControlsJoypadLabel->AutoSize(NULL, 5.0f);
    m_pControlsJoypadLabel->SetFont("default-bold-small");
    vecTemp.fY += 18;

    //Create a mini-scrollpane for the radio buttons (only way to group them together)
    m_pControlsInputTypePane = std::unique_ptr<CGUIScrollPane>(pManager->CreateScrollPane(m_pTab.get()));
    m_pControlsInputTypePane->SetProperty("ContentPaneAutoSized", "False");
    m_pControlsInputTypePane->SetPosition(CVector2D(0, vecTemp.fY));
    m_pControlsInputTypePane->SetSize(CVector2D(1.0f, 0.27f), true);
    m_pControlsInputTypePane->SetZOrderingEnabled(false);

    m_pStandardControls = std::unique_ptr<CGUIRadioButton>(pManager->CreateRadioButton(m_pControlsInputTypePane.get(), _("Standard controls (Mouse + Keyboard)")));
    m_pStandardControls->SetSelected(true);
    m_pStandardControls->SetPosition(CVector2D(11, 0));
    m_pStandardControls->GetPosition(vecTemp);
    m_pStandardControls->AutoSize(NULL, 20.0f);
    m_pStandardControls->GetSize(vecSize);

    m_pClassicControls = std::unique_ptr<CGUIRadioButton>(pManager->CreateRadioButton(m_pControlsInputTypePane.get(), _("Classic controls (Joypad)")));
    m_pClassicControls->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 15, vecTemp.fY));
    m_pClassicControls->AutoSize(NULL, 20.0f);

    m_pControlsInputTypePane->GetPosition(vecTemp);
    vecTemp.fY += 24;

    //Advanced Joypad settings
    {
        m_JoypadSettingsRevision = -1;

        CJoystickManagerInterface* JoyMan = GetJoystickManager();

        m_pJoypadName = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get()));
        m_pJoypadName->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
        m_pJoypadName->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        m_pJoypadName->SetPosition(CVector2D(270, vecTemp.fY));

        m_pJoypadUnderline = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get()));
        m_pJoypadUnderline->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
        m_pJoypadUnderline->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        m_pJoypadUnderline->SetPosition(CVector2D(270, vecTemp.fY + 2));
        vecTemp.fY += 50;

        m_pEditDeadzone = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pTab.get()));
        m_pEditDeadzone->SetPosition(CVector2D(10, vecTemp.fY));
        m_pEditDeadzone->SetSize(CVector2D(45.0f, 24.0f));
        m_pEditDeadzone->SetMaxLength(3);
        m_pEditDeadzone->SetTextChangedHandler(GUI_CALLBACK(&CControlsSettingsTab::OnJoypadTextChanged, this));
        vecTemp.fY += 31;

        m_pEditSaturation = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pTab.get()));
        m_pEditSaturation->SetPosition(CVector2D(10, vecTemp.fY));
        m_pEditSaturation->SetSize(CVector2D(45.0f, 24.0f));
        m_pEditSaturation->SetMaxLength(3);
        m_pEditSaturation->SetTextChangedHandler(GUI_CALLBACK(&CControlsSettingsTab::OnJoypadTextChanged, this));

        m_pLabelDeadZone = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Dead Zone")));
        m_pLabelDeadZone->SetPosition(m_pEditDeadzone->GetPosition() + CVector2D(52.f, -1.f));
        m_pLabelDeadZone->AutoSize();
        m_pLabelDeadZone->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);

        m_pLabelSaturation = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Saturation")));
        m_pLabelSaturation->SetPosition(m_pEditSaturation->GetPosition() + CVector2D(52.f, -1.f));
        m_pLabelSaturation->AutoSize();
        m_pLabelSaturation->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        vecTemp.fY += 106;

        m_pLabelHelp = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Use the 'Binds' tab for joypad buttons.")));
        m_pLabelHelp->SetPosition(CVector2D(10, vecTemp.fY));
        m_pLabelHelp->AutoSize();
        vecTemp.fY += -91;

        // Layout the mapping buttons like a dual axis joypad
        CVector2D vecPosList[] = { CVector2D(162,  202),     // Left Stick
            CVector2D(280,  202),
            CVector2D(221,  182),
            CVector2D(221,  220),

            CVector2D(351,  202),     // Right Stick
            CVector2D(469,  202),
            CVector2D(410,  182),
            CVector2D(410,  220),

            CVector2D(410,  276),     // Acceleration/Brake
            CVector2D(221,  276) };

        for (int i = 0; i < JoyMan->GetOutputCount() && i < 10; i++)
        {
            CVector2D vecPos = vecPosList[i];
            vecPos.fY += vecTemp.fY - 231;
            auto pButton = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get()));
            pButton->SetPosition(vecPos + CVector2D(10, 0));
            pButton->SetSize(CVector2D(48.0f, 24.0f));
            pButton->GetSize(vecSize);
            pButton->SetUserData((void*)i);
            pButton->SetClickHandler(GUI_CALLBACK(&CControlsSettingsTab::OnAxisSelectClick, this));
            pButton->SetZOrderingEnabled(false);

            auto pLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get()));
            pLabel->SetSize(CVector2D(160.0f, 24.0f));
            pLabel->SetPosition(CVector2D((vecPos.fX + 10) + vecSize.fX*0.5f - 80.0f, vecPos.fY - 26));
            pLabel->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
            pLabel->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
            pLabel->SetVisible(i >= 8);      // Hide all labels except 'Acceleration' and 'Brake'

            m_pJoypadLabels.push_back(std::move(pLabel));
            m_pJoypadButtons.push_back(std::move(pButton));
        }

        vecTemp.fY += -74;
        m_pLabelLeft = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Left Stick")));
        m_pLabelLeft->AutoSize();
        m_pLabelLeft->GetSize(vecSize);
        m_pLabelLeft->SetPosition(CVector2D(255.0f - vecSize.fX*0.5f, vecTemp.fY));
        m_pLabelLeft->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);

        m_pLabelRight = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Right Stick")));
        m_pLabelRight->AutoSize();
        m_pLabelRight->GetSize(vecSize);
        m_pLabelRight->SetPosition(CVector2D(444.0f - vecSize.fX*0.5f, vecTemp.fY));
        m_pLabelRight->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);

    }

    m_pTab->GetParent()->GetSize(vecTemp);
    m_pControlsDefButton = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Load defaults")));
    m_pControlsDefButton->SetClickHandler(GUI_CALLBACK(&CControlsSettingsTab::OnControlsDefaultClick, this));
    m_pControlsDefButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pControlsDefButton->GetSize(vecSize);
    m_pControlsDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    m_pControlsDefButton->SetZOrderingEnabled(false);

    m_pMouseSensitivity->SetOnScrollHandler(GUI_CALLBACK(&CControlsSettingsTab::OnMouseSensitivityChanged, this));
    m_pVerticalAimSensitivity->SetOnScrollHandler(GUI_CALLBACK(&CControlsSettingsTab::OnVerticalAimSensitivityChanged, this));
}


void CControlsSettingsTab::UpdateJoypadTab()
{
    CJoystickManagerInterface* JoyMan = GetJoystickManager();

    // Has anything changed?
    if (m_JoypadSettingsRevision == JoyMan->GetSettingsRevision())
        return;

    // Update the joystick name
    std::string strJoystickName = JoyMan->IsJoypadConnected() ? JoyMan->GetControllerName() : _("Joypad not detected  -  Check connections and restart game");

    m_pJoypadName->SetPosition(CVector2D(270, m_pJoypadName->GetPosition().fY));
    m_pJoypadName->SetText(strJoystickName.c_str());
    m_pJoypadName->AutoSize(strJoystickName.c_str());
    m_pJoypadName->SetPosition(m_pJoypadName->GetPosition() - CVector2D(m_pJoypadName->GetSize().fX * 0.5, 0.0f));


    // Joystick name underline
    std::string strUnderline = "";
    int inumChars = m_pJoypadName->GetSize().fX / 7.f + 0.5f;
    for (int i = 0; i < inumChars; i++)
        strUnderline = strUnderline + "_";

    m_pJoypadUnderline->SetPosition(CVector2D(270, m_pJoypadUnderline->GetPosition().fY));
    m_pJoypadUnderline->SetText(strUnderline.c_str());
    m_pJoypadUnderline->AutoSize(strUnderline.c_str());
    m_pJoypadUnderline->SetPosition(m_pJoypadUnderline->GetPosition() - CVector2D(m_pJoypadUnderline->GetSize().fX * 0.5, 0.0f));
    m_pJoypadUnderline->SetVisible(JoyMan->IsJoypadConnected());


    // Update DeadZone and Saturation edit boxes
    char szDeadzone[32] = "";
    char szSaturation[32] = "";
    snprintf(szDeadzone, 10, "%d", JoyMan->GetDeadZone());
    snprintf(szSaturation, 10, "%d", JoyMan->GetSaturation());

    m_pEditDeadzone->SetText(szDeadzone);
    m_pEditSaturation->SetText(szSaturation);


    // Update axes labels and buttons
    for (int i = 0; i < JoyMan->GetOutputCount() && i < (int)m_pJoypadButtons.size(); i++)
    {
        std::string outputName = JoyMan->GetOutputName(i);         // LeftStickPosX etc
        std::string inputName = JoyMan->GetOutputInputName(i);     // X+ or RZ- etc

        m_pJoypadLabels[i]->SetText(outputName.c_str());
        m_pJoypadButtons[i]->SetText(inputName.c_str());
    }

    m_JoypadSettingsRevision = JoyMan->GetSettingsRevision();
}


//
// React to the DeadZone and Saturation edit boxes changing
//
bool CControlsSettingsTab::OnJoypadTextChanged(CGUIElement* pElement)
{
    // Update from GUI
    GetJoystickManager()->SetDeadZone(atoi(m_pEditDeadzone->GetText().c_str()));
    GetJoystickManager()->SetSaturation(atoi(m_pEditSaturation->GetText().c_str()));

    // Dont immediately read back these settings
    m_JoypadSettingsRevision = GetJoystickManager()->GetSettingsRevision();

    return true;
}


//
// Called every frame. If capturing an axis, see if its all done yet.
//
void CControlsSettingsTab::UpdateCaptureAxis()
{
    if (m_bCaptureAxis)
    {
        // Are we done?        
        if (GetJoystickManager()->IsAxisBindComplete())
        {
            // Remove the messagebox we created earlier
            CCore::GetSingleton().RemoveMessageBox();

            // Update GUI elements
            UpdateJoypadTab();

            m_bCaptureAxis = false;
        }
    }
}


//
// Called when the user clicks on an map axis button. Starts the capture axis process.
//
bool CControlsSettingsTab::OnAxisSelectClick(CGUIElement* pElement)
{
    int index = reinterpret_cast <int> (pElement->GetUserData());

    if (GetJoystickManager()->BindNextUsedAxisToOutput(index))
    {
        m_bCaptureAxis = true;
        CCore::GetSingleton().ShowMessageBox(_("Binding axis"), _("Move an axis to bind, or escape to clear"), MB_ICON_QUESTION);
    }

    return true;
}


//
// Called when the user clicks on the controls tab 'Load Defaults' button.
//
bool CControlsSettingsTab::OnControlsDefaultClick(CGUIElement* pElement)
{
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    CControllerConfigManager* pController = g_pCore->GetGame()->GetControllerConfigManager();

    // Load the default settings
    GetJoystickManager()->SetDefaults();
    CVARS_SET("invert_mouse", false);
    CVARS_SET("fly_with_mouse", false);
    CVARS_SET("steer_with_mouse", false);
    CVARS_SET("classic_controls", false);
    pController->SetVerticalAimSensitivity(0.5f);
    CVARS_SET("vertical_aim_sensitivity", pController->GetVerticalAimSensitivityRawValue());
    gameSettings->SetMouseSensitivity(0.5f);

    // Set game vars
    pController->SetMouseInverted(CVARS_GET_VALUE < bool >("invert_mouse"));
    pController->SetFlyWithMouse(CVARS_GET_VALUE < bool >("fly_with_mouse"));
    pController->SetSteerWithMouse(CVARS_GET_VALUE < bool >("steer_with_mouse"));
    pController->SetClassicControls(CVARS_GET_VALUE < bool >("classic_controls"));
    pController->SetVerticalAimSensitivityRawValue(CVARS_GET_VALUE < float >("vertical_aim_sensitivity"));

    // Update the GUI
    UpdateJoypadTab();
    m_pInvertMouse->SetSelected(CVARS_GET_VALUE < bool >("invert_mouse"));
    m_pFlyWithMouse->SetSelected(CVARS_GET_VALUE < bool >("fly_with_mouse"));
    m_pSteerWithMouse->SetSelected(CVARS_GET_VALUE < bool >("steer_with_mouse"));
    m_pStandardControls->SetSelected(!CVARS_GET_VALUE < bool >("classic_controls"));
    m_pClassicControls->SetSelected(CVARS_GET_VALUE < bool >("classic_controls"));
    m_pMouseSensitivity->SetScrollPosition(gameSettings->GetMouseSensitivity());
    m_pVerticalAimSensitivity->SetScrollPosition(pController->GetVerticalAimSensitivity());

    return true;
}

//
// Saves the Joypad settings
//
void CControlsSettingsTab::ProcessJoypad(void)
{
    // Update from GUI
    GetJoystickManager()->SetDeadZone(atoi(m_pEditDeadzone->GetText().c_str()));
    GetJoystickManager()->SetSaturation(atoi(m_pEditSaturation->GetText().c_str()));

    GetJoystickManager()->SaveToXML();
}

bool CControlsSettingsTab::OnMouseSensitivityChanged(CGUIElement* pElement)
{
    int iMouseSensitivity = (m_pMouseSensitivity->GetScrollPosition()) * 100;

    m_pLabelMouseSensitivityValue->SetText(SString("%i%%", iMouseSensitivity).c_str());
    return true;
}

bool CControlsSettingsTab::OnVerticalAimSensitivityChanged(CGUIElement* pElement)
{
    int iSensitivity = m_pVerticalAimSensitivity->GetScrollPosition() * 100;
    m_pLabelVerticalAimSensitivityValue->SetText(SString("%i%%", iSensitivity));
    return true;
}

void CControlsSettingsTab::SaveData(void)
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    // Very hacky
    CControllerConfigManager * pController = g_pCore->GetGame()->GetControllerConfigManager();
    CVARS_SET("invert_mouse", m_pInvertMouse->GetSelected());
    pController->SetMouseInverted(m_pInvertMouse->GetSelected());
    CVARS_SET("steer_with_mouse", m_pSteerWithMouse->GetSelected());
    pController->SetSteerWithMouse(m_pSteerWithMouse->GetSelected());
    CVARS_SET("fly_with_mouse", m_pFlyWithMouse->GetSelected());
    pController->SetFlyWithMouse(m_pFlyWithMouse->GetSelected());
    CVARS_SET("classic_controls", m_pClassicControls->GetSelected());
    pController->SetClassicControls(m_pClassicControls->GetSelected());
    pController->SetVerticalAimSensitivity(m_pVerticalAimSensitivity->GetScrollPosition());
    CVARS_SET("vertical_aim_sensitivity", pController->GetVerticalAimSensitivityRawValue());
    gameSettings->SetMouseSensitivity(m_pMouseSensitivity->GetScrollPosition());
   
}

void CControlsSettingsTab::LoadData()
{
    bool bVar;
    // Controls
    CVARS_GET("invert_mouse", bVar); m_pInvertMouse->SetSelected(bVar);
    CVARS_GET("steer_with_mouse", bVar); m_pSteerWithMouse->SetSelected(bVar);
    CVARS_GET("fly_with_mouse", bVar); m_pFlyWithMouse->SetSelected(bVar);
    CVARS_GET("classic_controls", bVar); m_pClassicControls->SetSelected(bVar);

    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    CControllerConfigManager* pController = g_pCore->GetGame()->GetControllerConfigManager();

    m_pMouseSensitivity->SetScrollPosition(gameSettings->GetMouseSensitivity());
    pController->SetVerticalAimSensitivityRawValue(CVARS_GET_VALUE < float >("vertical_aim_sensitivity"));
    m_pVerticalAimSensitivity->SetScrollPosition(pController->GetVerticalAimSensitivity());
}



void CControlsSettingsTab::Update(void)
{
    // Once each 30 frames
    if (m_dwFrameCount >= CORE_SETTINGS_UPDATE_INTERVAL) {

        UpdateJoypadTab();

        m_dwFrameCount = 0;
    }
    m_dwFrameCount++;

    UpdateCaptureAxis();

}
