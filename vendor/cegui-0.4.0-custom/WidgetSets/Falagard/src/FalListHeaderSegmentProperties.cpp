/************************************************************************
    filename:   FalListHeaderSegmentProperties.cpp
    created:    Tue Jul 5 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "FalListHeaderSegmentProperties.h"
#include "FalListHeaderSegment.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
namespace FalagardListHeaderSegmentProperties
{
String SizingCursorImage::get(const PropertyReceiver* receiver) const
{
    const Image* img = static_cast<const FalagardListHeaderSegment*>(receiver)->getSizingCursorImage();
    return img ? PropertyHelper::imageToString(img) : String("");
}

void SizingCursorImage::set(PropertyReceiver* receiver, const String &value)
{
    static_cast<FalagardListHeaderSegment*>(receiver)->setSizingCursorImage(PropertyHelper::stringToImage(value));
}

String MovingCursorImage::get(const PropertyReceiver* receiver) const
{
    const Image* img = static_cast<const FalagardListHeaderSegment*>(receiver)->getMovingCursorImage();
    return img ? PropertyHelper::imageToString(img) : String("");
}

void MovingCursorImage::set(PropertyReceiver* receiver, const String &value)
{
    static_cast<FalagardListHeaderSegment*>(receiver)->setMovingCursorImage(PropertyHelper::stringToImage(value));
}

}
} // End of  CEGUI namespace section
