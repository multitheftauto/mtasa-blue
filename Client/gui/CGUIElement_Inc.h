/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

std::string GetID()
{
    return CGUIElement_Impl::GetID();
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

std::list<CGUIElement*> GetChildren()
{
    return CGUIElement_Impl::GetChildren();
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

int AddRenderFunction(std::function<void()> renderFunction)
{
    return CGUIElement_Impl::AddRenderFunction(renderFunction);
};

void RemoveRenderFunction(int index)
{
    CGUIElement_Impl::RemoveRenderFunction(index);
};

void DemoHookTest()
{
    CGUIElement_Impl::DemoHookTest();
};

