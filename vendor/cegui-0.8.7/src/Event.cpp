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
#include "CEGUI/Event.h"
#include "CEGUI/EventArgs.h"

#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
/*!
\brief
    Implementation helper functor which is used to locate a BoundSlot in the
    multimap collection of BoundSlots.
*/
class SubComp
{
public:
    SubComp(const BoundSlot& s) :
        d_s(s)
    {}

    bool operator()(std::pair<Event::Group, Event::Connection> e) const
    {
        return *(e.second) == d_s;
    }

private:
    void operator=(const SubComp&) {}
    const BoundSlot& d_s;
};

//----------------------------------------------------------------------------//
Event::Event(const String& name) :
    d_name(name)
{
}

//----------------------------------------------------------------------------//
Event::~Event()
{
    SlotContainer::iterator iter(d_slots.begin());
    const SlotContainer::const_iterator end_iter(d_slots.end());

    for (; iter != end_iter; ++iter)
    {
        iter->second->d_event = 0;
        iter->second->d_subscriber->cleanup();
    }

    d_slots.clear();
}

//----------------------------------------------------------------------------//
Event::Connection Event::subscribe(const Event::Subscriber& slot)
{
    return subscribe(static_cast<Group>(-1), slot);
}

//----------------------------------------------------------------------------//
Event::Connection Event::subscribe(Event::Group group,
                                   const Event::Subscriber& slot)
{
    Event::Connection c(CEGUI_NEW_AO BoundSlot(group, slot, *this));
    d_slots.insert(std::pair<Group, Connection>(group, c));
    return c;
}

//----------------------------------------------------------------------------//
void Event::operator()(EventArgs& args)
{
    SlotContainer::iterator iter(d_slots.begin());
    const SlotContainer::const_iterator end_iter(d_slots.end());

    // execute all subscribers, updating the 'handled' state as we go
    for (; iter != end_iter; ++iter)
        if ((*iter->second->d_subscriber)(args))
            ++args.handled;
}

//----------------------------------------------------------------------------//
void Event::unsubscribe(const BoundSlot& slot)
{
    // try to find the slot in our collection
    SlotContainer::iterator curr =
        std::find_if(d_slots.begin(),
                     d_slots.end(),
                     SubComp(slot));

    // erase our reference to the slot, if we had one.
    if (curr != d_slots.end())
        d_slots.erase(curr);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

