/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLArrayImpl.cpp
 *  PURPOSE:     XML array class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

CStack<unsigned long, 1> CXMLArray::m_IDStack;
std::vector<CXMLCommon*> CXMLArray::m_elements;
unsigned long            CXMLArray::m_capacity = 0;

#define XML_ARRAY_BASE_ID 0x01000000

void CXMLArray::Initialize()
{
    m_capacity = 0;
    ExpandBy(20000);
}

unsigned long CXMLArray::PopUniqueID(CXMLCommon* entry)
{
    // Add more ID's if required
    if (m_IDStack.GetUnusedAmount() < 10000)
        ExpandBy(10000);

    // Grab the next unused ID
    unsigned long physicalIndex;
    bool          success = m_IDStack.Pop(physicalIndex);
    assert(success);

    // Checks
    assert((physicalIndex != INVALID_XML_ID) && (physicalIndex <= m_capacity) && (m_elements.size() > physicalIndex) &&
           (m_elements[physicalIndex] == nullptr));

    m_elements[physicalIndex] = entry;

    // Map to ID
    unsigned long logicalID = physicalIndex + XML_ARRAY_BASE_ID;
    return logicalID;
}

void CXMLArray::PushUniqueID(unsigned long logicalID)
{
    // Map to index
    unsigned long physicalIndex = logicalID - XML_ARRAY_BASE_ID;

    // Checks
    assert((logicalID != INVALID_XML_ID) && (physicalIndex <= m_capacity) && (m_elements[physicalIndex]));

    m_IDStack.Push(physicalIndex);
    m_elements[physicalIndex] = nullptr;
}

void CXMLArray::PushUniqueID(CXMLCommon* entry)
{
    PushUniqueID(entry->GetID());
}

CXMLCommon* CXMLArray::GetEntry(unsigned long logicalID)
{
    // Return the element with the given ID

    // Map to index
    unsigned long physicalIndex = logicalID - XML_ARRAY_BASE_ID;

    if (logicalID != INVALID_XML_ID && physicalIndex <= m_capacity)
        return m_elements[physicalIndex];

    return NULL;
}

void CXMLArray::ExpandBy(unsigned long amount)
{
    m_IDStack.ExpandBy(amount);
    m_elements.resize(m_capacity + amount + 1, nullptr);
    m_capacity += amount;
    assert(m_IDStack.GetCapacity() == m_capacity);
}

unsigned long CXMLArray::GetCapacity()
{
    return m_capacity;
}

unsigned long CXMLArray::GetUnusedAmount()
{
    return m_IDStack.GetUnusedAmount();
}
