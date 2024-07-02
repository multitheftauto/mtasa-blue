/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCustomData.h
 *  PURPOSE:     Custom entity data class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CServerInterface.h>
#include "lua/CLuaArgument.h"
#include <map>
#include <string>

#define MAX_CUSTOMDATA_NAME_LENGTH 128

enum class ESyncType
{
    LOCAL,
    BROADCAST,
    SUBSCRIBE,
    PERSISTENT
};

struct SCustomData
{
    CLuaArgument Variable;
    ESyncType    syncType{ESyncType::BROADCAST};
};

struct SCustomDataResult
{
    using iterator = std::unordered_map<SString, SCustomData>::iterator;

    SCustomDataResult() = default;

    SCustomDataResult(const iterator& iter) :
        Iter(iter),
        bValid(true)
    {
    }

    const SString& GetName() const { return Iter->first; }
    const SCustomData& GetData() const { return Iter->second; }

    operator bool() const { return bValid; }

    iterator Iter;
    bool bValid{};
};

class CCustomData
{
public:
    void Copy(CCustomData* pCustomData);

    SCustomData* Get(const SString& strName, bool bCreate = false);
    SCustomData* GetSynced(const SString& strName);
    SCustomDataResult Set(SString&& strName, CLuaArgument&& Variable, ESyncType syncType = ESyncType::BROADCAST, SCustomData* pOldData = {});

    bool Delete(const SString& strName, SCustomData* pOldData = {});

    CXMLNode* OutputToXML(CXMLNode* pNode);

    const std::unordered_map<SString, SCustomData>& GetData() const { return m_Data; }
    const std::unordered_map<SString, SCustomData>& GetSyncedData() const { return m_SyncedData; }

private:
    bool DeleteSynced(const SString& strName);
    void UpdateSynced(const SString& strName, const CLuaArgument& Variable, ESyncType syncType);

    std::unordered_map<SString, SCustomData> m_Data;
    std::unordered_map<SString, SCustomData> m_SyncedData;
};
