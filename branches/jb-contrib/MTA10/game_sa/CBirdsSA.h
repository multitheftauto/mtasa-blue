/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.h
*  PURPOSE:     Header file for vehicle base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

// sizeof = 6
#define VAR_CBirds__Pool                            0xC6A8C4

#define FUNC_CBirds__Unknown1                       0x711EC0
#define FUNC_CBirds__SetNoFlyZone                   0x711EF0
#define FUNC_CBirds__Shutdown                       0x712300
#define FUNC_CBirds__Process                        0x712330
#define FUNC_CBirds__Unknown2                       0x712810
#define FUNC_CBirds__CheckIfHitByWeapon             0x712E40

class CBirdsSAInterface
{
public:
    uint8 pad1[4];
    CVector vecUnk1;
    uint32 pad2[5];
    uint8 pad3[8];
    CVector vecUnk2;
    CVector vecUnk3;
};
C_ASSERT(sizeof(CBirdsSAInterface) == 0x44);
