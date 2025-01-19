/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIGridList_Impl.cpp
 *  PURPOSE:     Grid list widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <numeric>

#define CGUIGRIDLAYOUT_NAME "CGUI/FrameWindow"

CGUIGridLayout_Impl::CGUIGridLayout_Impl(CGUI_Impl* pGUI, CGUIElement* pParent)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(pGUI->ResolveModernName(CGUIGRIDLAYOUT_NAME), szUnique);

    m_pWindow->setDestroyedByParent(false);
    m_pWindow->setRect(CEGUI::Relative, CEGUI::Rect(0.00f, 0.00f, 1.0f, 1.0f));

    CEGUI::FrameWindow* frameWindow = reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow);
    frameWindow->setTitleBarEnabled(false);
    frameWindow->setSizingEnabled(false);
    frameWindow->setDragMovingEnabled(false);
    frameWindow->setCloseButtonEnabled(false);
    frameWindow->setFrameEnabled(false);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    // Register our events
    // m_pWindow->subscribeEvent(CEGUI::MultiColumnList::EventSortColumnChanged, CEGUI::Event::Subscriber(&CGUIGridList_Impl::Event_OnSortColumn, this));
    AddEvents();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
    {
        SetParent(pParent);
    }
    else
    {
        pGUI->AddChild(this);
        SetParent(NULL);
    }

    // Create our cell textures
    m_cellTexture = m_pManager->CreateTexture();
    m_cellTextureAlt = m_pManager->CreateTexture();

    unsigned long ulBackgroundColor255 = COLOR_ARGB(255, 0, 0, 0);
    m_cellTexture->LoadFromMemory(&ulBackgroundColor255, 1, 1);

    unsigned long ulBackgroundColor150 = COLOR_ARGB(150, 0, 0, 0);
    m_cellTextureAlt->LoadFromMemory(&ulBackgroundColor150, 1, 1);
}

CGUIGridLayout_Impl::~CGUIGridLayout_Impl()
{
    DestroyElement();
}

const bool CGUIGridLayout_Impl::AddItem(CGUIElement* item, int column, int row, const bool moveToNextCell)
{
    auto* cell = GetCell(column, row);

    if (cell == nullptr)
        return false;

    if (cell->element != nullptr)
        RemoveItem(column, row, false, true);

    item->SetParent(cell->container);
    item->SetPosition(CVector2D(0.0f, 0.0f), true);
    item->ForceRedraw();

    cell->element = item;
    m_items.emplace(item, cell->id);

    if (moveToNextCell)
    {
        if (m_activeRow == m_rows)
        {
            if (m_activeColumn < m_columns)
            {
                m_activeColumn++;
                m_activeRow = 1;
            }
        }
        else
        {
            m_activeRow++;
        }
    }

    return true;
}

const bool CGUIGridLayout_Impl::AddItem(CGUIElement* item, const bool moveToNextCell)
{
    return AddItem(item, m_activeColumn, m_activeRow);
}

const bool CGUIGridLayout_Impl::RemoveItem(const int column, const int row, const bool moveToPreviousCell, const bool deleteItem)
{
    auto* cell = GetCell(column, row);

    if (cell == nullptr || cell->element == nullptr)
        return false;

    cell->element->SetParent(nullptr);
    cell->element->ForceRedraw();

    m_items.erase(cell->element);
    cell->element = nullptr;

    if (deleteItem)
        delete cell->element;

    if (moveToPreviousCell)
    {
        if (m_activeRow == 1)
        {
            if (m_activeColumn > 1)
            {
                m_activeColumn--;
                m_activeRow = m_rows;
            }
        }
        else
        {
            m_activeRow--;
        }
    }

    return true;
}

const bool CGUIGridLayout_Impl::RemoveItem(const CGUIElement* item, const bool moveToPreviousCell, const bool deleteItem)
{
    auto* cell = GetCell(item);

    if (cell == nullptr)
        return false;

    return RemoveItem(cell->column, cell->row, moveToPreviousCell);
}

