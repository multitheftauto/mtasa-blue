/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCamSA.cpp
*  PURPOSE:     Camera entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CEntity* CCamSA::GetTargetEntity ( void ) const
{
    CEntitySAInterface * pInterface = m_pInterface->CamTargetEntity;
    CPoolsSA* pPools = (CPoolsSA *)pGame->GetPools();
    CEntity* pReturn = NULL;

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

void CCamSA::GetDirection ( float& fHorizontal, float& fVertical )
{
    fHorizontal = m_pInterface->m_fHorizontalAngle;
    fVertical = m_pInterface->m_fVerticalAngle;
}

void CCamSA::SetDirection ( float fHorizontal, float fVertical )
{
    // Calculation @ sub 0x50F970
    m_pInterface->m_fHorizontalAngle = fHorizontal;
    m_pInterface->m_fVerticalAngle = fVertical;
}
