/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFavouritesAddByIP.h
*  PURPOSE:     Header file for add to favourites by IP dialog
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerBrowserLogin;

#ifndef __CSERVERBROWSERLOGIN_H
#define __CSERVERBROWSERLOGIN_H

typedef void (*BROWSERLOGINCALLBACK) ( void );

class CServerBrowserLogin
{
public:
                            CServerBrowserLogin ( void );
    virtual                ~CServerBrowserLogin ();

    void                    SetVisible          ( bool bVisible );
    bool                    IsVisible           ( void );

    void                    SetCallback         ( BROWSERLOGINCALLBACK );
    BROWSERLOGINCALLBACK    GetCallback         ( void );

    static void             OnLoginCallback     ( bool bResult, char* szError, void* obj );

private:
    bool                    OnButtonBackClick   ( CGUIElement* pElement );

    BROWSERLOGINCALLBACK    m_pCallBack;

    CGUIWindow*             m_pWindow;

    CGUILabel*              m_pLabelText;
    CGUILabel*              m_pLabelUsername;
    CGUILabel*              m_pLabelPassword;

    CGUIEdit*               m_pEditUsername;
    CGUIEdit*               m_pEditPassword;

    CGUIButton*             m_pButtonLogin;
    CGUIButton*             m_pButtonCancel;
};

#endif