/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CCustomData.cpp
 *  PURPOSE:     Custom data storage class
 *
 *****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CCustomData
#include "profiler/SharedUtil.Profiler.h"

void CCustomData::Copy(CCustomData* pCustomData)
{
    auto iter = pCustomData->IterBegin();
    for (; iter != pCustomData->IterEnd(); iter++)
    {
        Set(iter->first, iter->second.Variable);
    }
}

SCustomData* CCustomData::Get(CStringName name)
{
    assert(name);

    auto it = m_Data.find(name);
    if (it != m_Data.end())
        return &it->second;

    return NULL;
}

void CCustomData::Set(CStringName name, const CLuaArgument& Variable, bool bSynchronized)
{
    assert(name);

    // Grab the item with the given name
    SCustomData* pData = Get(name);
    if (pData)
    {
        // Update existing
        pData->Variable = Variable;
        pData->bSynchronized = bSynchronized;
    }
    else
    {
        // Add new
        SCustomData newData;
        newData.Variable = Variable;
        newData.bSynchronized = bSynchronized;
        m_Data[name] = newData;
    }
}

bool CCustomData::Delete(CStringName name)
{
    // Find the item and delete it
    auto it = m_Data.find(name);
    if (it != m_Data.end())
    {
        m_Data.erase(it);
        return true;
    }

    // Didn't exist
    return false;
}
