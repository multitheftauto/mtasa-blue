/***********************************************************************
    created:    Wed Aug 11 2010
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2010 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/ChainedXMLHandler.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
ChainedXMLHandler::ChainedXMLHandler() :
    d_chainedHandler(0),
    d_completed(false),
    d_deleteChaniedHandler(true)
{
}

//----------------------------------------------------------------------------//
ChainedXMLHandler::~ChainedXMLHandler()
{
    cleanupChainedHandler();
}

//----------------------------------------------------------------------------//
const String& ChainedXMLHandler::getSchemaName() const
{
    // this isn't likely to be used in ChainedXMLHandler instances
    static String empty = "";
    return empty;
}

//----------------------------------------------------------------------------//
const String& ChainedXMLHandler::getDefaultResourceGroup() const
{
    // this isn't likely to be used in ChainedXMLHandler instances
    static String empty = "";
    return empty;
}

//----------------------------------------------------------------------------//
void ChainedXMLHandler::elementStart(const String& element,
                                     const XMLAttributes& attributes)
{
    // chained handler gets first crack at this element
    if (d_chainedHandler)
    {
        d_chainedHandler->elementStart(element, attributes);
        // clean up if completed
        if (d_chainedHandler->completed())
            cleanupChainedHandler();
    }
    else
        elementStartLocal(element, attributes);
}

//----------------------------------------------------------------------------//
void ChainedXMLHandler::elementEnd(const String& element)
{
    // chained handler gets first crack at this element
    if (d_chainedHandler)
    {
        d_chainedHandler->elementEnd(element);
        // clean up if completed
        if (d_chainedHandler->completed())
            cleanupChainedHandler();
    }
    else
        elementEndLocal(element);
}

//----------------------------------------------------------------------------//
bool ChainedXMLHandler::completed() const
{
    return d_completed;
}

//----------------------------------------------------------------------------//
void ChainedXMLHandler::cleanupChainedHandler()
{
    if (d_deleteChaniedHandler)
        CEGUI_DELETE_AO d_chainedHandler;

    d_chainedHandler = 0;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

