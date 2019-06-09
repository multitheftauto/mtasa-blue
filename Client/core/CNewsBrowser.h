/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CNewsBrowser.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

struct SNewsItem
{
    SString              strContentFullDir;
    SString              strHeadline;
    SString              strDate;
    SString              strLayoutFilename;
    std::vector<SString> imagesetFilenameList;
};

class CNewsBrowser
{
public:
    CNewsBrowser();
    ~CNewsBrowser();

    void SetVisible(bool bVisible);
    bool IsVisible();
    void CreateHeadlines();
    void SwitchToTab(int iIndex);

protected:
    void        InitNewsItemList();
    void        AddNewsTab(const SNewsItem& newsItem);
    CGUIWindow* LoadLayoutAndImages(CGUIElement* pParent, const SNewsItem& newsItem);
    void        CreateGUI();
    void        DestroyGUI();
    bool        OnOKButtonClick(CGUIElement* pElement);

    std::vector<SNewsItem>   m_NewsitemList;
    CGUIWindow*              m_pWindow = nullptr; 
    CGUITabPanel*            m_pTabPanel = nullptr;
    CGUIButton*              m_pButtonOK = nullptr;
    std::vector<CGUITab*>    m_TabList;
    std::vector<CGUIWindow*> m_TabContentList;
};
