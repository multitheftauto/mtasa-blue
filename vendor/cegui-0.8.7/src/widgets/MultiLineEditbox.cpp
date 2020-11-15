/***********************************************************************
	created:	30/6/2004
	author:		Paul D Turner

	purpose:	Implementation of the Multi-line edit box base class
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
#include "CEGUI/widgets/MultiLineEditbox.h"
#include "CEGUI/widgets/Scrollbar.h"
#include "CEGUI/TextUtils.h"
#include "CEGUI/Image.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/CoordConverter.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Clipboard.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String MultiLineEditbox::EventNamespace("MultiLineEditbox");
const String MultiLineEditbox::WidgetTypeName("CEGUI/MultiLineEditbox");


/*************************************************************************
    MultiLineEditboxWindowRenderer
*************************************************************************/
MultiLineEditboxWindowRenderer::MultiLineEditboxWindowRenderer(const String& name) :
    WindowRenderer(name, MultiLineEditbox::EventNamespace)
{
}

//----------------------------------------------------------------------------//
void MultiLineEditboxWindowRenderer::onLookNFeelAssigned()
{
    assert(d_window != 0);

    // ensure window's text has a terminating \n
    String text(d_window->getText());
    if (text.empty() || text[text.length() - 1] != '\n')
    {
        text.append(1, '\n');
        d_window->setText(text);
    }
}

/*************************************************************************
	TODO:

	Clipboard support
	Undo support
*************************************************************************/
/*************************************************************************
	Constants
*************************************************************************/
// event names
const String MultiLineEditbox::EventReadOnlyModeChanged( "ReadOnlyModeChanged" );
const String MultiLineEditbox::EventWordWrapModeChanged( "WordWrapModeChanged" );
const String MultiLineEditbox::EventMaximumTextLengthChanged( "MaximumTextLengthChanged" );
const String MultiLineEditbox::EventCaretMoved( "CaretMoved" );
const String MultiLineEditbox::EventTextSelectionChanged( "TextSelectionChanged" );
const String MultiLineEditbox::EventEditboxFull( "EditboxFull" );
const String MultiLineEditbox::EventVertScrollbarModeChanged( "VertScrollbarModeChanged" );
const String MultiLineEditbox::EventHorzScrollbarModeChanged( "HorzScrollbarModeChanged" );

// Static data initialisation
String MultiLineEditbox::d_lineBreakChars("\n");

/*************************************************************************
    Child Widget name constants
*************************************************************************/
const String MultiLineEditbox::VertScrollbarName( "__auto_vscrollbar__" );
const String MultiLineEditbox::HorzScrollbarName( "__auto_hscrollbar__" );

/*************************************************************************
	Constructor for the MultiLineEditbox base class.
*************************************************************************/
MultiLineEditbox::MultiLineEditbox(const String& type, const String& name) :
	Window(type, name),
	d_readOnly(false),
	d_maxTextLen(String().max_size()),
	d_caretPos(0),
	d_selectionStart(0),
	d_selectionEnd(0),
	d_dragging(false),
	d_dragAnchorIdx(0),
	d_wordWrap(true),
	d_widestExtent(0.0f),
	d_forceVertScroll(false),
	d_forceHorzScroll(false),
	d_selectionBrush(0)
{
	addMultiLineEditboxProperties();

    // override default and disable text parsing
    d_textParsingEnabled = false;
    // Since parsing is ever allowed in the editbox, ban the property too.
    banPropertyFromXML("TextParsingEnabled");
}


/*************************************************************************
	Destructor for the MultiLineEditbox base class.
*************************************************************************/
MultiLineEditbox::~MultiLineEditbox(void)
{
}


/*************************************************************************
	Initialise the Window based object ready for use.
*************************************************************************/
void MultiLineEditbox::initialiseComponents(void)
{
	// create the component sub-widgets
	Scrollbar* vertScrollbar = getVertScrollbar();
	Scrollbar* horzScrollbar = getHorzScrollbar();

    vertScrollbar->subscribeEvent(
        Window::EventShown, Event::Subscriber(
            &MultiLineEditbox::handle_vertScrollbarVisibilityChanged, this));

    vertScrollbar->subscribeEvent(
        Window::EventHidden, Event::Subscriber(
            &MultiLineEditbox::handle_vertScrollbarVisibilityChanged, this));

    vertScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&MultiLineEditbox::handle_scrollChange, this));
    horzScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&MultiLineEditbox::handle_scrollChange, this));

	formatText(true);
	performChildWindowLayout();
}


/*************************************************************************
	return true if the edit box has input focus.
*************************************************************************/
bool MultiLineEditbox::hasInputFocus(void) const
{
	return isActive();
}


/*************************************************************************
	return the current selection start point.
*************************************************************************/
size_t MultiLineEditbox::getSelectionStartIndex(void) const
{
	return (d_selectionStart != d_selectionEnd) ? d_selectionStart : d_caretPos;
}


/*************************************************************************
	return the current selection end point.
*************************************************************************/
size_t MultiLineEditbox::getSelectionEndIndex(void) const
{
	return (d_selectionStart != d_selectionEnd) ? d_selectionEnd : d_caretPos;
}


/*************************************************************************
	return the length of the current selection (in code points / characters).
*************************************************************************/
size_t MultiLineEditbox::getSelectionLength(void) const
{
	return d_selectionEnd - d_selectionStart;
}


