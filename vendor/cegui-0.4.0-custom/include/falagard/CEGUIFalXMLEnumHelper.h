/************************************************************************
    filename:   CEGUIFalXMLEnumHelper.h
    created:    Mon Jul 18 2005
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
#ifndef _CEGUIFalXMLEnumHelper_h_
#define _CEGUIFalXMLEnumHelper_h_

#include "CEGUIString.h"
#include "CEGUIWindow.h"
#include "falagard/CEGUIFalEnums.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Utility helper class primarily intended for use by the falagard xml parser.
    */
    class CEGUIEXPORT FalagardXMLHelper
    {
    public:
        static VerticalFormatting stringToVertFormat(const String& str);
        static HorizontalFormatting stringToHorzFormat(const String& str);
        static VerticalAlignment stringToVertAlignment(const String& str);
        static HorizontalAlignment stringToHorzAlignment(const String& str);
        static DimensionType stringToDimensionType(const String& str);
        static VerticalTextFormatting stringToVertTextFormat(const String& str);
        static HorizontalTextFormatting stringToHorzTextFormat(const String& str);
        static FontMetricType stringToFontMetricType(const String& str);
        static DimensionOperator stringToDimensionOperator(const String& str);
        static FrameImageComponent stringToFrameImageComponent(const String& str);

        static String vertFormatToString(VerticalFormatting format);
        static String horzFormatToString(HorizontalFormatting format);
        static String vertAlignmentToString(VerticalAlignment alignment);
        static String horzAlignmentToString(HorizontalAlignment alignment);
        static String dimensionTypeToString(DimensionType dim);
        static String vertTextFormatToString(VerticalTextFormatting format);
        static String horzTextFormatToString(HorizontalTextFormatting format);
        static String fontMetricTypeToString(FontMetricType metric);
        static String dimensionOperatorToString(DimensionOperator op);
        static String frameImageComponentToString(FrameImageComponent imageComp);
    };
} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalXMLEnumHelper_h_
