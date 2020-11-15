/***********************************************************************
    created:    Wed Jul 6 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/WindowRendererSets/Core/ListHeader.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/TplWindowRendererProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardListHeader::TypeName("Core/ListHeader");

    FalagardListHeader::FalagardListHeader(const String& type) :
        ListHeaderWindowRenderer(type)
    {
        CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardListHeader,String,
        "SegmentWidgetType", "Property to get/set the widget type used when creating header segments.  Value should be \"[widgetTypeName]\".",
        &FalagardListHeader::setSegmentWidgetType,&FalagardListHeader::getSegmentWidgetType,
        "");
    }

    void FalagardListHeader::render()
    {
        const StateImagery* imagery;
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        // render basic imagery
        imagery = &wlf.getStateImagery(d_window->isEffectiveDisabled() ? "Disabled" : "Enabled");
        imagery->render(*d_window);
    }

    const String& FalagardListHeader::getSegmentWidgetType() const
    {
        return d_segmentWidgetType;
    }

    void FalagardListHeader::setSegmentWidgetType(const String& type)
    {
        d_segmentWidgetType = type;
    }

    ListHeaderSegment* FalagardListHeader::createNewSegment(const String& name) const
    {
        // make sure this has been set
        if (d_segmentWidgetType.empty())
        {
            CEGUI_THROW(InvalidRequestException(
                "Segment widget type has not been set!"));
        }

        Window* segment = WindowManager::getSingleton().createWindow(d_segmentWidgetType, name);
        segment->setAutoWindow(true);
        return static_cast<ListHeaderSegment*>(segment);
    }

    void FalagardListHeader::destroyListSegment(ListHeaderSegment* segment) const
    {
        // nothing special required here.
        WindowManager::getSingleton().destroyWindow(segment);
    }

} // End of  CEGUI namespace section
