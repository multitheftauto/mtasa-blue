/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCommunityRegistration.h
*  PURPOSE:     Header file for community registration dialog
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCommunityRegistration;

#ifndef __CCOMMUNITYREGISTRATION_H
#define __CCOMMUNITYREGISTRATION_H

#define REGISTRATION_DELAY              150000
#define REGISTRATION_URL                "http://dp3.community.mtasa.com/core/1.0/register.php"
#define REGISTRATION_DATA_BUFFER_SIZE   1024

class CCommunityRegistration
{
public:
                            CCommunityRegistration  ( void );
    virtual                ~CCommunityRegistration  ( void );

    void                    Open                ( void );

    void                    SetVisible          ( bool bVisible )               { return m_pWindow->SetVisible ( bVisible ); };
    bool                    IsVisible           ( void )                        { return m_pWindow->IsVisible(); };

    void                    DoPulse             ( void );

private:

    // GUI elements
    bool                    OnButtonClick       ( CGUIElement* pElement );

    CGUIWindow*             m_pWindow;

    CGUILabel*              m_pLabelUsername;
    CGUILabel*              m_pLabelPassword;
    CGUILabel*              m_pLabelConfirm;
    CGUILabel*              m_pLabelEmail;
    CGUILabel*              m_pLabelCode;

    CGUIEdit*               m_pEditUsername;
    CGUIEdit*               m_pEditPassword;
    CGUIEdit*               m_pEditConfirm;
    CGUIEdit*               m_pEditEmail;
    CGUIEdit*               m_pEditCode;

    CGUIButton*             m_pButtonRegister;
    CGUIButton*             m_pButtonCancel;

    // Community stuff
    CHTTPClient                 m_HTTP;
    char                        m_szBuffer[REGISTRATION_DATA_BUFFER_SIZE];

    unsigned long               m_ulStartTime;
};

#endif