/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CNewsBrowser.h
*  PURPOSE:
*  DEVELOPERS:  shitface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNEWSBROWSER_H
#define __CNEWSBROWSER_H

struct SNewsItem
{
    SString                     strContentFullDir;
    SString                     strHeadline;
    SString                     strDate;
    SString                     strLayoutFilename;
    std::vector < SString >     imagesetFilenameList;
};


class CNewsBrowser
{
public:
    ZERO_ON_NEW
                        CNewsBrowser            ( void );
                        ~CNewsBrowser           ( void );

    void                SetVisible              ( bool bVisible );
    bool                IsVisible               ( void );
    void                CreateHeadlines         ( void );
    void                SwitchToTab             ( int iIndex );

protected:
    void                InitNewsItemList        ( void );
    void                AddNewsTab              ( const SNewsItem& newsItem );
    CGUIWindow*         LoadLayoutAndImages     ( CGUIElement* pParent, const SNewsItem& newsItem );
    void                CreateGUI               ( void );
    bool                OnOKButtonClick         ( CGUIElement* pElement );

    std::vector < SNewsItem >       m_NewsitemList;
    std::unique_ptr<CGUIWindow>     m_pWindow;
    std::unique_ptr<CGUITabPanel>   m_pTabPanel;
    std::unique_ptr<CGUIButton>     m_pButtonOK;
    std::vector < std::unique_ptr<CGUITab> >        m_TabList;
    std::vector < std::unique_ptr<CGUIWindow> >     m_TabContentList;
};

#endif
