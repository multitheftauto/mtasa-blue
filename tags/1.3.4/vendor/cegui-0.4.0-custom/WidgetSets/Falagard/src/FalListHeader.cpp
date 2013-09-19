/************************************************************************
    filename:   FalListHeader.cpp
    created:    Wed Jul 6 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "StdInc.h"
#include "FalListHeader.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIWindowManager.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardListHeader::WidgetTypeName[] = "Falagard/ListHeader";

    // properties
    FalagardListHeaderProperties::SegmentWidgetType FalagardListHeader::d_segmentWidgetTypeProperty;


    FalagardListHeader::FalagardListHeader(const String& type, const String& name) :
        ListHeader(type, name)
    {
        addProperty(&d_segmentWidgetTypeProperty);
    }

    FalagardListHeader::~FalagardListHeader()
    {
    }

    void FalagardListHeader::populateRenderCache()
    {
        const StateImagery* imagery;
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        // render basic imagery
        imagery = &wlf.getStateImagery(isDisabled() ? "Disabled" : "Enabled");
        imagery->render(*this);
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
            InvalidRequestException("FalagardListHeader::createNewSegment - Segment widget type has not been set!");
        }

        return static_cast<ListHeaderSegment*>(WindowManager::getSingleton().createWindow(d_segmentWidgetType, name));
    }

    void FalagardListHeader::destroyListSegment(ListHeaderSegment* segment) const
    {
        // nothing special required here.
        WindowManager::getSingleton().destroyWindow(segment);
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardListHeaderFactory::createWindow(const String& name)
    {
        return new FalagardListHeader(d_type, name);
    }

    void FalagardListHeaderFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section
