/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

std::string GetID(bool imgui = false)
{
    return CGUIElement_Impl::GetID(imgui);
};

void SetText(std::string text)
{
    CGUIElement_Impl::SetText(text);
};

std::string GetText()
{
    return CGUIElement_Impl::GetText();
};

void SetPosition(CVector2D pos, bool relative = false)
{
    CGUIElement_Impl::SetPosition(pos, relative);
};

void SetSize(CVector2D size, bool relative = false)
{
    CGUIElement_Impl::SetSize(size, relative);
};

CVector2D GetPosition()
{
    return CGUIElement_Impl::GetPosition();
};

CVector2D GetSize()
{
    return CGUIElement_Impl::GetSize();
};

void ProcessPosition()
{
    return CGUIElement_Impl::ProcessPosition();
};

void ProcessSize()
{
    return CGUIElement_Impl::ProcessSize();
};

void AddChild(CGUIElement* child)
{
    CGUIElement_Impl::AddChild(child);
};

void RemoveChild(CGUIElement* child)
{
    CGUIElement_Impl::RemoveChild(child);
};

std::vector<CGUIElement*> GetChildren()
{
    return CGUIElement_Impl::GetChildren();
};

void SetIndex(int index)
{
    CGUIElement_Impl::SetIndex(index);
};

void SetChildIndex(CGUIElement* child, int index)
{
    CGUIElement_Impl::SetChildIndex(child, index);
};

int GetIndex()
{
    return CGUIElement_Impl::GetIndex();
};

int GetChildIndex(CGUIElement* child)
{
    return CGUIElement_Impl::GetChildIndex(child);
};

CGUIElement* GetParent()
{
    return CGUIElement_Impl::GetParent();
};

void SetParent(CGUIElement* parent)
{
    CGUIElement_Impl::SetParent(parent);
};

CGUIType GetType()
{
    return CGUIElement_Impl::GetType();
};

void BringToFront()
{
    CGUIElement_Impl::BringToFront();
};

void MoveToBack()
{
    CGUIElement_Impl::MoveToBack();
};

void SetFlag(ImGuiWindowFlags flag, bool state)
{
    CGUIElement_Impl::SetFlag(flag, state);
};

void ResetFlags()
{
    CGUIElement_Impl::ResetFlags();
};

std::set<ImGuiWindowFlags> GetFlags()
{
    return CGUIElement_Impl::GetFlags();
};

int GetFlagBits()
{
    return CGUIElement_Impl::GetFlagBits();
};

bool IsDestroyed()
{
    return CGUIElement_Impl::IsDestroyed();
};

void Destroy()
{
    CGUIElement_Impl::Destroy();
}

void SetFrameEnabled(bool state)
{
    CGUIElement_Impl::SetFrameEnabled(state);
};

bool GetFrameEnabled()
{
    return CGUIElement_Impl::GetFrameEnabled();
};

void SetDynamicPositionEnabled(bool state)
{
    CGUIElement_Impl::SetDynamicPositionEnabled(state);
};

bool GetDynamicPositionEnabled()
{
    return CGUIElement_Impl::GetDynamicPositionEnabled();
};

int AddRenderFunction(std::function<void()> renderFunction, bool preRender = false)
{
    return CGUIElement_Impl::AddRenderFunction(renderFunction, preRender);
};

bool RemoveRenderFunction(int index, bool preRender = false)
{
    return CGUIElement_Impl::RemoveRenderFunction(index, preRender);
};

std::list<std::function<void()>>& GetRenderFunctions(bool preRender = false)
{
    return CGUIElement_Impl::GetRenderFunctions(preRender);
};

void SetRenderingEnabled(bool state)
{
    CGUIElement_Impl::SetRenderingEnabled(state);
};

bool IsRenderingEnabled() const
{
    return CGUIElement_Impl::IsRenderingEnabled();
};

void SetVisible(bool state)
{
    CGUIElement_Impl::SetVisible(state);
};

bool IsVisible()
{
    return CGUIElement_Impl::IsVisible();
};

float GetAlpha(bool clamp = false)
{
    return CGUIElement_Impl::GetAlpha(clamp);
}

void SetAlpha(float alpha)
{
    CGUIElement_Impl::SetAlpha(alpha);
}

CVector2D GetTextSize()
{
    return CGUIElement_Impl::GetTextSize();
}

float GetTextExtent()
{
    return CGUIElement_Impl::GetTextExtent();
}

CColor GetTextColor()
{
    return CGUIElement_Impl::GetTextColor();
}

void SetTextColor(CColor color)
{
    CGUIElement_Impl::SetTextColor(color);
}

void SetTextColor(int r, int g, int b, int a = 255)
{
    CGUIElement_Impl::SetTextColor(r, g, b, a);
}

CGUITextAlignHorizontal GetTextHorizontalAlign()
{
    return CGUIElement_Impl::GetTextHorizontalAlign();
}

void SetTextHorizontalAlign(CGUITextAlignHorizontal align)
{
    CGUIElement_Impl::SetTextHorizontalAlign(align);
}

CGUITextAlignVertical GetTextVerticalAlign()
{
    return CGUIElement_Impl::GetTextVerticalAlign();
}

void SetTextVerticalAlign(CGUITextAlignVertical align)
{
    CGUIElement_Impl::SetTextVerticalAlign(align);
}

void SetAutoSizingEnabled(bool state)
{
    CGUIElement_Impl::SetAutoSizingEnabled(state);
}

bool IsAutoSizingEnabled()
{
    return CGUIElement_Impl::IsAutoSizingEnabled();
}
