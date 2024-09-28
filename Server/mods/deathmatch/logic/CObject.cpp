/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CObject.cpp
 *  PURPOSE:     Object entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObject.h"
#include "CLogger.h"
#include "Utils.h"

extern CGame* g_pGame;

CObject::CObject(CElement* pParent, CObjectManager* pObjectManager, bool bIsLowLod) : CElement(pParent), m_bIsLowLod(bIsLowLod), m_pLowLodObject(NULL)
{
    // Init
    m_iType = CElement::OBJECT;
    SetTypeName("object");

    m_pObjectManager = pObjectManager;
    m_usModel = 0xFFFF;
    m_pMoveAnimation = NULL;
    m_ucAlpha = 255;
    m_vecScale = CVector(1.0f, 1.0f, 1.0f);
    m_fHealth = 1000.0f;
    m_bSyncable = true;
    m_pSyncer = NULL;
    m_bIsFrozen = false;
    m_bDoubleSided = false;
    m_bBreakable = false;
    m_bRespawnable = true;

    m_bCollisionsEnabled = true;

    // Add us to the manager's list
    pObjectManager->AddToList(this);
}

CObject::CObject(const CObject& Copy) : CElement(Copy.m_pParent), m_bIsLowLod(Copy.m_bIsLowLod), m_pLowLodObject(Copy.m_pLowLodObject)
{
    // Init
    m_iType = CElement::OBJECT;
    SetTypeName("object");

    m_pObjectManager = Copy.m_pObjectManager;
    m_usModel = Copy.m_usModel;
    m_ucAlpha = Copy.m_ucAlpha;
    m_vecScale = CVector(Copy.m_vecScale.fX, Copy.m_vecScale.fY, Copy.m_vecScale.fZ);
    m_fHealth = Copy.m_fHealth;
    m_bSyncable = Copy.m_bSyncable;
    m_pSyncer = Copy.m_pSyncer;
    m_bIsFrozen = Copy.m_bIsFrozen;
    m_bDoubleSided = Copy.m_bDoubleSided;
    m_bBreakable = Copy.m_bBreakable;
    m_vecPosition = Copy.m_vecPosition;
    m_vecRotation = Copy.m_vecRotation;
    m_bRespawnable = Copy.m_bRespawnable;

    m_pMoveAnimation = NULL;
    if (Copy.m_pMoveAnimation != NULL)
    {
        m_pMoveAnimation = new CPositionRotationAnimation(*Copy.m_pMoveAnimation);
    }

    m_bCollisionsEnabled = Copy.m_bCollisionsEnabled;

    // Add us to the manager's list
    m_pObjectManager->AddToList(this);
    UpdateSpatialData();
}

CObject::~CObject()
{
    if (m_pMoveAnimation != NULL)
    {
        delete m_pMoveAnimation;
        m_pMoveAnimation = NULL;
    }

    // Remove syncer
    SetSyncer(NULL);

    // Unlink us from manager
    Unlink();
}

CElement* CObject::Clone(bool* bAddEntity, CResource* pResource)
{
    return new CObject(*this);
}

void CObject::Unlink()
{
    // Remove us from the manager's list
    m_pObjectManager->RemoveFromList(this);

    // Remove LowLod refs in others
    SetLowLodObject(NULL);
    while (!m_HighLodObjectList.empty())
        m_HighLodObjectList[0]->SetLowLodObject(NULL);
}

