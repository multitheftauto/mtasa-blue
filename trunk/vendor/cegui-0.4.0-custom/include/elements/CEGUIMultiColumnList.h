/************************************************************************
	filename: 	CEGUIMultiColumnList.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for MultiColumnList widget
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
#ifndef _CEGUIMultiColumnList_h_
#define _CEGUIMultiColumnList_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "CEGUIListHeader.h"
#include "elements/CEGUIMultiColumnListProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Simple grid index structure.
*/
struct CEGUIEXPORT MCLGridRef
{
	MCLGridRef(uint r, uint c) : row(r), column(c) {}

	uint	row;		//!< Zero based row index.
	uint	column;		//!< Zero based column index.

	// operators
	MCLGridRef& operator=(const MCLGridRef& rhs);
	bool operator<(const MCLGridRef& rhs) const;
	bool operator<=(const MCLGridRef& rhs) const;
	bool operator>(const MCLGridRef& rhs) const;
	bool operator>=(const MCLGridRef& rhs) const;
	bool operator==(const MCLGridRef& rhs) const;
	bool operator!=(const MCLGridRef& rhs) const;
};


/*!
\brief
	Base class for the multi column list widget.
*/
class CEGUIEXPORT MultiColumnList : public Window
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Constants
	*************************************************************************/
	// Event names
	static const String EventSelectionModeChanged;		//!< Event fired when the selection mode for the list box changes.
	static const String EventNominatedSelectColumnChanged;//!< Event fired when the nominated select column changes.
	static const String EventNominatedSelectRowChanged;	//!< Event fired when the nominated select row changes.
	static const String EventVertScrollbarModeChanged;	//!< Event fired when the vertical scroll bar 'force' setting changes.
	static const String EventHorzScrollbarModeChanged;	//!< Event fired when the horizontal scroll bar 'force' setting changes.
	static const String EventSelectionChanged;			//!< Event fired when the current selection(s) within the list box changes.
	static const String EventListContentsChanged;			//!< Event fired when the contents of the list box changes.
	static const String EventSortColumnChanged;			//!< Event fired when the sort column changes.
	static const String EventSortDirectionChanged;		//!< Event fired when the sort direction changes.
	static const String EventListColumnSized;				//!< Event fired when the width of a column in the list changes.
	static const String EventListColumnMoved;				//!< Event fired when the column order changes.


	/*************************************************************************
		Enumerations
	*************************************************************************/
	/*!
	\brief
		Enumerated values for the selection modes possible with a Multi-column list
	*/
	enum SelectionMode
	{
		RowSingle,					// Any single row may be selected.  All items in the row are selected.
		RowMultiple,				// Multiple rows may be selected.  All items in the row are selected.
		CellSingle,					// Any single cell may be selected.
		CellMultiple,				// Multiple cells bay be selected.
		NominatedColumnSingle,		// Any single item in a nominated column may be selected.
		NominatedColumnMultiple,	// Multiple items in a nominated column may be selected.
		ColumnSingle,				// Any single column may be selected.  All items in the column are selected.
		ColumnMultiple,				// Multiple columns may be selected.  All items in the column are selected.
		NominatedRowSingle,			// Any single item in a nominated row may be selected.
		NominatedRowMultiple		// Multiple items in a nominated row may be selected.
	};


	/*************************************************************************
		Accessor Methods
	*************************************************************************/
	/*!
	\brief
		Return whether user manipulation of the sort column and direction are enabled.

	\return
		true if the user may interactively modify the sort column and direction.  false if the user may not
		modify the sort column and direction (these can still be set programmatically).
	*/
	bool	isUserSortControlEnabled(void) const;


	/*!
	\brief
		Return whether the user may size column segments.

	\return
		true if the user may interactively modify the width of columns, false if they may not.
	*/
	bool	isUserColumnSizingEnabled(void) const;

    
    /*!
    \Added to MTA by Talidan
        Return whether a specific column is sizable
    \return
        true if the user may interactively modify the width of column, false if they may not.
    */
    bool	isUserColumnSegmentSizingEnabled(uint col_idx) const;

	/*!
	\brief
		Return whether the user may modify the order of the columns.

	\return
		true if the user may interactively modify the order of the columns, false if they may not.
	*/
	bool	isUserColumnDraggingEnabled(void) const;


	/*!
	\brief
		Return the number of columns in the multi-column list

	\return
		uint value equal to the number of columns in the list.
	*/
	uint	getColumnCount(void) const;


	/*!
	\brief
		Return the number of rows in the multi-column list.

	\return
		uint value equal to the number of rows currently in the list.
	*/
	uint	getRowCount(void) const;


	/*!
	\brief
		Return the zero based index of the current sort column.  There must be at least one column to successfully call this
		method.

	\return
		Zero based column index that is the current sort column.

	\exception	InvalidRequestException		thrown if there are no columns in this multi column list.
	*/
	uint	getSortColumn(void) const;

	
	/*!
	\brief
		Return the zero based column index of the column with the specified ID.

	\param col_id
		ID code of the column whos index is to be returned.

	\return
		Zero based column index of the first column whos ID matches \a col_id.

	\exception	InvalidRequestException		thrown if no attached column has the requested ID.
	*/
	uint	getColumnWithID(uint col_id) const;


	/*!
	\brief
		Return the zero based index of the column whos header text matches the specified text.

	\param text
		String object containing the text to be searched for.

	\return
		Zero based column index of the column whos header has the specified text.

	\exception InvalidRequestException	thrown if no columns header has the requested text.
	*/
	uint	getColumnWithHeaderText(const String& text) const;


	/*!
	\brief
		Return the total width of all column headers.

	\return
		Sum total of all the column header widths in whichever metrics system is active.
	*/
	float	getTotalColumnHeadersWidth(void) const;


	/*!
	\brief
		Return the width of the specified column header (and therefore the column itself).

	\param col_idx
		Zero based column index of the column whos width is to be returned.

	\return
		Width of the column header at the zero based column index specified by \a col_idx, in whichever is the active metrics system.

	\exception InvalidRequestException	thrown if \a column is out of range.
	*/
	float	getColumnHeaderWidth(uint col_idx) const;


	/*!
	\brief
		Return the currently set sort direction.

	\return
		One of the ListHeaderSegment::SortDirection enumerated values specifying the current sort direction.
	*/
	ListHeaderSegment::SortDirection	getSortDirection(void) const;


	/*!
	\brief
		Return the ListHeaderSegment object for the specified column

	\param col_idx
		zero based index of the column whos ListHeaderSegment is to be returned.

	\return
		ListHeaderSegment object for the column at the requested index.

	\exception InvalidRequestException	thrown if \a col_idx is out of range.
	*/
	ListHeaderSegment&	getHeaderSegmentForColumn(uint col_idx) const;


	/*!
	\brief
		Return the zero based index of the Row that contains \a item.

	\param item
		Pointer to the ListboxItem that the row index is to returned for.

	\return
		Zero based index of the row that contains ListboxItem \a item.

	\exception InvalidRequestException	thrown if \a item is not attached to the list box.
	*/
	uint	getItemRowIndex(const ListboxItem* item) const;


	/*!
	\brief
		Return the current zero based index of the column that contains \a item.

	\param item
		Pointer to the ListboxItem that the column index is to returned for.

	\return
		Zero based index of the column that contains ListboxItem \a item.

	\exception InvalidRequestException	thrown if \a item is not attached to the list box.
	*/
	uint	getItemColumnIndex(const ListboxItem* item) const;


	/*!
	\brief
		Return the grid reference for \a item.

	\param item
		Pointer to the ListboxItem whos current grid reference is to be returned.

	\return
		MCLGridRef object describing the current grid reference of ListboxItem \a item.

	\exception InvalidRequestException	thrown if \a item is not attached to the list box.
	*/
	MCLGridRef	getItemGridReference(const ListboxItem* item) const;

	
	/*!
	\brief
		Return a pointer to the ListboxItem at the specified grid reference.

	\param grid_ref
		MCLGridRef object that describes the position of the ListboxItem to be returned.

	\return
		Pointer to the ListboxItem at grid reference \a grid_ref.

	\exception InvalidRequestException	thrown if \a grid_ref is invalid for this list box.
	*/
	ListboxItem*	getItemAtGridReference(const MCLGridRef& grid_ref) const;


	/*!
	\brief
		return whether ListboxItem \a item is attached to the column at index \a col_idx.

	\param item
		Pointer to the ListboxItem to look for.

	\param col_idx
		Zero based index of the column that is to be searched.

	\return
		- true if \a item is attached to list box column \a col_idx.
		- false if \a item is not attached to list box column \a col_idx.

	\exception InvalidRequestException	thrown if \a col_idx is out of range.
	*/
	bool	isListboxItemInColumn(const ListboxItem* item, uint col_idx) const;


	/*!
	\brief
		return whether ListboxItem \a item is attached to the row at index \a row_idx.

	\param item
		Pointer to the ListboxItem to look for.

	\param row_idx
		Zero based index of the row that is to be searched.

	\return
		- true if \a item is attached to list box row \a row_idx.
		- false if \a item is not attached to list box row \a row_idx.

	\exception InvalidRequestException	thrown if \a row_idx is out of range.
	*/
	bool	isListboxItemInRow(const ListboxItem* item, uint row_idx) const;


	/*!
	\brief
		return whether ListboxItem \a item is attached to the list box.

	\param item
		Pointer to the ListboxItem to look for.

	\return
		- true if \a item is attached to list box.
		- false if \a item is not attached to list box.
	*/
	bool	isListboxItemInList(const ListboxItem* item) const;


	/*!
	\brief
		Return the ListboxItem in column \a col_idx that has the text string \a text.

	\param text
		String object containing the text to be searched for.

	\param col_idx
		Zero based index of the column to be searched.

	\param start_item
		Pointer to the ListboxItem where the exclusive search is to start, or NULL to search from the top of the column.

	\return
		Pointer to the first ListboxItem in column \a col_idx, after \a start_item, that has the string \a text.

	\exception InvalidRequestException	thrown if \a start_item is not attached to the list box, or if \a col_idx is out of range.
	*/
	ListboxItem*	findColumnItemWithText(const String& text, uint col_idx, const ListboxItem* start_item) const;


	/*!
	\brief
		Return the ListboxItem in row \a row_idx that has the text string \a text.

	\param text
		String object containing the text to be searched for.

	\param row_idx
		Zero based index of the row to be searched.

	\param start_item
		Pointer to the ListboxItem where the exclusive search is to start, or NULL to search from the start of the row.

	\return
		Pointer to the first ListboxItem in row \a row_idx, after \a start_item, that has the string \a text.

	\exception InvalidRequestException	thrown if \a start_item is not attached to the list box, or if \a row_idx is out of range.
	*/
	ListboxItem*	findRowItemWithText(const String& text, uint row_idx, const ListboxItem* start_item) const;


	/*!
	\brief
		Return the ListboxItem that has the text string \a text.
		
	\note
		List box searching progresses across the columns in each row.

	\param text
		String object containing the text to be searched for.

	\param start_item
		Pointer to the ListboxItem where the exclusive search is to start, or NULL to search the whole list box.

	\return
		Pointer to the first ListboxItem, after \a start_item, that has the string \a text.

	\exception InvalidRequestException	thrown if \a start_item is not attached to the list box.
	*/
	ListboxItem*	findListItemWithText(const String& text, const ListboxItem* start_item) const;


	/*!
	\brief
		Return a pointer to the first selected ListboxItem attached to this list box.

	\note
		List box searching progresses across the columns in each row.

	\return
		Pointer to the first ListboxItem attached to this list box that is selected, or NULL if no item is selected.
	*/
	ListboxItem*	getFirstSelectedItem(void) const;


	/*!
	\brief
		Return a pointer to the next selected ListboxItem after \a start_item.
		
	\note
		List box searching progresses across the columns in each row.

	\param start_item
		Pointer to the ListboxItem where the exclusive search is to start, or NULL to search the whole list box.

	\return
		Pointer to the first selected ListboxItem attached to this list box, after \a start_item, or NULL if no item is selected.

	\exception InvalidRequestException	thrown if \a start_item is not attached to the list box.
	*/
	ListboxItem*	getNextSelected(const ListboxItem* start_item) const;


	/*!
	\brief
		Return the number of selected ListboxItems attached to this list box.

	return
		uint value equal to the number of ListboxItems attached to this list box that are currently selected.
	*/
	uint	getSelectedCount(void) const;


	/*!
	\brief
		Return whether the ListboxItem at \a grid_ref is selected.

	\param grid_ref
		MCLGridRef object describing the grid reference that is to be examined.

	\return
		- true if there is a ListboxItem at \a grid_ref and it is selected.
		- false if there is no ListboxItem at \a grid_ref, or if the item is not selected.
	
	\exception InvalidRequestException	thrown if \a grid_ref contains an invalid grid position.
	*/
	bool	isItemSelected(const MCLGridRef& grid_ref) const;


	/*!
	\brief
		Return the ID of the currently set nominated selection column to be used when in one of the NominatedColumn*
		selection modes.

	\note
		You should only ever call this when getColumnCount() returns > 0.

	\return
		ID code of the nominated selection column.
	*/
	uint	getNominatedSelectionColumnID(void) const;


	/*!
	\brief
		Return the index of the currently set nominated selection column to be used when in one of the NominatedColumn*
		selection modes.

	\return
		Zero based index of the nominated selection column.
	*/
	uint	getNominatedSelectionColumn(void) const;


	/*!
	\brief
		Return the index of the currently set nominated selection row to be used when in one of the NominatedRow*
		selection modes.

	\return
		Zero based index of the nominated selection column.
	*/
	uint	getNominatedSelectionRow(void) const;


	/*!
	\brief
		Return the currently set selection mode.

	\return
		One of the MultiColumnList::SelectionMode enumerated values specifying the current selection mode.
	*/
	MultiColumnList::SelectionMode	getSelectionMode(void) const;


	/*!
	\brief
		Return whether the vertical scroll bar is always shown.

	\return
		- true if the scroll bar will always be shown even if it is not required.
		- false if the scroll bar will only be shown when it is required.
	*/
	bool	isVertScrollbarAlwaysShown(void) const;


	/*!
	\brief
		Return whether the horizontal scroll bar is always shown.

	\return
		- true if the scroll bar will always be shown even if it is not required.
		- false if the scroll bar will only be shown when it is required.
	*/
	bool	isHorzScrollbarAlwaysShown(void) const;


	/*!
	\brief
		Return the ID code assigned to the requested column.

	\param col_idx
		Zero based index of the column whos ID code is to be returned.

	\return
		Current ID code assigned to the column at the requested index.

	\exception InvalidRequestException	thrown if \a col_idx is out of range
	*/
	uint	getColumnID(uint col_idx) const;


	/*!
	\brief
		Return the ID code assigned to the requested row.

	\param row_idx
		Zero based index of the row who's ID code is to be returned.

	\return
		Current ID code assigned to the row at the requested index.

	\exception InvalidRequestException	thrown if \a row_idx is out of range
	*/
	uint	getRowID(uint row_idx) const;


	/*!
	\brief
		Return the zero based row index of the row with the specified ID.

	\param row_id
		ID code of the row who's index is to be returned.

	\return
		Zero based row index of the first row who's ID matches \a row_id.

	\exception	InvalidRequestException		thrown if no row has the requested ID.
	*/
	uint	getRowWithID(uint row_id) const;


	/*************************************************************************
		Manipulator Methods
	*************************************************************************/
	/*!
	\brief
		Initialise the Window based object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual void	initialise(void);


	/*!
	\brief
		Remove all items from the list.

		Note that this will cause 'AutoDelete' items to be deleted.
	*/
	void	resetList(void);


	/*!
	\brief
		Add a column to the list box.

	\param text
		String object containing the text label for the column header.

	\param col_id
		ID code to be assigned to the column header.

	\param width
		Initial width to be set for the column using the active metrics mode for this window.

	\return
		Nothing.
	*/
	void	addColumn(const String& text, uint col_id, float width);


	/*!
	\brief
		Insert a new column in the list.

	\param text
		String object containing the text label for the column header.

	\param col_id
		ID code to be assigned to the column header.

	\param width
		Initial width to be set for the column using the active metrics mode for this window.

	\param position
		Zero based index where the column is to be inserted.  If this is greater than the current
		number of columns, the new column is inserted at the end.

	\return
		Nothing.
	*/
	void	insertColumn(const String& text, uint col_id, float width, uint position);


	/*!
	\brief
		Removes a column from the list box.  This will cause any ListboxItem using the autoDelete option in the column to be deleted.

	\param col_idx
		Zero based index of the column to be removed.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if \a col_idx is invalid.
	*/
	void	removeColumn(uint col_idx);


	/*!
	\brief
		Removes a column from the list box.  This will cause any ListboxItem using the autoDelete option in the column to be deleted.

	\param col_id
		ID code of the column to be deleted.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if no column with \a col_id is available on this list box.
	*/
	void	removeColumnWithID(uint col_id);


	/*!
	\brief
		Move the column at index \a col_idx so it is at index \a position.

	\param col_idx
		Zero based index of the column to be moved.

	\param position
		Zero based index of the new position for the column.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if \a col_idx is invalid.
	*/
	void	moveColumn(uint col_idx, uint position);


	/*!
	\brief
		Move the column with ID \a col_id so it is at index \a position.

	\param col_id
		ID code of the column to be moved.

	\param position
		Zero based index of the new position for the column.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if no column with \a col_id is available on this list box.
	*/
	void	moveColumnWithID(uint col_id, uint position);


	/*!
	\brief
		Add an empty row to the list box.

	\param row_id
		ID code to be assigned to the new row.

	\note
		If the list is being sorted, the new row will appear at an appropriate position according to the sorting being
		applied.  If no sorting is being done, the new row will appear at the bottom of the list.

	\return
		Initial zero based index of the new row.
	*/
	uint	addRow(uint row_id = 0, bool fast = false);


	/*!
	\brief
		Add a row to the list box, and set the item in the column with ID \a col_id to \a item.

	\note
		If the list is being sorted, the new row will appear at an appropriate position according to the sorting being
		applied.  If no sorting is being done, the new row will appear at the bottom of the list.

	\param item
		Pointer to a ListboxItem to be used as the initial contents for the column with ID \a col_id.

	\param col_id
		ID code of the column whos initial item is to be set to \a item.

	\param row_id
		ID code to be assigned to the new row.

	\return
		Initial zero based index of the new row.

	\exception InvalidRequestException	thrown if no column with the specified ID is attached to the list box.
	*/
	uint	addRow(ListboxItem* item, uint col_id, uint row_id = 0, bool fast = false);


	/*!
	\brief
		Insert an empty row into the list box.

	\note
		If the list is being sorted, the new row will appear at an appropriate position according to the sorting being
		applied.  If no sorting is being done, the new row will appear at the specified index.

	\param row_idx
		Zero based index where the row should be inserted.  If this is greater than the current number of rows, the row is
		appended to the list.

	\param row_id
		ID code to be assigned to the new row.

	\return
		Zero based index where the row was actually inserted.
	*/
	uint	insertRow(uint row_idx, uint row_id = 0);


	/*!
	\brief
		Insert a row into the list box, and set the item in the column with ID \a col_id to \a item.

	\note
		If the list is being sorted, the new row will appear at an appropriate position according to the sorting being
		applied.  If no sorting is being done, the new row will appear at the specified index.

	\param item
		Pointer to a ListboxItem to be used as the initial contents for the column with ID \a col_id.

	\param col_id
		ID code of the column whos initial item is to be set to \a item.

	\param row_idx
		Zero based index where the row should be inserted.  If this is greater than the current number of rows, the row is
		appended to the list.

	\param row_id
		ID code to be assigned to the new row.

	\return
		Zero based index where the row was actually inserted.

	\exception InvalidRequestException	thrown if no column with the specified ID is attached to the list box.
	*/
	uint	insertRow(ListboxItem* item, uint col_id, uint row_idx, uint row_id = 0);


	/*!
	\brief
		Remove the list box row with index \a row_idx.  Any ListboxItem in row \a row_idx using autoDelete mode will be deleted.

	\param row_idx
		Zero based index of the row to be removed.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if \a row_idx is invalid.
	*/
	void	removeRow(uint row_idx);


	/*!
	\brief
		Set the ListboxItem for grid reference \a position.

	\param item
		Pointer to the ListboxItem to be set at \a position.

	\param position
		MCLGridRef describing the grid reference of the item to be set.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if \a position contains an invalid grid reference.
	*/
	void	setItem(ListboxItem* item, const MCLGridRef& position, bool fast = false);


    /*
    Added by eAi for MTA
    */
    void    forceUpdate();

	/*!
	\brief
		Set the ListboxItem for the column with ID \a col_id in row \a row_idx.

	\param item
		Pointer to the ListboxItem to be set into the list.

	\param col_id
		ID code of the column to receive \a item.

	\param row_idx
		Zero based index of the row to receive \a item.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if no column with ID \a col_id exists, or of \a row_idx is out of range.
	*/
	void	setItem(ListboxItem* item, uint col_id, uint row_idx, bool fast = false);


	/*!
	\brief
		Set the selection mode for the list box.

	\param sel_mode
		One of the MultiColumnList::SelectionMode enumerated values specifying the selection mode to be used.

	\return
		Nothing.

	\exception	InvalidRequestException	thrown if the value specified for \a sel_mode is invalid.
	*/
	void	setSelectionMode(MultiColumnList::SelectionMode sel_mode);


	/*!
	\brief
		Set the column to be used for the NominatedColumn* selection modes.

	\param	col_id
		ID code of the column to be used in NominatedColumn* selection modes.

	\return	
		Nothing.

	\exception InvalidRequestException	thrown if no column has ID code \a col_id.
	*/
	void	setNominatedSelectionColumnID(uint col_id);


	/*!
	\brief
		Set the column to be used for the NominatedColumn* selection modes.

	\param	col_idx
		zero based index of the column to be used in NominatedColumn* selection modes.

	\return	
		Nothing.

	\exception InvalidRequestException	thrown if \a col_idx is out of range.
	*/
	void	setNominatedSelectionColumn(uint col_idx);


	/*!
	\brief
		Set the row to be used for the NominatedRow* selection modes.

	\param	row_idx
		zero based index of the row to be used in NominatedRow* selection modes.

	\return	
		Nothing.

	\exception InvalidRequestException	thrown if \a row_idx is out of range.
	*/
	void	setNominatedSelectionRow(uint row_idx);


	/*!
	\brief
		Set the sort direction to be used.

	\param direction
		One of the ListHeaderSegment::SortDirection enumerated values specifying the sort direction to be used.

	\return
		Nothing.
	*/
	void	setSortDirection(ListHeaderSegment::SortDirection direction);


	/*!
	\brief
		Set the column to be used as the sort key.

	\param col_idx
		Zero based index of the column to use as the key when sorting the list items.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if col_idx is out of range.
	*/
	void	setSortColumn(uint col_idx);


	/*!
	\brief
		Set the column to be used as the sort key.

	\param col_id
		ID code of the column to use as the key when sorting the list items.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if col_id is invalid for this list box.
	*/
	void	setSortColumnByID(uint col_id);

	
	/*!
	\brief
		Set whether the vertical scroll bar should always be shown, or just when needed.

	\param setting
		- true to have the vertical scroll bar shown at all times.
		- false to have the vertical scroll bar appear only when needed.

	\return
		Nothing.
	*/
	void	setShowVertScrollbar(bool setting);


	/*!
	\brief
		Set whether the horizontal scroll bar should always be shown, or just when needed.

	\param setting
		- true to have the horizontal scroll bar shown at all times.
		- false to have the horizontal scroll bar appear only when needed.

	\return
		Nothing.
	*/
	void	setShowHorzScrollbar(bool setting);

	
	/*!
	\brief
		Removed the selected state from any currently selected ListboxItem attached to the list.

	\return
		Nothing.
	*/
	void	clearAllSelections(void);


	/*!
	\brief
		Sets or clears the selected state of the given ListboxItem which must be attached to the list.

	\note
		Depending upon the current selection mode, this may cause other items to be selected, other
		items to be deselected, or for nothing to actually happen at all.

	\param item
		Pointer to the attached ListboxItem to be affected.

	\param state
		- true to put the ListboxItem into the selected state.
		- false to put the ListboxItem into the de-selected state.

	\return
		Nothing.
	
	\exception InvalidRequestException	thrown if \a item is not attached to the list box.
	*/
	void	setItemSelectState(ListboxItem* item, bool state);


	/*!
	\brief
		Sets or clears the selected state of the ListboxItem at the given grid reference.

	\note
		Depending upon the current selection mode, this may cause other items to be selected, other
		items to be deselected, or for nothing to actually happen at all.

	\param grid_ref
		MCLGridRef object describing the position of the item to be affected.

	\param state
		- true to put the ListboxItem into the selected state.
		- false to put the ListboxItem into the de-selected state.

	\return
		Nothing.
	
	\exception InvalidRequestException	thrown if \a grid_ref is invalid for this list box.
	*/
	void	setItemSelectState(const MCLGridRef& grid_ref, bool state);

	
	/*!
	\brief
		Inform the list box that one or more attached ListboxItems have been externally modified, and
		the list should re-sync its internal state and refresh the display as needed.

	\return
		Nothing.
	*/
	void	handleUpdatedItemData(void);


	/*!
	\brief
		Set the width of the specified column header (and therefore the column itself).

	\param col_idx
		Zero based column index of the column whos width is to be set.

	\param width
		float value specifying the new width for the column using the active metrics system.
		
	\param relative
		*Added by Talidan for MTA* specifies whether the measurement is relative or absolute

	\return
		Nothing.

	\exception InvalidRequestException	thrown if \a column is out of range.
	*/
	void	setColumnHeaderWidth(uint col_idx, float width, bool relative = true);


	/*!
	\brief
		Set whether user manipulation of the sort column and direction are enabled.

	\param setting
		- true if the user may interactively modify the sort column and direction.
		- false if the user may not modify the sort column and direction (these can still be set programmatically).

	\return
		Nothing.
	*/
	void	setUserSortControlEnabled(bool setting);


	/*!
	\brief
		Set whether the user may size column segments.

	\param setting
		- true if the user may interactively modify the width of columns.
		- false if the user may not change the width of the columns.

	\return
		Nothing.
	*/
	void	setUserColumnSizingEnabled(bool setting);

    /*!
    \Added to MTA by Talidan
        Set whether the user may size a specific column segment

    \param setting
        -col_idx: Zero based column index of the column whos width is to be set.
        -setting: true if the user may interactively modify the width of columns.  false if the user may not change the width of the columns.
    \return
        Nothing
    */
    void	setUserColumnSegmentSizingEnabled(uint col_idx, bool setting);

	/*!
	\brief
		Set whether the user may modify the order of the columns.

	\param setting
		- true if the user may interactively modify the order of the columns.
		- false if the user may not modify the order of the columns.
	*/
	void	setUserColumnDraggingEnabled(bool setting);


	/*!
	\brief
		Automatically determines the "best fit" size for the specified column and sets
		the column width to the same.

	\param col_idx
		Zero based index of the column to be sized.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if \a col_idx is out of range.
	*/
	void	autoSizeColumnHeader(uint col_idx);


	/*!
	\brief
		Set the ID code assigned to a given row.

	\param row_idx
		Zero based index of the row who's ID code is to be set.

	\param row_id
		ID code to be assigned to the row at the requested index.

	\return
		Nothing.

	\exception InvalidRequestException	thrown if \a row_idx is out of range
	*/
	void	setRowID(uint row_idx, uint row_id);


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for the Multi-column list base class
	*/
	MultiColumnList(const String& type, const String& name);


	/*!
	\brief
		Destructor for the multi-column list base class.
	*/
	virtual ~MultiColumnList(void);


