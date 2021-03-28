/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CJsDefs.h
 *  PURPOSE:     Js definitions base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CJsDefs
{
public:
    enum class EClass : uint16_t
    {
        Invalid,            // To distinguish between nullptr and actual class.
        Vector2,
        Vector3,
        Vector4,
        Matrix,
        Count,
    };
};
