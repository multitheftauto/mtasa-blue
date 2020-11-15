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

#ifdef CEGUI_USE_FRIBIDI

#include "CEGUI/FribidiVisualMapping.h"
#include "CEGUI/Logger.h"
#include <fribidi.h>

#ifdef _MSC_VER
    #pragma comment(lib,"fribidi_dll")
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
BidiCharType FribidiVisualMapping::getBidiCharType(const utf32 char_to_check) const
{
    switch (fribidi_get_type((FriBidiChar)char_to_check))
    {
    case FRIBIDI_TYPE_RTL:
        return BCT_RIGHT_TO_LEFT;
        break;

    case FRIBIDI_TYPE_LTR:
        return BCT_LEFT_TO_RIGHT;
        break;

    case FRIBIDI_TYPE_ON:
    default:
        return BCT_NEUTRAL;
        break;
    }
}

//----------------------------------------------------------------------------//
bool FribidiVisualMapping::reorderFromLogicalToVisual(const String& logical,
                                                       String& visual,
                                                       StrIndexList& l2v,
                                                       StrIndexList& v2l) const
{
    visual = logical;

    if (logical.length() <= 1)
        return true;

    FriBidiCharType input_base_direction = FRIBIDI_TYPE_L;
    l2v.resize(logical.length());
    v2l.resize(logical.length());
    String logicalNotConst(logical);

    fribidi_boolean res =
        fribidi_log2vis(static_cast<FriBidiChar *>(logicalNotConst.ptr()),
                        static_cast<FriBidiStrIndex>(logical.length()),
                        &input_base_direction,
                        /* output */
                        static_cast<FriBidiChar *>(visual.ptr()),
                        &l2v[0], &v2l[0], 0);

    // success?
    if (res)
        return true;

    // log failure and continue anyway :-p
    Logger::getSingleton().logEvent(
        "FribidiVisualMapping::reorderFromLogicalToVisual: fribidi_log2vis "
        "call failed on logical string: " + logical, Errors);

    return false;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

#endif // CEGUI_USE_FRIBIDI
