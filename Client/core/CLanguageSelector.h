/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CLanguageSelector.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

struct CLangListItem
{
    CLangListItem() : fFocusEffectPos(0), fFocusEffectTarget(0), pContainerPane(nullptr), pFlag(nullptr), pLabel(nullptr) {}
    float            fFocusEffectPos = 0.0f;
    float            fFocusEffectTarget = 0.0f;
    SString          strLocale;
    CGUIElement*     pContainerPane = nullptr;
    CGUIStaticImage* pFlag = nullptr;
    CVector2D        vecFlagInitialPos;
    CVector2D        vecFlagInitialSize;
    CGUILabel*       pLabel = nullptr;
    CVector2D        vecLabelInitialPos;
    CVector2D        vecLabelInitialSize;
};

class CLanguageSelector
{
public:
    CLanguageSelector(CGUIElement* pMainMenuCanvas);
    ~CLanguageSelector();
    void DoPulse();

protected:
    void           CreateGUI(CGUIElement* pMainMenuCanvas);
    CLangListItem  CreateGUILangItem(CGUIElement* pGUIParent, const CVector2D& vecPanePosition, const SString& strLocale = "");
    void           SetLanguageListVisible(bool bVisible);
    SString        GetFlagFilename(SString strLocale = "");
    CLangListItem* GetListItemByIndex(uint uiIndex);
    bool           OnButtonEnter(CGUIElement* pElement);
    bool           OnButtonLeave(CGUIElement* pElement);
    bool           OnButtonClick(CGUIElement* pElement);
    bool           OnListItemEnter(CGUIElement* pElement);
    bool           OnListItemLeave(CGUIElement* pElement);
    bool           OnListItemClick(CGUIElement* pElement);
    bool           OnListDeactivate(CGUIElement* pElement);

    CGUIScrollPane*            m_pButtonWindow = nullptr;
    CGUIWindow*                m_pListWindow = nullptr;
    CLangListItem              m_ButtonItem;
    std::vector<CLangListItem> m_ListItems;
    CTickCount                 m_LastTickCount;
    bool                       m_bMouseOverButton = false;
    bool                       m_bListWasVisible = false;
};
