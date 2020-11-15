/***********************************************************************
    created:    19th June 2014
    author:     Lukas E Meindl

    purpose:    Implementation of PropertyHelper methods and variables
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2014 Paul D Turner & The CEGUI Development Team
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

#include "CEGUI/falagard/XMLEnumHelper.h"

namespace CEGUI
{

    //! Definitions of static constants
    const CEGUI::String FalagardXMLHelper<VerticalFormatting>::TopAligned("TopAligned");
    const CEGUI::String FalagardXMLHelper<VerticalFormatting>::CentreAligned("CentreAligned");
    const CEGUI::String FalagardXMLHelper<VerticalFormatting>::BottomAligned("BottomAligned");
    const CEGUI::String FalagardXMLHelper<VerticalFormatting>::Stretched("Stretched");
    const CEGUI::String FalagardXMLHelper<VerticalFormatting>::Tiled("Tiled");

    const CEGUI::String FalagardXMLHelper<HorizontalFormatting>::LeftAligned("LeftAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalFormatting>::CentreAligned("CentreAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalFormatting>::RightAligned("RightAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalFormatting>::Stretched("Stretched");
    const CEGUI::String FalagardXMLHelper<HorizontalFormatting>::Tiled("Tiled");

    const CEGUI::String FalagardXMLHelper<VerticalAlignment>::TopAligned("TopAligned");
    const CEGUI::String FalagardXMLHelper<VerticalAlignment>::CentreAligned("CentreAligned");
    const CEGUI::String FalagardXMLHelper<VerticalAlignment>::BottomAligned("BottomAligned");

    const CEGUI::String FalagardXMLHelper<HorizontalAlignment>::LeftAligned("LeftAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalAlignment>::CentreAligned("CentreAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalAlignment>::RightAligned("RightAligned");

    const CEGUI::String FalagardXMLHelper<DimensionType>::LeftEdge("LeftEdge");
    const CEGUI::String FalagardXMLHelper<DimensionType>::RightEdge("RightEdge");
    const CEGUI::String FalagardXMLHelper<DimensionType>::BottomEdge("BottomEdge");
    const CEGUI::String FalagardXMLHelper<DimensionType>::TopEdge("TopEdge");
    const CEGUI::String FalagardXMLHelper<DimensionType>::XPosition("XPosition");
    const CEGUI::String FalagardXMLHelper<DimensionType>::YPosition("YPosition");
    const CEGUI::String FalagardXMLHelper<DimensionType>::Width("Width");
    const CEGUI::String FalagardXMLHelper<DimensionType>::Height("Height");
    const CEGUI::String FalagardXMLHelper<DimensionType>::XOffset("XOffset");
    const CEGUI::String FalagardXMLHelper<DimensionType>::YOffset("YOffset");
    const CEGUI::String FalagardXMLHelper<DimensionType>::Invalid("Invalid");

    const CEGUI::String FalagardXMLHelper<VerticalTextFormatting>::TopAligned("TopAligned");
    const CEGUI::String FalagardXMLHelper<VerticalTextFormatting>::CentreAligned("CentreAligned");
    const CEGUI::String FalagardXMLHelper<VerticalTextFormatting>::BottomAligned("BottomAligned");

    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::LeftAligned("LeftAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::CentreAligned("CentreAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::RightAligned("RightAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::Justified("Justified");
    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::WordWrapLeftAligned("WordWrapLeftAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::WordWrapRightAligned("WordWrapRightAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::WordWrapCentreAligned("WordWrapCentreAligned");
    const CEGUI::String FalagardXMLHelper<HorizontalTextFormatting>::WordWrapJustified("WordWrapJustified");

    const CEGUI::String FalagardXMLHelper<FontMetricType>::LineSpacing("LineSpacing");
    const CEGUI::String FalagardXMLHelper<FontMetricType>::HorzExtent("HorzExtent");
    const CEGUI::String FalagardXMLHelper<FontMetricType>::Baseline("Baseline");

    const CEGUI::String FalagardXMLHelper<DimensionOperator>::Add("Add");
    const CEGUI::String FalagardXMLHelper<DimensionOperator>::Subtract("Subtract");
    const CEGUI::String FalagardXMLHelper<DimensionOperator>::Multiply("Multiply");
    const CEGUI::String FalagardXMLHelper<DimensionOperator>::Divide("Divide");
    const CEGUI::String FalagardXMLHelper<DimensionOperator>::Noop("Noop");

    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::TopLeftCorner("TopLeftCorner");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::TopRightCorner("TopRightCorner");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::BottomLeftCorner("BottomLeftCorner");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::BottomRightCorner("BottomRightCorner");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::TopEdge("TopEdge");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::LeftEdge("LeftEdge");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::RightEdge("RightEdge");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::BottomEdge("BottomEdge");
    const CEGUI::String FalagardXMLHelper<FrameImageComponent>::Background("Background");

    const CEGUI::String FalagardXMLHelper<ChildEventAction>::Redraw("Redraw");
    const CEGUI::String FalagardXMLHelper<ChildEventAction>::Layout("Layout");
    
}