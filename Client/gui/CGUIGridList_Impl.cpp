/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIGridList_Impl.cpp
 *  PURPOSE:     Grid list widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

namespace
{
    constexpr const char  kGridListName[] = "CGUI/MultiColumnList";
    constexpr const char  kGridListNoFrameName[] = "CGUI/MultiColumnList";  // MultiColumnListNoFrame
    constexpr const char  kGridListSpacer[] = "   ";
    constexpr std::size_t kGridListSpacerLength = (sizeof(kGridListSpacer) / sizeof(char)) - 1;
    constexpr std::size_t kGridListMaxTextLength = 256;

    constexpr CEGUI::ListHeaderSegment::SortDirection ToCeguiSortDirection(SortDirection direction) noexcept
    {
        switch (direction)
        {
            case SortDirections::Ascending:
                return CEGUI::ListHeaderSegment::Ascending;
            case SortDirections::Descending:
                return CEGUI::ListHeaderSegment::Descending;
            default:
                return CEGUI::ListHeaderSegment::None;
        }
    }

    constexpr SortDirection FromCeguiSortDirection(CEGUI::ListHeaderSegment::SortDirection direction) noexcept
    {
        switch (direction)
        {
            case CEGUI::ListHeaderSegment::Ascending:
                return SortDirections::Ascending;
            case CEGUI::ListHeaderSegment::Descending:
                return SortDirections::Descending;
            default:
                return SortDirections::None;
        }
    }

    inline unsigned int NormalizeSortColumn(unsigned int requestedColumn, unsigned int columnCount) noexcept
    {
        if (columnCount == 0)
            return 0;

        if (requestedColumn == 0 || requestedColumn > columnCount)
            return columnCount;

        return requestedColumn;
    }
}

CGUIGridList_Impl::CGUIGridList_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, bool bFrame)
{
    SetManager(pGUI);

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    if (bFrame)
        m_pWindow = pGUI->GetWindowManager()->createWindow(kGridListName, szUnique);
    else
        m_pWindow = pGUI->GetWindowManager()->createWindow(kGridListNoFrameName, szUnique);

    m_pWindow->setDestroyedByParent(false);
    m_pWindow->setRect(CEGUI::Relative, CEGUI::Rect(0.00f, 0.00f, 0.40f, 0.40f));

    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setUserColumnDraggingEnabled(false);
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setShowHorzScrollbar(false);
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setSortDirection(CEGUI::ListHeaderSegment::None);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    // Register our events
    m_pWindow->subscribeEvent(CEGUI::MultiColumnList::EventSortColumnChanged, CEGUI::Event::Subscriber(&CGUIGridList_Impl::Event_OnSortColumn, this));
    m_pWindow->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged, CEGUI::Event::Subscriber(&CGUIGridList_Impl::Event_OnSelectionChanged, this));
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
}

CGUIGridList_Impl::~CGUIGridList_Impl()
{
    Clear();
    DestroyElement();
}

void CGUIGridList_Impl::SetSortingEnabled(bool bEnabled)
{
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setUserSortControlEnabled(bEnabled);
}

bool CGUIGridList_Impl::IsSortingEnabled()
{
    return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->isUserSortControlEnabled();
}

void CGUIGridList_Impl::SetAutoSortSuppressed(bool bSuppressed)
{
    if (m_bAutoSortSuppressed == bSuppressed)
        return;

    CEGUI::MultiColumnList* pList = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);

    if (bSuppressed)
    {
        m_bAutoSortSuppressed = true;
        m_uiPendingSortColumn = static_cast<unsigned int>(pList->getSortColumn() + 1);
        m_PendingSortDirection = FromCeguiSortDirection(pList->getSortDirection());
        pList->setSortDirection(CEGUI::ListHeaderSegment::None);
        UpdateSortIndicator(m_uiPendingSortColumn, m_PendingSortDirection);
        return;
    }

    m_bAutoSortSuppressed = false;

    const unsigned int columnCount = static_cast<unsigned int>(pList->getColumnCount());
    if (columnCount == 0)
    {
        m_uiPendingSortColumn = 0;
        m_PendingSortDirection = SortDirections::None;
        return;
    }

    const unsigned int column = NormalizeSortColumn(m_uiPendingSortColumn, columnCount);
    UpdateSortIndicator(0, SortDirections::None);
    pList->setSortColumn(GetColumnIndex(column));
    pList->setSortDirection(ToCeguiSortDirection(m_PendingSortDirection));
}

