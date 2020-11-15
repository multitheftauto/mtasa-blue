/***********************************************************************
	created:	16/1/2005
	author:		Paul D Turner
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
#include "CEGUI/GlobalEventSet.h"
#include "CEGUI/Logger.h"


// Start of CEGUI namespace section
namespace CEGUI
{
	/*************************************************************************
		Static Data Definitions
	*************************************************************************/
	// singleton instance pointer
	template<> GlobalEventSet* Singleton<GlobalEventSet>::ms_Singleton	= 0;

	/*************************************************************************
		GlobalEventSet constructor.
	*************************************************************************/
	GlobalEventSet::GlobalEventSet()
	{
        char addr_buff[32];
        sprintf(addr_buff, "(%p)", static_cast<void*>(this));
		Logger::getSingleton().logEvent(
          "CEGUI::GlobalEventSet singleton created. " + String(addr_buff));
	}

	/*************************************************************************
		GlobalEventSet destructor.
	*************************************************************************/
	GlobalEventSet::~GlobalEventSet()
	{
        char addr_buff[32];
        sprintf(addr_buff, "(%p)", static_cast<void*>(this));
		Logger::getSingleton().logEvent(
          "CEGUI::GlobalEventSet singleton destroyed. " + String(addr_buff));
	}

	/*************************************************************************
		Return singleton object
	*************************************************************************/
	GlobalEventSet&	GlobalEventSet::getSingleton(void)
	{
		return Singleton<GlobalEventSet>::getSingleton();
	}

	/*************************************************************************
		Return singleton pointer
	*************************************************************************/
	GlobalEventSet*	GlobalEventSet::getSingletonPtr(void)
	{
		return Singleton<GlobalEventSet>::getSingletonPtr();
	}

	/*************************************************************************
		Overridden fireEvent which always succeeds.
	*************************************************************************/
	void GlobalEventSet::fireEvent(const String& name, EventArgs& args, const String& eventNamespace)
	{
        // here we are very explicit about how we construct the event string.
        // Doing it 'longhand' like this saves significant time when compared
        // to the obvious - and previous - implementation:
        //     fireEvent_impl(eventNamespace + "/" + name, args);
        String evt_name;
        evt_name.reserve(eventNamespace.length() + name.length() + 1);
        evt_name.append(eventNamespace);
        evt_name.append(1, '/');
        evt_name.append(name);
        fireEvent_impl(evt_name, args);
	}

} // End of  CEGUI namespace section
