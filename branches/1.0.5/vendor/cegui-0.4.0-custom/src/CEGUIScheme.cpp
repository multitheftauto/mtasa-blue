/************************************************************************
	filename: 	CEGUIScheme.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements GUI Scheme class
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
#include "CEGUIScheme.h"
#include "CEGUIExceptions.h"
#include "CEGUISchemeManager.h"
#include "CEGUILogger.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIFontManager.h"
#include "CEGUIFont.h"
#include "CEGUIWindowFactoryManager.h"
#include "CEGUIFactoryModule.h"
#include "CEGUIScheme_xmlHandler.h"
#include "CEGUIDataContainer.h"
#include "CEGUISystem.h"
#include "CEGUIXMLParser.h"
#include "falagard/CEGUIFalWidgetLookManager.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Static Data definitions
*************************************************************************/
// name of the xml schema for GUIScheme files
const char Scheme::GUISchemeSchemaName[]					= "GUIScheme.xsd";


/*************************************************************************
	Constructor for scheme objects
*************************************************************************/
Scheme::Scheme(const String& filename, const String& resourceGroup)
{
	if (filename.empty() || (filename == (utf8*)""))
	{
		throw InvalidRequestException((utf8*)"Scheme::Scheme - Filename supplied for Scheme loading must be valid");
	}

    // create handler object
    Scheme_xmlHandler handler(this);

    // do parse (which uses handler to create actual data)
	try
	{
        System::getSingleton().getXMLParser()->parseXMLFile(handler, filename, GUISchemeSchemaName, resourceGroup);
	}
	catch(...)
	{
        Logger::getSingleton().logEvent("Scheme::Scheme - loading of Scheme from file '" + filename +"' failed.", Errors);
        throw;
	}

	Logger::getSingleton().logEvent((utf8*)"Loaded GUI scheme '" + d_name + "' from data in file '" + filename + "'.", Informative);

	// attempt to load in resources
	loadResources();
}


/*************************************************************************
	Destructor for scheme objects
*************************************************************************/
Scheme::~Scheme(void)
{
	unloadResources();

	Logger::getSingleton().logEvent((utf8*)"GUI scheme '" + d_name + "' has been unloaded.", Informative);
}


