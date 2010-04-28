/************************************************************************
	filename: 	CEGUIScrollbarProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface for Scrollbar properties
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
#ifndef _CEGUIScrollbarProperties_h_
#define _CEGUIScrollbarProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ScrollbarProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the Scrollbar class
*/
namespace ScrollbarProperties
{
/*!
\brief
	Property to access the document size for the Scrollbar.

	\par Usage:
		- Name: DocumentSize
		- Format: "[float]".

	\par Where:
		- [float] specifies the size of the document being scrolled (as defined by the client code).
*/
class DocumentSize : public Property
{
public:
	DocumentSize() : Property(
		"DocumentSize",
		"Property to get/set the document size for the Scrollbar.  Value is a float.",
		"1.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the page size for the Scrollbar.

	\par Usage:
		- Name: PageSize
		- Format: "[float]".

	\par Where:
		- [float] specifies the size of the visible page (as defined by the client code).
*/
class PageSize : public Property
{
public:
	PageSize() : Property(
		"PageSize",
		"Property to get/set the page size for the Scrollbar.  Value is a float.",
		"0.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the step size for the Scrollbar.

	\par Usage:
		- Name: StepSize
		- Format: "[float]".

	\par Where:
		- [float] specifies the size of the increase/decrease button step (as defined by the client code).
*/
class StepSize : public Property
{
public:
	StepSize() : Property(
		"StepSize",
		"Property to get/set the step size for the Scrollbar.  Value is a float.",
		"1.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the overlap size for the Scrollbar.

	\par Usage:
		- Name: OverlapSize
		- Format: "[float]".

	\par Where:
		- [float] specifies the size of the per-page overlap (as defined by the client code).
*/
class OverlapSize : public Property
{
public:
	OverlapSize() : Property(
		"OverlapSize",
		"Property to get/set the overlap size for the Scrollbar.  Value is a float.",
		"0.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
	Property to access the scroll position of the Scrollbar.

	\par Usage:
		- Name: ScrollPosition
		- Format: "[float]".

	\par Where:
		- [float] specifies the current scroll position / value of the Scrollbar.
*/
class ScrollPosition : public Property
{
public:
	ScrollPosition() : Property(
		"ScrollPosition",
		"Property to get/set the scroll position of the Scrollbar.  Value is a float.",
		"0.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  ScrollbarProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIScrollbarProperties_h_
