/************************************************************************
	filename: 	CEGUISchemeManager.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements SchemeManager class
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
#include "StdInc.h"
#include "CEGUISchemeManager.h"
#include "CEGUIExceptions.h"
#include "CEGUILogger.h"
#include "CEGUIScheme.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> SchemeManager* Singleton<SchemeManager>::ms_Singleton	= NULL;



/*************************************************************************
	constructor
*************************************************************************/
SchemeManager::SchemeManager(void)
{
	Logger::getSingleton().logEvent((utf8*)"CEGUI::SchemeManager singleton created.");
}


/*************************************************************************
	Destructor
*************************************************************************/
SchemeManager::~SchemeManager(void)
{
	Logger::getSingleton().logEvent((utf8*)"---- Begining cleanup of GUI Scheme system ----");

	unloadAllSchemes();

	Logger::getSingleton().logEvent((utf8*)"CEGUI::SchemeManager singleton destroyed.");
}


/*************************************************************************
	Loads a scheme
*************************************************************************/
Scheme* SchemeManager::loadScheme(const String& scheme_filename, const String& resourceGroup)
{
	Logger::getSingleton().logEvent((utf8*)"Attempting to load Scheme from file '" + scheme_filename + "'.");

	Scheme* tmp = new Scheme(scheme_filename, resourceGroup);
	String name = tmp->getName();
	d_schemes[name] = tmp;
	return tmp;
}


/*************************************************************************
	Un-Loads a scheme
*************************************************************************/
void SchemeManager::unloadScheme(const String& scheme_name)
{
	SchemeRegistry::iterator pos = d_schemes.find(scheme_name);

	if (pos != d_schemes.end())
	{
		String tmpName(scheme_name);

		delete pos->second;
		d_schemes.erase(pos);

		Logger::getSingleton().logEvent((utf8*)"Scheme '" + tmpName + "' has been unloaded.");
	}
	else
	{
		Logger::getSingleton().logEvent((utf8*)"Unable to unload non-existant scheme '" + scheme_name + "'.", Errors);
	}

}


/*************************************************************************
	Returns a pointer to the Scheme object with the specified name.
*************************************************************************/
Scheme* SchemeManager::getScheme(const String& name) const
{
	SchemeRegistry::const_iterator pos = d_schemes.find(name);

	if (pos == d_schemes.end())
	{
		throw UnknownObjectException("SchemeManager::getScheme - A Scheme object with the specified name '" + name +"' does not exist within the system");
	}

	return pos->second;
}


SchemeManager& SchemeManager::getSingleton(void)
{
	return Singleton<SchemeManager>::getSingleton();
}


SchemeManager* SchemeManager::getSingletonPtr(void)
{
	return Singleton<SchemeManager>::getSingletonPtr();
}


/*************************************************************************
	Return a SchemeManager::SchemeIterator object to iterate over the
	available schemes.
*************************************************************************/
SchemeManager::SchemeIterator SchemeManager::getIterator(void) const
{
	return SchemeIterator(d_schemes.begin(), d_schemes.end());
}


/*************************************************************************
	Unload all schemes currently defined within the system.
*************************************************************************/
void SchemeManager::unloadAllSchemes(void)
{
	// unload all schemes
	while (!d_schemes.empty())
	{
		unloadScheme(d_schemes.begin()->first);
	}

}


} // End of  CEGUI namespace section
