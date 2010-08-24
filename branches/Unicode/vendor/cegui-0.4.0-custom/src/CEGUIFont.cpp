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
#include "CEGUIFont.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "CEGUIFontManager.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUITexture.h"
#include "CEGUILogger.h"
#include "CEGUITextUtils.h"
#include "CEGUIFont_xmlHandler.h"
#include "CEGUIFont_implData.h"
#include "CEGUIResourceProvider.h"
#include "CEGUIXMLParser.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <sstream>
#include "time.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	static data definitions
*************************************************************************/
const argb_t Font::DefaultColour					= 0xFFFFFFFF;
const uint	Font::InterGlyphPadSpace			= 2;

// XML related strings
const char	Font::FontSchemaName[]				= "Font.xsd";


/*************************************************************************
	Constructs a new Font object from a font definition file
*************************************************************************/
Font::Font(const String& filename, const String& resourceGroup, FontImplData* dat) :
    d_glyph_images(0),
    d_freetype(false),
    d_lineHeight(0),
    d_lineSpacing(0),
    d_max_bearingY(0),
    d_maxGlyphHeight(0),
    d_impldat(dat),
    d_ptSize(0),
    d_autoScale(false),
    d_horzScaling(1.0f),
    d_vertScaling(1.0f),
    d_nativeHorzRes(DefaultNativeHorzRes),
    d_nativeVertRes(DefaultNativeVertRes),
    d_antiAliased(false)
{
	load(filename, resourceGroup);

	// complete y spacing set-up for bitmap / static fonts
	calculateStaticVertSpacing();
}


/*************************************************************************
	Constructs a new Font object (via FreeType & a true-type font file)
	'glyph-set' describes the set of code points to be available via
	this font
*************************************************************************/
Font::Font(const String& name, const String& fontname, const String& resourceGroup, uint size, uint flags, const String& glyph_set, FontImplData* dat) :
    d_glyph_images(0),
    d_freetype(false),
    d_lineHeight(0),
    d_lineSpacing(0),
    d_max_bearingY(0),
    d_maxGlyphHeight(0),
    d_impldat(dat),
    d_ptSize(0),
    d_autoScale(false),
    d_horzScaling(1.0f),
    d_vertScaling(1.0f),
    d_nativeHorzRes(DefaultNativeHorzRes),
    d_nativeVertRes(DefaultNativeVertRes),
    d_antiAliased(false)
{
	constructor_impl(name, fontname, resourceGroup, size, flags, glyph_set);
}


/*************************************************************************
	Constructs a new Font object (via FreeType & a true-type font file)
	[first_code_point, last_code_point] describes the range of code
	points to be available via this font
*************************************************************************/
Font::Font(const String& name, const String& fontname, const String& resourceGroup, uint size, uint flags, utf32 first_code_point, utf32 last_code_point, FontImplData* dat) :
    d_glyph_images(0),
    d_freetype(false),
    d_lineHeight(0),
    d_lineSpacing(0),
    d_max_bearingY(0),
    d_maxGlyphHeight(0),
    d_impldat(dat),
    d_ptSize(0),
    d_autoScale(false),
    d_horzScaling(1.0f),
    d_vertScaling(1.0f),
    d_nativeHorzRes(DefaultNativeHorzRes),
    d_nativeVertRes(DefaultNativeVertRes),
    d_antiAliased(false)
{
	String tmp;

	for (utf32 cp = first_code_point; cp <= last_code_point; ++cp)
	{
		tmp += cp;
	}

	constructor_impl(name, fontname, resourceGroup, size, flags, tmp);
}


/*************************************************************************
	Constructs a new Font object (via FreeType & a true-type font file)
	The font file will provide support for 7-bit ASCII characters only
*************************************************************************/
Font::Font(const String& name, const String& fontname, const String& resourceGroup, uint size, uint flags, FontImplData* dat, bool bAutoScale, float fNativeResX, float fNativeResY) :
    d_glyph_images(0),
    d_freetype(false),
    d_lineHeight(0),
    d_lineSpacing(0),
    d_max_bearingY(0),
    d_maxGlyphHeight(0),
    d_impldat(dat),
    d_ptSize(0),
    d_autoScale(bAutoScale),
    d_horzScaling(1.0f),
    d_vertScaling(1.0f),
    d_nativeHorzRes(fNativeResX),
    d_nativeVertRes(fNativeResY),
    d_antiAliased(false)
{
	String tmp;

	for (utf32 cp = 32; cp <= 127; ++cp)
	{
		tmp += cp;
	}

	Size screensize = System::getSingleton().getRenderer()->getSize();
	d_horzScaling = screensize.d_width / d_nativeHorzRes;
	d_vertScaling = screensize.d_height / d_nativeVertRes;

	constructor_impl(name, fontname, resourceGroup, size, flags, tmp);
}


/*************************************************************************
	Destroys a Font object
*************************************************************************/
Font::~Font(void)
{
	unload();
	delete d_impldat;
}


/*************************************************************************
	Return the pixel width of the specified text if rendered with this Font.
*************************************************************************/
float Font::getTextExtent(const String& text, float x_scale) const
{
    float cur_extent = 0, adv_extent = 0, width;

    size_t char_count = text.length();
    CodepointMap::const_iterator pos, end = d_cp_map.end();

    for (size_t c = 0; c < char_count; ++c)
    {
        pos = d_cp_map.find(text[c]);

        if (pos != end)
        {
            width = (pos->second.d_image->getWidth() + pos->second.d_image->getOffsetX()) * x_scale;
            
            if (adv_extent + width > cur_extent)
            {
                cur_extent = adv_extent + width;
            }
            
            adv_extent += (float)pos->second.d_horz_advance * x_scale;
        }

    }

    return ceguimax(adv_extent, cur_extent);
}


