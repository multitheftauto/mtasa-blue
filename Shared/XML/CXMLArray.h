/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLArrayImpl.h
 *  PURPOSE:     XML array class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <xml/CXMLCommon.h>

class CXMLArray
{
public:
    static void Initialize();

    static unsigned long PopUniqueID(CXMLCommon* entry);
    static void          PushUniqueID(CXMLCommon* entry);

    static CXMLCommon*   GetEntry(unsigned long ID);
    static unsigned long GetCapacity();
    static unsigned long GetUnusedAmount();

private:
    static void ExpandBy(unsigned long amount);
    static void PushUniqueID(unsigned long ID);

    static CStack<unsigned long, 1> m_IDStack;
    static std::vector<CXMLCommon*> m_elements;
    static unsigned long            m_capacity;
};
