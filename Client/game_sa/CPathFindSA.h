/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPathFindSA.h
 *  PURPOSE:     Header file for game entity path finding class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPathFind.h>

#define FUNC_SwitchRoadsOffInArea               0x452C80
#define FUNC_SwitchPedRoadsOffInArea            0x452F00

#define CLASS_CPathFind                         0x96F050

#define VAR_PedDensityMultiplier                0x8D2530
#define VAR_CarDensityMultiplier                0x8A5B20

class CPathFindSA : public CPathFind
{
public:
    void SwitchRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable = false);
    void SwitchPedRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable = false);

    // Belong elsewhere - cpopulation and ccarctrl
    void SetPedDensity(float fPedDensity);
    void SetVehicleDensity(float fVehicleDensity);
};
