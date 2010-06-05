/************************************************************************
    filename:   FalScrollbarProperties.h
    created:    Mon Jul 4 2005
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
#ifndef _FalScrollbarProperties_h_
#define _FalScrollbarProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Namespace containing the specialised properties interface for the Scrollbar under Falagard class
*/
namespace FalagardScrollbarProperties
{
    /*!
    \brief
        Property to access the setting that controls whether the scrollbar is horizontal or vertical.

        \par Usage:
            - Name: VerticalScrollbar
            - Format: "[text]".

        \par Where [Text] is:
            - "True" to indicate the scrollbar operates in the vertical direction.
            - "False" to indicate the scrollbar operates in the horizontal direction.
    */
    class VerticalScrollbar : public Property
    {
    public:
        VerticalScrollbar() : Property(
            "VerticalScrollbar",
            "Property to get/set whether the Scrollbar operates in the vertical direction.  Value is either \"True\" or \"False\".",
            "False")
        {}

        String	get(const PropertyReceiver* receiver) const;
        void	set(PropertyReceiver* receiver, const String& value);
    };

}

} // End of  CEGUI namespace section


#endif  // end of guard _FalScrollbarProperties_h_
