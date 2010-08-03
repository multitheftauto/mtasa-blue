/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CQuestionBox.h
*  PURPOSE:     Header file for quick connect window class
*  DEVELOPERS:  Plectrum Handyworthingtonshire
*
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CQuestionBox;

#ifndef __CQUESTIONBOX_H
#define __CQUESTIONBOX_H

#include "CMainMenu.h"

typedef void (*pfnQuestionCallback)        ( void*, unsigned int );


class CQuestionBox
{
public:
                        CQuestionBox                ( void );
                        ~CQuestionBox               ( void );

    void                Hide                        ( void );
    void                Show                        ( void );
    void                Reset                       ( void );
    void                SetTitle                    ( const SString& strTitle );
    void                SetMessage                  ( const SString& strMsg );
    void                SetButton                   ( unsigned int uiButton, const SString& strText );
    void                SetCallback                 ( pfnQuestionCallback callback, void* ptr = NULL );
    unsigned int        PollButtons                 ( void );
    bool                IsVisible                   ( void );

private:
    bool                OnButtonClick               ( CGUIElement* pElement );

    CGUIWindow*                 m_pWindow;
    CGUILabel*                  m_pMessage;
    std::vector < CGUIButton* > m_ButtonList;
    unsigned int                m_uiLastButton;
    unsigned int                m_uiActiveButtons;
    pfnQuestionCallback         m_Callback;
    void*                       m_CallbackParameter;
};



#endif
