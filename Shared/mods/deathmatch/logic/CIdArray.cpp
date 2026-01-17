/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/CIdArray.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CIdArray.h"

#define SHARED_ARRAY_BASE_ID    0x02000000

CStack<SArrayId, 1>   CIdArray::m_IDStack;
std::vector<SIdEntry> CIdArray::m_Elements;
uint                  CIdArray::m_uiCapacity = 0;
bool                  CIdArray::m_bInitialized = false;
uint                  CIdArray::m_uiPopIdCounter = 0;
uint                  CIdArray::m_uiMinTicksBetweenIdReuse = 0;
CTickCount            CIdArray::m_LastPeriodStart;

#ifdef MTA_CLIENT
static const char* GetIdClassTypeName(EIdClassType eClass);
#endif

//
// Initialize
//
void CIdArray::Initialize()
{
    if (!m_bInitialized)
    {
        m_uiCapacity = 0;
        m_bInitialized = true;
        m_uiPopIdCounter = 0;
        m_uiMinTicksBetweenIdReuse = 1000 * 60 * 60;            // 1 hour
        m_LastPeriodStart = CTickCount::Now();
        ExpandBy(2000);
    }
}

//
// Add object to the list and get a new id for it
//
SArrayId CIdArray::PopUniqueId(void* pObject, EIdClassType idClass)
{
    if (!m_bInitialized)
        Initialize();

    // Measure amount of time it took to use 50% of free ids
    m_uiPopIdCounter++;
    if (m_uiPopIdCounter > m_IDStack.GetUnusedAmount() / 2)
    {
        CTickCount now = CTickCount::Now();
        uint       uiTimeToUse50PercentOfFreeIds = (uint)(now - m_LastPeriodStart).ToLongLong();

        // If it's less than 50% of the allowed time, add new ids
        if (uiTimeToUse50PercentOfFreeIds < m_uiMinTicksBetweenIdReuse / 2)
        {
            ExpandBy(2000);
        }
        else
        {
            m_LastPeriodStart = now;
            m_uiPopIdCounter = 0;
        }
    }

    // Ensure there is enough free ID's in all situations
    if (m_IDStack.GetUnusedAmount() < 1)
    {
        ExpandBy(2000);
    }

    // Grab the next unused ID
    SArrayId ulPhysicalIndex = INVALID_ARRAY_ID;
    bool     bSuccess = m_IDStack.Pop(ulPhysicalIndex);
    assert(bSuccess);

    // Checks
    assert(ulPhysicalIndex != INVALID_ARRAY_ID);

#ifdef MTA_CLIENT
    if (ulPhysicalIndex > m_uiCapacity)
    {
        WriteErrorEvent("--------------------------------------------------------------------------------");
        WriteErrorEvent("-- Error section for: CIdArray::PopUniqueId");
        WriteErrorEvent("--------------------------------------------------------------------------------");
        WriteErrorEvent(SString(" ulPhysicalIndex <= m_uiCapacity:   %s (must be false)", ulPhysicalIndex <= m_uiCapacity ? "true" : "false"));
        WriteErrorEvent(SString(" CIdArray::m_IDStack:               %lu unused, %lu capacity", m_IDStack.GetUnusedAmount(), m_IDStack.GetCapacity()));
        WriteErrorEvent(SString(" CIdArray::m_Elements:              %zu size, %u capacity", m_Elements.size(), m_Elements.capacity()));
        WriteErrorEvent(SString(" CIdArray::m_uiPopIdCounter:        %u", m_uiPopIdCounter));
        WriteErrorEvent(SString(" CIdArray::m_uiCapacity:            %u", m_uiCapacity));
        WriteErrorEvent(SString(" ulPhysicalIndex:                   %lu", ulPhysicalIndex));
        WriteErrorEvent("--------------------------------------------------------------------------------");
        WriteErrorEvent("-- Dump: CIdArray::m_Elements");
        WriteErrorEvent("--------------------------------------------------------------------------------");

        SArrayId startIndex = (ulPhysicalIndex < 5) ? 0 : (ulPhysicalIndex - 5);
        SArrayId stopIndex = std::min(startIndex + 10, static_cast<SArrayId>(m_Elements.size()));

        for (SArrayId i = startIndex; i < stopIndex; ++i)
        {
            WriteErrorEvent(
                SString(" [%lu] pObject = %p, idClass = %d (%s)", i, m_Elements[i].pObject, m_Elements[i].idClass, GetIdClassTypeName(m_Elements[i].idClass)));
        }

        WriteErrorEvent("--------------------------------------------------------------------------------");
        assert(false);
    }
#else
    assert(ulPhysicalIndex <= m_uiCapacity);
#endif
    assert(m_Elements.size() > ulPhysicalIndex);
    assert(m_Elements[ulPhysicalIndex].pObject == nullptr);
    assert(m_Elements[ulPhysicalIndex].idClass == EIdClass::NONE);

    m_Elements[ulPhysicalIndex].pObject = pObject;
    m_Elements[ulPhysicalIndex].idClass = idClass;

    // Map to ID
    SArrayId ulLogicalID = (ulPhysicalIndex + SHARED_ARRAY_BASE_ID);
    return ulLogicalID;
}

