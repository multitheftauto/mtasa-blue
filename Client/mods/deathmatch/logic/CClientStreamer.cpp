/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientStreamer.cpp
 *  PURPOSE:     Streamer class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientGame.h"
using std::list;

extern CClientGame* g_pClientGame;

void* CClientStreamer::pAddingElement = NULL;

enum class ElementCategory
{
    PLAYER,
    VEHICLE,
    OBJECT,
    OTHER
};

inline ElementCategory GetElementCategory(CClientStreamElement* e)
{
    if (IS_PLAYER(e))
        return ElementCategory::PLAYER;
    if (IS_VEHICLE(e))
        return ElementCategory::VEHICLE;
    if (IS_OBJECT(e))
        return ElementCategory::OBJECT;
    return ElementCategory::OTHER;
}

CClientStreamer::CClientStreamer(StreamerLimitReachedFunction* pLimitReachedFunc, float fMaxDistance, float fSectorSize, float fRowSize)
    : m_fSectorSize(fSectorSize), m_fRowSize(fRowSize)
{
    // Setup our distance variables
    m_fMaxDistanceExp = fMaxDistance * fMaxDistance;
    m_fMaxDistanceThreshold = (fMaxDistance + 50.0f) * (fMaxDistance + 50.0f);
    m_usDimension = 0;

    // We need the limit reached func
    assert(pLimitReachedFunc);
    m_pLimitReachedFunc = pLimitReachedFunc;

    // Create our main world sectors covering the mainland
    CVector2D size(m_fSectorSize, m_fRowSize);
    CVector2D bottomLeft(-WORLD_SIZE, -WORLD_SIZE);
    CVector2D topRight(WORLD_SIZE, WORLD_SIZE);
    CreateSectors(&m_WorldRows, size, bottomLeft, topRight);

    // Find our row and sector
    m_pRow = FindOrCreateRow(m_vecPosition);
    m_pSector = NULL;
    OnEnterSector(m_pRow->FindOrCreateSector(m_vecPosition));
}

CClientStreamer::~CClientStreamer()
{
    // Clear our mainland rows
    list<CClientStreamSectorRow*>::iterator iter = m_WorldRows.begin();
    for (; iter != m_WorldRows.end(); iter++)
    {
        delete *iter;
    }
    m_WorldRows.clear();

    // Clear our extra rows
    iter = m_ExtraRows.begin();
    for (; iter != m_ExtraRows.end(); iter++)
    {
        delete *iter;
    }
    m_ExtraRows.clear();
}

void CClientStreamer::CreateSectors(std::list<CClientStreamSectorRow*>* pList, CVector2D& vecSize, CVector2D& vecBottomLeft, CVector2D& vecTopRight)
{
    // Creates our sectors within rows, filling up our rectangle, connecting each sector and row
    CClientStreamSector *   pCurrent = NULL, *pPrevious = NULL, *pPreviousRowSector = NULL;
    CClientStreamSectorRow *pCurrentRow = NULL, *pPreviousRow = NULL;
    float                   fX = vecBottomLeft.fX, fY = vecBottomLeft.fY;

    while (fY < vecTopRight.fY)
    {
        pCurrentRow = new CClientStreamSectorRow(fY, fY + vecSize.fY, m_fSectorSize, m_fRowSize);
        pCurrentRow->m_pBottom = pPreviousRow;
        pList->push_back(pCurrentRow);

        if (pPreviousRow)
            pPreviousRow->m_pTop = pCurrentRow;

        while (fX < vecTopRight.fX)
        {
            CVector2D bottomLeft(fX, fY);
            CVector2D topRight(fX + vecSize.fX, fY + vecSize.fY);

            pPrevious = pCurrent;
            pCurrent = new CClientStreamSector(pCurrentRow, bottomLeft, topRight);
            pCurrentRow->Add(pCurrent);
            pCurrent->m_pLeft = pPrevious;

            if (pPrevious)
                pPrevious->m_pRight = pCurrent;

            if (pPreviousRowSector)
            {
                pCurrent->m_pBottom = pPreviousRowSector;
                pPreviousRowSector->m_pTop = pCurrent;
                pPreviousRowSector = pPreviousRowSector->m_pRight;
            }

            fX += vecSize.fX;
        }

        pPrevious = NULL;
        pCurrent = NULL;
        pPreviousRow = pCurrentRow;
        pPreviousRowSector = pPreviousRow->Front();
        fX = vecBottomLeft.fX;
        fY += vecSize.fY;
    }
}

