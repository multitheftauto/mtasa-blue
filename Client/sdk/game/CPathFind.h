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

#include <CVector.h>

// This class can be used to retrieve a CPathNode if required, using GetPathNode(..)
class CNodeAddress
{
public:
    short sRegion;
    short sIndex;

    CNodeAddress() { sRegion = -1; }
};

class CPathFind
{
public:
    virtual void SwitchRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable = false) = 0;
    virtual void SwitchPedRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable = false) = 0;
    // belong in CCarCtrl or CPopulation
    virtual void SetPedDensity(float fPedDensity) = 0;
    virtual void SetVehicleDensity(float fVehicleDensity) = 0;

};