SGridCellItem* CGUIGridLayout_Impl::GetCell(const int column, const int row) const
{
    if (!InGridRange(column, row))
        return nullptr;

    const auto& cell = m_cells.find(m_grid[column - 1][row - 1]);

    if (cell == m_cells.end())
        return nullptr;

    return cell->second;
}

SGridCellItem* CGUIGridLayout_Impl::GetCell(const CGUIElement* item) const
{
    int id = m_items.count(item) ? m_items.at(item) : 0;
    return id == 0 ? nullptr : m_cells.at(id);
}

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsInColumn(const int column) const
{
    return GetCellsInGrid(column, 0, column, m_rows - 1);
}

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsInRow(const int row) const
{
    return GetCellsInGrid(0, row, m_columns - 1, row);
}

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsInGrid(const int startColumn, const int startRow, const int endColumn, const int endRow) const
{
    if (startColumn < 1 || startRow < 1 || endColumn > m_columns || endRow > m_rows)
    {
        return std::vector<SGridCellItem*>();
    }

    std::vector<SGridCellItem*> cells;

    for (int i = startColumn; i <= endColumn; i++)
    {
        for (int j = startRow; j <= endRow; j++)
        {
            if (m_grid[i - 1][j - 1] != 0)
            {
                auto& cell = m_cells.find(m_grid[i - 1][j - 1]);

                if (cell != m_cells.end())
                {
                    cells.push_back(cell->second);
                }
            }
        }
    }

    return cells;
}

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsOutsideGrid(const int startColumn, const int startRow, const int endColumn, const int endRow) const
{
    if (startColumn < 1 || startRow < 1 || endColumn > m_columns || endRow > m_rows)
    {
        return std::vector<SGridCellItem*>();
    }

    std::vector<SGridCellItem*> cells;

    for (auto& cell : m_cells)
    {
        if (cell.second->column < startColumn || cell.second->column > endColumn || cell.second->row < startRow || cell.second->row > endRow)
        {
            cells.push_back(cell.second);
        }
    }

    return cells;
}

const bool CGUIGridLayout_Impl::SetColumns(int columns)
{
    if (columns < 0 || columns == m_columns)
    {
        return false;
    }

    bool set = SetGrid(columns, m_rows);

    if (set)
        m_columns = columns;

    return set;
}

const bool CGUIGridLayout_Impl::SetRows(int rows)
{
    if (rows < 0 || rows == m_rows)
        return false;

    bool set = SetGrid(m_columns, rows);

    if (set)
        m_rows = rows;

    return set;
}

const bool CGUIGridLayout_Impl::SetGrid(int columns, int rows)
{
    if (columns < 0 || rows < 0 || (columns == m_columns && rows == m_rows))
        return false;

    m_columns = columns;
    m_rows = rows;

    const size_t& size = m_grid.size();
    const bool    larger = (columns * rows) > (size * (size > 0 ? m_grid[0].size() : 0));

    m_grid.resize(columns);

    for (int i = 0; i < columns; i++)
    {
        m_grid[i].resize(rows, 0);
    }

    m_columnWidths.resize(m_columns, 0.0f);
    m_rowHeights.resize(m_rows, 0.0f);

    if (larger)
    {
        CreateGridCells();

        // Since the grid starts at 0,0 make sure an active cell is set
        m_activeColumn = (m_activeColumn == 0 ? 1 : m_activeColumn);
        m_activeRow = (m_activeRow == 0 ? 1 : m_activeRow);
    }
    else
    {
        CleanupGridCells();

        // Since the grid has been resized, we need to make sure the active cell is still within the grid
        m_activeColumn = std::min(m_activeColumn, columns);
        m_activeRow = std::min(m_activeRow, rows);
    }

    RepositionGridCells();
    return true;
}

const bool CGUIGridLayout_Impl::SetActiveCell(int column, int row)
{
    if (!InGridRange(column, row))
        return false;

    m_activeColumn = column;
    m_activeRow = row;
    return true;
}

const bool CGUIGridLayout_Impl::SetActiveColumn(int column)
{
    if (!InColumnRange(column))
        return false;

    m_activeColumn = column;
    return true;
}