bool CGUIGridList_Impl::IsAutoSortSuppressed() const
{
    return m_bAutoSortSuppressed;
}

void CGUIGridList_Impl::RemoveColumn(unsigned int uiColumn)
{
    try
    {
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->removeColumn(GetColumnIndex(uiColumn));
    }
    catch (CEGUI::Exception)
    {
    }
}

unsigned int CGUIGridList_Impl::AddColumn(const char* szTitle, float fWidth)
{
    // Create a new column with an unique handle
    int hUniqueHandle = GetUniqueHandle();
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->addColumn((CGUI_Impl::GetUTFString(szTitle)), hUniqueHandle, fWidth);

    int iColumnIndex = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getColumnWithID(hUniqueHandle);

    // Adjust the header style (zero-based)
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getHeaderSegmentForColumn(iColumnIndex).setFont("default-small");

    // Return the id (zero-based)
    return iColumnIndex + 1;
}

void CGUIGridList_Impl::SetColumnWidth(int hColumn, float fWidth, bool bRelative)
{
    try
    {
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setColumnHeaderWidth(GetColumnIndex(hColumn), fWidth, bRelative);
    }
    catch (CEGUI::Exception)
    {
    }
}

bool CGUIGridList_Impl::GetColumnWidth(int hColumn, float& fOutWidth, bool bRelative)
{
    try
    {
        fOutWidth = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getColumnHeaderWidth(GetColumnIndex(hColumn));

        if (!bRelative)
            fOutWidth = m_pWindow->relativeToAbsoluteX(fOutWidth);

        return true;
    }
    catch (CEGUI::Exception)
    {
    }
    return false;
}

void CGUIGridList_Impl::SetColumnTitle(int hColumn, const char* szTitle)
{
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setColumnHeaderTitle(GetColumnIndex(hColumn), szTitle);
}

const char* CGUIGridList_Impl::GetColumnTitle(int hColumn)
{
    return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getColumnHeaderTitle(GetColumnIndex(hColumn));
}

void CGUIGridList_Impl::SetHorizontalScrollBar(bool bEnabled)
{
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setShowHorzScrollbar(bEnabled);
}

void CGUIGridList_Impl::SetVerticalScrollBar(bool bEnabled)
{
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setShowVertScrollbar(bEnabled);
}

int CGUIGridList_Impl::GetRowCount()
{
    try
    {
        return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getRowCount();
    }
    catch (CEGUI::Exception)
    {
        return 0;
    }
}

int CGUIGridList_Impl::GetColumnCount()
{
    try
    {
        return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getColumnCount();
    }
    catch (CEGUI::Exception)
    {
        return 0;
    }
}

void CGUIGridList_Impl::ForceUpdate()
{
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->forceUpdate();
}

int CGUIGridList_Impl::AddRow(bool fast, std::vector<std::pair<SString, bool> >* m_items)
{
    try
    {
        int iRow = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->addRow(m_iIndex++, fast);
        if (m_items)
            return SetRowItemsText(iRow, m_items);
        else
            return iRow;
    }
    catch (CEGUI::Exception)
    {
        return -1;
    }
}

int CGUIGridList_Impl::SetRowItemsText(int iRow, std::vector<std::pair<SString, bool> >* m_items)
{
    const int sortColumn = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getSortColumn() + 1;  // MTA columns start at 1, CEGUI at 0

    int column = 1;
    for (const auto& [text, isNumber] : *m_items)
    {
        if (column != sortColumn)
            SetItemText(iRow, column, text.c_str(), isNumber, false, true);
        ++column;
    }

    if (sortColumn > 0)
    {
        const std::size_t sortIndex = static_cast<std::size_t>(sortColumn - 1);
        if (sortIndex < m_items->size())
        {
            const auto& [text, isNumber] = m_items->at(sortIndex);
            iRow = SetItemText(iRow, sortColumn, text.c_str(), isNumber, false, true);
        }
    }

    return iRow;
}