/*************************************************************************
	Specify whether the edit box is read-only.
*************************************************************************/
void MultiLineEditbox::setReadOnly(bool setting)
{
	// if setting is changed
	if (d_readOnly != setting)
	{
		d_readOnly = setting;
		WindowEventArgs args(this);
 		onReadOnlyChanged(args);
	}

}


/*************************************************************************
	Set the current position of the caret.
*************************************************************************/
void MultiLineEditbox::setCaretIndex(size_t caret_pos)
{
	// make sure new position is valid
    if (caret_pos > getText().length() - 1)
	{
       caret_pos = getText().length() - 1;
	}

	// if new position is different
	if (d_caretPos != caret_pos)
	{
		d_caretPos = caret_pos;
		ensureCaretIsVisible();

		// Trigger "caret moved" event
		WindowEventArgs args(this);
		onCaretMoved(args);
	}

}


/*************************************************************************
	Define the current selection for the edit box
*************************************************************************/
void MultiLineEditbox::setSelection(size_t start_pos, size_t end_pos)
{
	// ensure selection start point is within the valid range
    if (start_pos > getText().length() - 1)
	{
       start_pos = getText().length() - 1;
	}

	// ensure selection end point is within the valid range
    if (end_pos > getText().length() - 1)
	{
       end_pos = getText().length() - 1;
	}

	// ensure start is before end
	if (start_pos > end_pos)
	{
		size_t tmp = end_pos;
		end_pos = start_pos;
		start_pos = tmp;
	}

	// only change state if values are different.
	if ((start_pos != d_selectionStart) || (end_pos != d_selectionEnd))
	{
		// setup selection
		d_selectionStart = start_pos;
		d_selectionEnd	 = end_pos;

		// Trigger "selection changed" event
		WindowEventArgs args(this);
		onTextSelectionChanged(args);
	}

}

//----------------------------------------------------------------------------//
void MultiLineEditbox::setSelectionStart(size_t start_pos)
{
    this->setSelection(start_pos,start_pos + this->getSelectionLength());
}
//----------------------------------------------------------------------------//
void MultiLineEditbox::setSelectionLength(size_t length)
{
    this->setSelection(this->getSelectionStartIndex(),this->getSelectionStartIndex() + length);
}


/*************************************************************************
	set the maximum text length for this edit box.
*************************************************************************/
void MultiLineEditbox::setMaxTextLength(size_t max_len)
{
	if (d_maxTextLen != max_len)
	{
		d_maxTextLen = max_len;

		// Trigger max length changed event
		WindowEventArgs args(this);
		onMaximumTextLengthChanged(args);

		// trim string
        if (getText().length() > d_maxTextLen)
		{
            String newText = getText();
            newText.resize(d_maxTextLen);
            setText(newText);

			onTextChanged(args);
		}

	}

}


/*************************************************************************
	Scroll the view so that the current caret position is visible.
*************************************************************************/
void MultiLineEditbox::ensureCaretIsVisible(void)
{
    Scrollbar* vertScrollbar = getVertScrollbar();
    Scrollbar* horzScrollbar = getHorzScrollbar();

	// calculate the location of the caret
	const Font* fnt = getFont();
	size_t caretLine = getLineNumberFromIndex(d_caretPos);

	if (caretLine < d_lines.size())
	{
		Rectf textArea(getTextRenderArea());

		size_t caretLineIdx = d_caretPos - d_lines[caretLine].d_startIdx;

		float ypos = caretLine * fnt->getLineSpacing();
        float xpos = fnt->getTextAdvance(getText().substr(d_lines[caretLine].d_startIdx, caretLineIdx));

		// adjust position for scroll bars
		xpos -= horzScrollbar->getScrollPosition();
		ypos -= vertScrollbar->getScrollPosition();

		// if caret is above window, scroll up
		if (ypos < 0)
		{
			vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition() + ypos);
		}
		// if caret is below the window, scroll down
		else if ((ypos += fnt->getLineSpacing()) > textArea.getHeight())
		{
			vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition() + (ypos - textArea.getHeight()) + fnt->getLineSpacing());
		}

		// if caret is left of the window, scroll left
		if (xpos < 0)
		{
			horzScrollbar->setScrollPosition(horzScrollbar->getScrollPosition() + xpos - 50);
		}
		// if caret is right of the window, scroll right
		else if (xpos > textArea.getWidth())
		{
			horzScrollbar->setScrollPosition(horzScrollbar->getScrollPosition() + (xpos - textArea.getWidth()) + 50);
		}

	}

}


/*************************************************************************
	Set whether the text will be word wrapped or not.
*************************************************************************/
void MultiLineEditbox::setWordWrapping(bool setting)
{
	if (setting != d_wordWrap)
	{
		d_wordWrap = setting;
		formatText(true);

		WindowEventArgs args(this);
		onWordWrapModeChanged(args);
	}

}


