/************************************************************************
    filename:   CEGUIFalSectionSpecification.cpp
    created:    Mon Jun 13 2005
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
#include "falagard/CEGUIFalSectionSpecification.h"
#include "falagard/CEGUIFalImagerySection.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "CEGUIExceptions.h"
#include "CEGUIPropertyHelper.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    SectionSpecification::SectionSpecification(const String& owner, const String& sectionName) :
        d_owner(owner),
        d_sectionName(sectionName),
        d_usingColourOverride(false),
        d_colourProperyIsRect(false)
    {}

    SectionSpecification::SectionSpecification(const String& owner, const String& sectionName, const ColourRect& cols) :
        d_owner(owner),
        d_sectionName(sectionName),
        d_coloursOverride(cols),
        d_usingColourOverride(true),
        d_colourProperyIsRect(false)
    {}

    void SectionSpecification::render(Window& srcWindow, float base_z, const ColourRect* modcols, const Rect* clipper, bool clipToDisplay) const
    {
        try
        {
            // get the imagery section object with the name we're set up to use
			const ImagerySection* sect =
				&WidgetLookManager::getSingleton().getWidgetLook(d_owner).getImagerySection(d_sectionName);

            // decide what colours are to be used
            ColourRect finalColours;
            initColourRectForOverride(srcWindow, finalColours);
            finalColours.modulateAlpha(srcWindow.getEffectiveAlpha());

            if (modcols)
                finalColours *= *modcols;

            // render the imagery section
            sect->render(srcWindow, base_z, &finalColours, clipper, clipToDisplay);
        }
        // do nothing here, errors are non-faltal and are logged for debugging purposes.
        catch (Exception)
        {}
    }

    void SectionSpecification::render(Window& srcWindow, const Rect& baseRect, float base_z, const ColourRect* modcols, const Rect* clipper, bool clipToDisplay) const
    {
        try
        {
            // get the imagery section object with the name we're set up to use
            const ImagerySection* sect =
                &WidgetLookManager::getSingleton().getWidgetLook(d_owner).getImagerySection(d_sectionName);

            // decide what colours are to be used
            ColourRect finalColours;
            initColourRectForOverride(srcWindow, finalColours);
            finalColours.modulateAlpha(srcWindow.getEffectiveAlpha());

            if (modcols)
                finalColours *= *modcols;

            // render the imagery section
            sect->render(srcWindow, baseRect, base_z, &finalColours, clipper, clipToDisplay);
        }
        // do nothing here, errors are non-faltal and are logged for debugging purposes.
        catch (Exception)
        {}
    }

    const String& SectionSpecification::getOwnerWidgetLookFeel() const
    {
        return d_owner;
    }

    const String& SectionSpecification::getSectionName() const
    {
        return d_sectionName;
    }

    const ColourRect& SectionSpecification::getOverrideColours() const
    {
        return d_coloursOverride;
    }

    void SectionSpecification::setOverrideColours(const ColourRect& cols)
    {
        d_coloursOverride = cols;
    }

    bool SectionSpecification::isUsingOverrideColours() const
    {
        return d_usingColourOverride;
    }

    void SectionSpecification::setUsingOverrideColours(bool setting)
    {
        d_usingColourOverride = setting;
    }

    void SectionSpecification::setOverrideColoursPropertySource(const String& property)
    {
        d_colourPropertyName = property;
    }

    void SectionSpecification::initColourRectForOverride(const Window& wnd, ColourRect& cr) const
    {
        // if no override set
        if (!d_usingColourOverride)
        {
            colour val(1,1,1,1);
            cr.d_top_left     = val;
            cr.d_top_right    = val;
            cr.d_bottom_left  = val;
            cr.d_bottom_right = val;
        }
        // if override comes via a colour property
        else if (!d_colourPropertyName.empty())
        {
            // if property accesses a ColourRect
            if (d_colourProperyIsRect)
            {
                cr = PropertyHelper::stringToColourRect(wnd.getProperty(d_colourPropertyName));
            }
            // property accesses a colour
            else
            {
                colour val(PropertyHelper::stringToColour(wnd.getProperty(d_colourPropertyName)));
                cr.d_top_left     = val;
                cr.d_top_right    = val;
                cr.d_bottom_left  = val;
                cr.d_bottom_right = val;
            }
        }
        // override is an explicitly defined ColourRect.
        else
        {
            cr = d_coloursOverride;
        }
    }

    void SectionSpecification::setOverrideColoursPropertyIsColourRect(bool setting)
    {
        d_colourProperyIsRect = setting;
    }

    void SectionSpecification::writeXMLToStream(OutStream& out_stream) const
    {
        out_stream << "<Section ";

        if (!d_owner.empty())
            out_stream << "look=\"" << d_owner << "\" ";

        out_stream << "section=\"" << d_sectionName << "\"";

        if (d_usingColourOverride)
        {
            // terminate opening tag
            out_stream << ">" << std::endl;

            // output modulative colours for this section
            if (!d_colourPropertyName.empty())
            {
                if (d_colourProperyIsRect)
                    out_stream << "<ColourRectProperty ";
                else
                    out_stream << "<ColourProperty ";

                out_stream << "name=\"" << d_colourPropertyName << "\" />" << std::endl;
            }
            else if (!d_coloursOverride.isMonochromatic() || d_coloursOverride.d_top_left != colour(1,1,1,1))
            {
                out_stream << "<Colours ";
                out_stream << "topLeft=\"" << PropertyHelper::colourToString(d_coloursOverride.d_top_left) << "\" ";
                out_stream << "topRight=\"" << PropertyHelper::colourToString(d_coloursOverride.d_top_right) << "\" ";
                out_stream << "bottomLeft=\"" << PropertyHelper::colourToString(d_coloursOverride.d_bottom_left) << "\" ";
                out_stream << "bottomRight=\"" << PropertyHelper::colourToString(d_coloursOverride.d_bottom_right) << "\" />" << std::endl;
            }

            // output closing section tag
            out_stream << "</Section>" << std::endl;
        }
        else
        {
            // no sub elements, just terminate opening tag
            out_stream << " />" << std::endl;
        }
    }

} // End of  CEGUI namespace section
