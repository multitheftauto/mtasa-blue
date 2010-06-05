/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCommunityLogin.cpp
*  PURPOSE:     Header file for community login dialog
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCommunityLogin;

#ifndef __CCOMMUNITYLOGIN_H
#define __CCOMMUNITYLOGIN_H

typedef void (*BROWSERLOGINCALLBACK) ( void );

class CCommunityLogin
{
public:
                            CCommunityLogin     ( void );
    virtual                ~CCommunityLogin     ();

    void                    SetVisible          ( bool bVisible );
    bool                    IsVisible           ( void );

    void                    SetCallback         ( BROWSERLOGINCALLBACK );
    BROWSERLOGINCALLBACK    GetCallback         ( void );

    static void             OnLoginCallback     ( bool bResult, char* szError, void* obj );

private:
    bool                    OnButtonLoginClick  ( CGUIElement* pElement );
    bool                    OnButtonCancelClick ( CGUIElement* pElement );

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