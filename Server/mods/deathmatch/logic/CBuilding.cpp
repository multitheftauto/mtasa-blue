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
#include "CLogger.h"
#include "Utils.h"

extern CGame* g_pGame;

CBuilding::CBuilding(CElement* pParent, CBuildingManager* pBuildingManager) : CElement(pParent), m_pLowLodObject(NULL)
{
    // Init
    m_iType = CElement::BUILDING;
    SetTypeName("buidling");

    m_pObjectManager = pBuildingManager;
    m_usModel = 0xFFFF;
    m_ucAlpha = 255;
    m_bDoubleSided = false;
    m_bCollisionsEnabled = true;

    // Add us to the manager's list
    pBuildingManager->AddToList(this);
}

CBuilding::CBuilding(const CBuilding& Copy) : CElement(Copy.m_pParent), m_pLowLodObject(Copy.m_pLowLodObject)
{
    // Init
    m_iType = CElement::BUILDING;
    SetTypeName("buidling");

    m_pObjectManager = Copy.m_pObjectManager;
    m_usModel = Copy.m_usModel;
    m_ucAlpha = Copy.m_ucAlpha;
    m_bDoubleSided = Copy.m_bDoubleSided;
    m_vecPosition = Copy.m_vecPosition;
    m_vecRotation = Copy.m_vecRotation;
    m_bCollisionsEnabled = Copy.m_bCollisionsEnabled;

    // Add us to the manager's list
    m_pObjectManager->AddToList(this);
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
    m_pObjectManager->RemoveFromList(this);

    // Remove LowLod refs in others
    SetLowLodObject(nullptr);

    if (m_HighLodObject)
        m_HighLodObject->SetLowLodObject(nullptr);
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
        if (CBuildingManager::IsValidModel(iTemp))
        {
            // Set the building id
            m_usModel = static_cast<unsigned short>(iTemp);
        }
        else
        {
            CLogger::ErrorPrintf("Bad 'model' (%d) id specified in <building> (line %d)\n", iTemp, iLine);
            return false;
        }
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

bool CBuilding::SetLowLodObject(CBuilding* pNewLowLodObject)
{
    // Set or clear?
    if (!pNewLowLodObject)
    {
        if (m_pLowLodObject)
        {
            m_pLowLodObject->SetHighLodObject(nullptr);
            m_pLowLodObject = nullptr;
        }
        m_pLowLodObject = nullptr;
        return true;
    }
    else
    {
        // Remove any previous link
        SetLowLodObject(nullptr);

        // Make new link
        m_pLowLodObject = pNewLowLodObject;
        pNewLowLodObject->SetHighLodObject(this);
        return true;
    }
}

