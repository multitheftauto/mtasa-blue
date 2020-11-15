/***********************************************************************
    created:    Mon Jul 4 2005
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
#include "CEGUI/WindowRendererSets/Core/Listbox.h"
#include "CEGUI/widgets/Scrollbar.h"
#include "CEGUI/widgets/ListboxItem.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardListbox::TypeName("Core/Listbox");


    FalagardListbox::FalagardListbox(const String& type) :
        ListboxWindowRenderer(type)
    {
    }

    Rectf FalagardListbox::getListRenderArea(void) const
    {
    	Listbox* lb = (Listbox*)d_window;

        return getItemRenderingArea(lb->getHorzScrollbar()->isVisible(),
                                    lb->getVertScrollbar()->isVisible());
    }

    Rectf FalagardListbox::getItemRenderingArea(bool hscroll,
                                                bool vscroll) const
    {
    	const Listbox* const lb = static_cast<Listbox*>(d_window);
        const WidgetLookFeel& wlf = getLookNFeel();
        const String area_name("ItemRenderingArea");
        const String alternate_name("ItemRenderArea");
        const String scroll_suffix(
            vscroll ? hscroll ? "HVScroll" : "VScroll" : hscroll ? "HScroll" : "");

        if (wlf.isNamedAreaDefined(area_name + scroll_suffix))
                return wlf.getNamedArea(area_name + scroll_suffix).getArea().getPixelRect(*lb);

        if (wlf.isNamedAreaDefined(alternate_name + scroll_suffix))
                return wlf.getNamedArea(alternate_name + scroll_suffix).getArea().getPixelRect(*lb);

        // default to plain ItemRenderingArea
        if (wlf.isNamedAreaDefined(area_name))
            return wlf.getNamedArea(area_name).getArea().getPixelRect(*lb);
        else
            return wlf.getNamedArea(alternate_name).getArea().getPixelRect(*lb);
    }

    void FalagardListbox::resizeListToContent(bool fit_width,
                                              bool fit_height) const
    {
    	Listbox* const lb = static_cast<Listbox*>(d_window);

        const Rectf totalArea(lb->getUnclippedOuterRect().get());
        const Rectf contentArea(getItemRenderingArea(
            fit_width ? false : lb->getHorzScrollbar()->isVisible(),
            fit_height ? false : lb->getVertScrollbar()->isVisible()));
        const Rectf withScrollContentArea(getItemRenderingArea(true, true));

        const Sizef frameSize(totalArea.getSize() - contentArea.getSize());
        const Sizef withScrollFrameSize(totalArea.getSize() -
                                        withScrollContentArea.getSize());
        const Sizef contentSize(lb->getWidestItemWidth(),
                                lb->getTotalItemsHeight());

        const Sizef parentSize(lb->getParentPixelSize());
        const Sizef maxSize(parentSize.d_width -
                            CoordConverter::asAbsolute(lb->getXPosition(),
                                                       parentSize.d_width),
                            parentSize.d_height -
                            CoordConverter::asAbsolute(lb->getYPosition(),
                                                       parentSize.d_height));

        Sizef requiredSize(frameSize + contentSize + Sizef(1, 1));

        if (fit_height)
        {
            if (requiredSize.d_height > maxSize.d_height)
            {
                requiredSize.d_height = maxSize.d_height;
                requiredSize.d_width = ceguimin(
                    maxSize.d_width,
                    requiredSize.d_width - frameSize.d_width + withScrollFrameSize.d_width);
            }
        }

        if (fit_width)
        {
            if (requiredSize.d_width > maxSize.d_width)
            {
                requiredSize.d_width = maxSize.d_width;
                requiredSize.d_height = ceguimin(
                    maxSize.d_height,
                    requiredSize.d_height - frameSize.d_height + withScrollFrameSize.d_height);
            }
        }

        if (fit_height)
            lb->setHeight(UDim(0, requiredSize.d_height));

        if (fit_width)
            lb->setWidth(UDim(0, requiredSize.d_width));
    }

    void FalagardListbox::render()
    {
    	Listbox* lb = (Listbox*)d_window;
        // render frame and stuff before we handle the items
        cacheListboxBaseImagery();

        //
        // Render list items
        //
        Vector3f itemPos;
        Sizef itemSize;
        Rectf itemClipper, itemRect;
        const float widest = lb->getWidestItemWidth();

        // calculate position of area we have to render into
        Rectf itemsArea(getListRenderArea());

        // set up some initial positional details for items
        itemPos.d_x = itemsArea.left() - lb->getHorzScrollbar()->getScrollPosition();
        itemPos.d_y = itemsArea.top() - lb->getVertScrollbar()->getScrollPosition();
        itemPos.d_z = 0.0f;

        const float alpha = lb->getEffectiveAlpha();

        // loop through the items
        size_t itemCount = lb->getItemCount();

        for (size_t i = 0; i < itemCount; ++i)
        {
            ListboxItem* listItem = lb->getListboxItemFromIndex(i);
            itemSize.d_height = listItem->getPixelSize().d_height;

            // allow item to have full width of box if this is wider than items
            itemSize.d_width = ceguimax(itemsArea.getWidth(), widest);

            // calculate destination area for this item.
            itemRect.left(itemPos.d_x);
            itemRect.top(itemPos.d_y);
            itemRect.setSize(itemSize);
            itemClipper = itemRect.getIntersection(itemsArea);

            // skip this item if totally clipped
            if (itemClipper.getWidth() == 0)
            {
                itemPos.d_y += itemSize.d_height;
                continue;
            }

            // draw this item
            listItem->draw(lb->getGeometryBuffer(), itemRect, alpha, &itemClipper);

            // update position ready for next item
            itemPos.d_y += itemSize.d_height;
        }

    }

    void FalagardListbox::cacheListboxBaseImagery()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(d_window->isEffectiveDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*d_window);
    }

    bool FalagardListbox::handleFontRenderSizeChange(const Font* const font)
    {
        bool res = ListboxWindowRenderer::handleFontRenderSizeChange(font);

        if (!res)
        {
            Listbox* const listbox = static_cast<Listbox*>(d_window);

            for (size_t i = 0; i < listbox->getItemCount(); ++i)
                res |= listbox->getListboxItemFromIndex(i)->handleFontRenderSizeChange(font);

            if (res)
                listbox->invalidate();
        }

        return res;
    }

} // End of  CEGUI namespace section