/*************************************************************************
	Return the index of the closest text character in String 'text' that
	corresponds to pixel location 'pixel' if the text were rendered.
*************************************************************************/
size_t Font::getCharAtPixel(const String& text, size_t start_char, float pixel, float x_scale) const
{
	float cur_extent = 0;
	size_t char_count = text.length();

	// handle simple cases
	if ((pixel <= 0) || (char_count <= start_char))
	{
		return start_char;
	}

	CodepointMap::const_iterator	pos, end = d_cp_map.end();

	for (size_t c = start_char; c < char_count; ++c)
	{
		pos = d_cp_map.find(text[c]);

		if (pos != end)
		{
			cur_extent += (float)pos->second.d_horz_advance * x_scale;

			if (pixel < cur_extent)
			{
				return c;
			}

		}

	}

	return char_count;
}


/*************************************************************************
	Renders text on the display.  Return number of lines output.
*************************************************************************/
size_t Font::drawText(const String& text, const Rect& draw_area, float z, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale, float y_scale) const
{
	size_t thisCount;
	size_t lineCount = 0;

	float	y_base = draw_area.d_top + getBaseline(y_scale);

	Rect tmpDrawArea(
		PixelAligned(draw_area.d_left),
		PixelAligned(draw_area.d_top),
		PixelAligned(draw_area.d_right),
		PixelAligned(draw_area.d_bottom)
		);

	size_t lineStart = 0, lineEnd = 0;
	String	currLine;

	while (lineEnd < text.length())
	{
		if ((lineEnd = text.find_first_of('\n', lineStart)) == String::npos)
		{
			lineEnd = text.length();
		}

		currLine = text.substr(lineStart, lineEnd - lineStart);
		lineStart = lineEnd + 1;	// +1 to skip \n char

		switch(fmt)
		{
		case LeftAligned:
			drawTextLine(currLine, Vector3(tmpDrawArea.d_left, y_base, z), clip_rect, colours, x_scale, y_scale);
			thisCount = 1;
			y_base += getLineSpacing(y_scale);
			break;

		case RightAligned:
			drawTextLine(currLine, Vector3(tmpDrawArea.d_right - getTextExtent(currLine, x_scale), y_base, z), clip_rect, colours, x_scale, y_scale);
			thisCount = 1;
			y_base += getLineSpacing(y_scale);
			break;

		case Centred:
			drawTextLine(currLine, Vector3(PixelAligned(tmpDrawArea.d_left + ((tmpDrawArea.getWidth() - getTextExtent(currLine, x_scale)) / 2.0f)), y_base, z), clip_rect, colours, x_scale, y_scale);
			thisCount = 1;
			y_base += getLineSpacing(y_scale);
			break;

		case Justified:
			// new function in order to keep drawTextLine's signature unchanged
			drawTextLineJustified(currLine, draw_area, Vector3(tmpDrawArea.d_left, y_base, z), clip_rect, colours, x_scale, y_scale);
			thisCount = 1;
			y_base += getLineSpacing(y_scale);
			break;

		case WordWrapLeftAligned:
			thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, LeftAligned, colours, x_scale, y_scale);
			tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
			break;

		case WordWrapRightAligned:
			thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, RightAligned, colours, x_scale, y_scale);
			tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
			break;

		case WordWrapCentred:
			thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, Centred, colours, x_scale, y_scale);
			tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
			break;

		case WordWrapJustified:
			// no change needed
			thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, Justified, colours, x_scale, y_scale);
			tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
			break;

		default:
			throw InvalidRequestException((utf8*)"Font::drawText - Unknown or unsupported TextFormatting value specified.");
		}

		lineCount += thisCount;

	}

    // should not return 0
    return ceguimax(lineCount, (size_t)1);
}


/*************************************************************************
	Define the set of glyphs available for this font
*************************************************************************/
void Font::defineFontGlyphs(const String& glyph_set)
{
	d_glyphset = glyph_set;
	defineFontGlyphs_impl();

	Logger::getSingleton().logEvent("Font '" + d_name + "' now has the following glyphs defined: '" + d_glyphset + "'.", Informative);
}


/*************************************************************************
	Return the required texture size required to store imagery for the
	glyphs specified in 'glyph_set'
*************************************************************************/
uint Font::getRequiredTextureSize(const String& glyph_set)
{
	d_maxGlyphHeight = 0;

	uint	texSize = 32;			// start with a texture this size
	uint	width;

	uint	cur_x = 0;
	uint	cur_y = d_maxGlyphHeight;

	uint	glyph_set_length = static_cast<uint>(glyph_set.length());

    FT_GlyphSlot thisGlyph = d_impldat->fontFace->glyph;

	for (uint i = 0; i < glyph_set_length; ++i)
	{
        FT_GlyphSlot glyph = thisGlyph;

		// load-up required glyph
        if (!FT_Get_Char_Index( d_impldat->fontFace, glyph_set[i] ) )
        {
			//We couldnt find a glyph for this font.  Load it from the substitute font.
            glyph = (FT_GlyphSlot)FontManager::getSingleton().getSubstituteGlyph ( glyph_set[i] );
        }
		else if (FT_Load_Char(d_impldat->fontFace, glyph_set[i], FT_LOAD_RENDER | (d_antiAliased ? FT_LOAD_TARGET_NORMAL : FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO)))
		{
			// skip errors
			continue;
		}

		// if this glyph is taller than all others so far, update height and re-calculate cur_y
		if ((uint)glyph->bitmap.rows + InterGlyphPadSpace > d_maxGlyphHeight)
		{
			d_maxGlyphHeight = glyph->bitmap.rows + InterGlyphPadSpace;
			cur_y = (i + 1) * d_maxGlyphHeight;
		}

		width = glyph->bitmap.width + InterGlyphPadSpace;
		cur_x += width;

		// check for fit
		if (cur_x >= texSize)
		{
			cur_x = width;
			cur_y += d_maxGlyphHeight;

			if (cur_y >= texSize)
			{
				// texture is too small, set-up to start again...
				texSize *= 2;
				cur_x = 0;
				cur_y = d_maxGlyphHeight;
				i = (uint)-1;
			}

		}

	}

	return texSize;
}