bool CObject::ReadSpecialData(const int iLine)
{
    // Grab the "posX" data
    if (!GetCustomDataFloat("posX", m_vecPosition.fX, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posX' attribute in <object> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posY" data
    if (!GetCustomDataFloat("posY", m_vecPosition.fY, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posY' attribute in <object> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posZ" data
    if (!GetCustomDataFloat("posZ", m_vecPosition.fZ, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posZ' attribute in <object> (line %d)\n", iLine);
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
        if (CObjectManager::IsValidModel(iTemp))
        {
            // Set the object id
            m_usModel = static_cast<unsigned short>(iTemp);
        }
        else
        {
            CLogger::ErrorPrintf("Bad 'model' (%d) id specified in <object> (line %d)\n", iTemp, iLine);
            return false;
        }
    }
    else
    {
        CLogger::ErrorPrintf("Bad/missing 'model' attribute in <object> (line %d)\n", iLine);
        return false;
    }

    if (GetCustomDataInt("interior", iTemp, true))
        m_ucInterior = static_cast<unsigned char>(iTemp);

    if (GetCustomDataInt("dimension", iTemp, true))
    {
        if (iTemp == -1)
            m_bVisibleInAllDimensions = true;
        else
            m_usDimension = static_cast<unsigned short>(iTemp);
    }

    GetCustomDataBool("doublesided", m_bDoubleSided, true);

    if (!GetCustomDataFloat("scale", m_vecScale.fX, true))
        m_vecScale.fX = 1.0f;
    m_vecScale.fY = m_vecScale.fX;
    m_vecScale.fZ = m_vecScale.fX;
    GetCustomDataFloat("scaleX", m_vecScale.fX, true);
    GetCustomDataFloat("scaleY", m_vecScale.fY, true);
    GetCustomDataFloat("scaleZ", m_vecScale.fZ, true);

    if (!GetCustomDataBool("collisions", m_bCollisionsEnabled, true))
        m_bCollisionsEnabled = true;

    if (GetCustomDataInt("alpha", iTemp, true))
        m_ucAlpha = static_cast<unsigned char>(iTemp);

    GetCustomDataBool("frozen", m_bIsFrozen, true);

    if (!GetCustomDataBool("breakable", m_bBreakable, true))
        m_bBreakable = CObjectManager::IsBreakableModel(m_usModel);

    return true;
}

void CObject::GetMatrix(CMatrix& matrix)
{
    matrix.vPos = GetPosition();
    CVector vecRotation;
    GetRotation(vecRotation);

    // Do extra calculation to change rotation order if it will make a difference
    if (vecRotation.fX != 0 && vecRotation.fY != 0)
    {
        CElement* pAttachedToBase = this;
        while (pAttachedToBase->GetAttachedToElement())
            pAttachedToBase = pAttachedToBase->GetAttachedToElement();

        // Only change rotation order if base is an object
        if (pAttachedToBase->GetType() == CElement::OBJECT)
        {
            ConvertRadiansToDegreesNoWrap(vecRotation);
            vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_ZXY, EULER_ZYX);
            ConvertDegreesToRadiansNoWrap(vecRotation);
        }
    }
    matrix.SetRotation(vecRotation);
}

void CObject::SetMatrix(const CMatrix& matrix)
{
    // Set position and rotation from matrix
    SetPosition(matrix.vPos);
    CVector vecRotation = matrix.GetRotation();
    SetRotation(vecRotation);
}

const CVector& CObject::GetPosition()
{
    CVector vecOldPosition = m_vecPosition;

    // Are we attached to something?
    if (m_pAttachedTo)
        GetAttachedPosition(m_vecPosition);

    // Are we moving?
    else if (IsMoving())
    {
        SPositionRotation positionRotation;
        bool              bStillRunning = m_pMoveAnimation->GetValue(positionRotation);
        m_vecPosition = positionRotation.m_vecPosition;
        if (!bStillRunning)
        {
            StopMoving();
        }
    }

    if (vecOldPosition != m_vecPosition)
        UpdateSpatialData();            // This is necessary because 'GetAttachedPosition ( m_vecPosition )' can change alter this objects position
    // Finally, return it
    return m_vecPosition;
}

void CObject::SetPosition(const CVector& vecPosition)
{
    // If we're attached to something, dont change anything (attachment takes priority over moving here)
    if (m_pAttachedTo)
        return;

    // If we're moving, dont change anything
    if (IsMoving())
        return;

    // Different position?
    if (m_vecPosition != vecPosition)
    {
        // Update our vectors
        m_vecPosition = vecPosition;
        UpdateSpatialData();
    }
}

void CObject::GetRotation(CVector& vecRotation)
{
    vecRotation = m_vecRotation;

    // Are we attached to something?
    if (m_pAttachedTo)
        GetAttachedRotation(vecRotation);

    // Are we moving?
    else if (IsMoving())
    {
        SPositionRotation positionRotation;
        bool              bStillRunning = m_pMoveAnimation->GetValue(positionRotation);
        m_vecRotation = positionRotation.m_vecRotation;
        if (!bStillRunning)
        {
            StopMoving();
        }
        vecRotation = m_vecRotation;
    }
}

void CObject::SetRotation(const CVector& vecRotation)
{
    // If we're attached to something, dont change anything (attachment takes priority over moving here)
    if (m_pAttachedTo)
        return;

    // If we're moving, dont change anything
    if (IsMoving())
        return;

    // Different rotation?
    if (m_vecRotation != vecRotation)
    {
        // Set the new rotation
        m_vecRotation = vecRotation;
    }
}

bool CObject::IsMoving()
{
    // Are we currently moving?
    if (m_pMoveAnimation != NULL)
    {
        // Should we have stopped moving by now?
        if (!m_pMoveAnimation->IsRunning())
        {
            // Stop our movement
            StopMoving();
        }
    }
    // Are we still moving after the above check?
    return (m_pMoveAnimation != NULL);
}

void CObject::Move(const CPositionRotationAnimation& a_rMoveAnimation)
{
    // Are we already moving?
    if (IsMoving())
    {
        // Stop our current movement
        StopMoving();
    }

    if (a_rMoveAnimation.IsRunning())
    {
        m_pMoveAnimation = new CPositionRotationAnimation(a_rMoveAnimation);
        // Update the values since they might have changed since StopMoving () was called above
        m_pMoveAnimation->SetSourceValue(SPositionRotation(m_vecPosition, m_vecRotation));
    }
    // If we have a time of 0, move there now
    else
    {
        SPositionRotation positionRotation;
        a_rMoveAnimation.GetFinalValue(positionRotation);
        SetPosition(positionRotation.m_vecPosition);
        SetRotation(positionRotation.m_vecRotation);
    }
}

void CObject::StopMoving()
{
    // Were we moving in the first place
    if (m_pMoveAnimation != NULL)
    {
        SPositionRotation positionRotation;
        m_pMoveAnimation->GetValue(positionRotation);
        m_vecPosition = positionRotation.m_vecPosition;
        m_vecRotation = positionRotation.m_vecRotation;

        delete m_pMoveAnimation;
        m_pMoveAnimation = NULL;

        UpdateSpatialData();
    }
}

const CPositionRotationAnimation* CObject::GetMoveAnimation()
{
    if (IsMoving())            // Call IsMoving since it will make sure the anim is stopped if it's finished
    {
        return m_pMoveAnimation;
    }
    else
    {
        return NULL;
    }
}

void CObject::SetSyncer(CPlayer* pPlayer)
{
    // Prevent a recursive call loop when setting a syncer
    static bool bAlreadyIn = false;
    if (!bAlreadyIn)
    {
        // Update the last player if any
        bAlreadyIn = true;
        if (m_pSyncer)
        {
            m_pSyncer->RemoveSyncingObject(this);
        }

        // Update the new player
        if (pPlayer)
        {
            pPlayer->AddSyncingObject(this);
        }
        bAlreadyIn = false;

        // Set it
        m_pSyncer = pPlayer;
    }
}

bool CObject::IsLowLod()
{
    return m_bIsLowLod;
}

bool CObject::SetLowLodObject(CObject* pNewLowLodObject)
{
    // This object has to be high lod
    if (m_bIsLowLod)
        return false;

    // Set or clear?
    if (!pNewLowLodObject)
    {
        // Check if already clear
        if (!m_pLowLodObject)
            return false;

        // Verify link
        assert(ListContains(m_pLowLodObject->m_HighLodObjectList, this));

        // Clear there and here
        ListRemove(m_pLowLodObject->m_HighLodObjectList, this);
        m_pLowLodObject = NULL;
        return true;
    }
    else
    {
        // new object has to be low lod
        if (!pNewLowLodObject->m_bIsLowLod)
            return false;

        // Remove any previous link
        SetLowLodObject(NULL);

        // Make new link
        m_pLowLodObject = pNewLowLodObject;
        pNewLowLodObject->m_HighLodObjectList.push_back(this);
        return true;
    }
}

CObject* CObject::GetLowLodObject()
{
    if (m_bIsLowLod)
        return NULL;
    return m_pLowLodObject;
}
