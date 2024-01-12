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
        Set(SString(key), CLuaArgument(data.Variable));    
}

const SCustomData* CCustomData::Get(const SString& strName)
{
    if (auto it = m_Data.find(strName); it != m_Data.end())
        return &it->second;

    return {};
}

SCustomDataResult CCustomData::Set(SString&& strName, CLuaArgument&& Variable, bool bSynchronized, SCustomData* oldValue)
{
    if (strName.length() > MAX_CUSTOMDATA_NAME_LENGTH)
    {
        // Don't allow it to be set if the name is too long
        CLogger::ErrorPrintf("Custom data name too long (%s)", *strName.Left(MAX_CUSTOMDATA_NAME_LENGTH + 1));
        return {};
    }

    auto iter = m_Data.try_emplace(std::move(strName)).first;
    SCustomData& pCurrentVariable = iter->second;

    if (pCurrentVariable.Variable.IsEmpty() || pCurrentVariable.bSynchronized != bSynchronized || pCurrentVariable.Variable != Variable )
    {      
        // Save the old variable
        if (oldValue)
            *oldValue = std::move(pCurrentVariable);

        // Set the new data
        pCurrentVariable.Variable = std::move(Variable);
        pCurrentVariable.bSynchronized = bSynchronized;

        return SCustomDataResult(iter);
    }   

    return {};  
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