/*************************************************************************
	display required integrated scroll bars according to current state
	of the edit box and update their values.
*************************************************************************/
void MultiLineEditbox::configureScrollbars(void)
{
    Scrollbar* const vertScrollbar = getVertScrollbar();
    Scrollbar* const horzScrollbar = getHorzScrollbar();
    const float lspc = getFont()->getLineSpacing();

    //
    // First show or hide the scroll bars as needed (or requested)
    //
    // show or hide vertical scroll bar as required (or as specified by option)
    if (d_forceVertScroll ||
        (static_cast<float>(d_lines.size()) * lspc > getTextRenderArea().getHeight()))
    {
        vertScrollbar->show();

        // show or hide horizontal scroll bar as required (or as specified by option)
        horzScrollbar->setVisible(d_forceHorzScroll ||
                                  (d_widestExtent > getTextRenderArea().getWidth()));
    }
    // show or hide horizontal scroll bar as required (or as specified by option)
    else if (d_forceHorzScroll ||
             (d_widestExtent > getTextRenderArea().getWidth()))
    {
        horzScrollbar->show();

        // show or hide vertical scroll bar as required (or as specified by option)
        vertScrollbar->setVisible(d_forceVertScroll ||
            (static_cast<float>(d_lines.size()) * lspc > getTextRenderArea().getHeight()));
    }
    else
    {
        vertScrollbar->hide();
        horzScrollbar->hide();
    }

	//
	// Set up scroll bar values
	//
	Rectf renderArea(getTextRenderArea());

	vertScrollbar->setDocumentSize(static_cast<float>(d_lines.size()) * lspc);
	vertScrollbar->setPageSize(renderArea.getHeight());
	vertScrollbar->setStepSize(ceguimax(1.0f, renderArea.getHeight() / 10.0f));
	vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition());

	horzScrollbar->setDocumentSize(d_widestExtent);
	horzScrollbar->setPageSize(renderArea.getWidth());
	horzScrollbar->setStepSize(ceguimax(1.0f, renderArea.getWidth() / 10.0f));
	horzScrollbar->setScrollPosition(horzScrollbar->getScrollPosition());
}


/*************************************************************************
	Format the text into lines as needed by the current formatting options.
*************************************************************************/
void MultiLineEditbox::formatText(void)
{
    formatText(true);
}

//----------------------------------------------------------------------------//
void MultiLineEditbox::formatText(const bool update_scrollbars)
{
    // TODO: ASSAF - todo
	// clear old formatting data
	d_lines.clear();
	d_widestExtent = 0.0f;

	String paraText;

	const Font* fnt = getFont();

	if (fnt)
	{
		float areaWidth = getTextRenderArea().getWidth();

		String::size_type	currPos = 0;
		String::size_type	paraLen;
		LineInfo	line;

        while (currPos < getText().length())
		{
           if ((paraLen = getText().find_first_of(d_lineBreakChars, currPos)) == String::npos)
			{
               paraLen = getText().length() - currPos;
			}
			else
			{
				++paraLen -= currPos;
			}

            paraText = getText().substr(currPos, paraLen);

			if (!d_wordWrap || (areaWidth <= 0.0f))
			{
				// no word wrapping, so we are just one long line.
				line.d_startIdx = currPos;
				line.d_length	= paraLen;
				line.d_extent	= fnt->getTextExtent(paraText);
				d_lines.push_back(line);

				// update widest, if needed.
				if (line.d_extent > d_widestExtent)
				{
					d_widestExtent = line.d_extent;
				}

			}
			// must word-wrap the paragraph text
			else
			{
				String::size_type lineIndex = 0;

				// while there is text in the string
				while (lineIndex < paraLen)
				{
					String::size_type  lineLen = 0;
					float lineExtent = 0.0f;

					// loop while we have not reached the end of the paragraph string
					while (lineLen < (paraLen - lineIndex))
					{
						// get cp / char count of next token
						size_t nextTokenSize = getNextTokenLength(paraText, lineIndex + lineLen);

						// get pixel width of the token
						float tokenExtent  = fnt->getTextExtent(paraText.substr(lineIndex + lineLen, nextTokenSize));

						// would adding this token would overflow the available width
						if ((lineExtent + tokenExtent) > areaWidth)
						{
							// Was this the first token?
							if (lineLen == 0)
							{
								// get point at which to break the token
								lineLen = fnt->getCharAtPixel(paraText.substr(lineIndex, nextTokenSize), areaWidth);
							}

							// text wraps, exit loop early with line info up until wrap point
							break;
						}

						// add this token to current line
						lineLen    += nextTokenSize;
						lineExtent += tokenExtent;
					}

					// set up line info and add to collection
					line.d_startIdx = currPos + lineIndex;
					line.d_length	= lineLen;
					line.d_extent	= lineExtent;
					d_lines.push_back(line);

					// update widest, if needed.
					if (lineExtent > d_widestExtent)
					{
						d_widestExtent = lineExtent;
					}

					// update position in string
					lineIndex += lineLen;
				}

			}

			// skip to next 'paragraph' in text
			currPos += paraLen;
		}

	}

    if (update_scrollbars)
        configureScrollbars();

	invalidate();
}


/*************************************************************************
	Return the length of the next token in String 'text' starting at
	index 'start_idx'.
*************************************************************************/
size_t MultiLineEditbox::getNextTokenLength(const String& text, size_t start_idx) const
{
	String::size_type pos = text.find_first_of(TextUtils::DefaultWrapDelimiters, start_idx);

	// handle case where no more whitespace exists (so this is last token)
	if (pos == String::npos)
	{
		return (text.length() - start_idx);
	}
	// handle 'delimiter' token cases
	else if ((pos - start_idx) == 0)
	{
		return 1;
	}
	else
	{
		return (pos - start_idx);
	}

}


