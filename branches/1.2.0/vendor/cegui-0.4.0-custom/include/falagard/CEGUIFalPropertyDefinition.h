/************************************************************************
    filename:   CEGUIFalPropertyDefinition.h
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
#ifndef _CEGUIFalPropertyDefinition_h_
#define _CEGUIFalPropertyDefinition_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        class representing a new property to be available on all widgets that use the WidgetLook
        that this PropertyDefinition is defiend for.
    */
    class CEGUIEXPORT PropertyDefinition : public Property
    {
    public:
        PropertyDefinition(const String& name, const String& initialValue, bool redrawOnWrite, bool layoutOnWrite);

        // abstract members from Property
        String get(const PropertyReceiver* receiver) const;
        void set(PropertyReceiver* receiver, const String& value);

        /*!
        \brief
            Writes an xml representation of this PropertyDefinition  to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

    protected:
        String d_userStringName;
        bool d_writeCausesRedraw;
        bool d_writeCausesLayout;
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalPropertyDefinition_h_
