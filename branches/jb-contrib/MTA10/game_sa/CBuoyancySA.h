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

#define CLASS_CBuyoancy                                        0xC1C890

class CBuoyancySA
{
public:
	CVector vecPos;
	CMatrix EntityMatrix;
	uint32 pad1[4];
	float fWaterLevel;
	uint32 pad2;
	CVector vecUnk1;
	CVector vecUnk2;
	uint32 pad3[4];
	uint8 pad4[4];
	uint32 pad5[7];
	uint8 pad6[2];
	uint8 bProcessingBoat;
	uint8 pad7;
	float fTurnForceZ;
	CVector vecMoveForce;
	uint32 pad8;
};
//C_ASSERT(sizeof(CBuoyancySA) == 0xD0); fix this after sleep

#endif