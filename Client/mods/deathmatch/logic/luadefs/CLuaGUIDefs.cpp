/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaGUIDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

static const SFixedArray<const char*, MAX_CHATBOX_LAYOUT_CVARS> g_chatboxLayoutCVars = {{"chat_font",
                                                                                         "chat_lines",
                                                                                         "chat_color",
                                                                                         "chat_text_color",
                                                                                         "chat_input_color",
                                                                                         "chat_input_prefix_color",
                                                                                         "chat_input_text_color",
                                                                                         "chat_scale",
                                                                                         "chat_position_offset_x",
                                                                                         "chat_position_offset_y",
                                                                                         "chat_position_horizontal",
                                                                                         "chat_position_vertical",
                                                                                         "chat_text_alignment",
                                                                                         "chat_width",
                                                                                         "chat_css_style_text",
                                                                                         "chat_css_style_background",
                                                                                         "chat_line_life",
                                                                                         "chat_line_fade_out",
                                                                                         "chat_use_cegui",
                                                                                         "text_scale",
                                                                                         "chat_text_outline"}};

void CLuaGUIDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"guiGetInputEnabled", GUIGetInputEnabled},
        {"guiSetInputEnabled", GUISetInputEnabled},
        {"guiGetInputMode", GUIGetInputMode},
        {"guiSetInputMode", GUISetInputMode},

        {"isChatBoxInputActive", GUIIsChatBoxInputActive},
        {"isConsoleActive", GUIIsConsoleActive},
        {"isDebugViewActive", GUIIsDebugViewActive},
        {"setDebugViewActive", GUISetDebugViewActive},
        {"isMainMenuActive", GUIIsMainMenuActive},
        {"isMTAWindowActive", GUIIsMTAWindowActive},
        {"isTransferBoxActive", GUIIsTransferBoxActive},

        {"setChatboxCharacterLimit", ArgumentParser<GUISetChatboxCharacterLimit>},
        {"getChatboxCharacterLimit", ArgumentParser<GUIGetChatboxCharacterLimit>},

        {"guiCreateWindow", GUICreateWindow},
        {"guiCreateLabel", GUICreateLabel},
        {"guiCreateButton", GUICreateButton},
        {"guiCreateEdit", GUICreateEdit},
        {"guiCreateMemo", GUICreateMemo},
        {"guiCreateGridList", GUICreateGridList},
        {"guiCreateScrollPane", GUICreateScrollPane},
        {"guiCreateScrollBar", GUICreateScrollBar},
        {"guiCreateTabPanel", GUICreateTabPanel},
        {"guiCreateTab", GUICreateTab},
        {"guiCreateProgressBar", GUICreateProgressBar},
        {"guiCreateCheckBox", GUICreateCheckBox},
        {"guiCreateRadioButton", GUICreateRadioButton},
        {"guiCreateStaticImage", GUICreateStaticImage},
        {"guiCreateComboBox", GUICreateComboBox},
        {"guiCreateFont", GUICreateFont},

        {"guiStaticImageLoadImage", GUIStaticImageLoadImage},
        {"guiStaticImageGetNativeSize", GUIStaticImageGetNativeSize},
        {"guiGetSelectedTab", GUIGetSelectedTab},
        {"guiSetSelectedTab", GUISetSelectedTab},
        {"guiDeleteTab", GUIDeleteTab},

        {"guiGridListSetSortingEnabled", GUIGridListSetSortingEnabled},
        {"guiGridListIsSortingEnabled", GUIGridListIsSortingEnabled},
        {"guiGridListAddColumn", GUIGridListAddColumn},
        {"guiGridListRemoveColumn", GUIGridListRemoveColumn},
        {"guiGridListSetColumnWidth", GUIGridListSetColumnWidth},
        {"guiGridListGetColumnWidth", GUIGridListGetColumnWidth},
        {"guiGridListSetColumnTitle", GUIGridListSetColumnTitle},
        {"guiGridListGetColumnTitle", GUIGridListGetColumnTitle},
        {"guiGridListSetScrollBars", GUIGridListSetScrollBars},
        {"guiGridListGetRowCount", GUIGridListGetRowCount},
        {"guiGridListGetColumnCount", GUIGridListGetColumnCount},
        {"guiGridListAddRow", GUIGridListAddRow},
        {"guiGridListInsertRowAfter", GUIGridListInsertRowAfter},
        {"guiGridListRemoveRow", GUIGridListRemoveRow},
        {"guiGridListAutoSizeColumn", GUIGridListAutoSizeColumn},
        {"guiGridListClear", GUIGridListClear},
        {"guiGridListSetItemText", GUIGridListSetItemText},
        {"guiGridListGetItemText", GUIGridListGetItemText},
        {"guiGridListSetItemData", GUIGridListSetItemData},
        {"guiGridListGetItemData", GUIGridListGetItemData},
        {"guiGridListSetItemColor", GUIGridListSetItemColor},
        {"guiGridListGetItemColor", GUIGridListGetItemColor},
        {"guiGridListSetSelectionMode", GUIGridListSetSelectionMode},
        {"guiGridListGetSelectionMode", GUIGridListGetSelectionMode},
        {"guiGridListGetSelectedItem", GUIGridListGetSelectedItem},
        {"guiGridListGetSelectedItems", GUIGridListGetSelectedItems},
        {"guiGridListGetSelectedCount", GUIGridListGetSelectedCount},
        {"guiGridListSetSelectedItem", GUIGridListSetSelectedItem},
        {"guiGridListSetHorizontalScrollPosition", GUIGridListSetHorizontalScrollPosition},
        {"guiGridListGetHorizontalScrollPosition", GUIGridListGetHorizontalScrollPosition},
        {"guiGridListSetVerticalScrollPosition", GUIGridListSetVerticalScrollPosition},
        {"guiGridListGetVerticalScrollPosition", GUIGridListGetVerticalScrollPosition},

        {"guiScrollPaneSetScrollBars", GUIScrollPaneSetScrollBars},
        {"guiScrollPaneSetHorizontalScrollPosition", GUIScrollPaneSetHorizontalScrollPosition},
        {"guiScrollPaneGetHorizontalScrollPosition", GUIScrollPaneGetHorizontalScrollPosition},
        {"guiScrollPaneSetVerticalScrollPosition", GUIScrollPaneSetVerticalScrollPosition},
        {"guiScrollPaneGetVerticalScrollPosition", GUIScrollPaneGetVerticalScrollPosition},

        {"guiScrollBarSetScrollPosition", GUIScrollBarSetScrollPosition},
        {"guiScrollBarGetScrollPosition", GUIScrollBarGetScrollPosition},

        {"guiSetEnabled", GUISetEnabled},
        {"guiSetProperty", GUISetProperty},
        {"guiSetAlpha", GUISetAlpha},
        {"guiSetText", GUISetText},
        {"guiSetFont", GUISetFont},
        {"guiSetSize", GUISetSize},
        {"guiSetPosition", GUISetPosition},
        {"guiSetVisible", GUISetVisible},

        {"guiBringToFront", GUIBringToFront},
        {"guiMoveToBack", GUIMoveToBack},
        {"guiBlur", GUIBlur},
        {"guiFocus", GUIFocus},

        {"guiCheckBoxSetSelected", GUICheckBoxSetSelected},
        {"guiRadioButtonSetSelected", GUIRadioButtonSetSelected},

        {"guiGetEnabled", GUIGetEnabled},
        {"guiGetProperty", GUIGetProperty},
        {"guiGetProperties", GUIGetProperties},
        {"guiGetAlpha", GUIGetAlpha},
        {"guiGetText", GUIGetText},
        {"guiGetFont", GUIGetFont},
        {"guiGetSize", GUIGetSize},
        {"guiGetPosition", GUIGetPosition},
        {"guiGetVisible", GUIGetVisible},
        {"guiGetCursorType", GUIGetCursorType},

        {"guiCheckBoxGetSelected", GUICheckBoxGetSelected},
        {"guiRadioButtonGetSelected", GUIRadioButtonGetSelected},

        {"guiProgressBarSetProgress", GUIProgressBarSetProgress},
        {"guiProgressBarGetProgress", GUIProgressBarGetProgress},

        {"guiGetScreenSize", GUIGetScreenSize},

        {"guiEditSetCaretIndex", GUIEditSetCaretIndex},
        {"guiEditGetCaretIndex", GUIEditGetCaretIndex},
        {"guiEditSetMasked", GUIEditSetMasked},
        {"guiEditIsMasked", GUIEditIsMasked},
        {"guiEditSetMaxLength", GUIEditSetMaxLength},
        {"guiEditGetMaxLength", GUIEditGetMaxLength},
        {"guiEditSetReadOnly", GUIEditSetReadOnly},
        {"guiEditIsReadOnly", GUIEditIsReadOnly},

        {"guiMemoSetCaretIndex", GUIMemoSetCaretIndex},
        {"guiMemoGetCaretIndex", GUIMemoGetCaretIndex},
        {"guiMemoSetReadOnly", GUIMemoSetReadOnly},
        {"guiMemoIsReadOnly", GUIMemoIsReadOnly},
        {"guiMemoSetVerticalScrollPosition", GUIMemoSetVerticalScrollPosition},
        {"guiMemoGetVerticalScrollPosition", GUIMemoGetVerticalScrollPosition},

        {"guiLabelSetColor", GUILabelSetColor},
        {"guiLabelGetColor", GUILabelGetColor},
        {"guiLabelSetVerticalAlign", GUILabelSetVerticalAlign},
        {"guiLabelSetHorizontalAlign", GUILabelSetHorizontalAlign},

        {"guiLabelGetTextExtent", GUILabelGetTextExtent},
        {"guiLabelGetFontHeight", GUILabelGetFontHeight},

        {"guiWindowSetMovable", GUIWindowSetMovable},
        {"guiWindowSetSizable", GUIWindowSetSizable},
        {"guiWindowIsMovable", GUIWindowIsMovable},
        {"guiWindowIsSizable", GUIWindowIsSizable},

        {"getChatboxLayout", GUIGetChatboxLayout},

        {"guiComboBoxAddItem", GUIComboBoxAddItem},
        {"guiComboBoxRemoveItem", GUIComboBoxRemoveItem},
        {"guiComboBoxClear", GUIComboBoxClear},
        {"guiComboBoxGetSelected", GUIComboBoxGetSelected},
        {"guiComboBoxSetSelected", GUIComboBoxSetSelected},
        {"guiComboBoxGetItemText", GUIComboBoxGetItemText},
        {"guiComboBoxSetItemText", GUIComboBoxSetItemText},
        {"guiComboBoxGetItemCount", GUIComboBoxGetItemCount},
        {"guiComboBoxSetOpen", GUIComboBoxSetOpen},
        {"guiComboBoxIsOpen", GUIComboBoxIsOpen},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaGUIDefs::AddClass(lua_State* luaVM)
{
    AddGuiElementClass(luaVM);
    AddGuiFontClass(luaVM);
    AddGuiWindowClass(luaVM);
    AddGuiButtonClass(luaVM);
    AddGuiEditClass(luaVM);
    AddGuiLabelClass(luaVM);
    AddGuiMemoClass(luaVM);
    AddGuiImageClass(luaVM);
    AddGuiComboBoxClass(luaVM);
    AddGuiCheckBoxClass(luaVM);
    AddGuiRadioButtonClass(luaVM);
    AddGuiScrollPaneClass(luaVM);
    AddGuiScrollBarClass(luaVM);
    AddGuiProgressBarClass(luaVM);
    AddGuiGridlistClass(luaVM);
    AddGuiTabPanelClass(luaVM);
    AddGuiTabClass(luaVM);
}

