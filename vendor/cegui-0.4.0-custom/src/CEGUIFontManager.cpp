/************************************************************************
	filename: 	CEGUIFontManager.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements the FontManager class
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
#include "CEGUIFontManager.h"
#include "CEGUIExceptions.h"
#include "CEGUILogger.h"
#include "CEGUIFont.h"
#include "CEGUIFontManager_implData.h"
#include "CEGUIFont_implData.h"
#include "CEGUISystem.h"

#include <ft2build.h>
#include FT_FREETYPE_H


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> FontManager* Singleton<FontManager>::ms_Singleton	= NULL;



/*************************************************************************
	constructor
*************************************************************************/
FontManager::FontManager(void)
{
	d_implData = new FontManagerImplData;

	if (FT_Init_FreeType(&d_implData->d_ftlib))
	{
		throw GenericException((utf8*)"FontManager::FontManager - Failed to initialise the FreeType library.");
	}

	Logger::getSingleton().logEvent((utf8*)"CEGUI::FontManager singleton created.");
}


/*************************************************************************
	Destructor
*************************************************************************/
FontManager::~FontManager(void)
{
	Logger::getSingleton().logEvent((utf8*)"---- Begining cleanup of Font system ----");
	destroyAllFonts();

	FT_Done_FreeType(d_implData->d_ftlib);
	delete d_implData;

	Logger::getSingleton().logEvent((utf8*)"CEGUI::FontManager singleton destroyed.");
}


/*************************************************************************
	Create a font from a definition file
*************************************************************************/
Font* FontManager::createFont(const String& filename, const String& resourceGroup)
{
	Logger::getSingleton().logEvent((utf8*)"Attempting to create Font from the information specified in file '" + filename + "'.");

	Font* temp = new Font(filename, resourceGroup, new Font::FontImplData(d_implData->d_ftlib));

	String name = temp->getName();

	if (isFontPresent(name))
	{
		delete temp;

		throw AlreadyExistsException((utf8*)"FontManager::createFont - A font named '" + name + "' already exists.");
	}

	d_fonts[name] = temp;

    // if this was the first font created, set it as the default font
    if (d_fonts.size() == 1)
    {
        System::getSingleton().setDefaultFont(temp);
    }

	return temp; 
}


/*************************************************************************
	Create a font from an installed OS font
*************************************************************************/
Font* FontManager::createFont(const String& name, const String& fontname, uint size, uint flags, const String& resourceGroup)
{
	char strbuf[16];
	sprintf(strbuf, "%d", size);
	Logger::getSingleton().logEvent((utf8*)"Attempting to create Font '" + name + "' using the font file '" + fontname + "' and a size of " + strbuf + ".");

	// first ensure name uniqueness
	if (isFontPresent(name))
	{
		throw AlreadyExistsException((utf8*)"FontManager::createFont - A font named '" + name + "' already exists.");
	}

	Font* temp = new Font(name, fontname, resourceGroup, size, flags, new Font::FontImplData(d_implData->d_ftlib));
	d_fonts[name] = temp;

    // if this was the first font created, set it as the default font
    if (d_fonts.size() == 1)
    {
        System::getSingleton().setDefaultFont(temp);
    }

	return temp; 
}


/*************************************************************************
	Destroy the named font
*************************************************************************/
void FontManager::destroyFont(const String& name)
{
	 FontRegistry::iterator	pos = d_fonts.find(name);

	if (pos != d_fonts.end())
	{
		String tmpName(name);

		delete pos->second;
		d_fonts.erase(pos);

		Logger::getSingleton().logEvent((utf8*)"Font '" + tmpName +"' has been destroyed.");
	}

}


/*************************************************************************
	Destroys the given Font object
*************************************************************************/
void FontManager::destroyFont(Font* font)
{
	if (font != NULL)
	{
		destroyFont(font->getName());
	}

}


/*************************************************************************
	Destroys all Font objects registered in the system
*************************************************************************/
void FontManager::destroyAllFonts(void)
{
	while (!d_fonts.empty())
	{
		destroyFont(d_fonts.begin()->first);
	}

}


/*************************************************************************
	Check to see if a font is available
*************************************************************************/
bool FontManager::isFontPresent(const String& name) const
{
	return (d_fonts.find(name) != d_fonts.end());
}


/*************************************************************************
	Return a pointer to the named font
*************************************************************************/
Font* FontManager::getFont(const String& name) const
{
	FontRegistry::const_iterator pos = d_fonts.find(name);

	if (pos == d_fonts.end())
	{
		throw UnknownObjectException("FontManager::getFont - A Font object with the specified name '" + name +"' does not exist within the system");
	}

	return pos->second;
}


/*************************************************************************
	Notify the FontManager of the current (usually new) display
	resolution.
*************************************************************************/
void FontManager::notifyScreenResolution(const Size& size)
{
	// notify all attached Font objects of the change in resolution
	FontRegistry::iterator pos = d_fonts.begin(), end = d_fonts.end();

	for (; pos != end; ++pos)
	{
		pos->second->notifyScreenResolution(size);
	}

}


FontManager& FontManager::getSingleton(void)
{
	return Singleton<FontManager>::getSingleton();
}


FontManager* FontManager::getSingletonPtr(void)
{
	return Singleton<FontManager>::getSingletonPtr();
}


/*************************************************************************
	Return a FontManager::FontIterator object to iterate over the
	available Font objects.
*************************************************************************/
FontManager::FontIterator FontManager::getIterator(void) const
{
	return FontIterator(d_fonts.begin(), d_fonts.end());
}


void FontManager::writeFontToStream(const String& name, OutStream& out_stream) const
{
    const Font* font = getFont(name);

    // output xml header
    out_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;

    // output font data
    font->writeXMLToStream(out_stream);
}

} // End of  CEGUI namespace section
