/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CLimitsSA.cpp
*  PURPOSE:     Multiplayer limits class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// Most of this source code has been taken from SA Limit Adjuster
//----------------------------------------------------------
//
//   SA Limit Adjuster (LimitAdjuster.cpp)
//   Copyright 2008 Sacky
//
//----------------------------------------------------------

#include "StdInc.h"

#define INTEGER_AT(addr) *(int *)(addr)
#define FLOAT_AT(addr) *(float *)(addr)

int CLimitsSA::GetIPLFiles () const
{
    return INTEGER_AT(0x405F25);
}
bool CLimitsSA::SetIPLFiles ( int value )
{
    MemPut<unsigned char>(0x405F25, 0x68);
    MemPut<int>(0x405F26, value);
    return true;
}

int CLimitsSA::GetCollisionFiles () const
{
    return INTEGER_AT(0x411457);
}
bool CLimitsSA::SetCollisionFiles ( int value )
{
    MemPut<unsigned char>(0x411457, 0x68);
    MemPut<int>(0x411458, value);
    return true;
}

int CLimitsSA::GetQuadtreeNodes () const
{
    return INTEGER_AT(0x552C3E);
}
bool CLimitsSA::SetQuadtreeNodes ( int value )
{
    MemPut<unsigned char>(0x552C3E, 0x68);
    MemPut<int>(0x552C3F, value);
    return true;
}

int CLimitsSA::GetVehicleStructs () const
{
    return INTEGER_AT(0x5B8FE3);
}
bool CLimitsSA::SetVehicleStructs ( int value )
{
    MemPut<unsigned char>(0x5B8FE3, 0x6A);
    MemPut<unsigned char>(0x5B8FE4, static_cast<unsigned char>(value));
    return true;
}

int CLimitsSA::GetPolygons () const
{
    return INTEGER_AT(0x731F5F);
}
bool CLimitsSA::SetPolygons ( int value )
{
    MemPut<unsigned char>(0x731F5F, 0x68);
    MemPut<int>(0x731F60, value);
    return true;
}

int CLimitsSA::GetStreamingMemory () const
{
    return INTEGER_AT(0x8A5A80);
}
bool CLimitsSA::SetStreamingMemory ( int value )
{
    MemPutFast<int>(0x8A5A80, value);
    MemSet((void*)0x5B8E64, 0x90, 10);
    MemSet((void*)0x5BCD50, 0x90, 5);
    MemSet((void*)0x5BCD78, 0x90, 5);
    return true;
}

int CLimitsSA::GetStreamingVehicles () const
{
    return INTEGER_AT(0x8A5A84);
}
bool CLimitsSA::SetStreamingVehicles ( int value )
{
    MemPutFast<int>(0x8A5A84, value);
    MemPut<unsigned char>(0x611C3D, 0x83);
    MemPut<unsigned char>(0x611C3E, 0xFA);
    MemPut<unsigned char>(0x611C3F, static_cast<unsigned char>(value));
    MemSet((void*)0x5BCD9C, 0x90, 5);
	MemSet((void*)0x5B8E6E, 0x90, 10);
    return true;
}
