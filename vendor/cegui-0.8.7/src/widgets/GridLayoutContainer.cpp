/***********************************************************************
    created:    01/8/2010
    author:     Martin Preisler
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
#include "CEGUI/widgets/GridLayoutContainer.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/CoordConverter.h"
#include <limits>

#ifdef __MINGW32__

    /* Due to a bug in MinGW-w64, a false warning is sometimes issued when using
       "%llu" format with the "printf"/"scanf" family of functions. */
    #pragma GCC diagnostic ignored "-Wformat"
    #pragma GCC diagnostic ignored "-Wformat-extra-args"

#endif

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
    Constants
*************************************************************************/
// type name for this widget
const String GridLayoutContainer::WidgetTypeName("GridLayoutContainer");
// name for dummies, a number is added to the end of that
const String GridLayoutContainer::DummyName("__auto_dummy_");

const String GridLayoutContainer::EventNamespace("GridLayoutContainer");

const String GridLayoutContainer::EventChildOrderChanged("ChildOrderChanged");

/*************************************************************************
    Constructor
*************************************************************************/
GridLayoutContainer::GridLayoutContainer(const String& type,
                                         const String& name) :
    LayoutContainer(type, name),

    d_gridWidth(0),
    d_gridHeight(0),

    d_autoPositioning(AP_LeftToRight),
    d_nextAutoPositioningIdx(0),

    d_nextGridX(std::numeric_limits<size_t>::max()),
    d_nextGridY(std::numeric_limits<size_t>::max()),

    d_nextDummyIdx(0)
{
    // grid size is 0x0 that means 0 child windows,
    // no need to populate d_children with dummies

    addGridLayoutContainerProperties();
}

//----------------------------------------------------------------------------//
GridLayoutContainer::~GridLayoutContainer(void)
{}

//----------------------------------------------------------------------------//
void GridLayoutContainer::setGridDimensions(size_t width, size_t height)
{
    // copy the old children list
    ChildList oldChildren = d_children;

    // remove all child windows
    while (getChildCount() != 0)
    {
        Window* wnd = static_cast<Window*>(d_children[0]);
        removeChild(wnd);
    }

    // we simply fill the grid with dummies to ensure everything works smoothly
    // when something is added to the grid, it simply replaces the dummy
    for (size_t i = 0; i < width * height; ++i)
    {
        Window* dummy = createDummy();
        addChild(dummy);
    }

    const size_t oldWidth = d_gridWidth;

    const size_t oldHeight = d_gridHeight;

    const AutoPositioning oldAO = d_autoPositioning;

    d_gridWidth = width;

    d_gridHeight = height;

    // now we have to map oldChildren to new children
    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            // we have to skip if we are out of the old grid
            if (x >= oldWidth || y >= oldHeight)
                continue;

            const size_t oldIdx = mapFromGridToIdx(x, y, oldWidth, oldHeight);
            Window* previous = static_cast<Window*>(oldChildren[oldIdx]);

            if (isDummy(previous))
            {
                WindowManager::getSingleton().destroyWindow(previous);
            }
            else
            {
                addChildToPosition(previous, x, y);
            }

            oldChildren[oldIdx] = 0;
        }
    }

    setAutoPositioning(oldAO);
    // oldAOIdx could mean something completely different now!
    // todo: perhaps convert oldAOOdx to new AOIdx?
    setNextAutoPositioningIdx(0);

    // we have to destroy windows that don't fit the new grid if they are set
    // to be destroyed by parent
    for (size_t i = 0; i < oldChildren.size(); ++i)
    {
        if (oldChildren[i] && static_cast<Window*>(oldChildren[i])->isDestroyedByParent())
        {
            WindowManager::getSingleton().destroyWindow(static_cast<Window*>(oldChildren[i]));
        }
    }
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::setGrid(const Sizef &size)
{
    setGridDimensions(static_cast<size_t>(ceil(std::max(0.0f, size.d_width))),
        static_cast<size_t>(ceil(std::max(0.0f, size.d_height))));
}

//----------------------------------------------------------------------------//
size_t GridLayoutContainer::getGridWidth() const
{
    return d_gridWidth;
}

