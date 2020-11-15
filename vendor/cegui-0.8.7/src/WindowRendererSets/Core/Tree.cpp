/***********************************************************************
created:	5-13-07
author:		Jonathan Welch (Based on Code by David Durant)
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
#include "CEGUI/WindowRendererSets/Core/Tree.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/widgets/Scrollbar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardTree::TypeName("Core/Tree");

    FalagardTree::FalagardTree(const String& type, const String&) :
        WindowRenderer(type)
    {
    }

    FalagardTree::FalagardTree(const String& type) :
        WindowRenderer(type)
    {
    }

    FalagardTree::~FalagardTree()
    {
    }

    Rectf FalagardTree::getTreeRenderArea(void) const
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
		Tree* tree = (Tree*)d_window;

        bool v_visible = tree->getVertScrollbar()->isVisible();
        bool h_visible = tree->getHorzScrollbar()->isVisible();

        // if either of the scrollbars are visible, we might want to use another text rendering area
        if (v_visible || h_visible)
        {
            String area_name("ItemRenderingArea");

            if (h_visible)
            {
                area_name += "H";
            }
            if (v_visible)
            {
                area_name += "V";
            }
            area_name += "Scroll";

            if (wlf.isNamedAreaDefined(area_name))
            {
                return wlf.getNamedArea(area_name).getArea().getPixelRect(*tree);
            }
        }

        // default to plain ItemRenderingArea
        return wlf.getNamedArea("ItemRenderingArea").getArea().getPixelRect(*tree);
    }

	void FalagardTree::render()
	{
		Tree* tree = (Tree*)d_window;
		//Set the render area for this.
        Rectf rect = getTreeRenderArea();
        tree->setItemRenderArea(rect);
		const WidgetLookFeel& wlf = getLookNFeel();

		const StateImagery* imagery;
		//Get the Falagard imagery to render
		imagery = &wlf.getStateImagery(tree->isEffectiveDisabled()? "Disabled" : "Enabled");
		//Render the window
		imagery->render(*tree);
		//Fix Scrollbars
		tree->doScrollbars();
		//Render the tree.
		tree->doTreeRender();
	}
} // End of  CEGUI namespace section
