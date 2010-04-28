/************************************************************************
	filename: 	CEGUIListHeaderSegment.h
	created:	15/6/2004
	author:		Paul D Turner
	
	purpose:	Interface to list header segment class.
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
#ifndef _CEGUIListHeaderSegment_h_
#define _CEGUIListHeaderSegment_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIListHeaderSegmentProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Base class for list header segment window
*/
class CEGUIEXPORT ListHeaderSegment : public Window
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Constants
	*************************************************************************/
	// Event names
	static const String EventSegmentClicked;				//!< Event fired when the segment is clicked.
	static const String EventSplitterDoubleClicked;		//!< Event fired when the sizer/splitter is double-clicked.
	static const String EventSizingSettingChanged;		//!< Event fired when the sizing setting changes.
	static const String EventSortDirectionChanged;		//!< Event fired when the sort direction value changes.
	static const String EventMovableSettingChanged;		//!< Event fired when the movable setting changes.
	static const String EventSegmentDragStart;			//!< Event fired when the segment has started to be dragged.
	static const String EventSegmentDragStop;				//!< Event fired when segment dragging has stopped (via mouse release).
	static const String EventSegmentDragPositionChanged;	//!< Event fired when the segment drag position has changed.
	static const String EventSegmentSized;				//!< Event fired when the segment is sized.
	static const String EventClickableSettingChanged;		//!< Event fired when the clickable state of the segment changes.

	// Defaults
	static const float	DefaultSizingArea;		//!< Default size of the sizing area.
	static const float	SegmentMoveThreshold;	//!< Amount the mouse must be dragged before drag-moving is initiated.


	/*************************************************************************
		Enumerated types
	*************************************************************************/
	/*!
	\brief
		Enumeration of possible values for sorting direction used with ListHeaderSegment classes
	*/
	enum SortDirection
	{
		None,		//!< Items under this segment should not be sorted.
		Ascending,	//!< Items under this segment should be sorted in ascending order.
		Descending	//!< Items under this segment should be sorted in descending order.
	};


	/*************************************************************************
		Accessor Methods
	*************************************************************************/
	/*!
	\brief
		Return whether this segment can be sized.

	\return
		true if the segment can be horizontally sized, false if the segment can not be horizontally sized.
	*/
	bool	isSizingEnabled(void) const			{return d_sizingEnabled;}


	/*!
	\brief
		Return the current sort direction set for this segment.

		Note that this has no impact on the way the segment functions (aside from the possibility
		of varied rendering).  This is intended as a 'helper setting' to classes that make use of
		the ListHeaderSegment objects.

	\return
		One of the SortDirection enumerated values indicating the current sort direction set for this
		segment.
	*/
	SortDirection	getSortDirection(void) const	{return d_sortDir;}


	/*!
	\brief
		Return whether drag moving is enabled for this segment.

	\return
		true if the segment can be drag moved, false if the segment can not be drag moved.
	*/
	bool	isDragMovingEnabled(void) const		{return d_movingEnabled;}


	/*!
	\brief
		Return the current drag move position offset (in pixels relative to the top-left corner of the segment).

	\return
		Point object describing the drag move offset position.
	*/
	const Point&	getDragMoveOffset(void) const	{return d_dragPosition;}


	/*!
	\brief
		Return whether the segment is clickable.

	\return
		true if the segment can be clicked, false of the segment can not be clicked (so no highlighting or events will happen).
	*/
	bool	isClickable(void) const		{return d_allowClicks;}


	/*************************************************************************
		Manipulator Methods
	*************************************************************************/
	/*!
	\brief
		Set whether this segment can be sized.

	\param setting
		true if the segment may be horizontally sized, false if the segment may not be horizontally sized.

	\return
		Nothing.
	*/
	void	setSizingEnabled(bool setting);


	/*!
	\brief
		Set the current sort direction set for this segment.

		Note that this has no impact on the way the segment functions (aside from the possibility
		of varied rendering).  This is intended as a 'helper setting' to classes that make use of
		the ListHeaderSegment objects.

	\param sort_dir
		One of the SortDirection enumerated values indicating the current sort direction set for this
		segment.

	\return
		Nothing
	*/
	void	setSortDirection(SortDirection sort_dir);


	/*!
	\brief
		Set whether drag moving is allowed for this segment.

	\param setting
		true if the segment may be drag moved, false if the segment may not be drag moved.

	\return
		Nothing.
	*/
	void	setDragMovingEnabled(bool setting);


	/*!
	\brief
		Set whether the segment is clickable.

	\param setting
		true if the segment may be clicked, false of the segment may not be clicked (so no highlighting or events will happen).

	\return
		Nothing.
	*/
	void setClickable(bool setting);


	/*************************************************************************
		Construction & Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for list header segment base class
	*/
	ListHeaderSegment(const String& type, const String& name);


	/*!
	\brief
		Destructor for list header segment base class.
	*/
	virtual ~ListHeaderSegment(void);


