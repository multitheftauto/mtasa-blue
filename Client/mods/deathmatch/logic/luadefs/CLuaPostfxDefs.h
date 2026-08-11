/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPostfxDefs.h
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "luadefs/CLuaDefs.h"

class CLuaPostfxDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    static float GetPostFXValue(PostFXType type);
    static int   GetPostFXMode();
    static bool  IsPostFXEnabled(PostFXType type);
};