void CGUIGridList_Impl::RemoveRow(int iRow)
{
    try
    {
        return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->removeRow(iRow);
    }
    catch (CEGUI::Exception)
    {
    }
}

int CGUIGridList_Impl::InsertRowAfter(int iRow, std::vector<std::pair<SString, bool> >* m_items)
{
    try
    {
        int iNewRow = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->insertRow(iRow + 1, m_iIndex++);
        if (m_items)
            return SetRowItemsText(iNewRow, m_items);
        else
            return iNewRow;
    }
    catch (CEGUI::Exception)
    {
        return -1;
    }
}

void CGUIGridList_Impl::AutoSizeColumn(unsigned int hColumn)
{
    try
    {
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->autoSizeColumnHeader(GetColumnIndex(hColumn));
    }
    catch (CEGUI::Exception)
    {
    }
}

void CGUIGridList_Impl::Clear()
{
    try
    {
        m_iIndex = 0;

        // Don't remove sort options when the list is cleared
        // reinterpret_cast < CEGUI::MultiColumnList* > ( m_pWindow ) -> setSortColumn( 0 );
        // reinterpret_cast < CEGUI::MultiColumnList* > ( m_pWindow ) -> setSortDirection( CEGUI::ListHeaderSegment::None );
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->resetList();

        for (auto& entry : m_Items)
        {
            delete entry.second;
        }
        m_Items.clear();
    }
    catch (CEGUI::Exception)
    {
    }
}

CGUIListItem* CGUIGridList_Impl::GetItem(int iRow, int hColumn)
{
    try
    {
        CEGUI::MultiColumnList* pMultiColumnList = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);
        if ((uint)iRow >= pMultiColumnList->getRowCount() || (uint)GetColumnIndex(hColumn) >= pMultiColumnList->getColumnCount())
            return NULL;

        // Grab the item at the chosen row / column
        CEGUI::ListboxItem* pItem =
            reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getItemAtGridReference(CEGUI::MCLGridRef(iRow, GetColumnIndex(hColumn)));

        // If it exists, get the CGUIListItem by using the pool
        if (pItem)
        {
            return GetListItem(pItem);
        }
    }
    catch (CEGUI::Exception)
    {
    }

    return NULL;
}

const char* CGUIGridList_Impl::GetItemText(int iRow, int hColumn)
{
    try
    {
        CEGUI::MultiColumnList* pMultiColumnList = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);
        if ((uint)iRow >= pMultiColumnList->getRowCount() || (uint)GetColumnIndex(hColumn) >= pMultiColumnList->getColumnCount())
            return "";

        // Grab the item at the chosen row / column
        CEGUI::ListboxItem* pItem =
            reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getItemAtGridReference(CEGUI::MCLGridRef(iRow, GetColumnIndex(hColumn)));
        if (pItem)
        {
            const char* szRet = pItem->getText().c_str();

            if (!m_bIgnoreTextSpacer && GetColumnIndex(hColumn) == 0)
            {
                const unsigned char spacerSize = static_cast<unsigned char>(kGridListSpacerLength);
                if (strncmp(szRet, kGridListSpacer, spacerSize) == 0)
                    szRet += spacerSize;
            }

            return szRet;
        }
    }
    catch (CEGUI::Exception)
    {
        return "";
    }

    return "";
}

void* CGUIGridList_Impl::GetItemData(int iRow, int hColumn)
{
    CGUIListItem* pItem = GetItem(iRow, hColumn);

    return pItem ? pItem->GetData() : NULL;
}

void CGUIGridList_Impl::SetItemData(int iRow, int hColumn, void* pData, CGUICallback<void, void*> deleteDataCallback)
{
    // Get the current item at that offset and set the text
    CGUIListItem* pItem = GetItem(iRow, hColumn);
    if (pItem)
    {
        pItem->SetData(pData, deleteDataCallback);
    }
}

void CGUIGridList_Impl::SetItemData(int iRow, int hColumn, const char* pszData)
{
    // Get the current item at that offset and set the text
    CGUIListItem* pItem = GetItem(iRow, hColumn);
    if (pItem)
    {
        pItem->SetData(pszData);
    }
}

