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
    CFastHashMap<SString, SCustomData>::const_iterator iter = pCustomData->IterBegin();
    for (; iter != pCustomData->IterEnd(); iter++)
    {
        Set(iter->first, iter->second.Variable);
    }
}

SCustomData* CCustomData::Get(const char* szName)
{
    CFastHashMap<SString, SCustomData>::iterator it = m_Data.find(szName);
    if (it != m_Data.end())
        return &it->second;

    return NULL;
}

bool CCustomData::Set(const char* szName, const CLuaArgument& Variable, bool bSynchronized, CLuaArgument* pOldVariable)
{
    // Grab the item with the given name
    SCustomData* pData = MapFind(m_Data, szName);
    if (pData)
    {   
        if (pData->Variable != Variable || pData->bSynchronized != bSynchronized)
        {
            // Set old variable if needed
            if (pOldVariable)
                *pOldVariable = pData->Variable;

            // Update existing
            pData->Variable = Variable;
            pData->bSynchronized = bSynchronized;

            return true;
        }
    }
    else
    {
        // Add new
        SCustomData newData;
        newData.Variable = Variable;
        newData.bSynchronized = bSynchronized;
        m_Data[szName] = newData;

        return true;
    }
    return false;
}

bool CCustomData::Delete(const char* szName)
{
    // Find the item and delete it
    CFastHashMap<SString, SCustomData>::iterator it = m_Data.find(szName);
    if (it != m_Data.end())
    {
        m_Data.erase(it);
        return true;
    }

    // Didn't exist
    return false;
}
