/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/ElementTypes.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

namespace ElementType
{
    enum Enum
    {
        DUMMY,
        PLAYER,
        VEHICLE,
        OBJECT,
        MARKER,
        BLIP,
        PICKUP,
        RADAR_AREA,
        TEAM,
        PED,
        COLSHAPE,
        SCRIPTFILE,
        WATER,
        WEAPON,
        UNKNOWN,
        BUILDING,

        // Server only
        CONSOLE,
        DATABASE_CONNECTION,
        TRAIN_TACK,
        ROOT,

        // Client only
        CAMERA,
        PROJECTILE,
        GUI,
        DFF,
        COL,
        TXD,
        IFP,
        IMG,
        SOUND,
        DXFONT,
        GUIFONT,
        TEXTURE,
        SHADER,
        EFFECT,
        POINTLIGHTS,
        SEARCHLIGHT,
        SCREENSOURCE,
        RENDERTARGET,
        BROWSER,
        VECTORGRAPHIC
    };
}
