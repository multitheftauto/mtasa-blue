/************************************************************************
	filename: 	CEGUIScheme.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Defines abstract base class for the GUI Scheme object.
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
#ifndef _CEGUIScheme_h_
#define _CEGUIScheme_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUISchemeManager.h"

#include <vector>


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	A class that groups a set of GUI elements and initialises the system to access those elements.

	A GUI Scheme is a high-level construct that loads and initialises various lower-level objects
	and registers them within the system for usage.  So, for example, a Scheme might create some
	Imageset objects, some Font objects, and register a collection of WindowFactory objects within
	the system which would then be in a state to serve those elements to client code.
*/
class CEGUIEXPORT Scheme
{
	friend class Scheme_xmlHandler;
public:
	/*!
	\brief
		Loads all resources for this scheme.

	\return
		Nothing.
	*/
	void	loadResources(void);


	/*!
	\brief
		Unloads all resources for this scheme.  This should be used very carefully.

	\return
		Nothing.
	*/
	void	unloadResources(void);


	/*!
	\brief
		Return whether the resources for this Scheme are all loaded.

	\return
		true if all resources for the Scheme are loaded and available, or false of one or more resource is not currently loaded.
	*/
	bool	resourcesLoaded(void) const;


	/*!
	\brief
		Return the name of this Scheme.

	\return
		String object containing the name of this Scheme.
	*/
	const String& getName(void) const		{return d_name;}

private:
	/*************************************************************************
		Implementation Constants
	*************************************************************************/
	static const char	GUISchemeSchemaName[];			//!< Filename of the XML schema used for validating GUIScheme files.

	/*************************************************************************
		Friends
	*************************************************************************/
	friend	Scheme* SchemeManager::loadScheme(const String& scheme_filename, const String& resourceGroup);
	friend	void	SchemeManager::unloadScheme(const String& scheme_name);


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Creates a scheme object from the data specified in the file \a filename

	\param filename
		String object holding the name of the file to use when creating this Scheme object.

    \param resourceGroup
        Group identifier to be passed to the resource provider when loading the scheme
        specification file.

	\return
		Nothing.
	*/
	Scheme(const String& filename, const String& resourceGroup);


public:		// for luabind compatibility
	/*!
	\brief
		Destroys a Scheme object

	\return
		Nothing
	*/
	~Scheme(void);

	
private:
	/*************************************************************************
		Structs used to hold scheme information
	*************************************************************************/
	struct LoadableUIElement
	{
		String	name;
		String	filename;
        String  resourceGroup;
	};

	struct	UIElementFactory
	{
		String name;
	};

	struct	UIModule
	{
		String name;
		FactoryModule*	module;
		std::vector<UIElementFactory>	factories;
	};

	struct AliasMapping
	{
		String aliasName;
		String targetName;
	};

    struct FalagardMapping
    {
        String windowName;
        String targetName;
        String lookName;
    };

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	String	d_name;			//!< the name of this scheme.

	std::vector<LoadableUIElement>		d_imagesets;
	std::vector<LoadableUIElement>		d_imagesetsFromImages;
	std::vector<LoadableUIElement>		d_fonts;
	std::vector<UIModule>				d_widgetModules;
	std::vector<AliasMapping>			d_aliasMappings;
    std::vector<LoadableUIElement>		d_looknfeels;
    std::vector<FalagardMapping>        d_falagardMappings;
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIScheme_h_
