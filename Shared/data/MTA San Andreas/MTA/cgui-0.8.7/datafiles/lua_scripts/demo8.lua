-----------------------------------------
-- Start of handler functions
-----------------------------------------
-----------------------------------------
-- Alpha slider handler (not used!)
-----------------------------------------
function sliderHandler(args)
    CEGUI.System:getSingleton():getDefaultGUIContext():getRootWindow():setAlpha(CEGUI.toSlider(CEGUI.toWindowEventArgs(args).window):getCurrentValue())
end

-----------------------------------------
-- Handler to slide pane
--
-- Here we move the 'Demo8' sheet window
-- and re-position the scrollbar
-----------------------------------------
function panelSlideHandler(args)
    local scroller = CEGUI.toScrollbar(CEGUI.toWindowEventArgs(args).window)
    local demoWnd = CEGUI.System:getSingleton():getDefaultGUIContext():getRootWindow():getChild("Demo8")

    local parentPixelHeight = demoWnd:getParent():getPixelSize().height
    local relHeight = CEGUI.CoordConverter:asRelative(demoWnd:getHeight(), parentPixelHeight)

    scroller:setPosition(CEGUI.UVector2(CEGUI.UDim(0,0), CEGUI.UDim(scroller:getScrollPosition() / relHeight,0)))
    demoWnd:setPosition(CEGUI.UVector2(CEGUI.UDim(0,0), CEGUI.UDim(-scroller:getScrollPosition(),0)))
end

-----------------------------------------
-- Handler to set preview colour when
-- colour selector scrollers change
-----------------------------------------
function colourChangeHandler(args)
    local root = CEGUI.System:getSingleton():getDefaultGUIContext():getRootWindow()
    
    local r = CEGUI.toScrollbar(root:getChild("Demo8/Window1/Controls/Red")):getScrollPosition()
    local g = CEGUI.toScrollbar(root:getChild("Demo8/Window1/Controls/Green")):getScrollPosition()
    local b = CEGUI.toScrollbar(root:getChild("Demo8/Window1/Controls/Blue")):getScrollPosition()
    local col = CEGUI.Colour:new_local(r, g, b, 1)
    local crect = CEGUI.ColourRect(col)

    root:getChild("Demo8/Window1/Controls/ColourSample"):setProperty("ImageColours", CEGUI.PropertyHelper:colourRectToString(crect))
end


-----------------------------------------
-- Handler to add an item to the box
-----------------------------------------
function addItemHandler(args)
    local root = CEGUI.System:getSingleton():getDefaultGUIContext():getRootWindow()

    local text = root:getChild("Demo8/Window1/Controls/Editbox"):getText()
    local cols = CEGUI.PropertyHelper:stringToColourRect(root:getChild("Demo8/Window1/Controls/ColourSample"):getProperty("ImageColours"))

    local newItem = CEGUI.createListboxTextItem(text, 0, nil, false, true)
    newItem:setSelectionBrushImage("TaharezLook/MultiListSelectionBrush")
    newItem:setSelectionColours(cols)

    CEGUI.toListbox(root:getChild("Demo8/Window1/Listbox")):addItem(newItem)
end

-----------------------------------------
-- Script Entry Point
-----------------------------------------
local guiSystem = CEGUI.System:getSingleton()
local schemeMgr = CEGUI.SchemeManager:getSingleton()
local winMgr = CEGUI.WindowManager:getSingleton()

-- load our demo8 scheme
schemeMgr:createFromFile("TaharezLook.scheme");
-- load our demo8 window layout
local root = winMgr:loadLayoutFromFile("Demo8.layout")
-- set the layout as the root
guiSystem:getDefaultGUIContext():setRootWindow(root)
-- set default mouse cursor
guiSystem:getDefaultGUIContext():getMouseCursor():setDefaultImage("TaharezLook/MouseArrow")
-- set the Tooltip type
guiSystem:getDefaultGUIContext():setDefaultTooltipType("TaharezLook/Tooltip")

-- subscribe required events
root:getChild("Demo8/ViewScroll"):subscribeEvent("ScrollPositionChanged", "panelSlideHandler")
root:getChild("Demo8/Window1/Controls/Blue"):subscribeEvent("ScrollPositionChanged", "colourChangeHandler")
root:getChild("Demo8/Window1/Controls/Red"):subscribeEvent("ScrollPositionChanged", "colourChangeHandler")
root:getChild("Demo8/Window1/Controls/Green"):subscribeEvent("ScrollPositionChanged", "colourChangeHandler")
root:getChild("Demo8/Window1/Controls/Add"):subscribeEvent("Clicked", "addItemHandler")
