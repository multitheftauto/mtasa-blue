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

class CJsVector4Defs : public CJsClass<CVector4D>
{
public:
    static void AddClass();

    static CVector4D* Constructor(float x, float y, float z, float w);
};
