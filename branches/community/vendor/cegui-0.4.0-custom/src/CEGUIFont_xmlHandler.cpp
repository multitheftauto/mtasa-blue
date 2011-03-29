/************************************************************************
filename: 	CEGUIFont.cpp
created:	21/2/2004
author:		Paul D Turner

purpose:	Implements Font class
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
#include "CEGUIFont_xmlHandler.h"

#include "CEGUIExceptions.h"
#include "CEGUIImageset.h"
#include "CEGUILogger.h"
#include "CEGUIXMLAttributes.h"

#include <ft2build.h>
#include FT_FREETYPE_H


// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
static data definitions
*************************************************************************/

// XML related strings
const String Font_xmlHandler::FontElement( (utf8*)"Font" );
const String Font_xmlHandler::MappingElement( (utf8*)"Mapping" );
const String Font_xmlHandler::FontTypeStatic( (utf8*)"Static" );
const String Font_xmlHandler::FontTypeDynamic( (utf8*)"Dynamic" );
const String Font_xmlHandler::GlyphElement( (utf8*)"Glyph" );
const String Font_xmlHandler::GlyphRangeElement( (utf8*)"GlyphRange" );
const String Font_xmlHandler::GlyphSetElement( (utf8*)"GlyphSet" );
const char	Font_xmlHandler::FontNameAttribute[]			= "Name";
const char	Font_xmlHandler::FontFilenameAttribute[]		= "Filename";
const char	Font_xmlHandler::FontResourceGroupAttribute[]   = "ResourceGroup";
const char	Font_xmlHandler::FontTypeAttribute[]			= "Type";
const char	Font_xmlHandler::FontSizeAttribute[]			= "Size";
const char	Font_xmlHandler::FontFirstCodepointAttribute[]	= "FirstCodepoint";
const char	Font_xmlHandler::FontLastCodepointAttribute[]	= "LastCodepoint";
const char	Font_xmlHandler::FontNativeHorzResAttribute[]	= "NativeHorzRes";
const char	Font_xmlHandler::FontNativeVertResAttribute[]	= "NativeVertRes";
const char	Font_xmlHandler::FontAutoScaledAttribute[]		= "AutoScaled";
const char	Font_xmlHandler::FontAntiAliasedAttribute[]		= "AntiAlias";
const char	Font_xmlHandler::MappingCodepointAttribute[]	= "Codepoint";
const char	Font_xmlHandler::MappingImageAttribute[]		= "Image";
const char	Font_xmlHandler::MappingHorzAdvanceAttribute[]	= "HorzAdvance";
const char	Font_xmlHandler::GlyphCodepointAttribute[]		= "Codepoint";
const char	Font_xmlHandler::GlyphRangeStartCodepointAttribute[]	= "StartCodepoint";
const char	Font_xmlHandler::GlyphRangeEndCodepointAttribute[]	= "EndCodepoint";
const char	Font_xmlHandler::GlyphSetGlyphsAttribute[]		= "Glyphs";

