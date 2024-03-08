/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCustomData.cpp
 *  PURPOSE:     Custom entity data class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCustomData.h"

void CCustomData::Copy(CCustomData* pCustomData)
{
    for (const auto& [key, data] : pCustomData->GetData())
        Set(SString(key), CLuaArgument(data.Variable));
}

SCustomData* CCustomData::Get(const SString& strName, bool bCreate)
{
    auto it = m_Data.find(strName);
    if (it != m_Data.end())
        return &it->second;

    if (bCreate)
        return &m_Data[strName];

    return {};
}

SCustomData* CCustomData::GetSynced(const SString& strName)
{
    auto it = m_SyncedData.find(strName);
    if (it != m_SyncedData.end())
        return &it->second;

    return {};
}

bool CCustomData::DeleteSynced(const SString& strName)
{
    // Find the item and delete it
    auto iter = m_SyncedData.find(strName);
    if (iter != m_SyncedData.end())
    {
        m_SyncedData.erase(iter);
        return true;
    }

    // Didn't exist
    return false;
}

void CCustomData::UpdateSynced(const SString& strName, const CLuaArgument& Variable, ESyncType syncType)
{
    if (syncType == ESyncType::BROADCAST)
    {
        SCustomData* pDataSynced = GetSynced(strName);
        if (pDataSynced)
        {
            pDataSynced->Variable = Variable;
            pDataSynced->syncType = syncType;
        }
        else
        {
            SCustomData newData;
            newData.Variable = Variable;
            newData.syncType = syncType;
            m_SyncedData[strName] = newData;
        }
    }
    else
    {
        DeleteSynced(strName);
    }
}

SCustomDataResult CCustomData::Set(SString&& strName, CLuaArgument&& Variable, ESyncType syncType, SCustomData* pOldData)
{
    if (strName.length() > MAX_CUSTOMDATA_NAME_LENGTH)
        return {};

    auto iter = m_Data.try_emplace(std::move(strName)).first;
    SCustomData& pCurrentVariable = iter->second;

    if (pCurrentVariable.Variable.IsEmpty() || pCurrentVariable.syncType != syncType || pCurrentVariable.Variable != Variable)
    {
        if (syncType == ESyncType::PERSISTENT)
            syncType = pCurrentVariable.syncType;

        // Save the old variable
        if (pOldData)
            *pOldData = std::move(pCurrentVariable);        

        // Set the new data
        pCurrentVariable.Variable = std::move(Variable);        
        pCurrentVariable.syncType = syncType;
        UpdateSynced(strName, Variable, syncType);

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

        DeleteSynced(strName);
        m_Data.erase(it);
        return true;
    }

    // Didn't exist
    return false;
}

CXMLNode* CCustomData::OutputToXML(CXMLNode* pNode)
{
    for (const auto& [key, data] : m_Data)
    {
        const CLuaArgument& arg = data.Variable;

        switch (arg.GetType())
        {
            case LUA_TSTRING:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(key.c_str());
                attr->SetValue(arg.GetString().c_str());
                break;
            }
            case LUA_TNUMBER:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(key.c_str());
                attr->SetValue((float)arg.GetNumber());
                break;
            }
            case LUA_TBOOLEAN:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(key.c_str());
                attr->SetValue(arg.GetBoolean());
                break;
            }
        }
    }
    
    return pNode;
}