const bool CGUIGridLayout_Impl::SetActiveRow(int row)
{
    if (!InRowRange(row))
        return false;

    m_activeRow = row;
    return true;
}

const std::pair<int, int> CGUIGridLayout_Impl::GetActiveCell() const
{
    return std::make_pair(m_activeColumn, m_activeRow);
}

void CGUIGridLayout_Impl::SetItemAlignment(const CGUIElement* item, eGridLayoutItemAlignment alignment)
{
    auto* cell = GetCell(item);

    if (cell == nullptr)
        return;

    cell->alignment = alignment;
    RepositionGridCell(*cell, true);
}

const eGridLayoutItemAlignment CGUIGridLayout_Impl::GetItemAlignment(const CGUIElement* item) const
{
    const auto* cell = GetCell(item);

    if (cell == nullptr)
        return m_defaultAlignment;

    return cell->alignment;
}

const bool CGUIGridLayout_Impl::SetCellAlpha(const int column, const int row, const float alpha)
{
    auto* cell = GetCell(column, row);

    if (cell == nullptr)
        return false;

    cell->container->SetAlpha(alpha);
    return true;
}

const bool CGUIGridLayout_Impl::SetDefaultCellAlpha(const float alpha)
{
    m_defaultCellAlpha = alpha;

    for (auto& cell : m_cells)
    {
        cell.second->container->SetAlpha(alpha);
    }

    return true;
}

const bool CGUIGridLayout_Impl::SetColumnWidth(const int column, const float width)
{
    if (!InColumnRange(column))
        return false;

    m_columnWidths[column - 1] = width;
    RepositionGridCells();
    return true;
}

const bool CGUIGridLayout_Impl::SetRowHeight(const int row, const float height)
{
    if (!InRowRange(row))
        return false;

    m_rowHeights[row - 1] = height;
    RepositionGridCells();
    return true;
}

void CGUIGridLayout_Impl::CreateGridCells()
{
    auto* parent = reinterpret_cast<CGUIElement*>(this);

    // Calculate our own offsets here for performance reasons
    std::vector<float> columnOffset(m_columns, 0.0f);
    std::vector<float> rowOffset(m_rows, 0.0f);

    for (int i = 0; i < m_columns; i++)
    {
        for (int j = 0; j < m_rows; j++)
        {
            const auto cell = m_cells.count(m_grid[i][j]);

            if (cell == 0)
            {
                auto* cellItem = new SGridCellItem();
                cellItem->container = m_pManager->CreateStaticImage(parent);
                cellItem->alignment = m_defaultAlignment;

                // Assign the cell container texture, alternative between m_cellTexture and m_cellTextureAlt
                cellItem->container->LoadFromTexture((i + j) % 2 == 0 ? m_cellTexture : m_cellTextureAlt);
                cellItem->container->SetAlpha(m_defaultCellAlpha);

                auto offsets = AccumulateOffsets({columnOffset, rowOffset});

                // Position the cell container in the grid
                cellItem->container->SetPosition(CalculateCellPosition(*cellItem, offsets), true);

                // Size the cell container in the grid
                cellItem->container->SetSize(CalculateCellSize(*cellItem, offsets), true);

                cellItem->id = m_nextId;

                auto* label = m_pManager->CreateLabel(cellItem->container, std::to_string(m_nextId).c_str());
                label->AutoSize();

                CVector2D offset = GetAlignmentOffset(*cellItem, label->GetSize(true));
                label->SetPosition(offset, true);
                label->ForceRedraw();

                cellItem->element = label;
                cellItem->column = i + 1;
                cellItem->row = j + 1;

                m_cells.emplace(m_nextId, cellItem);
                m_grid[i][j] = m_nextId;

                m_nextId++;
            }

            rowOffset[j] = m_rowHeights[j];
        }

        columnOffset[i] = m_columnWidths[i];
        rowOffset.assign(m_rows, 0.0f);
    }
}

