/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/TaskJumpFallSA.cpp
*  PURPOSE:		Jump and fall game tasks
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTaskSimpleClimbSA::CTaskSimpleClimbSA ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )
{
	DEBUG_TRACE("CTaskSimpleClimbSA::CTaskSimpleClimbSA ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )");

	this->CreateTaskInterface ( sizeof ( CTaskSimpleClimbSAInterface ) );
	DWORD dwFunc = FUNC_CTaskSimpleClimb__Constructor;
	DWORD dwThisInterface = (DWORD)this->GetInterface();

	_asm
	{
		mov		ecx, dwThisInterface
        push	bForceClimb
        push	nHeight
        push	nSurfaceType
        push	fHeading
        push	vecTarget
        push	pClimbEnt
		call	dwFunc
	}
}


// ##############################################################################
// ## Name:    CTaskSimpleJetPack                                    
// ## Purpose: Allows the player to use a jetpack to 'fly' around
// ##############################################################################

CTaskSimpleJetPackSA::CTaskSimpleJetPackSA(const CVector *pVecTargetPos, float fCruiseHeight, int nHoverTime)
{
	DEBUG_TRACE("CTaskSimpleJetPackSA::CTaskSimpleJetPackSA(const CVector *pVecTargetPos = NULL, float fCruiseHeight = 10.0f, int nHoverTime = 0)");
	this->CreateTaskInterface(sizeof(CTaskSimpleJetPackSAInterface));
	DWORD dwFunc = FUNC_CTaskSimpleJetPack__Constructor;
	DWORD dwThisInterface = (DWORD)this->GetInterface();

	_asm
	{
		mov		ecx, dwThisInterface
		push	0				// pTargetEnt - ignored for simplicity's sake (we really don't need it)
		push	nHoverTime
		push	fCruiseHeight
		push	pVecTargetPos
		call	dwFunc
	}
}

