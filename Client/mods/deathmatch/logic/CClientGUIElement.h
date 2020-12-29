/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientGUIElement.h
 *  PURPOSE:     GUI wrapper entity class
 *
 *****************************************************************************/

class CClientGUIElement;

#pragma once

#include "CClientGUIManager.h"
#include "CClientEntity.h"
#include <gui/CGUI.h>

#define MAX_EVENT_NAME 256

#define CGUI_GET_CCLIENTGUIELEMENT(x)   static_cast < CClientGUIElement* > ( x->GetUserData () )
#define CGUI_SET_CCLIENTGUIELEMENT(x,y) x->SetUserData ( reinterpret_cast < void* > ( y ) )

// use just for events, for all gui specific stuff use the funcs in the CGUI* classes

// supports 2 events

#define IS_CGUIELEMENT_BUTTON(entity)           ((entity)->GetCGUIType()==CGUI_BUTTON)
#define IS_CGUIELEMENT_CHECKBOX(entity)         ((entity)->GetCGUIType()==CGUI_CHECKBOX)
#define IS_CGUIELEMENT_EDIT(entity)             ((entity)->GetCGUIType()==CGUI_EDIT)
#define IS_CGUIELEMENT_GRIDLIST(entity)         ((entity)->GetCGUIType()==CGUI_GRIDLIST)
#define IS_CGUIELEMENT_LABEL(entity)            ((entity)->GetCGUIType()==CGUI_LABEL)
#define IS_CGUIELEMENT_MEMO(entity)             ((entity)->GetCGUIType()==CGUI_MEMO)
#define IS_CGUIELEMENT_PROGRESSBAR(entity)      ((entity)->GetCGUIType()==CGUI_PROGRESSBAR)
#define IS_CGUIELEMENT_SCROLLBAR(entity)        ((entity)->GetCGUIType()==CGUI_SCROLLBAR)
#define IS_CGUIELEMENT_SCROLLPANE(entity)       ((entity)->GetCGUIType()==CGUI_SCROLLPANE)
#define IS_CGUIELEMENT_RADIOBUTTON(entity)      ((entity)->GetCGUIType()==CGUI_RADIOBUTTON)
#define IS_CGUIELEMENT_STATICIMAGE(entity)      ((entity)->GetCGUIType()==CGUI_STATICIMAGE)
#define IS_CGUIELEMENT_TAB(entity)              ((entity)->GetCGUIType()==CGUI_TAB)
#define IS_CGUIELEMENT_TABPANEL(entity)         ((entity)->GetCGUIType()==CGUI_TABPANEL)
#define IS_CGUIELEMENT_COMBOBOX(entity)         ((entity)->GetCGUIType()==CGUI_COMBOBOX)
#define IS_CGUIELEMENT_WINDOW(entity)           ((entity)->GetCGUIType()==CGUI_WINDOW)

#define IS_CGUIELEMENT_VALID_PARENT(entity)     (IS_CGUIELEMENT_SCROLLPANE(entity) || IS_CGUIELEMENT_WINDOW(entity) || IS_CGUIELEMENT_TAB(entity) )

class CClientGUIElement : public CClientEntity
{
    DECLARE_CLASS(CClientGUIElement, CClientEntity)
    friend class CClientGUIManager;

public:
    CClientGUIElement(CClientManager* pManager, CLuaMain* pLuaMain, CGUIElement* pCGUIElement, ElementID ID = INVALID_ELEMENT_ID);
    virtual ~CClientGUIElement();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTGUI; };
    eCGUIType         GetCGUIType() const { return m_pCGUIElement->GetType(); };
    const char*       GetCGUITypeName() const { return m_strCGUITypeName; };

    GUI_CALLBACK GetCallback1() { return GUI_CALLBACK(&CClientGUIElement::_CallbackEvent1, this); };
    GUI_CALLBACK GetCallback2() { return GUI_CALLBACK(&CClientGUIElement::_CallbackEvent2, this); };

    void SetEvents(const char* szFunc1, const char* szFunc2 = NULL);

    CLuaMain* GetVirtualMachine() { return m_pLuaMain; }

    // dummy overrides
    void SetPosition(const CVector& vecDummy){};
    void GetPosition(CVector& vecDummy) const {};

    // cgui element access
    CGUIElement* GetCGUIElement() { return m_pCGUIElement; };
    void         SetCGUIElement(CGUIElement* pCGUIElement) { m_pCGUIElement = pCGUIElement; };

    SString GetFont(class CClientGuiFont** ppFontElement);
    bool    SetFont(const SString& strFontName, CClientGuiFont* pFontElement);

    virtual void SetCallPropagationEnabled(bool bEnabled);

private:
    bool _CallbackEvent1(CGUIElement* pCGUIElement);
    bool _CallbackEvent2(CGUIElement* pCGUIElement);

    SString _strCallbackFunc1;
    SString _strCallbackFunc2;

    SString            m_strCGUITypeName;
    CClientGUIManager* m_pGUIManager;
    CGUIElement*       m_pCGUIElement;
    CClientGuiFont*    m_pFontElement;
    CLuaMain*          m_pLuaMain;
};
