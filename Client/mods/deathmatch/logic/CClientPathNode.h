/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPathNode.h
 *  PURPOSE:     Path node entity class
 *
 *****************************************************************************/

class CClientPathNode;

#pragma once

#include "CClientManager.h"

class CClientPathNode final : public CClientEntity
{
    DECLARE_CLASS(CClientPathNode, CClientEntity)
    friend class CClientPathManager;

public:
    enum ePathNodeStyle
    {
        PATH_STYLE_JUMP = 0,
        PATH_STYLE_SMOOTH
    };
    CClientPathNode(CClientManager* pManager, CVector& vecPosition, CVector& vecRotation, int iTime, ElementID ID, ePathNodeStyle PathStyle = PATH_STYLE_JUMP,
                    CClientPathNode* pNextNode = NULL);
    ~CClientPathNode();

    void Unlink();

    void DoPulse();

    eClientEntityType GetType() const { return CCLIENTPATHNODE; }

    void             GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; }
    void             SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; }
    void             GetRotationRadians(CVector& vecRotation) const { vecRotation = m_vecRotation; }
    void             SetRotationRadians(const CVector& vecRotation) { m_vecRotation = vecRotation; }
    int              GetTime() { return m_iTime; }
    ePathNodeStyle   GetStyle() { return m_Style; }
    CClientPathNode* GetPreviousNode() { return m_pPreviousNode; }
    void             SetPreviousNode(CClientPathNode* pPathNode) { m_pPreviousNode = pPathNode; }
    CClientPathNode* GetNextNode() { return m_pNextNode; }
    void             SetNextNode(CClientPathNode* pPathNode) { m_pNextNode = pPathNode; }
    void             SetNextNodeID(ElementID ID) { m_NextNodeID = ID; }

    void AttachEntity(CClientEntity* pEntity) { m_List.push_back(pEntity); }
    void DetachEntity(CClientEntity* pEntity)
    {
        if (!m_List.empty())
            m_List.remove(pEntity);
    }
    bool IsEntityAttached(CClientEntity* pEntity);

    std::list<CClientEntity*>::iterator AttachedIterBegin() { return m_List.begin(); }
    std::list<CClientEntity*>::iterator AttachedIterEnd() { return m_List.end(); }

private:
    CClientPathManager* m_pPathManager;

    CVector          m_vecPosition;
    CVector          m_vecRotation;
    int              m_iTime;
    ePathNodeStyle   m_Style;
    ElementID        m_NextNodeID;
    CClientPathNode *m_pPreviousNode, *m_pNextNode;

    std::list<CClientEntity*> m_List;
};