void CGUIGridLayout_Impl::CleanupGridCells()
{
    for (auto& cell : m_cells)
    {
        if (cell.second->column > m_columns || cell.second->row > m_rows)
        {
            delete cell.second->container;

            if (cell.second->element)
            {
                m_items.erase(cell.second->element);

                cell.second->element->SetParent(nullptr);
                cell.second->element->ForceRedraw();

                delete cell.second->element;
                cell.second->element = nullptr;
            }

            delete cell.second;
            m_cells.erase(cell.first);
        }
    }
}

const CVector2D CGUIGridLayout_Impl::CalculateCellPosition(const SGridCellItem& cell, const std::pair<float, float>& offsets) const
{
    int widths = CountColumnWidths();
    int heights = CountRowHeights();

    if (cell.element != nullptr)
        cell.element->SetText(std::to_string(std::max(0, cell.column - widths)).c_str());

    return CVector2D(offsets.first + (std::max(0, cell.column - CountColumnWidths(cell.column) - 1) *
                                      ((1.0f - AccumulateOffset(m_columnWidths)) / (m_columns - widths))),
                     offsets.second + (std::max(0, cell.row - CountRowHeights(cell.row) - 1) *
                                       ((1.0f - AccumulateOffset(m_rowHeights)) / (m_rows - heights))));
}

const CVector2D CGUIGridLayout_Impl::CalculateCellSize(const SGridCellItem& cell, const std::pair<float, float>& offsets) const
{
    int widths = CountColumnWidths();
    int heights = CountRowHeights();

    return CVector2D(
        m_columnWidths[cell.column - 1] == 0.0f ? (1.0f - AccumulateOffset(m_columnWidths)) / (m_columns - widths) : m_columnWidths[cell.column - 1],
        m_rowHeights[cell.row - 1] == 0.0f ? (1.0f - AccumulateOffset(m_rowHeights)) / (m_rows - heights) : m_rowHeights[cell.row - 1]);
}

const std::pair<std::vector<float>, std::vector<float>> CGUIGridLayout_Impl::CalculateGridOffsets(const int column, const int row) const
{
    std::vector<float> columnOffset(m_columns, 0.0f);
    std::vector<float> rowOffset(m_rows, 0.0f);

    for (int i = 0; i < (column == 0 ? m_columns : column) - 1; i++)
    {
        for (int j = 0; j < (row == 0 ? m_rows : row) - 1; j++)
        {
            if (j < (row == 0 ? m_rows : row))
            {
                rowOffset[j + 1] = m_rowHeights[j];
            }
        }

        columnOffset[i + 1] = m_columnWidths[i];
        rowOffset.assign(m_rows, 0.0f);
    }

    return {columnOffset, rowOffset};
}

const float CGUIGridLayout_Impl::AccumulateOffset(const std::vector<float>& offset) const
{
    return std::reduce(offset.begin(), offset.end());
}

const std::pair<float, float> CGUIGridLayout_Impl::AccumulateOffsets(const std::pair<std::vector<float>, std::vector<float>>& offsets) const
{
    return {AccumulateOffset(offsets.first), AccumulateOffset(offsets.second)};
}

void CGUIGridLayout_Impl::RepositionGridCells(const bool itemOnly) const
{
    std::vector<float> columnOffset(m_columns, 0.0f);
    std::vector<float> rowOffset(m_rows, 0.0f);

    for (int i = 0; i < m_columns; i++)
    {
        for (int j = 0; j < m_rows; j++)
        {
            const auto cell = m_cells.count(m_grid[i][j]);

            if (cell != 0)
            {
                const auto& cellItem = m_cells.at(m_grid[i][j]);

                if (!itemOnly)
                {
                    auto offsets = AccumulateOffsets({columnOffset, rowOffset});

                    cellItem->container->SetPosition(CalculateCellPosition(*cellItem, offsets), true);
                    cellItem->container->SetSize(CalculateCellSize(*cellItem, offsets), true);
                    cellItem->container->ForceRedraw();
                }

                if (cellItem->element != nullptr)
                {
                    CVector2D position = GetAlignmentOffset(*cellItem, cellItem->element->GetSize(true));
                    cellItem->element->SetPosition(position, true);
                    cellItem->element->ForceRedraw();
                }
            }

            rowOffset[j] = m_rowHeights[j];
        }

        columnOffset[i] = m_columnWidths[i];
        rowOffset.assign(m_rows, 0.0f);
    }
}

