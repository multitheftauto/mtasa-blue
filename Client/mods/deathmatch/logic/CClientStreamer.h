/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientStreamer.h
 *  PURPOSE:     Streamer class header
 *
 *****************************************************************************/

#pragma once

#include "CClientCommon.h"
#include <list>
class CClientStreamSector;
class CClientStreamSectorRow;
class CClientStreamElement;
typedef bool(StreamerLimitReachedFunction)();

class CClientStreamer
{
    friend class CClientStreamElement;

public:
    CClientStreamer(StreamerLimitReachedFunction* pLimitReachedFunc, float fMaxDistance, float fSectorSize, float fRowSize);
    ~CClientStreamer();

    void DoPulse(CVector& vecPosition);
    void SetDimension(unsigned short usDimension);

    static bool CompareExpDistance(CClientStreamElement* p1, CClientStreamElement* p2);

    unsigned int                               CountActiveElements() { return (unsigned int)m_ActiveElements.size(); }
    bool                                       IsActiveElement(CClientStreamElement* pElement);
    std::list<CClientStreamElement*>::iterator ActiveElementsBegin() { return m_ActiveElements.begin(); }
    std::list<CClientStreamElement*>::iterator ActiveElementsEnd() { return m_ActiveElements.end(); }

    std::uint16_t                              GetDimension() const noexcept { return m_usDimension; }

private:
    void CreateSectors(std::list<CClientStreamSectorRow*>* pList, CVector2D& vecSize, CVector2D& vecBottomLeft, CVector2D& vecTopRight);
    void ConnectSector(CClientStreamSector* pSector);
    void ConnectRow(CClientStreamSectorRow* pRow);

    CClientStreamSectorRow* FindOrCreateRow(CVector& vecPosition, CClientStreamSectorRow* pSurrounding = NULL);
    CClientStreamSectorRow* FindRow(float fY);
    void                    OnUpdateStreamPosition(CClientStreamElement* pElement);

    void AddElement(CClientStreamElement* pElement);
    void RemoveElement(CClientStreamElement* pElement);

    void SetExpDistances(std::list<CClientStreamElement*>* pList);
    void AddToSortedList(std::list<CClientStreamElement*>* pList, CClientStreamElement* pElement);

    void Restream(bool bMovedFar);
    bool ReachedLimit() { return m_pLimitReachedFunc(); }

    void OnEnterSector(CClientStreamSector* pSector);
    void OnElementEnterSector(CClientStreamElement* pElement, CClientStreamSector* pSector);
    void OnElementForceStreamIn(CClientStreamElement* pElement);
    void OnElementForceStreamOut(CClientStreamElement* pElement);
    void OnElementDimension(CClientStreamElement* pElement);

    const float                        m_fSectorSize;
    const float                        m_fRowSize;
    float                              m_fMaxDistanceExp;
    float                              m_fMaxDistanceThreshold;
    StreamerLimitReachedFunction*      m_pLimitReachedFunc;
    std::list<CClientStreamSectorRow*> m_WorldRows;
    std::list<CClientStreamSectorRow*> m_ExtraRows;
    CClientStreamSectorRow*            m_pRow;
    CClientStreamSector*               m_pSector;
    CVector                            m_vecPosition;
    unsigned short                     m_usDimension;
    std::list<CClientStreamElement*>   m_ActiveElements;
    std::list<CClientStreamElement*>   m_ToStreamOut;

    static void* pAddingElement;
};