void CClientStreamer::ConnectRow(CClientStreamSectorRow* pRow)
{
    float fTop, fBottom;
    pRow->GetPosition(fTop, fBottom);

    // Connect up our row
    pRow->m_pTop = FindRow(fTop + (m_fRowSize / 2.0f));
    pRow->m_pBottom = FindRow(fBottom - (m_fRowSize / 2.0f));

    // Connect the other rows to us
    if (pRow->m_pTop)
        pRow->m_pTop->m_pBottom = pRow;
    if (pRow->m_pBottom)
        pRow->m_pBottom->m_pTop = pRow;
}

void                CClientStreamer::DoPulse(CVector& vecPosition)
{
    /* Debug code
    CClientStreamSector * pSector;
    list < CClientStreamSector * > ::iterator iterSector;
    list < CClientStreamSectorRow * > ::iterator iterRow = m_WorldRows.begin ();
    for ( ; iterRow != m_WorldRows.end () ; iterRow++ )
    {
        iterSector = (*iterRow)->Begin ();
        for ( ; iterSector != (*iterRow)->End () ; iterSector++ )
        {
            pSector = *iterSector;
            if ( !pSector->m_pArea )
            {
                pSector->m_pArea = new CClientRadarArea ( g_pClientGame->GetManager (), INVALID_ELEMENT_ID );
                pSector->m_pArea->SetPosition ( pSector->m_vecBottomLeft );
                CVector2D vecSize ( pSector->m_vecTopRight.fX - pSector->m_vecBottomLeft.fX, pSector->m_vecTopRight.fY - pSector->m_vecBottomLeft.fY );
                pSector->m_pArea->SetSize ( vecSize );
                pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
            }
            pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
        }
    }
    iterRow = m_ExtraRows.begin ();
    for ( ; iterRow != m_ExtraRows.end () ; iterRow++ )
    {
        iterSector = (*iterRow)->Begin ();
        for ( ; iterSector != (*iterRow)->End () ; iterSector++ )
        {
            pSector = *iterSector;
            if ( !pSector->m_pArea )
            {
                pSector->m_pArea = new CClientRadarArea ( g_pClientGame->GetManager (), INVALID_ELEMENT_ID );
                pSector->m_pArea->SetPosition ( pSector->m_vecBottomLeft );
                CVector2D vecSize ( pSector->m_vecTopRight.fX - pSector->m_vecBottomLeft.fX, pSector->m_vecTopRight.fY - pSector->m_vecBottomLeft.fY );
                pSector->m_pArea->SetSize ( vecSize );
                pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
            }
            pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
        }
    }
    */

    bool bMovedFar = false;
    // Has our position changed?
    if (vecPosition != m_vecPosition)
    {
        bMovedFar = ((m_vecPosition - vecPosition).LengthSquared() > (50 * 50));
        m_vecPosition = vecPosition;

        // Have we changed row?
        if (!m_pRow->DoesContain(vecPosition))
        {
            m_pRow = FindOrCreateRow(vecPosition, m_pRow);
            OnEnterSector(m_pRow->FindOrCreateSector(vecPosition));
        }
        // Have we changed sector?
        else if (!m_pSector->DoesContain(vecPosition))
        {
            // Grab our new sector
            OnEnterSector(m_pRow->FindOrCreateSector(vecPosition, m_pSector));
        }
    }

    // Update distances every frame
    SetExpDistances(&m_ActiveElements);
    m_ActiveElements.sort(CompareExpDistance);

    Restream(bMovedFar);
}

