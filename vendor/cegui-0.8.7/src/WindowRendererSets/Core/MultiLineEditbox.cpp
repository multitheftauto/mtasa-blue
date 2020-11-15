/***********************************************************************
    created:    Thu Jul 7 2005
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
#include "CEGUI/WindowRendererSets/Core/MultiLineEditbox.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/widgets/Scrollbar.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/Image.h"
#include "CEGUI/TplWindowRendererProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{

const String FalagardMultiLineEditbox::TypeName("Core/MultiLineEditbox");

const String FalagardMultiLineEditbox::UnselectedTextColourPropertyName( "NormalTextColour" );
const String FalagardMultiLineEditbox::SelectedTextColourPropertyName( "SelectedTextColour" );
const String FalagardMultiLineEditbox::ActiveSelectionColourPropertyName( "ActiveSelectionColour" );
const String FalagardMultiLineEditbox::InactiveSelectionColourPropertyName( "InactiveSelectionColour" );
const float FalagardMultiLineEditbox::DefaultCaretBlinkTimeout(0.66f);

FalagardMultiLineEditbox::FalagardMultiLineEditbox(const String& type) :
    MultiLineEditboxWindowRenderer(type),
    d_blinkCaret(false),
    d_caretBlinkTimeout(DefaultCaretBlinkTimeout),
    d_caretBlinkElapsed(0.0f),
    d_showCaret(true)
{

    CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardMultiLineEditbox,bool,
        "BlinkCaret", "Property to get/set whether the Editbox caret should blink.  "
        "Value is either \"true\" or \"false\".",
        &FalagardMultiLineEditbox::setCaretBlinkEnabled,&FalagardMultiLineEditbox::isCaretBlinkEnabled,
        false);
    CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardMultiLineEditbox,float,
        "BlinkCaretTimeout", "Property to get/set the caret blink timeout / speed.  "
        "Value is a float value indicating the timeout in seconds.",
        &FalagardMultiLineEditbox::setCaretBlinkTimeout,&FalagardMultiLineEditbox::getCaretBlinkTimeout,
        0.66f);
}

Rectf FalagardMultiLineEditbox::getTextRenderArea(void) const
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    const WidgetLookFeel& wlf = getLookNFeel();
    bool v_visible = w->getVertScrollbar()->isVisible();
    bool h_visible = w->getHorzScrollbar()->isVisible();

    // if either of the scrollbars are visible, we might want to use another text rendering area
    if (v_visible || h_visible)
    {
        String area_name("TextArea");

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
            return wlf.getNamedArea(area_name).getArea().getPixelRect(*w);
        }
    }

    // default to plain TextArea
    return wlf.getNamedArea("TextArea").getArea().getPixelRect(*w);
}

void FalagardMultiLineEditbox::cacheEditboxBaseImagery()
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    const StateImagery* imagery;

    // get WidgetLookFeel for the assigned look.
    const WidgetLookFeel& wlf = getLookNFeel();
    // try and get imagery for our current state
    imagery = &wlf.getStateImagery(w->isEffectiveDisabled() ? "Disabled" : (w->isReadOnly() ? "ReadOnly" : "Enabled"));
    // peform the rendering operation.
    imagery->render(*w);
}

void FalagardMultiLineEditbox::cacheCaretImagery(const Rectf& textArea)
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    const Font* fnt = w->getFont();

    // require a font so that we can calculate caret position.
    if (fnt)
    {
        // get line that caret is in
        size_t caretLine = w->getLineNumberFromIndex(w->getCaretIndex());

        const MultiLineEditbox::LineList& d_lines = w->getFormattedLines();

        // if caret line is valid.
        if (caretLine < d_lines.size())
        {
            // calculate pixel offsets to where caret should be drawn
            size_t caretLineIdx = w->getCaretIndex() - d_lines[caretLine].d_startIdx;
            float ypos = caretLine * fnt->getLineSpacing();
            float xpos = fnt->getTextAdvance(w->getText().substr(d_lines[caretLine].d_startIdx, caretLineIdx));

//             // get base offset to target layer for cursor.
//             Renderer* renderer = System::getSingleton().getRenderer();
//             float baseZ = renderer->getZLayer(7) - renderer->getCurrentZ();

            // get WidgetLookFeel for the assigned look.
            const WidgetLookFeel& wlf = getLookNFeel();
            // get caret imagery
            const ImagerySection& caretImagery = wlf.getImagerySection("Caret");

            // calculate finat destination area for caret
            Rectf caretArea;
            caretArea.left(textArea.left() + xpos);
            caretArea.top(textArea.top() + ypos);
            caretArea.setWidth(caretImagery.getBoundingRect(*w).getSize().d_width);
            caretArea.setHeight(fnt->getLineSpacing());
            caretArea.offset(Vector2f(-w->getHorzScrollbar()->getScrollPosition(), -w->getVertScrollbar()->getScrollPosition()));

            // cache the caret image for rendering.
            caretImagery.render(*w, caretArea, 0, &textArea);
        }
    }
}

void FalagardMultiLineEditbox::render()
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    // render general frame and stuff before we handle the text itself
    cacheEditboxBaseImagery();

    // Render edit box text
    Rectf textarea(getTextRenderArea());
    cacheTextLines(textarea);

    // draw caret
    if ((w->hasInputFocus() && !w->isReadOnly()) &&
        (!d_blinkCaret || d_showCaret))
            cacheCaretImagery(textarea);
}

void FalagardMultiLineEditbox::cacheTextLines(const Rectf& dest_area)
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    // text is already formatted, we just grab the lines and render them with the required alignment.
    Rectf drawArea(dest_area);
    float vertScrollPos = w->getVertScrollbar()->getScrollPosition();
    drawArea.offset(Vector2f(-w->getHorzScrollbar()->getScrollPosition(), -vertScrollPos));

    const Font* fnt = w->getFont();

    if (fnt)
    {
        // calculate final colours to use.
        ColourRect colours;
        const float alpha = w->getEffectiveAlpha();
        ColourRect normalTextCol;
        setColourRectToUnselectedTextColour(normalTextCol);
        normalTextCol.modulateAlpha(alpha);
        ColourRect selectTextCol;
        setColourRectToSelectedTextColour(selectTextCol);
        selectTextCol.modulateAlpha(alpha);
        ColourRect selectBrushCol;
        w->hasInputFocus() ? setColourRectToActiveSelectionColour(selectBrushCol) :
                             setColourRectToInactiveSelectionColour(selectBrushCol);
        selectBrushCol.modulateAlpha(alpha);

        const MultiLineEditbox::LineList& d_lines = w->getFormattedLines();
        const size_t numLines = d_lines.size();

        // calculate the range of visible lines
        size_t sidx,eidx;
        sidx = static_cast<size_t>(vertScrollPos / fnt->getLineSpacing());
        eidx = 1 + sidx + static_cast<size_t>(dest_area.getHeight() / fnt->getLineSpacing());
        eidx = ceguimin(eidx, numLines);
        drawArea.d_min.d_y += fnt->getLineSpacing()*static_cast<float>(sidx);

        // for each formatted line.
        for (size_t i = sidx; i < eidx; ++i)
        {
            Rectf lineRect(drawArea);
            const MultiLineEditbox::LineInfo& currLine = d_lines[i];
            String lineText(w->getTextVisual().substr(currLine.d_startIdx, currLine.d_length));

            // offset the font little down so that it's centered within its own spacing
            const float old_top = lineRect.top();
            lineRect.d_min.d_y += (fnt->getLineSpacing() - fnt->getFontHeight()) * 0.5f;

            // if it is a simple 'no selection area' case
            if ((currLine.d_startIdx >= w->getSelectionEndIndex()) ||
                ((currLine.d_startIdx + currLine.d_length) <= w->getSelectionStartIndex()) ||
                (w->getSelectionBrushImage() == 0))
            {
                colours = normalTextCol;
                // render the complete line.
                fnt->drawText(w->getGeometryBuffer(), lineText,
                                lineRect.getPosition(), &dest_area, colours);
            }
            // we have at least some selection highlighting to do
            else
            {
                // Start of actual rendering section.
                String sect;
                size_t sectIdx = 0, sectLen;
                float selStartOffset = 0.0f, selAreaWidth = 0.0f;

                // render any text prior to selected region of line.
                if (currLine.d_startIdx < w->getSelectionStartIndex())
                {
                    // calculate length of text section
                    sectLen = w->getSelectionStartIndex() - currLine.d_startIdx;

                    // get text for this section
                    sect = lineText.substr(sectIdx, sectLen);
                    sectIdx += sectLen;

                    // get the pixel offset to the beginning of the selection area highlight.
                    selStartOffset = fnt->getTextAdvance(sect);

                    // draw this portion of the text
                    colours = normalTextCol;
                    fnt->drawText(w->getGeometryBuffer(), sect,
                                    lineRect.getPosition(), &dest_area, colours);

                    // set position ready for next portion of text
                    lineRect.d_min.d_x += selStartOffset;
                }

                // calculate the length of the selected section
                sectLen = ceguimin(w->getSelectionEndIndex() - currLine.d_startIdx, currLine.d_length) - sectIdx;

                // get the text for this section
                sect = lineText.substr(sectIdx, sectLen);
                sectIdx += sectLen;

                // get the extent to use as the width of the selection area highlight
                selAreaWidth = fnt->getTextAdvance(sect);

                const float text_top = lineRect.top();
                lineRect.top(old_top);

                // calculate area for the selection brush on this line
                lineRect.left(drawArea.left() + selStartOffset);
                lineRect.right(lineRect.left() + selAreaWidth);
                lineRect.bottom(lineRect.top() + fnt->getLineSpacing());

                // render the selection area brush for this line
                colours = selectBrushCol;
                w->getSelectionBrushImage()->render(w->getGeometryBuffer(), lineRect, &dest_area, colours);

                // draw the text for this section
                colours = selectTextCol;
                fnt->drawText(w->getGeometryBuffer(), sect,
                                lineRect.getPosition(), &dest_area, colours);

                lineRect.top(text_top);

                // render any text beyond selected region of line
                if (sectIdx < currLine.d_length)
                {
                    // update render position to the end of the selected area.
                    lineRect.d_min.d_x += selAreaWidth;

                    // calculate length of this section
                    sectLen = currLine.d_length - sectIdx;

                    // get the text for this section
                    sect = lineText.substr(sectIdx, sectLen);

                    // render the text for this section.
                    colours = normalTextCol;
                    fnt->drawText(w->getGeometryBuffer(), sect,
                                    lineRect.getPosition(), &dest_area, colours);
                }
            }

            // update master position for next line in paragraph.
            drawArea.d_min.d_y += fnt->getLineSpacing();
        }
    }
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::setColourRectToUnselectedTextColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(UnselectedTextColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::setColourRectToSelectedTextColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(SelectedTextColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::setColourRectToActiveSelectionColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(ActiveSelectionColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::setColourRectToInactiveSelectionColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(InactiveSelectionColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::setColourRectToOptionalPropertyColour(
                                                const String& propertyName,
                                                ColourRect& colour_rect) const
{
    if (d_window->isPropertyPresent(propertyName))
        colour_rect = 
            d_window->getProperty<ColourRect>(propertyName);
    else
        colour_rect.setColours(0);
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::update(float elapsed)
{
    // do base class stuff
    WindowRenderer::update(elapsed);

    // only do the update if we absolutely have to
    if (d_blinkCaret &&
        !static_cast<MultiLineEditbox*>(d_window)->isReadOnly() &&
        static_cast<MultiLineEditbox*>(d_window)->hasInputFocus())
    {
        d_caretBlinkElapsed += elapsed;

        if (d_caretBlinkElapsed > d_caretBlinkTimeout)
        {
            d_caretBlinkElapsed = 0.0f;
            d_showCaret ^= true;
            // state changed, so need a redraw
            d_window->invalidate();
        }
    }
}

//----------------------------------------------------------------------------//
bool FalagardMultiLineEditbox::isCaretBlinkEnabled() const
{
    return d_blinkCaret;
}

//----------------------------------------------------------------------------//
float FalagardMultiLineEditbox::getCaretBlinkTimeout() const
{
    return d_caretBlinkTimeout;
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::setCaretBlinkEnabled(bool enable)
{
    d_blinkCaret = enable;
}

//----------------------------------------------------------------------------//
void FalagardMultiLineEditbox::setCaretBlinkTimeout(float seconds)
{
    d_caretBlinkTimeout = seconds;
}

//----------------------------------------------------------------------------//
bool FalagardMultiLineEditbox::handleFontRenderSizeChange(const Font* const font)
{
    const bool res = WindowRenderer::handleFontRenderSizeChange(font);

    if (d_window->getFont() == font)
    {
        d_window->invalidate();
        static_cast<MultiLineEditbox*>(d_window)->formatText(true);
        return true;
    }

    return res;
}

} // End of  CEGUI namespace section
