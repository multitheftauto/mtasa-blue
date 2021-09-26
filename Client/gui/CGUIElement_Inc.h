/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

void SetPosition(CVector2D pos)
{
    CGUIElement_Impl::SetPosition(pos);
};

void SetSize(CVector2D size)
{
    CGUIElement_Impl::SetSize(size);
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

std::string GetType()
{
    return CGUIElement_Impl::GetType();
};

bool IsDeleted()
{
    return CGUIElement_Impl::IsDeleted();
};


void SetFrameEnabled(bool state)
{
    CGUIElement_Impl::SetFrameEnabled(state);
};

bool GetFrameEnabled()
{
    return CGUIElement_Impl::GetFrameEnabled();
};
