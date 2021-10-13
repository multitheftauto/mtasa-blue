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

    std::string GetID(bool imgui = false);

    void SetPosition(CVector2D pos, bool relative = false);
    void SetSize(CVector2D size, bool relative = false);

    CVector2D GetPosition();
    CVector2D GetSize();

    void        SetText(std::string text);
    std::string GetText();

    void         SetParent(CGUIElement* parent);
    CGUIElement* GetParent();

    void AddChild(CGUIElement* child);
    void RemoveChild(CGUIElement* child);

    std::vector<CGUIElement*> GetChildren();

    CGUIType GetType();

    void SetFlag(ImGuiWindowFlags flag, bool state);
    void ResetFlags();

    std::set<ImGuiWindowFlags> GetFlags();
    int                        GetFlagBits();

    void ProcessPosition();
    void ProcessSize();

    void SetFrameEnabled(bool state);
    bool GetFrameEnabled();

    void SetDynamicPositionEnabled(bool state);
    bool GetDynamicPositionEnabled();

    int  AddRenderFunction(std::function<void()> renderFunction, bool preRender = false);
    bool RemoveRenderFunction(int index, bool preRender = false);

    void SetIndex(int index);
    void SetChildIndex(CGUIElement* child, int index);

    int GetIndex();
    int GetChildIndex(CGUIElement* child);

    void BringToFront();
    void MoveToBack();

    void Destroy();
    bool IsDestroyed();

    void SetRenderingEnabled(bool state);
    bool IsRenderingEnabled() const;

    void SetVisible(bool state);
    bool IsVisible();

    void SetAlpha(float alpha);
    float  GetAlpha(bool clamp = false);

    CVector2D GetTextSize();
    float     GetTextExtent();

    CColor GetTextColor();
    void   SetTextColor(CColor color);
    void   SetTextColor(int r, int g, int b, int a = 255);

    CGUITextAlignHorizontal GetTextHorizontalAlign();
    void                    SetTextHorizontalAlign(CGUITextAlignHorizontal align);

    CGUITextAlignVertical GetTextVerticalAlign();
    void                  SetTextVerticalAlign(CGUITextAlignVertical align);

    void SetAutoSizingEnabled(bool state);
    bool IsAutoSizingEnabled();

    std::list<std::function<void()>>& GetRenderFunctions(bool preRender = false);

protected:
    CGUI_Impl*   m_pManager = nullptr;
    CGUIElement* m_pParent = nullptr;

    std::vector<CGUIElement*> m_children;

    std::list<std::function<void()>>                    m_renderFunctions = {};
    std::map<int, decltype(m_renderFunctions.cbegin())> m_renderFunctionIndexMap;
    int                                                 m_numLifetimeRenderFunctions = 0;

    std::list<std::function<void()>>                       m_preRenderFunctions = {};
    std::map<int, decltype(m_preRenderFunctions.cbegin())> m_preRenderFunctionIndexMap;
    int                                                    m_numLifetimePreRenderFunctions = 0;

    std::string m_text = {};
    std::string m_uid;

    CGUIType m_type = CGUIType::ELEMENT;

    std::set<ImGuiWindowFlags> m_flags = {};

    float     m_borderSize = 0.0f;
    CVector2D m_padding = {};

    bool m_hasFrame = false;
    bool m_hasDynamicPosition = false;

    CVector2D m_position = {};
    CVector2D m_size = {};

    float m_alpha = 255.f;

    bool m_render = true;
    bool m_visible = true;

    bool m_rendering = false;
    bool m_deleted = false;

    bool m_updatePosition = false;
    bool m_updateSize = false;

    bool m_bringToFront = false;

    CColor m_textColor = {255, 255, 255, 255};

    std::pair<CGUITextAlignHorizontal, CGUITextAlignVertical> m_textAlign = {CGUITextAlignHorizontal::LEFT, CGUITextAlignVertical::TOP};

    bool m_autoSize = false;
};
