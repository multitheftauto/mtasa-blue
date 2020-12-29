/************************************************************************
    filename:   CEGUIXMLParser.h
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
#ifndef _CEGUIXMLParser_h_
#define _CEGUIXMLParser_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        This is an abstract class that is used by CEGUI to interface with XML parser libraries.
     */
    class CEGUIEXPORT XMLParser
    {
    public:
        /*!
        \brief
            XMLParser base class constructor.
         */
        XMLParser(void);

        /*!
        \brief
            XMLParser base class destructor.
         */
        virtual ~XMLParser(void);

        /*!
        \brief
            Initialises the XMLParser module ready for use.

            Note that this calls the protected abstract method 'initialiseImpl', which should
            be provided in your implementation to perform any required initialisation.

        \return
            - true if the module initialised successfully.
            - false if the module initialisation failed.
         */
        bool initialise(void);

        /*!
        \brief
            Cleans up the XMLParser module after use.

            Note that this calls the protected abstract method 'cleanupImpl', which should
            be provided in your implementation to perform any required cleanup.

        \return
            Nothing.
         */
        void cleanup(void);

        /*!
        \brief
            abstract method which initiates parsing of an XML file.

        \param handler
            XMLHandler based object which will process the XML elements.

        \param filename
            String object holding the filename of the XML file to be parsed.

        \param schemaName
            String object holding the name of the XML schema file to use for validating the XML.
            Note that whether this is used or not is dependant upon the XMLParser in use.

        \param resourceGroup
            String object holding the resource group identifier which will be passed to the
            ResourceProvider when loading the XML and schema files.

        \return
            Nothing.
         */
        virtual void parseXMLFile(XMLHandler& handler, const String& filename, const String& schemaName, const String& resourceGroup) = 0;

        /*!
        \brief
            Return identification string for the XML parser module.  If the internal id string has not been
            set by the XML parser module creator, a generic string of "Unknown XML parser" will be returned.

        \return
            String object holding a string that identifies the XML parser in use.
        */
        const String& getIdentifierString() const;

    protected:
        /*!
        \brief
            abstract method which initialises the XMLParser ready for use.

        \return
            - true if the module initialised successfully.
            - false if the module initialisation failed.
         */
        virtual bool initialiseImpl(void) = 0;

        /*!
        \brief
            abstract method which cleans up the XMLParser after use.

        \return
            Nothing.
         */

        virtual void cleanupImpl(void) = 0;

        // data fields
        String d_identifierString;                 //!< String that holds some id information about the module.

    private:
        bool d_initialised;     //!< true if the parser module has been initialised,
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIXMLParser_h_
