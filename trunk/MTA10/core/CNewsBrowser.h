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

class CNewsBrowser
{
public:
    ZERO_ON_NEW
                        CNewsBrowser            ( void );
                        ~CNewsBrowser           ( void );

    void                SetVisible              ( bool bVisible );
    bool                IsVisible               ( void );

protected:
    void                AddNewsTab              ( const SString& strTitle, const SString& strContent );
    CGUIWindow*         LoadLayoutAndImages     ( CGUIElement* pParent, const SString& strFilename );
    void                CreateGUI               ( void );
    void                DestroyGUI              ( void );
    bool                OnOKButtonClick         ( CGUIElement* pElement );

    CGUIWindow*                     m_pWindow;
    CGUITabPanel*                   m_pTabPanel;
    CGUIButton*                     m_pButtonOK;
    std::vector < CGUITab* >        m_TabList;
    std::vector < CGUIWindow* >     m_TabContentList;
};

#endif
