/***********************************************************************
    created:    Sun Nov 21 2010
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2010 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/falagard/EventLinkDefinition.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Window.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/LinkedEvent.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
// String holding parent link identifier
// This is deprecated and declared as an extern in PropertyLinkDefinition.h
static const String S_parentIdentifier("__parent__");


//----------------------------------------------------------------------------//
EventLinkDefinition::EventLinkDefinition(const String& event_name) :
    d_eventName(event_name)
{
}

//----------------------------------------------------------------------------//
EventLinkDefinition::~EventLinkDefinition()
{
}

//----------------------------------------------------------------------------//
void EventLinkDefinition::addLinkTarget(const String& widget,
                                        const String& event)
{
    d_targets.push_back(std::make_pair(widget, event));
}

//----------------------------------------------------------------------------//
void EventLinkDefinition::clearLinkTargets()
{
    d_targets.clear();
}

//----------------------------------------------------------------------------//
void EventLinkDefinition::initialiseWidget(Window& window) const
{
    LinkedEvent* e = CEGUI_NEW_AO LinkedEvent(d_eventName, &window);

    LinkTargetCollection::const_iterator i = d_targets.begin();
    for ( ; i != d_targets.end(); ++i)
    {
        Window* target = getTargetWindow(window, i->first);

        if (target)
            e->addLinkedTarget(*target->getEventObject(i->second, true));
    }
}

//----------------------------------------------------------------------------//
void EventLinkDefinition::cleanUpWidget(Window& window) const
{
    window.removeEvent(d_eventName);
}

//----------------------------------------------------------------------------//
void EventLinkDefinition::setName(const String& name)
{
    d_eventName = name;
}

//----------------------------------------------------------------------------//
const String& EventLinkDefinition::getName() const
{
    return d_eventName;
}

//----------------------------------------------------------------------------//
Window* EventLinkDefinition::getTargetWindow(Window& start_wnd,
                                             const String& name) const
{
    if (name.empty())
        return &start_wnd;

    if (name == Falagard_xmlHandler::ParentIdentifier)
        return start_wnd.getParent();

    return start_wnd.getChild(name);
}

//----------------------------------------------------------------------------//
EventLinkDefinition::LinkTargetIterator EventLinkDefinition::getLinkTargetIterator() const
{
    return LinkTargetIterator(d_targets.begin(),d_targets.end());
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

