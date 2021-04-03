/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsVector2Defs.h
 *  PURPOSE:     Js CVector2 class construction info
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CJsVector2Defs : public CJsClass<CVector2D>
{
public:
    static void AddClass();

    static CVector2D* Constructor(float x, float y);
};