int CGUIGridList_Impl::SetItemText(int iRow, int hColumn, const char* szText, bool bNumber, bool bSection, bool bFast, const char* szSortText)
{
    try
    {
        CEGUI::MultiColumnList* list = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);
        const int               columnIndex = GetColumnIndex(hColumn);
        CGUIListItem_Impl*      pItem = reinterpret_cast<CGUIListItem_Impl*>(GetItem(iRow, hColumn));

        if (!pItem)
        {
            pItem = new CGUIListItem_Impl(szText, bNumber);
            CEGUI::ListboxItem* pListboxItem = pItem->GetListItem();
            list->setItem(pListboxItem, CEGUI::MCLGridRef(iRow, columnIndex), bFast);
            m_Items[pItem->GetListItem()] = pItem;
        }

        if (bSection)
        {
            pItem->SetFont("default-bold-small");
            pItem->SetDisabled(true);
            pItem->SetText(szText, szSortText);
        }
        else
        {
            pItem->SetFont(list->getFont()->getName().c_str());
            pItem->SetDisabled(false);

            if (columnIndex == 0 && !m_bIgnoreTextSpacer)
            {
                char szBuf[kGridListMaxTextLength];
                snprintf(szBuf, kGridListMaxTextLength - 1, "%s%s", kGridListSpacer, szText);
                szBuf[kGridListMaxTextLength - 1] = '\0';
                pItem->SetText(szBuf, szSortText);
            }
            else
            {
                pItem->SetText(szText, szSortText);
            }
        }

        if (list->getSortDirection() != SortDirections::None && list->getSortColumn() == columnIndex)
        {
            list->setSortColumn(list->getSortColumn());
            return GetItemRowIndex(pItem);
        }

        return iRow;
    }
    catch (CEGUI::Exception)
    {
    }

    return 0;
}

void CGUIGridList_Impl::SetItemColor(int iRow, int hColumn, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha)
{
    try
    {
        CEGUI::MultiColumnList* win = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);

        // Get the current item at that offset and set the color
        CGUIListItem_Impl* pItem = reinterpret_cast<CGUIListItem_Impl*>(GetItem(iRow, hColumn));
        if (pItem)
        {
            pItem->SetColor(ucRed, ucGreen, ucBlue, ucAlpha);
        }
    }
    catch (CEGUI::Exception)
    {
    }
}

bool CGUIGridList_Impl::GetItemColor(int iRow, int hColumn, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha)
{
    try
    {
        CEGUI::MultiColumnList* win = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);

        // Get the current item at that offset and get it's color
        CGUIListItem_Impl* pItem = reinterpret_cast<CGUIListItem_Impl*>(GetItem(iRow, hColumn));
        if (pItem)
        {
            return pItem->GetColor(ucRed, ucGreen, ucBlue, ucAlpha);
        }
    }
    catch (CEGUI::Exception)
    {
    }
    return false;
}

void CGUIGridList_Impl::SetColumnSegmentSizingEnabled(int hColumn, bool bEnabled)
{
    try
    {
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setUserColumnSegmentSizingEnabled(hColumn, bEnabled);
    }
    catch (CEGUI::Exception)
    {
    }
}

bool CGUIGridList_Impl::IsColumnSegmentSizingEnabled(int hColumn)
{
    try
    {
        return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->isUserColumnSegmentSizingEnabled(GetColumnIndex(hColumn));
    }
    catch (CEGUI::Exception)
    {
        return false;
    }
}

void CGUIGridList_Impl::SetItemImage(int iRow, int hColumn, CGUIStaticImage* pImage)
{
    try
    {
        CEGUI::MultiColumnList* pMultiColumnList = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);
        if ((uint)iRow >= pMultiColumnList->getRowCount() || (uint)GetColumnIndex(hColumn) >= pMultiColumnList->getColumnCount())
            return;

        // Get the current item at that offset
        CGUIListItem* pItem = GetItem(iRow, hColumn);
        if (pItem != NULL)
        {
            pItem->SetImage(pImage);
        }
        else
        {
            // If it doesn't, create it and set it in the gridlist
            CGUIListItem_Impl*  pNewItem = new CGUIListItem_Impl("", CGUIListItem_Impl::ImageItem, (CGUIStaticImage_Impl*)pImage);
            CEGUI::ListboxItem* pListboxItem = pNewItem->GetListItem();
            pMultiColumnList->setItem(pListboxItem, CEGUI::MCLGridRef(iRow, GetColumnIndex(hColumn)), true);

            // Put our new item in the map
            m_Items[pNewItem->GetListItem()] = pNewItem;
        }
    }
    catch (CEGUI::Exception)
    {
    }
}

