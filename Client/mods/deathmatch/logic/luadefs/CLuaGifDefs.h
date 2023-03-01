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
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static CClientGif* GifCreate(lua_State* luaVM, std::string pathOrRawdata);
    static bool        GifPlay(CClientGif* gif);
    static bool        GifStop(CClientGif* gif);

    static bool                                 GifSetProperty(lua_State* luaVM, CClientGif* gif, std::string property, std::variant<int, std::string> value, std::optional<int> frame);
    static std::variant<bool, std::string, int> GifGetProperty(lua_State* luaVM, CClientGif* gif, std::string property, std::optional<int> frame);

    static bool GifIsPlaying(CClientGif* gif);
};