void CClientStreamer::SetDimension(unsigned short usDimension)
{
    // Different dimension than before?
    if (usDimension == m_usDimension)
        return;

    // Set the new dimension
    m_usDimension = usDimension;

    const CClientStreamElement* lastOutsideElement = m_outsideCurrentDimensionElements.empty() ? nullptr : m_outsideCurrentDimensionElements.back();

    auto filterElementInRows = [this](list<CClientStreamSectorRow*>& list)
    {
        for (CClientStreamSectorRow* sectorRow : list)
        {
            for (CClientStreamSector* sector : sectorRow->GetList())
            {
                auto& elements = sector->GetElements();
                auto  iter = elements.begin();
                while (iter != sector->End())
                {
                    CClientStreamElement* element = *iter;

                    if (IsElementShouldVisibleInCurrentDimesnion(element))
                        iter++;
                    else
                    {
                        iter = elements.erase(iter);
                        m_outsideCurrentDimensionElements.push_back(element);
                        element->SetStreamSector(nullptr);

                        if (element->IsStreamedIn())
                            m_ToStreamOut.push_back(element);
                    }
                }
            }
        }
    };

    filterElementInRows(m_WorldRows);
    filterElementInRows(m_ExtraRows);

    if (!lastOutsideElement)
        return;

    auto iter = m_outsideCurrentDimensionElements.begin();

    while (*iter != lastOutsideElement)
    {
        CClientStreamElement* element = *iter;
        if (element->GetDimension() == usDimension)
        {
            iter = m_outsideCurrentDimensionElements.erase(iter);
            AddElementInSectors(element);
        }
        else
        {
            iter++;
        }
    }
}

CClientStreamSectorRow* CClientStreamer::FindOrCreateRow(CVector& vecPosition, CClientStreamSectorRow* pSurrounding)
{
    // Do we have a row to check around first?
    if (pSurrounding)
    {
        // Check the above and below rows
        if (pSurrounding->m_pTop && pSurrounding->m_pTop->DoesContain(vecPosition))
            return pSurrounding->m_pTop;
        if (pSurrounding->m_pBottom && pSurrounding->m_pBottom->DoesContain(vecPosition))
            return pSurrounding->m_pBottom;
    }

    // Search through our main world rows
    CClientStreamSectorRow*                 pRow = NULL;
    list<CClientStreamSectorRow*>::iterator iter = m_WorldRows.begin();
    for (; iter != m_WorldRows.end(); iter++)
    {
        pRow = *iter;
        if (pRow->DoesContain(vecPosition))
        {
            return pRow;
        }
    }

    // Search through our extra rows
    iter = m_ExtraRows.begin();
    for (; iter != m_ExtraRows.end(); iter++)
    {
        pRow = *iter;
        if (pRow->DoesContain(vecPosition))
        {
            return pRow;
        }
    }
    // We need a new row, align it with the others
    float fBottom = float((int)(vecPosition.fY / m_fRowSize)) * m_fRowSize;
    if (vecPosition.fY < 0.0f)
        fBottom -= m_fRowSize;
    pRow = new CClientStreamSectorRow(fBottom, fBottom + m_fRowSize, m_fSectorSize, m_fRowSize);
    ConnectRow(pRow);
    pRow->SetExtra(true);
    m_ExtraRows.push_back(pRow);
    return pRow;
}

CClientStreamSectorRow* CClientStreamer::FindRow(float fY)
{
    // Search through our main world rows
    CClientStreamSectorRow*                 pRow = NULL;
    list<CClientStreamSectorRow*>::iterator iter = m_WorldRows.begin();
    for (; iter != m_WorldRows.end(); iter++)
    {
        pRow = *iter;
        if (pRow->DoesContain(fY))
        {
            return pRow;
        }
    }

    // Search through our extra rows
    iter = m_ExtraRows.begin();
    for (; iter != m_ExtraRows.end(); iter++)
    {
        pRow = *iter;
        if (pRow->DoesContain(fY))
        {
            return pRow;
        }
    }
    return NULL;
}

void CClientStreamer::OnUpdateStreamPosition(CClientStreamElement* pElement)
{
    if (!pElement->GetStreamSector())
        return;

    CVector                 vecPosition = pElement->GetStreamPosition();
    CClientStreamSectorRow* pRow = pElement->GetStreamRow();
    CClientStreamSector*    pSector = pElement->GetStreamSector();

    // Have we changed row?
    if (!pRow->DoesContain(vecPosition))
    {
        pRow = FindOrCreateRow(vecPosition);
        pElement->SetStreamRow(pRow);
        OnElementEnterSector(pElement, pRow->FindOrCreateSector(vecPosition));
    }
    // Have we changed sector?
    else if (!pSector->DoesContain(vecPosition))
    {
        // Grab our new sector
        OnElementEnterSector(pElement, pRow->FindOrCreateSector(vecPosition, pSector));
    }
    else
    {
        // Make sure our distance is updated
        pElement->SetExpDistance(pElement->GetDistanceToBoundingBoxSquared(m_vecPosition));
    }
}