protected:
	/*************************************************************************
		Implementation Functions (abstract interface)
	*************************************************************************/
	/*!
	\brief
		Return a Rect object describing, in un-clipped pixels, the window relative area
		that is to be used for rendering list items.

	\return
		Rect object describing the area of the Window to be used for rendering
		list box items.
	*/
	virtual	Rect	getListRenderArea(void) const		= 0;


	/*!
	\brief
		create and return a pointer to a ListHeaer widget for use as the column headers.

	\param name
	   String holding the name to be assigned to the created component.

	\return
		Pointer to a ListHeader based object.
	*/
	virtual ListHeader*	createListHeader(const String& name) const		= 0;
 

	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as vertical scroll bar

	\param name
	   String holding the name to be assigned to the created component.

	\return
		Pointer to a Scrollbar to be used for scrolling the list vertically.
	*/
	virtual Scrollbar*	createVertScrollbar(const String& name) const		= 0;
 

	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as horizontal scroll bar

	\param name
	   String holding the name to be assigned to the created component.

	\return
		Pointer to a Scrollbar to be used for scrolling the list horizontally.
	*/
	virtual Scrollbar*	createHorzScrollbar(const String& name) const		= 0;


	/*!
	\brief
		Perform rendering of the widget control frame and other 'static' areas.  This
		method should not render the actual items.  Note that the items are typically
		rendered to layer 3, other layers can be used for rendering imagery behind and
		infront of the items.

	\return
		Nothing.
	*/
	virtual	void	cacheListboxBaseImagery()		= 0;


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add multi column list box specific events
	*/
	void	addMultiColumnListboxEvents(void);

    // overridden from base class.
	virtual	void populateRenderCache();


	/*!
	\brief
		display required integrated scroll bars according to current state of the list box and update their values.
	*/
	void	configureScrollbars(void);


	/*!
	\brief
		select all strings between positions \a start and \a end.  (inclusive).  Returns true if something was modified.
	*/
	bool	selectRange(const MCLGridRef& start, const MCLGridRef& end);


	/*!
	\brief
		Return the sum of all row heights
	*/
	float	getTotalRowsHeight(void) const;


	/*!
	\brief
		Return the width of the widest item in the given column
	*/
	float	getWidestColumnItemWidth(uint col_idx) const;


	/*!
	\brief
		Return the height of the highest item in the given row.
	*/
	float	getHighestRowItemHeight(uint row_idx) const;


	/*!
	\brief
		Clear the selected state for all items (implementation)

	\return
		true if some selections were cleared, false nothing was changed.
	*/
	bool	clearAllSelections_impl(void);


	/*!
	\brief
		Return the ListboxItem under the given window local pixel co-ordinate.

	\return
		ListboxItem that is under window pixel co-ordinate \a pt, or NULL if no
		item is under that position.
	*/
	ListboxItem*	getItemAtPoint(const Point& pt) const;


	/*!
	\brief
		Set select state for the given item.  This appropriately selects other
		items depending upon the select mode.  Returns true if something is
		changed, else false.
	*/
	bool	setItemSelectState_impl(const MCLGridRef grid_ref, bool state);


	/*!
	\brief
		Set select state for all items in the given row
	*/
	void	setSelectForItemsInRow(uint row_idx, bool state);


	/*!
	\brief
		Set select state for all items in the given column
	*/
	void	setSelectForItemsInColumn(uint col_idx, bool state);


	/*!
	\brief
		Move the column at index \a col_idx so it is at index \a position.  Implementation version which does not move the
		header segment (since that may have already happened).

	\exception InvalidRequestException	thrown if \a col_idx is invalid.
	*/
	void	moveColumn_impl(uint col_idx, uint position);


	/*!
	\brief
		Remove all items from the list.

	\note
		Note that this will cause 'AutoDelete' items to be deleted.

	\return
		- true if the list contents were changed.
		- false if the list contents were not changed (list already empty).
	*/
	bool	resetList_impl(void);


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
		if (class_name==(const utf8*)"MultiColumnList")	return true;
		return Window::testClassName_impl(class_name);
	}

    // overrides function in base class.
    int writePropertiesXML(OutStream& out_stream) const;

	/*************************************************************************
		New event handlers for multi column list
	*************************************************************************/
	/*!
	\brief
		Handler called when the selection mode of the list box changes
	*/
	virtual	void	onSelectionModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the nominated selection column changes
	*/
	virtual	void	onNominatedSelectColumnChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the nominated selection row changes.
	*/
	virtual	void	onNominatedSelectRowChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the vertical scroll bar 'force' mode is changed.
	*/
	virtual	void	onVertScrollbarModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the horizontal scroll bar 'force' mode is changed.
	*/
	virtual	void	onHorzScrollbarModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the current selection changes.
	*/
	virtual	void	onSelectionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the list contents is changed.
	*/
	virtual	void	onListContentsChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the sort column changes.
	*/
	virtual	void	onSortColumnChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the sort direction changes.
	*/
	virtual	void	onSortDirectionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when a column is sized.
	*/
	virtual	void	onListColumnSized(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the column order is changed.
	*/
	virtual	void	onListColumnMoved(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event handlers
	*************************************************************************/
	virtual void	onSized(WindowEventArgs& e);
	virtual void	onMouseButtonDown(MouseEventArgs& e);
	virtual	void	onMouseWheel(MouseEventArgs& e);


	/*************************************************************************
		Handlers for subscribed events
	*************************************************************************/
	bool	handleHeaderScroll(const EventArgs& e);
	bool	handleHeaderSegMove(const EventArgs& e);
	bool	handleColumnSizeChange(const EventArgs& e);
	bool	handleHorzScrollbar(const EventArgs& e);
	bool	handleVertScrollbar(const EventArgs& e);
	bool	handleSortColumnChange(const EventArgs& e);
	bool	handleSortDirectionChange(const EventArgs& e);
	bool	handleHeaderSegDblClick(const EventArgs& e);

    /*!
    \brief
        Struct used internally to represent a row in the list and also to ease
        sorting of the rows.
    */
	struct ListRow
	{
		typedef	std::vector<ListboxItem*>	RowItems;
		RowItems	d_items;
		uint		d_sortColumn;
		uint		d_rowID;

		// operators
		ListboxItem* const& operator[](uint idx) const	{return d_items[idx];}
		ListboxItem*&	operator[](uint idx) {return d_items[idx];}
		bool	operator<(const ListRow& rhs) const;
		bool	operator>(const ListRow& rhs) const;
	};


	/*!
	\brief
		std algorithm predicate used for sorting in descending order
	*/
	static bool pred_descend(const ListRow& a, const ListRow& b);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	// component widgets and settings.
	Scrollbar*	d_vertScrollbar;	//!< vertical scroll-bar widget
	Scrollbar*	d_horzScrollbar;	//!< horizontal scroll-bar widget
	ListHeader*	d_header;			//!< The ListHeader attached to this multi-column list.
	bool	d_forceVertScroll;		//!< true if vertical scrollbar should always be displayed
	bool	d_forceHorzScroll;		//!< true if horizontal scrollbar should always be displayed

	// selection abilities.
	SelectionMode	d_selectMode;	//!< Holds selection mode (represented by settings below).
	uint	d_nominatedSelectCol;	//!< Nominated column for single column selection.
	uint	d_nominatedSelectRow;	//!< Nominated row for single row selection.
	bool	d_multiSelect;			//!< Allow multiple selections.
	bool	d_fullRowSelect;		//!< All items in a row are selected.
	bool	d_fullColSelect;		//!< All items in a column are selected.
	bool	d_useNominatedRow;		//!< true if we use a nominated row to select.
	bool	d_useNominatedCol;		//!< true if we use a nominated col to select.
	ListboxItem*	d_lastSelected;	//!< holds pointer to the last selected item (used in range selections)

	// storage of items in the list box.
	typedef std::vector<ListRow>		ListItemGrid;
	ListItemGrid	d_grid;			//!< Holds the list box data.


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static MultiColumnListProperties::ColumnsMovable				d_columnsMovableProperty;
	static MultiColumnListProperties::ColumnsSizable				d_columnsSizableProperty;
	static MultiColumnListProperties::ForceHorzScrollbar			d_forceHorzScrollProperty;
	static MultiColumnListProperties::ForceVertScrollbar			d_forceVertScrollProperty;
	static MultiColumnListProperties::NominatedSelectionColumnID	d_nominatedSelectColProperty;
	static MultiColumnListProperties::NominatedSelectionRow			d_nominatedSelectRowProperty;
	static MultiColumnListProperties::SelectionMode					d_selectModeProperty;
	static MultiColumnListProperties::SortColumnID					d_sortColumnIDProperty;
	static MultiColumnListProperties::SortDirection					d_sortDirectionProperty;
	static MultiColumnListProperties::SortSettingEnabled			d_sortSettingProperty;
	static MultiColumnListProperties::ColumnHeader					d_columnHeaderProperty;
	static MultiColumnListProperties::RowCount						d_rowCountProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addMultiColumnListProperties(void);
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIMultiColumnList_h_
