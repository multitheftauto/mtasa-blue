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
    element_data_iter iter = m_broadcastedMap.find(name);
    if (iter == m_broadcastedMap.end())
    {
        iter = m_localOrSubMap.find(name);
        if (iter == m_localOrSubMap.end())
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

const SCustomData* CCustomData::Set(const SString& name, const CLuaArgument& newValue, const ESyncType newSyncType, SCustomData* const oldData)
{
    element_data_iter dataIter;
    SCustomData* data = Get(name, &dataIter);

    if (data)
    {
        bool hasChanged = false;

        if (data->variable != newValue)
        {
            if (oldData)
                *oldData = *data;

            data->variable = newValue;
            hasChanged = true;
        }

        if (data->syncType != newSyncType)
        {
            if (!hasChanged && oldData)
                *oldData = *data;

            if (newSyncType == ESyncType::BROADCAST)
            {
                // first insert, then erase, because the iterator is invalidated after erasing
                data = &m_broadcastedMap.insert(std::move(*dataIter)).first->second;
                m_localOrSubMap.erase(dataIter);
            }
            else
            {
                // first insert, then erase, because the iterator is invalidated after erasing
                data = &m_localOrSubMap.insert(std::move(*dataIter)).first->second;
                m_broadcastedMap.erase(dataIter);
            }

            data->syncType = newSyncType;

            return data;
        }
        return hasChanged ? data : nullptr;
    }
    else
    {
        auto& mapToInsertInto = (newSyncType == ESyncType::BROADCAST) ? m_broadcastedMap : m_localOrSubMap;
        const auto& insertResult = mapToInsertInto.insert(std::make_pair(name, SCustomData(newValue, newSyncType)));

        return insertResult.second ? &insertResult.first->second : nullptr;
    }
}

// same as the above, but syncType remains the same. (if data not found, then: syncType = ESyncType::BROADCAST)
const SCustomData* CCustomData::Set(const SString& name, const CLuaArgument& newValue, SCustomData* const oldData)
{
    element_data_iter dataIter;
    SCustomData* const data = Get(name, &dataIter);

    if (data)
    {
        if (data->variable != newValue)
        {
            if (oldData)
                *oldData = *data;

            data->variable = newValue;
            return data;
        }
        return nullptr;
    }
    else
    {
        // this returns a pointer to the inserted SCustomData.
        return &m_broadcastedMap.insert(std::make_pair(name, SCustomData(newValue, ESyncType::BROADCAST))).first->second;
    }
}

bool CCustomData::Delete(const SString& name, SCustomData* const oldData)
{
    element_data_iter iter;
    SCustomData* const data = Get(name, &iter);
    if (data)
    {
        if (oldData)
            *oldData = std::move(*data);

        if (data->syncType == ESyncType::BROADCAST)
            m_broadcastedMap.erase(iter);
        else
            m_localOrSubMap.erase(iter);

        return true;
    }
    return false;
}

bool CCustomData::Delete(const SString& name, const ESyncType syncType, SCustomData* const oldData)
{
    element_data_iter iter;
    SCustomData* const data = Get(name, syncType, &iter);
    if (data)
    {
        if (oldData)
            *oldData = std::move(*data);

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
