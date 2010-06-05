/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/TaskAttackSA.cpp
*  PURPOSE:		Attack game tasks
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTaskSimpleGangDriveBySA::CTaskSimpleGangDriveBySA ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )
{
	DEBUG_TRACE("CTaskSimpleGangDriveBySA::CTaskSimpleGangDriveBySA ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )");

	this->CreateTaskInterface ( sizeof ( CTaskSimpleGangDriveBySAInterface ) );
	DWORD dwFunc = FUNC_CTaskSimpleGangDriveBy__Constructor;
	DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwTargetEntity = ( pTargetEntity ) ? ( DWORD ) pTargetEntity->GetInterface () : 0;
	_asm
	{
		mov		ecx, dwThisInterface
		push    bSeatRHS
        push    nDrivebyStyle
        push    FrequencyPercentage
        push    fAbortRange
        push    pVecTarget
        push    dwTargetEntity
		call	dwFunc
	}
}


CTaskSimpleUseGunSA::CTaskSimpleUseGunSA ( CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )
{
    DEBUG_TRACE("CTaskSimpleUseGunSA::CTaskSimpleUseGunSA ( CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )");

    this->CreateTaskInterface ( sizeof ( CTaskSimpleUseGunSAInterface ) );
    DWORD dwFunc = FUNC_CTaskSimpleUseGun__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwTargetEntity = ( pTargetEntity ) ? ( DWORD ) pTargetEntity->GetInterface () : 0;
    float fTargetX = vecTarget.fX, fTargetY = vecTarget.fY, fTargetZ = vecTarget.fZ;
    DWORD dwBurstLength = nBurstLength;
    _asm
    {
        mov     ecx, dwThisInterface
        push    bAimImmediate
        push    dwBurstLength
        push    nCommand
        push    fTargetZ
        push    fTargetY
        push    fTargetX
        push    dwTargetEntity
        call    dwFunc
    }
}


bool CTaskSimpleUseGunSA::ControlGun ( CPed * pPed, CEntity * pTargetEntity, char nCommand )
{
    bool bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGun;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwPedInterface = ( DWORD ) pPed->GetInterface ();
    DWORD dwTargetEntity = ( pTargetEntity ) ? ( DWORD ) pTargetEntity->GetInterface () : 0;
    _asm
    {
        mov     ecx, dwThisInterface
        push    nCommand
        push    dwTargetEntity
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


bool CTaskSimpleUseGunSA::ControlGunMove ( CVector2D * pMoveVec )
{
    bool bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGunMove;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    _asm
    {
        mov     ecx, dwThisInterface
        push    pMoveVec
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


void CTaskSimpleUseGunSA::Reset ( CPed *pPed, CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength )
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_Reset;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwPedInterface = ( DWORD ) pPed->GetInterface ();
    DWORD dwTargetEntity = ( pTargetEntity ) ? ( DWORD ) pTargetEntity->GetInterface () : 0;
    float fTargetX = vecTarget.fX, fTargetY = vecTarget.fY, fTargetZ = vecTarget.fZ;
    DWORD dwBurstLength = nBurstLength;    
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwBurstLength
        push    nCommand
        push    fTargetZ
        push    fTargetY
        push    fTargetX
        push    dwTargetEntity
        push    dwPedInterface
        call    dwFunc
    }
}


CTaskSimpleFightSA::CTaskSimpleFightSA ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )
{
    DEBUG_TRACE("CTaskSimpleFightSA::CTaskSimpleFightSA ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )");

    this->CreateTaskInterface ( sizeof ( CTaskSimpleFightSAInterface ) );
    DWORD dwFunc = FUNC_CTaskSimpleFight__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwTargetEntity = ( pTargetEntity ) ? ( DWORD ) pTargetEntity->GetInterface () : 0;
    _asm
    {
        mov     ecx, dwThisInterface
        push    nIdlePeriod
        push    nCommand
        push    dwTargetEntity
        call    dwFunc
    }
}