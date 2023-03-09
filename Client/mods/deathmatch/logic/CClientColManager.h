/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColManager.h
 *  PURPOSE:     Collision entity manager class
 *
 *****************************************************************************/

#pragma once

#include "CClientCommon.h"
#include <list>

class CClientColShape;
class CClientEntity;

class CClientColManager
{
    friend class CClientColShape;

public:
    ~CClientColManager();

    void DoPulse();
    void DoHitDetection(const CVector& vecNowPosition, float fRadius, CClientEntity* pElement, CClientColShape* pJustThis = NULL, bool bChildren = false);

    bool Exists(CClientColShape* pShape);
    void DeleteAll();

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); }

protected:
    void AddToList(CClientColShape* pShape) { m_List.push_back(pShape); };
    void RemoveFromList(CClientColShape* pShape);
    void DoHitDetectionOld(const CVector& vecNowPosition, float fRadius, CClientEntity* pElement, CClientColShape* pJustThis = NULL, bool bChildren = false);
    void DoHitDetectionNew(const CVector& vecNowPosition, float fRadius, CClientEntity* pElement, CClientColShape* pJustThis = NULL, bool bChildren = false);
    void DoHitDetectionForColShape(CClientColShape* pShape);
    void DoHitDetectionForEntity(const CVector& vecNowPosition, float fRadius, CClientEntity* pEntity);
    void HandleHitDetectionResult(bool bHit, CClientColShape* pShape, CClientEntity* pEntity);

    std::vector<CClientColShape*> m_List;
};
