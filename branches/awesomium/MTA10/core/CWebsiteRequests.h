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

    void SetPendingRequests ( const std::vector<SString>& pendingRequests );
    void Clear ();

protected:
    CGUIWindow*     m_pWindow;
    CGUILabel*      m_pLabel;
    CGUIMemo*       m_pAddressMemo;
    CGUIButton*     m_pButtonAllow;
    CGUIButton*     m_pButtonDeny;

private:
    bool OnAllowButtonClick(CGUIElement* pElement);
    bool OnDenyButtonClick(CGUIElement* pElement);

};

#endif