void CClientStreamer::AddElementInSectors(CClientStreamElement* pElement)
{
    assert(pAddingElement == NULL);
    pAddingElement = pElement;
    CVector                 vecPosition = pElement->GetStreamPosition();
    CClientStreamSectorRow* pRow = FindOrCreateRow(vecPosition);
    pElement->SetStreamRow(pRow);
    OnElementEnterSector(pElement, pRow->FindOrCreateSector(vecPosition));
    pAddingElement = NULL;
}

void CClientStreamer::RemoveElementFromSectors(CClientStreamElement* pElement)
{
    OnElementEnterSector(pElement, nullptr);
    m_ToStreamOut.remove(pElement);
}

bool CClientStreamer::IsElementShouldVisibleInCurrentDimesnion(CClientStreamElement* pElement)
{
    return pElement->GetDimension() == m_usDimension || pElement->IsVisibleInAllDimensions();
}

void CClientStreamer::AddElement(CClientStreamElement* pElement)
{
    if (IsElementShouldVisibleInCurrentDimesnion(pElement))
        AddElementInSectors(pElement);
    else
        m_outsideCurrentDimensionElements.push_back(pElement);
}

void CClientStreamer::RemoveElement(CClientStreamElement* pElement)
{
    if (pElement->GetStreamSector())
        RemoveElementFromSectors(pElement);
    else
        m_outsideCurrentDimensionElements.remove(pElement);
}

void CClientStreamer::SetExpDistances(list<CClientStreamElement*>* pList)
{
    // Run through our list setting distances to world center
    CClientStreamElement*                 pElement = NULL;
    list<CClientStreamElement*>::iterator iter = pList->begin();
    for (; iter != pList->end(); iter++)
    {
        pElement = *iter;
        // Set its distance ^ 2
        pElement->SetExpDistance(pElement->GetDistanceToBoundingBoxSquared(m_vecPosition));
    }
}

void CClientStreamer::AddToSortedList(list<CClientStreamElement*>* pList, CClientStreamElement* pElement)
{
    // Make sure it's exp distance is updated
    float fDistance = pElement->GetDistanceToBoundingBoxSquared(m_vecPosition);
    pElement->SetExpDistance(fDistance);

    // Don't add if already in the list
    if (ListContains(*pList, pElement))
        return;

    // Search through our list. Add it behind the first item further away than this
    CClientStreamElement*                 pTemp = NULL;
    list<CClientStreamElement*>::iterator iter = pList->begin();
    for (; iter != pList->end(); iter++)
    {
        pTemp = *iter;

        // Is it further than the one we add?
        if (pTemp->GetDistanceToBoundingBoxSquared(m_vecPosition) > fDistance)
        {
            // Add it before here
            pList->insert(iter, pElement);
            return;
        }
    }

    // We have no elements in the list, add it at the beginning
    pList->push_back(pElement);
}

bool CClientStreamer::CompareExpDistance(CClientStreamElement* p1, CClientStreamElement* p2)
{
    return p1->GetExpDistance() < p2->GetExpDistance();
}

bool CClientStreamer::IsActiveElement(CClientStreamElement* pElement)
{
    list<CClientStreamElement*>::iterator iter = m_ActiveElements.begin();
    for (; iter != m_ActiveElements.end(); iter++)
    {
        if (*iter == pElement)
        {
            return true;
        }
    }
    return false;
}

