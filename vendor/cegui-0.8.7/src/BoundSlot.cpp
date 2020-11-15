/************************************************************************
    created:    Tue Feb 28 2006
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/BoundSlot.h"
#include "CEGUI/Event.h"

// Start of CEGUI namespace section
namespace CEGUI
{
BoundSlot::BoundSlot(Group group, const SubscriberSlot& subscriber, Event& event) :
    d_group(group),
    d_subscriber(CEGUI_NEW_AO SubscriberSlot(subscriber)),
    d_event(&event)
{}


BoundSlot::BoundSlot(const BoundSlot& other) :
    d_group(other.d_group),
    d_subscriber(other.d_subscriber),
    d_event(other.d_event)
{}


BoundSlot::~BoundSlot()
{
    disconnect();
    CEGUI_DELETE_AO d_subscriber;
}


BoundSlot& BoundSlot::operator=(const BoundSlot& other)
{
    d_group      = other.d_group;
    d_subscriber = other.d_subscriber;
    d_event      = other.d_event;

    return *this;
}


bool BoundSlot::operator==(const BoundSlot& other) const
{
    return d_subscriber == other.d_subscriber;
}


bool BoundSlot::operator!=(const BoundSlot& other) const
{
    return !(*this == other);
}


bool BoundSlot::connected() const
{
    return (d_subscriber != 0) && d_subscriber->connected();
}


void BoundSlot::disconnect()
{
    // cleanup the bound subscriber functor
    if (connected())
        d_subscriber->cleanup();

    // remove the owning Event's reference to us
    if (d_event)
    {
        // get the event to erase the subscriber functor.
        d_event->unsubscribe(*this);
        d_event = 0;
    }

}

} // End of  CEGUI namespace section

