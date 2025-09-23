/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCustomData.cpp
 *  PURPOSE:     Custom entity data class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCustomData.h"

void CCustomData::Copy(CCustomData* pCustomData)
{
    auto iter = pCustomData->IterBegin();
    for (; iter != pCustomData->IterEnd(); iter++)
    {
        Set(iter->first, iter->second.Variable);
    }
}

SCustomData* CCustomData::Get(CStringName name) const
{
    assert(name);

    auto it = m_Data.find(name);
    if (it != m_Data.end())
        return (SCustomData*)&it->second;

    return NULL;
}

SCustomData* CCustomData::GetSynced(CStringName name)
{
    assert(name);

    auto it = m_SyncedData.find(name);
    if (it != m_SyncedData.end())
        return (SCustomData*)&it->second;

    return NULL;
}

bool CCustomData::DeleteSynced(CStringName name)
{
    // Find the item and delete it
    auto iter = m_SyncedData.find(name);
    if (iter != m_SyncedData.end())
    {
        m_SyncedData.erase(iter);
        return true;
    }

    // Didn't exist
    return false;
}

void CCustomData::UpdateSynced(CStringName name, const CLuaArgument& Variable, ESyncType syncType)
{
    if (syncType == ESyncType::BROADCAST)
    {
        SCustomData* pDataSynced = GetSynced(name);
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
            m_SyncedData[name] = newData;
        }
    }
    else
    {
        DeleteSynced(name);
    }
}

void CCustomData::Set(CStringName name, const CLuaArgument& Variable, ESyncType syncType)
{
    assert(name);

    // Grab the item with the given name
    SCustomData* pData = Get(name);
    if (pData)
    {
        // Update existing
        pData->Variable = Variable;
        pData->syncType = syncType;
        UpdateSynced(name, Variable, syncType);
    }
    else
    {
        // Add new
        SCustomData newData;
        newData.Variable = Variable;
        newData.syncType = syncType;
        newData.clientChangesMode = eCustomDataClientTrust::UNSET;
        m_Data[name] = newData;
        UpdateSynced(name, Variable, syncType);
    }
}

bool CCustomData::Delete(CStringName name)
{
    // Find the item and delete it
    auto it = m_Data.find(name);
    if (it != m_Data.end())
    {
        DeleteSynced(name);
        m_Data.erase(it);
        return true;
    }

    // Didn't exist
    return false;
}

void CCustomData::SetClientChangesMode(CStringName name, eCustomDataClientTrust mode)
{
    SCustomData& pData = m_Data[name];
    pData.clientChangesMode = mode;
}

CXMLNode* CCustomData::OutputToXML(CXMLNode* pNode)
{
    auto iter = m_Data.begin();
    for (; iter != m_Data.end(); iter++)
    {
        CLuaArgument* arg = (CLuaArgument*)&iter->second.Variable;

        switch (arg->GetType())
        {
            case LUA_TSTRING:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.ToCString());
                attr->SetValue(arg->GetString().c_str());
                break;
            }
            case LUA_TNUMBER:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.ToCString());
                attr->SetValue((float)arg->GetNumber());
                break;
            }
            case LUA_TBOOLEAN:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.ToCString());
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
