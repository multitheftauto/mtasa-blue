/************************************************************************
	filename: 	CEGUIProgressBar.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of ProgressBar widget base class
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
#include "elements/CEGUIProgressBar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String ProgressBar::EventNamespace("ProgressBar");

/*************************************************************************
	Definitions of Properties for this class
*************************************************************************/
ProgressBarProperties::CurrentProgress	ProgressBar::d_currentProgressProperty;
ProgressBarProperties::StepSize			ProgressBar::d_stepSizeProperty;


/*************************************************************************
	Event name constants
*************************************************************************/
const String ProgressBar::EventProgressChanged( (utf8*)"ProgressChanged" );
const String ProgressBar::EventProgressDone( (utf8*)"ProgressDone" );


/*************************************************************************
	Constructor for ProgressBar class
*************************************************************************/
ProgressBar::ProgressBar(const String& type, const String& name) :
	Window(type, name),
	d_progress(0),
	d_step(0.01f)
{
	addProgressBarEvents();
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
	Add progress bar specific events to the window	
*************************************************************************/
void ProgressBar::addProgressBarEvents(bool bCommon)
{
	if ( bCommon == false )	addEvent(EventProgressChanged);
	if ( bCommon == false )	addEvent(EventProgressDone);
}

/*************************************************************************
	event triggered when progress changes	
*************************************************************************/
void ProgressBar::onProgressChanged(WindowEventArgs& e)
{
	requestRedraw();

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
void ProgressBar::addProgressBarProperties( bool bCommon )
{
	if ( bCommon == false )   addProperty(&d_stepSizeProperty);
	if ( bCommon == false )   addProperty(&d_currentProgressProperty);
}


} // End of  CEGUI namespace section
