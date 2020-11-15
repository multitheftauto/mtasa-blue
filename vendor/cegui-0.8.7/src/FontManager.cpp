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

#include "CEGUI/FontManager.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
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
// singleton instance pointer
template<> FontManager* Singleton<FontManager>::ms_Singleton = 0;

//----------------------------------------------------------------------------//
FontManager::FontManager() :
    NamedXMLResourceManager<Font, Font_xmlHandler>("Font")
{
    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::FontManager singleton created. " + String(addr_buff));
}

//----------------------------------------------------------------------------//
FontManager::~FontManager()
{
    Logger::getSingleton().logEvent(
        "---- Begining cleanup of Font system ----");

    destroyAll();

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::FontManager singleton destroyed. " + String(addr_buff));
}

//----------------------------------------------------------------------------//

Font& FontManager::createFreeTypeFont(const String& font_name,
                                      const float point_size,
                                      const bool anti_aliased,
                                      const String& font_filename,
                                      const String& resource_group,
                                      const AutoScaledMode auto_scaled,
                                      const Sizef& native_res,
                                      XMLResourceExistsAction action)
{
#ifdef CEGUI_HAS_FREETYPE
    CEGUI_LOGINSANE("Attempting to create FreeType font '" +
        font_name + "' using font file '" + font_filename + "'.");

    // create new object ahead of time
    Font* object = CEGUI_NEW_AO FreeTypeFont(font_name, point_size, anti_aliased,
                                    font_filename, resource_group, auto_scaled,
                                    native_res);

    // return appropriate object instance (deleting any not required)
    return doExistingObjectAction(font_name, object, action);

#else
    CEGUI_THROW(InvalidRequestException(
        "CEGUI was compiled without freetype support."));
#endif
}

//----------------------------------------------------------------------------//
Font& FontManager::createPixmapFont(const String& font_name,
                                    const String& imageset_filename,
                                    const String& resource_group,
                                    const AutoScaledMode auto_scaled,
                                    const Sizef& native_res,
                                    XMLResourceExistsAction action)
{
    CEGUI_LOGINSANE("Attempting to create Pixmap font '" +
        font_name + "' using imageset file '" + imageset_filename + "'.");

    // create new object ahead of time
    Font* object = CEGUI_NEW_AO PixmapFont(font_name, imageset_filename, resource_group,
                                  auto_scaled, native_res);

    // return appropriate object instance (deleting any not required)
    return doExistingObjectAction(font_name, object, action);;
}

//----------------------------------------------------------------------------//
void FontManager::notifyDisplaySizeChanged(const Sizef& size)
{
    // notify all attached Font objects of the change in resolution
    ObjectRegistry::iterator pos = d_objects.begin(), end = d_objects.end();

    for (; pos != end; ++pos)
        pos->second->notifyDisplaySizeChanged(size);
}

//----------------------------------------------------------------------------//
FontManager::FontIterator FontManager::getIterator(void) const
{
    return FontIterator(d_objects.begin(), d_objects.end());
}

//----------------------------------------------------------------------------//
void FontManager::writeFontToStream(const String& name,
                                    OutStream& out_stream) const
{
    XMLSerializer xml(out_stream);
    // output font data
    get(name).writeXMLToStream(xml);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
