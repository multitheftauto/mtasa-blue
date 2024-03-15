/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLArrayImpl.h
 *  PURPOSE:     XML array class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CXMLCommon;

class CXMLArray
{
public:
    static void Initialize();

    static std::uint32_t PopUniqueID(CXMLCommon* pEntry);
    static void          PushUniqueID(CXMLCommon* pEntry);

    static CXMLCommon*   GetEntry(std::uint32_t ulID);
    static std::uint32_t GetCapacity();
    static std::uint32_t GetUnusedAmount();

private:
    static void ExpandBy(std::uint32_t ulAmount);
    static void PushUniqueID(std::uint32_t ulID);

    static CStack<std::uint32_t, 1> m_IDStack;
    static std::vector<CXMLCommon*> m_Elements;
    static std::uint32_t            m_ulCapacity;
};
