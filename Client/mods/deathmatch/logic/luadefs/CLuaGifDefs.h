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
    static void AddClass(lua_State*);
    static int  GifCreate(lua_State*);
    static int  GifPlay(lua_State*);
    static int  GifStop(lua_State*);
    static int  GifSetProperty(lua_State*);
    static int  GifGetProperty(lua_State*);
    static int  GifIsPlaying(lua_State*);
};
