/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLArrayImpl.cpp
 *  PURPOSE:     XML array class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using namespace std;

#define XML_ARRAY_BASE_ID    0x01000000

CStack<std::uint32_t, 1> CXMLArray::m_IDStack;
std::vector<CXMLCommon*> CXMLArray::m_Elements;
std::uint32_t            CXMLArray::m_ulCapacity = 0;

void CXMLArray::Initialize()
{
    m_ulCapacity = 0;
    ExpandBy(20000);
}

std::uint32_t CXMLArray::PopUniqueID(CXMLCommon* pEntry)
{
    // Add more ID's if required
    if (m_IDStack.GetUnusedAmount() < 10000)
    {
        ExpandBy(10000);
    }

    // Grab the next unused ID
    std::uint32_t ulPhysicalIndex;
    bool          bSuccess = m_IDStack.Pop(ulPhysicalIndex);
    assert(bSuccess);

    // Checks
    assert((ulPhysicalIndex != INVALID_XML_ID) && (ulPhysicalIndex <= m_ulCapacity) && (m_Elements.size() > ulPhysicalIndex) &&
           (m_Elements[ulPhysicalIndex] == NULL));

    m_Elements[ulPhysicalIndex] = pEntry;

    // Map to ID
    std::uint32_t ulLogicalID = ulPhysicalIndex + XML_ARRAY_BASE_ID;
    return ulLogicalID;
}

void CXMLArray::PushUniqueID(std::uint32_t ulLogicalID)
{
    // Map to index
    std::uint32_t ulPhysicalIndex = ulLogicalID - XML_ARRAY_BASE_ID;

    // Checks
    assert((ulLogicalID != INVALID_XML_ID) && (ulPhysicalIndex <= m_ulCapacity) && (m_Elements[ulPhysicalIndex]));

    m_IDStack.Push(ulPhysicalIndex);
    m_Elements[ulPhysicalIndex] = NULL;
}

void CXMLArray::PushUniqueID(CXMLCommon* pEntry)
{
    PushUniqueID(pEntry->GetID());
}

CXMLCommon* CXMLArray::GetEntry(std::uint32_t ulLogicalID)
{
    // Return the element with the given ID

    // Map to index
    std::uint32_t ulPhysicalIndex = ulLogicalID - XML_ARRAY_BASE_ID;

    if (ulLogicalID != INVALID_XML_ID && ulPhysicalIndex <= m_ulCapacity)
    {
        return m_Elements[ulPhysicalIndex];
    }
    return NULL;
}

void CXMLArray::ExpandBy(std::uint32_t ulAmount)
{
    m_IDStack.ExpandBy(ulAmount);
    m_Elements.resize(m_ulCapacity + ulAmount + 1, NULL);
    m_ulCapacity += ulAmount;
    assert(m_IDStack.GetCapacity() == m_ulCapacity);
}

std::uint32_t CXMLArray::GetCapacity()
{
    return m_ulCapacity;
}

std::uint32_t CXMLArray::GetUnusedAmount()
{
    return m_IDStack.GetUnusedAmount();
}