/*************************************************************************
	Render a set of glyph images into the given memory buffer.
	pixels will be in A8R8G8B8 format
*************************************************************************/
void Font::createFontGlyphSet(const String& glyph_set, uint size, argb_t* buffer)
{
	String	imageName;
	Rect	rect;
	Point	offset;

	FT_GlyphSlot thisGlyph = d_impldat->fontFace->glyph;

	d_max_bearingY = 0;

	size_t	glyph_set_length = glyph_set.length();
	uint	cur_x = 0;
	uint	cur_y = 0;
	uint	width;

	for (uint i = 0; i < glyph_set_length; ++i)
	{
        FT_GlyphSlot glyph = thisGlyph;

        //Check for repetitions
	    CodepointMap::const_iterator	pos, end = d_cp_map.end();
        pos = d_cp_map.find(glyph_set[i]);
        if (pos != end)
            continue;

        // Check if the glyph exists in this font
        if (!FT_Get_Char_Index( d_impldat->fontFace, glyph_set[i] ) )
        {
			//We couldnt find a glyph for this font.  Load it from the substitute font.
            glyph = (FT_GlyphSlot)FontManager::getSingleton().getSubstituteGlyph ( glyph_set[i] );
        }
        else if (FT_Load_Char(d_impldat->fontFace, glyph_set[i], FT_LOAD_RENDER | (d_antiAliased ? FT_LOAD_TARGET_NORMAL : FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO)))
		{
			// skip errors (but now we log them!)
			std::stringstream err;
			err << "Font::createFontGlyphSet - Failed to load glyph for codepoint: ";
			err << static_cast<unsigned int>(glyph_set[i]);
			Logger::getSingleton().logEvent(err.str(), Errors);

			continue;
		}

		width = glyph->bitmap.width + InterGlyphPadSpace;

		// see if we need to wrap to next row
		if (cur_x + width >= size)
		{
			cur_x = 0;
			cur_y += d_maxGlyphHeight;
		}

		// calculate offset into buffer for this glyph
		argb_t* dest_buff = buffer + (cur_y * size) + cur_x;

		// draw glyph into buffer
		drawGlyphToBuffer((void*)glyph,dest_buff, size);

		// define Image on Imageset for this glyph to save re-rendering glyph later
        char* b    =  new char[];
        sprintf(b,"%u",i);
        imageName		= ("glyph_" + d_name + "_" + String(b));
		rect.d_left		= (float)cur_x;
		rect.d_top		= (float)cur_y;
		rect.d_right	= (float)(cur_x + width - InterGlyphPadSpace);
		rect.d_bottom	= (float)(cur_y + d_maxGlyphHeight - InterGlyphPadSpace);

		offset.d_x		= (float)(glyph->metrics.horiBearingX >> 6);
		offset.d_y		= -(float)(glyph->metrics.horiBearingY >> 6);

		d_glyph_images->defineImage(imageName, rect, offset,glyph_set[i]);

		cur_x += width;

//		// check and update maximum bearingY value
//		if (static_cast<float>(glyph->metrics.horiBearingY >> 6) > d_max_bearingY)
//		{
//			d_max_bearingY = static_cast<float>(glyph->metrics.horiBearingY >> 6);
//		}

		// create entry in code-point to Image map
		glyphDat	dat;
		dat.d_image = &d_glyph_images->getImage(imageName);
        const Image* image = dat.d_image;
		dat.d_horz_advance = glyph->advance.x >> 6;
		d_cp_map[glyph_set[i]] = dat;
	}

}


/*************************************************************************
	Render a range of glyph images into the given memory buffer.
	pixels will be in A8R8G8B8 format
*************************************************************************/
void Font::createFontGlyphSet(utf32 first_code_point, utf32 last_code_point, uint size, argb_t* buffer)
{
	String tmp;

	for (utf32 cp = first_code_point; cp <=last_code_point; ++cp)
	{
		tmp += cp;
	}

	createFontGlyphSet(tmp, size, buffer);
}


/*************************************************************************
	Return the required texture size required to store imagery for the
	glyphs specified in by the inclusive range
	[first_code_point, last_code_point]
*************************************************************************/
uint Font::getRequiredTextureSize(utf32 first_code_point, utf32 last_code_point)
{
	String tmp;

	for (utf32 cp = first_code_point; cp <=last_code_point; ++cp)
	{
		tmp += cp;
	}

	return getRequiredTextureSize(tmp);
}


/*************************************************************************
	Define the range of glyphs available for this font
*************************************************************************/
void Font::defineFontGlyphs(utf32 first_code_point, utf32 last_code_point)
{
	String tmp;

	for (utf32 cp = first_code_point; cp <=last_code_point; ++cp)
	{
		tmp += cp;
	}

	defineFontGlyphs(tmp);
}


