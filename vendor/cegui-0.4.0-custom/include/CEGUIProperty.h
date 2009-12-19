/************************************************************************
	filename: 	CEGUIProperty.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Defines the Property class which forms part of a
				PropertySet
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
#ifndef _CEGUIProperty_h_
#define _CEGUIProperty_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Dummy base class to ensure correct casting of receivers.
*/
class CEGUIEXPORT PropertyReceiver
{
public:
	PropertyReceiver() {}
	virtual ~PropertyReceiver() {}
};


/*!
\brief
	An abstract class that defines the interface to access object properties by name.

	Property objects allow (via a PropertySet) access to certain properties of objects
	by using simple get/set functions and the name of the property to be accessed.
*/
class CEGUIEXPORT Property
{
public:
	/*!
	\brief
		Creates a new Property object.

	\param name
		String containing the name of the new Property.

	\param help
		String containing a description of the Property and it's usage.
		
	\param defaultValue
		String holding the textual representation of the default value for this Property

    \param writesXML
        Specifies whether the writeXMLToStream method should do anything for this Property.  This
        enables selectivity in what properties within a PropertySet will get output as XML.
	*/
	Property(const String& name, const String& help, const String& defaultValue = "", bool writesXML = true) :
	  d_name(name),
	  d_help(help),
	  d_default(defaultValue),
	  d_writeXML(writesXML)
	{
	}


	/*!
	\brief
		Destructor for Property objects
	*/
	virtual ~Property(void) {}


	/*!
	\brief
		Return a String that describes the purpose and usage of this Property.

	\return
		String that contains the help text
	*/
	const String& getHelp(void) const		{return d_help;}


	/*!
	\brief
		Return a the name of this Property

	\return
		String containing the name of the Property
	*/
	const String& getName(void) const		{return d_name;}


	/*!
	\brief
		Return the current value of the Property as a String

	\param receiver
		Pointer to the target object.

	\return
		String object containing a textual representation of the current value of the Property
	*/
	virtual String	get(const PropertyReceiver* receiver) const = 0;


	/*!
	\brief
		Sets the value of the property

	\param receiver
		Pointer to the target object.

	\param value
		A String object that contains a textual representation of the new value to assign to the Property.

	\return
		Nothing.

	\exception InvalidRequestException	Thrown when the Property was unable to interpret the content of \a value.
	*/
	virtual void	set(PropertyReceiver* receiver, const String& value) = 0;


	/*!
	\brief
		Returns whether the property is at it's default value.

	\param receiver
		Pointer to the target object.

	\return
		- true if the property has it's default value.
		- false if the property has been modified from it's default value.
	*/
	virtual bool	isDefault(const PropertyReceiver* receiver) const;


	/*!
	\brief
		Returns the default value of the Property as a String.

	\param receiver
		Pointer to the target object.

	\return
		String object containing a textual representation of the default value for this property.
	*/
	virtual String	getDefault(const PropertyReceiver* receiver) const;


    /*!
    \brief
        Writes out an XML representation of this class to the given stream.

    \note
        This would normally have been implemented via XMLGenerator base class, but in this
        case we require the target PropertyReceiver in order to obtain the property value.
    */
    void writeXMLToStream(const PropertyReceiver* receiver, OutStream& out_stream) const;

protected:
	String	d_name;		//!< String that stores the Property name.
	String	d_help;		//!< String that stores the Property help text.
	String	d_default;	//!< String that stores the Property default value string.
	bool    d_writeXML; //!< Specifies whether writeXMLToStream should do anything for this property.
};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIProperty_h_
