/************************************************************************
	filename: 	CEGUIScrollablePaneProperties.cpp
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
#include "StdInc.h"
#include "elements/CEGUIScrollablePaneProperties.h"
#include "elements/CEGUIScrollablePane.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of ScrollablePaneProperties namespace section
namespace ScrollablePaneProperties
{
    String ContentPaneAutoSized::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::boolToString(static_cast<const ScrollablePane*>(receiver)->isContentPaneAutoSized());
    }

    void ContentPaneAutoSized::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setContentPaneAutoSized(PropertyHelper::stringToBool(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String ContentArea::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::rectToString(static_cast<const ScrollablePane*>(receiver)->getContentPaneArea());
    }

    void ContentArea::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setContentPaneArea(PropertyHelper::stringToRect(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String ForceVertScrollbar::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::boolToString(static_cast<const ScrollablePane*>(receiver)->isVertScrollbarAlwaysShown());
    }

    void ForceVertScrollbar::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setShowVertScrollbar(PropertyHelper::stringToBool(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String ForceHorzScrollbar::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::boolToString(static_cast<const ScrollablePane*>(receiver)->isHorzScrollbarAlwaysShown());
    }

    void ForceHorzScrollbar::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setShowHorzScrollbar(PropertyHelper::stringToBool(value));
    }

    //////////////////////////////////////////////////////////////////////////
    
    String HorzStepSize::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const ScrollablePane*>(receiver)->getHorizontalStepSize());
    }

    void HorzStepSize::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setHorizontalStepSize(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String HorzOverlapSize::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const ScrollablePane*>(receiver)->getHorizontalOverlapSize());
    }

    void HorzOverlapSize::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setHorizontalOverlapSize(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String HorzScrollPosition::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const ScrollablePane*>(receiver)->getHorizontalScrollPosition());
    }

    void HorzScrollPosition::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setHorizontalScrollPosition(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String VertStepSize::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const ScrollablePane*>(receiver)->getHorizontalStepSize());
    }

    void VertStepSize::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setVerticalStepSize(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String VertOverlapSize::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const ScrollablePane*>(receiver)->getHorizontalOverlapSize());
    }

    void VertOverlapSize::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setVerticalOverlapSize(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String VertScrollPosition::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const ScrollablePane*>(receiver)->getHorizontalScrollPosition());
    }

    void VertScrollPosition::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<ScrollablePane*>(receiver)->setVerticalScrollPosition(PropertyHelper::stringToFloat(value));
    }

} // End of  ScrollablePaneProperties namespace section
} // End of  CEGUI namespace section
