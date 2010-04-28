/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsole.h
*  PURPOSE:     Header file for console class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CConsole;

#ifndef __CCONSOLE_H
#define __CCONSOLE_H

#include <core/CConsoleInterface.h>
#include "CCore.h"
#include "CCommands.h"
#include "CSingleton.h"
#include "CConsoleHistory.h"

class CConsole : public CConsoleInterface
{
public:
                        CConsole                        ( CGUI* pManager, CGUIElement* pParent = NULL );
                        ~CConsole                       ( void );

    void                Echo                            ( const char* szText );
    void                Print                           ( const char* szText );
    void                Printf                          ( const char* szFormat, ... );

    void                Clear                           ( void );

    bool                IsEnabled                       ( void );
    void                SetEnabled                      ( bool bEnabled );

    bool                IsVisible                       ( void );
    void                SetVisible                      ( bool bVisible );
    void                Show                            ( void );
    void                Hide                            ( void );
    void                ActivateInput                   ( void );

    void                HandleTextAccepted              ( bool bHandled );
    void                GetCommandInfo                  ( const std::string &strIn, std::string & strCmdOut, std::string & strCmdLineOut );

    void                SetNextHistoryText              ( void );
    void                SetPreviousHistoryText          ( void );
    void                SetNextAutoCompleteMatch        ( void );
    void                ResetAutoCompleteMatch          ( void );

    CVector2D           GetPosition                     ( void );
    void                SetPosition                     ( CVector2D& vecPosition );

    CVector2D           GetSize                         ( void );
    void                SetSize                         ( CVector2D& vecSize );

protected:
    bool                OnCloseButtonClick              ( CGUIElement* pElement );
    bool                Edit_OnTextAccepted             ( CGUIElement* pElement );
    bool                History_OnTextChanged           ( CGUIElement* pElement );

private:
    void                CreateElements                  ( CGUIElement* pParent = NULL );
    void                DestroyElements                 ( void );

    bool                OnWindowSize                    ( CGUIElement* pElement );

    CGUI*               m_pManager;

    CConsoleHistory*    m_pConsoleHistory;
    int                 m_iHistoryIndex;
    std::vector < int > m_AutoCompleteList;
    int                 m_iAutoCompleteIndex;

    CGUIWindow*         m_pWindow;
    CGUIEdit*           m_pInput;
    CGUIMemo*           m_pHistory;

    bool                m_bIsEnabled;

    float               m_fWindowSpacer;
    float               m_fWindowSpacerTop;
    float               m_fWindowX;
    float               m_fWindowY;
    float               m_fInputHeight;

};

#endif
