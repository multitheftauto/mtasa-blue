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
#include "CEGUI/LinkedEvent.h"
#include "CEGUI/EventArgs.h"
#include "CEGUI/EventSet.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
LinkedEvent::LinkedEvent(const String& event_name, EventSet* target_event_set) :
    Event(event_name),
    d_owner(target_event_set)
{
    if (target_event_set)
        target_event_set->addEvent(*this);
}

//----------------------------------------------------------------------------//
LinkedEvent::~LinkedEvent()
{
    LinkedConnections::iterator i = d_connections.begin();
    for ( ; i != d_connections.end(); ++i)
        (*i)->disconnect();
}

//----------------------------------------------------------------------------//
void LinkedEvent::addLinkedTarget(Event& link_target)
{
    Connection con(link_target.subscribe(
        Subscriber(&LinkedEvent::handler, this)));

    d_connections.push_back(con);
}

//----------------------------------------------------------------------------//
bool LinkedEvent::handler(const EventArgs& args)
{
    LinkedEventArgs our_args(args, d_owner);

    (*this)(our_args);

    return our_args.handled != 0;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

