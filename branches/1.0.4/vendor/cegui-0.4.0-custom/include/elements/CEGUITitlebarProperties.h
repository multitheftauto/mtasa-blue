/************************************************************************
	filename: 	CEGUITitlebarProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface for title bar property classes
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
#ifndef _CEGUITitlebarProperties_h_
#define _CEGUITitlebarProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of TitlebarProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the Titlebar class
*/
namespace TitlebarProperties
{
/*!
\brief
	Property to access the state of the dragging enabled setting for the Titlebar.

	\par Usage:
		- Name: DraggingEnabled
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate that drag moving is enabled.
		- "False" to indicate that drag moving is disabled.
*/
class DraggingEnabled : public Property
{
public:
	DraggingEnabled() : Property(
		"DraggingEnabled",
		"Property to get/set the state of the dragging enabled setting for the Titlebar.  Value is either \"True\" or \"False\".",
		"True")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to colour used for rendering the caption text.

	\par Usage:
		- Name: CaptionColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class CaptionColour : public Property
{
public:
	CaptionColour() : Property(
		"CaptionColour",
		"Property to get/set the colour used for rendering the caption text.  Value is \"aarrggbb\" (hex).",
		"FF000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  TitlebarProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUITitlebarProperties_h_
