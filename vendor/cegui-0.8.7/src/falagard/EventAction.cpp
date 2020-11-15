/***********************************************************************
    created:    Fri Mar 02 2012
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "CEGUI/falagard/EventAction.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Window.h"
#include "CEGUI/Logger.h"

namespace CEGUI
{
//----------------------------------------------------------------------------//
// Functor used to subscribe to events - this is where the magic happens!
struct EventActionFunctor
{
    EventActionFunctor(Window& window, ChildEventAction action) :
        window(window),
        action(action)
    {}

    bool operator()(const EventArgs& /*args*/) const
    {
        switch (action)
        {
        case CEA_REDRAW:
            window.invalidate(false);
            return true;

        case CEA_LAYOUT:
            window.performChildWindowLayout();
            return true;

        default:
            CEGUI_THROW(InvalidRequestException("invalid action."));
        }

        return false;
    }

    Window& window;
    ChildEventAction action;
};

//----------------------------------------------------------------------------//
EventAction::EventAction(const String& event_name,
                         ChildEventAction action) :
    d_eventName(event_name),
    d_action(action)
{
}

//----------------------------------------------------------------------------//
EventAction::~EventAction()
{
}

//----------------------------------------------------------------------------//
void EventAction::setEventName(const String& event_name)
{
    d_eventName = event_name;
}

//----------------------------------------------------------------------------//
const String& EventAction::getEventName() const
{
    return d_eventName;
}

//----------------------------------------------------------------------------//
void EventAction::setAction(ChildEventAction action)
{
    d_action = action;
}

//----------------------------------------------------------------------------//
ChildEventAction EventAction::getAction() const
{
    return d_action;
}

//----------------------------------------------------------------------------//
void EventAction::initialiseWidget(Window& widget) const
{
    Window* parent = widget.getParent();

    if (!parent)
        CEGUI_THROW(InvalidRequestException(
            "EvenAction can only be initialised on child widgets."));

    d_connections.insert(
        std::make_pair(makeConnectionKeyName(widget),
                       widget.subscribeEvent(d_eventName,
                            EventActionFunctor(*parent, d_action))));
}

//----------------------------------------------------------------------------//
void EventAction::cleanupWidget(Window& widget) const
{
    const String keyname(makeConnectionKeyName(widget));

    ConnectionMap::iterator i = d_connections.find(keyname);

    if (i != d_connections.end())
        d_connections.erase(i);
    else
        Logger::getSingleton().logEvent("EventAction::cleanupWidget: "
            "An event connection with key '" + keyname + "' was not "
            "found.  This may be harmless, but most likely could point "
            "to a double-deletion or some other serious issue.", Errors);
}

//----------------------------------------------------------------------------//
void EventAction::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::EventActionElement)
        .attribute(Falagard_xmlHandler::EventAttribute, d_eventName)
        .attribute(Falagard_xmlHandler::ActionAttribute, FalagardXMLHelper<ChildEventAction>::toString(d_action))
        .closeTag();
}

//----------------------------------------------------------------------------//
String EventAction::makeConnectionKeyName(const Window& widget) const
{
    char addr[32];
    std::sprintf(addr, "%p", static_cast<const void *>(&widget));

    return String(addr) +
           d_eventName +
           FalagardXMLHelper<ChildEventAction>::toString(d_action);
}

//----------------------------------------------------------------------------//

}

