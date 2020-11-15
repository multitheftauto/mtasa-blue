/***********************************************************************
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of ProgressBar widget base class
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
#include "CEGUI/widgets/ProgressBar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String ProgressBar::WidgetTypeName("CEGUI/ProgressBar");
const String ProgressBar::EventNamespace("ProgressBar");

/*************************************************************************
	Event name constants
*************************************************************************/
const String ProgressBar::EventProgressChanged( "ProgressChanged" );
const String ProgressBar::EventProgressDone( "ProgressDone" );


/*************************************************************************
	Constructor for ProgressBar class
*************************************************************************/
ProgressBar::ProgressBar(const String& type, const String& name) :
	Window(type, name),
	d_progress(0),
	d_step(0.01f)
{
	addProgressBarProperties();
}


/*************************************************************************
	Destructor for ProgressBar
*************************************************************************/
ProgressBar::~ProgressBar(void)
{
}


/*************************************************************************
	set the current progress.	
*************************************************************************/
void ProgressBar::setProgress(float progress)
{
	// legal progress rangeis : 0.0f <= progress <= 1.0f
	progress = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;

	if (progress != d_progress)
	{
		// update progress and fire off event.
		d_progress = progress;
		WindowEventArgs args(this);
		onProgressChanged(args);

		// if new progress is 100%, fire off the 'done' event as well.
		if (d_progress == 1.0f)
		{
			onProgressDone(args);
		}

	}

}


/*************************************************************************
	event triggered when progress changes	
*************************************************************************/
void ProgressBar::onProgressChanged(WindowEventArgs& e)
{
	invalidate();

	fireEvent(EventProgressChanged, e, EventNamespace);
}


/*************************************************************************
	event triggered when progress reaches 100%	
*************************************************************************/
void ProgressBar::onProgressDone(WindowEventArgs& e)
{
	fireEvent(EventProgressDone, e, EventNamespace);
}

/*************************************************************************
	add properties defined for this class
*************************************************************************/
void ProgressBar::addProgressBarProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(ProgressBar, float,
        "CurrentProgress", "Property to get/set the current progress of the progress bar.  Value is a float  value between 0.0 and 1.0 specifying the progress.",
        &ProgressBar::setProgress, &ProgressBar::getProgress, 0.0f /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(ProgressBar, float,
        "StepSize", "Property to get/set the step size setting for the progress bar.  Value is a float value.",
        &ProgressBar::setStepSize, &ProgressBar::getStepSize, 0.0f
    );
}


} // End of  CEGUI namespace section
