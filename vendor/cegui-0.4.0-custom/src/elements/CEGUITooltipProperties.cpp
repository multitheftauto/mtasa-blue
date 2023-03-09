/************************************************************************
    filename:   CEGUITooltipProperties.cpp
    created:    3/3/2005
    author:     Paul D Turner
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
#include "elements/CEGUITooltipProperties.h"
#include "elements/CEGUITooltip.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of TooltipProperties namespace section
namespace TooltipProperties
{
    String HoverTime::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const Tooltip*>(receiver)->getHoverTime());
    }

    void HoverTime::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<Tooltip*>(receiver)->setHoverTime(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String DisplayTime::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const Tooltip*>(receiver)->getDisplayTime());
    }

    void DisplayTime::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<Tooltip*>(receiver)->setDisplayTime(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String FadeTime::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const Tooltip*>(receiver)->getFadeTime());
    }

    void FadeTime::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<Tooltip*>(receiver)->setFadeTime(PropertyHelper::stringToFloat(value));
    }

} // End of  TooltipProperties namespace section
} // End of  CEGUI namespace section
