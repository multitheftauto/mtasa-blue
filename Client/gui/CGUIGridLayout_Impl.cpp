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
    if (!InGridRange(column, row))
        return false;

    // If cell is already occupied
    if (m_grid[column - 1][row - 1] != 0)
    {
        return false;
    }

    auto* cell = GetCell(column, row);
    cell->element = item;

    item->SetParent(nullptr);
    item->SetParent(cell->container);

    item->SetPosition(CVector2D(0.0f, 0.0f), true);
    item->SetSize(CVector2D(1.0f, 1.0f), true);

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

const bool CGUIGridLayout_Impl::RemoveItem(const int column, const int row, const bool moveToPreviousCell)
{
    auto* cell = GetCell(column, row);

    if (cell == nullptr)
        return false;

    auto& element = cell->element;
    element->SetParent(nullptr);
    element = nullptr;
    m_grid[column - 1][row - 1] = 0;

    m_items.erase(element);

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

const bool CGUIGridLayout_Impl::RemoveItem(const CGUIElement* item, const bool moveToPreviousCell)
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
    return m_cells.count(id) ? m_cells.at(id) : nullptr;
}

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsInColumn(const int column)
{
    return GetCellsInGrid(column, 0, column, m_rows - 1);
}

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsInRow(const int row)
{
    return GetCellsInGrid(0, row, m_columns - 1, row);
}

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsInGrid(const int startColumn, const int startRow, const int endColumn, const int endRow)
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

std::vector<SGridCellItem*> CGUIGridLayout_Impl::GetCellsOutsideGrid(const int startColumn, const int startRow, const int endColumn, const int endRow)
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

    if (larger)
    {
        CreateGridCells();

        // Since the grid starts at 0,0 make sure an active cell is set
        m_activeColumn = (m_activeColumn == 0 ? 1 : m_activeColumn);
        m_activeRow = (m_activeRow == 0 ? 1 : m_activeRow);
    }
    else
    {
        CleanupGridItems();

        // Since the grid has been resized, we need to make sure the active cell is still within the grid
        m_activeColumn = std::min(m_activeColumn, columns);
        m_activeRow = std::min(m_activeRow, rows);
    }

    RepositionGridItems();
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

const std::pair<int, int> CGUIGridLayout_Impl::GetActiveCell()
{
    return std::make_pair(m_activeColumn, m_activeRow);
}

void CGUIGridLayout_Impl::SetItemAlignment(const CGUIElement* item, eGridLayoutItemAlignment alignment)
{
    auto* cell = GetCell(item);

    if (cell == nullptr)
        return;

    cell->alignment = alignment;
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
    if (!InGridRange(column, row))
        return false;

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

void CGUIGridLayout_Impl::CreateGridCells()
{
    auto* parent = reinterpret_cast<CGUIElement*>(this);

    for (int i = 0; i < m_columns; i++)
    {
        for (int j = 0; j < m_rows; j++)
        {
            const auto cell = m_cells.count(m_grid[i][j]);

            if (cell == 0)
            {
                auto* cell = new SGridCellItem();
                cell->container = m_pManager->CreateStaticImage(parent);
                cell->container->SetFrameEnabled(false);

                // Assign the cell container texture, alternative between m_cellTexture and m_cellTextureAlt
                cell->container->LoadFromTexture((i + j) % 2 == 0 ? m_cellTexture : m_cellTextureAlt);
                cell->container->SetAlpha(m_defaultCellAlpha);

                // Position the cell container in the grid
                cell->container->SetPosition(CVector2D(i * (1.0f / m_columns), j * (1.0f / m_rows)), true);

                // Size the cell container in the grid
                cell->container->SetSize(CVector2D(1.0f / m_columns, 1.0f / m_rows), true);

                cell->id = m_nextId;
                cell->element = nullptr;
                cell->column = i + 1;
                cell->row = j + 1;
                cell->alignment = m_defaultAlignment;

                m_cells.emplace(m_nextId, cell);
                m_grid[i][j] = m_nextId;

                m_nextId++;
            }
        }
    }
}

void CGUIGridLayout_Impl::CleanupGridItems()
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
                cell.second->element = nullptr;
            }

            delete cell.second;
            m_cells.erase(cell.first);
        }
    }
}

void CGUIGridLayout_Impl::RepositionGridItems()
{
    for (auto& cell : m_cells)
    {
        cell.second->container->SetPosition(CVector2D((cell.second->column - 1) * (1.0f / m_columns), (cell.second->row - 1) * (1.0f / m_rows)), true);
        cell.second->container->SetSize(CVector2D(1.0f / m_columns, 1.0f / m_rows), true);
    }
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
