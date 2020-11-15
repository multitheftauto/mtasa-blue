/***********************************************************************
    created:    21/2/2004
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/Font.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Font_xmlHandler.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/System.h"
#include "CEGUI/Image.h"

namespace CEGUI
{
//----------------------------------------------------------------------------//
// amount of bits in a uint
#define BITS_PER_UINT   (sizeof (uint) * 8)
// must be a power of two
#define GLYPHS_PER_PAGE 256

//----------------------------------------------------------------------------//
const argb_t Font::DefaultColour = 0xFFFFFFFF;
String Font::d_defaultResourceGroup;

//----------------------------------------------------------------------------//
const String Font::EventNamespace("Font");
const String Font::EventRenderSizeChanged("RenderSizeChanged");

//----------------------------------------------------------------------------//
Font::Font(const String& name, const String& type_name, const String& filename,
           const String& resource_group, const AutoScaledMode auto_scaled,
           const Sizef& native_res):
    d_name(name),
    d_type(type_name),
    d_filename(filename),
    d_resourceGroup(resource_group),
    d_ascender(0),
    d_descender(0),
    d_height(0),
    d_autoScaled(auto_scaled),
    d_nativeResolution(native_res),
    d_maxCodepoint(0),
    d_glyphPageLoaded(0)
{
    addFontProperties();

    const Sizef size(System::getSingleton().getRenderer()->getDisplaySize());
    Image::computeScalingFactors(d_autoScaled, size, d_nativeResolution,
                                 d_horzScaling, d_vertScaling);
}

//----------------------------------------------------------------------------//
Font::~Font()
{
    if (d_glyphPageLoaded)
    {
        const uint old_size = (((d_maxCodepoint + GLYPHS_PER_PAGE) / GLYPHS_PER_PAGE)
            + BITS_PER_UINT - 1) / BITS_PER_UINT;
        #ifndef CEGUI_CUSTOM_ALLOCATORS
            CEGUI_UNUSED(old_size);
        #endif
        CEGUI_DELETE_ARRAY_PT(d_glyphPageLoaded, uint, old_size, Font);
    }
}

//----------------------------------------------------------------------------//
const String& Font::getName() const
{
    return d_name;
}

//----------------------------------------------------------------------------//
const String& Font::getTypeName() const
{
    return d_type;
}

//----------------------------------------------------------------------------//
const String& Font::getFileName() const
{
    return d_filename;
}

//----------------------------------------------------------------------------//
void Font::addFontProperties()
{
    const String propertyOrigin("Font");

    CEGUI_DEFINE_PROPERTY(Font, Sizef,
        "NativeRes", "Native screen resolution for this font."
        "Value uses the 'w:# h:#' format.",
        &Font::setNativeResolution, &Font::getNativeResolution, Sizef::zero()
    );

    CEGUI_DEFINE_PROPERTY(Font, String,
        "Name", "This is font name.  Value is a string.",
        0, &Font::getName, ""
    );

    CEGUI_DEFINE_PROPERTY(Font, AutoScaledMode,
        "AutoScaled", "This indicating whether and how to autoscale font depending on "
        "resolution.  Value can be 'false', 'vertical', 'horizontal' or 'true'.",
        &Font::setAutoScaled, &Font::getAutoScaled, ASM_Disabled
    );
}

//----------------------------------------------------------------------------//
void Font::setMaxCodepoint(utf32 codepoint)
{
    if (d_glyphPageLoaded)
    {
        const uint old_size = (((d_maxCodepoint + GLYPHS_PER_PAGE) / GLYPHS_PER_PAGE)
            + BITS_PER_UINT - 1) / BITS_PER_UINT;
        #ifndef CEGUI_CUSTOM_ALLOCATORS
            CEGUI_UNUSED(old_size);
        #endif
        CEGUI_DELETE_ARRAY_PT(d_glyphPageLoaded, uint, old_size, Font);
    }

    d_maxCodepoint = codepoint;

    const uint npages = (codepoint + GLYPHS_PER_PAGE) / GLYPHS_PER_PAGE;
    const uint size = (npages + BITS_PER_UINT - 1) / BITS_PER_UINT;

    d_glyphPageLoaded = CEGUI_NEW_ARRAY_PT(uint, size, Font);
    memset(d_glyphPageLoaded, 0, size * sizeof(uint));
}

//----------------------------------------------------------------------------//
const FontGlyph* Font::getGlyphData(utf32 codepoint) const
{
    if (codepoint > d_maxCodepoint)
        return 0;

    const FontGlyph* const glyph = findFontGlyph(codepoint);

    if (d_glyphPageLoaded)
    {
        // Check if glyph page has been rasterised
        uint page = codepoint / GLYPHS_PER_PAGE;
        uint mask = 1 << (page & (BITS_PER_UINT - 1));
        if (!(d_glyphPageLoaded[page / BITS_PER_UINT] & mask))
        {
            d_glyphPageLoaded[page / BITS_PER_UINT] |= mask;
            rasterise(codepoint & ~(GLYPHS_PER_PAGE - 1),
                      codepoint | (GLYPHS_PER_PAGE - 1));
        }
    }

    return glyph;
}

//----------------------------------------------------------------------------//
const FontGlyph* Font::findFontGlyph(const utf32 codepoint) const
{
    CodepointMap::const_iterator pos = d_cp_map.find(codepoint);
    return (pos != d_cp_map.end()) ? &pos->second : 0;
}

//----------------------------------------------------------------------------//
float Font::getTextExtent(const String& text, float x_scale) const
{
    const FontGlyph* glyph;
    float cur_extent = 0, adv_extent = 0, width;

    for (size_t c = 0; c < text.length(); ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
            width = glyph->getRenderedAdvance(x_scale);

            if (adv_extent + width > cur_extent)
                cur_extent = adv_extent + width;

            adv_extent += glyph->getAdvance(x_scale);
        }
    }

    return ceguimax(adv_extent, cur_extent);
}

//----------------------------------------------------------------------------//
float Font::getTextAdvance(const String& text, float x_scale) const
{
    float advance = 0.0f;

    for (size_t c = 0; c < text.length(); ++c)
    {
        if (const FontGlyph* glyph = getGlyphData(text[c]))
            advance += glyph->getAdvance(x_scale);
    }

    return advance;
}

//----------------------------------------------------------------------------//
size_t Font::getCharAtPixel(const String& text, size_t start_char, float pixel,
                            float x_scale) const
{
    const FontGlyph* glyph;
    float cur_extent = 0;
    size_t char_count = text.length();

    // handle simple cases
    if ((pixel <= 0) || (char_count <= start_char))
        return start_char;

    for (size_t c = start_char; c < char_count; ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
            cur_extent += glyph->getAdvance(x_scale);

            if (pixel < cur_extent)
                return c;
        }
    }

    return char_count;
}

//----------------------------------------------------------------------------//
float Font::drawText(GeometryBuffer& buffer, const String& text,
                    const Vector2f& position, const Rectf* clip_rect,
                    const ColourRect& colours, const float space_extra,
                    const float x_scale, const float y_scale) const
{
    const float base_y = position.d_y + getBaseline(y_scale);
    Vector2f glyph_pos(position);

    for (size_t c = 0; c < text.length(); ++c)
    {
        const FontGlyph* glyph;
        if ((glyph = getGlyphData(text[c]))) // NB: assignment
        {
            const Image* const img = glyph->getImage();
            glyph_pos.d_y =
                base_y - (img->getRenderedOffset().d_y - img->getRenderedOffset().d_y * y_scale);
            img->render(buffer, glyph_pos,
                      glyph->getSize(x_scale, y_scale), clip_rect, colours);
            glyph_pos.d_x += glyph->getAdvance(x_scale);
            // apply extra spacing to space chars
            if (text[c] == ' ')
                glyph_pos.d_x += space_extra;
        }
    }

    return glyph_pos.d_x;
}

//----------------------------------------------------------------------------//
void Font::setNativeResolution(const Sizef& size)
{
    d_nativeResolution = size;

    // re-calculate scaling factors & notify images as required
    notifyDisplaySizeChanged(
        System::getSingleton().getRenderer()->getDisplaySize());
}

//----------------------------------------------------------------------------//
const Sizef& Font::getNativeResolution() const
{
    return d_nativeResolution;
}

//----------------------------------------------------------------------------//
void Font::setAutoScaled(const AutoScaledMode auto_scaled)
{
    if (auto_scaled == d_autoScaled)
        return;

    d_autoScaled = auto_scaled;
    updateFont();

    FontEventArgs args(this);
    onRenderSizeChanged(args);
}

//----------------------------------------------------------------------------//
AutoScaledMode Font::getAutoScaled() const
{
    return d_autoScaled;
}

//----------------------------------------------------------------------------//
void Font::notifyDisplaySizeChanged(const Sizef& size)
{
    Image::computeScalingFactors(d_autoScaled, size, d_nativeResolution,
                                 d_horzScaling, d_vertScaling);

    if (d_autoScaled != ASM_Disabled)
    {
        updateFont();

        FontEventArgs args(this);
        onRenderSizeChanged(args);
    }
}

//----------------------------------------------------------------------------//
void Font::rasterise(utf32, utf32) const
{
    // do nothing by default
}

//----------------------------------------------------------------------------//
void Font::writeXMLToStream(XMLSerializer& xml_stream) const
{
    // output starting <Font ... > element
    xml_stream.openTag("Font")
        .attribute(Font_xmlHandler::FontNameAttribute, d_name)
        .attribute(Font_xmlHandler::FontFilenameAttribute, d_filename);

    if (!d_resourceGroup.empty())
        xml_stream.attribute(Font_xmlHandler::FontResourceGroupAttribute,
                             d_resourceGroup);

    if (d_nativeResolution.d_width != DefaultNativeHorzRes)
        xml_stream.attribute(Font_xmlHandler::FontNativeHorzResAttribute,
            PropertyHelper<uint>::toString(static_cast<uint>(d_nativeResolution.d_width)));

    if (d_nativeResolution.d_height != DefaultNativeVertRes)
        xml_stream.attribute(Font_xmlHandler::FontNativeVertResAttribute,
            PropertyHelper<uint>::toString(static_cast<uint>(d_nativeResolution.d_height)));

    if (d_autoScaled != ASM_Disabled)
        xml_stream.attribute(Font_xmlHandler::FontAutoScaledAttribute,
            PropertyHelper<AutoScaledMode>::toString(d_autoScaled));

    writeXMLToStream_impl(xml_stream);

    // output closing </Font> element.
    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
void Font::onRenderSizeChanged(FontEventArgs& e)
{
    fireEvent(EventRenderSizeChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
