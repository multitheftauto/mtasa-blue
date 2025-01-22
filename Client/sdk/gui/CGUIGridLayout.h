/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIGridList.h
 *  PURPOSE:     Grid list widget interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUIGridLayout.h"
#include <../Shared/sdk/CVector2D.h>
#include <../Shared/sdk/CColor.h>

#include <vector>

class CGUIStaticImage;
class CGUITexture;

enum class eGridLayoutItemAlignment
{
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    MIDDLE_LEFT,
    MIDDLE_CENTER,
    MIDDLE_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
};

struct SGridCellItem
{
    int                      id;
    CGUIStaticImage*         container;
    CGUIElement*             element;
    eGridLayoutItemAlignment alignment;
    int                      column;
    int                      row;
    bool                     forceFullSize;
    CVector2D                padding;
};

class CGUIGridLayout : public CGUIElement
{
public:
    virtual ~CGUIGridLayout(){};

    virtual const bool SetColumns(int columns) = 0;
    virtual const bool SetRows(int rows) = 0;
    virtual const bool SetGrid(int columns, int rows) = 0;

    virtual const int GetColumns() const = 0;
    virtual const int GetRows() const = 0;

    virtual const bool SetActiveCell(int column, int row) = 0;
    virtual const bool SetActiveColumn(int column) = 0;
    virtual const bool SetActiveRow(int row) = 0;

    virtual const int                 GetActiveColumn() const = 0;
    virtual const int                 GetActiveRow() const = 0;
    virtual const std::pair<int, int> GetActiveCell() const = 0;

    virtual const bool AddItem(CGUIElement* item, int column, int row, const bool moveToNextCell = true) = 0;
    virtual const bool AddItem(CGUIElement* item, const bool moveToNextCell = true) = 0;

    virtual const bool RemoveItem(const int column, const int row, const bool moveToPreviousCell = false, const bool deleteItem = false) = 0;
    virtual const bool RemoveItem(const CGUIElement* item, const bool moveToPreviousCell = false, const bool deleteItem = false) = 0;

    virtual SGridCellItem* GetCell(const int column, const int row) const = 0;
    virtual SGridCellItem* GetCell(const CGUIElement* item) const = 0;

    virtual std::vector<SGridCellItem*> GetCellsInGrid(const int startColumn, const int startRow, const int endColumn, const int endRow) const = 0;
    virtual std::vector<SGridCellItem*> GetCellsOutsideGrid(const int startColumn, const int startRow, const int endColumn, const int endRow) const = 0;
    virtual std::vector<SGridCellItem*> GetCellsInColumn(const int column) const = 0;
    virtual std::vector<SGridCellItem*> GetCellsInRow(const int row) const = 0;

    virtual void SetItemAlignment(const CGUIElement* item, eGridLayoutItemAlignment alignment) = 0;
    virtual void SetDefaultItemAlignment(eGridLayoutItemAlignment alignment) = 0;

    virtual const eGridLayoutItemAlignment GetItemAlignment(const CGUIElement* item) const = 0;
    virtual const eGridLayoutItemAlignment GetDefaultItemAlignment() const = 0;

    virtual const bool SetCellAlpha(const int column, const int row, const float alpha) = 0;
    virtual const bool SetDefaultCellAlpha(const float alpha) = 0;

    virtual const bool SetColumnWidth(const int column, const float width) = 0;
    virtual const bool SetRowHeight(const int row, const float height) = 0;

    virtual const bool SetCellFullSize(const int column, const int row, const bool fullSize) = 0;
    virtual const bool SetDefaultCellFullSize(const bool fullSize, const bool updateExisting = false) = 0;

    virtual const bool GetCellFullSize(const int column, const int row) const = 0;
    virtual const bool GetDefaultCellFullSize() const = 0;

    virtual const bool SetCellPadding(const int column, const int row, const CVector2D& padding) = 0;
    virtual const bool SetDefaultCellPadding(const CVector2D& padding, const bool updateExisting) = 0;

    virtual const CVector2D& GetCellPadding(const int column, const int row) const = 0;
    virtual const CVector2D& GetDefaultCellPadding() const = 0;

    virtual const bool SetCellTexture(const int column, const int row, CGUITexture* texture, const bool alt = false) = 0;
    virtual const bool SetCellColor(const int column, const int row, const CColor& color, const bool alt = false) = 0;

    virtual const bool SetDefaultCellTexture(CGUITexture* texture, const bool alt = false, const bool updateExisting = false) = 0;
    virtual const bool SetDefaultCellColor(const CColor& color, const bool alt = false, const bool updateExisting = false) = 0;
};
