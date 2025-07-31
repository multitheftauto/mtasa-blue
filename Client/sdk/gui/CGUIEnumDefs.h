/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIEnumDefs.h
 *  PURPOSE:     Graphical User Interface module interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Message box types
enum CMessageBoxFlag
{
    MB_BUTTON_NONE = 0,
    MB_BUTTON_OK = 1,
    MB_BUTTON_CANCEL = 2,
    MB_BUTTON_YES = 4,
    MB_BUTTON_NO = 8,
    MB_ICON_INFO = 16,
    MB_ICON_QUESTION = 32,
    MB_ICON_WARNING = 64,
    MB_ICON_ERROR = 128,
};

// Input handler switcher
enum eInputChannel
{
    INPUT_CORE,
    INPUT_MOD,
    INPUT_CHANNEL_COUNT,
};

enum eInputMode
{
    INPUTMODE_ALLOW_BINDS,
    INPUTMODE_NO_BINDS,
    INPUTMODE_NO_BINDS_ON_EDIT,
};

enum eCursorType
{
    CURSORTYPE_NONE,
    CURSORTYPE_DEFAULT,
    CURSORTYPE_SIZING_NS,
    CURSORTYPE_SIZING_EW,
    CURSORTYPE_SIZING_NWSE,
    CURSORTYPE_SIZING_NESW,
    CURSORTYPE_SIZING_ESWE,
    CURSORTYPE_MOVE,
    CURSORTYPE_DRAG,
    CURSORTYPE_SEG_MOVING,
    CURSORTYPE_SEG_SIZING
};
