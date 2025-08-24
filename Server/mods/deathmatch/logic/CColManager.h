/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColManager.h
 *  PURPOSE:     Collision entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CColShape.h"
#include <list>

class CColManager
{
    friend class CColShape;

public:
    CColManager();
    ~CColManager();

    void DoHitDetection(const CVector& vecNowPosition, CElement* pElement, CColShape* pJustThis = NULL, bool bChildren = false);

    bool Exists(CColShape* pShape);
    void DeleteAll();

    std::vector<CColShape*>::const_iterator IterBegin() { return m_List.begin(); }
    std::vector<CColShape*>::const_iterator IterEnd() { return m_List.end(); }

private:
    void AddToList(CColShape* pShape) { m_List.push_back(pShape); };
    void RemoveFromList(CColShape* pShape);
    void TakeOutTheTrash();
    void DoHitDetectionForColShape(CColShape* pShape);
    void DoHitDetectionForEntity(const CVector& vecNowPosition, CElement* pEntity);
    void HandleHitDetectionResult(bool bHit, CColShape* pShape, CElement* pEntity);

    std::vector<CColShape*> m_List;
    bool                    m_bCanRemoveFromList;
    bool                    m_bIteratingList;
    std::vector<CColShape*> m_TrashCan;
};
