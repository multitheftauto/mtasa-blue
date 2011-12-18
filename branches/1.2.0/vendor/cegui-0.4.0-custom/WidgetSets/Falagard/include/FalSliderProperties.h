/************************************************************************
    filename:   FalSliderProperties.h
    created:    Sun Jul 3 2005
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
#ifndef _FalSliderProperties_h_
#define _FalSliderProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Namespace containing the specialised properties interface for the Slider under Falagard class
*/
namespace FalagardSliderProperties
{
    /*!
    \brief
        Property to access the setting that controls whether the slider is horizontal or vertical.

        \par Usage:
            - Name: VerticalSlider
            - Format: "[text]".

        \par Where [Text] is:
            - "True" to indicate the slider operates in the vertical direction.
            - "False" to indicate the slider operates in the horizontal direction.
    */
    class VerticalSlider : public Property
    {
    public:
        VerticalSlider() : Property(
            "VerticalSlider",
            "Property to get/set whether the Slider operates in the vertical direction.  Value is either \"True\" or \"False\".",
            "False")
        {}

        String	get(const PropertyReceiver* receiver) const;
        void	set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
        Property to access the setting that controls the positive direction for the slider

        \par Usage:
            - Name: ReversedDirection
            - Format: "[text]".

        \par Where [Text] is:
            - "True" to indicate the slider value increases towards the bottom or left edges.
            - "False" to indicate the slider value increases towards the top or right edges (default).
    */
    class ReversedDirection : public Property
    {
    public:
        ReversedDirection() : Property(
            "ReversedDirection",
            "Property to get/set whether the Slider operates in reversed direction.  Value is either \"True\" or \"False\".",
            "False")
        {}

        String	get(const PropertyReceiver* receiver) const;
        void	set(PropertyReceiver* receiver, const String& value);
    };
}

} // End of  CEGUI namespace section


#endif  // end of guard _FalSliderProperties_h_
