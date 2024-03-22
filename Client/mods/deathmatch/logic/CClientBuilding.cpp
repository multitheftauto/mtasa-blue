/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientBuilding.cpp
 *  PURPOSE:     Buildings handling class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientBuilding::CClientBuilding(class CClientManager* pManager, ElementID ID, uint16_t usModelId, const CVector &pos, const CVector &rot, uint8_t interior)
    : ClassInit(this),
      CClientEntity(ID),
      m_pBuildingManager(pManager->GetBuildingManager()),
      m_usModelId(usModelId),
      m_vPos(pos),
      m_vRot(rot),
      m_interior(interior),
      m_pBuilding(nullptr)
{
    m_pManager = pManager;
    SetTypeName("building");
    m_pBuildingManager->AddToList(this);
    Create();
}

CClientBuilding::~CClientBuilding()
{
    m_pBuildingManager->RemoveFromList(this);
    Destroy();
}

void CClientBuilding::SetPosition(const CVector& vecPosition)
{
    if (!CClientBuildingManager::IsValidPosition(vecPosition))
        return;

    if (m_vPos == vecPosition)
        return;
    m_vPos = vecPosition;
    Recreate();
}

void CClientBuilding::SetRotationRadians(const CVector& vecRadians)
{
    if (m_vRot == vecRadians)
        return;
    m_vRot = vecRadians;
    Recreate();
}

bool CClientBuilding::SetMatrix(const CMatrix& matrix)
{
    if (!CClientBuildingManager::IsValidPosition(matrix.vPos))
        return false;

    m_vPos = matrix.vPos;

    CVector vecRotation;
    g_pMultiplayer->ConvertMatrixToEulerAngles(matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ);

    ConvertRadiansToDegreesNoWrap(vecRotation);
    vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_MINUS_ZYX, EULER_ZXY);
    ConvertDegreesToRadiansNoWrap(vecRotation);

    m_vRot = vecRotation;

    Recreate();
    return true;
}

void CClientBuilding::SetInterior(uint8_t ucInterior)
{
    if (m_interior == ucInterior)
        return;
    m_interior = ucInterior;
    Recreate();
} 

void CClientBuilding::SetModel(uint16_t model)
{
    if (CClientBuildingManager::IsValidModel(model))
    {
        m_usModelId = model;
        Recreate();
    }
}

void CClientBuilding::Create()
{
    if (m_pBuilding)
        return;

    CVector4D vRot4D;
    ConvertZXYEulersToQuaternion(m_vRot, vRot4D);

    m_pBuilding = g_pGame->GetPools()->AddBuilding(this, m_usModelId, &m_vPos, &vRot4D, m_interior);
}

void CClientBuilding::Destroy()
{
    if (m_pBuilding)
    {
        g_pGame->GetPools()->RemoveBuilding(m_pBuilding);
        m_pBuilding = nullptr;
    }
}
