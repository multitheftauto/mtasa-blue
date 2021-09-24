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
    CLangListItem() : fFocusEffectPos(0), fFocusEffectTarget(0) {}
    float            fFocusEffectPos;
    float            fFocusEffectTarget;
    SString          strLocale;
    //CGUIElement*     pContainerPane; /* TODO AFTER CEGUI API REWRITE */
    //CGUIStaticImage* pFlag; /* TODO AFTER CEGUI API REWRITE */
    CVector2D        vecFlagInitialPos;
    CVector2D        vecFlagInitialSize;
    //CGUILabel*       pLabel; /* TODO AFTER CEGUI API REWRITE */
    CVector2D        vecLabelInitialPos;
    CVector2D        vecLabelInitialSize;
};

class CLanguageSelector
{
public:
    ZERO_ON_NEW

    //CLanguageSelector(CGUIElement* pMainMenuCanvas); /* TODO AFTER CEGUI API REWRITE */
    CLanguageSelector();

    ~CLanguageSelector();
    void DoPulse();

protected:
    //void           CreateGUI(CGUIElement* pMainMenuCanvas); /* TODO AFTER CEGUI API REWRITE */
    //CLangListItem  CreateGUILangItem(CGUIElement* pGUIParent, const CVector2D& vecPanePosition, const SString& strLocale = ""); /* TODO AFTER CEGUI API REWRITE */
    void           SetLanguageListVisible(bool bVisible);
    SString        GetFlagFilename(SString strLocale = "");
    CLangListItem* GetListItemByIndex(uint uiIndex);

    /* TODO AFTER CEGUI API REWRITE */
    //bool           OnButtonEnter(CGUIElement* pElement);
    //bool           OnButtonLeave(CGUIElement* pElement);
    //bool           OnButtonClick(CGUIElement* pElement);
    //bool           OnListItemEnter(CGUIElement* pElement);
    //bool           OnListItemLeave(CGUIElement* pElement);
    //bool           OnListItemClick(CGUIElement* pElement);
    //bool           OnListDeactivate(CGUIElement* pElement);

    /* TODO AFTER CEGUI API REWRITE */
    //CGUIScrollPane*            m_pButtonWindow;
    //CGUIWindow*                m_pListWindow;

    CLangListItem              m_ButtonItem;
    std::vector<CLangListItem> m_ListItems;
    CTickCount                 m_LastTickCount;
    bool                       m_bMouseOverButton;
    bool                       m_bListWasVisible;
};
