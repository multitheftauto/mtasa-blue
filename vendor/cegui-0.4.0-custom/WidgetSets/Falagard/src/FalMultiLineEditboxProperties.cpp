/************************************************************************
    filename:   FalMultiLineEditboxProperties.cpp
    created:    Thu Jul 7 2005
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
#include "FalMultiLineEditboxProperties.h"
#include "FalMultiLineEditbox.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
namespace FalagardMultiLineEditboxProperties
{
    String SelectionBrushImage::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const FalagardMultiLineEditbox*>(receiver)->getSelectionBrushImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }

    void SelectionBrushImage::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<FalagardMultiLineEditbox*>(receiver)->setSelectionBrushImage(PropertyHelper::stringToImage(value));
    }

}
} // End of  CEGUI namespace section
