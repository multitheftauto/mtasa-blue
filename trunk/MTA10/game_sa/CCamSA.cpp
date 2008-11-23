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
				pReturn = (CEntity*)(pPools->GetVehicle((CVehicleSAInterface *)pInterface));
				break;
            case ENTITY_TYPE_OBJECT:
                pReturn = (CEntity*)(pPools->GetObject ((CObjectSAInterface *)pInterface));
                break;
			default:
				break;
		}
	}
	return pReturn;
}