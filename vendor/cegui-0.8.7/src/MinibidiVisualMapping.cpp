/***********************************************************************
    created:    Wed Aug 5 2009
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

#include "CEGUI/Config.h"

#ifdef CEGUI_USE_MINIBIDI

#include "CEGUI/MinibidiVisualMapping.h"
#include "CEGUI/Logger.h"

// include minibidi code directly
#include "minibidi.cpp"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
BidiCharType MinibidiVisualMapping::getBidiCharType(const utf32 char_to_check) const
{
    switch (getType(char_to_check))
    {
    case R:
        return BCT_RIGHT_TO_LEFT;
        break;

    case L:
        return BCT_LEFT_TO_RIGHT;
        break;

    default:
        return BCT_NEUTRAL;
        break;
    }
}

//----------------------------------------------------------------------------//
bool MinibidiVisualMapping::reorderFromLogicalToVisual(const String& logical,
                                                       String& visual,
                                                       StrIndexList& l2v,
                                                       StrIndexList& v2l) const
{
    visual = logical;

    if (logical.length() <= 1)
        return true;

    l2v.resize(logical.length());
    v2l.resize(logical.length());

    doBidi(visual.ptr(), static_cast<int>(logical.length()),
           true, false,
           &v2l[0], &l2v[0]);

    return true;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

#endif // CEGUI_USE_MINIBIDI