/*************************************************************************
	Return the text code point index that is rendered closest to screen
	position 'pt'.
*************************************************************************/
size_t MultiLineEditbox::getTextIndexFromPosition(const Vector2f& pt) const
{
	//
	// calculate final window position to be checked
	//
	Vector2f wndPt = CoordConverter::screenToWindow(*this, pt);

	Rectf textArea(getTextRenderArea());

    wndPt -= textArea.d_min;

	// factor in scroll bar values
	wndPt.d_x += getHorzScrollbar()->getScrollPosition();
	wndPt.d_y += getVertScrollbar()->getScrollPosition();

	size_t lineNumber = static_cast<size_t>(
        ceguimax(0.0f, wndPt.d_y) / getFont()->getLineSpacing());

	if (lineNumber >= d_lines.size())
	{
		lineNumber = d_lines.size() - 1;
	}

    String lineText(getText().substr(d_lines[lineNumber].d_startIdx, d_lines[lineNumber].d_length));

	size_t lineIdx = getFont()->getCharAtPixel(lineText, wndPt.d_x);

	if (lineIdx >= lineText.length() - 1)
	{
		lineIdx = lineText.length() - 1;
	}

	return d_lines[lineNumber].d_startIdx + lineIdx;
}


/*************************************************************************
	Return the line number a given index falls on with the current
	formatting.  Will return last line if index is out of range.
*************************************************************************/
size_t MultiLineEditbox::getLineNumberFromIndex(size_t index) const
{
	size_t lineCount = d_lines.size();

	if (lineCount == 0)
	{
		return 0;
	}
    else if (index >= getText().length() - 1)
	{
		return lineCount - 1;
	}
	else
	{
		size_t indexCount = 0;
		size_t caretLine = 0;

		for (; caretLine < lineCount; ++caretLine)
		{
			indexCount += d_lines[caretLine].d_length;

			if (index < indexCount)
			{
				return caretLine;
			}

		}

	}

	CEGUI_THROW(InvalidRequestException(
        "Unable to identify a line from the given, invalid, index."));
}



/*************************************************************************
	Clear the current selection setting
*************************************************************************/
void MultiLineEditbox::clearSelection(void)
{
	// perform action only if required.
	if (getSelectionLength() != 0)
	{
		setSelection(0, 0);
	}

}


/*************************************************************************
	Erase the currently selected text.
*************************************************************************/
void MultiLineEditbox::eraseSelectedText(bool modify_text)
{
	if (getSelectionLength() != 0)
	{
		// setup new caret position and remove selection highlight.
		setCaretIndex(getSelectionStartIndex());

		// erase the selected characters (if required)
		if (modify_text)
		{
            String newText = getText();
            newText.erase(getSelectionStartIndex(), getSelectionLength());
            setText(newText);

			// trigger notification that text has changed.
			WindowEventArgs args(this);
			onTextChanged(args);
		}

		clearSelection();
	}

}

//----------------------------------------------------------------------------//
bool MultiLineEditbox::performCopy(Clipboard& clipboard)
{
    if (getSelectionLength() == 0)
        return false;
    
    const String selectedText = getText().substr(
        getSelectionStartIndex(), getSelectionLength());
    
    clipboard.setText(selectedText);
    return true;
}

//----------------------------------------------------------------------------//
bool MultiLineEditbox::performCut(Clipboard& clipboard)
{
    if (isReadOnly())
        return false;
    
    if (!performCopy(clipboard))
        return false;
    
    handleDelete();
    return true;
}

//----------------------------------------------------------------------------//
bool MultiLineEditbox::performPaste(Clipboard& clipboard)
{
    if (isReadOnly())
        return false;
    
    String clipboardText = clipboard.getText();
    
    if (clipboardText.empty())
        return false;
    
    // backup current text
    String tmp(getText());
    tmp.erase(getSelectionStartIndex(), getSelectionLength());
    
    // erase selected text
    eraseSelectedText();
    
    // if there is room
    if (getText().length() + clipboardText.length() < d_maxTextLen)
    {
        String newText = getText();
        newText.insert(getCaretIndex(), clipboardText);
        setText(newText);
        
        d_caretPos += clipboardText.length();
        
        WindowEventArgs args(this);
        onTextChanged(args);

        return true;
    }
    else
    {
        // Trigger text box full event
        WindowEventArgs args(this);
        onEditboxFullEvent(args);

        return false;
    }
}

/*************************************************************************
	Processing for backspace key
*************************************************************************/
void MultiLineEditbox::handleBackspace(void)
{
	if (!isReadOnly())
	{
		if (getSelectionLength() != 0)
		{
			eraseSelectedText();
		}
		else if (d_caretPos > 0)
		{
            String newText = getText();
            newText.erase(d_caretPos - 1, 1);
            setCaretIndex(d_caretPos - 1);
            setText(newText);

			WindowEventArgs args(this);
			onTextChanged(args);
		}

	}
}


/*************************************************************************
	Processing for Delete key
*************************************************************************/
void MultiLineEditbox::handleDelete(void)
{
	if (!isReadOnly())
	{
		if (getSelectionLength() != 0)
		{
			eraseSelectedText();
		}
        else if (getCaretIndex() < getText().length() - 1)
		{
            String newText = getText();
            newText.erase(d_caretPos, 1);
            setText(newText);

			ensureCaretIsVisible();

			WindowEventArgs args(this);
			onTextChanged(args);
		}

	}

}


