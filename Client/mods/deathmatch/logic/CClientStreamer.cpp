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
#include <cmath>
#include <algorithm>
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
    : m_fSectorSize(fSectorSize), m_fRowSize(fRowSize), m_pRow(NULL), m_pSector(NULL)
{
    // Setup our distance variables
    m_fMaxDistanceExp = fMaxDistance * fMaxDistance;
    m_fMaxDistanceThreshold = (fMaxDistance + 50.0f) * (fMaxDistance + 50.0f);
    m_usDimension = 0;

    // We need the limit reached func
    assert(pLimitReachedFunc);
    m_pLimitReachedFunc = pLimitReachedFunc;

    // Initialize position to valid default (0,0,0)
    m_vecPosition = CVector(0.0f, 0.0f, 0.0f);

    // Create our main world sectors covering the mainland
    CVector2D size(m_fSectorSize, m_fRowSize);
    CVector2D bottomLeft(-WORLD_SIZE, -WORLD_SIZE);
    CVector2D topRight(WORLD_SIZE, WORLD_SIZE);
    CreateSectors(&m_WorldRows, size, bottomLeft, topRight);

    // Find our row and sector
    m_pRow = FindOrCreateRow(m_vecPosition);
    if (m_pRow)
    {
        CClientStreamSector* pInitialSector = m_pRow->FindOrCreateSector(m_vecPosition);
        if (pInitialSector)
        {
            OnEnterSector(pInitialSector);
        }
    }
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
    if (!pList)
        return;

    // Creates our sectors within rows, filling up our rectangle, connecting each sector and row
    CClientStreamSector *   pCurrent = NULL, *pPrevious = NULL, *pPreviousRowSector = NULL;
    CClientStreamSectorRow *pCurrentRow = NULL, *pPreviousRow = NULL;
    float                   fX = vecBottomLeft.fX, fY = vecBottomLeft.fY;

    while (fY < vecTopRight.fY)
    {
        pCurrentRow = new CClientStreamSectorRow(fY, fY + vecSize.fY, m_fSectorSize, m_fRowSize);
        if (!pCurrentRow)
            break;

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
            if (!pCurrent)
                break;

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
    if (!pRow)
        return;

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

void CClientStreamer::DoPulse(CVector& vecPosition)
{
    // Validate position
    if (!std::isfinite(vecPosition.fX) || !std::isfinite(vecPosition.fY) || !std::isfinite(vecPosition.fZ))
    {
        // Invalid position, use last valid position
        vecPosition = m_vecPosition;
        return;
    }

    bool bMovedFar = false;
    bool bPositionChanged = false;

    // Has our position changed?
    if (vecPosition != m_vecPosition)
    {
        bPositionChanged = true;
        float fDistSquared = (m_vecPosition - vecPosition).LengthSquared();
        bMovedFar = (fDistSquared > (50.0f * 50.0f));
        m_vecPosition = vecPosition;

        // Ensure m_pRow exists
        if (!m_pRow)
        {
            m_pRow = FindOrCreateRow(vecPosition);
            if (m_pRow)
            {
                CClientStreamSector* pSector = m_pRow->FindOrCreateSector(vecPosition);
                if (pSector)
                    OnEnterSector(pSector);
            }
            return;
        }

        // Have we changed row?
        if (!m_pRow->DoesContain(vecPosition))
        {
            CClientStreamSectorRow* pNewRow = FindOrCreateRow(vecPosition, m_pRow);
            if (pNewRow)
            {
                m_pRow = pNewRow;
                CClientStreamSector* pSector = m_pRow->FindOrCreateSector(vecPosition);
                if (pSector)
                    OnEnterSector(pSector);
            }
        }
        // Have we changed sector?
        else if (m_pSector && !m_pSector->DoesContain(vecPosition))
        {
            // Grab our new sector
            CClientStreamSector* pNewSector = m_pRow->FindOrCreateSector(vecPosition, m_pSector);
            if (pNewSector)
                OnEnterSector(pNewSector);
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

    // Reserve space for vectors
    std::vector<CClientStreamElement*> elementsToHide;
    std::vector<CClientStreamElement*> elementsToShow;

    elementsToHide.reserve(100);
    elementsToShow.reserve(100);

    // Collect elements from sectors that need to be hidden
    auto collectFromRows = [this, &elementsToHide](list<CClientStreamSectorRow*>& rowList)
    {
        for (CClientStreamSectorRow* sectorRow : rowList)
        {
            if (!sectorRow)
                continue;

            for (CClientStreamSector* sector : sectorRow->GetList())
            {
                if (!sector)
                    continue;

                // Process directly without making a full copy
                for (auto iter = sector->Begin(); iter != sector->End(); ++iter)
                {
                    CClientStreamElement* element = *iter;
                    if (!element)
                        continue;

                    // Should this element be hidden in the new dimension?
                    if (!IsElementShouldVisibleInCurrentDimension(element))
                    {
                        elementsToHide.push_back(element);
                    }
                }
            }
        }
    };

    collectFromRows(m_WorldRows);
    collectFromRows(m_ExtraRows);

    // Collect elements that should be shown in new dimension
    for (CClientStreamElement* element : m_outsideCurrentDimensionElements)
    {
        if (!element)
            continue;

        // Should this element be visible in new dimension?
        if (element->GetDimension() == usDimension || element->IsVisibleInAllDimensions())
        {
            elementsToShow.push_back(element);
        }
    }

    // Process elements to hide
    for (CClientStreamElement* element : elementsToHide)
    {
        if (!element)
            continue;

        CClientStreamSector* sector = element->GetStreamSector();
        if (sector)
        {
            sector->Remove(element);
            element->SetStreamSector(nullptr);
            m_ActiveElements.remove(element);
            m_outsideCurrentDimensionElements.push_back(element);

            if (element->IsStreamedIn())
            {
                m_ToStreamOut.push_back(element);
            }
        }
    }

    // Process elements to show
    for (CClientStreamElement* element : elementsToShow)
    {
        if (!element)
            continue;
        m_outsideCurrentDimensionElements.remove(element);
        AddElementInSectors(element);
    }
}

CClientStreamSectorRow* CClientStreamer::FindOrCreateRow(CVector& vecPosition, CClientStreamSectorRow* pSurrounding)
{
    // Validate position
    if (!std::isfinite(vecPosition.fY))
        return NULL;

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
        if (pRow && pRow->DoesContain(vecPosition))
        {
            return pRow;
        }
    }

    // Search through our extra rows
    iter = m_ExtraRows.begin();
    for (; iter != m_ExtraRows.end(); iter++)
    {
        pRow = *iter;
        if (pRow && pRow->DoesContain(vecPosition))
        {
            return pRow;
        }
    }
    // We need a new row, align it with the others
    float fBottom = float((int)(vecPosition.fY / m_fRowSize)) * m_fRowSize;
    if (vecPosition.fY < 0.0f)
        fBottom -= m_fRowSize;
    pRow = new CClientStreamSectorRow(fBottom, fBottom + m_fRowSize, m_fSectorSize, m_fRowSize);
    if (!pRow)
        return NULL;

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
        if (pRow && pRow->DoesContain(fY))
        {
            return pRow;
        }
    }

    // Search through our extra rows
    iter = m_ExtraRows.begin();
    for (; iter != m_ExtraRows.end(); iter++)
    {
        pRow = *iter;
        if (pRow && pRow->DoesContain(fY))
        {
            return pRow;
        }
    }
    return NULL;
}

void CClientStreamer::OnUpdateStreamPosition(CClientStreamElement* pElement)
{
    if (!pElement || !pElement->GetStreamSector())
        return;

    CVector vecPosition = pElement->GetStreamPosition();

    // Validate element position
    if (!std::isfinite(vecPosition.fX) || !std::isfinite(vecPosition.fY) || !std::isfinite(vecPosition.fZ))
        return;

    CClientStreamSectorRow* pRow = pElement->GetStreamRow();
    CClientStreamSector*    pSector = pElement->GetStreamSector();

    if (!pRow || !pSector)
        return;

    // Have we changed row?
    if (!pRow->DoesContain(vecPosition))
    {
        pRow = FindOrCreateRow(vecPosition);
        if (!pRow)
            return;

        pElement->SetStreamRow(pRow);
        CClientStreamSector* pNewSector = pRow->FindOrCreateSector(vecPosition);
        if (pNewSector)
            OnElementEnterSector(pElement, pNewSector);
    }
    // Have we changed sector?
    else if (!pSector->DoesContain(vecPosition))
    {
        // Grab our new sector
        CClientStreamSector* pNewSector = pRow->FindOrCreateSector(vecPosition, pSector);
        if (pNewSector)
            OnElementEnterSector(pElement, pNewSector);
    }
    else
    {
        // Make sure our distance is updated
        pElement->SetExpDistance(pElement->GetDistanceToBoundingBoxSquared(m_vecPosition));
    }
}

void CClientStreamer::AddElementInSectors(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    // Check if already in a sector
    if (pElement->GetStreamSector())
    {
        // Element already in a sector, update its position instead
        OnUpdateStreamPosition(pElement);
        return;
    }

    assert(pAddingElement == NULL);
    pAddingElement = pElement;

    CVector vecPosition = pElement->GetStreamPosition();

    // Validate position
    if (!std::isfinite(vecPosition.fX) || !std::isfinite(vecPosition.fY) || !std::isfinite(vecPosition.fZ))
    {
        pAddingElement = NULL;
        return;
    }

    CClientStreamSectorRow* pRow = FindOrCreateRow(vecPosition);
    if (pRow)
    {
        pElement->SetStreamRow(pRow);
        CClientStreamSector* pSector = pRow->FindOrCreateSector(vecPosition);
        if (pSector)
            OnElementEnterSector(pElement, pSector);
    }
    pAddingElement = NULL;
}

void CClientStreamer::RemoveElementFromSectors(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    OnElementEnterSector(pElement, nullptr);
    m_ToStreamOut.remove(pElement);
}

bool CClientStreamer::IsElementShouldVisibleInCurrentDimension(CClientStreamElement* pElement)
{
    if (!pElement)
        return false;
    return pElement->GetDimension() == m_usDimension || pElement->IsVisibleInAllDimensions();
}

void CClientStreamer::AddElement(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    if (IsElementShouldVisibleInCurrentDimension(pElement))
        AddElementInSectors(pElement);
    else
        m_outsideCurrentDimensionElements.push_back(pElement);
}

void CClientStreamer::RemoveElement(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    if (pElement->GetStreamSector())
    {
        CClientStreamSector* sector = pElement->GetStreamSector();
        sector->Remove(pElement);
        pElement->SetStreamSector(nullptr);
    }

    m_ActiveElements.remove(pElement);
    m_ToStreamOut.remove(pElement);
    m_outsideCurrentDimensionElements.remove(pElement);
}

void CClientStreamer::SetExpDistances(list<CClientStreamElement*>* pList)
{
    if (!pList)
        return;

    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        CClientStreamElement* pElement = *iter;
        if (!pElement)
            continue;

        pElement->SetExpDistance(pElement->GetDistanceToBoundingBoxSquared(m_vecPosition));
    }
}

void CClientStreamer::AddToSortedList(list<CClientStreamElement*>* pList, CClientStreamElement* pElement)
{
    if (!pList || !pElement)
        return;

    // Make sure it's exp distance is updated
    float fDistance = pElement->GetDistanceToBoundingBoxSquared(m_vecPosition);
    pElement->SetExpDistance(fDistance);

    // Don't add if already in the list
    if (ListContains(*pList, pElement))
        return;

    // Search through our list. Add it behind the first item further away than this
    list<CClientStreamElement*>::iterator iter = pList->begin();
    for (; iter != pList->end(); iter++)
    {
        CClientStreamElement* pTemp = *iter;
        if (!pTemp)
            continue;

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
    if (!p1 && !p2)
        return false;
    if (!p1)
        return false;
    if (!p2)
        return true;
    return p1->GetExpDistance() < p2->GetExpDistance();
}

bool CClientStreamer::IsActiveElement(CClientStreamElement* pElement)
{
    if (!pElement)
        return false;

    for (auto iter = m_ActiveElements.begin(); iter != m_ActiveElements.end(); ++iter)
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
    int  processedOut = 0;
    auto iter = m_ToStreamOut.begin();

    while (iter != m_ToStreamOut.end() && processedOut < iMaxOut)
    {
        CClientStreamElement* e = *iter;

        if (!e)
        {
            iter = m_ToStreamOut.erase(iter);
            continue;
        }

        // Only stream out if no references remain
        if (e->GetTotalStreamReferences() == 0)
        {
            e->InternalStreamOut();
            iter = m_ToStreamOut.erase(iter);
            processedOut++;
        }
        else
        {
            ++iter;
        }
    }

    // Prepare temporary lists
    std::vector<CClientStreamElement*> closestOut;
    std::vector<CClientStreamElement*> furthestIn;

    // Reserve space
    closestOut.reserve(iMaxIn);
    furthestIn.reserve(50);

    bool reachedLimit = ReachedLimit();
    int  processedIn = 0;

    // Elements are sorted: nearest -> furthest
    for (CClientStreamElement* e : m_ActiveElements)
    {
        if (!e)
            continue;

        // Early exit if we've processed enough
        if (processedIn >= iMaxIn && processedOut >= iMaxOut && !bMovedFar)
            break;

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
                        {
                            m_ToStreamOut.push_back(e);
                            continue;
                        }
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
                    {
                        m_ToStreamOut.push_back(e);
                        continue;
                    }
                }
                break;

                default:
                    break;
            }

            // Too far -> stream out (use threshold to avoid flickering)
            if (dist > m_fMaxDistanceThreshold)
            {
                if (processedOut < iMaxOut && e->GetTotalStreamReferences() == 0)
                {
                    e->InternalStreamOut();
                    processedOut++;
                }
                else
                {
                    m_ToStreamOut.push_back(e);
                }
            }
            else
            {
                // Still inside distance -> keep track for possible swapping
                if (furthestIn.size() < 50)            // Limit size
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
            if (processedIn < iMaxIn)
            {
                e->InternalStreamIn(bMovedFar);
                processedIn++;

                reachedLimit = ReachedLimit();
            }
        }
    }

    // Swap logic when at streaming limit
    if (!reachedLimit || closestOut.empty() || furthestIn.empty())
        return;

    int          fi = (int)furthestIn.size() - 1;
    unsigned int co = 0;

    // Up to 10 swaps per frame
    for (int i = 0; i < 10; i++)
    {
        if (processedIn >= iMaxIn || processedOut >= iMaxOut)
            break;
        if (fi < 0)
            break;
        if (co >= closestOut.size())
            break;

        auto* fIn = furthestIn[fi];
        auto* cOut = closestOut[co];

        if (!fIn || !cOut)
            break;

        // Only swap if the streamed-out item is closer
        if (cOut->GetExpDistance() >= fIn->GetExpDistance())
            break;

        // Stream out the furthest streamed-in item
        if (fIn->GetTotalStreamReferences() == 0)
        {
            fIn->InternalStreamOut();
            processedOut++;
        }
        m_ToStreamOut.remove(fIn);
        --fi;

        // Stream in the closest streamed-out item
        if (!ReachedLimit())
        {
            cOut->InternalStreamIn(bMovedFar);
            processedIn++;
            ++co;
        }
    }
}

