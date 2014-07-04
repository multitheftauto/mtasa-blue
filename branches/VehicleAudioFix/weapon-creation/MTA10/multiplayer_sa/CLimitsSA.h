/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CLimitsSA.h
*  PURPOSE:     Multiplayer limits class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <multiplayer/CLimits.h>

class CLimitsSA : public CLimits
{
public:
                ~CLimitsSA () {}

    int         GetIPLFiles         () const;
    bool        SetIPLFiles         ( int );

    int         GetCollisionFiles   () const;
    bool        SetCollisionFiles   ( int );

    int         GetQuadtreeNodes    () const;
    bool        SetQuadtreeNodes    ( int );

    int         GetVehicleStructs   () const;
    bool        SetVehicleStructs   ( int );

    int         GetPolygons         () const;
    bool        SetPolygons         ( int );

    int         GetStreamingMemory  () const;
    bool        SetStreamingMemory  ( int );

    int         GetStreamingVehicles () const;
    bool        SetStreamingVehicles ( int );
};