float CGUIGridList_Impl::GetHorizontalScrollPosition()
{
    try
    {
        CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->d_horzScrollbar;
        if (pScrollbar)
            return (pScrollbar->getScrollPosition() / (pScrollbar->getDocumentSize() - pScrollbar->getPageSize()));
    }
    catch (CEGUI::Exception)
    {
    }
    return 0.0f;
}

float CGUIGridList_Impl::GetVerticalScrollPosition()
{
    try
    {
        CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->d_vertScrollbar;
        if (pScrollbar)
            return (pScrollbar->getScrollPosition() / (pScrollbar->getDocumentSize() - pScrollbar->getPageSize()));
    }
    catch (CEGUI::Exception)
    {
    }
    return 0.0f;
}

void CGUIGridList_Impl::SetHorizontalScrollPosition(float fPosition)
{
    try
    {
        CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->d_horzScrollbar;
        if (pScrollbar)
            pScrollbar->setScrollPosition(fPosition * (pScrollbar->getDocumentSize() - pScrollbar->getPageSize()));
    }
    catch (CEGUI::Exception)
    {
    }
}

void CGUIGridList_Impl::SetVerticalScrollPosition(float fPosition)
{
    try
    {
        CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->d_vertScrollbar;
        if (pScrollbar)
            pScrollbar->setScrollPosition(fPosition * (pScrollbar->getDocumentSize() - pScrollbar->getPageSize()));
    }
    catch (CEGUI::Exception)
    {
    }
}

int CGUIGridList_Impl::GetColumnIndex(int hColumn)
{
    /*
    try
    {
        // non zero-based
        return reinterpret_cast < CEGUI::MultiColumnList* > ( m_pWindow ) -> getColumnWithID ( hColumn );
    }
    catch ( CEGUI::Exception )
    {
        return -1;
    }
    */
    return hColumn - 1;
}

int CGUIGridList_Impl::GetSelectedCount()
{
    return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getSelectedCount();
}

int CGUIGridList_Impl::GetItemColumnIndex(CGUIListItem* pItem)
{
    if (pItem)
    {
        try
        {
            CGUIListItem_Impl* pItemImpl = (CGUIListItem_Impl*)pItem;
            return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getItemColumnIndex(pItemImpl->GetListItem());
        }
        catch (CEGUI::Exception)
        {
        }
    }
    return -1;
}

int CGUIGridList_Impl::GetItemRowIndex(CGUIListItem* pItem)
{
    if (pItem)
    {
        try
        {
            CGUIListItem_Impl* pItemImpl = (CGUIListItem_Impl*)pItem;
            return reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getItemRowIndex(pItemImpl->GetListItem());
        }
        catch (CEGUI::Exception)
        {
        }
    }
    return -1;
}

void CGUIGridList_Impl::SetSelectionMode(SelectionMode mode)
{
    try
    {
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setSelectionMode((CEGUI::MultiColumnList::SelectionMode)mode);
    }
    catch (CEGUI::Exception)
    {
    }
}

SelectionMode CGUIGridList_Impl::GetSelectionMode()
{
    return (SelectionMode) reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getSelectionMode();
}

void CGUIGridList_Impl::GetVisibleRowRange(int& iFirst, int& iLast)
{
    reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getVisibleRowRange(iFirst, iLast);
}

CGUIListItem* CGUIGridList_Impl::GetSelectedItem()
{
    return GetListItem(reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getFirstSelectedItem());
}

CGUIListItem* CGUIGridList_Impl::GetNextSelectedItem(CGUIListItem* pItem)
{
    if (pItem)
        return GetListItem(reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getNextSelected(((CGUIListItem_Impl*)pItem)->GetListItem()));
    else
        return GetListItem(reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getFirstSelectedItem());
}

