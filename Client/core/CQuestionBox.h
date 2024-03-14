/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CQuestionBox.h
 *  PURPOSE:     Header file for quick connect window class
 *
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CQuestionBox;

#pragma once

#include "CMainMenu.h"

typedef void (*pfnQuestionCallback)(void*, std::uint32_t);
typedef void (*pfnQuestionEditCallback)(void*, std::uint32_t, std::string);

class CQuestionBox
{
public:
    CQuestionBox();
    ~CQuestionBox();

    void         Hide();
    void         Show();
    void         Reset();
    void         SetTitle(const SString& strTitle);
    void         SetMessage(const SString& strMsg);
    void         AppendMessage(const SString& strMsg);
    void         SetButton(std::uint32_t uiButton, const SString& strText);
    CGUIEdit*    SetEditbox(std::uint32_t uiEditbox, const SString& strText);
    void         SetCallback(pfnQuestionCallback callback, void* ptr = NULL);
    void         SetCallbackEdit(pfnQuestionEditCallback callback, void* ptr = NULL);
    void         SetOnLineHelpOption(const SString& strTroubleType);
    std::uint32_t PollButtons();
    bool         IsVisible();
    void         SetAutoCloseOnConnect(bool bEnable);
    void         OnConnect();

private:
    bool OnButtonClick(CGUIElement* pElement);

    CGUIWindow*              m_pWindow;
    CGUILabel*               m_pMessage;
    std::vector<CGUIButton*> m_ButtonList;
    std::vector<CGUIEdit*>   m_EditList;
    std::uint32_t             m_uiLastButton;
    std::uint32_t             m_uiActiveButtons;
    std::uint32_t             m_uiActiveEditboxes;
    pfnQuestionCallback      m_Callback;
    pfnQuestionEditCallback  m_CallbackEdit;
    void*                    m_CallbackParameter;
    SString                  m_strMsg;
    bool                     m_bAutoCloseOnConnect;
};