// TODO: vector class
void CLuaGUIDefs::AddGuiElementClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "bringToFront", "guiBringToFront");
    lua_classfunction(luaVM, "moveToBack", "guiMoveToBack");
    lua_classfunction(luaVM, "blur", "guiBlur");
    lua_classfunction(luaVM, "focus", "guiFocus");

    lua_classfunction(luaVM, "isChatBoxInputActive", "isChatBoxInputActive");
    lua_classfunction(luaVM, "isConsoleActive", "isConsoleActive");
    lua_classfunction(luaVM, "isDebugViewActive", "isDebugViewActive");
    lua_classfunction(luaVM, "setDebugViewActive", "setDebugViewActive");
    lua_classfunction(luaVM, "isMainMenuActive", "isMainMenuActive");
    lua_classfunction(luaVM, "isMTAWindowActive", "isMTAWindowActive");
    lua_classfunction(luaVM, "isTransferBoxActive", "isTransferBoxActive");
    lua_classfunction(luaVM, "isInputEnabled", "guiGetInputEnabled");
    lua_classfunction(luaVM, "getInputMode", "guiGetInputMode");
    lua_classfunction(luaVM, "getCursorType", "guiGetCursorType");
    lua_classfunction(luaVM, "setChatboxCharacterLimit", "setChatboxCharacterLimit");
    lua_classfunction(luaVM, "getChatboxCharacterLimit", "getChatboxCharacterLimit");

    lua_classfunction(luaVM, "getScreenSize", "guiGetScreenSize");
    lua_classfunction(luaVM, "getProperties", "guiGetProperties");
    lua_classfunction(luaVM, "getAlpha", "guiGetAlpha");
    lua_classfunction(luaVM, "getFont", "guiGetFont");
    lua_classfunction(luaVM, "getEnabled", "guiGetEnabled");
    lua_classfunction(luaVM, "getVisible", "guiGetVisible");
    lua_classfunction(luaVM, "getText", "guiGetText");
    lua_classfunction(luaVM, "getPosition", "guiGetPosition");
    lua_classfunction(luaVM, "getSize", "guiGetSize");
    lua_classfunction(luaVM, "getProperty", "guiGetProperty");

    lua_classfunction(luaVM, "setInputEnabled", "guiSetInputEnabled");
    lua_classfunction(luaVM, "setAlpha", "guiSetAlpha");
    lua_classfunction(luaVM, "setEnabled", "guiSetEnabled");
    lua_classfunction(luaVM, "setFont", "guiSetFont");
    lua_classfunction(luaVM, "setVisible", "guiSetVisible");
    lua_classfunction(luaVM, "setText", "guiSetText");
    lua_classfunction(luaVM, "setInputMode", "guiSetInputMode");
    lua_classfunction(luaVM, "setProperty", "guiSetProperty");
    lua_classfunction(luaVM, "setPosition", "guiSetPosition");
    lua_classfunction(luaVM, "setSize", "guiSetSize");

    lua_classvariable(luaVM, "chatBoxInputActive", NULL, "isChatBoxInputActive");
    lua_classvariable(luaVM, "consoleActive", NULL, "isConsoleActive");
    lua_classvariable(luaVM, "debugViewActive", "setDebugViewActive", "isDebugViewActive");
    lua_classvariable(luaVM, "mainMenuActive", NULL, "isMainMenuActive");
    lua_classvariable(luaVM, "mtaWindowActive", NULL, "isMTAWindowActive");
    lua_classvariable(luaVM, "transferBoxActive", NULL, "isTransferBoxActive");
    lua_classvariable(luaVM, "inputEnabled", "guiSetInputEnabled", "guiGetInputEnabled");
    lua_classvariable(luaVM, "inputMode", "guiGetInputMode", "guiSetInputMode");
    lua_classvariable(luaVM, "cursorAlpha", "setCursorAlpha", "getCursorAlpha");
    lua_classvariable(luaVM, "cursorType", NULL, "guiGetCursorType");
    lua_classvariable(luaVM, "font", "guiSetFont", "guiGetFont");
    lua_classvariable(luaVM, "visible", "guiSetVisible", "guiGetVisible");
    lua_classvariable(luaVM, "properties", NULL, "guiGetProperties");
    lua_classvariable(luaVM, "alpha", "guiSetAlpha", "guiGetAlpha");
    lua_classvariable(luaVM, "enabled", "guiSetEnabled", "guiGetEnabled");
    lua_classvariable(luaVM, "text", "guiSetText", "guiGetText");
    lua_classvariable(luaVM, "size", "guiSetSize", "guiGetSize");
    lua_classvariable(luaVM, "position", "guiSetPosition", "guiGetPosition");
    lua_classvariable(luaVM, "chatboxCharacterLimit", "setChatboxCharacterLimit", "getChatboxCharacterLimit");

    lua_registerclass(luaVM, "GuiElement", "Element");
}

void CLuaGUIDefs::AddGuiWindowClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateWindow");

    lua_classfunction(luaVM, "setMovable", "guiWindowSetMovable");
    lua_classfunction(luaVM, "setSizable", "guiWindowSetSizable");

    lua_classfunction(luaVM, "isMovable", "guiWindowIsMovable");
    lua_classfunction(luaVM, "isSizable", "guiWindowIsSizable");

    lua_classvariable(luaVM, "movable", "guiWindowSetMovable", "guiWindowIsMovable");
    lua_classvariable(luaVM, "sizable", "guiWindowSetSizable", "guiWindowIsSizable");

    lua_registerclass(luaVM, "GuiWindow", "GuiElement");
}

void CLuaGUIDefs::AddGuiButtonClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateButton");

    lua_registerclass(luaVM, "GuiButton", "GuiElement");
}

void CLuaGUIDefs::AddGuiEditClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateEdit");
    lua_classfunction(luaVM, "getCaretIndex", "guiEditGetCaretIndex");
    lua_classfunction(luaVM, "setCaretIndex", "guiEditSetCaretIndex");
    lua_classfunction(luaVM, "setReadOnly", "guiEditSetReadOnly");
    lua_classfunction(luaVM, "setMasked", "guiEditSetMasked");
    lua_classfunction(luaVM, "setMaxLength", "guiEditSetMaxLength");

    lua_classvariable(luaVM, "caretIndex", "guiEditSetCaretIndex", "guiEditGetCaretIndex");
    lua_classvariable(luaVM, "readOnly", "guiEditSetReadOnly", "guiEditIsReadOnly");
    lua_classvariable(luaVM, "masked", "guiEditSetMasked", "guiEditIsMasked");
    lua_classvariable(luaVM, "maxLength", "guiEditSetMaxLength", "guiEditGetMaxLength");

    lua_registerclass(luaVM, "GuiEdit", "GuiElement");
}

// TODO: specials
void CLuaGUIDefs::AddGuiLabelClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateLabel");

    lua_classfunction(luaVM, "getFontHeight", "guiLabelGetFontHeight");
    lua_classfunction(luaVM, "getTextExtent", "guiLabelGetTextExtent");
    lua_classfunction(luaVM, "getColor", "guiLabelGetColor");

    lua_classfunction(luaVM, "setColor", "guiLabelSetColor");
    lua_classfunction(luaVM, "setHorizontalAlign", "guiLabelSetHorizontalAlign");
    lua_classfunction(luaVM, "setVerticalAlign", "guiLabelSetVerticalAlign");

    lua_classvariable(luaVM, "horizontalAlign", "guiLabelSetHorizontalAlign", NULL);
    lua_classvariable(luaVM, "verticalAlign", "guiLabelSetVerticalAlign", NULL);
    lua_classvariable(luaVM, "fontHeight", NULL, "guiLabelGetFontHeight");
    lua_classvariable(luaVM, "textExtent", NULL, "guiLabelGetTextExtent");

    lua_registerclass(luaVM, "GuiLabel", "GuiElement");
}

void CLuaGUIDefs::AddGuiMemoClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateMemo");

    lua_classfunction(luaVM, "getCaretIndex", "guiMemoGetCaretIndex");
    lua_classfunction(luaVM, "getVerticalScrollPosition", "guiMemoGetVerticalScrollPosition");

    lua_classfunction(luaVM, "setCaretIndex", "guiMemoSetCaretIndex");
    lua_classfunction(luaVM, "setVerticalScrollPosition", "guiMemoSetVerticalScrollPosition");
    lua_classfunction(luaVM, "setReadOnly", "guiMemoSetReadOnly");

    lua_classvariable(luaVM, "caretIndex", "guiMemoSetCaretIndex", "guiMemoGetCaretIndex");
    lua_classvariable(luaVM, "verticalScrollPosition", "guiMemoSetVerticalScrollPosition", "guiMemoGetVerticalScrollPosition");
    lua_classvariable(luaVM, "readOnly", "guiMemoSetReadOnly", "guiMemoIsReadOnly");

    lua_registerclass(luaVM, "GuiMemo", "GuiElement");
}

void CLuaGUIDefs::AddGuiImageClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateStaticImage");
    lua_classfunction(luaVM, "loadImage", "guiStaticImageLoadImage");
    lua_classfunction(luaVM, "getNativeSize", "guiStaticImageGetNativeSize");

    lua_classvariable(luaVM, "image", "guiStaticImageLoadImage", NULL);

    lua_registerclass(luaVM, "GuiStaticImage", "GuiElement");
}

