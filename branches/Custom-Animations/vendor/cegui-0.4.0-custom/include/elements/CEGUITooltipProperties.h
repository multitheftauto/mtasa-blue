/************************************************************************
	filename: 	CEGUITooltipProperties.h
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
#ifndef _CEGUITooltipProperties_h_
#define _CEGUITooltipProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of TooltipProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the Tooltip class
*/
namespace TooltipProperties
{
    /*!
    \brief
        Property to access the timout that must expire before the tooltip gets activated.

    \par Usage:
        - Name: HoverTime
        - Format: "[float]".

    \par Where:
        - [float] specifies the number of seconds the mouse must hover stationary on a widget before the tooltip gets activated.
    */
    class HoverTime : public Property
    {
    public:
        HoverTime() : Property(
            "HoverTime",
            "Property to get/set the hover timeout value in seconds.  Value is a float.",
            "0.400000")
        {}

        String  get(const PropertyReceiver* receiver) const;
        void    set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
        Property to access the time after which the tooltip automatically de-activates itself.

    \par Usage:
        - Name: DisplayTime
        - Format: "[float]".

    \par Where:
        - [float] specifies the number of seconds after which the tooltip will deactivate itself if the mouse has remained stationary.
    */
    class DisplayTime : public Property
    {
    public:
        DisplayTime() : Property(
            "DisplayTime",
            "Property to get/set the display timeout value in seconds.  Value is a float.",
            "7.500000")
        {}

        String  get(const PropertyReceiver* receiver) const;
        void    set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
        Property to access the duration of the fade effect for the tooltip.

    \par Usage:
        - Name: FadeTime
        - Format: "[float]".

    \par Where:
        - [float] specifies the number of seconds over which the fade in / fade out effect will happen.
    */
    class FadeTime : public Property
    {
    public:
        FadeTime() : Property(
            "FadeTime",
            "Property to get/set duration of the fade effect in seconds.  Value is a float.",
            "0.330000")
        {}

        String  get(const PropertyReceiver* receiver) const;
        void    set(PropertyReceiver* receiver, const String& value);
    };

} // End of  TooltipProperties namespace section
} // End of  CEGUI namespace section


#endif	// end of guard _CEGUITooltipProperties_h_
