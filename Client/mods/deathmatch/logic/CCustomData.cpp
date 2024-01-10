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
    for (const auto& [key, data] : pCustomData->GetData())
        Set(key, data.Variable);    
}

SCustomData* CCustomData::Get(const SString& strName, bool bCreate)
{
    if (auto it = m_Data.find(strName); it != m_Data.end())
        return &it->second;

    if (bCreate)
        return &m_Data[strName];

    return {};
}

bool CCustomData::Set(const SString& strName, const CLuaArgument& Variable, bool bSynchronized, SCustomData* pOldData)
{
    if (strName.length() > MAX_CUSTOMDATA_NAME_LENGTH)
    {
        // Don't allow it to be set if the name is too long
        CLogger::ErrorPrintf("Custom data name too long (%s)", *strName.Left(MAX_CUSTOMDATA_NAME_LENGTH + 1));
        return false;
    }

    SCustomData* pCurrentVariable = Get(strName, true);
    assert(pCurrentVariable);

    if (pCurrentVariable->Variable.IsEmpty() || pCurrentVariable->Variable != Variable || pCurrentVariable->bSynchronized != bSynchronized)
    {
        // Save the old variable
        if (pOldData)
            *pOldData = *pCurrentVariable;

        // Set the new data
        pCurrentVariable->Variable = Variable;
        pCurrentVariable->bSynchronized = bSynchronized;  

        return true;
    }   

    return false;  
}

bool CCustomData::Delete(const SString& strName, SCustomData* pOldData)
{
    // Find the item and delete it
    auto it = m_Data.find(strName);
    if (it != m_Data.end())
    {
        if (pOldData)
            *pOldData = it->second;

        m_Data.erase(it);
        return true;
    }

    // Didn't exist
    return false;
}
