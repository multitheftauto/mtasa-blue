/***********************************************************************
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements PropertySet class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/PropertySet.h"
#include "CEGUI/Property.h"
#include "CEGUI/Exceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Add a new property to the set
*************************************************************************/
void PropertySet::addProperty(Property* property)
{
	if (!property)
	{
		CEGUI_THROW(NullObjectException("The given Property object pointer is invalid."));
	}

	if (!d_properties.insert(std::make_pair(property->getName(), property)).second)
	{
		CEGUI_THROW(AlreadyExistsException("A Property named '" + property->getName() + "' already exists in the PropertySet."));
	}

    property->initialisePropertyReceiver(this);
}

/*************************************************************************
	Remove a property from the set
*************************************************************************/
void PropertySet::removeProperty(const String& name)
{
	PropertyRegistry::iterator pos = d_properties.find(name);

	if (pos != d_properties.end())
	{
		d_properties.erase(pos);
	}
}

/*************************************************************************
    Retrieves a property instance from the set
*************************************************************************/
Property* PropertySet::getPropertyInstance(const String& name) const
{
    PropertyRegistry::const_iterator pos = d_properties.find(name);

    if (pos == d_properties.end())
    {
        CEGUI_THROW(UnknownObjectException("There is no Property named '" + name + "' available in the set."));
    }

    return pos->second;
}

/*************************************************************************
	Remove all properties from the set
*************************************************************************/
void PropertySet::clearProperties(void)
{
	d_properties.clear();
}

/*************************************************************************
	Return true if a property with the given name is in the set
*************************************************************************/
bool PropertySet::isPropertyPresent(const String& name) const
{
	return (d_properties.find(name) != d_properties.end());
}

/*************************************************************************
	Return the help string for a property
*************************************************************************/
const String& PropertySet::getPropertyHelp(const String& name) const
{
	PropertyRegistry::const_iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		CEGUI_THROW(UnknownObjectException("There is no Property named '" + name + "' available in the set."));
	}

	return pos->second->getHelp();
}

/*************************************************************************
	Return the current value of a property
*************************************************************************/
String PropertySet::getProperty(const String& name) const
{
	PropertyRegistry::const_iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		CEGUI_THROW(UnknownObjectException("There is no Property named '" + name + "' available in the set."));
	}

	return pos->second->get(this);
}

/*************************************************************************
	Set the current value of a property
*************************************************************************/
void PropertySet::setProperty(const String& name,const String& value)
{
	PropertyRegistry::iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		CEGUI_THROW(UnknownObjectException("There is no Property named '" + name + "' available in the set."));
	}

	pos->second->set(this, value);
}


/*************************************************************************
	Return a PropertySet::PropertyIterator object to iterate over the
	available Properties.
*************************************************************************/
PropertySet::PropertyIterator PropertySet::getPropertyIterator(void) const
{
	return PropertyIterator(d_properties.begin(), d_properties.end());
}


/*************************************************************************
	Returns whether a Property is at it's default value.
*************************************************************************/
bool PropertySet::isPropertyDefault(const String& name) const
{
	PropertyRegistry::const_iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		CEGUI_THROW(UnknownObjectException("There is no Property named '" + name + "' available in the set."));
	}

	return pos->second->isDefault(this);
}


/*************************************************************************
	Returns the default value of a Property as a String.	
*************************************************************************/
String PropertySet::getPropertyDefault(const String& name) const
{
	PropertyRegistry::const_iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		CEGUI_THROW(UnknownObjectException("There is no Property named '" + name + "' available in the set."));
	}

	return pos->second->getDefault(this);
}

} // End of  CEGUI namespace section
