/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"
#include "CClientGif.h"

class CLuaGifDefs : public CLuaDefs
{
public:
    static void                                  LoadFunctions();
    static void                                  AddClass(lua_State*);
    static CClientGif*                           GifCreate(lua_State*, std::string);
    static bool                                  GifPlay(CClientGif*);
    static bool                                  GifStop(CClientGif*);
    static bool                                  GifSetProperty(lua_State*, CClientGif*, std::string, std::variant<int, std::string>, std::optional<int>);
    static std::variant<bool, std::string, int>  GifGetProperty(lua_State*, CClientGif*, std::string, std::optional<int>);
    static bool                                  GifIsPlaying(CClientGif*);
};