/*************************************************************************
	Copy the FreeType glyph bitmap into the given memory buffer
*************************************************************************/
void Font::drawGlyphToBuffer(void* glyph, argb_t* buffer, uint buf_width)
{
    FT_GlyphSlot thisGlyph = (FT_GlyphSlot)glyph;
	FT_Bitmap* glyph_bitmap = &thisGlyph->bitmap;

	for (int i = 0; i < glyph_bitmap->rows; ++i)
	{
		for (int j = 0; j < glyph_bitmap->width; ++j)
		{
			switch (glyph_bitmap->pixel_mode)
			{
			case FT_PIXEL_MODE_GRAY:
				{
					uchar* bytebuff = reinterpret_cast<uchar*>(&buffer[j]);
					*bytebuff++ = 0xFF;
					*bytebuff++ = 0xFF;
					*bytebuff++ = 0xFF;
					*bytebuff = glyph_bitmap->buffer[(i * glyph_bitmap->pitch) + j];
				}
				break;

			case FT_PIXEL_MODE_MONO:
				buffer[j] = ((glyph_bitmap->buffer[(i * glyph_bitmap->pitch) + j / 8] << (j % 8)) & 0x80) ? 0xFFFFFFFF : 0x00000000;
				break;

			default:
				throw InvalidRequestException((utf8*)"Font::drawGlyphToBuffer - The glyph could not be drawn because the pixel mode is unsupported.");
				break;
			}

		}

		buffer += buf_width;
	}

}


/*************************************************************************
	draws wrapped text
*************************************************************************/
size_t Font::drawWrappedText(const String& text, const Rect& draw_area, float z, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale, float y_scale) const
{
	size_t	line_count = 0;
	Rect	dest_area(draw_area);
	float	wrap_width = draw_area.getWidth();

	String  whitespace = TextUtils::DefaultWhitespace;
	String	thisLine, thisWord;
	size_t	currpos = 0;

	// get first word.
	currpos += getNextWord(text, currpos, thisLine);

	// while there are words left in the string...
	while (String::npos != text.find_first_not_of(whitespace, currpos)) {
		// get next word of the string...
		currpos += getNextWord(text, currpos, thisWord);

		// if the new word would make the string too long
		if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width) {
			// output what we had until this new word
			line_count += drawText(thisLine, dest_area, z, clip_rect, fmt, colours, x_scale, y_scale);

			// remove whitespace from next word - it will form start of next line
			thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));

			// reset for a new line.
			thisLine.clear();

			// update y co-ordinate for next line
			dest_area.d_top += getLineSpacing(y_scale);
		}

		// add the next word to the line
		thisLine += thisWord;
	}

	// Last line is left aligned
	TextFormatting last_fmt = (fmt == Justified ? LeftAligned : fmt);
	// output last bit of string
	line_count += drawText(thisLine, dest_area, z, clip_rect, last_fmt, colours, x_scale, y_scale);

	return line_count;
}


/*************************************************************************
	helper function for renderWrappedText to get next word of a string
*************************************************************************/
size_t Font::getNextWord(const String& in_string, size_t start_idx, String& out_string) const
{
	out_string = TextUtils::getNextWord(in_string, start_idx, TextUtils::DefaultWrapDelimiters);

	return out_string.length();
}


/*************************************************************************
	Draw a line of text.  No formatting is applied.
*************************************************************************/
void Font::drawTextLine(const String& text, const Vector3& position, const Rect& clip_rect, const ColourRect& colours, float x_scale, float y_scale) const
{
	Vector3	cur_pos(position);

	float base_y = position.d_y;

	size_t char_count = text.length();
	CodepointMap::const_iterator	pos, end = d_cp_map.end();

    String strNewGlyphs = cleanGlyphCache(d_glyphset);  //Purge unused glyphs
	for (size_t c = 0; c < char_count; ++c)
	{
        strNewGlyphs += OnGlyphDrawn(text[c],true);
    }
    if ( strNewGlyphs != d_glyphset )
    {
        const_cast<Font*>( this )->defineFontGlyphs(strNewGlyphs);
    }

	for (size_t c = 0; c < char_count; ++c)
	{
		pos = d_cp_map.find(text[c]);

		if (pos != end)
		{
			const Image* img = pos->second.d_image;
			cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);
			Size sz(img->getWidth() * x_scale, img->getHeight() * y_scale);
			img->draw(cur_pos, sz, clip_rect, colours);
			cur_pos.d_x += (float)pos->second.d_horz_advance * x_scale;
		}

	}
}


/*************************************************************************
	Draw a justified line of text.
*************************************************************************/
void Font::drawTextLineJustified(const String& text, const Rect& draw_area, const Vector3& position, const Rect& clip_rect, const ColourRect& colours, float x_scale, float y_scale) const
{
	Vector3	cur_pos(position);

	float base_y = position.d_y;
	// Calculate the length difference between the justified text and the same text, left aligned
	// This space has to be shared between the space characters of the line
	float lost_space = getFormattedTextExtent(text, draw_area, Justified, x_scale) - getTextExtent(text, x_scale);

	size_t char_count = text.length();
	CodepointMap::const_iterator	pos, end = d_cp_map.end();

	// The number of spaces and tabs in the current line
	uint space_count = 0;
    size_t c;
	for (c = 0; c < char_count; ++c)
		if ((text[c] == ' ') || (text[c] == '\t')) ++space_count;

	// The width that must be added to each space character in order to transform the left aligned text in justified text
	float shared_lost_space = 0.0;
	if (space_count > 0) shared_lost_space = lost_space / (float)space_count;

    //First ensure our glyphs are loaded
    String strNewGlyphs = cleanGlyphCache(d_glyphset);  //Purge unused glyphs
	for (size_t c = 0; c < char_count; ++c)
	{
        strNewGlyphs += OnGlyphDrawn(text[c],true);
    }
    if ( strNewGlyphs != d_glyphset )
    {
        const_cast<Font*>( this )->defineFontGlyphs(strNewGlyphs);
    }
    
    for (c = 0; c < char_count; ++c)
	{
		pos = d_cp_map.find(text[c]);

		if (pos != end)
		{
			const Image* img = pos->second.d_image;
			cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);
			Size sz(img->getWidth() * x_scale, img->getHeight() * y_scale);
			img->draw(cur_pos, sz, clip_rect, colours);
			cur_pos.d_x += (float)pos->second.d_horz_advance * x_scale;
			// That's where we adjust the size of each space character
			if ((text[c] == ' ') || (text[c] == '\t')) cur_pos.d_x += shared_lost_space;
		}

	}

}


