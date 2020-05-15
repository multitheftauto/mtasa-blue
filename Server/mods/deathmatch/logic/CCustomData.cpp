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


void CCustomData::Copy(const CCustomData* const from)
{
    m_localOrSubMap = from->m_localOrSubMap;
    m_broadcastedMap = from->m_broadcastedMap;
}

SCustomData* CCustomData::Get(const SString& name, element_data_iter* const outIter)
{
    element_data_iter iter = m_localOrSubMap.find(name);
    if (iter == m_localOrSubMap.end())
    {
        iter = m_broadcastedMap.find(name);
        if (iter == m_broadcastedMap.end())
            return nullptr;
    }

    if (outIter)
        *outIter = iter;

    return &iter->second;
}

SCustomData* CCustomData::Get(const SString& name, const ESyncType syncType, element_data_iter* const outIter)
{
    element_data_iter iter;
    if (syncType == ESyncType::BROADCAST)
    {
        iter = m_broadcastedMap.find(name);
        if (iter == m_broadcastedMap.end())
            return nullptr;
    }
    else
    {
        iter = m_localOrSubMap.find(name);
        if (iter == m_localOrSubMap.end())
            return nullptr;
    }

    if (outIter)
        *outIter = iter;

    return &iter->second;
}

void CCustomData::Set(const SString& name, const CLuaArgument& var, const ESyncType syncType, SCustomData* const oldData)
{
    element_data_iter dataIter;
    SCustomData* data = Get(name, &dataIter);

    if (data)
    {
        if (data->syncType != syncType)
        {
            // here we can std::move, because it'll be deleted when erased from the map.
            if (oldData)
                *oldData = std::move(*data);

            if (syncType == ESyncType::BROADCAST)
            {
                // first insert, then erase, because the iterator is invalidated after erasing
                m_broadcastedMap.insert(std::move(*dataIter));
                m_localOrSubMap.erase(dataIter);
            }
            else
            {
                // first insert, then erase, because the iterator is invalidated after erasing
                m_localOrSubMap.insert(std::move(*dataIter));
                m_broadcastedMap.erase(dataIter);
            }
            data->syncType = syncType;
        }
        else if (oldData)
        {
            *oldData = *data;
        }

        data->variable = var;
    }
    else
    {
        if (syncType == ESyncType::BROADCAST)
            m_broadcastedMap[name] = SCustomData(var, syncType);
        else
            m_localOrSubMap[name] = SCustomData(var, syncType);
    }
}

bool CCustomData::Delete(const SString& name)
{
    element_data_iter iter;
    const SCustomData* const data = Get(name, &iter);
    if (data)
    {
        if (data->syncType == ESyncType::BROADCAST)
            m_broadcastedMap.erase(iter);
        else
            m_localOrSubMap.erase(iter);

        return true;
    }
    return false;
}

bool CCustomData::Delete(const SString& name, const ESyncType syncType)
{
    element_data_iter iter;
    const SCustomData* const data = Get(name, syncType, &iter);
    if (data)
    {
        if (data->syncType == ESyncType::BROADCAST)
            m_broadcastedMap.erase(iter);
        else
            m_localOrSubMap.erase(iter);

        return true;
    }
    return false;
}

CXMLNode* CCustomData::OutputToXML(CXMLNode* pNode)
{
    element_data_map& map = m_localOrSubMap;

// hack, so we dont need to copy paste code for the other map
redo:

    for (const auto& pair : map)
    {
        const CLuaArgument& arg = pair.second.variable;
        switch (arg.GetType())
        {
            case LUA_TSTRING:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(pair.first.c_str());
                attr->SetValue(arg.GetString().c_str());
                break;
            }
            case LUA_TNUMBER:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(pair.first.c_str());

                attr->SetValue((float)arg.GetNumber());
                break;
            }
            case LUA_TBOOLEAN:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(pair.first.c_str());
                attr->SetValue(arg.GetBoolean());
                break;
            }
        }
    }

    if (&map == &m_localOrSubMap)
    {
        map = m_broadcastedMap;
        goto redo;
    }

    return pNode;
}
