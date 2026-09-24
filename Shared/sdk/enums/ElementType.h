/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/ElementType.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "EntityType.h"

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
        TRAIN_TRACK,
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
        PATH_NODE,
        POINTLIGHTS,
        SEARCHLIGHT,
        SCREENSOURCE,
        RENDERTARGET,
        BROWSER,
        VECTORGRAPHIC
    };

    inline ElementType::Enum GetElementTypeFromEntityType(EntityType::Enum entityType)
    {
        switch (entityType)
        {
            case EntityType::PED:
                return ElementType::PED;
            case EntityType::VEHICLE:
                return ElementType::VEHICLE;
            case EntityType::OBJECT:
                return ElementType::OBJECT;
            case EntityType::BUILDING:
                return ElementType::BUILDING;
            default:
                return ElementType::UNKNOWN;
        }
    }
}
