/***********************************************************************
    created:    Sun Jul 19 2009
    author:     Paul D Turner <paul@cegui.org.uk>
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUI/Font_xmlHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
#include "CEGUI/XMLAttributes.h"
#include "CEGUI/System.h"
#include "CEGUI/XMLParser.h"
#include "CEGUI/PixmapFont.h"

#ifdef CEGUI_HAS_FREETYPE
#   include "CEGUI/FreeTypeFont.h"
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const String Font_xmlHandler::FontSchemaName("Font.xsd");
const String Font_xmlHandler::FontElement("Font");
const String Font_xmlHandler::MappingElement("Mapping");
const String Font_xmlHandler::FontTypeAttribute("type");
const String Font_xmlHandler::FontNameAttribute("name");
const String Font_xmlHandler::FontFilenameAttribute("filename");
const String Font_xmlHandler::FontResourceGroupAttribute("resourceGroup");
const String Font_xmlHandler::FontAutoScaledAttribute("autoScaled");
const String Font_xmlHandler::FontNativeHorzResAttribute("nativeHorzRes");
const String Font_xmlHandler::FontNativeVertResAttribute("nativeVertRes");
const String Font_xmlHandler::FontLineSpacingAttribute("lineSpacing");
const String Font_xmlHandler::FontSizeAttribute("size");
const String Font_xmlHandler::FontAntiAliasedAttribute("antiAlias");
const String Font_xmlHandler::MappingCodepointAttribute("codepoint");
const String Font_xmlHandler::MappingImageAttribute("image");
const String Font_xmlHandler::MappingHorzAdvanceAttribute("horzAdvance");
const String Font_xmlHandler::FontVersionAttribute( "version" );
const String Font_xmlHandler::FontTypeFreeType("FreeType");
const String Font_xmlHandler::FontTypePixmap("Pixmap");

//----------------------------------------------------------------------------//
// note: The assets' versions aren't usually the same as CEGUI version, they
// are versioned from version 1 onwards!
//
// previous versions (though not specified in files until 3)
// 1 - CEGUI up to and including 0.4.x
// 2 - CEGUI versions 0.5.x through 0.7.x (Static/Dynamic types renamed to Pixmap/TrueType
//                                         Removed facility to pre-declare glyphs and glyph ranges)
// 3 - CEGUI version 1.x.x (changed case of attr names, added version support)
const String NativeVersion( "3" );

//----------------------------------------------------------------------------//
Font_xmlHandler::Font_xmlHandler():
    d_font(0),
    d_objectRead(false)
{}

//----------------------------------------------------------------------------//
Font_xmlHandler::~Font_xmlHandler()
{
    if (!d_objectRead)
        CEGUI_DELETE_AO d_font;
}

//----------------------------------------------------------------------------//
const String& Font_xmlHandler::getObjectName() const
{
    if (!d_font)
        CEGUI_THROW(InvalidRequestException(
            "Attempt to access null object."));

    return d_font->getName();
}

//----------------------------------------------------------------------------//
Font& Font_xmlHandler::getObject() const
{
    if (!d_font)
        CEGUI_THROW(InvalidRequestException(
            "Attempt to access null object."));

    d_objectRead = true;
    return *d_font;
}

//----------------------------------------------------------------------------//
const String& Font_xmlHandler::getSchemaName() const
{
    return FontSchemaName;
}

//----------------------------------------------------------------------------//
const String& Font_xmlHandler::getDefaultResourceGroup() const
{
    return Font::getDefaultResourceGroup();
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::elementStart(const String& element,
                                   const XMLAttributes& attributes)
{
    // handle root Font element
    if (element == FontElement)
        elementFontStart(attributes);
    // handle a Mapping element
    else if (element == MappingElement)
        elementMappingStart(attributes);
    // anything else is a non-fatal error.
    else
        Logger::getSingleton().logEvent("Font_xmlHandler::elementStart: "
            "Unknown element encountered: <" + element + ">", Errors);
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::elementEnd(const String& element)
{
    if (element == FontElement)
        elementFontEnd();
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::elementFontStart(const XMLAttributes& attributes)
{
    validateFontFileVersion(attributes);

    // get type of font being created
    const String font_type(attributes.getValueAsString(FontTypeAttribute));

    // log the start of font creation.
    CEGUI_LOGINSANE(
        "Started creation of Font from XML specification:");

    if (font_type == FontTypeFreeType)
        createFreeTypeFont(attributes);
    else if (font_type == FontTypePixmap)
        createPixmapFont(attributes);
    else
        CEGUI_THROW(InvalidRequestException(
            "Encountered unknown font type of '" + font_type + "'"));
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::validateFontFileVersion(const XMLAttributes& attrs)
{
    const String version(attrs.getValueAsString(FontVersionAttribute,
                                                "unknown"));

    if (version == NativeVersion)
        return;

    CEGUI_THROW(InvalidRequestException(
        "You are attempting to load a font of version '" + version + "' but "
        "this CEGUI version is only meant to load fonts of version '" +
        NativeVersion + "'. Consider using the migrate.py script bundled with "
        "CEGUI Unified Editor to migrate your data."));
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::elementFontEnd()
{
    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(d_font));
    Logger::getSingleton().logEvent("Finished creation of Font '" +
        d_font->getName() + "' via XML file. " + addr_buff, Informative);
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::elementMappingStart(const XMLAttributes& attributes)
{
    if (!d_font)
        CEGUI_THROW(InvalidRequestException(
            "Attempt to access null object."));

    // double-check font type just in case - report issues as 'soft' errors
    if (d_font->getTypeName() != FontTypePixmap)
        Logger::getSingleton().logEvent(
            "Imageset_xmlHandler::elementMappingStart: <Mapping> element is "
            "only valid for Pixmap type fonts.", Errors);
    else
        static_cast<PixmapFont*>(d_font)->defineMapping(
            attributes.getValueAsInteger(MappingCodepointAttribute),
            attributes.getValueAsString(MappingImageAttribute),
            attributes.getValueAsFloat(MappingHorzAdvanceAttribute, -1.0f));
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::createFreeTypeFont(const XMLAttributes& attributes)
{
    const String name(attributes.getValueAsString(FontNameAttribute));
    const String filename(attributes.getValueAsString(FontFilenameAttribute));
    const String resource_group(attributes.getValueAsString(FontResourceGroupAttribute));

#ifdef CEGUI_HAS_FREETYPE
    CEGUI_LOGINSANE("---- CEGUI font name: " + name);
    CEGUI_LOGINSANE("----       Font type: FreeType");
    CEGUI_LOGINSANE("----     Source file: " + filename +
                    " in resource group: " + (resource_group.empty() ?
                                              "(Default)" : resource_group));
    CEGUI_LOGINSANE("---- Real point size: " +
            attributes.getValueAsString(FontSizeAttribute, "12"));

    d_font = CEGUI_NEW_AO FreeTypeFont(name,
        attributes.getValueAsFloat(FontSizeAttribute, 12.0f),
        attributes.getValueAsBool(FontAntiAliasedAttribute, true),
        filename, resource_group,
        PropertyHelper<AutoScaledMode>::fromString(
                attributes.getValueAsString(FontAutoScaledAttribute)),
        Sizef(attributes.getValueAsFloat(FontNativeHorzResAttribute, 640.0f),
              attributes.getValueAsFloat(FontNativeVertResAttribute, 480.0f)),
        attributes.getValueAsFloat(FontLineSpacingAttribute, 0.0f));
#else
    CEGUI_THROW(InvalidRequestException(
        "CEGUI was compiled without freetype support."));
#endif
}

//----------------------------------------------------------------------------//
void Font_xmlHandler::createPixmapFont(const XMLAttributes& attributes)
{
    const String name(attributes.getValueAsString(FontNameAttribute));
    const String filename(attributes.getValueAsString(FontFilenameAttribute));
    const String resource_group(attributes.getValueAsString(FontResourceGroupAttribute));

    CEGUI_LOGINSANE("---- CEGUI font name: " + name);
    CEGUI_LOGINSANE("----       Font type: Pixmap");
    CEGUI_LOGINSANE("----     Source file: " + filename +
                    " in resource group: " + (resource_group.empty() ? "(Default)" : resource_group));

    d_font = CEGUI_NEW_AO PixmapFont(name, filename, resource_group,
        PropertyHelper<AutoScaledMode>::fromString(
                        attributes.getValueAsString(FontAutoScaledAttribute)),
        Sizef(attributes.getValueAsFloat(FontNativeHorzResAttribute, 640.0f),
              attributes.getValueAsFloat(FontNativeVertResAttribute, 480.0f)));
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
