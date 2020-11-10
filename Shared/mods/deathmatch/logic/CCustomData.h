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

#ifdef MTA_CLIENT
    #include "lua/CLuaArgument.h"
#else
    #include <core/CServerInterface.h>
    #include "lua/CLuaArgument.h"
    #include "SharedUtil.FastHashMap.h"
    #include <string>
#endif
#define MAX_CUSTOMDATA_NAME_LENGTH 128

enum class ESyncType : unsigned char
{
    LOCAL,
    BROADCAST,
    SUBSCRIBE,
};

#ifdef MTA_CLIENT
struct SCustomData
{
    CLuaArgument Variable;
    bool         bSynchronized;
};
#else
struct SCustomData
{
    CLuaArgument Variable;
    ESyncType    syncType;
};
#endif

class CCustomData
{
public:
    SCustomData* Get(const std::string& name) { return MapFind(m_Data, name); }

#ifdef MTA_CLIENT
    void Set(const std::string& name, const CLuaArgument& Variable, bool bSynchronized = true)
    {
        auto& data = m_Data[name];
        data.Variable = Variable;
        data.bSynchronized = bSynchronized;
    }
#else
    void Set(const std::string& name, const CLuaArgument& Variable, ESyncType syncType = ESyncType::BROADCAST)
    {
        auto& data = m_Data[name];
        data.Variable = Variable;
        data.syncType = syncType;
    }

    // Server only
    CXMLNode* OutputToXML(CXMLNode* pNode);
#endif

    // Shared functions
    const size_t Count() const noexcept { return m_Data.size(); }
    const size_t CountSynced() const
    {
        return std::count_if(m_Data.begin(), m_Data.end(), [](const auto& kvpair) {
#ifdef MTA_CLIENT
            return kvpair.second.bSynchronized;
#else
            return kvpair.second.syncType != ESyncType::LOCAL;
#endif
        });
    }
    
    bool Delete(const std::string& name) { return (bool)m_Data.erase(name); }
    const auto& GetAll() const noexcept { return m_Data; }
private:
    SharedUtil::CFastHashMap<std::string, SCustomData> m_Data;
};