/*************************************************************************
	Processing to move caret one character left
*************************************************************************/
void MultiLineEditbox::handleCharLeft(uint sysKeys)
{
	if (d_caretPos > 0)
	{
		setCaretIndex(d_caretPos - 1);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move caret one word left
*************************************************************************/
void MultiLineEditbox::handleWordLeft(uint sysKeys)
{
	if (d_caretPos > 0)
	{
        setCaretIndex(TextUtils::getWordStartIdx(getText(), getCaretIndex()));
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move caret one character right
*************************************************************************/
void MultiLineEditbox::handleCharRight(uint sysKeys)
{
   if (d_caretPos < getText().length() - 1)
	{
		setCaretIndex(d_caretPos + 1);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move caret one word right
*************************************************************************/
void MultiLineEditbox::handleWordRight(uint sysKeys)
{
   if (d_caretPos < getText().length() - 1)
	{
        setCaretIndex(TextUtils::getNextWordStartIdx(getText(), getCaretIndex()));
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move caret to the start of the text.
*************************************************************************/
void MultiLineEditbox::handleDocHome(uint sysKeys)
{
	if (d_caretPos > 0)
	{
		setCaretIndex(0);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move caret to the end of the text
*************************************************************************/
void MultiLineEditbox::handleDocEnd(uint sysKeys)
{
   if (d_caretPos < getText().length() - 1)
	{
       setCaretIndex(getText().length() - 1);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move caret to the start of the current line.
*************************************************************************/
void MultiLineEditbox::handleLineHome(uint sysKeys)
{
	size_t line = getLineNumberFromIndex(d_caretPos);

	if (line < d_lines.size())
	{
		size_t lineStartIdx = d_lines[line].d_startIdx;

		if (d_caretPos > lineStartIdx)
		{
			setCaretIndex(lineStartIdx);
		}

		if (sysKeys & Shift)
		{
			setSelection(d_caretPos, d_dragAnchorIdx);
		}
		else
		{
			clearSelection();
		}

	}

}


/*************************************************************************
	Processing to move caret to the end of the current line
*************************************************************************/
void MultiLineEditbox::handleLineEnd(uint sysKeys)
{
	size_t line = getLineNumberFromIndex(d_caretPos);

	if (line < d_lines.size())
	{
		size_t lineEndIdx = d_lines[line].d_startIdx + d_lines[line].d_length - 1;

		if (d_caretPos < lineEndIdx)
		{
			setCaretIndex(lineEndIdx);
		}

		if (sysKeys & Shift)
		{
			setSelection(d_caretPos, d_dragAnchorIdx);
		}
		else
		{
			clearSelection();
		}

	}

}


/*************************************************************************
	Processing to move caret up a line.
*************************************************************************/
void MultiLineEditbox::handleLineUp(uint sysKeys)
{
	size_t caretLine = getLineNumberFromIndex(d_caretPos);

	if (caretLine > 0)
	{
        float caretPixelOffset = getFont()->getTextAdvance(getText().substr(d_lines[caretLine].d_startIdx, d_caretPos - d_lines[caretLine].d_startIdx));

		--caretLine;

        size_t newLineIndex = getFont()->getCharAtPixel(getText().substr(d_lines[caretLine].d_startIdx, d_lines[caretLine].d_length), caretPixelOffset);

		setCaretIndex(d_lines[caretLine].d_startIdx + newLineIndex);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move caret down a line.
*************************************************************************/
void MultiLineEditbox::handleLineDown(uint sysKeys)
{
	size_t caretLine = getLineNumberFromIndex(d_caretPos);

	if ((d_lines.size() > 1) && (caretLine < (d_lines.size() - 1)))
	{
        float caretPixelOffset = getFont()->getTextAdvance(getText().substr(d_lines[caretLine].d_startIdx, d_caretPos - d_lines[caretLine].d_startIdx));

		++caretLine;

        size_t newLineIndex = getFont()->getCharAtPixel(getText().substr(d_lines[caretLine].d_startIdx, d_lines[caretLine].d_length), caretPixelOffset);

		setCaretIndex(d_lines[caretLine].d_startIdx + newLineIndex);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caretPos, d_dragAnchorIdx);
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to insert a new line / paragraph.
*************************************************************************/
void MultiLineEditbox::handleNewLine(uint /*sysKeys*/)
{
	if (!isReadOnly())
	{
		// erase selected text
		eraseSelectedText();

		// if there is room
       if (getText().length() - 1 < d_maxTextLen)
		{
            String newText = getText();
            newText.insert(getCaretIndex(), 1, 0x0a);
            setText(newText);

			d_caretPos++;

			WindowEventArgs args(this);
			onTextChanged(args);
		}

	}

}


/*************************************************************************
    Processing to move caret one page up
*************************************************************************/
void MultiLineEditbox::handlePageUp(uint sysKeys)
{
    size_t caretLine = getLineNumberFromIndex(d_caretPos);
    size_t nbLine = static_cast<size_t>(getTextRenderArea().getHeight() / getFont()->getLineSpacing());
    size_t newline = 0;
    if (nbLine < caretLine)
    {
        newline = caretLine - nbLine;
    }
    setCaretIndex(d_lines[newline].d_startIdx);
    if (sysKeys & Shift)
    {
        setSelection(d_caretPos, d_selectionEnd);
    }
    else
    {
        clearSelection();
    }
    ensureCaretIsVisible();
}


/*************************************************************************
    Processing to move caret one page down
*************************************************************************/
void MultiLineEditbox::handlePageDown(uint sysKeys)
{
    size_t caretLine = getLineNumberFromIndex(d_caretPos);
    size_t nbLine =  static_cast<size_t>(getTextRenderArea().getHeight() / getFont()->getLineSpacing());
    size_t newline = caretLine + nbLine;
    if (!d_lines.empty())
    {
        newline = std::min(newline,d_lines.size() - 1);
    }
    setCaretIndex(d_lines[newline].d_startIdx + d_lines[newline].d_length - 1);
    if (sysKeys & Shift)
    {
        setSelection(d_selectionStart, d_caretPos);
    }
    else
    {
        clearSelection();
    }
    ensureCaretIsVisible();
}


/*************************************************************************
	Handler for when a mouse button is pushed
*************************************************************************/
void MultiLineEditbox::onMouseButtonDown(MouseEventArgs& e)
{
	// base class handling
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		// grab inputs
		if (captureInput())
		{
			// handle mouse down
			clearSelection();
			d_dragging = true;
			d_dragAnchorIdx = getTextIndexFromPosition(e.position);
			setCaretIndex(d_dragAnchorIdx);
		}

		++e.handled;
	}

}


/*************************************************************************
	Handler for when mouse button is released
*************************************************************************/
void MultiLineEditbox::onMouseButtonUp(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseButtonUp(e);

	if (e.button == LeftButton)
	{
		releaseInput();
		++e.handled;
	}

}


/*************************************************************************
	Handler for when mouse button is double-clicked
*************************************************************************/
void MultiLineEditbox::onMouseDoubleClicked(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseDoubleClicked(e);

	if (e.button == LeftButton)
	{
        d_dragAnchorIdx = TextUtils::getWordStartIdx(getText(), (d_caretPos == getText().length()) ? d_caretPos : d_caretPos + 1);
        d_caretPos      = TextUtils::getNextWordStartIdx(getText(), d_caretPos);

		// perform actual selection operation.
		setSelection(d_dragAnchorIdx, d_caretPos);

		++e.handled;
	}

}


/*************************************************************************
	Handler for when mouse button is triple-clicked.
*************************************************************************/
void MultiLineEditbox::onMouseTripleClicked(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseTripleClicked(e);

	if (e.button == LeftButton)
	{
		size_t caretLine = getLineNumberFromIndex(d_caretPos);
		size_t lineStart = d_lines[caretLine].d_startIdx;

		// find end of last paragraph
        String::size_type paraStart = getText().find_last_of(d_lineBreakChars, lineStart);

		// if no previous paragraph, selection will start at the beginning.
		if (paraStart == String::npos)
		{
			paraStart = 0;
		}

		// find end of this paragraph
        String::size_type paraEnd = getText().find_first_of(d_lineBreakChars, lineStart);

		// if paragraph has no end, which actually should never happen, fix the
		// erroneous situation and select up to end at end of text.
		if (paraEnd == String::npos)
		{
            String newText = getText();
            newText.append(1, '\n');
            setText(newText);

            paraEnd = getText().length() - 1;
		}

		// set up selection using new values.
		d_dragAnchorIdx = paraStart;
		setCaretIndex(paraEnd);
		setSelection(d_dragAnchorIdx, d_caretPos);
		++e.handled;
	}

}


/*************************************************************************
	Handler for when mouse moves in the window.
*************************************************************************/
void MultiLineEditbox::onMouseMove(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseMove(e);

	if (d_dragging)
	{
		setCaretIndex(getTextIndexFromPosition(e.position));
		setSelection(d_caretPos, d_dragAnchorIdx);
	}

	++e.handled;
}


/*************************************************************************
	Handler for when capture is lost.
*************************************************************************/
void MultiLineEditbox::onCaptureLost(WindowEventArgs& e)
{
	d_dragging = false;

	// base class processing
	Window::onCaptureLost(e);

	++e.handled;
}


/*************************************************************************
	Handler for when character (printable keys) are typed
*************************************************************************/
void MultiLineEditbox::onCharacter(KeyEventArgs& e)
{
    // NB: We are not calling the base class handler here because it propogates
    // inputs back up the window hierarchy, whereas, as a consumer of key
    // events, we want such propogation to cease with us regardless of whether
    // we actually handle the event.

    // fire event.
    fireEvent(EventCharacterKey, e, Window::EventNamespace);

	// only need to take notice if we have focus
	if (e.handled == 0 && hasInputFocus() && !isReadOnly() &&
        getFont()->isCodepointAvailable(e.codepoint))
	{
		// erase selected text
		eraseSelectedText();

		// if there is room
       if (getText().length() - 1 < d_maxTextLen)
		{
           String newText = getText();
           newText.insert(getCaretIndex(), 1, e.codepoint);
           setText(newText);

			d_caretPos++;

			WindowEventArgs args(this);
			onTextChanged(args);

            ++e.handled;
		}
		else
		{
			// Trigger text box full event
			WindowEventArgs args(this);
			onEditboxFullEvent(args);
		}

	}

}


/*************************************************************************
	Handler for when non-printable keys are typed.
*************************************************************************/
void MultiLineEditbox::onKeyDown(KeyEventArgs& e)
{
    // fire event.
    fireEvent(EventKeyDown, e, Window::EventNamespace);

	if (e.handled == 0 && hasInputFocus())
	{
        if (isReadOnly())
        {
            Window::onKeyDown(e);
            return;
        }

		WindowEventArgs args(this);
		switch (e.scancode)
		{
		case Key::LeftShift:
		case Key::RightShift:
			if (getSelectionLength() == 0)
			{
				d_dragAnchorIdx = getCaretIndex();
			}
			break;

		case Key::Backspace:
			handleBackspace();
			break;

		case Key::Delete:
			handleDelete();
			break;

		case Key::Return:
		case Key::NumpadEnter:
			handleNewLine(e.sysKeys);
			break;

		case Key::ArrowLeft:
			if (e.sysKeys & Control)
			{
				handleWordLeft(e.sysKeys);
			}
			else
			{
				handleCharLeft(e.sysKeys);
			}
			break;

		case Key::ArrowRight:
			if (e.sysKeys & Control)
			{
				handleWordRight(e.sysKeys);
			}
			else
			{
				handleCharRight(e.sysKeys);
			}
			break;

		case Key::ArrowUp:
			handleLineUp(e.sysKeys);
			break;

		case Key::ArrowDown:
			handleLineDown(e.sysKeys);
			break;

		case Key::Home:
			if (e.sysKeys & Control)
			{
				handleDocHome(e.sysKeys);
			}
			else
			{
				handleLineHome(e.sysKeys);
			}
			break;

		case Key::End:
			if (e.sysKeys & Control)
			{
				handleDocEnd(e.sysKeys);
			}
			else
			{
				handleLineEnd(e.sysKeys);
			}
			break;

        case Key::PageUp:
            handlePageUp(e.sysKeys);
            break;

        case Key::PageDown:
            handlePageDown(e.sysKeys);
            break;

        default:
            Window::onKeyDown(e);
            return;
		}

		++e.handled;
	}

}


/*************************************************************************
	Handler for when text is programmatically changed.
*************************************************************************/
void MultiLineEditbox::onTextChanged(WindowEventArgs& e)
{
    // ensure last character is a new line
    if ((getText().length() == 0) || (getText()[getText().length() - 1] != '\n'))
    {
        String newText = getText();
        newText.append(1, '\n');
        setText(newText);
    }


    // base class processing
    Window::onTextChanged(e);

    // clear selection
    clearSelection();
    // layout new text
    formatText(true);
    // layout child windows (scrollbars) since text layout may have changed
    performChildWindowLayout();
    // ensure caret is still within the text
    setCaretIndex(getCaretIndex());
    // ensure caret is visible
    // NB: this will already have been called at least once, but since we
    // may have changed the formatting of the text, it needs to be called again.
    ensureCaretIsVisible();

    ++e.handled;
}


/*************************************************************************
	Handler for when widget size is changed.
*************************************************************************/
void MultiLineEditbox::onSized(ElementEventArgs& e)
{
	formatText(true);

	// base class handling
	Window::onSized(e);

	++e.handled;
}


/*************************************************************************
	Handler for mouse wheel changes
*************************************************************************/
void MultiLineEditbox::onMouseWheel(MouseEventArgs& e)
{
	// base class processing.
	Window::onMouseWheel(e);

    Scrollbar* vertScrollbar = getVertScrollbar();
    Scrollbar* horzScrollbar = getHorzScrollbar();

	if (vertScrollbar->isEffectiveVisible() && (vertScrollbar->getDocumentSize() > vertScrollbar->getPageSize()))
	{
		vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition() + vertScrollbar->getStepSize() * -e.wheelChange);
	}
	else if (horzScrollbar->isEffectiveVisible() && (horzScrollbar->getDocumentSize() > horzScrollbar->getPageSize()))
	{
		horzScrollbar->setScrollPosition(horzScrollbar->getScrollPosition() + horzScrollbar->getStepSize() * -e.wheelChange);
	}

	++e.handled;
}

void MultiLineEditbox::onFontChanged(WindowEventArgs& e)
{
    Window::onFontChanged(e);
    formatText(true);
}


bool MultiLineEditbox::validateWindowRenderer(const WindowRenderer* renderer) const
{
	return dynamic_cast<const MultiLineEditboxWindowRenderer*>(renderer) != 0;
}

/*************************************************************************
	Handler called when the read-only state of the edit box changes
*************************************************************************/
void MultiLineEditbox::onReadOnlyChanged(WindowEventArgs& e)
{
	fireEvent(EventReadOnlyModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the word wrap mode for the the edit box changes
*************************************************************************/
void MultiLineEditbox::onWordWrapModeChanged(WindowEventArgs& e)
{
	fireEvent(EventWordWrapModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the maximum text length for the edit box changes
*************************************************************************/
void MultiLineEditbox::onMaximumTextLengthChanged(WindowEventArgs& e)
{
	fireEvent(EventMaximumTextLengthChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the caret moves.
*************************************************************************/
void MultiLineEditbox::onCaretMoved(WindowEventArgs& e)
{
	invalidate();
	fireEvent(EventCaretMoved, e, EventNamespace);
}


/*************************************************************************
	Handler called when the text selection changes
*************************************************************************/
void MultiLineEditbox::onTextSelectionChanged(WindowEventArgs& e)
{
	invalidate();
	fireEvent(EventTextSelectionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the edit box is full
*************************************************************************/
void MultiLineEditbox::onEditboxFullEvent(WindowEventArgs& e)
{
	fireEvent(EventEditboxFull, e, EventNamespace);
}


/*************************************************************************
	Handler called when the 'always show' setting for the vertical
	scroll bar changes.
*************************************************************************/
void MultiLineEditbox::onVertScrollbarModeChanged(WindowEventArgs& e)
{
	invalidate();
	fireEvent(EventVertScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when 'always show' setting for the horizontal scroll
	bar changes.
*************************************************************************/
void MultiLineEditbox::onHorzScrollbarModeChanged(WindowEventArgs& e)
{
	invalidate();
	fireEvent(EventHorzScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Return whether the text in the edit box will be word-wrapped.
*************************************************************************/
bool MultiLineEditbox::isWordWrapped(void) const
{
	return d_wordWrap;
}


/*************************************************************************
	Add new properties for this class
*************************************************************************/
void MultiLineEditbox::addMultiLineEditboxProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;
    
    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, bool,
        "ReadOnly","Property to get/set the read-only setting for the Editbox.  Value is either \"true\" or \"false\".",
        &MultiLineEditbox::setReadOnly, &MultiLineEditbox::isReadOnly, false
    );
    
    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, size_t,
        "CaretIndex","Property to get/set the current caret index.  Value is \"[uint]\".",
        &MultiLineEditbox::setCaretIndex, &MultiLineEditbox::getCaretIndex, 0
    );
    
    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, size_t,
        "SelectionStart","Property to get/set the zero based index of the selection start position within the text.  Value is \"[uint]\".",
        &MultiLineEditbox::setSelectionStart, &MultiLineEditbox::getSelectionStartIndex, 0
    );
    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, size_t,
        "SelectionLength","Property to get/set the length of the selection (as a count of the number of code points selected).  Value is \"[uint]\".",
        &MultiLineEditbox::setSelectionLength, &MultiLineEditbox::getSelectionLength, 0
    );
    
    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, size_t,
        "MaxTextLength","Property to get/set the the maximum allowed text length (as a count of code points).  Value is \"[uint]\".",
        &MultiLineEditbox::setMaxTextLength, &MultiLineEditbox::getMaxTextLength, String().max_size()
    );

    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, bool,
        "WordWrap", "Property to get/set the word-wrap setting of the edit box.  Value is either \"true\" or \"false\".",
        &MultiLineEditbox::setWordWrapping, &MultiLineEditbox::isWordWrapped, true /* TODO: Inconsistency */
    );

    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, Image*,
        "SelectionBrushImage", "Property to get/set the selection brush image for the editbox.  Value should be \"set:[imageset name] image:[image name]\".",
        &MultiLineEditbox::setSelectionBrushImage, &MultiLineEditbox::getSelectionBrushImage, 0
    );

    CEGUI_DEFINE_PROPERTY(MultiLineEditbox, bool,
        "ForceVertScrollbar", "Property to get/set the 'always show' setting for the vertical scroll bar of the list box."
        "Value is either \"true\" or \"false\".",
        &MultiLineEditbox::setShowVertScrollbar, &MultiLineEditbox::isVertScrollbarAlwaysShown, false /* TODO: Inconsistency */
    );
}

/*************************************************************************
    Handler for scroll position changes.
*************************************************************************/
bool MultiLineEditbox::handle_scrollChange(const EventArgs&)
{
    // simply trigger a redraw of the Listbox.
    invalidate();
    return true;
}

/*************************************************************************
    Return a pointer to the vertical scrollbar component widget.
*************************************************************************/
Scrollbar* MultiLineEditbox::getVertScrollbar() const
{
    return static_cast<Scrollbar*>(getChild(VertScrollbarName));
}

/*************************************************************************
	Return whether the vertical scroll bar is always shown.
*************************************************************************/
bool MultiLineEditbox::isVertScrollbarAlwaysShown(void) const
{
	return d_forceVertScroll;
}

/*************************************************************************
    Return a pointer to the horizontal scrollbar component widget.
*************************************************************************/
Scrollbar* MultiLineEditbox::getHorzScrollbar() const
{
    return static_cast<Scrollbar*>(getChild(HorzScrollbarName));
}

/*************************************************************************
    Get the text rendering area
*************************************************************************/
Rectf MultiLineEditbox::getTextRenderArea() const
{
    if (d_windowRenderer != 0)
    {
        MultiLineEditboxWindowRenderer* wr = (MultiLineEditboxWindowRenderer*)d_windowRenderer;
        return wr->getTextRenderArea();
    }
    else
    {
        //return getTextRenderArea_impl();
        CEGUI_THROW(InvalidRequestException(
            "This function must be implemented by the window renderer module"));
    }
}

const Image* MultiLineEditbox::getSelectionBrushImage() const
{
    return d_selectionBrush;
}

void MultiLineEditbox::setSelectionBrushImage(const Image* image)
{
    d_selectionBrush = image;
    invalidate();
}

/*************************************************************************
	Set whether the vertical scroll bar should always be shown.
*************************************************************************/
void MultiLineEditbox::setShowVertScrollbar(bool setting)
{
	if (d_forceVertScroll != setting)
	{
		d_forceVertScroll = setting;

		configureScrollbars();
		WindowEventArgs args(this);
		onVertScrollbarModeChanged(args);
	}
}

//----------------------------------------------------------------------------//
bool MultiLineEditbox::handle_vertScrollbarVisibilityChanged(const EventArgs&)
{
    if (d_wordWrap)
        formatText(false);

    return true;
}


} // End of  CEGUI namespace section
