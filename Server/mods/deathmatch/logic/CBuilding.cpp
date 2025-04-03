/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBuilding.cpp
 *  PURPOSE:     Object entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuilding.h"
#include "CBuildingManager.h"
#include "CLogger.h"
#include "Utils.h"

extern CGame* g_pGame;

CBuilding::CBuilding(CElement* pParent, CBuildingManager* pBuildingManager) : CElement(pParent)
{
    // Init
    m_iType = CElement::BUILDING;
    SetTypeName("buidling");

    m_pBuildingManager = pBuildingManager;
    m_model = 0xFFFF;
    m_bDoubleSided = false;
    m_bCollisionsEnabled = true;
    m_pLowLodBuilding = nullptr;
    m_pHighLodBuilding = nullptr;

    // Add us to the manager's list
    pBuildingManager->AddToList(this);
}

CBuilding::CBuilding(const CBuilding& Copy) : CElement(Copy.m_pParent), m_pLowLodBuilding(Copy.m_pLowLodBuilding)
{
    // Init
    m_iType = CElement::BUILDING;
    SetTypeName("buidling");

    m_pBuildingManager = Copy.m_pBuildingManager;
    m_model = Copy.m_model;
    m_bDoubleSided = Copy.m_bDoubleSided;
    m_vecPosition = Copy.m_vecPosition;
    m_vecRotation = Copy.m_vecRotation;
    m_bCollisionsEnabled = Copy.m_bCollisionsEnabled;
    m_pHighLodBuilding = nullptr;

    // Add us to the manager's list
    m_pBuildingManager->AddToList(this);
    UpdateSpatialData();
}

CBuilding::~CBuilding()
{
    // Unlink us from manager
    Unlink();
}

CElement* CBuilding::Clone(bool* bAddEntity, CResource* pResource)
{
    return new CBuilding(*this);
}

void CBuilding::Unlink()
{
    // Remove us from the manager's list
    m_pBuildingManager->RemoveFromList(this);

    // Remove LowLod refs in others
    SetLowLodBuilding(nullptr);

    if (m_pHighLodBuilding)
        m_pHighLodBuilding->SetLowLodBuilding(nullptr);
}

bool CBuilding::ReadSpecialData(const int iLine)
{
    // Grab the "posX" data
    if (!GetCustomDataFloat("posX", m_vecPosition.fX, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posX' attribute in <building> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posY" data
    if (!GetCustomDataFloat("posY", m_vecPosition.fY, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posY' attribute in <building> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posZ" data
    if (!GetCustomDataFloat("posZ", m_vecPosition.fZ, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posZ' attribute in <building> (line %d)\n", iLine);
        return false;
    }

    // Grab the "rotX", "rotY" and "rotZ" data
    GetCustomDataFloat("rotX", m_vecRotation.fX, true);
    GetCustomDataFloat("rotY", m_vecRotation.fY, true);
    GetCustomDataFloat("rotZ", m_vecRotation.fZ, true);
    // We store radians, but load degrees
    ConvertDegreesToRadians(m_vecRotation);

    // Grab the "model" data
    int iTemp;
    if (GetCustomDataInt("model", iTemp, true))
    {
        // Valid id?
        if (!CBuildingManager::IsValidModel(iTemp))
        {
            CLogger::ErrorPrintf("Bad 'model' (%d) id specified in <building> (line %d)\n", iTemp, iLine);
            return false;
        }

        // Set the building id
        m_model = static_cast<unsigned short>(iTemp);
    }
    else
    {
        CLogger::ErrorPrintf("Bad/missing 'model' attribute in <building> (line %d)\n", iLine);
        return false;
    }

    if (GetCustomDataInt("interior", iTemp, true))
        m_ucInterior = static_cast<unsigned char>(iTemp);

    if (!GetCustomDataBool("collisions", m_bCollisionsEnabled, true))
        m_bCollisionsEnabled = true;

    return true;
}

void CBuilding::GetMatrix(CMatrix& matrix)
{
    matrix.vPos = GetPosition();
    CVector vecRotation;
    GetRotation(vecRotation);

    // Do extra calculation to change rotation order if it will make a difference
    if (vecRotation.fX != 0 && vecRotation.fY != 0)
    {
        ConvertRadiansToDegreesNoWrap(vecRotation);
        vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_ZXY, EULER_ZYX);
        ConvertDegreesToRadiansNoWrap(vecRotation);
    }
    matrix.SetRotation(vecRotation);
}

void CBuilding::SetMatrix(const CMatrix& matrix)
{
    // Set position and rotation from matrix
    SetPosition(matrix.vPos);
    CVector vecRotation = matrix.GetRotation();
    SetRotation(vecRotation);
}

const CVector& CBuilding::GetPosition()
{
    return m_vecPosition;
}

void CBuilding::SetPosition(const CVector& vecPosition)
{
    // Different position?
    if (m_vecPosition != vecPosition)
    {
        // Update our vectors
        m_vecPosition = vecPosition;
        UpdateSpatialData();
    }
}

void CBuilding::GetRotation(CVector& vecRotation)
{
    vecRotation = m_vecRotation;
}

void CBuilding::SetRotation(const CVector& vecRotation)
{
    m_vecRotation = vecRotation;
}

bool CBuilding::SetLowLodBuilding(CBuilding* pNewLowLodBuilding) noexcept
{
    // Set or clear?
    if (!pNewLowLodBuilding)
    {
        if (m_pLowLodBuilding)
        {
            m_pLowLodBuilding->SetHighLodObject(nullptr);
            m_pLowLodBuilding = nullptr;
        }
        m_pLowLodBuilding = nullptr;
        return true;
    }
    else
    {
        // Remove any previous link
        SetLowLodBuilding(nullptr);

        // Make new link
        m_pLowLodBuilding = pNewLowLodBuilding;
        pNewLowLodBuilding->SetHighLodObject(this);
        return true;
    }
}