/*************************************************************************
	Load all resources for this scheme
*************************************************************************/
void Scheme::loadResources(void)
{
	Logger::getSingleton().logEvent((utf8*)"---- Begining resource loading for GUI scheme '" + d_name + "' ----", Informative);

	ImagesetManager& ismgr		= ImagesetManager::getSingleton();
	FontManager& fntmgr			= FontManager::getSingleton();
	WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();
    WidgetLookManager& wlfMgr   = WidgetLookManager::getSingleton();

	std::vector<LoadableUIElement>::const_iterator	pos;

	// check imagesets
	for (pos = d_imagesets.begin(); pos != d_imagesets.end(); ++pos)
	{
		if (!ismgr.isImagesetPresent((*pos).name))
		{
			Imageset* iset = ismgr.createImageset((*pos).filename, (*pos).resourceGroup);

			// check for wrong imageset for specified name
			String realname = iset->getName();

			if (realname != (*pos).name)
			{
				ismgr.destroyImageset(iset);
				throw InvalidRequestException((utf8*)"Scheme::loadResources - The Imageset created by file '" + 
					(*pos).filename + "' is named '" + realname + "', not '" + (*pos).name + "' as required by Scheme '" + d_name + "'.");
			}

		}

	}

    // check imagesets that are created directly from image files
    for (pos = d_imagesetsFromImages.begin(); pos != d_imagesetsFromImages.end(); ++pos)
    {
        if (!ismgr.isImagesetPresent((*pos).name))
            ismgr.createImagesetFromImageFile((*pos).name, (*pos).filename, (*pos).resourceGroup);
    }

	// check fonts
	for (pos = d_fonts.begin(); pos != d_fonts.end(); ++pos)
	{
		if (!fntmgr.isFontPresent((*pos).name))
		{
			Font* font = fntmgr.createFont((*pos).filename, (*pos).resourceGroup);

			// check for wrong font for specified name
			String realname = font->getName();

			if (realname != (*pos).name)
			{
				fntmgr.destroyFont(font);
				throw InvalidRequestException((utf8*)"Scheme::loadResources - The Font created by file '" + 
					(*pos).filename + "' is named '" + realname + "', not '" + (*pos).name + "' as required by Scheme '" + d_name + "'.");
			}

		}

	}

    // load look'n'feels (can't actually check these, so just re-parse data; it does no harm except maybe wase a bit of time)
    for (pos = d_looknfeels.begin(); pos != d_looknfeels.end(); ++pos)
    {
        wlfMgr.parseLookNFeelSpecification((*pos).filename, (*pos).resourceGroup);
    }

	// check factories
	std::vector<UIModule>::iterator	cmod = d_widgetModules.begin();
	for (;cmod != d_widgetModules.end(); ++cmod)
	{
		// create and load dynamic module as required
		if ((*cmod).module == NULL)
		{
			(*cmod).module = new FactoryModule((*cmod).name);
		}

        // see if we should just register all factories available in the module (i.e. No factories explicitly specified)
        if ((*cmod).factories.size() == 0)
        {
            Logger::getSingleton().logEvent("No window factories specified for module '" + (*cmod).name + "' - adding all available factories...");
            (*cmod).module->registerAllFactories();
        }
        // some names were explicitly given, so only register those.
        else
        {
            std::vector<UIElementFactory>::const_iterator	elem = (*cmod).factories.begin();
            for (; elem != (*cmod).factories.end(); ++elem)
            {
                if (!wfmgr.isFactoryPresent((*elem).name))
                {
                    (*cmod).module->registerFactory((*elem).name);
                }
            }
        }
	}

	// check aliases
	std::vector<AliasMapping>::iterator alias = d_aliasMappings.begin();
	for (;alias != d_aliasMappings.end(); ++alias)
	{
		// get iterator 
		WindowFactoryManager::TypeAliasIterator iter = wfmgr.getAliasIterator();

		// look for this alias
		while (!iter.isAtEnd() && (iter.getCurrentKey() != (*alias).aliasName))
			++iter;

		// if the alias exists
		if (!iter.isAtEnd())
		{
			// if the current target type matches
			if (iter.getCurrentValue().getActiveTarget() == (*alias).targetName)
			{
				// assume this mapping is ours and skip to next alias
				continue;
			}

		}

		// create a new alias entry
		wfmgr.addWindowTypeAlias((*alias).aliasName, (*alias).targetName);
	}

    // check falagard window mappings.
    std::vector<FalagardMapping>::iterator falagard = d_falagardMappings.begin();
    for (;falagard != d_falagardMappings.end(); ++falagard)
    {
        // get iterator
        WindowFactoryManager::FalagardMappingIterator iter = wfmgr.getFalagardMappingIterator();

        // look for this mapping
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*falagard).windowName))
            ++iter;

        // if the alias exists
        if (!iter.isAtEnd())
        {
            // if the current target and looks match
            if ((iter.getCurrentValue().d_baseType == (*falagard).targetName) &&
                (iter.getCurrentValue().d_lookName == (*falagard).lookName))
            {
                // assume this mapping is ours and skip to next
                continue;
            }
        }

        // create a new mapping entry
        wfmgr.addFalagardWindowMapping((*falagard).windowName, (*falagard).targetName, (*falagard).lookName);
    }

	Logger::getSingleton().logEvent((utf8*)"---- Resource loading for GUI scheme '" + d_name + "' completed ----", Informative);
}


