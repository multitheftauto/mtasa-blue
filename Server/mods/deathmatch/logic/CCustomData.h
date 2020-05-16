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

#include "SString.h"
#include "SharedUtil.FastHashMap.h"
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
    CLuaArgument variable{};
    ESyncType    syncType = ESyncType::BROADCAST;

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
        if (this != &rhs)
        {
            variable = rhs.variable;
            syncType = rhs.syncType;
        }
        return *this;
    }

    SCustomData& operator=(SCustomData&& rhs) noexcept
    {
        if (this != &rhs)
        {
            variable = std::move(rhs.variable);
            syncType = rhs.syncType;
        }
        return *this;
    }

    // i know its silly we need to pass in the data name, but.. yeah, i have no better solution
    // its still more maintable than the current code
    void WriteToBitStream(NetBitStreamInterface& bitstream, const SString& dataName) const
    {
        const unsigned short usNameLength = static_cast<unsigned short>(dataName.length());
        bitstream.WriteCompressed(usNameLength);
        bitstream.Write(dataName.c_str(), usNameLength);
        variable.WriteToBitStream(bitstream);
    }
};

// doing 'using' here, because of SCustomData
using element_data_map = CFastHashMap<SString, SCustomData>;
using element_data_iter = element_data_map::iterator;
using element_data_const_iter = element_data_map::const_iterator;

class CCustomData
{
public:
    void Copy(const CCustomData* const from);

    // the public versions of the private functions
    const SCustomData* Get(const SString& name) { return Get(name, nullptr); }
    const SCustomData* Get(const SString& name, const ESyncType syncType) { return Get(name, syncType, nullptr); } 

    // Returns the(maybe newly created) pointer to the SCustomData, if modified, otherwise nullptr
    const SCustomData*  Set(const SString& name, const CLuaArgument& newValue, const ESyncType newSyncType, SCustomData* const oldData = nullptr);

    // Returns the pointer to the (maybe newly created) SCustomData, if modified, otherwise nullptr.
    const SCustomData* Set(const SString& name, const CLuaArgument& newValue, SCustomData* const oldData = nullptr);

    bool Delete(const SString& name, SCustomData* const oldData = nullptr);
    bool Delete(const SString& name, const ESyncType syncType, SCustomData* const oldData);

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