void CClientStreamer::OnEnterSector(CClientStreamSector* pSector)
{
    if (!pSector)
        return;

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
            if (!pTempSector)
                continue;

            // Make sure we dont unload our new sector
            if (pTempSector != pSector)
            {
                if (pTempSector->IsActivated())
                {
                    for (auto elemIter = pTempSector->Begin(); elemIter != pTempSector->End(); elemIter++)
                    {
                        pElement = *elemIter;
                        if (pElement && pElement->IsStreamedIn())
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

        // Activate the wanted sectors
        iter = uncommon.begin();
        for (; iter != uncommon.end(); iter++)
        {
            pTempSector = *iter;
            if (!pTempSector)
                continue;

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
    if (!pElement)
        return;

    CClientStreamSector* pPreviousSector = pElement->GetStreamSector();
    if (pPreviousSector)
    {
        // Skip if disconnecting
        if (g_pClientGame && g_pClientGame->IsBeingDeleted())
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
            if (m_pSector && pSector->IsExtra() && (m_pSector->IsMySurroundingSector(pSector) || m_pSector == pSector))
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
    if (!pElement)
        return;

    // Make sure we're streamed in
    pElement->InternalStreamIn(true);
}

void CClientStreamer::OnElementForceStreamOut(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    // Make sure we're streamed out if need be
    if (!IsActiveElement(pElement))
    {
        m_ToStreamOut.push_back(pElement);
    }
}

void CClientStreamer::OnElementDimension(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    bool shouldBeVisible = IsElementShouldVisibleInCurrentDimension(pElement);
    bool currentlyInSector = (pElement->GetStreamSector() != nullptr);

    // Should be visible but not in sector -> Add it
    if (shouldBeVisible && !currentlyInSector)
    {
        m_outsideCurrentDimensionElements.remove(pElement);
        AddElementInSectors(pElement);
    }
    // Should NOT be visible but IS in sector -> Remove it
    else if (!shouldBeVisible && currentlyInSector)
    {
        // Remove from sector
        CClientStreamSector* sector = pElement->GetStreamSector();
        if (sector)
        {
            sector->Remove(pElement);
        }

        pElement->SetStreamSector(nullptr);
        m_ActiveElements.remove(pElement);
        m_outsideCurrentDimensionElements.push_back(pElement);

        // Stream out if needed
        if (pElement->IsStreamedIn())
        {
            m_ToStreamOut.push_back(pElement);
        }
    }
}
