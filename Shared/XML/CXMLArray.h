/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLArrayImpl.h
 *  PURPOSE:     XML array class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CXMLCommon;

class CXMLArray
{
public:
    static void Initialize();

    static unsigned long PopUniqueID(CXMLCommon* pEntry);
    static void          PushUniqueID(CXMLCommon* pEntry);

    static CXMLCommon*   GetEntry(unsigned long ulID);
    static unsigned long GetCapacity();
    static unsigned long GetUnusedAmount();

private:
    static void ExpandBy(unsigned long ulAmount);
    static void PushUniqueID(unsigned long ulID);

    static CStack<unsigned long, 1> m_IDStack;
    static std::vector<CXMLCommon*> m_Elements;
    static unsigned long            m_ulCapacity;
};
