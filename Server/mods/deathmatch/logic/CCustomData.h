<<<<<<< HEAD
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

#include <StdInc.h>
#include <core/CServerInterface.h>
#include "lua/CLuaArgument.h"

#define MAX_CUSTOMDATA_NAME_LENGTH 128


enum class ESyncType
{
    LOCAL,
    BROADCAST,
    SUBSCRIBE,
};

struct SCustomData
{
    CLuaArgument variable;
    ESyncType    syncType;

    SCustomData() = default;

    SCustomData(const CLuaArgument& var, const ESyncType syncType) noexcept :
        variable(var),
        syncType(syncType)
    {
    }

    SCustomData(const SCustomData& rhs) noexcept :
        variable(rhs.variable),
        syncType(rhs.syncType)
    {
    }

    SCustomData(SCustomData&& rhs) noexcept :
        variable(std::move(rhs.variable)),
        syncType(rhs.syncType)
    {
    }

    SCustomData& operator=(const SCustomData& rhs) noexcept
    {
        if (this == &rhs)
            return *this;

        variable = rhs.variable;
        syncType = rhs.syncType;
    }

    SCustomData& operator=(SCustomData&& rhs) noexcept
    {
        if (this == &rhs)
            return *this;

        variable = std::move(rhs.variable);
        syncType = rhs.syncType;
    }
};

// doing 'using' here, because of SCustomData
using element_data_map = CFastHashMap<std::string, SCustomData>;
using element_data_iter = element_data_map::iterator;
using element_data_const_iter = element_data_map::const_iterator;

class CCustomData
{
public:
    void Copy(const CCustomData* const from);

    const SCustomData* Get(const SString& name) { return Get(name); };
    const SCustomData* Get(const SString& name, const ESyncType syncType) { return Get(name, syncType); } 

    void                Set(const SString& name, const CLuaArgument& var, const ESyncType syncType = ESyncType::BROADCAST, SCustomData* const oldData = nullptr);

    bool Delete(const SString& name);
    bool Delete(const SString& name, const ESyncType syncType);

    unsigned int GetBroadcastedCount() const { return m_broadcastedMap.size(); }

    CXMLNode* OutputToXML(CXMLNode* node);

    element_data_const_iter LocalOrSubDataBegin() const { return m_localOrSubMap.begin(); }
    element_data_const_iter LocalOrSubDataEnd() const { return m_localOrSubMap.end(); }

    element_data_const_iter BroadcastedBegin() const { return m_broadcastedMap.begin(); }
    element_data_const_iter BroadcastedEnd() const { return m_broadcastedMap.end(); }

private:
    // outIter used for returning the position of the data in the appropriate map(m_broadcastedMap if the data's type is BROADCAST, otherwise m_localOrSubMap)
    SCustomData* Get(const SString& name, element_data_iter* const outIter);
    // This version is used to speed up the search, because we dont need to search two maps, just one. Useful when we know its syncType
    SCustomData* Get(const SString& name, const ESyncType syncType, element_data_iter* const outIter);
        
    // ACHTUNG(WARNING): no key can be in both maps at the same time.
    element_data_map m_localOrSubMap; // local, and subscribed edata
    element_data_map m_broadcastedMap; // broadcasted edata
};
=======
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
#include "SharedUtil.FastHashMap.h"

#define MAX_CUSTOMDATA_NAME_LENGTH 128

struct SCustomData
{
    CLuaArgument Variable;
    bool         bSynchronized;
};

class CCustomData
{
public:
    void Copy(CCustomData* pCustomData);

    SCustomData* Get(const char* szName);
    SCustomData* GetSynced(const char* szName);
    bool         Set(const char* szName, const CLuaArgument& Variable, bool bSynchronized = true, CLuaArgument* pOldVariable = nullptr);

    bool Delete(const char* szName);

    unsigned short CountOnlySynchronized();

    CXMLNode* OutputToXML(CXMLNode* pNode);

    CFastHashMap<SString, SCustomData>::const_iterator IterBegin() { return m_Data.begin(); }
    CFastHashMap<SString, SCustomData>::const_iterator IterEnd() { return m_Data.end(); }

    CFastHashMap<SString, SCustomData>::const_iterator SyncedIterBegin() { return m_SyncedData.begin(); }
    CFastHashMap<SString, SCustomData>::const_iterator SyncedIterEnd() { return m_SyncedData.end(); }

private:
    bool DeleteSynced(const char* szName);
    void UpdateSynced(const char* szName, const CLuaArgument& Variable, bool bSynchronized);

    CFastHashMap<SString, SCustomData> m_Data;
    CFastHashMap<SString, SCustomData> m_SyncedData;
};
>>>>>>> 55b22ca230a22c71693ca4b90825ca94c486efd5
