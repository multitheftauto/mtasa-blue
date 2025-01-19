/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto: San Andreas
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIGridLayout.h>
#include "CGUIElement_Impl.h"

class CGUIGridLayout_Impl : public CGUIGridLayout, public CGUIElement_Impl
{
public:
    CGUIGridLayout_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL);
    ~CGUIGridLayout_Impl();

    eCGUIType GetType() { return CGUI_GRIDLAYOUT; };

    const bool SetColumns(int columns);
    const bool SetRows(int rows);
    const bool SetGrid(int columns, int rows);

    const int GetColumns() const { return m_columns; }
    const int GetRows() const { return m_rows; }

    const bool SetActiveCell(int column, int row);
    const bool SetActiveColumn(int column);
    const bool SetActiveRow(int row);

    const int                 GetActiveColumn() const { return m_activeColumn; }
    const int                 GetActiveRow() const { return m_activeRow; }
    const std::pair<int, int> GetActiveCell() const;

    const bool AddItem(CGUIElement* item, int column, int row, const bool moveToNextCell = true);
    const bool AddItem(CGUIElement* item, const bool moveToNextCell = true);

    const bool RemoveItem(const int column, const int row, const bool moveToPreviousCell = false, const bool deleteItem = false);
    const bool RemoveItem(const CGUIElement* item, const bool moveToPreviousCell = false, const bool deleteItem = false);

    SGridCellItem* GetCell(const int column, const int row) const;
    SGridCellItem* GetCell(const CGUIElement* item) const;

    std::vector<SGridCellItem*> GetCellsInGrid(const int startColumn, const int startRow, const int endColumn, const int endRow) const;
    std::vector<SGridCellItem*> GetCellsOutsideGrid(const int startColumn, const int startRow, const int endColumn, const int endRow) const;
    std::vector<SGridCellItem*> GetCellsInColumn(const int column) const;
    std::vector<SGridCellItem*> GetCellsInRow(const int row) const;

    void SetItemAlignment(const CGUIElement* item, eGridLayoutItemAlignment alignment);
    void SetDefaultItemAlignment(eGridLayoutItemAlignment alignment) { m_defaultAlignment = alignment; }

    const eGridLayoutItemAlignment GetItemAlignment(const CGUIElement* item) const;
    const eGridLayoutItemAlignment GetDefaultItemAlignment() const { return m_defaultAlignment; }

    const bool SetCellAlpha(const int column, const int row, const float alpha);
    const bool SetDefaultCellAlpha(const float alpha);

    const bool SetColumnWidth(const int column, const float width);
    const bool SetRowHeight(const int row, const float height);

#include "CGUIElement_Inc.h"

private:
    int m_columns = 0;
    int m_rows = 0;

    int m_activeColumn = 0;
    int m_activeRow = 0;

    int m_nextId = 1;

    float m_defaultCellAlpha = 1.0f;

    std::vector<std::vector<int>>               m_grid;
    std::unordered_map<int, SGridCellItem*>     m_cells;
    std::unordered_map<const CGUIElement*, int> m_items;

    std::vector<float> m_columnWidths;
    std::vector<float> m_rowHeights;

    CGUITexture* m_cellTexture = nullptr;
    CGUITexture* m_cellTextureAlt = nullptr;

    eGridLayoutItemAlignment m_defaultAlignment = eGridLayoutItemAlignment::MIDDLE_CENTER;

    void CreateGridCells();
    void CleanupGridCells();

    void RepositionGridCells(const bool itemOnly = false) const;
    void RepositionGridCell(const SGridCellItem& cell, const bool itemOnly = false) const;
    void RepositionGridCell(const int column, const int row, const bool itemOnly = false) const;

    const bool InGridRange(const int column, const int row) const;
    const bool InColumnRange(const int column) const;
    const bool InRowRange(const int row) const;

    const CVector2D CalculateCellPosition(const SGridCellItem& cell, const std::pair<float, float>& offsets) const;
    const CVector2D CalculateCellSize(const SGridCellItem& cell, const std::pair<float, float>& offset) const;

    const std::pair<std::vector<float>, std::vector<float>> CalculateGridOffsets(const int column = 0, const int row = 0) const;
    const std::pair<float, float>                           AccumulateOffsets(const std::pair<std::vector<float>, std::vector<float>>& offsets) const;
    const float                                             AccumulateOffset(const std::vector<float>& offset) const;

    const int CountColumnWidths(const int maxColumns = 0) const;
    const int CountRowHeights(const int maxRows = 0) const;

    const CVector2D GetAlignmentOffset(const SGridCellItem& cell, const CVector2D& size) const;
};
