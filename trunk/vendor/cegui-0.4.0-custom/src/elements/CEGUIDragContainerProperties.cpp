/************************************************************************
	filename: 	CEGUIDragContainerProperties.cpp
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
#include "StdInc.h"
#include "elements/CEGUIDragContainerProperties.h"
#include "elements/CEGUIDragContainer.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of DragContainerProperties namespace section
namespace DragContainerProperties
{
    String DraggingEnabled::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::boolToString(static_cast<const DragContainer*>(receiver)->isDraggingEnabled());
    }

    void DraggingEnabled::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<DragContainer*>(receiver)->setDraggingEnabled(PropertyHelper::stringToBool(value));
    }

    //////////////////////////////////////////////////////////////////////////
    
    String DragAlpha::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const DragContainer*>(receiver)->getDragAlpha());
    }

    void DragAlpha::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<DragContainer*>(receiver)->setDragAlpha(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String DragThreshold::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const DragContainer*>(receiver)->getPixelDragThreshold());
    }

    void DragThreshold::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<DragContainer*>(receiver)->setPixelDragThreshold(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String DragCursorImage::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const DragContainer*>(receiver)->getDragCursorImage();
        return img ? PropertyHelper::imageToString(img) : String();
    }

    void DragCursorImage::set(PropertyReceiver* receiver, const String& value)
    {
        if (!value.empty())
        {
            static_cast<DragContainer*>(receiver)->setDragCursorImage(PropertyHelper::stringToImage(value));
        }
    }

} // End of  DragContainerProperties namespace section


} // End of  CEGUI namespace section