/*************************************************************************
	Function to do real work of constructor
*************************************************************************/
void Font::constructor_impl(const String& name, const String& fontname, const String& resourceGroup, uint size, uint flags, const String& glyph_set)
{
	ImagesetManager& ismgr	= ImagesetManager::getSingleton();

    // pull a-a setting from flags
	d_antiAliased = (flags == NoAntiAlias) ? false : true;

	// create an blank Imageset
	d_glyph_images = ismgr.createImageset(name + "_auto_glyph_images", System::getSingleton().getRenderer()->createTexture());

	uint		horzdpi		= System::getSingleton().getRenderer()->getHorzScreenDPI();
	uint		vertdpi		= System::getSingleton().getRenderer()->getVertScreenDPI();
	String		errMsg;

    System::getSingleton().getResourceProvider()->loadRawDataContainer(fontname, d_impldat->fontData, resourceGroup);

	// create face using input font
	if (FT_New_Memory_Face(d_impldat->library, d_impldat->fontData.getDataPtr(), 
                (FT_Long)d_impldat->fontData.getSize(), 0, &d_impldat->fontFace) == 0)
	{
		// check that default Unicode character map is available
		if (d_impldat->fontFace->charmap != NULL)	
		{
			try
			{
				d_glyphset = glyph_set;
				d_name = name;
				d_freetype = true;
				createFontFromFT_Face(size, horzdpi, vertdpi);
				return;
			}
			catch(...)
			{
				ismgr.destroyImageset(d_glyph_images);
				d_glyph_images = NULL;

				FT_Done_Face(d_impldat->fontFace);
				d_freetype = false;

				// re-throw
				throw;
			}

		}
		// missing Unicode character map
		else
		{
			FT_Done_Face(d_impldat->fontFace);
			d_freetype = false;

			errMsg = (utf8*)"Font::constructor_impl - The source font '" + fontname +"' does not have a Unicode charmap, and cannot be used.";
		}

	}
	// failed to create face (a problem with the font file?)
	else
	{
		errMsg = (utf8*)"Font::constructor_impl - An error occurred while trying to create a FreeType face from source font '" + fontname + "'.";
	}

	// cleanup image set we created here
	ismgr.destroyImageset(d_glyph_images);

	throw GenericException(errMsg);
}


/*************************************************************************
	Load and complete construction of 'this' via an XML file
*************************************************************************/
void Font::load(const String& filename, const String& resourceGroup)
{
	// unload old data
	unload();

	if (filename.empty() || (filename == (utf8*)""))
	{
		throw InvalidRequestException((utf8*)"Font::load - Filename supplied for Font loading must be valid");
	}

    // create handler object
    Font_xmlHandler handler(this);

	// do parse (which uses handler to create actual data)
	try
	{
        System::getSingleton().getXMLParser()->parseXMLFile(handler, filename, FontSchemaName, resourceGroup);
	}
	catch(...)
	{
		unload();

        Logger::getSingleton().logEvent("Font::load - loading of Font from file '" + filename +"' failed.", Errors);
		throw;
	}

}


/*************************************************************************
	Unload data associated with the font (font is then useless.
	this is intended for cleanup).
*************************************************************************/
void Font::unload(void)
{
	d_cp_map.clear();

	// cleanup Imageset if it's valid
	if (d_glyph_images != NULL)
	{
		ImagesetManager::getSingleton().destroyImageset(d_glyph_images);
		d_glyph_images = NULL;
	}

	// cleanup FreeType face if this is a FreeType based font.
	if (d_freetype)
	{
		FT_Done_Face(d_impldat->fontFace);
		d_freetype = false;
	}

	System::getSingleton().getResourceProvider()->unloadRawDataContainer(d_impldat->fontData);

}


/*************************************************************************
	Defines the set of code points on the font. (implementation).
*************************************************************************/
void Font::defineFontGlyphs_impl(void)
{
	// must be a font using the FreeType system
	if (!d_freetype)
	{
		throw InvalidRequestException((utf8*)"Font::defineFontGlyphs_impl - operation not supported on bitmap based fonts.");
	}

	uint texture_size = getRequiredTextureSize(d_glyphset);

	// check renderer can do a texture big enough
	if (texture_size > System::getSingleton().getRenderer()->getMaxTextureSize())
	{
		throw	RendererException((utf8*)"Font::defineFontGlyphs_impl - operation requires a texture larger than the supported maximum.");
	}

	// allocate memory buffer where we will define the imagery
	argb_t* mem_buffer;

	try
	{
		mem_buffer = new argb_t[texture_size * texture_size];
	}
	catch (std::bad_alloc)
	{
		throw	MemoryException((utf8*)"Font::defineFontGlyphs_impl - failed to allocate required memory buffer.");
	}

	// initialise background to transparent black.
	memset(mem_buffer, 0, ((texture_size * texture_size) * sizeof(argb_t)));

	// clear old data about glyphs and images
	d_cp_map.clear();
	d_glyph_images->undefineAllImages();

	// render new glyphs and define Imageset images.
	createFontGlyphSet(d_glyphset, texture_size, mem_buffer);

	// update Imageset texture with new imagery.
	d_glyph_images->getTexture()->loadFromMemory(mem_buffer, texture_size, texture_size);

	delete[] mem_buffer;

	d_lineHeight = (float)d_maxGlyphHeight;

	// calculate spacing and base-line
    d_max_bearingY = ((float)d_impldat->fontFace->ascender / (float)d_impldat->fontFace->units_per_EM) * (float)d_impldat->fontFace->size->metrics.y_ppem;
	d_lineSpacing = ((float)d_impldat->fontFace->height / (float)d_impldat->fontFace->units_per_EM) * (float)d_impldat->fontFace->size->metrics.y_ppem;
}


