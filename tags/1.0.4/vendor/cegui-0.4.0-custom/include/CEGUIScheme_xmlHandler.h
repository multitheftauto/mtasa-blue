/************************************************************************
    filename:   CEGUIScheme_xmlHandler.h
    created:    21/2/2004
    author:     Paul D Turner
    
    purpose:    Defines abstract base class for the GUI Scheme object.
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
#ifndef _CEGUIScheme_xmlHandler_h_
#define _CEGUIScheme_xmlHandler_h_

#include "CEGUIScheme.h"
#include "CEGUIXMLHandler.h"

// Start of CEGUI namespace section
namespace CEGUI
{

    /*!
    \brief
        Handler class used to parse the Scheme XML files using SAX2
    */
    class Scheme_xmlHandler : public XMLHandler
    {
    public:
        /*************************************************************************
        Construction & Destruction
        *************************************************************************/
        /*!
        \brief
            Constructor for Scheme::xmlHandler objects

        \param scheme
            Pointer to the Scheme object creating this xmlHandler object
        */
        Scheme_xmlHandler(Scheme* scheme) : d_scheme(scheme)
        {}

        /*!
        \brief
            Destructor for Scheme::xmlHandler objects
        */
        virtual ~Scheme_xmlHandler(void)
        {}

        /*************************************************************************
        SAX2 Handler overrides
        *************************************************************************/
        /*!
        \brief
            document processing (only care about elements, schema validates format)
        */
        virtual void elementStart(const String& element, const XMLAttributes& attributes);
        virtual void elementEnd(const String& element);

    private:
        /*************************************************************************
        Implementation Constants
        *************************************************************************/
        // XML related strings
        static const String GUISchemeElement;               //!< Root GUIScheme element.
        static const String ImagesetElement;                //!< Element specifying an Imageset
        static const String ImagesetFromImageElement;       //!< Element specifying an Imageset to be created directly via an image file.
        static const String FontElement;                    //!< Element specifying a Font
        static const String WindowSetElement;               //!< Element specifying a module and set of WindowFactory elements.
        static const String WindowFactoryElement;           //!< Element specifying a WindowFactory type.
        static const String WindowAliasElement;             //!< Element specifying a WindowFactory type alias
        static const String FalagardMappingElement;         //!< Element specifying a Falagard window mapping
        static const String LookNFeelElement;               //!< Element specifying a LookNFeel.
        static const String NameAttribute;                  //!< Attribute specifying the name of some object.
        static const String FilenameAttribute;              //!< Attribute specifying the name of some file.
        static const String AliasAttribute;                 //!< Attribute specifying an alias name.
        static const String TargetAttribute;                //!< Attribute specifying target for an alias.
        static const String ResourceGroupAttribute;         //!< Attribute specifying resource group for some loadable resource.
        static const String WindowTypeAttribute;            //!< Attribute specifying the type of a window being created via a mapping.
        static const String TargetTypeAttribute;            //!< Attribute specifying the base target type of a falagard mapped window type.
        static const String LookNFeelAttribute;             //!< Attribute specifying the name of a LookNFeel for a falagard mapping..


        /*************************************************************************
        Implementation Data
        *************************************************************************/
        Scheme* d_scheme;           //!< Scheme object that we are helping to build
    };

} // End of  CEGUI namespace section

#endif  // end of guard _CEGUIScheme_xmlHandler_h_
