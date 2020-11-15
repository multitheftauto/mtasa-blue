/***********************************************************************
    created:    28/05/2009
    author:     Paul Turner
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
#include "CEGUI/BasicRenderedStringParser.h"
#include "CEGUI/RenderedStringTextComponent.h"
#include "CEGUI/RenderedStringImageComponent.h"
#include "CEGUI/RenderedStringWidgetComponent.h"
#include "CEGUI/Logger.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/Font.h"
#include "CEGUI/Image.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
// Internal helper to parse part of a string, using backslash as an escape char
static bool parse_section(String::const_iterator& pos,
                          const String::const_iterator& end,
                          String::value_type delim,
                          String& out);

//----------------------------------------------------------------------------//
const String BasicRenderedStringParser::ColourTagName("colour");
const String BasicRenderedStringParser::FontTagName("font");
const String BasicRenderedStringParser::ImageTagName("image");
const String BasicRenderedStringParser::WindowTagName("window");
const String BasicRenderedStringParser::VertAlignmentTagName("vert-alignment");
const String BasicRenderedStringParser::PaddingTagName("padding");
const String BasicRenderedStringParser::TopPaddingTagName("top-padding");
const String BasicRenderedStringParser::BottomPaddingTagName("bottom-padding");
const String BasicRenderedStringParser::LeftPaddingTagName("left-padding");
const String BasicRenderedStringParser::RightPaddingTagName("right-padding");
const String BasicRenderedStringParser::AspectLockTagName("aspect-lock");
const String BasicRenderedStringParser::ImageSizeTagName("image-size");
const String BasicRenderedStringParser::ImageWidthTagName("image-width");
const String BasicRenderedStringParser::ImageHeightTagName("image-height");
const String BasicRenderedStringParser::TopAlignedValueName("top");
const String BasicRenderedStringParser::BottomAlignedValueName("bottom");
const String BasicRenderedStringParser::CentreAlignedValueName("centre");
const String BasicRenderedStringParser::StretchAlignedValueName("stretch");

//----------------------------------------------------------------------------//
BasicRenderedStringParser::BasicRenderedStringParser() :
    d_initialColours(0xFFFFFFFF),
    d_vertAlignment(VF_BOTTOM_ALIGNED),
    d_imageSize(0, 0),
    d_aspectLock(false),
    d_initialised(false)
{
    initialiseDefaultState();
}

//----------------------------------------------------------------------------//
BasicRenderedStringParser::BasicRenderedStringParser(const String& initial_font,
                                            const ColourRect& initial_colours) :
    d_initialFontName(initial_font),
    d_initialColours(initial_colours),
    d_vertAlignment(VF_BOTTOM_ALIGNED),
    d_imageSize(0, 0),
    d_aspectLock(false),
    d_initialised(false)
{
    initialiseDefaultState();
}

//----------------------------------------------------------------------------//
BasicRenderedStringParser::~BasicRenderedStringParser()
{
}

//----------------------------------------------------------------------------//
RenderedString BasicRenderedStringParser::parse(const String& input_string,
                                                const Font* active_font,
                                                const ColourRect* active_colours)
{
    // first-time initialisation (due to issues with static creation order)
    if (!d_initialised)
        initialiseTagHandlers();

    initialiseDefaultState();

    // Override active font if necessary
    if (active_font)
        d_fontName = active_font->getName();

    // Override active font if necessary
    if (active_colours)
        d_colours = *active_colours;

    RenderedString rs;
    String curr_section, tag_string;

    for (String::const_iterator input_iter(input_string.begin());
         input_iter != input_string.end();
         /* no-op*/)
    {
        const bool found_tag = parse_section(input_iter, input_string.end(), '[', curr_section);
        appendRenderedText(rs, curr_section);

        if (!found_tag)
            return rs;

        if (!parse_section(input_iter, input_string.end(), ']', tag_string))
        {
            Logger::getSingleton().logEvent(
                "BasicRenderedStringParser::parse: Ignoring unterminated tag : [" +
                tag_string);

            return rs;
        }

        processControlString(rs, tag_string);
    }

    return rs;
}