void CClientStreamer::Restream(bool bMovedFar)
{
    // Adjust stream-in/out limits (large movement allows massive updates)
    int iMaxOut = bMovedFar ? 1000 : 6;
    int iMaxIn = bMovedFar ? 1000 : 6;

    // Process pending stream-out elements
    while (!m_ToStreamOut.empty() && iMaxOut > 0)
    {
        CClientStreamElement* e = m_ToStreamOut.front();
        m_ToStreamOut.pop_front();

        // Only stream out if no references remain
        if (e->GetTotalStreamReferences() == 0)
        {
            e->InternalStreamOut();
            --iMaxOut;
        }
    }

    // Prepare temporary lists
    std::vector<CClientStreamElement*> closestOut;
    std::vector<CClientStreamElement*> furthestIn;

    closestOut.clear();
    furthestIn.clear();

    bool reachedLimit = ReachedLimit();

    // Elements are sorted: nearest -> furthest
    for (CClientStreamElement* e : m_ActiveElements)
    {
        float           dist = e->GetExpDistance();
        ElementCategory category = GetElementCategory(e);
        bool            isIn = e->IsStreamedIn();

        if (isIn)
        {
            // Extra handling for vehicles and players
            switch (category)
            {
                case ElementCategory::VEHICLE:
                {
                    auto* v = DynamicCast<CClientVehicle>(e);

                    // Skip if player in vehicle had a lightsync update
                    if (v && v->GetOccupant() && IS_PLAYER(v->GetOccupant()))
                    {
                        auto* p = DynamicCast<CClientPlayer>(v->GetOccupant());
                        if (p && p->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                            m_ToStreamOut.push_back(e);
                    }

                    // Towed vehicles are handled by the tow vehicle
                    if (v && v->GetTowedByVehicle())
                        continue;
                }
                break;

                case ElementCategory::PLAYER:
                {
                    auto* p = DynamicCast<CClientPlayer>(e);

                    // Lightsync players should not be loaded
                    if (p && p->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                        m_ToStreamOut.push_back(e);
                }
                break;

                default:
                    break;
            }

            // Too far -> stream out (use threshold to avoid flickering)
            if (dist > m_fMaxDistanceThreshold)
            {
                if (iMaxOut > 0 && e->GetTotalStreamReferences() == 0)
                {
                    e->InternalStreamOut();
                    --iMaxOut;
                }
                else
                    m_ToStreamOut.push_back(e);
            }
            else
            {
                // Still inside distance -> keep track for possible swapping
                furthestIn.push_back(e);
            }

            continue;
        }

        // Too far -> ignore
        if (dist > m_fMaxDistanceExp)
            continue;

        // Special rules for vehicles / players
        switch (category)
        {
            case ElementCategory::VEHICLE:
            {
                auto* v = DynamicCast<CClientVehicle>(e);

                // Lightsync driver -> do not stream in
                if (v && v->GetOccupant() && IS_PLAYER(v->GetOccupant()))
                {
                    auto* p = DynamicCast<CClientPlayer>(v->GetOccupant());
                    if (p && p->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                        continue;
                }

                // Towed vehicles are handled elsewhere
                if (v && v->GetTowedByVehicle())
                    continue;
            }
            break;

            case ElementCategory::PLAYER:
            {
                auto* p = DynamicCast<CClientPlayer>(e);
                if (p && p->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                    continue;
            }
            break;

            default:
                break;
        }

        // Attached element: only load if LOD does not match
        if (auto* at = DynamicCast<CClientStreamElement>(e->GetAttachedTo()))
        {
            if (!at->IsStreamedIn())
            {
                auto* o = DynamicCast<CClientObject>(e);
                auto* o_att = DynamicCast<CClientObject>(at);
                if (!o || !o_att || o->IsLowLod() == o_att->IsLowLod())
                    continue;
            }
        }

        // Reached streaming limit -> mark as candidate
        if (reachedLimit)
        {
            if ((int)closestOut.size() < iMaxIn)
                closestOut.push_back(e);
        }
        else
        {
            // Stream in now (instant if moved far)
            e->InternalStreamIn(bMovedFar);

            reachedLimit = ReachedLimit();
            if (!reachedLimit)
            {
                --iMaxIn;
                if (iMaxIn <= 0)
                    break;
            }
        }
    }

    // Swap logic when at streaming limit
    if (!reachedLimit)
        return;

    int          fi = (int)furthestIn.size() - 1;
    unsigned int co = 0;

    // Up to 10 swaps per frame
    for (int i = 0; i < 10; i++)
    {
        if (iMaxIn <= 0 || iMaxOut <= 0)
            break;
        if (fi < 0)
            break;
        if (co >= closestOut.size())
            break;

        auto* fIn = furthestIn[fi];
        auto* cOut = closestOut[co];

        // Only swap if the streamed-out item is closer
        if (cOut->GetExpDistance() >= fIn->GetExpDistance())
            break;

        // Stream out the furthest streamed-in item
        if (fIn->GetTotalStreamReferences() == 0)
        {
            fIn->InternalStreamOut();
            --iMaxOut;
        }
        m_ToStreamOut.remove(fIn);
        --fi;

        // Stream in the closest streamed-out item
        if (!ReachedLimit())
        {
            cOut->InternalStreamIn(bMovedFar);
            --iMaxIn;
            ++co;
        }
    }
}

void CClientStreamer::OnEnterSector(CClientStreamSector* pSector)
{
    CClientStreamElement* pElement = NULL;
    if (m_pSector)
    {
        // Grab the unwanted sectors
        list<CClientStreamSector*> common, uncommon;
        pSector->CompareSurroundings(m_pSector, &common, &uncommon, true);

        // Deactivate the unwanted sectors
        CClientStreamSector*                 pTempSector = NULL;
        list<CClientStreamSector*>::iterator iter = uncommon.begin();
        for (; iter != uncommon.end(); iter++)
        {
            pTempSector = *iter;
            // Make sure we dont unload our new sector
            if (pTempSector != pSector)
            {
                if (pTempSector->IsActivated())
                {
                    list<CClientStreamElement*>::iterator iter = pTempSector->Begin();
                    for (; iter != pTempSector->End(); iter++)
                    {
                        pElement = *iter;
                        if (pElement->IsStreamedIn())
                        {
                            // Add it to our streaming out list
                            m_ToStreamOut.push_back(pElement);
                        }
                    }
                    pTempSector->RemoveElements(&m_ActiveElements);
                    pTempSector->SetActivated(false);
                }
            }
        }

        // Grab the wanted sectors
        m_pSector->CompareSurroundings(pSector, &common, &uncommon, true);

        // Activate the unwanted sectors
        iter = uncommon.begin();
        for (; iter != uncommon.end(); iter++)
        {
            pTempSector = *iter;
            if (!pTempSector->IsActivated())
            {
                pTempSector->AddElements(&m_ActiveElements);
                pTempSector->SetActivated(true);
            }
        }
    }
    m_pSector = pSector;
    SetExpDistances(&m_ActiveElements);
    m_ActiveElements.sort(CompareExpDistance);
}

void CClientStreamer::OnElementEnterSector(CClientStreamElement* pElement, CClientStreamSector* pSector)
{
    CClientStreamSector* pPreviousSector = pElement->GetStreamSector();
    if (pPreviousSector)
    {
        // Skip if disconnecting
        if (g_pClientGame->IsBeingDeleted())
            return;

        // Remove the element from its old sector
        pPreviousSector->Remove(pElement);
    }
    if (pSector)
    {
        // Add the element to its new sector
        pSector->Add(pElement);

        // Is this new sector activated?
        if (pSector->IsActivated())
        {
            // Was the previous sector not active?
            if (!pPreviousSector || !pPreviousSector->IsActivated())
            {
                // Add this element to our active-elements list
                AddToSortedList(&m_ActiveElements, pElement);
            }
        }
        else
        {
            // Should we deactivate the element?
            if (pPreviousSector && pPreviousSector->IsActivated())
                m_ActiveElements.remove(pElement);

            // Should we activate this sector?
            if (pSector->IsExtra() && (m_pSector->IsMySurroundingSector(pSector) || m_pSector == pSector))
            {
                pSector->AddElements(&m_ActiveElements);
                pSector->SetActivated(true);
            }
            // If we're in a deactivated sector and streamed in, stream us out
            else if (pElement->IsStreamedIn())
            {
                m_ToStreamOut.push_back(pElement);
            }
        }
    }
    else if (pPreviousSector && pPreviousSector->IsActivated())
    {
        // The element was removed from sectors.
        // Remove it from active elements too.
        m_ActiveElements.remove(pElement);
    }
    pElement->SetStreamSector(pSector);
}

void CClientStreamer::OnElementForceStreamIn(CClientStreamElement* pElement)
{
    // Make sure we're streamed in
    pElement->InternalStreamIn(true);
}

void CClientStreamer::OnElementForceStreamOut(CClientStreamElement* pElement)
{
    // Make sure we're streamed out if need be
    if (!IsActiveElement(pElement))
    {
        m_ToStreamOut.push_back(pElement);
    }
}

void CClientStreamer::OnElementDimension(CClientStreamElement* pElement)
{
    if (IsElementShouldVisibleInCurrentDimesnion(pElement))
    {
        if (!pElement->GetStreamSector())
        {
            AddElementInSectors(pElement);
            m_outsideCurrentDimensionElements.remove(pElement);
        }
    }
    else
    {
        if (pElement->GetStreamSector())
        {
            m_outsideCurrentDimensionElements.push_back(pElement);
            RemoveElementFromSectors(pElement);

            if (pElement->IsStreamedIn())
                m_ToStreamOut.push_back(pElement);
        }
    }
}
