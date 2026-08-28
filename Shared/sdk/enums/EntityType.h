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

// Used to determine the entity type (GTA interfaces). For the corresponding element type in MTA, check ElementType.h.
namespace EntityType
{
    enum Enum
    {
        NOTHING,
        BUILDING,
        VEHICLE,
        PED,
        OBJECT,
        DUMMY,
        NOTINPOOLS
    };
}
