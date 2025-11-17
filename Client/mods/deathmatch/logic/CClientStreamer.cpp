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

extern CClientGame* g_pClientGame;

void* CClientStreamer::pAddingElement = nullptr;

// Constants
#define STREAMER_MOVEMENT_THRESHOLD 50.0f
#define STREAMER_MAX_OUT_DEFAULT    6
#define STREAMER_MAX_IN_DEFAULT     6
#define STREAMER_MAX_OUT_FAR        1000
#define STREAMER_MAX_IN_FAR         1000
#define STREAMER_MAX_SWAPS          10
#define STREAMER_FURTHEST_IN_LIMIT  50

// Element categories
enum class ElementCategory
{
    PLAYER,
    VEHICLE,
    OBJECT,
    OTHER
};

inline ElementCategory GetElementCategory(CClientStreamElement* pElement)
{
    if (IS_PLAYER(pElement))
        return ElementCategory::PLAYER;

    if (IS_VEHICLE(pElement))
        return ElementCategory::VEHICLE;

    if (IS_OBJECT(pElement))
        return ElementCategory::OBJECT;

    return ElementCategory::OTHER;
}

CClientStreamer::CClientStreamer(StreamerLimitReachedFunction* pLimitReachedFunc, float fMaxDistance, float fSectorSize, float fRowSize)
    : m_fSectorSize(fSectorSize),
      m_fRowSize(fRowSize),
      m_pRow(nullptr),
      m_pSector(nullptr),
      m_fMaxDistanceExp(fMaxDistance * fMaxDistance),
      m_fMaxDistanceThreshold((fMaxDistance + 50.0f) * (fMaxDistance + 50.0f)),
      m_usDimension(0),
      m_vecPosition(0.0f, 0.0f, 0.0f),
      m_pLimitReachedFunc(pLimitReachedFunc),
      m_iMaxInDefault(STREAMER_MAX_IN_DEFAULT),
      m_iMaxOutDefault(STREAMER_MAX_OUT_DEFAULT),
      m_iMaxInFar(STREAMER_MAX_IN_FAR),
      m_iMaxOutFar(STREAMER_MAX_OUT_FAR)
{
    // We need the limit reached func
    assert(pLimitReachedFunc);

    // Create our main world sectors covering the mainland
    CVector2D size(m_fSectorSize, m_fRowSize);
    CVector2D bottomLeft(-WORLD_SIZE, -WORLD_SIZE);
    CVector2D topRight(WORLD_SIZE, WORLD_SIZE);
    CreateSectors(&m_WorldRows, size, bottomLeft, topRight);

    // Find our row and sector
    m_pRow = FindOrCreateRow(m_vecPosition);
    if (!m_pRow)
        return;

    CClientStreamSector* pInitialSector = m_pRow->FindOrCreateSector(m_vecPosition);
    if (pInitialSector)
        OnEnterSector(pInitialSector);
}

CClientStreamer::~CClientStreamer()
{
    // Clear our mainland rows
    for (CClientStreamSectorRow* pRow : m_WorldRows)
        delete pRow;

    m_WorldRows.clear();

    // Clear our extra rows
    for (CClientStreamSectorRow* pRow : m_ExtraRows)
        delete pRow;

    m_ExtraRows.clear();
}

