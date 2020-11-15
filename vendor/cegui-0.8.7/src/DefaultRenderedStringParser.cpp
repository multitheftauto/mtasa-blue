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
#include "CEGUI/DefaultRenderedStringParser.h"
#include "CEGUI/RenderedStringTextComponent.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
void appendSubstring(RenderedString& rs,
                     const String& string,
                     const Font* initial_font,
                     const ColourRect* initial_colours)
{
    RenderedStringTextComponent rstc(string, initial_font);

    if (initial_colours)
        rstc.setColours(*initial_colours);

    rs.appendComponent(rstc);
}

//----------------------------------------------------------------------------//
RenderedString DefaultRenderedStringParser::parse(
                                        const String& input_string,
                                        const Font* initial_font,
                                        const ColourRect* initial_colours)
{
    RenderedString rs;

    size_t epos, spos = 0;

    while ((epos = input_string.find('\n', spos)) != String::npos)
    {
        appendSubstring(rs, input_string.substr(spos, epos - spos),
                        initial_font, initial_colours);
        rs.appendLineBreak();

        // set new start position (skipping the previous \n we found)
        spos = epos + 1;
    }

    if (spos < input_string.length())
        appendSubstring(rs, input_string.substr(spos),
                        initial_font, initial_colours);

    return rs;
}

//----------------------------------------------------------------------------//
    
} // End of  CEGUI namespace section