/*************************************************************************
	Unload all resources for this scheme
*************************************************************************/
void Scheme::unloadResources(void)
{
	Logger::getSingleton().logEvent((utf8*)"---- Begining resource cleanup for GUI scheme '" + d_name + "' ----", Informative);

	ImagesetManager& ismgr		= ImagesetManager::getSingleton();
	FontManager& fntmgr			= FontManager::getSingleton();
	WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

	std::vector<LoadableUIElement>::const_iterator	pos;

	// check fonts
	for (pos = d_fonts.begin(); pos != d_fonts.end(); ++pos)
	{
		fntmgr.destroyFont((*pos).name);
	}

	// check Imagesets
	for (pos = d_imagesets.begin(); pos != d_imagesets.end(); ++pos)
	{
		ismgr.destroyImageset((*pos).name);
	}

    // check imagesets that are created directly from image files
    for (pos = d_imagesetsFromImages.begin(); pos != d_imagesetsFromImages.end(); ++pos)
    {
        ismgr.destroyImageset((*pos).name);
    }

	// check factories
	std::vector<UIModule>::iterator	cmod = d_widgetModules.begin();
	for (;cmod != d_widgetModules.end(); ++cmod)
	{
		std::vector<UIElementFactory>::const_iterator	elem = (*cmod).factories.begin();
		for (; elem != (*cmod).factories.end(); ++elem)
		{
			wfmgr.removeFactory((*elem).name);
		}

		// unload dynamic module as required
		if ((*cmod).module != NULL)
		{
			delete (*cmod).module;
			(*cmod).module = NULL;
		}

	}

	// check aliases
	std::vector<AliasMapping>::iterator alias = d_aliasMappings.begin();
	for (;alias != d_aliasMappings.end(); ++alias)
	{
		// get iterator 
		WindowFactoryManager::TypeAliasIterator iter = wfmgr.getAliasIterator();

		// look for this alias
		while (!iter.isAtEnd() && (iter.getCurrentKey() != (*alias).aliasName))
			++iter;

		// if the alias exists
		if (!iter.isAtEnd())
		{
			// create a new alias entry
			wfmgr.removeWindowTypeAlias((*alias).aliasName, (*alias).targetName);
		}

	}

    // check falagard window mappings.
    std::vector<FalagardMapping>::iterator falagard = d_falagardMappings.begin();
    for (;falagard != d_falagardMappings.end(); ++falagard)
    {
        // get iterator
        WindowFactoryManager::FalagardMappingIterator iter = wfmgr.getFalagardMappingIterator();

        // look for this mapping
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*falagard).windowName))
            ++iter;

        // if the alias exists
        if (!iter.isAtEnd())
        {
            // if the current target and looks match
            if ((iter.getCurrentValue().d_baseType == (*falagard).targetName) &&
                (iter.getCurrentValue().d_lookName == (*falagard).lookName))
            {
                // assume this mapping is ours and delete it
                wfmgr.removeFalagardWindowMapping((*falagard).targetName);
            }
        }
    }

	Logger::getSingleton().logEvent((utf8*)"---- Resource cleanup for GUI scheme '" + d_name + "' completed ----", Informative);
}


/*************************************************************************
	Check if all resources for this Scheme are loaded
*************************************************************************/
bool Scheme::resourcesLoaded(void) const
{
	ImagesetManager& ismgr		= ImagesetManager::getSingleton();
	FontManager& fntmgr			= FontManager::getSingleton();
	WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

	std::vector<LoadableUIElement>::const_iterator	pos;

	// check imagesets
	for (pos = d_imagesets.begin(); pos != d_imagesets.end(); ++pos)
	{
		if (!ismgr.isImagesetPresent((*pos).name))
		{
			return false;
		}

	}

	// check fonts
	for (pos = d_fonts.begin(); pos != d_fonts.end(); ++pos)
	{
		if (!fntmgr.isFontPresent((*pos).name))
		{
			return false;
		}

	}

	// check factories
	std::vector<UIModule>::const_iterator	cmod = d_widgetModules.begin();
	for (;cmod != d_widgetModules.end(); ++cmod)
	{
		std::vector<UIElementFactory>::const_iterator	elem = (*cmod).factories.begin();

		for (; elem != (*cmod).factories.end(); ++elem)
		{
			if (!wfmgr.isFactoryPresent((*elem).name))
			{
				return false;
			}

		}

	}

	// check aliases
	std::vector<AliasMapping>::const_iterator alias = d_aliasMappings.begin();
	for (;alias != d_aliasMappings.end(); ++alias)
	{
		// get iterator 
		WindowFactoryManager::TypeAliasIterator iter = wfmgr.getAliasIterator();

		// look for this alias
		while (!iter.isAtEnd() && (iter.getCurrentKey() != (*alias).aliasName))
			++iter;

		// if the alias exists
		if (!iter.isAtEnd())
		{
			// if the current target type matches
			if (iter.getCurrentValue().getActiveTarget() == (*alias).targetName)
			{
				// target matches, assume we set it and continue to next alias
				continue;
			}

		}

		// no alias or target type does not match
		return false;
	}

	return true;
}


} // End of  CEGUI namespace section
