/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CLanguageSelector.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLanguageSelector.h"

#define LANGUAGE_ICON_SIZE_X         20
#define LANGUAGE_ICON_SIZE_Y         24
#define LANGUAGE_ICON_LABEL_GAP_X    10            // Gap between language icon and label

#define LABEL_SIZE_X                 165
#define LABEL_SIZE_Y                 16

#define LANGUAGE_ICON_LABEL_OFFSET_Y ((LANGUAGE_ICON_SIZE_Y - LABEL_SIZE_Y) / 2)

// Language icon and label combo
#define ITEM_SIZE_X                  165
#define ITEM_SIZE_Y                  24

// Current language button
#define BUTTON_MARGIN_X              20
#define BUTTON_MARGIN_Y              5

// All languages list
#define LIST_MARGIN_X                20
#define LIST_MARGIN_Y                18
#define LIST_ITEM_SPACING_X          10
#define LIST_ITEM_SPACING_Y          1

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::CLanguageSelector
//
// Create GUI items
//
///////////////////////////////////////////////////////////////
CLanguageSelector::CLanguageSelector(CGUIElement* pMainMenuCanvas)
{
    CreateGUI(pMainMenuCanvas);
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::~CLanguageSelector
//
// Delete GUI items
//
///////////////////////////////////////////////////////////////
CLanguageSelector::~CLanguageSelector()
{
    SAFE_DELETE(m_pButtonWindow);
    SAFE_DELETE(m_pListWindow);
    SAFE_DELETE(m_ButtonItem.pContainerPane);
    SAFE_DELETE(m_ButtonItem.pIcon);
    SAFE_DELETE(m_ButtonItem.pLabel);
    for (CLangListItem& item : m_ListItems)
    {
        SAFE_DELETE(item.pContainerPane);
        SAFE_DELETE(item.pIcon);
        SAFE_DELETE(item.pLabel);
    }
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::CreateGUI
//
// Create GUI items
//
///////////////////////////////////////////////////////////////
void CLanguageSelector::CreateGUI(CGUIElement* pMainMenuCanvas)
{
    //
    // Create current language button
    //
    CVector2D vecButtonSize(ITEM_SIZE_X + BUTTON_MARGIN_X * 2, ITEM_SIZE_Y + BUTTON_MARGIN_Y * 2);
    CVector2D vecButtonPos = pMainMenuCanvas->GetSize() - vecButtonSize - CVector2D(10, 3);
    {
        m_pButtonWindow = reinterpret_cast<CGUIScrollPane*>(g_pCore->GetGUI()->CreateScrollPane(pMainMenuCanvas));
        m_pButtonWindow->SetProperty("ContentPaneAutoSized", "False");
        m_pButtonWindow->SetPosition(vecButtonPos);
        m_pButtonWindow->SetSize(vecButtonSize);
        m_pButtonWindow->SetZOrderingEnabled(false);
        m_pButtonWindow->SetAlwaysOnTop(true);
        m_pButtonWindow->MoveToBack();
        m_pButtonWindow->SetVisible(true);

        CVector2D vecItemPosition(BUTTON_MARGIN_X, BUTTON_MARGIN_Y);

        CVector2D vecPaneSize(ITEM_SIZE_X, ITEM_SIZE_Y);
        CVector2D vecIconPos = vecItemPosition + CVector2D(LANGUAGE_ICON_SIZE_X / 2, (vecPaneSize.fY - LANGUAGE_ICON_SIZE_Y) / 2);
        CVector2D vecLabelPos = vecIconPos + CVector2D(LANGUAGE_ICON_SIZE_X + LANGUAGE_ICON_LABEL_GAP_X, LANGUAGE_ICON_LABEL_OFFSET_Y);

        CGUIScrollPane* pContainerPane = reinterpret_cast<CGUIScrollPane*>(g_pCore->GetGUI()->CreateScrollPane(m_pButtonWindow));
        pContainerPane->SetProperty("ContentPaneAutoSized", "False");
        pContainerPane->SetPosition(vecItemPosition);
        pContainerPane->SetSize(vecPaneSize);
        pContainerPane->SetZOrderingEnabled(false);
        pContainerPane->SetAlwaysOnTop(true);
        pContainerPane->SetVisible(true);

        CGUIStaticImage* pIcon = reinterpret_cast<CGUIStaticImage*>(g_pCore->GetGUI()->CreateStaticImage(m_pButtonWindow));
        pIcon->SetPosition(vecIconPos);
        pIcon->SetSize(CVector2D(LANGUAGE_ICON_SIZE_X, LANGUAGE_ICON_SIZE_Y));
        pIcon->SetZOrderingEnabled(false);
        pIcon->LoadFromFile(CalcMTASAPath("MTA\\cgui\\images\\the_language_icon.png"));

        CGUILabel* pLabel = reinterpret_cast<CGUILabel*>(g_pCore->GetGUI()->CreateLabel(m_pButtonWindow));
        pLabel->SetPosition(vecLabelPos);
        pLabel->SetSize(CVector2D(LABEL_SIZE_X, LABEL_SIZE_Y));
        pLabel->SetZOrderingEnabled(false);
        pLabel->SetText(g_pLocalization->GetLanguageNativeName());

        CLangListItem m_ButtonItem;
        m_ButtonItem.strLocale = "";
        m_ButtonItem.pContainerPane = pContainerPane;
        m_ButtonItem.pIcon = pIcon;
        m_ButtonItem.vecIconInitialPos = pIcon->GetPosition();
        m_ButtonItem.vecIconInitialSize = pIcon->GetSize();
        m_ButtonItem.pLabel = pLabel;
        m_ButtonItem.vecLabelInitialPos = pLabel->GetPosition();
        m_ButtonItem.vecLabelInitialSize = pLabel->GetSize();
        m_ButtonItem.pContainerPane->SetMouseButtonDownHandler(GUI_CALLBACK(&CLanguageSelector::OnButtonClick, this));
        m_ButtonItem.pContainerPane->SetMouseEnterHandler(GUI_CALLBACK(&CLanguageSelector::OnButtonEnter, this));
        m_ButtonItem.pContainerPane->SetMouseLeaveHandler(GUI_CALLBACK(&CLanguageSelector::OnButtonLeave, this));
    }

    //
    // Create all languages list
    //
    {
        m_pListWindow = reinterpret_cast<CGUIWindow*>(g_pCore->GetGUI()->CreateWnd(pMainMenuCanvas));
        m_pListWindow->SetMovable(false);
        m_pListWindow->SetAlwaysOnTop(true);
        m_pListWindow->SetFrameEnabled(false);
        m_pListWindow->SetTitlebarEnabled(false);
        m_pListWindow->SetCloseButtonEnabled(false);
        m_pListWindow->SetVisible(false);
        m_pListWindow->SetAlpha(0.98f);
        m_pListWindow->SetDeactivateHandler(GUI_CALLBACK(&CLanguageSelector::OnListDeactivate, this));

        // Calculate list position and size
        const uint uiNumColums = 3;
        const uint uiNumRows = (g_pCore->GetLocalization()->GetAvailableLocales().size() + uiNumColums - 1) / uiNumColums;
        CVector2D  vecListSize;
        vecListSize.fX = LIST_MARGIN_X * 2 + uiNumColums * ITEM_SIZE_X + (uiNumColums - 1) * LIST_ITEM_SPACING_X;
        vecListSize.fY = LIST_MARGIN_Y * 2 + uiNumRows * ITEM_SIZE_Y + (uiNumRows - 1) * LIST_ITEM_SPACING_Y;
        CVector2D vecListPos = CVector2D(vecButtonPos.fX + vecButtonSize.fX - vecListSize.fX, vecButtonPos.fY - vecListSize.fY + 2);
        m_pListWindow->SetPosition(vecListPos);
        m_pListWindow->SetSize(vecListSize);

        // Create each list item
        uint uiColumn = 0;
        uint uiRow = 0;
        for (const SString& strLocale : g_pCore->GetLocalization()->GetAvailableLocales())
        {
            CVector2D vecItemPosition;
            vecItemPosition.fX = LIST_MARGIN_X + uiColumn * (ITEM_SIZE_X + LIST_ITEM_SPACING_X);
            vecItemPosition.fY = LIST_MARGIN_Y + uiRow * (ITEM_SIZE_Y + LIST_ITEM_SPACING_Y);
            CLangListItem item = CreateGUILangItem(m_pListWindow, vecItemPosition, strLocale);
            item.pContainerPane->SetMouseButtonDownHandler(GUI_CALLBACK(&CLanguageSelector::OnListItemClick, this));
            item.pContainerPane->SetMouseEnterHandler(GUI_CALLBACK(&CLanguageSelector::OnListItemEnter, this));
            item.pContainerPane->SetMouseLeaveHandler(GUI_CALLBACK(&CLanguageSelector::OnListItemLeave, this));
            item.pContainerPane->SetUserData((void*)m_ListItems.size());
            m_ListItems.push_back(item);

            // Advance to next column/row
            if (++uiColumn == uiNumColums)
            {
                uiColumn = 0;
                uiRow++;
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::CreateGUILangItem
//
// Create GUI items for a language.
// ContainerPane is used to handle mouse interaction
//
///////////////////////////////////////////////////////////////
CLangListItem CLanguageSelector::CreateGUILangItem(CGUIElement* pGUIParent, const CVector2D& vecPanePosition, const SString& strLocale)
{
    CVector2D vecPaneSize(ITEM_SIZE_X, ITEM_SIZE_Y);
    CVector2D vecLabelPos = vecPanePosition;

    CGUIScrollPane* pContainerPane = reinterpret_cast<CGUIScrollPane*>(g_pCore->GetGUI()->CreateScrollPane(pGUIParent));
    pContainerPane->SetProperty("ContentPaneAutoSized", "False");
    pContainerPane->SetPosition(vecPanePosition);
    pContainerPane->SetSize(vecPaneSize);
    pContainerPane->SetZOrderingEnabled(false);
    pContainerPane->SetAlwaysOnTop(true);
    pContainerPane->SetVisible(true);

    CGUILabel* pLabel = reinterpret_cast<CGUILabel*>(g_pCore->GetGUI()->CreateLabel(pGUIParent));
    pLabel->SetPosition(vecLabelPos);
    pLabel->SetSize(CVector2D(LABEL_SIZE_X, LABEL_SIZE_Y));
    pLabel->SetZOrderingEnabled(false);
    pLabel->SetText(g_pLocalization->GetLanguageNativeName(strLocale));

    CLangListItem item;
    item.strLocale = strLocale;
    item.pContainerPane = pContainerPane;
    item.pLabel = pLabel;
    item.vecLabelInitialPos = pLabel->GetPosition();
    item.vecLabelInitialSize = pLabel->GetSize();
    return item;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::DoPulse
//
// Update visibilty & animations
//
///////////////////////////////////////////////////////////////
void CLanguageSelector::DoPulse()
{
    if (CLocalGUI::GetSingleton().GetMainMenu()->GetIsIngame())
    {
        // Can't change locale when connected, so hide
        SetLanguageListVisible(false);
        m_pButtonWindow->SetVisible(false);
        return;
    }

    // Calc delta seconds since last frame
    CTickCount now = CTickCount::Now();
    float      fDeltaSeconds = (now - m_LastTickCount).ToDouble() / 1000.f;
    m_LastTickCount = now;
    fDeltaSeconds = Clamp(0.01f, fDeltaSeconds, 0.10f);

    // Update current language button mouseover effect
    m_bListWasVisible = m_pListWindow->IsVisible();
    m_pButtonWindow->SetAlpha((m_bMouseOverButton || m_bListWasVisible) ? 1.f : 0.5f);
    m_pButtonWindow->SetVisible(true);

    // Animate list items if list is visible
    if (m_bListWasVisible)
    {
        for (CLangListItem& item : m_ListItems)
        {
            CLangListItem* pItem = &item;

            // Update animation position
            float fMaxAmount = fDeltaSeconds * 16.0f;
            pItem->fFocusEffectPos += Clamp(-fMaxAmount, pItem->fFocusEffectTarget - pItem->fFocusEffectPos, fMaxAmount);

            // Move label
            CVector2D vecLabelZoomPos = pItem->vecLabelInitialPos + CVector2D(pItem->vecLabelInitialSize.fX * pItem->fFocusEffectPos * 0.05f, 0);
            pItem->pLabel->SetPosition(vecLabelZoomPos);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::SetLanguageListVisible
//
// Show/hide list of languages
//
///////////////////////////////////////////////////////////////
void CLanguageSelector::SetLanguageListVisible(bool bVisible)
{
    if (bVisible)
    {
        m_pListWindow->BringToFront();
        if (!m_pListWindow->IsActive())
            m_pListWindow->Activate();
    }
    m_pListWindow->SetVisible(bVisible);
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::GetListItemByIndex
//
// Get list item pointer from supplied list index.
// Returns nullptr if index is out of range
//
///////////////////////////////////////////////////////////////
CLangListItem* CLanguageSelector::GetListItemByIndex(uint uiIndex)
{
    if (uiIndex < m_ListItems.size())
        return &m_ListItems[uiIndex];
    return nullptr;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::OnButtonEnter
//
// For mouseover effect
//
///////////////////////////////////////////////////////////////
bool CLanguageSelector::OnButtonEnter(CGUIElement* pElement)
{
    m_bMouseOverButton = true;
    return true;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::OnButtonLeave
//
// For mouseover effect
//
///////////////////////////////////////////////////////////////
bool CLanguageSelector::OnButtonLeave(CGUIElement* pElement)
{
    m_bMouseOverButton = false;
    return true;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::OnButtonClick
//
// Show the language list if it was hidden in the previous frame
//
///////////////////////////////////////////////////////////////
bool CLanguageSelector::OnButtonClick(CGUIElement* pElement)
{
    if (!m_bListWasVisible)
        SetLanguageListVisible(true);
    return true;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::OnListItemEnter
//
// For mouseover effect
//
///////////////////////////////////////////////////////////////
bool CLanguageSelector::OnListItemEnter(CGUIElement* pElement)
{
    CLangListItem* pItem = GetListItemByIndex((uint)pElement->GetUserData());
    if (pItem)
        pItem->fFocusEffectTarget = 1;
    return true;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::OnListItemLeave
//
// For mouseover effect
//
///////////////////////////////////////////////////////////////
bool CLanguageSelector::OnListItemLeave(CGUIElement* pElement)
{
    CLangListItem* pItem = GetListItemByIndex((uint)pElement->GetUserData());
    if (pItem)
        pItem->fFocusEffectTarget = 0;
    return true;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::OnListItemClick
//
// User has selected a language!
//
///////////////////////////////////////////////////////////////
bool CLanguageSelector::OnListItemClick(CGUIElement* pElement)
{
    CLangListItem* pItem = GetListItemByIndex((uint)pElement->GetUserData());
    if (pItem)
    {
        SetLanguageListVisible(false);
        SString strPrevLocale = CVARS_GET_VALUE<SString>("locale");
        SString strNewLocale = pItem->strLocale;
        if (strNewLocale != strPrevLocale)
        {
            // Locale change
            CVARS_SET("locale", strNewLocale);
            SetApplicationSetting("locale", strNewLocale);
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////
//
// CLanguageSelector::OnListDeactivate
//
// Clicked outside the language list, so hide it
//
///////////////////////////////////////////////////////////////
bool CLanguageSelector::OnListDeactivate(CGUIElement* pElement)
{
    SetLanguageListVisible(false);
    return true;
}
