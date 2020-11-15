/***********************************************************************
	created:	28/8/2004
	author:		Paul D Turner
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
#include "CEGUI/widgets/DefaultWindow.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const String DefaultWindow::WidgetTypeName("DefaultWindow");

/*************************************************************************
    Constructor
*************************************************************************/
DefaultWindow::DefaultWindow(const String& type, const String& name) :
    Window(type, name)
{
    USize sz(cegui_reldim(1.0f), cegui_reldim(1.0f));
    setMaxSize(sz);
    setSize(sz);
}

//----------------------------------------------------------------------------//
void DefaultWindow::onMouseMove(MouseEventArgs& e)
{
    // always call the base class handler
    Window::onMouseMove(e);
    updateMouseEventHandled(e);
}

//----------------------------------------------------------------------------//
void DefaultWindow::onMouseWheel(MouseEventArgs& e)
{
    // always call the base class handler
    Window::onMouseWheel(e);
    updateMouseEventHandled(e);
}

//----------------------------------------------------------------------------//
void DefaultWindow::onMouseButtonDown(MouseEventArgs& e)
{
    // always call the base class handler
    Window::onMouseButtonDown(e);
    updateMouseEventHandled(e);
}

//----------------------------------------------------------------------------//
void DefaultWindow::onMouseButtonUp(MouseEventArgs& e)
{
    // always call the base class handler
    Window::onMouseButtonUp(e);
    updateMouseEventHandled(e);
}

//----------------------------------------------------------------------------//
void DefaultWindow::onMouseClicked(MouseEventArgs& e)
{
    // always call the base class handler
    Window::onMouseClicked(e);
    // only adjust the handled state if event was directly injected
    if (!getGUIContext().isMouseClickEventGenerationEnabled())
        updateMouseEventHandled(e);
}

//----------------------------------------------------------------------------//
void DefaultWindow::onMouseDoubleClicked(MouseEventArgs& e)
{
    // always call the base class handler
    Window::onMouseDoubleClicked(e);
    updateMouseEventHandled(e);
}

//----------------------------------------------------------------------------//
void DefaultWindow::onMouseTripleClicked(MouseEventArgs& e)
{
    // always call the base class handler
    Window::onMouseTripleClicked(e);
    updateMouseEventHandled(e);
}

//----------------------------------------------------------------------------//
void DefaultWindow::updateMouseEventHandled(MouseEventArgs& e) const
{
    // by default, if we are a root window (no parent) with pass-though enabled
    // we do /not/ mark mouse events as handled.
    if (!d_parent && e.handled && d_mousePassThroughEnabled)
        --e.handled;
}

//----------------------------------------------------------------------------//
bool DefaultWindow::moveToFront_impl(bool wasClicked)
{
    const bool took_action = Window::moveToFront_impl(wasClicked);

    if (!d_parent && d_mousePassThroughEnabled)
        return false;
    else
        return took_action;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
