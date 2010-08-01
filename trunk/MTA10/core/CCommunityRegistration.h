/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCommunityRegistration.h
*  PURPOSE:     Header file for community registration dialog
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCommunityRegistration;

#ifndef __CCOMMUNITYREGISTRATION_H
#define __CCOMMUNITYREGISTRATION_H

#define REGISTRATION_DELAY              9000
#define REGISTRATION_URL                "http://dp3.community.mtasa.com/core/1.1/register.php"
#define REGISTRATION_TEMP_FILE          "MTA\\temp.png"

enum eRegistrationResult
{
    REGISTRATION_ERROR_UNEXPECTED,
    REGISTRATION_ERROR_SUCCESS,
    REGISTRATION_ERROR_REQUEST,
    REGISTRATION_ERROR_ERROR
};

class CCommunityRegistration
{
    friend class CLocalGUI;

public:
                            CCommunityRegistration  ( void );
    virtual                ~CCommunityRegistration  ( void );

    void                    Open                ( void );

    void                    SetVisible          ( bool bVisible );
    bool                    IsVisible           ( void )                        { return m_pWindow->IsVisible(); };

    void                    SetRegisterFrozen   ( bool bFrozen );

    void                    DoPulse             ( void );

    bool                    HashString          ( const char* szString, std::string &strHashString );

protected:
    void                    CreateWindows       ( void );

private:

    // GUI elements
    bool                    OnButtonRegisterClick       ( CGUIElement* pElement );
    bool                    OnButtonCancelClick         ( CGUIElement* pElement );

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

    CGUIStaticImage*        m_pImageCode;

    CGUIButton*             m_pButtonRegister;
    CGUIButton*             m_pButtonCancel;

    // Community stuff
    CHTTPClient                 m_HTTP;

    std::string                 m_strCommunityHash;

    unsigned long               m_ulStartTime;
};

#endif
