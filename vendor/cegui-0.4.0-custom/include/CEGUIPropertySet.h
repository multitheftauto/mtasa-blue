/************************************************************************
	filename: 	CEGUIPropertySet.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Defines interface for the PropertySet class
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
#ifndef _CEGUIPropertySet_h_
#define _CEGUIPropertySet_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIIteratorBase.h"
#include "CEGUIProperty.h"
#include <map>


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Class that contains a collection of Property objects.
*/
class CEGUIEXPORT PropertySet : public PropertyReceiver
{
public:
	/*!
	\brief
		Constructs a new PropertySet object
	*/
	PropertySet(void) {}


	/*!
	\brief
		Destructor for PropertySet objects.
	*/
	virtual ~PropertySet(void) {}


	/*!
	\brief
		Adds a new Property to the PropertySet

	\param property
		Pointer to the Property object to be added to the PropertySet.

	\return
		Nothing.

	\exception NullObjectException		Thrown if \a property is NULL.
	\exception AlreadyExistsException	Thrown if a Property with the same name as \a property already exists in the PropertySet
	*/
	void	addProperty(Property* property);


	/*!
	\brief
		Removes a Property from the PropertySet.

	\param name
		String containing the name of the Property to be removed.  If Property \a name is not in the set, nothing happens.

	\return
		Nothing.
	*/
	void	removeProperty(const String& name);


	/*!
	\brief
		Removes all Property objects from the PropertySet.

	\return
		Nothing.
	*/
	void	clearProperties(void);


	/*!
	\brief
		Checks to see if a Property with the given name is in the PropertySet

	\param name
		String containing the name of the Property to check for.

	\return
		true if a Property named \a name is in the PropertySet.  false if no Property named \a name is in the PropertySet.
	*/
	bool	isPropertyPresent(const String& name) const;


	/*!
	\brief
		Return the help text for the specified Property.

	\param name
		String holding the name of the Property who's help text is to be returned.

	\return
		String object containing the help text for the Property \a name.

	\exception UnknownObjectException	Thrown if no Property named \a name is in the PropertySet.
	*/
	const String&	getPropertyHelp(const String& name) const;


	/*!
	\brief
		Gets the current value of the specified Property.

	\param name
		String containing the name of the Property who's value is to be returned.

	\return
		String object containing a textual representation of the requested Property.

	\exception UnknownObjectException	Thrown if no Property named \a name is in the PropertySet.
	*/
	String	getProperty(const String& name) const;


	/*!
	\brief
		Sets the current value of a Property.

	\param name
		String containing the name of the Property who's value is to be set.

	\param value
		String containing a textual representation of the new value for the Property

	\return
		Nothing

	\exception UnknownObjectException	Thrown if no Property named \a name is in the PropertySet.
	\exception InvalidRequestException	Thrown when the Property was unable to interpret the content of \a value.
	*/
	void	setProperty(const String& name, const String& value);


	/*!
	\brief
		Returns whether a Property is at it's default value.

	\param name
		String containing the name of the Property who's default state is to be tested.

	\return
		- true if the property has it's default value.
		- false if the property has been modified from it's default value.
	*/
	bool	isPropertyDefault(const String& name) const;


	/*!
	\brief
		Returns the default value of a Property as a String.

	\param name
		String containing the name of the Property who's default string is to be returned.

	\return
		String object containing a textual representation of the default value for this property.
	*/
	String	getPropertyDefault(const String& name) const;

private:
	typedef std::map<String, Property*>	PropertyRegistry;
	PropertyRegistry	d_properties;


public:
	/*************************************************************************
		Iterator stuff
	*************************************************************************/
	typedef	ConstBaseIterator<PropertyRegistry>	PropertyIterator;

	/*!
	\brief
		Return a PropertySet::PropertyIterator object to iterate over the available Properties.
	*/
	PropertyIterator	getIterator(void) const;
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIPropertySet_h_
