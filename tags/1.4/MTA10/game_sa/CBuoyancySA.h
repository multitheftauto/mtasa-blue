/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBuoyancySA.cpp
*  PURPOSE:     Buoyancy calculations
*  DEVELOPERS:  JoeBullet
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BUOYANCY
#define __CGAMESA_BUOYANCY

#include <game/CPhysical.h>
#include "CEntitySA.h"
#include <CVector.h>
#include <CMatrix_Pad.h>

// CBuoyancy::CBuoyancy(CBuoyancy* pThis)
#define FUNC_CBuoyancy__constructor                            0x6C2740
// bool CBuoyancy::ProcessBuoyancy(CBuoyancy* pThis, CEntity* pEntity, float fBuoyancy, CVector* pOutVecMoveForce, CVector* pOutVecTurnForce)
#define FUNC_CBuoyancy__ProcessBuoyancy                        0x6C3EF0
// bool CBuoyancy::CalculateForces(CBuoyancy* pThis, CEntity* pEntity, CVector* pOutVecMoveForce, CVector* pOutVecTurnForce)
#define FUNC_CBuoyancy__CalculateForces                        0x6C2750
// bool CBuoyancy::PreCalcSetup(CBuoyancy* pThis, CEntity* pEntity, float fBuoyancy)
#define FUNC_CBuoyancy__PreCalcSetup                           0x6C2B90

#define CLASS_CBuoyancy                                        0xC1C890

class CBuoyancySA
{
public:
	CVector vecPos; // 0
	CMatrixEx EntityMatrix; // 12
	uint32 pad1[4]; // 84
	float fWaterLevel; // 100
	uint32 pad2; // 104
	float fBuoyancy; // 108
	CVector vecUnk1; // 112
	CVector vecUnk2; // 124
	uint32 pad3[4]; // 136
	uint8 pad4[4]; // 152
	uint32 pad5[7]; // 156
	uint8 pad6[2]; // 184
	uint8 bProcessingBoat; // 186
	uint8 pad7; // 187
	float fTurnForceZ; // 188
	CVector vecMoveForce; // 192
	uint32 pad8; // 204
};
C_ASSERT(sizeof(CBuoyancySA) == 0xD0);

#endif