void CGUIGridLayout_Impl::RepositionGridCell(const SGridCellItem& cell, const bool itemOnly) const
{
    if (!itemOnly)
    {
        auto offsets = AccumulateOffsets(CalculateGridOffsets(cell.column, cell.row));

        cell.container->SetPosition(CalculateCellPosition(cell, offsets), true);
        cell.container->SetSize(CalculateCellSize(cell, offsets), true);
        cell.container->ForceRedraw();
    }

    if (cell.element != nullptr)
    {
        CVector2D position = GetAlignmentOffset(cell, cell.element->GetSize(true));
        cell.element->SetPosition(position, true);
        cell.element->ForceRedraw();
    }
}

void CGUIGridLayout_Impl::RepositionGridCell(const int column, const int row, const bool itemOnly) const
{
    const auto* cell = GetCell(column, row);

    if (cell == nullptr)
        return;

    RepositionGridCell(*cell);
}

const bool CGUIGridLayout_Impl::InGridRange(const int column, const int row) const
{
    return column >= 1 && row >= 1 && column <= m_columns && row <= m_rows;
}

const bool CGUIGridLayout_Impl::InColumnRange(const int column) const
{
    return column >= 1 && column <= m_columns;
}

const bool CGUIGridLayout_Impl::InRowRange(const int row) const
{
    return row >= 1 && row <= m_rows;
}

const CVector2D CGUIGridLayout_Impl::GetAlignmentOffset(const SGridCellItem& cell, const CVector2D& size) const
{
    CVector2D offset;

    switch (cell.alignment)
    {
        case eGridLayoutItemAlignment::TOP_LEFT:
            offset = CVector2D(0.0f, 0.0f);
            break;
        case eGridLayoutItemAlignment::TOP_CENTER:
            offset = CVector2D((1.0f - size.fX) / 2.0f, 0.0f);
            break;
        case eGridLayoutItemAlignment::TOP_RIGHT:
            offset = CVector2D(1.0f - size.fX, 0.0f);
            break;
        case eGridLayoutItemAlignment::MIDDLE_LEFT:
            offset = CVector2D(0.0f, (1.0f - size.fY) / 2.0f);
            break;
        case eGridLayoutItemAlignment::MIDDLE_CENTER:
            offset = CVector2D((1.0f - size.fX) / 2.0f, (1.0f - size.fY) / 2.0f);
            break;
        case eGridLayoutItemAlignment::MIDDLE_RIGHT:
            offset = CVector2D(1.0f - size.fX, (1.0f - size.fY) / 2.0f);
            break;
        case eGridLayoutItemAlignment::BOTTOM_LEFT:
            offset = CVector2D(0.0f, 1.0f - size.fY);
            break;
        case eGridLayoutItemAlignment::BOTTOM_CENTER:
            offset = CVector2D((1.0f - size.fX) / 2.0f, 1.0f - size.fY);
            break;
        case eGridLayoutItemAlignment::BOTTOM_RIGHT:
            offset = CVector2D(1.0f - size.fX, 1.0f - size.fY);
            break;
    }

    return offset;
}

const int CGUIGridLayout_Impl::CountColumnWidths(const int maxColumns) const
{
    return std::count_if(m_columnWidths.begin(), m_columnWidths.begin() + (maxColumns == 0 || maxColumns < 1 ? m_columns : maxColumns) - 1,
                         [&](const float& width) { return width > 0.0f; });
}

const int CGUIGridLayout_Impl::CountRowHeights(const int maxRows) const
{
    return std::count_if(m_rowHeights.begin(), m_rowHeights.begin() + (maxRows == 0 || maxRows < 1 ? m_rows : maxRows) - 1,
                         [&](const float& height) { return height > 0.0f; });
}
