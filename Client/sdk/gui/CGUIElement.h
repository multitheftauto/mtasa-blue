/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <set>
#include <../Shared/sdk/CVector2D.h>
#include "CGUITypes.h"

class CColor;

typedef int ImGuiWindowFlags;

class CGUIElement
{
public:
    virtual ~CGUIElement(){};

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual std::string GetID(bool imgui = false) = 0;

    virtual void        SetText(std::string text) = 0;
    virtual std::string GetText() = 0;

    virtual void SetPosition(CVector2D pos, bool relative = false) = 0;
    virtual void SetSize(CVector2D size, bool relative = false) = 0;

    virtual CVector2D GetPosition() = 0;
    virtual CVector2D GetSize() = 0;

    virtual void         SetParent(CGUIElement* element) = 0;
    virtual CGUIElement* GetParent() = 0;

    virtual void AddChild(CGUIElement* child) = 0;
    virtual void RemoveChild(CGUIElement* child) = 0;

    virtual std::vector<CGUIElement*> GetChildren() = 0;

    virtual void SetIndex(int index) = 0;
    virtual void SetChildIndex(CGUIElement* element, int index) = 0;

    virtual int GetIndex() = 0;
    virtual int GetChildIndex(CGUIElement* child) = 0;

    virtual CGUIType GetType() = 0;

    virtual void BringToFront() = 0;
    virtual void MoveToBack() = 0;

    virtual void SetFlag(ImGuiWindowFlags flag, bool state) = 0;
    virtual void ResetFlags() = 0;

    virtual std::set<ImGuiWindowFlags> GetFlags() = 0;
    virtual int                        GetFlagBits() = 0;

    virtual void ProcessPosition() = 0;
    virtual void ProcessSize() = 0;

    virtual void SetFrameEnabled(bool state) = 0;
    virtual bool GetFrameEnabled() = 0;

    virtual void SetDynamicPositionEnabled(bool state) = 0;
    virtual bool GetDynamicPositionEnabled() = 0;

    virtual int  AddRenderFunction(std::function<void()> renderFunction, bool preRender = false) = 0;
    virtual bool RemoveRenderFunction(int index, bool preRender = false) = 0;
    virtual std::list<std::function<void()>>& GetRenderFunctions(bool preRender = false) = 0;

    virtual void SetRenderingEnabled(bool state) = 0;
    virtual bool IsRenderingEnabled() const = 0;

    virtual void Destroy() = 0;
    virtual bool IsDestroyed() = 0;

    virtual void SetVisible(bool state) = 0;
    virtual bool IsVisible() = 0;

    virtual void SetAlpha(float alpha) = 0;
    virtual float  GetAlpha() = 0;

    virtual CVector2D GetTextSize() = 0;
    virtual float     GetTextExtent() = 0;

    virtual CColor GetTextColor() = 0;
    virtual void   SetTextColor(CColor color) = 0;
    virtual void   SetTextColor(int r, int g, int b, int a = 255) = 0;

    virtual CGUITextAlignHorizontal GetTextHorizontalAlign() = 0;
    virtual void                    SetTextHorizontalAlign(CGUITextAlignHorizontal align) = 0;

    virtual CGUITextAlignVertical GetTextVerticalAlign() = 0;
    virtual void                  SetTextVerticalAlign(CGUITextAlignVertical align) = 0;

    virtual void SetAutoSizingEnabled(bool state) = 0;
    virtual bool IsAutoSizingEnabled() = 0;
};
