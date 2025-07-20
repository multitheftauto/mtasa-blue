/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIGridList.h
 *  PURPOSE:     Grid list widget interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUIStaticImage.h"
#include "CGUIListItem.h"
#include "CGUICallback.h"

namespace SelectionModes
{
    enum SelectionMode
    {
        RowSingle,
        RowMultiple,
        CellSingle,
        CellMultiple,
        NominatedColumnSingle,
        NominatedColumnMultiple,
        ColumnSingle,
        ColumnMultiple,
        NominatedRowSingle,
        NominatedRowMultiple
    };
}

using SelectionModes::SelectionMode;

namespace SortDirections
{
    enum SortDirection
    {
        None,
        Ascending,
        Descending
    };
}

using SortDirections::SortDirection;

class CGUIGridList : public CGUIElement
{
public:
    virtual ~CGUIGridList(){};

    virtual unsigned int AddColumn(const char* szTitle, float fWidth) = 0;
    virtual void         RemoveColumn(unsigned int uiColumn) = 0;
    virtual void         AutoSizeColumn(unsigned int hColumn) = 0;
    virtual void         SetColumnWidth(int hColumn, float fWidth, bool bRelative = true) = 0;
    virtual bool         GetColumnWidth(int hColumn, float& fOutWidth, bool bRelative = true) = 0;
    virtual void         SetColumnTitle(int hColumn, const char* szTitle) = 0;
    virtual const char*  GetColumnTitle(int hColumn) = 0;

    virtual void          SetSelectionMode(SelectionMode mode) = 0;
    virtual SelectionMode GetSelectionMode() = 0;

    virtual void          ForceUpdate() = 0;
    virtual int           AddRow(bool bFast = false, std::vector<std::pair<SString, bool> >* m_items = NULL) = 0;
    virtual void          RemoveRow(int iRow) = 0;
    virtual int           InsertRowAfter(int iRow, std::vector<std::pair<SString, bool> >* m_items = NULL) = 0;
    virtual void          Clear() = 0;
    virtual CGUIListItem* GetItem(int iRow, int hColumn) = 0;
    virtual const char*   GetItemText(int iRow, int hColumn) = 0;
    virtual int           SetItemText(int iRow, int hColumn, const char* szText, bool bNumber = false, bool bSection = false, bool bFast = false,
                                      const char* szSortText = NULL) = 0;
    virtual void          SetItemData(int iRow, int hColumn, void* pData, CGUICallback<void, void*> deleteDataCallback = NULL) = 0;
    virtual void          SetItemData(int iRow, int hColumn, const char* pszData) = 0;
    virtual void*         GetItemData(int iRow, int hColumn) = 0;
    virtual void          SetItemColor(int iRow, int hColumn, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha) = 0;
    virtual bool          GetItemColor(int iRow, int hColumn, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha) = 0;

    virtual void SetHorizontalScrollBar(bool bEnabled) = 0;
    virtual void SetVerticalScrollBar(bool bEnabled) = 0;
    virtual void SetSortingEnabled(bool bEnabled) = 0;
    virtual bool IsSortingEnabled() = 0;
    virtual void SetItemImage(int iRow, int hColumn, CGUIStaticImage* pImage) = 0;

    virtual float GetHorizontalScrollPosition() = 0;
    virtual float GetVerticalScrollPosition() = 0;
    virtual void  SetHorizontalScrollPosition(float fPosition) = 0;
    virtual void  SetVerticalScrollPosition(float fPosition) = 0;

    virtual int           GetColumnIndex(int hColumn) = 0;
    virtual int           GetItemColumnIndex(CGUIListItem* pItem) = 0;
    virtual int           GetItemRowIndex(CGUIListItem* pItem) = 0;
    virtual void          GetVisibleRowRange(int& iFirst, int& iLast) = 0;
    virtual int           GetSelectedCount() = 0;
    virtual CGUIListItem* GetSelectedItem() = 0;
    virtual CGUIListItem* GetNextSelectedItem(CGUIListItem* pItem) = 0;
    virtual int           GetSelectedItemRow() = 0;
    virtual int           GetSelectedItemColumn() = 0;
    virtual int           GetRowCount() = 0;
    virtual int           GetColumnCount() = 0;

    virtual void Sort(unsigned int uiColumn, SortDirection direction) = 0;
    virtual void GetSort(unsigned int& uiColumn, SortDirection& direction) = 0;

    virtual bool IsColumnSegmentSizingEnabled(int hColumn) = 0;
    virtual void SetColumnSegmentSizingEnabled(int hColumn, bool bEnabled) = 0;

    virtual void SetSelectedItem(int iRow, int hColumn, bool bReset) = 0;

    virtual void SetSortColumnHandler(GUI_CALLBACK Callback) = 0;

    virtual void SetIgnoreTextSpacer(bool bIgnoreTextSpacer) = 0;
};