int CGUIGridList_Impl::GetSelectedItemRow()
{
    CEGUI::ListboxItem* pItem = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getFirstSelectedItem();
    if (pItem)
    {
        CGUIListItem* pListItem = GetListItem(pItem);
        if (pListItem)
        {
            return GetItemRowIndex(pListItem);
        }
    }

    return -1;
}

int CGUIGridList_Impl::GetSelectedItemColumn()
{
    CEGUI::ListboxItem* pItem = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->getFirstSelectedItem();
    if (pItem)
    {
        CGUIListItem* pListItem = GetListItem(pItem);
        if (pListItem)
        {
            return GetItemColumnIndex(pListItem);
        }
    }

    return -1;
}

void CGUIGridList_Impl::SetSelectedItem(int iRow, int hColumn, bool bReset)
{
    if (bReset)
    {
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->clearAllSelections();
    }

    // Get the current item at that offset
    CGUIListItem* pItem = GetItem(iRow, hColumn);
    if (pItem != NULL)
    {
        // Use the grid to select the row, rather than the pItem method call as the grid one will take into account the
        // grid settings for full row select, etc...
        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->setItemSelectState(reinterpret_cast<CGUIListItem_Impl*>(pItem)->GetListItem(), true);

        reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow)->requestRedraw();
    }
}

void CGUIGridList_Impl::Sort(unsigned int uiColumn, SortDirection direction)
{
    CEGUI::MultiColumnList* pList = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);
    const unsigned int      columnCount = static_cast<unsigned int>(pList->getColumnCount());
    const unsigned int      column = NormalizeSortColumn(uiColumn, columnCount);
    if (column == 0)
        return;

    if (m_bAutoSortSuppressed)
    {
        m_uiPendingSortColumn = column;
        m_PendingSortDirection = direction;
        UpdateSortIndicator(column, direction);
        return;
    }

    pList->setSortColumn(GetColumnIndex(column));
    pList->setSortDirection(ToCeguiSortDirection(direction));
}

void CGUIGridList_Impl::GetSort(unsigned int& uiColumn, SortDirection& direction)
{
    if (m_bAutoSortSuppressed)
    {
        uiColumn = m_uiPendingSortColumn;
        direction = m_PendingSortDirection;
        return;
    }

    CEGUI::MultiColumnList* pList = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);
    uiColumn = pList->getSortColumn() + 1;
    direction = FromCeguiSortDirection(pList->getSortDirection());
}

void CGUIGridList_Impl::SetSortColumnHandler(GUI_CALLBACK Callback)
{
    m_OnSortColumn = Callback;
}

void CGUIGridList_Impl::SetSelectionHandler(GUI_CALLBACK Callback)
{
    m_OnSelectionChanged = Callback;
}

bool CGUIGridList_Impl::Event_OnSortColumn(const CEGUI::EventArgs& e)
{
    if (m_OnSortColumn)
        m_OnSortColumn(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIGridList_Impl::Event_OnSelectionChanged(const CEGUI::EventArgs& e)
{
    if (m_OnSelectionChanged)
        m_OnSelectionChanged(reinterpret_cast<CGUIElement*>(this));
    return true;
}

void CGUIGridList_Impl::UpdateSortIndicator(unsigned int uiColumn, SortDirection direction)
{
    try
    {
        CEGUI::MultiColumnList* pList = reinterpret_cast<CEGUI::MultiColumnList*>(m_pWindow);
        const auto              columnCount = static_cast<int>(pList->getColumnCount());
        const int               targetIndex = (uiColumn > 0) ? static_cast<int>(uiColumn - 1) : -1;

        for (auto i = 0; i < columnCount; ++i)
        {
            auto segDirection = CEGUI::ListHeaderSegment::None;
            if (i == targetIndex && direction != SortDirections::None)
                segDirection = ToCeguiSortDirection(direction);

            pList->getHeaderSegmentForColumn(i).setSortDirection(segDirection);
        }
    }
    catch (CEGUI::Exception)
    {
    }
}

unsigned int CGUIGridList_Impl::GetUniqueHandle()
{
    return ++m_hUniqueHandle;
}

CGUIListItem_Impl* CGUIGridList_Impl::GetListItem(CEGUI::ListboxItem* pItem)
{
    if (const auto it = m_Items.find(pItem); it != m_Items.end())
        return it->second;

    return nullptr;
}