//----------------------------------------------------------------------------//
size_t GridLayoutContainer::getGridHeight() const
{
    return d_gridHeight;
}
//----------------------------------------------------------------------------//
Sizef GridLayoutContainer::getGrid() const
{
    return Sizef(static_cast<float>(getGridWidth()), static_cast<float>(getGridHeight()));
}



//----------------------------------------------------------------------------//
void GridLayoutContainer::setAutoPositioning(AutoPositioning positioning)
{
    d_autoPositioning = positioning;
    d_nextAutoPositioningIdx = 0;
}

//----------------------------------------------------------------------------//
GridLayoutContainer::AutoPositioning GridLayoutContainer::getAutoPositioning() const
{
    return d_autoPositioning;
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::setNextAutoPositioningIdx(size_t idx)
{
    d_nextAutoPositioningIdx = idx;
}

//----------------------------------------------------------------------------//
size_t GridLayoutContainer::getNextAutoPositioningIdx() const
{
    return d_nextAutoPositioningIdx;
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::autoPositioningSkipCells(size_t cells)
{
    setNextAutoPositioningIdx(getNextAutoPositioningIdx() + cells);
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::addChildToPosition(Window* window,
                                             size_t gridX, size_t gridY)
{
    // when user starts to add windows to specific locations, AO has to be disabled
    setAutoPositioning(AP_Disabled);
    d_nextGridX = gridX;
    d_nextGridY = gridY;

    LayoutContainer::addChild(window);
}

//----------------------------------------------------------------------------//
Window* GridLayoutContainer::getChildAtPosition(size_t gridX,
                                                size_t gridY)
{
    assert(gridX < d_gridWidth && "out of bounds");
    assert(gridY < d_gridHeight && "out of bounds");

    return getChildAtIdx(mapFromGridToIdx(gridX, gridY,
                                          d_gridWidth, d_gridHeight));
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::removeChildFromPosition(size_t gridX,
                                                  size_t gridY)
{
    removeChild(getChildAtPosition(gridX, gridY));
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::swapChildPositions(size_t wnd1, size_t wnd2)
{
    if (wnd1 < d_children.size() && wnd2 < d_children.size())
    {
        std::swap(d_children[wnd1], d_children[wnd2]);

        WindowEventArgs args(this);
        onChildOrderChanged(args);
    }
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::swapChildPositions(size_t gridX1, size_t gridY1,
                                             size_t gridX2, size_t gridY2)
{
    swapChildPositions(
        mapFromGridToIdx(gridX1, gridY1, d_gridWidth, d_gridHeight),
        mapFromGridToIdx(gridX2, gridY2, d_gridWidth, d_gridHeight));
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::swapChildren(Window* wnd1, Window* wnd2)
{
    swapChildPositions(getIdxOfChild(wnd1),
                       getIdxOfChild(wnd2));
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::swapChildren(Window* wnd1, const String& wnd2)
{
    swapChildren(wnd1, getChild(wnd2));
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::swapChildren(const String& wnd1, Window* wnd2)
{
    swapChildren(getChild(wnd1), wnd2);
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::moveChildToPosition(Window* wnd,
                                                    size_t gridX, size_t gridY)
{
    removeChild(wnd);
    addChildToPosition(wnd, gridX, gridY);
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::moveChildToPosition(const String& wnd,
                                                    size_t gridX, size_t gridY)
{
    moveChildToPosition(getChild(wnd), gridX, gridY);
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::layout()
{
    std::vector<UDim> colSizes(d_gridWidth, UDim(0, 0));
    std::vector<UDim> rowSizes(d_gridHeight, UDim(0, 0));

    // used to compare UDims
    const float absWidth = getChildContentArea().get().getWidth();
    const float absHeight = getChildContentArea().get().getHeight();

    // first, we need to determine rowSizes and colSizes, this is needed before
    // any layouting work takes place
    for (size_t y = 0; y < d_gridHeight; ++y)
    {
        for (size_t x = 0; x < d_gridWidth; ++x)
        {
            // x and y is the position of window in the grid
            const size_t childIdx =
                mapFromGridToIdx(x, y, d_gridWidth, d_gridHeight);

            Window* window = getChildAtIdx(childIdx);
            const UVector2 size = getBoundingSizeForWindow(window);

            if (CoordConverter::asAbsolute(colSizes[x], absWidth) <
                CoordConverter::asAbsolute(size.d_x, absWidth))
            {
                colSizes[x] = size.d_x;
            }

            if (CoordConverter::asAbsolute(rowSizes[y], absHeight) <
                CoordConverter::asAbsolute(size.d_y, absHeight))
            {
                rowSizes[y] = size.d_y;
            }
        }
    }

    // OK, now in rowSizes[y] is the height of y-th row
    //         in colSizes[x] is the width of x-th column

    // second layouting phase starts now
    for (size_t y = 0; y < d_gridHeight; ++y)
    {
        for (size_t x = 0; x < d_gridWidth; ++x)
        {
            // x and y is the position of window in the grid
            const size_t childIdx = mapFromGridToIdx(x, y,
                                                     d_gridWidth, d_gridHeight);
            Window* window = getChildAtIdx(childIdx);
            const UVector2 offset = getOffsetForWindow(window);
            const UVector2 gridCellOffset = getGridCellOffset(colSizes,
                                                              rowSizes,
                                                              x, y);

            window->setPosition(gridCellOffset + offset);
        }
    }

    // now we just need to determine the total width and height and set it
    setSize(getGridSize(colSizes, rowSizes));
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::onChildOrderChanged(WindowEventArgs& e)
{
    markNeedsLayouting();

    fireEvent(EventChildOrderChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
size_t GridLayoutContainer::mapFromGridToIdx(size_t gridX,
                                             size_t gridY,
                                             size_t gridWidth,
                                             size_t gridHeight) const
{
    // example:
    // d_children = {1, 2, 3, 4, 5, 6}
    // grid is 3x2
    // 1 2 3
    // 4 5 6

    assert(gridX < gridWidth);
    assert(gridY < gridHeight);
    CEGUI_UNUSED(gridHeight); // silence warning in release builds

    return gridY * gridWidth + gridX;
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::mapFromIdxToGrid(size_t idx,
                                           size_t& gridX,
                                           size_t& gridY,
                                           size_t gridWidth,
                                           size_t gridHeight) const
{
    gridX = 0;
    gridY = 0;

    while (idx >= gridWidth)
    {
        idx -= gridWidth;
        ++gridY;
    }

    assert(gridY < gridHeight);
    CEGUI_UNUSED(gridHeight); // silence warning in release builds

    gridX = idx;
}

//----------------------------------------------------------------------------//
UVector2 GridLayoutContainer::getGridCellOffset(
        const std::vector<UDim>& colSizes,
        const std::vector<UDim>& rowSizes,
        size_t gridX, size_t gridY) const
{
    assert(gridX < d_gridWidth);
    assert(gridY < d_gridHeight);

    UVector2 ret(UDim(0, 0), UDim(0, 0));

    for (size_t i = 0; i < gridX; ++i)
    {
        ret.d_x += colSizes[i];
    }

    for (size_t i = 0; i < gridY; ++i)
    {
        ret.d_y += rowSizes[i];
    }

    return ret;
}

//----------------------------------------------------------------------------//
USize GridLayoutContainer::getGridSize(const std::vector<UDim>& colSizes,
        const std::vector<UDim>& rowSizes) const
{
    USize ret(UDim(0, 0), UDim(0, 0));

    for (size_t i = 0; i < colSizes.size(); ++i)
    {
        ret.d_width += colSizes[i];
    }

    for (size_t i = 0; i < rowSizes.size(); ++i)
    {
        ret.d_height += rowSizes[i];
    }

    return ret;
}

//----------------------------------------------------------------------------//
size_t GridLayoutContainer::translateAPToGridIdx(size_t APIdx) const
{
    // todo: more auto positioning variants? will someone use them?

    if (d_autoPositioning == AP_Disabled)
    {
        assert(0);
    }
    else if (d_autoPositioning == AP_LeftToRight)
    {
        // this is the same positioning as implementation
        return APIdx;
    }
    else if (d_autoPositioning == AP_TopToBottom)
    {
        // we want
        // 1 3 5
        // 2 4 6

        size_t x = 0;
        size_t y = 0;
        bool done = false;

        for (x = 0; x < d_gridWidth; ++x)
        {
            for (y = 0; y < d_gridHeight; ++y)
            {
                if (APIdx == 0)
                {
                    done = true;
                    break;
                }

                --APIdx;
            }

            if (done)
            {
                break;
            }
        }

        assert(APIdx == 0);
        return mapFromGridToIdx(x, y, d_gridWidth, d_gridHeight);
    }

    // should never happen
    assert(0);
    return APIdx;
}

//----------------------------------------------------------------------------//
Window* GridLayoutContainer::createDummy()
{
    char i_buff[32];
    sprintf(i_buff, "%llu", static_cast<unsigned long long>(d_nextDummyIdx));
    ++d_nextDummyIdx;

    Window* dummy = WindowManager::getSingleton().createWindow("DefaultWindow",
                    DummyName + String(i_buff));

    dummy->setAutoWindow(true);
    dummy->setVisible(false);
    dummy->setSize(USize(UDim(0, 0), UDim(0, 0)));
    dummy->setDestroyedByParent(true);

    return dummy;
}

//----------------------------------------------------------------------------//
bool GridLayoutContainer::isDummy(Window* wnd) const
{
    // all auto windows inside grid are dummies
    return wnd->isAutoWindow();
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::addChild_impl(Element* element)
{
    Window* wnd = dynamic_cast<Window*>(element);
    
    if (!wnd)
    {
        CEGUI_THROW(InvalidRequestException(
            "GridLayoutContainer can only have Elements of type Window added "
            "as children (Window path: " + getNamePath() + ")."));
    }
    
    if (isDummy(wnd))
    {
        LayoutContainer::addChild_impl(wnd);
    }
    else
    {
        LayoutContainer::addChild_impl(wnd);

        // OK, wnd is already in d_children

        // idx is the future index of the child that's being added
        size_t idx;

        if (d_autoPositioning == AP_Disabled)
        {
            if ((d_nextGridX == std::numeric_limits<size_t>::max()) &&
                (d_nextGridY == std::numeric_limits<size_t>::max()))
            {
                CEGUI_THROW(InvalidRequestException(
                    "Unable to add child without explicit grid position "
                    "because auto positioning is disabled.  Consider using the "
                    "GridLayoutContainer::addChildToPosition functions."));
            }

            idx = mapFromGridToIdx(d_nextGridX, d_nextGridY,
                                   d_gridWidth, d_gridHeight);

            // reset location to sentinel values.
            d_nextGridX = d_nextGridY = std::numeric_limits<size_t>::max();
        }
        else
        {
            idx = translateAPToGridIdx(d_nextAutoPositioningIdx);
            ++d_nextAutoPositioningIdx;
        }

        // we swap the dummy and the added child
        // this essentially places the added child to it's right position and
        // puts the dummy at the end of d_children it will soon get removed from
        std::swap(d_children[idx], d_children[d_children.size() - 1]);

        Window* toBeRemoved = static_cast<Window*>(d_children[d_children.size() - 1]);
        removeChild(toBeRemoved);

        if (toBeRemoved->isDestroyedByParent())
        {
            WindowManager::getSingleton().destroyWindow(toBeRemoved);
        }
    }
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::removeChild_impl(Element* element)
{
    Window* wnd = static_cast<Window*>(element);
    
    if (!isDummy(wnd) && !WindowManager::getSingleton().isLocked())
    {
        // before we remove the child, we must add new dummy and place it
        // instead of the removed child
        Window* dummy = createDummy();
        addChild(dummy);

        const size_t i = getIdxOfChild(wnd);
        std::swap(d_children[i], d_children[d_children.size() - 1]);
    }

    LayoutContainer::removeChild_impl(wnd);
}

//----------------------------------------------------------------------------//
void GridLayoutContainer::addGridLayoutContainerProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(GridLayoutContainer, Sizef,
        "GridSize", "Size of the grid of this layout container. "
        "Value uses the 'w:# h:#' format and will be rounded up because "
        "only integer values are valid as grid size.",
        &GridLayoutContainer::setGrid, &GridLayoutContainer::getGrid, Sizef::zero()
    );
    
    CEGUI_DEFINE_PROPERTY(GridLayoutContainer, AutoPositioning,
        "AutoPositioning", "Sets the method used for auto positioning. "
        "Possible values: 'Disabled', 'Left to Right', 'Top to Bottom'.",
        &GridLayoutContainer::setAutoPositioning, &GridLayoutContainer::getAutoPositioning, GridLayoutContainer::AP_LeftToRight
    );

}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

#ifdef __MINGW32__
    #pragma GCC diagnostic pop
#endif