/*************************************************************************
	Calculate the vertical spacing fields for a static / bitmap font
*************************************************************************/
void Font::calculateStaticVertSpacing(void)
{
	if (!d_freetype)
	{
		float scale = d_autoScale ? d_vertScaling : 1.0f;

		d_lineHeight	= 0;
		d_max_bearingY	= 0;

		CodepointMap::iterator pos = d_cp_map.begin(), end = d_cp_map.end();

		for (;pos != end; ++pos)
		{
			const Image* img = pos->second.d_image;

			if (-img->getOffsetY() > d_max_bearingY)
				d_max_bearingY = -img->getOffsetY();

			if (img->getHeight() > d_lineHeight)
				d_lineHeight = img->getHeight();
		}

		d_max_bearingY *= scale;
		d_lineHeight *= scale;
		d_lineSpacing = d_lineHeight;
        d_maxGlyphHeight = static_cast<uint>(d_lineHeight);
	}

}


/*************************************************************************
	Set the native resolution for this Font
*************************************************************************/
void Font::setNativeResolution(const Size& size)
{
	d_nativeHorzRes = size.d_width;
	d_nativeVertRes = size.d_height;

	// set native resolution for underlying imageset for bitmap fonts
	if ((!d_freetype) && (d_glyph_images != NULL))
	{
		d_glyph_images->setNativeResolution(size);
	}

	// re-calculate scaling factors & notify images as required
	notifyScreenResolution(System::getSingleton().getRenderer()->getSize());
}


/*************************************************************************
	Notify the Font of the current (usually new) display resolution.
*************************************************************************/
void Font::notifyScreenResolution(const Size& size)
{
	// notification should come from System which would have notified the Imageset anyway, but
	// in case client code decides to call us, we must pass on notification here too.
	if (d_glyph_images)
	{
		d_glyph_images->notifyScreenResolution(size);
	}

	d_horzScaling = size.d_width / d_nativeHorzRes;
	d_vertScaling = size.d_height / d_nativeVertRes;

	if (d_autoScale)
	{
		updateFontScaling();
	}

}


/*************************************************************************
	Enable or disable auto-scaling for this Font.
*************************************************************************/
void Font::setAutoScalingEnabled(bool setting)
{
	if (setting != d_autoScale)
	{
		if ((!d_freetype) && (d_glyph_images != NULL))
		{
			d_glyph_images->setAutoScalingEnabled(setting);
		}

		d_autoScale = setting;
		updateFontScaling();
	}

}


/*************************************************************************
	Update the font as required according to the current scaling
*************************************************************************/
void Font::updateFontScaling(void)
{
	if (d_freetype)
	{
		uint hdpi = System::getSingleton().getRenderer()->getHorzScreenDPI();
		uint vdpi = System::getSingleton().getRenderer()->getVertScreenDPI();

		createFontFromFT_Face(d_ptSize, hdpi, vdpi);
	}
	// bitmapped font
	else
	{
		float hscale = d_autoScale ? d_horzScaling : 1.0f;

		// perform update on font mapping advance values
		CodepointMap::iterator pos = d_cp_map.begin(), end = d_cp_map.end();
		for (; pos != end; ++pos)
		{
			pos->second.d_horz_advance = (uint)(((float)pos->second.d_horz_advance_unscaled) * hscale);
		}

		// re-calculate height
		calculateStaticVertSpacing();
	}

}


/*************************************************************************
	Set the size of the free-type font (via d_impldat->fontFace which should already
	be setup) and render the glyphs in d_glyphset.
*************************************************************************/
void Font::createFontFromFT_Face(uint size, uint horzDpi, uint vertDpi)
{
	if (d_autoScale)
	{
		horzDpi = (uint)(((float)horzDpi) * d_horzScaling);
		vertDpi = (uint)(((float)vertDpi) * d_vertScaling);
	}

	d_ptSize = size;

	if (FT_Set_Char_Size(d_impldat->fontFace, 0, d_ptSize * 64, horzDpi, vertDpi) == 0)
	{
		defineFontGlyphs_impl();
	}
	// failed to set size for font
	else
	{
		throw GenericException((utf8*)"Font::createFontFromFT_Face - An error occurred while creating a source font with the requested size.");
	}

}


/*************************************************************************
	Return the number of lines the given text would be formatted to.	
*************************************************************************/
size_t Font::getFormattedLineCount(const String& text, const Rect& format_area, TextFormatting fmt, float x_scale) const
{
	// handle simple non-wrapped cases.
	if ((fmt == LeftAligned) || (fmt == Centred) || (fmt == RightAligned) || (fmt == Justified))
	{
		return std::count(text.begin(), text.end(), static_cast<utf8>('\n')) + 1;
	}

	// handle wraping cases
	size_t lineStart = 0, lineEnd = 0;
	String	sourceLine;

	float	wrap_width = format_area.getWidth();
	String  whitespace = TextUtils::DefaultWhitespace;
	String	thisLine, thisWord;
	size_t	line_count = 0, currpos = 0;

	while (lineEnd < text.length())
	{
		if ((lineEnd = text.find_first_of('\n', lineStart)) == String::npos)
		{
			lineEnd = text.length();
		}

		sourceLine = text.substr(lineStart, lineEnd - lineStart);
		lineStart = lineEnd + 1;

		// get first word.
		currpos = getNextWord(sourceLine, 0, thisLine);

		// while there are words left in the string...
		while (String::npos != sourceLine.find_first_not_of(whitespace, currpos))
		{
			// get next word of the string...
			currpos += getNextWord(sourceLine, currpos, thisWord);

			// if the new word would make the string too long
			if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
			{
				// too long, so that's another line of text
				line_count++;

				// remove whitespace from next word - it will form start of next line
				thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));

				// reset for a new line.
				thisLine.clear();
			}

			// add the next word to the line
			thisLine += thisWord;
		}

		// plus one for final line
		line_count++;
	}

	return line_count;
}