void CLuaGUIDefs::AddGuiComboBoxClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateComboBox");
    lua_classfunction(luaVM, "addItem", "guiComboBoxAddItem");
    lua_classfunction(luaVM, "clear", "guiComboBoxClear");
    lua_classfunction(luaVM, "removeItem", "guiComboBoxRemoveItem");

    lua_classfunction(luaVM, "getSelected", "guiComboBoxGetSelected");
    lua_classfunction(luaVM, "getItemText", "guiComboBoxGetItemText");
    lua_classfunction(luaVM, "isOpen", "guiComboBoxIsOpen");
    lua_classfunction(luaVM, "getItemCount", "guiComboBoxGetItemCount");

    lua_classfunction(luaVM, "setItemText", "guiComboBoxSetItemText");
    lua_classfunction(luaVM, "setSelected", "guiComboBoxSetSelected");
    lua_classfunction(luaVM, "setOpen", "guiComboBoxSetOpen");

    lua_classvariable(luaVM, "selected", "guiComboBoxSetSelected", "guiComboBoxGetSelected");
    lua_classvariable(luaVM, "itemCount", nullptr, "guiComboBoxGetItemCount");
    lua_classvariable(luaVM, "open", "guiComboBoxSetOpen", "guiComboBoxIsOpen");

    lua_registerclass(luaVM, "GuiComboBox", "GuiElement");
}

void CLuaGUIDefs::AddGuiCheckBoxClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateCheckBox");

    lua_classfunction(luaVM, "getSelected", "guiCheckBoxGetSelected");

    lua_classfunction(luaVM, "setSelected", "guiCheckBoxSetSelected");

    lua_classvariable(luaVM, "selected", "guiCheckBoxSetSelected", "guiCheckBoxGetSelected");

    lua_registerclass(luaVM, "GuiCheckBox", "GuiElement");
}

void CLuaGUIDefs::AddGuiRadioButtonClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateRadioButton");

    lua_classfunction(luaVM, "getSelected", "guiRadioButtonGetSelected");

    lua_classfunction(luaVM, "setSelected", "guiRadioButtonSetSelected");

    lua_classvariable(luaVM, "selected", "guiRadioButtonSetSelected", "guiRadioButtonGetSelected");

    lua_registerclass(luaVM, "GuiRadioButton", "GuiElement");
}

void CLuaGUIDefs::AddGuiScrollPaneClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateScrollPane");

    lua_classfunction(luaVM, "getHorizontalScrollPosition", "guiScrollPaneGetHorizontalScrollPosition");
    lua_classfunction(luaVM, "getVerticalScrollPosition", "guiScrollPaneGetVerticalScrollPosition");

    lua_classfunction(luaVM, "setHorizontalScrollPosition", "guiScrollPaneSetHorizontalScrollPosition");
    lua_classfunction(luaVM, "setScrollBars", "guiScrollPaneSetScrollBars");
    lua_classfunction(luaVM, "setVerticalScrollPosition", "guiScrollPaneSetVerticalScrollPosition");

    lua_classvariable(luaVM, "horizontalScrollPosition", "guiScrollPaneSetHorizontalScrollPosition", "guiScrollPaneGetHorizontalScrollPosition");
    lua_classvariable(luaVM, "verticalScrollPosition", "guiScrollPaneSetVerticalScrollPosition", "guiScrollPaneGetVerticalScrollPosition");

    lua_registerclass(luaVM, "GuiScrollPane", "GuiElement");
}

void CLuaGUIDefs::AddGuiScrollBarClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateScrollBar");

    lua_classfunction(luaVM, "getScrollPosition", "guiScrollBarGetScrollPosition");

    lua_classfunction(luaVM, "setScrollPosition", "guiScrollBarSetScrollPosition");

    lua_classvariable(luaVM, "scrollPosition", "guiScrollBarSetScrollPosition", "guiScrollBarGetScrollPosition");

    lua_registerclass(luaVM, "GuiScrollBar", "GuiElement");
}

void CLuaGUIDefs::AddGuiProgressBarClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateProgressBar");

    lua_classfunction(luaVM, "getProgress", "guiProgressBarGetProgress");

    lua_classfunction(luaVM, "setProgress", "guiProgressBarSetProgress");

    lua_classvariable(luaVM, "progress", "guiProgressBarSetProgress", "guiProgressBarGetProgress");

    lua_registerclass(luaVM, "GuiProgressBar", "GuiElement");
}

// TODO: special tables
void CLuaGUIDefs::AddGuiGridlistClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateGridList");
    lua_classfunction(luaVM, "addColumn", "guiGridListAddColumn");
    lua_classfunction(luaVM, "addRow", "guiGridListAddRow");
    lua_classfunction(luaVM, "autoSizeColumn", "guiGridListAutoSizeColumn");
    lua_classfunction(luaVM, "clear", "guiGridListClear");
    lua_classfunction(luaVM, "insertRowAfter", "guiGridListInsertRowAfter");
    lua_classfunction(luaVM, "removeColumn", "guiGridListRemoveColumn");
    lua_classfunction(luaVM, "removeRow", "guiGridListRemoveRow");

    lua_classfunction(luaVM, "getItemData", "guiGridListGetItemData");
    lua_classfunction(luaVM, "getItemText", "guiGridListGetItemText");
    lua_classfunction(luaVM, "getRowCount", "guiGridListGetRowCount");
    lua_classfunction(luaVM, "getSelectedItem", "guiGridListGetSelectedItem");
    lua_classfunction(luaVM, "getSelectionMode", "guiGridListGetSelectionMode");
    lua_classfunction(luaVM, "isSortingEnabled", "guiGridListIsSortingEnabled");
    lua_classfunction(luaVM, "getItemColor", "guiGridListGetItemColor");
    lua_classfunction(luaVM, "getColumnTitle", "guiGridListGetColumnTitle");
    lua_classfunction(luaVM, "getHorizontalScrollPosition", "guiGridListGetHorizontalScrollPosition");
    lua_classfunction(luaVM, "getVerticalScrollPosition", "guiGridListGetVerticalScrollPosition");
    lua_classfunction(luaVM, "getSelectedCount", "guiGridListGetSelectedCount");
    lua_classfunction(luaVM, "getSelectedItems", "guiGridListGetSelectedItems");
    lua_classfunction(luaVM, "getColumnCount", "guiGridListGetColumnCount");
    lua_classfunction(luaVM, "getColumnWidth", "guiGridListGetColumnWidth");

    lua_classfunction(luaVM, "setItemData", "guiGridListSetItemData");
    lua_classfunction(luaVM, "setItemText", "guiGridListSetItemText");
    lua_classfunction(luaVM, "setScrollBars", "guiGridListSetScrollBars");
    lua_classfunction(luaVM, "setSelectedItem", "guiGridListSetSelectedItem");
    lua_classfunction(luaVM, "setSelectionMode", "guiGridListSetSelectionMode");
    lua_classfunction(luaVM, "setSortingEnabled", "guiGridListSetSortingEnabled");
    lua_classfunction(luaVM, "setColumnWidth", "guiGridListSetColumnWidth");
    lua_classfunction(luaVM, "setItemColor", "guiGridListSetItemColor");
    lua_classfunction(luaVM, "setColumnTitle", "guiGridListSetColumnTitle");
    lua_classfunction(luaVM, "setHorizontalScrollPosition", "guiGridListSetHorizontalScrollPosition");
    lua_classfunction(luaVM, "setVerticalScrollPosition", "guiGridListSetVerticalScrollPosition");

    lua_classvariable(luaVM, "rowCount", NULL, "guiGridListGetRowCount");
    lua_classvariable(luaVM, "selectedCount", NULL, "guiGridListGetSelectedCount");
    lua_classvariable(luaVM, "selectedItems", NULL, "guiGridListGetSelectedItems");
    lua_classvariable(luaVM, "columnCount", NULL, "guiGridListGetColumnCount");
    lua_classvariable(luaVM, "selectionMode", "guiGridListSetSelectionMode", "guiGridListGetSelectionMode");
    lua_classvariable(luaVM, "sortingEnabled", "guiGridListSetSortingEnabled", "guiGridListIsSortingEnabled");
    lua_classvariable(luaVM, "horizontalScrollPosition", "guiGridListSetHorizontalScrollPosition", "guiGridListGetHorizontalScrollPosition");
    lua_classvariable(luaVM, "verticalScrollPosition", "guiGridListGetVerticalScrollPosition", "guiGridListGetVerticalScrollPosition");

    lua_registerclass(luaVM, "GuiGridList", "GuiElement");
}

void CLuaGUIDefs::AddGuiTabPanelClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateTabPanel");

    lua_classfunction(luaVM, "getSelectedTab", "guiGetSelectedTab");

    lua_classfunction(luaVM, "setSelectedTab", "guiSetSelectedTab");

    lua_classvariable(luaVM, "selectedTab", "guiSetSelectedTab", "guiGetSelectedTab");

    lua_registerclass(luaVM, "GuiTabPanel", "GuiElement");
}

void CLuaGUIDefs::AddGuiTabClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateTab");
    lua_classfunction(luaVM, "delete", "guiDeleteTab");

    lua_registerclass(luaVM, "GuiTab", "GuiElement");
}

void CLuaGUIDefs::AddGuiFontClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "guiCreateFont");

    lua_registerclass(luaVM, "GuiFont", "Element");
}

int CLuaGUIDefs::GUIGetInputEnabled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, CStaticFunctionDefinitions::GUIGetInputEnabled());
    return 1;
}

int CLuaGUIDefs::GUISetInputEnabled(lua_State* luaVM)
{
    //  bool guiSetInputEnabled ( bool enabled )
    bool enabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(enabled);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetInputMode(enabled ? INPUTMODE_NO_BINDS : INPUTMODE_ALLOW_BINDS);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetInputMode(lua_State* luaVM)
{
    //  bool guiSetInputMode ( string mode )
    eInputMode mode;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(mode);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetInputMode(mode);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetInputMode(lua_State* luaVM)
{
    eInputMode mode = CStaticFunctionDefinitions::GUIGetInputMode();
    lua_pushstring(luaVM, EnumToString(mode));
    return 1;
}

int CLuaGUIDefs::GUIIsChatBoxInputActive(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pCore->IsChatInputEnabled());
    return 1;
}

int CLuaGUIDefs::GUIIsConsoleActive(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pCore->GetConsole()->IsVisible());
    return 1;
}

int CLuaGUIDefs::GUIIsDebugViewActive(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pCore->IsDebugVisible());
    return 1;
}

int CLuaGUIDefs::GUISetDebugViewActive(lua_State* luaVM)
{
    bool enabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(enabled);

    if (!argStream.HasErrors())
    {
        g_pCore->SetDebugVisible(enabled);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIIsMainMenuActive(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pCore->IsMenuVisible());
    return 1;
}

int CLuaGUIDefs::GUIIsMTAWindowActive(lua_State* luaVM)
{
    bool bActive =
        (g_pCore->IsChatInputEnabled() || g_pCore->IsMenuVisible() || g_pCore->GetConsole()->IsVisible() || g_pClientGame->GetTransferBox()->IsVisible());

    lua_pushboolean(luaVM, bActive);
    return 1;
}

int CLuaGUIDefs::GUIIsTransferBoxActive(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pClientGame->GetTransferBox()->IsVisible());
    return 1;
}