// General constants
const int	Font_xmlHandler::AutoGenerateHorzAdvance		= -1;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*************************************************************************
SAX2 Handler methods
*************************************************************************/
void Font_xmlHandler::elementStart(const String& element, const XMLAttributes& attributes)
{
	// handle a Mapping element
	if (element == MappingElement)
	{
		if (!d_font->d_freetype)
		{
            String	image_name(attributes.getValueAsString(MappingImageAttribute));
			utf32 codepoint = (utf32)attributes.getValueAsInteger(MappingCodepointAttribute);
            int horzAdvance = attributes.getValueAsInteger(MappingHorzAdvanceAttribute, -1);

			Font::glyphDat	mapDat;
			mapDat.d_image = &d_font->d_glyph_images->getImage(image_name);

			// calculate advance width if it was not specified
			if (horzAdvance == AutoGenerateHorzAdvance)
			{
				horzAdvance = (int)(mapDat.d_image->getWidth() + mapDat.d_image->getOffsetX());
			}

			mapDat.d_horz_advance_unscaled = horzAdvance;
            mapDat.d_horz_advance = (uint)(((float)horzAdvance) * (d_font->d_autoScale ? d_font->d_horzScaling : 1.0f));
			d_font->d_cp_map[codepoint] = mapDat;
		}
		else
		{
			Logger::getSingleton().logEvent((utf8*)"Mapping element encountered.  This element is invalid for dynamic fonts.", Informative);
		}
	}
	// handle root Font element
	else if (element == FontElement)
	{
		// get name of font we are creating
		String font_name(attributes.getValueAsString(FontNameAttribute));

		// get filename for the font
        String filename(attributes.getValueAsString(FontFilenameAttribute));
        // get resource group for font file.
        String resourceGroup(attributes.getValueAsString(FontResourceGroupAttribute));

		Logger::getSingleton().logEvent("Started creation of Font '" + font_name + "' via XML file.", Informative);

		//
		// load auto-scaling configuration
		//
		float hres, vres;
		bool auto_scale;

		// get native horizontal resolution
        hres = (float)attributes.getValueAsInteger(FontNativeHorzResAttribute, 640);

		// get native vertical resolution
        vres = (float)attributes.getValueAsInteger(FontNativeVertResAttribute, 480);

		// get auto-scaling setting
        auto_scale = attributes.getValueAsBool(FontAutoScaledAttribute, false);

		//
		// get type of font
		//
        String	font_type(attributes.getValueAsString(FontTypeAttribute));

		// dynamic (ttf) font
		if (font_type == FontTypeDynamic)
		{
			// get size of font
            uint size = (uint)attributes.getValueAsInteger(FontSizeAttribute, 12);

			// extract codepoint range
            utf32 first_codepoint = (utf32)attributes.getValueAsInteger(FontFirstCodepointAttribute, 32);
            utf32 last_codepoint = (utf32)attributes.getValueAsInteger(FontLastCodepointAttribute, 127);

			// build string containing the required code-points.
			for (;first_codepoint <= last_codepoint; ++first_codepoint)
			{
				d_glyphSet += first_codepoint;
			}

            uint flags = attributes.getValueAsBool(FontAntiAliasedAttribute, true) ? 0 : NoAntiAlias;

			// perform pre-initialisation
			d_font->setNativeResolution(Size(hres, vres));
			d_font->setAutoScalingEnabled(auto_scale);

			// Finalise construction of font without glyphs.
			// Glyphs will defined after we know which ones we need.
			d_font->constructor_impl(font_name, filename, resourceGroup, size, flags, String(""));
		}
		// static (Imageset based) font
		else if (font_type == FontTypeStatic)
		{
			d_font->d_name = font_name;
			d_font->d_freetype = false;

			// load the Imageset
			d_font->d_glyph_images = ImagesetManager::getSingleton().createImageset(filename, resourceGroup);

			d_font->setNativeResolution(Size(hres, vres));
			d_font->setAutoScalingEnabled(auto_scale);
		}
		// error (should never happen)
		else
		{
			throw FileIOException("Font::xmlHandler::startElement - The unknown Font:Type attribute value '" + font_type + "' was encountered while processing the Font file.");
		}

        d_font->d_sourceFilename = filename;
	}
	// Glyph element
	else if (element == GlyphElement)
	{
		if (d_font->d_freetype)
		{
            utf32 codepoint = (utf32)attributes.getValueAsInteger(GlyphCodepointAttribute);

			if (d_glyphSet.find(codepoint) == String::npos)
			{
				d_glyphSet.append(1, codepoint);
			}
		}
		else
		{
			Logger::getSingleton().logEvent((utf8*)"Glyph element encountered.  This element is invalid for static fonts.", Informative);
		}
	}
	// GlyphRange element
	else if (element == GlyphRangeElement)
	{
		if (d_font->d_freetype)
		{
            utf32 start = (utf32)attributes.getValueAsInteger(GlyphRangeStartCodepointAttribute);
            utf32 end	= (utf32)attributes.getValueAsInteger(GlyphRangeEndCodepointAttribute);

			for (utf32 codepoint = start; codepoint <= end; ++codepoint)
			{
				if (d_glyphSet.find(codepoint) == String::npos)
				{
					d_glyphSet.append(1, codepoint);
				}
			}

		}
		else
		{
			Logger::getSingleton().logEvent((utf8*)"GlyphRange element encountered.  This element is invalid for static fonts.", Informative);
		}
	}
	// GlyphSet element
	else if (element == GlyphSetElement)
	{
		if (d_font->d_freetype)
		{
            String glyphs(attributes.getValueAsString(GlyphSetGlyphsAttribute));

			for (String::size_type i = 0; i < glyphs.length(); ++i)
			{
				utf32 codepoint = glyphs[i];

				if (d_glyphSet.find(codepoint) == String::npos)
				{
					d_glyphSet.append(1, codepoint);
				}

			}

		}
		else
		{
			Logger::getSingleton().logEvent((utf8*)"GlyphSet element encountered.  This element is invalid for static fonts.", Informative);
		}
	}
	// anything else is an error which *should* have already been caught by XML validation
	else
	{
		throw FileIOException("Font::xmlHandler::startElement - Unexpected data was found while parsing the Font file: '" + element + "' is unknown.");
	}

}

void Font_xmlHandler::elementEnd(const String& element)
{
	if (element == FontElement)
	{
		// if this is a freetype based font, perform glyph definition
		if (d_font->d_freetype)
		{
			d_font->defineFontGlyphs(d_glyphSet);
		}

		Logger::getSingleton().logEvent("Finished creation of Font '" + d_font->d_name + "' via XML file.", Informative);
	}

}

} // End of  CEGUI namespace section