/*************************************************************************
	Return whether this font is anti-aliased or not.
*************************************************************************/
bool Font::isAntiAliased(void) const
{
	return d_freetype ? d_antiAliased : false;
}


/*************************************************************************
	Set whether the font is anti-aliased or not.
*************************************************************************/
void Font::setAntiAliased(bool setting)
{
	if (d_freetype && (d_antiAliased != setting))
	{
		d_antiAliased = setting;

		// regenerate font
		createFontFromFT_Face(d_ptSize, System::getSingleton().getRenderer()->getHorzScreenDPI(), System::getSingleton().getRenderer()->getVertScreenDPI());
	}

}


/*************************************************************************
	Return the horizontal pixel extent given text would be formatted to.	
*************************************************************************/
float Font::getFormattedTextExtent(const String& text, const Rect& format_area, TextFormatting fmt, float x_scale) const
{
	float lineWidth;
	float widest = 0;

	size_t lineStart = 0, lineEnd = 0;
	String	currLine;

	while (lineEnd < text.length())
	{
		if ((lineEnd = text.find_first_of('\n', lineStart)) == String::npos)
		{
			lineEnd = text.length();
		}

		currLine = text.substr(lineStart, lineEnd - lineStart);
		lineStart = lineEnd + 1;	// +1 to skip \n char

		switch(fmt)
		{
		case Centred:
		case RightAligned:
		case LeftAligned:
			lineWidth = getTextExtent(currLine, x_scale);
			break;

		case Justified:
			// usually we use the width of the rect but we have to ensure the current line is not wider than that
			lineWidth = ceguimax(format_area.getWidth(), getTextExtent(currLine, x_scale));
			break;

		case WordWrapLeftAligned:
		case WordWrapRightAligned:
		case WordWrapCentred:
			lineWidth = getWrappedTextExtent(currLine, format_area.getWidth(), x_scale);
			break;

		case WordWrapJustified:
			// same as above
			lineWidth = ceguimax(format_area.getWidth(), getWrappedTextExtent(currLine, format_area.getWidth(), x_scale));
			break;

		default:
			throw InvalidRequestException((utf8*)"Font::getFormattedTextExtent - Unknown or unsupported TextFormatting value specified.");
		}

		if (lineWidth > widest)
		{
			widest = lineWidth;
		}

	}

	return widest;
}


/*************************************************************************
	returns extent of widest line of wrapped text.
*************************************************************************/
float Font::getWrappedTextExtent(const String& text, float wrapWidth, float x_scale) const
{
	String  whitespace = TextUtils::DefaultWhitespace;
	String	thisWord;
	size_t	currpos;
	float	lineWidth, wordWidth;
	float	widest = 0;

	// get first word.
	currpos = getNextWord(text, 0, thisWord);
	lineWidth = getTextExtent(thisWord, x_scale);

	// while there are words left in the string...
	while (String::npos != text.find_first_not_of(whitespace, currpos)) {
		// get next word of the string...
		currpos += getNextWord(text, currpos, thisWord);
		wordWidth = getTextExtent(thisWord, x_scale);

		// if the new word would make the string too long
		if ((lineWidth + wordWidth) > wrapWidth) {
			
			if (lineWidth > widest)
			{
				widest = lineWidth;
			}

			// remove whitespace from next word - it will form start of next line
			thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));
			wordWidth = getTextExtent(thisWord, x_scale);

			// reset for a new line.
			lineWidth = 0;
		}

		// add the next word to the line
		lineWidth += wordWidth;
	}

	if (lineWidth > widest)
	{
		widest = lineWidth;
	}

	return widest;
}


/*************************************************************************
	Return a String object that contains the code-points that the font
	is currently configured to render.
*************************************************************************/
const String& Font::getAvailableGlyphs(void) const
{
	return d_glyphset;
}

/*************************************************************************
!Talidan!: Return whether a certain glyph in the font is being used or not
*************************************************************************/
bool Font::isGlyphBeingUsed (unsigned long ulGlyph) const
{
    /* stuff */
    return false;
}

/*************************************************************************
!Talidan!: Callback for when a glyph has just been used from this font
*************************************************************************/
String Font::OnGlyphDrawn ( unsigned long ulGlyph, bool bCreateNewCache ) const
{
    //std::map< unsigned long, unsigned long >* m_pGlyphCache = &m_GlyphCache;
    // Are we an ASCII character (always loaded), or out of range character?
    if ( ulGlyph < 128 )
        return "";

    unsigned int iRange = 128;
    // To prevent overlapping, we set limits 
    unsigned int iLowerBound = 127;
    unsigned int iUpperBound = 65534;

    std::map< unsigned long, unsigned long >::const_iterator itToErase;
    bool bErase = false;

    std::map< unsigned long, unsigned long >* m_pGlyphCache = const_cast< std::map< unsigned long, unsigned long >* >( &m_GlyphCache );

    // Let's check our cache to see if this glyph is already loaded
    for ( std::map< unsigned long, unsigned long >::const_iterator it = m_pGlyphCache->begin(); it != m_pGlyphCache->end(); ++it )
    {
        // If we're in range of another cache, we only update that cache
        if ( (unsigned int)abs(it->first - ulGlyph) <= iRange )
        {
            itToErase = it;
            bErase = true;
            break;
        }

        // Assuming this is a new glyph, we need to find boundaries to avoid overwriting glyphs
        unsigned int iterLowerBound = it->first - iRange;
        unsigned int iterUpperBound = it->first + iRange;
        // Is this upper bound closer than the previous lower bound?
        if ( ( iterUpperBound < ulGlyph ) && ((ulGlyph - iLowerBound) < (ulGlyph - iterUpperBound)) )
            iLowerBound = iterUpperBound;

        // Is this lower bound closer than the previous upper bound?
        if ( ( iterLowerBound > ulGlyph ) && ((iUpperBound - ulGlyph) > (iterLowerBound - ulGlyph)) )
            iUpperBound = iterLowerBound;
    }

    if ( bErase )
    {
        unsigned long ulGlyphToUpdate = itToErase->first;
        m_pGlyphCache->erase(itToErase);
        m_pGlyphCache->insert ( std::pair<unsigned long,unsigned long>(ulGlyphToUpdate,clock()/CLOCKS_PER_SEC) );
        return "";
    }

    if ( !bCreateNewCache )
        return "";

    // Not in any other cache, let's create a new cache
    m_pGlyphCache->insert ( std::pair<unsigned long,unsigned long>(ulGlyph,clock()/CLOCKS_PER_SEC) );


    CEGUI::String strNewCache = "";
    unsigned long i;
    for(i=(ulGlyph-iRange); i <= (ulGlyph+iRange); i++)
    {
        if ( ( i > iLowerBound ) && ( i < iUpperBound ) )
            strNewCache += (CEGUI::utf32)i;
    }
    return strNewCache;
}