int CLuaGUIDefs::GUICreateWindow(lua_State* luaVM)
{
    //  element guiCreateWindow ( float x, float y, float width, float height, string titleBarText, bool relative )
    CVector2D position;
    CVector2D size;
    SString   titleBarText;
    bool      relative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(titleBarText);
    argStream.ReadBool(relative, false);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateWindow(*pLuaMain, position, size, titleBarText, relative);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateLabel(lua_State* luaVM)
{
    //  element guiCreateLabel ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    SString            text;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(text);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateLabel(*pLuaMain, position, size, text, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateStaticImage(lua_State* luaVM)
{
    //  element guiCreateStaticImage ( float x, float y, float width, float height, string path, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    SString            path;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(path);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            SString    strPath;
            if (CResourceManager::ParseResourcePathInput(path, pResource, &strPath))
            {
                if (FileExists(strPath))
                {
                    CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateStaticImage(*pLuaMain, position, size, strPath, relative, parent);
                    if (pGUIElement != nullptr)
                    {
                        lua_pushelement(luaVM, pGUIElement);
                        return 1;
                    }
                    else
                        argStream.SetCustomError(path, "Failed to create static image");
                }
                else
                    argStream.SetCustomError(path, "File not found");
            }
            else
                argStream.SetCustomError(path, "Bad file path");
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateButton(lua_State* luaVM)
{
    //  element guiCreateButton ( float x, float y, float width, float height, string text, bool relative, [ element parent = nil ] )
    CVector2D          position;
    CVector2D          size;
    SString            text;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(text);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateButton(*pLuaMain, position, size, text, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateProgressBar(lua_State* luaVM)
{
    //  element guiCreateProgressBar ( float x, float y, float width, float height, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateProgressBar(*pLuaMain, position, size, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateCheckBox(lua_State* luaVM)
{
    //  element guiCreateCheckBox ( float x, float y, float width, float height, string text, bool selected, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    SString            text;
    bool               selected;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(text);
    argStream.ReadBool(selected);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateCheckBox(*pLuaMain, position, size, text, selected, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateRadioButton(lua_State* luaVM)
{
    //  element guiCreateRadioButton ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    SString            text;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(text);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateRadioButton(*pLuaMain, position, size, text, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateEdit(lua_State* luaVM)
{
    //  gui-edit guiCreateEdit ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    SString            text;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(text);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateEdit(*pLuaMain, position, size, text, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateMemo(lua_State* luaVM)
{
    //  gui-memo guiCreateMemo ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    SString            text;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(text);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateMemo(*pLuaMain, position, size, text, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateGridList(lua_State* luaVM)
{
    //  element guiCreateGridList ( float x, float y, float width, float height, bool relative, [ element parent = nil ] )
    CVector2D          position;
    CVector2D          size;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateGridList(*pLuaMain, position, size, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateScrollPane(lua_State* luaVM)
{
    //  element guiCreateScrollPane( float x, float y, float width, float height, bool relative, [gui-element parent = nil])
    CVector2D          position;
    CVector2D          size;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollPane(*pLuaMain, position, size, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateScrollBar(lua_State* luaVM)
{
    //  gui-scrollbar guiCreateScrollBar ( float x, float y, float width, float height, bool horizontal, bool relative, [gui-element parent = nil])
    CVector2D          position;
    CVector2D          size;
    bool               horizontal;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadBool(horizontal);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollBar(*pLuaMain, position, size, horizontal, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateTabPanel(lua_State* luaVM)
{
    //  element guiCreateTabPanel ( float x, float y, float width, float height, bool relative, [element parent = nil ] )
    CVector2D          position;
    CVector2D          size;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTabPanel(*pLuaMain, position, size, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIStaticImageLoadImage(lua_State* luaVM)
{
    //  bool guiStaticImageLoadImage ( element theElement, string filename )
    CClientGUIElement* theElement;
    SString            filename;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(theElement);
    argStream.ReadString(filename);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            SString    strPath;
            if (CResourceManager::ParseResourcePathInput(filename, pResource, &strPath))
            {
                if (CStaticFunctionDefinitions::GUIStaticImageLoadImage(*theElement, strPath))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                else
                    argStream.SetCustomError(filename, "Error loading image");
            }
            else
                argStream.SetCustomError(filename, "Bad file path");
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIStaticImageGetNativeSize(lua_State* luaVM)
{
    //  bool guiStaticImageGetNativeSize ( element theElement, string filename )
    CClientGUIElement* theElement;
    CVector2D          vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(theElement);

    if (!argStream.HasErrors())
        if (CStaticFunctionDefinitions::GUIStaticImageGetNativeSize(*theElement, vecSize))
        {
            lua_pushnumber(luaVM, vecSize.fX);
            lua_pushnumber(luaVM, vecSize.fY);
            return 2;
        }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateTab(lua_State* luaVM)
{
    //  element guiCreateTab ( string text, element parent )
    SString            text;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(text);
    argStream.ReadUserData(parent);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTab(*pLuaMain, text, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetSelectedTab(lua_State* luaVM)
{
    //  element guiGetSelectedTab ( element tabPanel )
    CClientGUIElement* tabPanel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUITabPanel>(tabPanel);

    if (!argStream.HasErrors())
    {
        CClientGUIElement* pTab = CStaticFunctionDefinitions::GUIGetSelectedTab(*tabPanel);
        lua_pushelement(luaVM, pTab);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetSelectedTab(lua_State* luaVM)
{
    //  bool guiSetSelectedTab ( element tabPanel, element theTab )
    CClientGUIElement* tabPanel;
    CClientGUIElement* theTab;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUITabPanel>(tabPanel);
    argStream.ReadUserData<CGUITab>(theTab);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GUISetSelectedTab(*tabPanel, *theTab))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIDeleteTab(lua_State* luaVM)
{
    //  bool guiDeleteTab ( element tabToDelete, element tabPanel )
    CClientGUIElement* tabToDelete;
    CClientGUIElement* tabPanel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUITab>(tabToDelete);
    argStream.ReadUserData<CGUITabPanel>(tabPanel);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (CStaticFunctionDefinitions::GUIDeleteTab(*pLuaMain, tabToDelete, tabPanel))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetText(lua_State* luaVM)
{
    //  bool guiSetText ( element guiElement, string text )
    CClientGUIElement* guiElement;
    SString            text;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadString(text);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetText(*guiElement, text);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetFont(lua_State* luaVM)
{
    //  bool guiSetFont ( element guiElement, mixed font )
    CClientGUIElement* guiElement;
    SString            strFontName;
    CClientGuiFont*    pGuiFontElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    MixedReadGuiFontString(argStream, strFontName, "default-normal", pGuiFontElement);

    if (!argStream.HasErrors())
    {
        if (guiElement->SetFont(strFontName, pGuiFontElement))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIBringToFront(lua_State* luaVM)
{
    //  bool guiBringToFront ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GUIBringToFront(*guiElement))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIMoveToBack(lua_State* luaVM)
{
    //  bool guiMoveToBack( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIMoveToBack(*guiElement);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIRadioButtonSetSelected(lua_State* luaVM)
{
    //  bool guiRadioButtonSetSelected ( element guiRadioButton, bool state )
    CClientGUIElement* guiRadioButton;
    bool               state;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIRadioButton>(guiRadioButton);
    argStream.ReadBool(state);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIRadioButtonSetSelected(*guiRadioButton, state);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICheckBoxSetSelected(lua_State* luaVM)
{
    //  bool guiCheckBoxSetSelected ( element theCheckbox, bool state )
    CClientGUIElement* theCheckbox;
    bool               state;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUICheckBox>(theCheckbox);
    argStream.ReadBool(state);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUICheckBoxSetSelected(*theCheckbox, state);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIRadioButtonGetSelected(lua_State* luaVM)
{
    //  bool guiRadioButtonGetSelected( element guiRadioButton )
    CClientGUIElement* guiRadioButton;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIRadioButton>(guiRadioButton);

    if (!argStream.HasErrors())
    {
        bool bResult = static_cast<CGUIRadioButton*>(guiRadioButton->GetCGUIElement())->GetSelected();
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICheckBoxGetSelected(lua_State* luaVM)
{
    //  bool guiCheckBoxGetSelected ( element theCheckbox )
    CClientGUIElement* theCheckbox;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUICheckBox>(theCheckbox);

    if (!argStream.HasErrors())
    {
        bool bResult = static_cast<CGUICheckBox*>(theCheckbox->GetCGUIElement())->GetSelected();
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIProgressBarSetProgress(lua_State* luaVM)
{
    //  bool guiProgressBarSetProgress ( progressBar theProgressbar, float progress )
    CClientGUIElement* theProgressbar;
    float              progress;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIProgressBar>(theProgressbar);
    argStream.ReadNumber(progress);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIProgressBarSetProgress(*theProgressbar, static_cast<int>(progress));
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIScrollBarSetScrollPosition(lua_State* luaVM)
{
    //  bool guiScrollBarSetScrollPosition ( gui-scrollBar theScrollBar, float amount )
    CClientGUIElement* theScrollBar;
    float              amount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIScrollBar>(theScrollBar);
    argStream.ReadNumber(amount);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIScrollBarSetScrollPosition(*theScrollBar, static_cast<int>(amount));
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIScrollPaneSetHorizontalScrollPosition(lua_State* luaVM)
{
    //  bool guiScrollPaneSetHorizontalScrollPosition ( gui-scrollPane theScrollPane, float amount )
    CClientGUIElement* theScrollPane;
    float              amount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIScrollPane>(theScrollPane);
    argStream.ReadNumber(amount);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIScrollPaneSetHorizontalScrollPosition(*theScrollPane, amount);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIScrollPaneSetVerticalScrollPosition(lua_State* luaVM)
{
    //  bool guiScrollPaneSetVerticalScrollPosition ( gui-scrollPane theScrollPane, float amount )
    CClientGUIElement* theScrollPane;
    float              amount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIScrollPane>(theScrollPane);
    argStream.ReadNumber(amount);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIScrollPaneSetVerticalScrollPosition(*theScrollPane, amount);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIProgressBarGetProgress(lua_State* luaVM)
{
    //  float guiProgressBarGetProgress ( progressBar theProgressbar );
    CClientGUIElement* theProgressbar;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIProgressBar>(theProgressbar);

    if (!argStream.HasErrors())
    {
        int iProgress = (int)(static_cast<CGUIProgressBar*>(theProgressbar->GetCGUIElement())->GetProgress() * 100.0f + 0.5f);
        lua_pushnumber(luaVM, iProgress);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIScrollPaneGetHorizontalScrollPosition(lua_State* luaVM)
{
    //  float guiScrollPaneGetHorizontalScrollPosition ( gui-scrollPane theScrollPane  )
    CClientGUIElement* theScrollPane;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIScrollPane>(theScrollPane);

    if (!argStream.HasErrors())
    {
        float fPos = static_cast<CGUIScrollPane*>(theScrollPane->GetCGUIElement())->GetHorizontalScrollPosition() * 100.0f;
        lua_pushnumber(luaVM, fPos);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIScrollPaneGetVerticalScrollPosition(lua_State* luaVM)
{
    //  float guiScrollPaneGetVerticalScrollPosition ( gui-scrollPane theScrollPane  )
    CClientGUIElement* theScrollPane;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIScrollPane>(theScrollPane);

    if (!argStream.HasErrors())
    {
        float fPos = static_cast<CGUIScrollPane*>(theScrollPane->GetCGUIElement())->GetVerticalScrollPosition() * 100.0f;
        lua_pushnumber(luaVM, fPos);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIScrollBarGetScrollPosition(lua_State* luaVM)
{
    //  float guiScrollBarGetScrollPosition ( gui-scrollBar theScrollBar )
    CClientGUIElement* theScrollBar;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIScrollBar>(theScrollBar);

    if (!argStream.HasErrors())
    {
        int iPos = (int)(static_cast<CGUIScrollBar*>(theScrollBar->GetCGUIElement())->GetScrollPosition() * 100.0f);
        lua_pushnumber(luaVM, iPos);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetText(lua_State* luaVM)
{
    //  string guiGetText ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        SString strText = guiElement->GetCGUIElement()->GetText();
        lua_pushstring(luaVM, strText);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetFont(lua_State* luaVM)
{
    //  string,font guiGetFont ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        CClientGuiFont* pGuiFontElement;
        SString         strFontName = guiElement->GetFont(&pGuiFontElement);

        if (strFontName != "")
            lua_pushstring(luaVM, strFontName);
        else
            lua_pushnil(luaVM);
        lua_pushelement(luaVM, pGuiFontElement);
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetSize(lua_State* luaVM)
{
    //  float float guiGetSize ( element theElement, bool relative )
    CClientGUIElement* theElement;
    bool               relative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(theElement);
    argStream.ReadBool(relative, false);

    if (!argStream.HasErrors())
    {
        CVector2D Size;
        theElement->GetCGUIElement()->GetSize(Size, relative);

        lua_pushnumber(luaVM, Size.fX);
        lua_pushnumber(luaVM, Size.fY);
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetScreenSize(lua_State* luaVM)
{
    const CVector2D Size = CStaticFunctionDefinitions::GUIGetScreenSize();

    lua_pushnumber(luaVM, Size.fX);
    lua_pushnumber(luaVM, Size.fY);
    return 2;
}

int CLuaGUIDefs::GUIGetPosition(lua_State* luaVM)
{
    //  float, float guiGetPosition ( element guiElement, bool relative )
    CClientGUIElement* guiElement;
    bool               relative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadBool(relative, false);

    if (!argStream.HasErrors())
    {
        CVector2D Pos;
        guiElement->GetCGUIElement()->GetPosition(Pos, relative);

        lua_pushnumber(luaVM, Pos.fX);
        lua_pushnumber(luaVM, Pos.fY);
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetAlpha(lua_State* luaVM)
{
    //  bool guiSetAlpha ( element guielement, float alpha )
    CClientGUIElement* guiElement;
    float              alpha;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadNumber(alpha);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetAlpha(*guiElement, Clamp(0.0f, alpha, 1.0f));
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetAlpha(lua_State* luaVM)
{
    //  int guiGetAlpha ( element guiElement [, bool effectiveAlpha = false] )
    CClientGUIElement* guiElement;
    bool               bEffectiveAlpha;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadBool(bEffectiveAlpha, false);

    if (!argStream.HasErrors())
    {
        float fAlpha = !bEffectiveAlpha ? guiElement->GetCGUIElement()->GetAlpha() : guiElement->GetCGUIElement()->GetEffectiveAlpha();
        lua_pushnumber(luaVM, fAlpha);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetVisible(lua_State* luaVM)
{
    //  bool guiSetVisible ( element guiElement, bool state )
    CClientGUIElement* guiElement;
    bool               state;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadBool(state);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetVisible(*guiElement, state);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetEnabled(lua_State* luaVM)
{
    //  bool guiSetEnabled ( element guiElement, bool enabled )
    CClientGUIElement* guiElement;
    bool               enabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadBool(enabled);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetEnabled(*guiElement, enabled);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIBlur(lua_State* luaVM)
{
    //  bool guiBlur ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::GUIBlur(*guiElement));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIFocus(lua_State* luaVM)
{
    //  bool guiFocus ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::GUIFocus(*guiElement));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetProperty(lua_State* luaVM)
{
    //  bool guiSetProperty ( element guiElement, string property, string value )
    CClientGUIElement* guiElement;
    SString            property;
    SString            value;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadString(property);
    argStream.ReadString(value);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetProperty(*guiElement, property, value);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetVisible(lua_State* luaVM)
{
    //  bool guiGetVisible ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        bool bResult = guiElement->GetCGUIElement()->IsVisible();
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetEnabled(lua_State* luaVM)
{
    //  bool guiGetEnabled ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        bool bResult = guiElement->GetCGUIElement()->IsEnabled();
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetProperty(lua_State* luaVM)
{
    //  string guiGetProperty ( element guiElement, string property )
    CClientGUIElement* guiElement;
    SString            property;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadString(property);

    if (!argStream.HasErrors())
    {
        SString strValue = guiElement->GetCGUIElement()->GetProperty(property);
        lua_pushstring(luaVM, strValue);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetProperties(lua_State* luaVM)
{
    //  table guiGetProperties ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);

    if (!argStream.HasErrors())
    {
        // Create a new table
        lua_newtable(luaVM);

        // Add all our properties to the table on top of the given lua main's stack
        unsigned int     uiIndex = 0;
        CGUIPropertyIter iter = guiElement->GetCGUIElement()->GetPropertiesBegin();
        CGUIPropertyIter iterEnd = guiElement->GetCGUIElement()->GetPropertiesEnd();
        for (; iter != iterEnd; iter++)
        {
            const char* szKey = (*iter)->strKey;
            const char* szValue = (*iter)->strValue;

            // Add it to the table
            lua_pushstring(luaVM, szKey);
            lua_pushstring(luaVM, szValue);
            lua_settable(luaVM, -3);
        }

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetSize(lua_State* luaVM)
{
    //  bool guiSetSize ( element guiElement, float width, float height, bool relative )
    CClientGUIElement* guiElement;
    float              width;
    float              height;
    bool               relative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadNumber(width);
    argStream.ReadNumber(height);
    argStream.ReadBool(relative, false);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetSize(*guiElement, CVector2D(width, height), relative);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUISetPosition(lua_State* luaVM)
{
    //  bool guiSetPosition ( element guiElement, float x, float y, bool relative )
    CClientGUIElement* guiElement;
    float              x;
    float              y;
    bool               relative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(guiElement);
    argStream.ReadNumber(x);
    argStream.ReadNumber(y);
    argStream.ReadBool(relative, false);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUISetPosition(*guiElement, CVector2D(x, y), relative);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetSortingEnabled(lua_State* luaVM)
{
    //  bool guiGridListSetSortingEnabled ( element guiGridlist, bool enabled )
    CClientGUIElement* guiGridlist;
    bool               enabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadBool(enabled);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetSortingEnabled(*guiGridlist, enabled);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListIsSortingEnabled(lua_State* luaVM)
{
    //  bool guiGridListIsSortingEnabled ( element guiGridlist )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->IsSortingEnabled());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUIGridListAddColumn(lua_State* luaVM)
{
    //  int guiGridListAddColumn ( element gridList, string title, float width )
    CClientGUIElement* guiGridlist;
    SString            title;
    float              width;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadString(title);
    argStream.ReadNumber(width);

    if (!argStream.HasErrors())
    {
        uint id = CStaticFunctionDefinitions::GUIGridListAddColumn(*guiGridlist, title, width);
        lua_pushnumber(luaVM, id);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListRemoveColumn(lua_State* luaVM)
{
    //  bool guiGridListRemoveColumn ( element guiGridlist, int columnIndex )
    CClientGUIElement* guiGridlist;
    int                columnIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(columnIndex);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListRemoveColumn(*guiGridlist, columnIndex);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetColumnWidth(lua_State* luaVM)
{
    //  bool guiGridListSetColumnWidth ( element gridList, int columnIndex, number width, bool relative )
    CClientGUIElement* guiGridlist;
    int                columnIndex;
    float              width;
    bool               relative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(columnIndex);
    argStream.ReadNumber(width);
    argStream.ReadBool(relative, false);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetColumnWidth(*guiGridlist, columnIndex, width, relative);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetColumnWidth(lua_State* luaVM)
{
    //  float guiGridListGetColumnWidth ( element gridList, int columnIndex, bool relative )
    CClientGUIElement* pGridList;
    int                columnIndex;
    bool               relative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(pGridList);
    argStream.ReadNumber(columnIndex);
    argStream.ReadBool(relative, false);

    if (!argStream.HasErrors())
    {
        float width;
        if (static_cast<CGUIGridList*>(pGridList->GetCGUIElement())->GetColumnWidth(columnIndex, width, relative))
        {
            lua_pushnumber(luaVM, width);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetColumnTitle(lua_State* luaVM)
{
    //  bool guiGridListSetColumnTitle ( element guiGridlist, int columnIndex, string title )
    CClientGUIElement* guiGridlist;
    int                iColumnIndex;
    SString            sTitle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(iColumnIndex);
    argStream.ReadString(sTitle);

    if (!argStream.HasErrors())
    {
        int iColumnCount = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetColumnCount();
        if (iColumnIndex > 0 && iColumnCount >= iColumnIndex)
        {
            CStaticFunctionDefinitions::GUIGridListSetColumnTitle(*guiGridlist, iColumnIndex, sTitle);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetColumnTitle(lua_State* luaVM)
{
    //  string guiGridListGetColumnTitle ( element guiGridlist, int columnIndex )
    CClientGUIElement* guiGridlist;
    int                iColumnIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(iColumnIndex);

    if (!argStream.HasErrors())
    {
        int iColumnCount = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetColumnCount();
        if (iColumnIndex > 0 && iColumnCount >= iColumnIndex)
        {
            const char* szTitle = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetColumnTitle(iColumnIndex);
            lua_pushstring(luaVM, szTitle);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListAddRow(lua_State* luaVM)
{
    //  int guiGridListAddRow ( element gridList )
    //  int guiGridListAddRow ( element gridList, int/string itemText1, int/string itemText2 ... )
    CClientGUIElement* guiGridlist;
    CLuaArguments      Arguments;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadLuaArguments(Arguments);

    if (!argStream.HasErrors())
    {
        int iRet = 0;
        if (Arguments.Count() == 0)
            iRet = CStaticFunctionDefinitions::GUIGridListAddRow(*guiGridlist, true);
        else
        {
            // Vector containing our string arguments.  We add a bool to store whether it was originally a number.
            std::vector<std::pair<SString, bool>> m_items;
            for (CLuaArgument* pArgument : Arguments)
            {
                SString strItemText;
                bool    bNumber = false;

                // Check the type of the argument and convert it to a string we can process
                uint type = pArgument->GetType();
                if (type == LUA_TNUMBER)
                {
                    // Grab the lua string and its size
                    const char* szLuaString = lua_tostring(luaVM, pArgument->GetIndex());
                    size_t      sizeLuaString = lua_strlen(luaVM, pArgument->GetIndex());

                    // Set our string
                    strItemText.assign(szLuaString, sizeLuaString);
                    bNumber = true;
                }
                else if (type == LUA_TSTRING)
                    strItemText = pArgument->GetString();
                else
                    continue;

                m_items.push_back(std::make_pair(strItemText, bNumber));
            }
            iRet = CStaticFunctionDefinitions::GUIGridListAddRow(*guiGridlist, true, &m_items);
        }

        if (iRet >= 0)
        {
            m_pGUIManager->QueueGridListUpdate(guiGridlist);
            lua_pushnumber(luaVM, iRet);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListInsertRowAfter(lua_State* luaVM)
{
    //  int guiGridListInsertRowAfter ( element gridList, int rowIndex )
    //  int guiGridListInsertRowAfter ( element gridList, int rowIndex, int/string itemText1, int/string itemText2 ... )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    CLuaArguments      Arguments;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadLuaArguments(Arguments);

    if (!argStream.HasErrors())
    {
        int iRet = 0;
        if (Arguments.Count() == 0)
            iRet = CStaticFunctionDefinitions::GUIGridListInsertRowAfter(*guiGridlist, rowIndex);
        else
        {
            // Vector containing our string arguments.  We add a bool to store whether it was originally a number.
            std::vector<std::pair<SString, bool> >     m_items;
            for (CLuaArgument* pArgument : Arguments)
            {
                SString strItemText;
                bool    bNumber = false;

                // Check the type of the argument and convert it to a string we can process
                uint type = pArgument->GetType();
                if (type == LUA_TNUMBER)
                {
                    // Grab the lua string and its size
                    const char* szLuaString = lua_tostring(luaVM, pArgument->GetIndex());
                    size_t      sizeLuaString = lua_strlen(luaVM, pArgument->GetIndex());

                    // Set our string
                    strItemText.assign(szLuaString, sizeLuaString);
                    bNumber = true;
                }
                else if (type == LUA_TSTRING)
                    strItemText = pArgument->GetString();
                else
                    continue;

                m_items.push_back(std::make_pair(strItemText, bNumber));
            }
            iRet = CStaticFunctionDefinitions::GUIGridListInsertRowAfter(*guiGridlist, rowIndex, &m_items);
        }

        if (iRet >= 0)
        {
            lua_pushnumber(luaVM, iRet);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListAutoSizeColumn(lua_State* luaVM)
{
    //  bool guiGridListAutoSizeColumn ( element gridList, int columnIndex )
    CClientGUIElement* guiGridlist;
    int                columnIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(columnIndex);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListAutoSizeColumn(*guiGridlist, columnIndex);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListClear(lua_State* luaVM)
{
    //  bool guiGridListClear ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListClear(*guiGridlist);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetSelectionMode(lua_State* luaVM)
{
    //  bool guiGridListSetSelectionMode ( guiElement gridlist, int mode )
    CClientGUIElement* guiGridlist;
    int                mode;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(mode);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetSelectionMode(*guiGridlist, mode);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetSelectionMode(lua_State* luaVM)
{
    //  int guiGridListGetSelectionMode ( guiElement gridlist )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetSelectionMode());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetSelectedItem(lua_State* luaVM)
{
    //  int, int guiGridListGetSelectedItem ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        int iRow = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetSelectedItemRow();
        int iColumn = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetSelectedItemColumn();
        lua_pushnumber(luaVM, iRow);
        lua_pushnumber(luaVM, iColumn + 1);            // columns start at 1
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetSelectedItems(lua_State* luaVM)
{
    //  table guiGridListGetSelectedItems ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        CGUIGridList* pList = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement());
        CGUIListItem* pItem = NULL;

        lua_newtable(luaVM);

        for (int i = 1; i <= pList->GetSelectedCount(); i++)
        {
            pItem = pList->GetNextSelectedItem(pItem);
            if (!pItem)
                break;

            lua_pushnumber(luaVM, i);
            lua_newtable(luaVM);

            // column
            lua_pushstring(luaVM, "column");
            lua_pushnumber(luaVM, pList->GetItemColumnIndex(pItem));
            lua_settable(luaVM, -3);

            // row
            lua_pushstring(luaVM, "row");
            lua_pushnumber(luaVM, pList->GetItemRowIndex(pItem));
            lua_settable(luaVM, -3);

            // push to main table
            lua_settable(luaVM, -3);
        }

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetSelectedCount(lua_State* luaVM)
{
    //  int guiGridListGetSelectedCount ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        int iCount = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetSelectedCount();
        lua_pushnumber(luaVM, iCount);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetSelectedItem(lua_State* luaVM)
{
    //  bool guiGridListSetSelectedItem ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    int                columnIndex;
    bool               bReset;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadNumber(columnIndex);
    argStream.ReadBool(bReset, true);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetSelectedItem(*guiGridlist, rowIndex, columnIndex, bReset);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListRemoveRow(lua_State* luaVM)
{
    //  bool guiGridListRemoveRow ( element gridList, int rowIndex )
    CClientGUIElement* guiGridlist;
    int                rowIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListRemoveRow(*guiGridlist, rowIndex);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetItemText(lua_State* luaVM)
{
    //  string guiGridListGetItemText ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    int                columnIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadNumber(columnIndex);

    if (!argStream.HasErrors())
    {
        const char* szText = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetItemText(rowIndex, columnIndex);
        lua_pushstring(luaVM, szText);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetItemData(lua_State* luaVM)
{
    //  string guiGridListGetItemData ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    int                columnIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadNumber(columnIndex);

    if (!argStream.HasErrors())
    {
        void*         pData = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetItemData(rowIndex, columnIndex);
        CLuaArgument* pVariable = reinterpret_cast<CLuaArgument*>(pData);
        if (pVariable)
            pVariable->Push(luaVM);
        else
            lua_pushnil(luaVM);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetItemColor(lua_State* luaVM)
{
    //  int int int int guiGridListGetItemColor ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    int                columnIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadNumber(columnIndex);

    if (!argStream.HasErrors())
    {
        unsigned char ucRed = 255, ucGreen = 255, ucBlue = 255, ucAlpha = 255;
        if (static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetItemColor(rowIndex, columnIndex, ucRed, ucGreen, ucBlue, ucAlpha))
        {
            lua_pushnumber(luaVM, ucRed);
            lua_pushnumber(luaVM, ucGreen);
            lua_pushnumber(luaVM, ucBlue);
            lua_pushnumber(luaVM, ucAlpha);
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetHorizontalScrollPosition(lua_State* luaVM)
{
    //  float guiGridListGetHorizontalScrollPosition ( element guiGridlist )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        float fPosition = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetHorizontalScrollPosition() * 100.0f;
        lua_pushnumber(luaVM, fPosition);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetVerticalScrollPosition(lua_State* luaVM)
{
    //  float guiGridListGetVerticalScrollPosition ( element guiGridlist )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        float fPosition = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetVerticalScrollPosition() * 100.0f;
        lua_pushnumber(luaVM, fPosition);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetItemText(lua_State* luaVM)
{
    //  bool guiGridListSetItemText ( element gridList, int rowIndex, int columnIndex, string text, bool section, bool number )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    int                columnIndex;
    SString            text;
    bool               section;
    bool               number;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadNumber(columnIndex);
    argStream.ReadString(text);
    argStream.ReadBool(section);
    argStream.ReadBool(number);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetItemText(*guiGridlist, rowIndex, columnIndex, text, section, number, true);
        m_pGUIManager->QueueGridListUpdate(guiGridlist);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetItemData(lua_State* luaVM)
{
    //  bool guiGridListSetItemData ( element gridList, int rowIndex, int columnIndex, string data )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    int                columnIndex;
    CLuaArgument       data;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadNumber(columnIndex);
    argStream.ReadLuaArgument(data);

    if (!argStream.HasErrors())
    {
        CLuaArgument* pData = new CLuaArgument(data);
        CStaticFunctionDefinitions::GUIGridListSetItemData(*guiGridlist, rowIndex, columnIndex, pData);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetItemColor(lua_State* luaVM)
{
    //  bool guiGridListSetItemColor ( element gridList, int rowIndex, int columnIndex, int red, int green, int blue[, int alpha = 255 ] )
    CClientGUIElement* guiGridlist;
    int                rowIndex;
    int                columnIndex;
    int                red;
    int                green;
    int                blue;
    int                alpha;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(rowIndex);
    argStream.ReadNumber(columnIndex);
    argStream.ReadNumber(red);
    argStream.ReadNumber(green);
    argStream.ReadNumber(blue);
    argStream.ReadNumber(alpha, 255);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetItemColor(*guiGridlist, rowIndex, columnIndex, red, green, blue, alpha);
        m_pGUIManager->QueueGridListUpdate(guiGridlist);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetScrollBars(lua_State* luaVM)
{
    //  bool guiGridListSetScrollBars ( element guiGridlist, bool horizontalBar, bool verticalBar )
    CClientGUIElement* guiGridlist;
    bool               horizontalBar;
    bool               verticalBar;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadBool(horizontalBar);
    argStream.ReadBool(verticalBar);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetScrollBars(*guiGridlist, horizontalBar, verticalBar);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetHorizontalScrollPosition(lua_State* luaVM)
{
    //  bool guiGridListSetHorizontalScrollPosition ( element guiGridlist, float fPosition )
    CClientGUIElement* guiGridlist;
    float              fPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(fPosition);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetHorizontalScrollPosition(*guiGridlist, fPosition);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListSetVerticalScrollPosition(lua_State* luaVM)
{
    //  bool guiGridListSetVerticalScrollPosition ( element guiGridlist, float fPosition )
    CClientGUIElement* guiGridlist;
    float              fPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);
    argStream.ReadNumber(fPosition);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIGridListSetVerticalScrollPosition(*guiGridlist, fPosition);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIScrollPaneSetScrollBars(lua_State* luaVM)
{
    //  bool guiScrollPaneSetScrollBars ( element scrollPane, bool horizontal, bool vertical )
    CClientGUIElement* scrollPane;
    bool               horizontalBar;
    bool               verticalBar;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIScrollPane>(scrollPane);
    argStream.ReadBool(horizontalBar);
    argStream.ReadBool(verticalBar);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIScrollPaneSetScrollBars(*scrollPane, horizontalBar, verticalBar);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetRowCount(lua_State* luaVM)
{
    //  int guiGridListGetRowCount ( element theList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        int iRowCount = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetRowCount();
        lua_pushnumber(luaVM, iRowCount);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGridListGetColumnCount(lua_State* luaVM)
{
    //  int guiGridListGetColumnCount ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIGridList>(guiGridlist);

    if (!argStream.HasErrors())
    {
        int iColumnCount = static_cast<CGUIGridList*>(guiGridlist->GetCGUIElement())->GetColumnCount();
        lua_pushnumber(luaVM, iColumnCount);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIEditSetReadOnly(lua_State* luaVM)
{
    //  bool guiEditSetReadOnly ( element editField, bool status )
    CClientGUIElement* editField;
    bool               status;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(editField);
    argStream.ReadBool(status);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIEditSetReadOnly(*editField, status);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIEditIsReadOnly(lua_State* luaVM)
{
    // bool guiEditIsReadOnly( element editField )
    CClientGUIElement* editField;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(editField);

    if (!argStream.HasErrors())
    {
        bool readOnly = static_cast<CGUIEdit*>(editField->GetCGUIElement())->IsReadOnly();
        lua_pushboolean(luaVM, readOnly);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUIMemoSetReadOnly(lua_State* luaVM)
{
    //  bool guiMemoSetReadOnly ( gui-memo theMemo, bool status )
    CClientGUIElement* theMemo;
    bool               status;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIMemo>(theMemo);
    argStream.ReadBool(status);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIMemoSetReadOnly(*theMemo, status);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIMemoIsReadOnly(lua_State* luaVM)
{
    // bool guiMemoIsReadOnly( gui-memo theMemo )
    CClientGUIElement* theMemo;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIMemo>(theMemo);

    if (!argStream.HasErrors())
    {
        bool readOnly = static_cast<CGUIMemo*>(theMemo->GetCGUIElement())->IsReadOnly();
        lua_pushboolean(luaVM, readOnly);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUIEditSetMasked(lua_State* luaVM)
{
    //  bool guiEditSetMasked ( element theElement, bool status )
    CClientGUIElement* theElement;
    bool               status;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(theElement);
    argStream.ReadBool(status);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIEditSetMasked(*theElement, status);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIEditIsMasked(lua_State* luaVM)
{
    // bool guiEditIsMasked(element theElement)
    CClientGUIElement* theElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(theElement);

    if (!argStream.HasErrors())
    {
        bool masked = static_cast<CGUIEdit*>(theElement->GetCGUIElement())->IsMasked();
        lua_pushboolean(luaVM, masked);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUIEditSetMaxLength(lua_State* luaVM)
{
    //  bool guiEditSetMaxLength ( element theElement, int length )
    CClientGUIElement* theElement;
    int                length;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(theElement);
    argStream.ReadNumber(length);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIEditSetMaxLength(*theElement, length);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIEditGetMaxLength(lua_State* luaVM)
{
    // int guiEditGetMaxLength(element theElement)
    CClientGUIElement* theElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(theElement);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, static_cast<CGUIEdit*>(theElement->GetCGUIElement())->GetMaxLength());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIEditSetCaretIndex(lua_State* luaVM)
{
    //  bool guiEditSetCaretIndex ( element theElement, int index )
    CClientGUIElement* theElement;
    int                index;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(theElement);
    argStream.ReadNumber(index);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIEditSetCaretIndex(*theElement, index);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIEditGetCaretIndex(lua_State* luaVM)
{
    //  int guiEditGetCaretIndex ( element theElement )
    CClientGUIElement* theElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIEdit>(theElement);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, static_cast<CGUIEdit*>(theElement->GetCGUIElement())->GetCaretIndex());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIMemoSetCaretIndex(lua_State* luaVM)
{
    //  bool guiMemoSetCaretIndex ( gui-memo theMemo, int index )
    CClientGUIElement* theMemo;
    int                index;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIMemo>(theMemo);
    argStream.ReadNumber(index);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIMemoSetCaretIndex(*theMemo, index);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIMemoSetVerticalScrollPosition(lua_State* luaVM)
{
    //  bool guiMemoSetVerticalScrollPosition ( gui-memo theMemo, float fPosition )
    CClientGUIElement* theMemo;
    float              fPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIMemo>(theMemo);
    argStream.ReadNumber(fPosition);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIMemoSetVerticalScrollPosition(*theMemo, fPosition);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIMemoGetCaretIndex(lua_State* luaVM)
{
    //  bool guiMemoGetCaretIndex ( gui-memo theMemo )
    CClientGUIElement* theMemo;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIMemo>(theMemo);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, static_cast<CGUIMemo*>(theMemo->GetCGUIElement())->GetCaretIndex());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIMemoGetVerticalScrollPosition(lua_State* luaVM)
{
    //  float guiMemoGetVerticalScrollPosition ( gui-memo theMemo )
    CClientGUIElement* theMemo;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIMemo>(theMemo);

    if (!argStream.HasErrors())
    {
        CGUIMemo* guiMemo = static_cast<CGUIMemo*>(theMemo->GetCGUIElement());
        float     fPos = guiMemo->GetVerticalScrollPosition() / guiMemo->GetMaxVerticalScrollPosition() * 100.0f;
        lua_pushnumber(luaVM, fPos);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIWindowSetMovable(lua_State* luaVM)
{
    //  bool guiWindowSetMovable ( element theElement, bool status )
    CClientGUIElement* theElement;
    bool               status;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIWindow>(theElement);
    argStream.ReadBool(status);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIWindowSetMovable(*theElement, status);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIWindowIsMovable(lua_State* luaVM)
{
    // bool guiWindowIsMovable( element theElement )
    CClientGUIElement* theElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIWindow>(theElement);

    if (!argStream.HasErrors())
    {
        bool movable = static_cast<CGUIWindow*>(theElement->GetCGUIElement())->IsMovable();
        lua_pushboolean(luaVM, movable);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUIWindowSetSizable(lua_State* luaVM)
{
    //  bool guiWindowSetSizable ( element theElement, bool status )
    CClientGUIElement* theElement;
    bool               status;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIWindow>(theElement);
    argStream.ReadBool(status);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUIWindowSetSizable(*theElement, status);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIWindowIsSizable(lua_State* luaVM)
{
    // bool guiWindowIsSizable( elemen theElement )
    CClientGUIElement* theElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIWindow>(theElement);

    if (!argStream.HasErrors())
    {
        bool sizable = static_cast<CGUIWindow*>(theElement->GetCGUIElement())->IsSizingEnabled();
        lua_pushboolean(luaVM, sizable);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUILabelGetTextExtent(lua_State* luaVM)
{
    //  float guiLabelGetTextExtent ( element theLabel )
    CClientGUIElement* theLabel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUILabel>(theLabel);

    if (!argStream.HasErrors())
    {
        float fExtent = static_cast<CGUILabel*>(theLabel->GetCGUIElement())->GetTextExtent();
        lua_pushnumber(luaVM, fExtent);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUILabelGetFontHeight(lua_State* luaVM)
{
    //  float guiLabelGetFontHeight ( element theLabel )
    CClientGUIElement* theLabel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUILabel>(theLabel);

    if (!argStream.HasErrors())
    {
        float fHeight = static_cast<CGUILabel*>(theLabel->GetCGUIElement())->GetFontHeight();
        lua_pushnumber(luaVM, fHeight);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUILabelSetColor(lua_State* luaVM)
{
    //  bool guiLabelSetColor ( element theElement, int red, int green, int blue )
    CClientGUIElement* theElement;
    int                red;
    int                green;
    int                blue;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUILabel>(theElement);
    argStream.ReadNumber(red);
    argStream.ReadNumber(green);
    argStream.ReadNumber(blue);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUILabelSetColor(*theElement, red, green, blue);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUILabelGetColor(lua_State* luaVM)
{
    //  int r, int g, int b guiLabelGetColor ( element theElement )
    CClientGUIElement* theElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUILabel>(theElement);

    if (!argStream.HasErrors())
    {
        unsigned char ucRed = 255, ucGreen = 255, ucBlue = 255;
        static_cast<CGUILabel*>(theElement->GetCGUIElement())->GetTextColor(ucRed, ucGreen, ucBlue);
        lua_pushnumber(luaVM, ucRed);
        lua_pushnumber(luaVM, ucGreen);
        lua_pushnumber(luaVM, ucBlue);
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUILabelSetVerticalAlign(lua_State* luaVM)
{
    //  bool guiLabelSetVerticalAlign ( element theLabel, string align )
    CClientGUIElement* theLabel;
    CGUIVerticalAlign  align;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUILabel>(theLabel);
    argStream.ReadEnumString(align);

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::GUILabelSetVerticalAlign(*theLabel, align);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUILabelSetHorizontalAlign(lua_State* luaVM)
{
    //  bool guiLabelSetHorizontalAlign ( element theLabel, string align, [ bool wordwrap = false ] )
    CClientGUIElement*  theLabel;
    CGUIHorizontalAlign align;
    bool                wordwrap;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUILabel>(theLabel);
    argStream.ReadEnumString(align);
    argStream.ReadBool(wordwrap, false);

    if (!argStream.HasErrors())
    {
        if (wordwrap)
            align = (CGUIHorizontalAlign)(align + 4);
        CStaticFunctionDefinitions::GUILabelSetHorizontalAlign(*theLabel, align);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetChatboxLayout(lua_State* luaVM)
{
    //* chat_font - Returns the chatbox font
    //* chat_lines - Returns how many lines the chatbox has
    //* chat_color - Returns the background color of the chatbox
    //* chat_text_color - Returns the chatbox text color
    //* chat_input_color - Returns the background color of the chatbox input
    //* chat_input_prefix_color - Returns the color of the input prefix text
    //* chat_input_text_color - Returns the color of the text in the chatbox input
    //* chat_scale - Returns the scale of the text in the chatbox
    //* chat_position_offset_x - Returns the position offset of the chatbox on the x axis
    //* chat_position_offset_y - Returns the position offset of the chatbox on the y axis
    //* chat_position_horizontal - Returns the horizontal position of the chatbox
    //* chat_position_vertical - Returns the vertical position of the chatbox
    //* chat_text_alignment - Returns the horizontal alignment of the chatbox text
    //* chat_width - Returns the scale of the background width
    //* chat_css_style_text - Returns whether text fades out over time
    //* chat_css_style_background - Returns whether the background fades out over time
    //* chat_line_life - Returns how long it takes for text to start fading out
    //* chat_line_fade_out - Returns how long takes for text to fade out
    //* chat_use_cegui - Returns whether CEGUI is used to render the chatbox
    //* text_scale - Returns text scale
    //* chat_text_outline - Returns whether text black/white outline is used

    CScriptArgReader  argStream(luaVM);
    CCVarsInterface*  pCVars = g_pCore->GetCVars();
    float             fNumber;
    SString           strCVarValue;
    std::stringstream ss;
    int               iR, iG, iB, iA;
    SString           strCVarArg;
    bool              bAll = argStream.NextIsNone();

    // If we are asking for all CVars, prepare a new table
    if (bAll)
        lua_newtable(luaVM);
    else
        argStream.ReadString(strCVarArg);

    if (!argStream.HasErrors())
    {
        // Loop through all CVars
        for (unsigned int i = 0; i < MAX_CHATBOX_LAYOUT_CVARS; i++)
        {
            // If we are asking for all CVars, or we can match the requested CVar with this CVar
            if (bAll || !stricmp(g_chatboxLayoutCVars[i], strCVarArg))
            {
                // Push color values into a table
                if (g_chatboxLayoutCVars[i] == "chat_color" || g_chatboxLayoutCVars[i] == "chat_text_color" || g_chatboxLayoutCVars[i] == "chat_input_color" ||
                    g_chatboxLayoutCVars[i] == "chat_input_prefix_color" || g_chatboxLayoutCVars[i] == "chat_input_text_color")
                {
                    pCVars->Get(g_chatboxLayoutCVars[i], strCVarValue);
                    if (!strCVarValue.empty())
                    {
                        ss.clear();
                        ss.str(strCVarValue);
                        ss >> iR >> iG >> iB >> iA;
                        lua_newtable(luaVM);
                        lua_pushnumber(luaVM, 1);
                        lua_pushnumber(luaVM, iR);
                        lua_settable(luaVM, -3);
                        lua_pushnumber(luaVM, 2);
                        lua_pushnumber(luaVM, iG);
                        lua_settable(luaVM, -3);
                        lua_pushnumber(luaVM, 3);
                        lua_pushnumber(luaVM, iB);
                        lua_settable(luaVM, -3);
                        lua_pushnumber(luaVM, 4);
                        lua_pushnumber(luaVM, iA);
                        lua_settable(luaVM, -3);
                    }
                }
                // Push chat scale into a table
                else if (g_chatboxLayoutCVars[i] == "chat_scale")
                {
                    pCVars->Get(g_chatboxLayoutCVars[i], strCVarValue);
                    if (!strCVarValue.empty())
                    {
                        float fX, fY;
                        ss.clear();
                        ss.str(strCVarValue);
                        ss >> fX >> fY;
                        lua_newtable(luaVM);
                        lua_pushnumber(luaVM, 1);
                        lua_pushnumber(luaVM, fX);
                        lua_settable(luaVM, -3);
                        lua_pushnumber(luaVM, 2);
                        lua_pushnumber(luaVM, fY);
                        lua_settable(luaVM, -3);
                    }
                }
                else
                {
                    pCVars->Get(g_chatboxLayoutCVars[i], fNumber);
                    if (g_chatboxLayoutCVars[i] == "chat_use_cegui")
                        lua_pushboolean(luaVM, fNumber ? true : false);
                    else
                        lua_pushnumber(luaVM, fNumber);
                }

                // If we are asking for all CVars, push this into the table with its CVar name, otherwise just stop here
                if (bAll)
                    lua_setfield(luaVM, -2, g_chatboxLayoutCVars[i]);
                else
                    return 1;
            }
        }

        // We wanted all CVars and that's done so let's stop now
        if (bAll)
            return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUICreateComboBox(lua_State* luaVM)
{
    //  element guiCreateComboBox ( float x, float y, float width, float height, string caption, bool relative, [ element parent = nil ] )
    CVector2D          position;
    CVector2D          size;
    SString            caption;
    bool               relative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadString(caption);
    argStream.ReadBool(relative, false);
    argStream.ReadUserData(parent, NULL);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateComboBox(*pLuaMain, position, size, caption, relative, parent);
            lua_pushelement(luaVM, pGUIElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxAddItem(lua_State* luaVM)
{
    // int guiComboBoxAddItem( element comboBox, string value )
    CClientGUIElement* comboBox;
    SString            value;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);
    argStream.ReadString(value);

    if (!argStream.HasErrors())
    {
        int newId = CStaticFunctionDefinitions::GUIComboBoxAddItem(*comboBox, value);
        lua_pushnumber(luaVM, newId);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxRemoveItem(lua_State* luaVM)
{
    //  bool guiComboBoxRemoveItem( element comboBox, int itemId )
    CClientGUIElement* comboBox;
    int                itemId;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);
    argStream.ReadNumber(itemId);

    if (!argStream.HasErrors())
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxRemoveItem(*comboBox, itemId);
        lua_pushboolean(luaVM, ret);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxClear(lua_State* luaVM)
{
    //  bool guiComboBoxClear ( element comboBox )
    CClientGUIElement* comboBox;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);

    if (!argStream.HasErrors())
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxClear(*comboBox);
        lua_pushboolean(luaVM, ret);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxGetSelected(lua_State* luaVM)
{
    //  int guiComboBoxGetSelected ( element comboBox )
    CClientGUIElement* comboBox;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);

    if (!argStream.HasErrors())
    {
        int selected = CStaticFunctionDefinitions::GUIComboBoxGetSelected(*comboBox);
        lua_pushnumber(luaVM, selected);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxSetSelected(lua_State* luaVM)
{
    //  bool guiComboBoxSetSelected ( element comboBox, int itemIndex )
    CClientGUIElement* comboBox;
    int                itemIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);
    argStream.ReadNumber(itemIndex);

    if (!argStream.HasErrors())
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxSetSelected(*comboBox, itemIndex);
        lua_pushboolean(luaVM, ret);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxGetItemText(lua_State* luaVM)
{
    //  string guiComboBoxGetItemText ( element comboBox, int itemId )
    CClientGUIElement* comboBox;
    int                itemId;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);
    argStream.ReadNumber(itemId);

    if (!argStream.HasErrors())
    {
        SString ret = CStaticFunctionDefinitions::GUIComboBoxGetItemText(*comboBox, itemId);
        lua_pushstring(luaVM, ret);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxSetItemText(lua_State* luaVM)
{
    //  bool guiComboBoxSetItemText ( element comboBox, int itemId, string text )
    CClientGUIElement* comboBox;
    int                itemId;
    SString            text;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);
    argStream.ReadNumber(itemId);
    argStream.ReadString(text);

    if (!argStream.HasErrors())
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxSetItemText(*comboBox, itemId, text);
        lua_pushboolean(luaVM, ret);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxGetItemCount(lua_State* luaVM)
{
    // int guiComboBoxGetItemCount( element comboBox )
    CClientGUIElement* comboBox;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);

    if (!argStream.HasErrors())
    {
        int items = CStaticFunctionDefinitions::GUIComboBoxGetItemCount(*comboBox);
        lua_pushnumber(luaVM, items);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxSetOpen(lua_State* luaVM)
{
    // bool guiComboBoxSetOpen( element comboBox, bool state)
    CClientGUIElement* comboBox;
    bool               state;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);
    argStream.ReadBool(state);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::GUIComboBoxSetOpen(*comboBox, state));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIComboBoxIsOpen(lua_State* luaVM)
{
    // bool guiComboBoxIsOpen( element comboBox )
    CClientGUIElement* comboBox;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData<CGUIComboBox>(comboBox);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::GUIComboBoxIsOpen(*comboBox));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushnil(luaVM);
    return 1;
}

int CLuaGUIDefs::GUICreateFont(lua_State* luaVM)
{
    //  element guiCreateFont( string filepath [, int size=9 ] )
    SString strFilePath;
    int     iSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strFilePath);
    argStream.ReadNumber(iSize, 9);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pParentResource = pLuaMain->GetResource();
            CResource* pFileResource = pParentResource;
            SString    strPath, strMetaPath;
            if (CResourceManager::ParseResourcePathInput(strFilePath, pFileResource, &strPath, &strMetaPath))
            {
                if (FileExists(strPath))
                {
                    SString strUniqueName = SString("%s*%s*%s", pParentResource->GetName(), pFileResource->GetName(), strMetaPath.c_str()).Replace("\\", "/");
                    CClientGuiFont* pGuiFont = g_pClientGame->GetManager()->GetRenderElementManager()->CreateGuiFont(strPath, strUniqueName, iSize);
                    if (pGuiFont)
                    {
                        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's
                        // ElementGroup? **
                        pGuiFont->SetParent(pParentResource->GetResourceDynamicEntity());
                        lua_pushelement(luaVM, pGuiFont);
                        return 1;
                    }
                    argStream.SetCustomError(strFilePath, "Error creating font");
                }
                else
                    argStream.SetCustomError(strFilePath, "File not found");
            }
            else
                argStream.SetCustomError(strFilePath, "Bad file path");
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaGUIDefs::GUIGetCursorType(lua_State* luaVM)
{
    //  string guiGetCursorType ( )
    auto eType = CStaticFunctionDefinitions::GUIGetCursorType();
    lua_pushstring(luaVM, EnumToString(eType));
    return 1;
}

bool CLuaGUIDefs::GUISetChatboxCharacterLimit(int charLimit)
{
    if (charLimit == -1)
    {
        g_pCore->ResetChatboxCharacterLimit();
        return true;
    }

    int maxCharLimit = g_pCore->GetChatboxMaxCharacterLimit();

    if (charLimit < 0 || charLimit > maxCharLimit)
        throw std::invalid_argument(SString("Character limit must be %s than, or equal to %i (got: %i)", (charLimit < 0) ? "greater" : "less",
                                            (charLimit < 0) ? 0 : maxCharLimit, charLimit));

    return g_pCore->SetChatboxCharacterLimit(charLimit);
}

int CLuaGUIDefs::GUIGetChatboxCharacterLimit()
{
    return g_pCore->GetChatboxCharacterLimit();
}
