/***********************************************************************
    created:    14/6/2006
    author:     Andrew Zabolotny
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
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

#include "CEGUI/PixmapFont.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/BasicImage.h"
#include "CEGUI/Font_xmlHandler.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/Logger.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
static const String BuiltInResourceGroup ("*");

//----------------------------------------------------------------------------//
PixmapFont::PixmapFont(const String& font_name, const String& imageset_filename,
                       const String& resource_group,
                       const AutoScaledMode auto_scaled,
                       const Sizef& native_res):
    Font(font_name, Font_xmlHandler::FontTypePixmap, imageset_filename,
         resource_group, auto_scaled, native_res),
    d_origHorzScaling(1.0f),
    d_imagesetOwner(false)
{
    addPixmapFontProperties();

    reinit();
    updateFont();
}

//----------------------------------------------------------------------------//
PixmapFont::~PixmapFont()
{
    if (d_imagesetOwner)
        ImageManager::getSingleton().destroyImageCollection(d_imageNamePrefix);
}

//----------------------------------------------------------------------------//
void PixmapFont::addPixmapFontProperties ()
{
    const String propertyOrigin("PixmapFont");

    CEGUI_DEFINE_PROPERTY(PixmapFont, String,
        "ImageNamePrefix",
        "This is the name prefix used by the images that contain the glyph "
        "imagery for this font.",
        &PixmapFont::setImageNamePrefix, &PixmapFont::getImageNamePrefix, ""
    );

    CEGUI_DEFINE_PROPERTY(PixmapFont, String,
        "Mapping",
        "This is the glyph-to-image mapping font property. It cannot be read. "
        "Format is: codepoint,advance,imagename",
        &PixmapFont::defineMapping, 0, ""
    );
}

//----------------------------------------------------------------------------//
void PixmapFont::reinit()
{
    if (d_imagesetOwner)
        ImageManager::getSingleton().destroyImageCollection(d_imageNamePrefix);

    if (d_resourceGroup == BuiltInResourceGroup)
    {
        d_imageNamePrefix = d_filename;
        d_imagesetOwner = false;
    }
    else
    {
        ImageManager::getSingleton().loadImageset(d_filename, d_resourceGroup);
        // here we assume the imageset name will match the font name
        d_imageNamePrefix = d_name; 
        d_imagesetOwner = true;
    }
}

//----------------------------------------------------------------------------//
void PixmapFont::updateFont()
{
    const float factor = (d_autoScaled != ASM_Disabled ? d_horzScaling : 1.0f) / d_origHorzScaling;

    d_ascender = 0;
    d_descender = 0;
    d_height = 0;
    d_maxCodepoint = 0;

    for (CodepointMap::iterator i = d_cp_map.begin(); i != d_cp_map.end(); ++i)
    {
        if (i->first > d_maxCodepoint)
            d_maxCodepoint = i->first;

        i->second.setAdvance(i->second.getAdvance() * factor);

        Image* img = i->second.getImage();

        BasicImage* bi = dynamic_cast<BasicImage*>(img);
        if (bi)
        {
            bi->setAutoScaled(d_autoScaled);
            bi->setNativeResolution(d_nativeResolution);
        }

        if (img->getRenderedOffset().d_y < d_ascender)
            d_ascender = img->getRenderedOffset().d_y;
        if (img->getRenderedSize().d_height + img->getRenderedOffset().d_y > d_descender)
            d_descender = img->getRenderedSize().d_height + img->getRenderedOffset().d_y;
    }

    d_ascender = -d_ascender;
    d_descender = -d_descender;
    d_height = d_ascender - d_descender;

    d_origHorzScaling = d_autoScaled != ASM_Disabled ? d_horzScaling : 1.0f;
}

//----------------------------------------------------------------------------//
void PixmapFont::writeXMLToStream_impl (XMLSerializer& xml_stream) const
{
    float advscale = 1.0f / d_origHorzScaling;
    for (CodepointMap::const_iterator i = d_cp_map.begin(); i != d_cp_map.end(); ++i)
    {
        xml_stream.openTag("Mapping")
            .attribute(Font_xmlHandler::MappingCodepointAttribute,
                       PropertyHelper<uint>::toString(i->first))
            .attribute(Font_xmlHandler::MappingHorzAdvanceAttribute,
                       PropertyHelper<float>::toString(i->second.getAdvance() * advscale))
            .attribute(Font_xmlHandler::MappingImageAttribute,
                       i->second.getImage()->getName());

        xml_stream.closeTag();
    }
}

//----------------------------------------------------------------------------//
void PixmapFont::defineMapping(const utf32 codepoint, const String& image_name,
                               const float horz_advance)
{
     Image& image(
        ImageManager::getSingleton().get(d_imageNamePrefix + '/' + image_name));

    float adv = (horz_advance == -1.0f) ?
        (float)(int)(image.getRenderedSize().d_width + image.getRenderedOffset().d_x) :
        horz_advance;

    if (d_autoScaled != ASM_Disabled)
        adv *= d_origHorzScaling;

    if (codepoint > d_maxCodepoint)
        d_maxCodepoint = codepoint;

    // create a new FontGlyph with given character code
    const FontGlyph glyph(adv, &image, true);

    if (image.getRenderedOffset().d_y < -d_ascender)
        d_ascender = -image.getRenderedOffset().d_y;
    if (image.getRenderedSize().d_height + image.getRenderedOffset().d_y > -d_descender)
        d_descender = -(image.getRenderedSize().d_height + image.getRenderedOffset().d_y);

    d_height = d_ascender - d_descender;

    // add glyph to the map
    d_cp_map[codepoint] = glyph;
}

//----------------------------------------------------------------------------//
void PixmapFont::defineMapping(const String& value)
{
    char img[33];
    utf32 codepoint;
    float adv;
    if (sscanf (value.c_str(), " %u , %g , %32s", &codepoint, &adv, img) != 3)
        CEGUI_THROW(InvalidRequestException(
            "Bad glyph Mapping specified: " + value));
    
    defineMapping(codepoint, img, adv);
}

//----------------------------------------------------------------------------//
const String& PixmapFont::getImageNamePrefix() const
{
    return d_imageNamePrefix;
}

//----------------------------------------------------------------------------//
void PixmapFont::setImageNamePrefix(const String& name_prefix)
{
    d_resourceGroup = "*";
    d_imageNamePrefix = name_prefix;
    reinit();
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