//----------------------------------------------------------------------------//
bool parse_section(String::const_iterator& pos, const String::const_iterator& end,
                   String::value_type delim, String& out)
{
    const String::value_type escape('\\');
    out.resize(0);

    String::const_iterator start_iter(pos);

    for ( ; pos != end; ++pos)
    {
        if (*pos == delim)
        {
            out.append(start_iter, pos++);
            return true;
        }

        if (*pos == escape)
        {
            out.append(start_iter, pos++);

            if (pos == end)
                return false;

            start_iter = pos;
        }
    }

    out.append(start_iter, pos);
    return false;
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::appendRenderedText(RenderedString& rs,
                                                   const String& text) const
{
    size_t cpos = 0;
    // split the given string into lines based upon the newline character
    while (text.length() > cpos)
    {
        // find next newline
        const size_t nlpos = text.find('\n', cpos);
        // calculate length of this substring
        const size_t len =
            ((nlpos != String::npos) ? nlpos : text.length()) - cpos;

        // construct new text component and append it.
        RenderedStringTextComponent rtc(text.substr(cpos, len), d_fontName);
        rtc.setPadding(d_padding);
        rtc.setColours(d_colours);
        rtc.setVerticalFormatting(d_vertAlignment);
        rtc.setAspectLock(d_aspectLock);
        rs.appendComponent(rtc);

        // break line if needed
        if (nlpos != String::npos)
            rs.appendLineBreak();

        // advance current position.  +1 to skip the \n char
        cpos += len + 1;
    }
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::processControlString(RenderedString& rs,
                                                     const String& ctrl_str)
{
    // all our default strings are of the form <var> = <val>
    // so do a quick check for the = char and abort if it's not there.
    if (String::npos == ctrl_str.find('='))
    {
        Logger::getSingleton().logEvent(
            "BasicRenderedStringParser::processControlString: unable to make "
            "sense of control string '" + ctrl_str + "'.  Ignoring!");
        
        return;
    }

    char var_buf[128];
    char val_buf[128];
    int successfullyFilledItems = sscanf(ctrl_str.c_str(), " %127[^ =] = '%127[^']", var_buf, val_buf);

    const String var_str(var_buf);
    // look up handler function
    TagHandlerMap::iterator i = d_tagHandlers.find(var_str);
    // dispatch handler, or log error
    if (successfullyFilledItems >= 1 && i != d_tagHandlers.end())
    {
        // If both variables were read correctly, proceed as usual
        if(successfullyFilledItems == 2)
        {
            const String val_str(val_buf);
            (this->*(*i).second)(rs, val_str);
        }
        // Otherwise, since the handler was found, we are sure that the
        // second variable couldn't be read, meaning it was empty. We will supply
        // and empty string
        else
            (this->*(*i).second)(rs, "");
    }
    else
        Logger::getSingleton().logEvent(
            "BasicRenderedStringParser::processControlString: unknown "
            "control variable '" + var_str + "'.  Ignoring!");
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::initialiseDefaultState()
{
    d_padding = Rectf(0, 0, 0, 0);
    //!  \deprecated This assignment is deprecated and will be replaced by assignment to "" in the next major version */
    d_colours = d_initialColours;
    //!  \deprecated This assignment is deprecated and will be replaced by assignment Colour 0xFFFFFFFF in the next major version */
    d_fontName = d_initialFontName;
    d_imageSize.d_width = d_imageSize.d_height = 0.0f;
    d_vertAlignment = VF_BOTTOM_ALIGNED;
    d_aspectLock = false;
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::setInitialFontName(const String& font_name)
{
    d_initialFontName = font_name;
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::setInitialColours(const ColourRect& colours)
{
    d_initialColours = colours;
}

//----------------------------------------------------------------------------//
const String& BasicRenderedStringParser::getInitialFontName() const
{
    return d_initialFontName;
}

//----------------------------------------------------------------------------//
const ColourRect& BasicRenderedStringParser::getInitialColours() const
{
    return d_initialColours;
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::initialiseTagHandlers()
{
    d_tagHandlers[ColourTagName] = &BasicRenderedStringParser::handleColour;
    d_tagHandlers[FontTagName] = &BasicRenderedStringParser::handleFont;
    d_tagHandlers[ImageTagName] = &BasicRenderedStringParser::handleImage;
    d_tagHandlers[WindowTagName] = &BasicRenderedStringParser::handleWindow;
    d_tagHandlers[VertAlignmentTagName] = &BasicRenderedStringParser::handleVertAlignment;
    d_tagHandlers[PaddingTagName] = &BasicRenderedStringParser::handlePadding;
    d_tagHandlers[TopPaddingTagName] = &BasicRenderedStringParser::handleTopPadding;
    d_tagHandlers[BottomPaddingTagName] = &BasicRenderedStringParser::handleBottomPadding;
    d_tagHandlers[LeftPaddingTagName] = &BasicRenderedStringParser::handleLeftPadding;
    d_tagHandlers[RightPaddingTagName] = &BasicRenderedStringParser::handleRightPadding;
    d_tagHandlers[AspectLockTagName] = &BasicRenderedStringParser::handleAspectLock;
    d_tagHandlers[ImageSizeTagName] = &BasicRenderedStringParser::handleImageSize;
    d_tagHandlers[ImageWidthTagName] = &BasicRenderedStringParser::handleImageWidth;
    d_tagHandlers[ImageHeightTagName] = &BasicRenderedStringParser::handleImageHeight;

    d_initialised = true;
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleColour(RenderedString&, const String& value)
{
    d_colours.setColours(PropertyHelper<Colour>::fromString(value));
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleFont(RenderedString&, const String& value)
{
    d_fontName = value;
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleImage(RenderedString& rs, const String& value)
{
    RenderedStringImageComponent ric(
        PropertyHelper<Image*>::fromString(value));
    ric.setPadding(d_padding);
    ric.setColours(d_colours);
    ric.setVerticalFormatting(d_vertAlignment);
    ric.setSize(d_imageSize);
    ric.setAspectLock(d_aspectLock);
    rs.appendComponent(ric);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleWindow(RenderedString& rs, const String& value)
{
    RenderedStringWidgetComponent rwc(value);
    rwc.setPadding(d_padding);
    rwc.setVerticalFormatting(d_vertAlignment);
    rwc.setAspectLock(d_aspectLock);
    rs.appendComponent(rwc);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleVertAlignment(RenderedString&, const String& value)
{
    if (value == TopAlignedValueName)
        d_vertAlignment = VF_TOP_ALIGNED;
    else if (value == BottomAlignedValueName)
        d_vertAlignment = VF_BOTTOM_ALIGNED;
    else if (value == CentreAlignedValueName)
        d_vertAlignment = VF_CENTRE_ALIGNED;
    else if (value == StretchAlignedValueName)
        d_vertAlignment = VF_STRETCHED;
    else
        Logger::getSingleton().logEvent(
            "BasicRenderedStringParser::handleVertAlignment: unknown "
            "vertical alignment '" + value + "'.  Ignoring!");
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handlePadding(RenderedString&,
                                              const String& value)
{
    d_padding = PropertyHelper<Rectf >::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleTopPadding(RenderedString&,
                                                 const String& value)
{
    d_padding.d_min.d_y = PropertyHelper<float>::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleBottomPadding(RenderedString&,
                                                    const String& value)
{
    d_padding.d_max.d_y = PropertyHelper<float>::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleLeftPadding(RenderedString&,
                                                  const String& value)
{
    d_padding.d_min.d_x = PropertyHelper<float>::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleRightPadding(RenderedString&,
                                                   const String& value)
{
    d_padding.d_max.d_x = PropertyHelper<float>::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleAspectLock(RenderedString&,
                                                 const String& value)
{
    d_aspectLock = PropertyHelper<bool>::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleImageSize(RenderedString&,
                                                const String& value)
{
    d_imageSize = PropertyHelper<Sizef >::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleImageWidth(RenderedString&,
                                                 const String& value)
{
    d_imageSize.d_width = PropertyHelper<float>::fromString(value);
}

//----------------------------------------------------------------------------//
void BasicRenderedStringParser::handleImageHeight(RenderedString&,
                                                  const String& value)
{
    d_imageSize.d_height = PropertyHelper<float>::fromString(value);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
