/************************************************************************
    created:    03/23/2004
    author:     Lars 'Levia' Wesselius (Content Pane based on Tomas Lindquist Olsen's code)

    purpose:    Implementation of base class for the groupbox
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/WindowManager.h"
#include "CEGUI/widgets/GroupBox.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const String GroupBox::EventNamespace("GroupBox");
const String GroupBox::WidgetTypeName("CEGUI/GroupBox");
const String GroupBox::ContentPaneName("__auto_contentpane__");

//----------------------------------------------------------------------------//
GroupBox::GroupBox(const String& type, const String& name) :
    Window(type, name)
{
    // When clicked, don't rise. Required because a Groupbox does not have an
    // actual parent child relation with the widgets which appear inside it.
    d_riseOnClick = false;
}

//----------------------------------------------------------------------------//
void GroupBox::addChild_impl(Element* element)
{
    Window* wnd = dynamic_cast<Window*>(element);
    
    if (!wnd)
        CEGUI_THROW(InvalidRequestException(
            "GroupBox can only have Elements of type Window added as children "
            "(Window path: " + getNamePath() + ")."));

    if (wnd->isAutoWindow())
        Window::addChild_impl(wnd);
    else if (Window * pane = getContentPane())
        pane->addChild(wnd);
}

//----------------------------------------------------------------------------//
void GroupBox::removeChild_impl(Element* element)
{
    Window* wnd = static_cast<Window*>(element);

    if (wnd->isAutoWindow())
        Window::removeChild_impl(wnd);
    else if (Window * pane = getContentPane())
        pane->removeChild(wnd);
}

//----------------------------------------------------------------------------//
Window * GroupBox::getContentPane() const
{
    return getChild(ContentPaneName);    
}

//----------------------------------------------------------------------------//
bool GroupBox::drawAroundWidget(const CEGUI::Window*)
{
    Logger::getSingleton().logEvent("TODO: GroupBox::drawAroundWidget");
    return true;
}

//----------------------------------------------------------------------------//
bool GroupBox::drawAroundWidget(const String& name)
{
    return drawAroundWidget(getChild(name));
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
