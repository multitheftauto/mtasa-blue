/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <array>

enum CGUIType
{
    BROWSER,
    BUTTON,
    CHECKBOX,
    COMBOBOX,
    EDIT,
    ELEMENT,
    GRIDLIST,
    LABEL,
    MEMO,
    PROGRESSBAR,
    RADIOBUTTON,
    SCROLLBAR,
    SCROLLPANE,
    STATICIMAGE,
    TAB,
    TABPANEL,
    TEXTURE,
    WINDOW,
};

enum class CGUITextAlignHorizontal
{
    LEFT,
    CENTER,
    RIGHT
};

enum class CGUITextAlignVertical
{
    TOP,
    MIDDLE,
    BOTTOM
};

enum class CGUIMouseButton
{
    LEFT,
    RIGHT,
    MIDDLE
};

const std::array<CGUIMouseButton, 3> CGUIMouseButtons = {CGUIMouseButton::LEFT, CGUIMouseButton::RIGHT, CGUIMouseButton::MIDDLE};
