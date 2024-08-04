/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CClientChatboxVariables.h
 *  PURPOSE:     Header file for client chatbox variables
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <SharedUtil.Misc.h>

#define MAX_CHATBOX_LAYOUT_CVARS 21

static const SFixedArray<const char*, MAX_CHATBOX_LAYOUT_CVARS>& g_chatboxLayoutCVars = {{"chat_font",
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
