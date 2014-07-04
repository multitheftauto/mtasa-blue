/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/multiplayer/CLimits.h
*  PURPOSE:     Multiplayer limits interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CLimits
{
public:
    virtual         ~CLimits            () {}

    virtual int     GetIPLFiles         () const = 0;
    virtual bool    SetIPLFiles         ( int ) = 0;

    virtual int     GetCollisionFiles   () const = 0;
    virtual bool    SetCollisionFiles   ( int ) = 0;

    virtual int     GetQuadtreeNodes    () const = 0;
    virtual bool    SetQuadtreeNodes    ( int ) = 0;

    virtual int     GetVehicleStructs   () const = 0;
    virtual bool    SetVehicleStructs   ( int ) = 0;

    virtual int     GetPolygons         () const = 0;
    virtual bool    SetPolygons         ( int ) = 0;

    virtual int     GetStreamingMemory  () const = 0;
    virtual bool    SetStreamingMemory  ( int ) = 0;

    virtual int     GetStreamingVehicles () const = 0;
    virtual bool    SetStreamingVehicles ( int ) = 0;
};
