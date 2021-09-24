/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CWebsiteRequests.h
 *  PURPOSE:     Website requests GUI class
 *
 *****************************************************************************/
#pragma once

#define WEBSITEREQUESTS_WINDOW_DEFAULTWIDTH 476
#define WEBSITEREQUESTS_WINDOW_DEFAULTHEIGHT 297

/* TODO AFTER CEGUI API REWRITE */
//class CGUIWindow;
//class CGUILabel;
//class CGUIMemo;
//class CGUIButton;

class CWebsiteRequests
{
public:
    CWebsiteRequests();
    ~CWebsiteRequests();

    void Show();
    void Hide();
    bool IsVisible();

    void SetPendingRequests(const std::unordered_set<SString>& pendingRequests, WebRequestCallback* pCallback = nullptr);
    void Clear();

protected:
    /* TODO AFTER CEGUI API REWRITE */
    //CGUIWindow*                   m_pWindow;
    //CGUILabel*                    m_pLabel1;
    //CGUILabel*                    m_pLabel2;
    //CGUIMemo*                     m_pAddressMemo;
    //CGUICheckBox*                 m_pCheckRemember;
    //CGUIButton*                   m_pButtonAllow;
    //CGUIButton*                   m_pButtonDeny;
    std::list<WebRequestCallback> m_Callbacks;

private:
    void Callback(bool bAllow, const std::unordered_set<SString>& requests);

    /* TODO AFTER CEGUI API REWRITE */
    //bool OnAllowButtonClick();
    //bool OnDenyButtonClick();
};