void CIdArray::PushUniqueId(void* pObject, EIdClassType idClass, SArrayId id)
{
    dassert(m_bInitialized);

    // Map to index
    assert(id >= SHARED_ARRAY_BASE_ID);
    SArrayId ulPhysicalIndex = id - SHARED_ARRAY_BASE_ID;

    // Checks
    assert(id != INVALID_ARRAY_ID);
    assert(ulPhysicalIndex <= m_uiCapacity);
    assert(m_Elements[ulPhysicalIndex].pObject == pObject);
    assert(m_Elements[ulPhysicalIndex].idClass == idClass);

    m_IDStack.Push(ulPhysicalIndex);
    m_Elements[ulPhysicalIndex].pObject = NULL;
    m_Elements[ulPhysicalIndex].idClass = EIdClass::NONE;
}

void* CIdArray::FindEntry(SArrayId id, EIdClassType idClass)
{
    // Return the element with the given ID

    // Map to index
    SArrayId ulPhysicalIndex = id - SHARED_ARRAY_BASE_ID;

    if (id != INVALID_ARRAY_ID && ulPhysicalIndex <= m_uiCapacity)
    {
        if (m_Elements[ulPhysicalIndex].idClass == idClass)
            return m_Elements[ulPhysicalIndex].pObject;
    }
    return NULL;
}

void CIdArray::ExpandBy(uint uiAmount)
{
    // Don't expand if more than 150000 unused items
    if (m_IDStack.GetUnusedAmount() > 150000)
        return;

    m_IDStack.ExpandBy(uiAmount);
    SIdEntry blankEntry = {NULL, EIdClass::NONE};
    m_Elements.resize(m_uiCapacity + uiAmount + 1, blankEntry);
    m_uiCapacity += uiAmount;
    assert(m_IDStack.GetCapacity() == m_uiCapacity);
}

uint CIdArray::GetCapacity()
{
    return m_uiCapacity;
}

uint CIdArray::GetUnusedAmount()
{
    return m_IDStack.GetUnusedAmount();
}

#ifdef MTA_CLIENT
static const char* GetIdClassTypeName(EIdClassType eClass)
{
    switch (eClass)
    {
        case EIdClassType::ACCOUNT:
            return "Account";
        case EIdClassType::ACL:
            return "ACL";
        case EIdClassType::ACL_GROUP:
            return "ACL Group";
        case EIdClassType::BAN:
            return "Ban";
        case EIdClassType::DB_JOBDATA:
            return "DB Jobdata";
        case EIdClassType::RESOURCE:
            return "Resource";
        case EIdClassType::TEXT_DISPLAY:
            return "Text Display";
        case EIdClassType::TEXT_ITEM:
            return "Text Item";
        case EIdClassType::TIMER:
            return "Timer";
        case EIdClassType::VECTOR2:
            return "Vector2";
        case EIdClassType::VECTOR3:
            return "Vector3";
        case EIdClassType::VECTOR4:
            return "Vector4";
        case EIdClassType::MATRIX:
            return "Matrix";
        default:
            return "None";
    }
}
#endif
