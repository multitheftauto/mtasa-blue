/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPathFind.h
*  PURPOSE:     Path finder interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

// CInteriorFurnitureSA Pool[128];
#define ARRAY_CInteriorFurniture__Pool             0xBAD3F8

class CInteriorFurnitureSAInterface
{
public:
    uint32 pad1[6]; // probably pos + rot
    uint8 bCanBePlacedInFrontOfWindow;
    uint8 bIsTall;
    uint8 bIsStealable;
    uint8 pad2;
};