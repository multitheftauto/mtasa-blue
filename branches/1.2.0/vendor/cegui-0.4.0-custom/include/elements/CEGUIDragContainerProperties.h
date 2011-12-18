/************************************************************************
	filename: 	CEGUIDragContainerProperties.h
	created:	15/2/2005
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
#ifndef _CEGUIDragContainerProperties_h_
#define _CEGUIDragContainerProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of DragContainerProperties namespace section
namespace DragContainerProperties
{
    /*!
    \brief
	    Property to access the state of the dragging enabled setting.

	    \par Usage:
		    - Name: DraggingEnabled
		    - Format: "[text]".

	    \par Where [Text] is:
		    - "True" to indicate that dragging is enabled.
		    - "False" to indicate that dragging is disabled.
    */
    class DraggingEnabled : public Property
    {
    public:
	    DraggingEnabled() : Property(
		    "DraggingEnabled",
		    "Property to get/set the state of the dragging enabled setting for the DragContainer.  Value is either \"True\" or \"False\".",
		    "True")
	    {}

	    String	get(const PropertyReceiver* receiver) const;
	    void	set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
	    Property to access the dragging alpha value.

	    \par Usage:
		    - Name: DragAlpha
		    - Format: "[float]".

	    \par Where:
		    - [float] represents the alpha value to set when dragging.
    */
    class DragAlpha : public Property
    {
    public:
	    DragAlpha() : Property(
		    "DragAlpha",
		    "Property to get/set the dragging alpha value.  Value is a float.",
		    "0.500000")
	    {}

	    String get(const PropertyReceiver* receiver) const;
	    void set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
	    Property to access the dragging threshold value.

	    \par Usage:
		    - Name: DragThreshold
		    - Format: "[float]".

	    \par Where:
		    - [float] represents the movement threshold (in pixels) which must be exceeded to commence dragging.
    */
    class DragThreshold : public Property
    {
    public:
	    DragThreshold() : Property(
		    "DragThreshold",
		    "Property to get/set the dragging threshold value.  Value is a float.",
		    "8.000000")
	    {}

	    String get(const PropertyReceiver* receiver) const;
	    void set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
	    Property to access the dragging mouse cursor setting.

	    This property offers access to the mouse cursor image used when dragging the DragContainer.

	    \par Usage:
		    - Name: DragCursorImage
		    - Format: "set:[text] image:[text]".

	    \par Where:
		    - set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		    - image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
    */
    class DragCursorImage : public Property
    {
    public:
	    DragCursorImage() : Property(
		    "DragCursorImage",
		    "Property to get/set the mouse cursor image used when dragging.  Value should be \"set:<imageset name> image:<image name>\".",
		    "")
	    {}

	    String	get(const PropertyReceiver* receiver) const;
	    void	set(PropertyReceiver* receiver, const String& value);
    };

} // End of  DragContainerProperties namespace section
} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIDragContainerProperties_h_
