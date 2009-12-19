/************************************************************************
    filename:   CEGUIXMLHandler.h
    created:    12/3/2005
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
#ifndef _CEGUIXMLHandler_h_
#define _CEGUIXMLHandler_h_

#include "CEGUIBase.h"


// Start of CEGUI namespace section
namespace CEGUI
{
    class CEGUIEXPORT XMLHandler
    {
    public:
        /*!
        \brief
            XMLHandler base class constructor.
         */
        XMLHandler(void);

        /*!
        \brief
            XMLHandler base class destructor.
         */
        virtual ~XMLHandler(void);
        
        /*!
        \brief
            Method called to notify the handler at the start of each XML element encountered.

        \param element
            String object holding the name of the element that is starting.

        \param attributes
            An XMLAttributes object holding the collection of attributes specified for the element.

        \return
            Nothing.
         */
        virtual void elementStart(const String& element, const XMLAttributes& attributes);
        
        /*!
        \brief
            Method called to notify the handler at the end of each XML element encountered.

        \param element
            String object holding the name of the element that is ending.

        \return
            Nothing.
         */
        virtual void elementEnd(const String& element);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIXMLHandler_h_
