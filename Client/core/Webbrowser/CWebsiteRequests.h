/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebsiteRequests.h
*  PURPOSE:     Website requests GUI class
*
*****************************************************************************/

#ifndef __CWEBSITEREQUESTS_H
#define __CWEBSITEREQUESTS_H

#define WEBSITEREQUESTS_WINDOW_DEFAULTWIDTH 476
#define WEBSITEREQUESTS_WINDOW_DEFAULTHEIGHT 297
class CGUIWindow;
class CGUILabel;
class CGUIMemo;
class CGUIButton;

class CWebsiteRequests
{
public:
    CWebsiteRequests  ();
    ~CWebsiteRequests ();

    void Show ();
    void Hide ();
    bool IsVisible ();

    void SetPendingRequests ( const std::vector<SString>& pendingRequests, WebRequestCallback* pCallback = nullptr );
    void Clear ();

protected:
    CGUIWindow*     m_pWindow;
    CGUILabel*      m_pLabel1;
    CGUILabel*      m_pLabel2;
    CGUIMemo*       m_pAddressMemo;
    CGUICheckBox*   m_pCheckRemember;
    CGUIButton*     m_pButtonAllow;
    CGUIButton*     m_pButtonDeny;
    std::list<WebRequestCallback> m_Callbacks;

private:
    void Callback ( bool bAllow );
    bool OnAllowButtonClick ( CGUIElement* pElement );
    bool OnDenyButtonClick  ( CGUIElement* pElement );

};

#endif
