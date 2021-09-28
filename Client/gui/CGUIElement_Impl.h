/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIElement.h>

class CGUI_Impl;

class CGUIElement_Impl : public CGUIElement
{
public:
    CGUIElement_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative);
    ~CGUIElement_Impl();

    void Begin();
    void End();

    std::string GetID();

    void SetPosition(CVector2D pos, bool relative = false);
    void SetSize(CVector2D size, bool relative = false);

    CVector2D GetPosition();
    CVector2D GetSize();

    void         SetParent(CGUIElement* parent);
    CGUIElement* GetParent();

    void AddChild(CGUIElement* child);
    void RemoveChild(CGUIElement* child);

    std::list<CGUIElement*> GetChildren();

    CGUIType GetType();

    void ProcessPosition();
    void ProcessSize();

    void SetFrameEnabled(bool state);
    bool GetFrameEnabled();

    void SetDynamicPositionEnabled(bool state);
    bool GetDynamicPositionEnabled();

    int  AddRenderFunction(std::function<void()> renderFunction);
    void RemoveRenderFunction(int index);

    void Destroy();
    bool IsDestroyed();

    void DemoHookTest();

protected:
    CGUI_Impl*   m_pManager = nullptr;
    CGUIElement* m_pParent = nullptr;

    std::list<CGUIElement*> m_children;

    std::list<std::function<void()>>                    m_renderFunctions = {};
    std::map<int, decltype(m_renderFunctions.cbegin())> m_renderFunctionIndexMap;
    int                                                 m_numLifetimeRenderFunctions = 0;

    std::string m_title = {};
    std::string m_uid;

    CGUIType m_type = CGUIType::ELEMENT;

    bool m_hasFrame = false;
    bool m_hasDynamicPosition = false;

    CVector2D m_position = {};
    CVector2D m_size = {};

    bool m_rendering = false;
    bool m_deleted = false;

    bool m_updatePosition = false;
    bool m_updateSize = false;
};
