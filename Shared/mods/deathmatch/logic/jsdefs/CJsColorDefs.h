/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsVector4Defs.h
 *  PURPOSE:     Js CVector4 class construction info
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CJsColorDefs : public CJsClass<SColor>
{
public:
    static void AddClass();

    static SColor* Constructor(float r, float g, float b, float a);
};
