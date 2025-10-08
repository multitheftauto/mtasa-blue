/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIGridList_Impl.h
 *  PURPOSE:     Grid list widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIGridList.h>
#include "CGUIElement_Impl.h"

class CGUIListItem_Impl;

class CGUIGridList_Impl : public CGUIGridList, public CGUIElement_Impl
{
public:
    CGUIGridList_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, bool bFrame = true);
    ~CGUIGridList_Impl();

    unsigned int AddColumn(const char* szTitle, float fWidth);
    void         RemoveColumn(unsigned int uiColumn);
    void         AutoSizeColumn(unsigned int hColumn);
    void         SetColumnWidth(int hColumn, float fWidth, bool bRelative = true);
    bool         GetColumnWidth(int hColumn, float& fOutWidth, bool bRelative = true);
    void         SetColumnTitle(int hColumn, const char* szTitle);
    const char*  GetColumnTitle(int hColumn);

    void          SetSelectionMode(SelectionMode mode);
    SelectionMode GetSelectionMode();
    int           SetRowItemsText(int iRow, std::vector<std::pair<SString, bool> >* m_items);

    void          ForceUpdate();
    int           AddRow(bool bFast = false, std::vector<std::pair<SString, bool> >* m_items = NULL);
    void          RemoveRow(int iRow);
    int           InsertRowAfter(int iRow, std::vector<std::pair<SString, bool> >* m_items = NULL);
    void          Clear();
    CGUIListItem* GetItem(int iRow, int hColumn);
    const char*   GetItemText(int iRow, int hColumn);
    int  SetItemText(int iRow, int hColumn, const char* szText, bool bNumber = false, bool bSection = false, bool bFast = false, const char* szSortText = NULL);
    void SetItemData(int iRow, int hColumn, void* pData, CGUICallback<void, void*> deleteDataCallback = NULL);
    void SetItemData(int iRow, int hColumn, const char* pszData);
    void* GetItemData(int iRow, int hColumn);
    void  SetItemColor(int iRow, int hColumn, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha);
    bool  GetItemColor(int iRow, int hColumn, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha);

    void SetHorizontalScrollBar(bool bEnabled);
    void SetVerticalScrollBar(bool bEnabled);
    void SetSortingEnabled(bool bEnabled);
    bool IsSortingEnabled();
    void SetItemImage(int iRow, int hColumn, CGUIStaticImage* pImage);

    float GetHorizontalScrollPosition();
    float GetVerticalScrollPosition();
    void  SetHorizontalScrollPosition(float fPosition);
    void  SetVerticalScrollPosition(float fPosition);

    int           GetColumnIndex(int hColumn);
    int           GetItemColumnIndex(CGUIListItem* pItem);
    int           GetItemRowIndex(CGUIListItem* pItem);
    void          GetVisibleRowRange(int& iFirst, int& iLast);
    int           GetSelectedCount();
    CGUIListItem* GetSelectedItem();
    CGUIListItem* GetNextSelectedItem(CGUIListItem* pItem);
    int           GetSelectedItemRow();
    int           GetSelectedItemColumn();
    int           GetRowCount();
    int           GetColumnCount();

    bool IsColumnSegmentSizingEnabled(int hColumn);
    void SetColumnSegmentSizingEnabled(int hColumn, bool bEnabled);

    void Sort(unsigned int uiColumn, SortDirection direction);
    void GetSort(unsigned int& uiColumn, SortDirection& direction);

    void SetSelectedItem(int iRow, int hColumn, bool bReset);

    void SetSortColumnHandler(GUI_CALLBACK Callback);

    void      SetIgnoreTextSpacer(bool bIgnoreTextSpacer) { m_bIgnoreTextSpacer = bIgnoreTextSpacer; };
    eCGUIType GetType() { return CGUI_GRIDLIST; };

    #include "CGUIElement_Inc.h"

private:
    bool Event_OnSortColumn(const CEGUI::EventArgs& e);

    int m_iIndex;

    unsigned int       GetUniqueHandle();
    CGUIListItem_Impl* GetListItem(CEGUI::ListboxItem* pItem);
    unsigned int       m_hUniqueHandle;

    CFastHashMap<CEGUI::ListboxItem*, CGUIListItem_Impl*> m_Items;

    GUI_CALLBACK m_OnSortColumn;

    bool m_bIgnoreTextSpacer;
};