void CClientStreamer::CreateSectors(std::list<CClientStreamSectorRow*>* pList, CVector2D& vecSize, CVector2D& vecBottomLeft, CVector2D& vecTopRight)
{
    if (!pList)
        return;

    // Creates our sectors within rows, filling up our rectangle, connecting each sector and row
    CClientStreamSector*    pCurrent = nullptr;
    CClientStreamSector*    pPrevious = nullptr;
    CClientStreamSector*    pPreviousRowSector = nullptr;
    CClientStreamSectorRow* pCurrentRow = nullptr;
    CClientStreamSectorRow* pPreviousRow = nullptr;
    float                   fX = vecBottomLeft.fX;
    float                   fY = vecBottomLeft.fY;

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

        pPrevious = nullptr;
        pCurrent = nullptr;
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

    bool bPositionChanged = false;
    bool bMovedFar = false;

    // Has our position changed?
    if (vecPosition != m_vecPosition)
    {
        bPositionChanged = true;
        const float fDistSquared = (m_vecPosition - vecPosition).LengthSquared();
        bMovedFar = (fDistSquared > (STREAMER_MOVEMENT_THRESHOLD * STREAMER_MOVEMENT_THRESHOLD));
        m_vecPosition = vecPosition;

        // Ensure m_pRow exists
        if (!m_pRow)
        {
            m_pRow = FindOrCreateRow(vecPosition);
            if (!m_pRow)
                return;

            CClientStreamSector* pSector = m_pRow->FindOrCreateSector(vecPosition);
            if (pSector)
                OnEnterSector(pSector);

            return;
        }

        // Have we changed row?
        if (!m_pRow->DoesContain(vecPosition))
        {
            CClientStreamSectorRow* pNewRow = FindOrCreateRow(vecPosition, m_pRow);
            if (!pNewRow)
                return;

            m_pRow = pNewRow;
            CClientStreamSector* pSector = m_pRow->FindOrCreateSector(vecPosition);
            if (pSector)
                OnEnterSector(pSector);
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

    // Collect elements from sectors that need to be hidden
    auto collectFromRows = [this, &elementsToHide](std::list<CClientStreamSectorRow*>& rowList)
    {
        for (CClientStreamSectorRow* pSectorRow : rowList)
        {
            if (!pSectorRow)
                continue;

            for (CClientStreamSector* pSector : pSectorRow->GetList())
            {
                if (!pSector)
                    continue;

                // Process directly without making a full copy
                for (auto iter = pSector->Begin(); iter != pSector->End(); ++iter)
                {
                    CClientStreamElement* pElement = *iter;
                    if (!pElement)
                        continue;

                    // Should this element be hidden in the new dimension?
                    if (!IsElementShouldVisibleInCurrentDimension(pElement))
                        elementsToHide.push_back(pElement);
                }
            }
        }
    };

    collectFromRows(m_WorldRows);
    collectFromRows(m_ExtraRows);

    // Collect elements that should be shown in new dimension
    for (CClientStreamElement* pElement : m_outsideCurrentDimensionElements)
    {
        if (!pElement)
            continue;

        // Should this element be visible in new dimension?
        if (pElement->GetDimension() == usDimension || pElement->IsVisibleInAllDimensions())
            elementsToShow.push_back(pElement);
    }

    // Process elements to hide
    for (CClientStreamElement* pElement : elementsToHide)
    {
        if (!pElement)
            continue;

        CClientStreamSector* pSector = pElement->GetStreamSector();
        if (!pSector)
            continue;

        pSector->Remove(pElement);
        pElement->SetStreamSector(nullptr);
        m_ActiveElements.remove(pElement);
        m_outsideCurrentDimensionElements.push_back(pElement);

        if (pElement->IsStreamedIn())
            m_ToStreamOut.push_back(pElement);
    }

    // Process elements to show
    for (CClientStreamElement* pElement : elementsToShow)
    {
        if (!pElement)
            continue;

        m_outsideCurrentDimensionElements.remove(pElement);
        AddElementInSectors(pElement);
    }
}

CClientStreamSectorRow* CClientStreamer::FindOrCreateRow(CVector& vecPosition, CClientStreamSectorRow* pSurrounding)
{
    // Validate position
    if (!std::isfinite(vecPosition.fY))
        return nullptr;

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
    for (CClientStreamSectorRow* pRow : m_WorldRows)
    {
        if (pRow && pRow->DoesContain(vecPosition))
            return pRow;
    }

    // Search through our extra rows
    for (CClientStreamSectorRow* pRow : m_ExtraRows)
    {
        if (pRow && pRow->DoesContain(vecPosition))
            return pRow;
    }

    // We need a new row, align it with the others
    float fBottom = static_cast<float>(static_cast<int>(vecPosition.fY / m_fRowSize)) * m_fRowSize;
    if (vecPosition.fY < 0.0f)
        fBottom -= m_fRowSize;

    CClientStreamSectorRow* pRow = new CClientStreamSectorRow(fBottom, fBottom + m_fRowSize, m_fSectorSize, m_fRowSize);
    if (!pRow)
        return nullptr;

    ConnectRow(pRow);
    pRow->SetExtra(true);
    m_ExtraRows.push_back(pRow);

    return pRow;
}

CClientStreamSectorRow* CClientStreamer::FindRow(float fY)
{
    // Search through our main world rows
    for (CClientStreamSectorRow* pRow : m_WorldRows)
    {
        if (pRow && pRow->DoesContain(fY))
            return pRow;
    }

    // Search through our extra rows
    for (CClientStreamSectorRow* pRow : m_ExtraRows)
    {
        if (pRow && pRow->DoesContain(fY))
            return pRow;
    }

    return nullptr;
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

    assert(pAddingElement == nullptr);
    pAddingElement = pElement;

    CVector vecPosition = pElement->GetStreamPosition();

    // Validate position
    if (!std::isfinite(vecPosition.fX) || !std::isfinite(vecPosition.fY) || !std::isfinite(vecPosition.fZ))
    {
        pAddingElement = nullptr;
        return;
    }

    CClientStreamSectorRow* pRow = FindOrCreateRow(vecPosition);
    if (!pRow)
    {
        pAddingElement = nullptr;
        return;
    }

    pElement->SetStreamRow(pRow);
    CClientStreamSector* pSector = pRow->FindOrCreateSector(vecPosition);
    if (pSector)
        OnElementEnterSector(pElement, pSector);

    pAddingElement = nullptr;
}

void CClientStreamer::RemoveElementFromSectors(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    OnElementEnterSector(pElement, nullptr);

    // Clear row and sector
    pElement->SetStreamRow(nullptr);

    // Remove from active elements
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
        CClientStreamSector* pSector = pElement->GetStreamSector();
        pSector->Remove(pElement);
        pElement->SetStreamSector(nullptr);
    }

    m_ActiveElements.remove(pElement);
    m_ToStreamOut.remove(pElement);
    m_outsideCurrentDimensionElements.remove(pElement);
}

void CClientStreamer::SetExpDistances(std::list<CClientStreamElement*>* pList)
{
    if (!pList)
        return;

    for (CClientStreamElement* pElement : *pList)
    {
        if (!pElement)
            continue;

        pElement->SetExpDistance(pElement->GetDistanceToBoundingBoxSquared(m_vecPosition));
    }
}

void CClientStreamer::AddToSortedList(std::list<CClientStreamElement*>* pList, CClientStreamElement* pElement)
{
    if (!pList || !pElement)
        return;

    // Make sure its exp distance is updated
    const float fDistance = pElement->GetDistanceToBoundingBoxSquared(m_vecPosition);
    pElement->SetExpDistance(fDistance);

    // Don't add if already in the list
    if (ListContains(*pList, pElement))
        return;

    // Search through our list. Add it behind the first item further away than this
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
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
    if (!p1 || !p2)
        return false;

    return p1->GetExpDistance() < p2->GetExpDistance();
}

bool CClientStreamer::IsActiveElement(CClientStreamElement* pElement)
{
    if (!pElement)
        return false;

    for (CClientStreamElement* pActive : m_ActiveElements)
    {
        if (pActive == pElement)
            return true;
    }

    return false;
}

void CClientStreamer::Restream(bool bMovedFar)
{
    // Adjust stream-in/out limits (large movement allows massive updates)
    const int iMaxOut = bMovedFar ? m_iMaxOutFar : m_iMaxOutDefault;
    const int iMaxIn = bMovedFar ? m_iMaxInFar : m_iMaxInDefault;

    // Process pending stream-out elements
    int  iProcessedOut = 0;
    auto iter = m_ToStreamOut.begin();

    while (iter != m_ToStreamOut.end() && iProcessedOut < iMaxOut)
    {
        CClientStreamElement* pElement = *iter;

        if (!pElement)
        {
            iter = m_ToStreamOut.erase(iter);
            continue;
        }

        // Only stream out if no references remain
        if (pElement->GetTotalStreamReferences() == 0)
        {
            pElement->InternalStreamOut();
            iter = m_ToStreamOut.erase(iter);
            iProcessedOut++;
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
    furthestIn.reserve(STREAMER_FURTHEST_IN_LIMIT);

    bool bReachedLimit = ReachedLimit();
    int  iProcessedIn = 0;

    // Elements are sorted: nearest -> furthest
    for (CClientStreamElement* pElement : m_ActiveElements)
    {
        if (!pElement)
            continue;

        // Early exit if we've processed enough
        if (iProcessedIn >= iMaxIn && iProcessedOut >= iMaxOut && !bMovedFar)
            break;

        const float     fDist = pElement->GetExpDistance();
        ElementCategory category = GetElementCategory(pElement);
        const bool      bIsIn = pElement->IsStreamedIn();

        if (bIsIn)
        {
            // Extra handling for vehicles and players
            switch (category)
            {
                case ElementCategory::VEHICLE:
                {
                    auto* pVehicle = DynamicCast<CClientVehicle>(pElement);
                    if (!pVehicle)
                        break;

                    // Skip if player in vehicle had a lightsync update
                    if (pVehicle->GetOccupant() && IS_PLAYER(pVehicle->GetOccupant()))
                    {
                        auto* pPlayer = DynamicCast<CClientPlayer>(pVehicle->GetOccupant());
                        if (pPlayer && pPlayer->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                        {
                            m_ToStreamOut.push_back(pElement);
                            continue;
                        }
                    }

                    // Towed vehicles are handled by the tow vehicle
                    if (pVehicle->GetTowedByVehicle())
                        continue;
                }
                break;

                case ElementCategory::PLAYER:
                {
                    auto* pPlayer = DynamicCast<CClientPlayer>(pElement);

                    // Lightsync players should not be loaded
                    if (pPlayer && pPlayer->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                    {
                        m_ToStreamOut.push_back(pElement);
                        continue;
                    }
                }
                break;

                default:
                    break;
            }

            // Too far -> stream out (use threshold to avoid flickering)
            if (fDist > m_fMaxDistanceThreshold)
            {
                if (iProcessedOut < iMaxOut && pElement->GetTotalStreamReferences() == 0)
                {
                    pElement->InternalStreamOut();
                    iProcessedOut++;
                }
                else
                {
                    m_ToStreamOut.push_back(pElement);
                }
            }
            else
            {
                // Still inside distance -> keep track for possible swapping
                if (furthestIn.size() < STREAMER_FURTHEST_IN_LIMIT)
                    furthestIn.push_back(pElement);
            }

            continue;
        }

        // Too far -> ignore
        if (fDist > m_fMaxDistanceExp)
            continue;

        // Special rules for vehicles / players
        switch (category)
        {
            case ElementCategory::VEHICLE:
            {
                auto* pVehicle = DynamicCast<CClientVehicle>(pElement);
                if (!pVehicle)
                    break;

                // Lightsync driver -> do not stream in
                if (pVehicle->GetOccupant() && IS_PLAYER(pVehicle->GetOccupant()))
                {
                    auto* pPlayer = DynamicCast<CClientPlayer>(pVehicle->GetOccupant());
                    if (pPlayer && pPlayer->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                        continue;
                }

                // Towed vehicles are handled elsewhere
                if (pVehicle->GetTowedByVehicle())
                    continue;
            }
            break;

            case ElementCategory::PLAYER:
            {
                auto* pPlayer = DynamicCast<CClientPlayer>(pElement);
                if (pPlayer && pPlayer->GetLastPuresyncType() == PURESYNC_TYPE_LIGHTSYNC)
                    continue;
            }
            break;

            default:
                break;
        }

        // Attached element: only load if LOD does not match
        if (auto* pAttached = DynamicCast<CClientStreamElement>(pElement->GetAttachedTo()))
        {
            if (!pAttached->IsStreamedIn())
            {
                auto* pObject = DynamicCast<CClientObject>(pElement);
                auto* pObjectAtt = DynamicCast<CClientObject>(pAttached);
                if (!pObject || !pObjectAtt || pObject->IsLowLod() == pObjectAtt->IsLowLod())
                    continue;
            }
        }

        // Reached streaming limit -> mark as candidate
        if (bReachedLimit)
        {
            if (static_cast<int>(closestOut.size()) < iMaxIn)
                closestOut.push_back(pElement);
        }
        else
        {
            // Stream in now (instant if moved far)
            if (iProcessedIn < iMaxIn)
            {
                pElement->InternalStreamIn(bMovedFar);
                iProcessedIn++;

                bReachedLimit = ReachedLimit();
            }
        }
    }

    // Swap logic when at streaming limit
    if (!bReachedLimit || closestOut.empty() || furthestIn.empty())
        return;

    int          iFurthestIndex = static_cast<int>(furthestIn.size()) - 1;
    unsigned int uiClosestIndex = 0;

    // Up to STREAMER_MAX_SWAPS swaps per frame
    for (int i = 0; i < STREAMER_MAX_SWAPS; i++)
    {
        if (iProcessedIn >= iMaxIn || iProcessedOut >= iMaxOut)
            break;

        if (iFurthestIndex < 0)
            break;

        if (uiClosestIndex >= closestOut.size())
            break;

        auto* pFurthestIn = furthestIn[iFurthestIndex];
        auto* pClosestOut = closestOut[uiClosestIndex];

        if (!pFurthestIn || !pClosestOut)
            break;

        // Only swap if the streamed-out item is closer
        if (pClosestOut->GetExpDistance() >= pFurthestIn->GetExpDistance())
            break;

        // Stream out the furthest streamed-in item
        if (pFurthestIn->GetTotalStreamReferences() == 0)
        {
            pFurthestIn->InternalStreamOut();
            iProcessedOut++;
        }

        m_ToStreamOut.remove(pFurthestIn);
        --iFurthestIndex;

        // Stream in the closest streamed-out item
        if (!ReachedLimit())
        {
            pClosestOut->InternalStreamIn(bMovedFar);
            iProcessedIn++;
            ++uiClosestIndex;
        }
    }
}

void CClientStreamer::OnEnterSector(CClientStreamSector* pSector)
{
    if (!pSector)
        return;

    if (!m_pSector)
    {
        m_pSector = pSector;
        SetExpDistances(&m_ActiveElements);
        m_ActiveElements.sort(CompareExpDistance);
        return;
    }

    // Grab the unwanted sectors
    std::list<CClientStreamSector*> common;
    std::list<CClientStreamSector*> uncommon;
    pSector->CompareSurroundings(m_pSector, &common, &uncommon, true);

    // Deactivate the unwanted sectors
    for (CClientStreamSector* pTempSector : uncommon)
    {
        if (!pTempSector)
            continue;

        // Make sure we dont unload our new sector
        if (pTempSector == pSector)
            continue;

        if (!pTempSector->IsActivated())
            continue;

        for (auto elemIter = pTempSector->Begin(); elemIter != pTempSector->End(); ++elemIter)
        {
            CClientStreamElement* pElement = *elemIter;
            if (pElement && pElement->IsStreamedIn())
                m_ToStreamOut.push_back(pElement);
        }

        pTempSector->RemoveElements(&m_ActiveElements);
        pTempSector->SetActivated(false);
    }

    // Grab the wanted sectors
    m_pSector->CompareSurroundings(pSector, &common, &uncommon, true);

    // Activate the wanted sectors
    for (CClientStreamSector* pTempSector : uncommon)
    {
        if (!pTempSector)
            continue;

        if (!pTempSector->IsActivated())
        {
            pTempSector->AddElements(&m_ActiveElements);
            pTempSector->SetActivated(true);
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
                AddToSortedList(&m_ActiveElements, pElement);
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

    // Update the element's sector pointer
    pElement->SetStreamSector(pSector);

    // If the element no longer has a sector, clear the row pointer too
    if (!pSector)
        pElement->SetStreamRow(nullptr);
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
        m_ToStreamOut.push_back(pElement);
}

void CClientStreamer::OnElementDimension(CClientStreamElement* pElement)
{
    if (!pElement)
        return;

    const bool bShouldBeVisible = IsElementShouldVisibleInCurrentDimension(pElement);
    const bool bCurrentlyInSector = (pElement->GetStreamSector() != nullptr);

    // Should be visible but not in sector -> Add it
    if (bShouldBeVisible && !bCurrentlyInSector)
    {
        m_outsideCurrentDimensionElements.remove(pElement);
        AddElementInSectors(pElement);
        return;
    }

    // Should NOT be visible but IS in sector -> Remove it
    if (!bShouldBeVisible && bCurrentlyInSector)
    {
        // Remove from sector
        CClientStreamSector* pSector = pElement->GetStreamSector();
        if (pSector)
            pSector->Remove(pElement);

        pElement->SetStreamSector(nullptr);
        m_ActiveElements.remove(pElement);
        m_outsideCurrentDimensionElements.push_back(pElement);

        // Stream out if needed
        if (pElement->IsStreamedIn())
            m_ToStreamOut.push_back(pElement);

        // Clear row pointer
        pElement->SetStreamRow(nullptr);
    }
}

void CClientStreamer::SetStreamerLimits(int normalIn, int normalOut, int farIn, int farOut)
{
    if (normalIn <= 0 || normalOut <= 0 || farIn <= 0 || farOut <= 0)
        return;

    m_iMaxInDefault = normalIn;
    m_iMaxOutDefault = normalOut;
    m_iMaxInFar = farIn;
    m_iMaxOutFar = farOut;
}

void CClientStreamer::ResetStreamerLimits()
{
    m_iMaxInDefault = STREAMER_MAX_IN_DEFAULT;
    m_iMaxOutDefault = STREAMER_MAX_OUT_DEFAULT;
    m_iMaxInFar = STREAMER_MAX_IN_FAR;
    m_iMaxOutFar = STREAMER_MAX_OUT_FAR;
    m_iMaxSwaps = STREAMER_MAX_SWAPS;
    m_iFurthestInLimit = STREAMER_FURTHEST_IN_LIMIT;
}

void CClientStreamer::GetStreamingLimits(int& normalIn, int& normalOut, int& farIn, int& farOut, int& maxSwaps, int& furthestInLimit) const noexcept
{
    normalIn = m_iMaxInDefault;
    normalOut = m_iMaxOutDefault;
    farIn = m_iMaxInFar;
    farOut = m_iMaxOutFar;
    maxSwaps = m_iMaxSwaps;
    furthestInLimit = m_iFurthestInLimit;
}

void CClientStreamer::SetStreamerMaxSwaps(int maxSwaps)
{
    if (maxSwaps <= 0)
        return;

    m_iMaxSwaps = maxSwaps;
}

void CClientStreamer::ResetStreamerMaxSwaps()
{
    m_iMaxSwaps = STREAMER_MAX_SWAPS;
}

void CClientStreamer::SetStreamerFurthestInLimit(int limit)
{
    if (limit <= 0)
        return;

    m_iFurthestInLimit = limit;
}

void CClientStreamer::ResetStreamerFurthestInLimit()
{
    m_iFurthestInLimit = STREAMER_FURTHEST_IN_LIMIT;
}
