/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDeathmatchObject.h
 *  PURPOSE:     Header for deathmatch object class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientObject.h"

class CDeathmatchObject final : public CClientObject
{
    DECLARE_CLASS(CDeathmatchObject, CClientObject)
public:
#ifdef WITH_OBJECT_SYNC
    CDeathmatchObject(CClientManager* pManager, class CMovingObjectsManager* pMovingObjectsManager, class CObjectSync* pObjectSync, ElementID ID,
                      unsigned short usModel);
#else
    CDeathmatchObject(CClientManager* pManager, class CMovingObjectsManager* pMovingObjectsManager, ElementID ID, unsigned short usModel, bool bLowLod);
#endif
    ~CDeathmatchObject();

    // CClientEntity interface
    void SetPosition(const CVector& vecPosition);

    // CClientObject interface
    void SetRotationRadians(const CVector& vecRotation);
    void SetOrientation(const CVector& vecPosition, const CVector& vecRotationRadians);

    void StartMovement(const CPositionRotationAnimation& a_rMoveAnimation);
    void UpdateMovement();
    void StopMovement();

    bool IsMoving() { return m_pMoveAnimation != NULL; };

    void UpdateContactingBegin(const CVector& vecPreviousPosition, const CVector& vecPreviousRotation);
    void UpdateContacting(const CVector& vecCenterOfRotation, const CVector& vecFrameTranslation, const CVector& vecFrameRotation);

protected:
    class CMovingObjectsManager* m_pMovingObjectsManager;
#ifdef WITH_OBJECT_SYNC
    class CObjectSync* m_pObjectSync;
#endif
    void _StopMovement(bool a_bUnregister);

    CPositionRotationAnimation* m_pMoveAnimation;
};
