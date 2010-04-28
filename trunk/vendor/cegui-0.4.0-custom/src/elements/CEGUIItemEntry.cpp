/************************************************************************
	filename: 	CEGUIItemEntry.cpp
	created:	31/3/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Implementation of ItemEntry widget base class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "StdInc.h"
#include "elements/CEGUIItemEntry.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constructor for ItemEntry base class.
*************************************************************************/
ItemEntry::ItemEntry(const String& type, const String& name)
	: Window(type, name)
{
}


/*************************************************************************
	Destructor for ItemEntry base class.
*************************************************************************/
ItemEntry::~ItemEntry(void)
{
}

} // End of  CEGUI namespace section
