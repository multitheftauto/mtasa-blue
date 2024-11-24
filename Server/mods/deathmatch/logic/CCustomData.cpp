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
    std::map<std::string, SCustomData>::const_iterator iter = pCustomData->IterBegin();
    for (; iter != pCustomData->IterEnd(); iter++)
    {
        Set(iter->first.c_str(), iter->second.Variable);
    }
}

SCustomData* CCustomData::Get(const char* szName) const
{
    assert(szName);

    std::map<std::string, SCustomData>::const_iterator it = m_Data.find(szName);
    if (it != m_Data.end())
        return (SCustomData*)&it->second;

    return NULL;
}

SCustomData* CCustomData::GetSynced(const char* szName)
{
    assert(szName);

    std::map<std::string, SCustomData>::const_iterator it = m_SyncedData.find(szName);
    if (it != m_SyncedData.end())
        return (SCustomData*)&it->second;

    return NULL;
}

bool CCustomData::DeleteSynced(const char* szName)
{
    // Find the item and delete it
    std::map<std::string, SCustomData>::iterator iter = m_SyncedData.find(szName);
    if (iter != m_SyncedData.end())
    {
        m_SyncedData.erase(iter);
        return true;
    }

    // Didn't exist
    return false;
}

void CCustomData::UpdateSynced(const char* szName, const CLuaArgument& Variable, ESyncType syncType)
{
    if (syncType == ESyncType::BROADCAST)
    {
        SCustomData* pDataSynced = GetSynced(szName);
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
            m_SyncedData[szName] = newData;
        }
    }
    else
    {
        DeleteSynced(szName);
    }
}

void CCustomData::Set(const char* szName, const CLuaArgument& Variable, ESyncType syncType)
{
    assert(szName);

    // Grab the item with the given name
    SCustomData* pData = Get(szName);
    if (pData)
    {
        // Update existing
        pData->Variable = Variable;
        pData->syncType = syncType;
        UpdateSynced(szName, Variable, syncType);
    }
    else
    {
        // Add new
        SCustomData newData;
        newData.Variable = Variable;
        newData.syncType = syncType;
        newData.clientChangesMode = eCustomDataClientTrust::UNSET;
        m_Data[szName] = newData;
        UpdateSynced(szName, Variable, syncType);
    }
}

bool CCustomData::Delete(const char* szName)
{
    // Find the item and delete it
    std::map<std::string, SCustomData>::iterator it = m_Data.find(szName);
    if (it != m_Data.end())
    {
        DeleteSynced(szName);
        m_Data.erase(it);
        return true;
    }

    // Didn't exist
    return false;
}

void CCustomData::SetClientChangesMode(const char* szName, eCustomDataClientTrust mode)
{
    SCustomData& pData = m_Data[szName];
    pData.clientChangesMode = mode;
}

CXMLNode* CCustomData::OutputToXML(CXMLNode* pNode)
{
    std::map<std::string, SCustomData>::const_iterator iter = m_Data.begin();
    for (; iter != m_Data.end(); iter++)
    {
        CLuaArgument* arg = (CLuaArgument*)&iter->second.Variable;

        switch (arg->GetType())
        {
            case LUA_TSTRING:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.c_str());
                attr->SetValue(arg->GetString().c_str());
                break;
            }
            case LUA_TNUMBER:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.c_str());
                attr->SetValue((float)arg->GetNumber());
                break;
            }
            case LUA_TBOOLEAN:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.c_str());
                attr->SetValue(arg->GetBoolean());
                break;
            }
        }
    }
    return pNode;
}

unsigned short CCustomData::CountOnlySynchronized()
{
    return static_cast<unsigned short>(m_SyncedData.size());
}
