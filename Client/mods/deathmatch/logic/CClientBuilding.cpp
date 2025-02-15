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

CClientBuilding::CClientBuilding(class CClientManager* pManager, ElementID ID, uint16_t usModelId, const CVector& pos, const CVector& rot, uint8_t interior)
    : ClassInit(this),
      CClientEntity(ID),
      m_pBuildingManager(pManager->GetBuildingManager()),
      m_usModelId(usModelId),
      m_vPos(pos),
      m_vRot(rot),
      m_interior(interior),
      m_pBuilding(nullptr),
      m_usesCollision(true),
      m_pHighBuilding(nullptr),
      m_pLowBuilding(nullptr)
{
    m_pManager = pManager;
    m_pModelInfo = g_pGame->GetModelInfo(usModelId);
    SetTypeName("building");
    m_pBuildingManager->AddToList(this);
    Create();
}

CClientBuilding::~CClientBuilding()
{
    m_pBuildingManager->RemoveFromList(this);
}

void CClientBuilding::Unlink()
{
    if (m_pHighBuilding)
    {
        m_pHighBuilding->SetLowLodBuilding();
    }
    if (m_pLowBuilding)
    {
        SetLowLodBuilding();
    }
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
        m_pModelInfo = g_pGame->GetModelInfo(model);
        Recreate();
    }
}

void CClientBuilding::SetUsesCollision(bool state)
{
    if (m_usesCollision == state)
        return;

    if (m_pBuilding)
        m_pBuilding->SetUsesCollision(state);

    // Remove all contacts
    for (const auto& ped : m_Contacts)
        RemoveContact(ped);

    m_usesCollision = state;
}

void CClientBuilding::Create()
{
    if (m_pBuilding)
        return;

    if (m_bBeingDeleted)
        return;

    m_pBuilding = g_pGame->GetPools()->GetBuildingsPool().AddBuilding(this, m_usModelId, &m_vPos, &m_vRot, m_interior);

    if (!m_pBuilding)
        return;

	if (m_bDoubleSidedInit)
		m_pBuilding->SetBackfaceCulled(!m_bDoubleSided);

    if (!m_usesCollision)
    {
        m_pBuilding->SetUsesCollision(m_usesCollision);
    }
    if (m_pHighBuilding)
    {
        m_pHighBuilding->GetBuildingEntity()->SetLod(m_pBuilding);
    }
}

void CClientBuilding::Destroy()
{
    if (!m_pBuilding)
        return;

    if (m_pHighBuilding && m_pHighBuilding->IsValid())
    {
        m_pHighBuilding->GetBuildingEntity()->SetLod(nullptr);
    }
    g_pGame->GetPools()->GetBuildingsPool().RemoveBuilding(m_pBuilding);
    m_pBuilding = nullptr;
}

bool CClientBuilding::SetLowLodBuilding(CClientBuilding* pLod)
{
    if (pLod)
    {
        // Remove prev LOD
        SetLowLodBuilding();

        // Unlink old high lod element
        CClientBuilding* pOveridedBuilding = pLod->GetHighLodBuilding();
        if (pOveridedBuilding && pOveridedBuilding != this)
        {
            pOveridedBuilding->SetLowLodBuilding();
        }

        // Add new LOD
        m_pLowBuilding = pLod;
        m_pBuilding->SetLod(pLod->GetBuildingEntity());

        pLod->SetHighLodBuilding(this);
    }
    else
    {
        // Remove LOD
        if (m_pLowBuilding)
        {
            m_pLowBuilding->SetHighLodBuilding();
        }
        m_pBuilding->SetLod(nullptr);
        m_pLowBuilding = nullptr;
    }
    return true;
}