String Font::cleanGlyphCache ( String strCache ) const
{
    unsigned int iRange = 128;
    unsigned int iLowerBound = 127;
    unsigned int iUpperBound = 65534;
    bool bPurged = false; //Have we purged any outdated caches?

    std::map< unsigned long, unsigned long >* m_pGlyphCache = const_cast< std::map< unsigned long, unsigned long >* >( &m_GlyphCache );
    std::vector<unsigned long> l_Outdated;
    unsigned int uiCurrentTime = (clock()/CLOCKS_PER_SEC);

    // Let's check for outdated caches
    for ( std::map< unsigned long, unsigned long >::const_iterator it = m_pGlyphCache->begin(); it != m_pGlyphCache->end(); ++it )
    {
        if ( (uiCurrentTime - it->second) > 30 ) //If outdated, add to our erase list
        {
            l_Outdated.push_back(it->first);
            bPurged = true;
        }
    }
    if ( !bPurged )
        return strCache;

    String strNewCache = strCache;
    std::vector<unsigned long>::iterator iter;
    for (iter=l_Outdated.begin(); iter!=l_Outdated.end(); iter++)
    {
        String::iterator c;
        for (c = strNewCache.begin(); c != strNewCache.end(); c++)
	    {
            unsigned long ulGlyph = (unsigned long)*c;
            if ( ( ulGlyph > iLowerBound) && ( ulGlyph < iUpperBound ) )
            {
                if ( ( (*iter-iRange) <= ulGlyph ) && ( (ulGlyph <= (*iter+iRange) ) ) )
                {
                    strNewCache.erase(c);
                    c--;
                }
            }
        }
        m_pGlyphCache->erase(*iter);
    }

    return strNewCache;
}


/*************************************************************************
	Return the point size of a dynamic (ttf based) font.
*************************************************************************/
uint Font::getPointSize(void) const
{
	if (d_freetype)
	{
		return d_ptSize;
	}
	else
	{
		throw InvalidRequestException("Font::getPointSize - unable to return point size for a static (bitmap based) font.");
	}
}


/*************************************************************************
    Writes an xml representation of this Font to \a out_stream.
*************************************************************************/
void Font::writeXMLToStream(OutStream& out_stream) const
{
    // output starting <Font ... > element
    out_stream << "<Font Name=\"" << d_name << "\" Filename=\"" << d_sourceFilename << "\" ";

    if (d_freetype)
        out_stream << "Size=\"" << d_ptSize << "\" ";

    if (d_nativeHorzRes != DefaultNativeHorzRes)
        out_stream << "NativeHorzRes=\"" << static_cast<uint>(d_nativeHorzRes) << "\" ";

    if (d_nativeVertRes != DefaultNativeVertRes)
        out_stream << "NativeVertRes=\"" << static_cast<uint>(d_nativeVertRes) << "\" ";

    if (d_autoScale)
        out_stream << "AutoScaled=\"True\" ";

    out_stream << ">" << std::endl;

    // dynamic font so output defined glyphs
    if (d_freetype)
    {
        size_t start = 0, idx = 0;

        while(start < d_glyphset.length())
        {
            // find end of range
            while ((idx + 1 < d_glyphset.length()) && (d_glyphset[idx] + 1 == d_glyphset[idx + 1]))
                ++idx;

            if (start == idx)
                // if range is a just a single codepoint
                out_stream << "<Glyph Codepoint=\"" << d_glyphset[start] << "\" />" << std::endl;
            else
                // range contains >1 codepoint
                out_stream << "<GlyphRange StartCodepoint=\"" << d_glyphset[start] << "\" EndCodepoint=\"" << d_glyphset[idx] << "\" />" << std::endl;

            start = ++idx;
        }
    }
    // static font, so output glyph to imageset mappings
    else
    {
        for (CodepointMap::const_iterator iter = d_cp_map.begin(); iter != d_cp_map.end(); ++iter)
        {
            out_stream << "<Mapping Codepoint=\"" << (*iter).first << "\" Image=\"" << (*iter).second.d_image->getName() << "\" ";

            if ((*iter).second.d_horz_advance_unscaled != -1)
                out_stream << "HorzAdvance=\"" << (*iter).second.d_horz_advance_unscaled << "\" ";

            out_stream << "/>" << std::endl;
        }
    }

    // output closing </Font> element.
    out_stream << "</Font>" << std::endl;
}


} // End of  CEGUI namespace section
