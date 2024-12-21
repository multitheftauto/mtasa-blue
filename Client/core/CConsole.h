/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CConsole.h
 *  PURPOSE:     Header file for console class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CConsole;

#pragma once

#include <core/CConsoleInterface.h>
#include "CCore.h"
#include "CCommands.h"
#include "CSingleton.h"
#include "CEntryHistory.h"

class CConsole : public CConsoleInterface
{
public:
    CConsole(CGUI* pManager, CGUIElement* pParent = NULL);
    ~CConsole();

    void Echo(const char* szText);
    void Print(const char* szText);
    void Printf(const char* szFormat, ...);

    void Clear();

    bool IsEnabled();
    void SetEnabled(bool bEnabled);

    bool IsVisible();
    void SetVisible(bool bVisible);
    void Show();
    void Hide();
    bool IsInputActive();
    void ActivateInput();

    void GetCommandInfo(const std::string& strIn, std::string& strCmdOut, std::string& strCmdLineOut);

    void ResetHistoryChanges();
    void SelectHistoryEntry(int iEntry);
    bool SetNextHistoryText();
    bool SetPreviousHistoryText();

    void SetNextAutoCompleteMatch();
    void ResetAutoCompleteMatch();

    CVector2D GetPosition();
    void      SetPosition(CVector2D& vecPosition);

    CVector2D GetSize();
    void      SetSize(CVector2D& vecSize);

protected:
    bool OnCloseButtonClick(CGUIElement* pElement);
    bool Edit_OnTextAccepted(CGUIElement* pElement);
    bool History_OnTextChanged(CGUIElement* pElement);
    void FlushPendingAdd();
    void GracefullySetEditboxText(const char* szText);
    bool GracefullyMoveEditboxCaret(CGUIElement* pElement);

private:
    void CreateElements(CGUIElement* pParent = NULL);
    void DestroyElements();

    bool OnWindowSize(CGUIElement* pElement);

    CGUI* m_pManager;

    CEntryHistory*   m_pConsoleHistory;
    int              m_iHistoryIndex;
    std::vector<int> m_AutoCompleteList;
    int              m_iAutoCompleteIndex;

    CGUIWindow* m_pWindow;
    CGUIEdit*   m_pInput;
    CGUIMemo*   m_pHistory;

    bool m_bIsEnabled;

    float   m_fWindowSpacer;
    float   m_fWindowSpacerTop;
    float   m_fWindowX;
    float   m_fWindowY;
    float   m_fInputHeight;
    SString m_strPendingAdd;
    SString m_strSavedInputText;
};
