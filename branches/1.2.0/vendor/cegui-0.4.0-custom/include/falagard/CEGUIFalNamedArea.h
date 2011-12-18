/************************************************************************
    filename:   CEGUIFalNamedArea.h
    created:    Sun Jun 26 2005
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
#ifndef _CEGUIFalNamedArea_h_
#define _CEGUIFalNamedArea_h_

#include "falagard/CEGUIFalDimensions.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        NamedArea defines an area for a component which may later be obtained
        and referenced by a name unique to the WidgetLook holding the NamedArea.
    */
    class CEGUIEXPORT NamedArea
    {
    public:
        NamedArea() {}
        NamedArea(const String& name);

        /*!
        \brief
            Return the name of this NamedArea.

        \return
            String object holding the name of this NamedArea.
        */
        const String& getName() const;

        /*!
        \brief
            Return the ComponentArea of this NamedArea

        \return
            ComponentArea object describing the NamedArea's current target area.
        */
        const ComponentArea& getArea() const;

        /*!
        \brief
            Set the Area for this NamedArea.

        \param area
            ComponentArea object describing a new target area for the NamedArea..

        \return
            Nothing.
        */
        void setArea(const ComponentArea& area);

        /*!
        \brief
            Writes an xml representation of this NamedArea to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

    private:
        String d_name;
        ComponentArea d_area;
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalNamedArea_h_
