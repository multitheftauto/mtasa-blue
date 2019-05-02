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

void CCustomData::Copy(CCustomData* pCustomData)
{
    CFastHashMap<SString, SCustomData>::const_iterator iter = pCustomData->IterBegin();
    for (; iter != pCustomData->IterEnd(); iter++)
        Set(iter->first, iter->second.Variable);
}

SCustomData* CCustomData::Get(const char* szName)
{
    assert(szName);

    return MapFind(m_Data, szName);
}

SCustomData* CCustomData::GetSynced(const char* szName)
{
    assert(szName);

    return MapFind(m_SyncedData, szName);
}

bool CCustomData::DeleteSynced(const char* szName)
{
    // Find the item and delete it
    CFastHashMap<SString, SCustomData>::iterator iter = m_SyncedData.find(szName);
    if (iter != m_SyncedData.end())
    {
        m_SyncedData.erase(iter);
        return true;
    }

    // Didn't exist
    return false;
}

void CCustomData::UpdateSynced(const char* szName, const CLuaArgument& Variable, bool bSynchronized)
{
    if (bSynchronized)
    {
        SCustomData* pDataSynced = MapFind(m_SyncedData, szName);
        if (pDataSynced)
        {
            pDataSynced->Variable = Variable;
            pDataSynced->bSynchronized = bSynchronized;
        }
        else
        {
            SCustomData newData;
            newData.Variable = Variable;
            newData.bSynchronized = bSynchronized;
            m_SyncedData[szName] = newData;
        }
    }
    else
        DeleteSynced(szName);
}

// Returns false in case we try to set the same value for the custom data and we didn't change bSync, otherwise true
bool CCustomData::Set(const char* szName, const CLuaArgument& Variable, bool bSynchronized, CLuaArgument* pOldVariable)
{
    assert(szName);

    // Grab the item with the given name
    SCustomData* pData = MapFind(m_Data, szName);
    if (pData)
    {
        if (pData->bSynchronized != bSynchronized || pData->Variable != Variable)
        {
            // Set the old variable(its used by the onElementDataChange event)
            if (pOldVariable)
            * pOldVariable = pData->Variable;
            // Update existing
            pData->Variable = Variable;
            pData->bSynchronized = bSynchronized;
            UpdateSynced(szName, Variable, bSynchronized);

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
        UpdateSynced(szName, Variable, bSynchronized);

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
        DeleteSynced(szName);
        m_Data.erase(it);
        return true;
    }

    // Didn't exist
    return false;
}

CXMLNode* CCustomData::OutputToXML(CXMLNode* pNode)
{
    CFastHashMap<SString, SCustomData>::const_iterator iter = m_Data.begin();
    for (; iter != m_Data.end(); iter++)
    {
        CLuaArgument* arg = (CLuaArgument*)& iter->second.Variable;

        switch (arg->GetType())
        {
        case LUA_TSTRING:
        {
            CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first);
            attr->SetValue(arg->GetString().c_str());
            break;
        }
        case LUA_TNUMBER:
        {
            CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first);
            attr->SetValue((float)arg->GetNumber());
            break;
        }
        case LUA_TBOOLEAN:
        {
            CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first);
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
