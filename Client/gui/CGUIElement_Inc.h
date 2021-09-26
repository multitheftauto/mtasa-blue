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

CVector2D GetInitialPosition() const
{
    return CGUIElement_Impl::GetInitialPosition();
};

CVector2D GetInitialSize() const
{
    return CGUIElement_Impl::GetInitialSize();
};

void ProcessPosition()
{
    return CGUIElement_Impl::ProcessPosition();
};

void ProcessSize()
{
    return CGUIElement_Impl::ProcessSize();
};

bool IsDeleted()
{
    return CGUIElement_Impl::IsDeleted();
};