protected:
	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		Add list header segment specific events
	*/
	void	addHeaderSegmentEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addHeaderSegmentEvents(false); }


	/*!
	\brief
		Update state for drag sizing.

	\param local_mouse
		Mouse position as a pixel offset from the top-left corner of this window.

	\return
		Nothing.
	*/
	void	doDragSizing(const Point& local_mouse);


	/*!
	\brief
		Update state for drag moving.

	\param local_mouse
		Mouse position as a pixel offset from the top-left corner of this window.

	\return
		Nothing.
	*/
	void	doDragMoving(const Point& local_mouse);


	/*!
	\brief
		Initialise the required states to put the widget into drag-moving mode.
	*/
	void	initDragMoving(void);


	/*!
	\brief
		Initialise the required states when we are hovering over the sizing area.
	*/
	void	initSizingHoverState(void);


	/*!
	\brief
		Initialise the required states when we are hovering over the main segment area.
	*/
	void	initSegmentHoverState(void);


	/*!
	\brief
		Return whether the required minimum movement threshold before initiating drag-moving
		has been exceeded.

	\param local_mouse
		Mouse position as a pixel offset from the top-left corner of this window.

	\return
		true if the threshold has been exceeded and drag-moving should be initiated, or false
		if the threshold has not been exceeded.
	*/		
	bool	isDragMoveThresholdExceeded(const Point& local_mouse);


	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"ListHeaderSegment")	return true;
		return Window::testClassName_impl(class_name);
	}


	/*************************************************************************
		New Event Handlers
	*************************************************************************/
	/*!
	\brief
		Handler called when segment is clicked.
	*/
	virtual void	onSegmentClicked(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the sizer/splitter is double-clicked.
	*/
	virtual void	onSplitterDoubleClicked(WindowEventArgs& e);


	/*!
	\brief
		Handler called when sizing setting changes.
	*/
	virtual void	onSizingSettingChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the sort direction value changes.
	*/
	virtual void	onSortDirectionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the drag-movable setting is changed.
	*/
	virtual void	onMovableSettingChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the user starts dragging the segment.
	*/
	virtual void	onSegmentDragStart(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the user stops dragging the segment (releases mouse button)
	*/
	virtual void	onSegmentDragStop(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the drag position changes.
	*/
	virtual void	onSegmentDragPositionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the segment is sized.
	*/
	virtual void	onSegmentSized(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the clickable setting for the segment changes
	*/
	virtual void	onClickableSettingChanged(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event Handlers
	*************************************************************************/
	virtual void	onMouseMove(MouseEventArgs& e);
	virtual void	onMouseButtonDown(MouseEventArgs& e);
	virtual void	onMouseButtonUp(MouseEventArgs& e);
	virtual void	onMouseDoubleClicked(MouseEventArgs& e);
	virtual void	onMouseLeaves(MouseEventArgs& e);
	virtual void	onCaptureLost(WindowEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	const Image*	d_sizingMouseCursor;	//!< Image to use for mouse when sizing (typically set by derived class).
	const Image*	d_movingMouseCursor;	//!< Image to use for mouse when moving (typically set by derived class).

	float	d_splitterSize;		//!< pixel width of the sizing area.
	bool	d_splitterHover;	//!< True if the mouse is over the splitter

	bool	d_dragSizing;		//!< true when we are being sized.
	Point	d_dragPoint;		//!< point we are being dragged at when sizing or moving.

	SortDirection	d_sortDir;	//!< Direction for sorting (used for deciding what icon to display).

	bool	d_segmentHover;		//!< true when the mouse is within the segment area (and not in sizing area).
	bool	d_segmentPushed;	//!< true when the left mouse button has been pressed within the confines of the segment.
	bool	d_sizingEnabled;	//!< true when sizing is enabled for this segment.
	bool	d_movingEnabled;	//!< True when drag-moving is enabled for this segment;
	bool	d_dragMoving;		//!< true when segment is being drag moved.
	Point	d_dragPosition;		//!< position of dragged segment.
	bool	d_allowClicks;		//!< true if the segment can be clicked.


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static ListHeaderSegmentProperties::Clickable		d_clickableProperty;
	static ListHeaderSegmentProperties::Dragable		d_dragableProperty;
	static ListHeaderSegmentProperties::Sizable			d_sizableProperty;
	static ListHeaderSegmentProperties::SortDirection	d_sortDirectionProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addHeaderSegmentProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addHeaderSegmentProperties(false); }
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIListHeaderSegment_h_
