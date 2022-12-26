/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaGUIDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

#define MAX_CHATBOX_LAYOUT_CVARS 21

class CLuaGUIDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(GUIGetInputEnabled);
    LUA_DECLARE(GUIGetInputMode);
    LUA_DECLARE(GUISetInputEnabled);
    LUA_DECLARE(GUISetInputMode);
    LUA_DECLARE(GUIIsChatBoxInputActive);
    LUA_DECLARE(GUIIsConsoleActive);
    LUA_DECLARE(GUIIsDebugViewActive);
    LUA_DECLARE(GUISetDebugViewActive);
    LUA_DECLARE(GUIIsMainMenuActive);
    LUA_DECLARE(GUIIsMTAWindowActive);
    LUA_DECLARE(GUIIsTransferBoxActive);
    LUA_DECLARE(GUICreateWindow);
    LUA_DECLARE(GUICreateLabel);
    LUA_DECLARE(GUICreateButton);
    LUA_DECLARE(GUICreateEdit);
    LUA_DECLARE(GUICreateMemo);
    LUA_DECLARE(GUICreateGridList);
    LUA_DECLARE(GUICreateScrollBar);
    LUA_DECLARE(GUICreateScrollPane);
    LUA_DECLARE(GUICreateTabPanel);
    LUA_DECLARE(GUICreateTab);
    LUA_DECLARE(GUICreateProgressBar);
    LUA_DECLARE(GUICreateCheckBox);
    LUA_DECLARE(GUICreateRadioButton);
    LUA_DECLARE(GUICreateStaticImage);
    LUA_DECLARE(GUICreateFont);
    LUA_DECLARE(GUIStaticImageLoadImage);
    LUA_DECLARE(GUIStaticImageGetNativeSize);
    LUA_DECLARE(GUIGetSelectedTab);
    LUA_DECLARE(GUISetSelectedTab);
    LUA_DECLARE(GUIDeleteTab);
    LUA_DECLARE(GUIScrollBarSetScrollPosition);
    LUA_DECLARE(GUIScrollBarGetScrollPosition);
    LUA_DECLARE(GUIGridListSetSortingEnabled);
    LUA_DECLARE(GUIGridListIsSortingEnabled);
    LUA_DECLARE(GUIGridListAddColumn);
    LUA_DECLARE(GUIGridListRemoveColumn);
    LUA_DECLARE(GUIGridListSetColumnWidth);
    LUA_DECLARE(GUIGridListGetColumnWidth);
    LUA_DECLARE(GUIGridListSetColumnTitle);
    LUA_DECLARE(GUIGridListGetColumnTitle);
    LUA_DECLARE(GUIGridListSetScrollBars);
    LUA_DECLARE(GUIGridListGetRowCount);
    LUA_DECLARE(GUIGridListGetColumnCount);
    LUA_DECLARE(GUIGridListAddRow);
    LUA_DECLARE(GUIGridListInsertRowAfter);
    LUA_DECLARE(GUIGridListRemoveRow);
    LUA_DECLARE(GUIGridListAutoSizeColumn);
    LUA_DECLARE(GUIGridListClear);
    LUA_DECLARE(GUIGridListSetItemText);
    LUA_DECLARE(GUIGridListGetItemText);
    LUA_DECLARE(GUIGridListSetItemData);
    LUA_DECLARE(GUIGridListGetItemData);
    LUA_DECLARE(GUIGridListSetItemColor);
    LUA_DECLARE(GUIGridListGetItemColor);
    LUA_DECLARE(GUIGridListSetSelectionMode);
    LUA_DECLARE(GUIGridListGetSelectionMode);
    LUA_DECLARE(GUIGridListGetSelectedItem);
    LUA_DECLARE(GUIGridListGetSelectedItems);
    LUA_DECLARE(GUIGridListGetSelectedCount);
    LUA_DECLARE(GUIGridListSetSelectedItem);
    LUA_DECLARE(GUIGridListSetHorizontalScrollPosition);
    LUA_DECLARE(GUIGridListGetHorizontalScrollPosition);
    LUA_DECLARE(GUIGridListSetVerticalScrollPosition);
    LUA_DECLARE(GUIGridListGetVerticalScrollPosition);
    LUA_DECLARE(GUIScrollPaneSetScrollBars);
    LUA_DECLARE(GUIScrollPaneSetHorizontalScrollPosition);
    LUA_DECLARE(GUIScrollPaneGetHorizontalScrollPosition);
    LUA_DECLARE(GUIScrollPaneSetVerticalScrollPosition);
    LUA_DECLARE(GUIScrollPaneGetVerticalScrollPosition);
    LUA_DECLARE(GUIDestroyElement);
    LUA_DECLARE(GUISetEnabled);
    LUA_DECLARE(GUISetText);
    LUA_DECLARE(GUISetFont);
    LUA_DECLARE(GUISetSize);
    LUA_DECLARE(GUISetPosition);
    LUA_DECLARE(GUISetVisible);
    LUA_DECLARE(GUISetAlpha);
    LUA_DECLARE(GUISetProperty);
    LUA_DECLARE(GUIBringToFront);
    LUA_DECLARE(GUIMoveToBack);
    LUA_DECLARE(GUIBlur);
    LUA_DECLARE(GUIFocus);
    LUA_DECLARE(GUICheckBoxSetSelected);
    LUA_DECLARE(GUIRadioButtonSetSelected);
    LUA_DECLARE(GUIGetEnabled);
    LUA_DECLARE(GUIGetText);
    LUA_DECLARE(GUIGetFont);
    LUA_DECLARE(GUIGetSize);
    LUA_DECLARE(GUIGetPosition);
    LUA_DECLARE(GUIGetVisible);
    LUA_DECLARE(GUIGetAlpha);
    LUA_DECLARE(GUIGetProperty);
    LUA_DECLARE(GUIGetProperties);
    LUA_DECLARE(GUICheckBoxGetSelected);
    LUA_DECLARE(GUIRadioButtonGetSelected);
    LUA_DECLARE(GUIGetScreenSize);
    LUA_DECLARE(GUIProgressBarSetProgress);
    LUA_DECLARE(GUIProgressBarGetProgress);
    LUA_DECLARE(GUIEditSetReadOnly);
    LUA_DECLARE(GUIEditIsReadOnly);
    LUA_DECLARE(GUIEditSetMasked);
    LUA_DECLARE(GUIEditIsMasked);
    LUA_DECLARE(GUIEditSetMaxLength);
    LUA_DECLARE(GUIEditGetMaxLength);
    LUA_DECLARE(GUIEditSetCaretIndex);
    LUA_DECLARE(GUIEditGetCaretIndex);
    LUA_DECLARE(GUIMemoSetReadOnly);
    LUA_DECLARE(GUIMemoIsReadOnly);
    LUA_DECLARE(GUIMemoSetCaretIndex);
    LUA_DECLARE(GUIMemoGetCaretIndex);
    LUA_DECLARE(GUIMemoGetVerticalScrollPosition);
    LUA_DECLARE(GUIMemoSetVerticalScrollPosition);
    LUA_DECLARE(GUIWindowSetMovable);
    LUA_DECLARE(GUIWindowSetSizable);
    LUA_DECLARE(GUIWindowIsMovable);
    LUA_DECLARE(GUIWindowIsSizable);
    LUA_DECLARE(GUIWindowGetCloseButtonEnabled);
    LUA_DECLARE(GUIWindowGetTitleBarEnabled);
    LUA_DECLARE(GUILabelSetColor);
    LUA_DECLARE(GUILabelGetColor);
    LUA_DECLARE(GUILabelSetVerticalAlign);
    LUA_DECLARE(GUILabelSetHorizontalAlign);
    LUA_DECLARE(GUILabelGetTextExtent);
    LUA_DECLARE(GUILabelGetFontHeight);
    LUA_DECLARE(GUIGetChatboxLayout);
    LUA_DECLARE(GUICreateComboBox);
    LUA_DECLARE(GUIComboBoxAddItem);
    LUA_DECLARE(GUIComboBoxRemoveItem);
    LUA_DECLARE(GUIComboBoxClear);
    LUA_DECLARE(GUIComboBoxGetSelected);
    LUA_DECLARE(GUIComboBoxSetSelected);
    LUA_DECLARE(GUIComboBoxGetItemText);
    LUA_DECLARE(GUIComboBoxSetItemText);
    LUA_DECLARE(GUIComboBoxGetItemCount);
    LUA_DECLARE(GUIComboBoxSetOpen);
    LUA_DECLARE(GUIComboBoxIsOpen);
    LUA_DECLARE(GUIGetCursorType);

    static bool GUISetChatboxCharacterLimit(int charLimit);
    static int  GUIGetChatboxCharacterLimit();

private:
    static void AddGuiElementClass(lua_State* luaVM);
    static void AddGuiFontClass(lua_State* luaVM);
    static void AddGuiWindowClass(lua_State* luaVM);
    static void AddGuiButtonClass(lua_State* luaVM);
    static void AddGuiEditClass(lua_State* luaVM);
    static void AddGuiLabelClass(lua_State* luaVM);
    static void AddGuiMemoClass(lua_State* luaVM);
    static void AddGuiImageClass(lua_State* luaVM);
    static void AddGuiComboBoxClass(lua_State* luaVM);
    static void AddGuiCheckBoxClass(lua_State* luaVM);
    static void AddGuiRadioButtonClass(lua_State* luaVM);
    static void AddGuiScrollPaneClass(lua_State* luaVM);
    static void AddGuiScrollBarClass(lua_State* luaVM);
    static void AddGuiProgressBarClass(lua_State* luaVM);
    static void AddGuiGridlistClass(lua_State* luaVM);
    static void AddGuiTabPanelClass(lua_State* luaVM);
    static void AddGuiTabClass(lua_State* luaVM);
};
