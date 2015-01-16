/************************************************************************
	filename: 	CEGUIScrolledContainerProperties.h
	created:	3/3/2005
	author:		Paul D Turner
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
#ifndef _CEGUIScrolledContainerProperties_h_
#define _CEGUIScrolledContainerProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of ScrolledContainerProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the ScrolledContainer class
*/
namespace ScrolledContainerProperties
{
    /*!
    \brief
        Property to access the setting which controls whether the content pane is automatically
        resized according to the size and position of attached content.

    \par Usage:
        - Name: ContentPaneAutoSized
        - Format: "[text]"

    \par Where [Text] is:
        - "True" to indicate the pane should automatically resize itself.
        - "False" to indicate the pane should not automatically resize itself.
    */
    class ContentPaneAutoSized : public Property
    {
    public:
        ContentPaneAutoSized() : Property(
            "ContentPaneAutoSized",
            "Property to get/set the setting which controls whether the content pane will auto-size itself.  Value is either \"True\" or \"False\".",
            "True")
        {}

        String	get(const PropertyReceiver* receiver) const;
        void	set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
       Property to access the current content pane area rectangle (as window relative pixels).

    \par Usage:
        - Name: ContentArea
        - Format: "l:[float] t:[float] r:[float] b:[float]".

    \par Where:
        - l:[float]	specifies the position of the left edge of the area as a floating point number.
        - t:[float]	specifies the position of the top edge of the area as a floating point number.
        - r:[float]	specifies the position of the right edge of the area as a floating point number.
        - b:[float]	specifies the position of the bottom edge of the area as a floating point number.
    */
    class ContentArea : public Property
    {
    public:
        ContentArea() : Property(
            "ContentArea",
            "Property to get/set the current content area rectangle of the content pane.  Value is \"l:[float] t:[float] r:[float] b:[float]\" (where l is left, t is top, r is right, and b is bottom).",
            "l:0.000000 t:0.000000 r:0.000000 b:0.000000")
        {}

        String	get(const PropertyReceiver* receiver) const;
        void	set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
        Property offering read-only access to the current content extents rectangle (as window relative pixels).

    \par Usage:
        - Name: ChildExtentsArea
        - Format: "l:[float] t:[float] r:[float] b:[float]".

    \par Where:
        - l:[float]	specifies the position of the left edge of the area as a floating point number.
        - t:[float]	specifies the position of the top edge of the area as a floating point number.
        - r:[float]	specifies the position of the right edge of the area as a floating point number.
        - b:[float]	specifies the position of the bottom edge of the area as a floating point number.
    */
    class ChildExtentsArea : public Property
    {
    public:
        ChildExtentsArea() : Property(
            "ChildExtentsArea",
            "Property to get the current content extents rectangle.  Value is \"l:[float] t:[float] r:[float] b:[float]\" (where l is left, t is top, r is right, and b is bottom).",
            "l:0.000000 t:0.000000 r:0.000000 b:0.000000")
        {}

        String	get(const PropertyReceiver* receiver) const;
        void	set(PropertyReceiver* receiver, const String& value);
    };

} // End of  ScrolledContainerProperties namespace section
} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIScrolledContainerProperties_h_
