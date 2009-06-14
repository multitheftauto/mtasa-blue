/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CCamSA.cpp
*  PURPOSE:		Camera entity
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVector * CCamSA::GetFront()
{
	DEBUG_TRACE("CVector * CCamSA::GetFront()");
	return &this->GetInterface()->Front;
}

CVector * CCamSA::GetUp()
{
	DEBUG_TRACE("CVector * CCamSA::GetUp()");
	return &this->GetInterface()->Up;
}

CVector * CCamSA::GetSource()
{
	DEBUG_TRACE("CVector * CCamSA::GetSource()");
	CCamSAInterface * pCamInterface = this->GetInterface();
	return &pCamInterface->Source;
}

CVector	* CCamSA::GetFixedModeSource()
{
	DEBUG_TRACE("CVector * CCamSA::GetFixedModeSource()");
	CCamSAInterface * pCamInterface = this->GetInterface();
	return &pCamInterface->m_cvecCamFixedModeSource;
}

CVector	* CCamSA::GetFixedModeVector()
{
	DEBUG_TRACE("CVector * CCamSA::GetFixedModeVector()");
	return &this->GetInterface()->m_cvecCamFixedModeVector;
}


CEntity * CCamSA::GetTargetEntity ( void )
{
    CEntitySAInterface * pInterface = this->internalInterface->CamTargetEntity;
    CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
	CEntity * pReturn = NULL;

	if ( pPools && pInterface )
	{
		switch ( pInterface->nType )
		{
            case ENTITY_TYPE_PED:
                pReturn = (CEntity*)(pPools->GetPed((DWORD *)pInterface));
                break;
			case ENTITY_TYPE_VEHICLE:
				pReturn = (CEntity*)(pPools->GetVehicle((DWORD *)pInterface));
				break;
            case ENTITY_TYPE_OBJECT:
                pReturn = (CEntity*)(pPools->GetObject ((DWORD *)pInterface));
                break;
			default:
				break;
		}
	}
	return pReturn;
}

void GetMatrixForGravity ( const CVector& vecGravity, CMatrix& mat )
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if ( vecGravity.Length () > 0.0001f )
    {
        mat.vUp = -vecGravity;
        mat.vUp.Normalize ();
        if ( abs(mat.vUp.fX) > 0.0001f || abs(mat.vUp.fZ) > 0.0001f )
        {
            CVector y ( 0.0f, 1.0f, 0.0f );
            mat.vFront = vecGravity;
            mat.vFront.CrossProduct ( &y );
            mat.vFront.CrossProduct ( &vecGravity );
            mat.vFront.Normalize ();
        }
        else
        {
            mat.vFront = CVector ( 0.0f, 0.0f, vecGravity.fY );
        }
        mat.vRight = mat.vFront;
        mat.vRight.CrossProduct ( &mat.vUp );
    }
    else
    {
        // No gravity, use default axes
        mat.vRight = CVector ( 1.0f, 0.0f, 0.0f );
        mat.vFront = CVector ( 0.0f, 1.0f, 0.0f );
        mat.vUp    = CVector ( 0.0f, 0.0f, 1.0f );
    }
}

void CCamSA::AdjustToNewGravity ( const CVector* pvecOldGravity, const CVector* pvecNewGravity )
{
    CEntitySAInterface* pEntity = (CEntitySAInterface *)internalInterface->CamTargetEntity;
    if ( !pEntity )
        return;

    CMatrix matOld, matNew;
    GetMatrixForGravity ( *pvecOldGravity, matOld );
    GetMatrixForGravity ( *pvecNewGravity, matNew );
    
    CVector* pvecPosition = &pEntity->Placeable.matrix->vPos;

    matOld.Invert ();
    internalInterface->m_aTargetHistoryPos [ 0 ] = matOld * (internalInterface->m_aTargetHistoryPos [ 0 ] - *pvecPosition);
    internalInterface->m_aTargetHistoryPos [ 0 ] = matNew * internalInterface->m_aTargetHistoryPos [ 0 ] + *pvecPosition;

    internalInterface->m_aTargetHistoryPos [ 1 ] = matOld * (internalInterface->m_aTargetHistoryPos [ 1 ] - *pvecPosition);
    internalInterface->m_aTargetHistoryPos [ 1 ] = matNew * internalInterface->m_aTargetHistoryPos [ 1 ] + *pvecPosition;
}
