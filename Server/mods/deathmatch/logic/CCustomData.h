/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCustomData.h
 *  PURPOSE:     Custom entity data class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
};

enum class eCustomDataClientTrust : std::uint8_t
{
    UNSET,
    ALLOW,
    DENY,
};

struct SCustomData
{
    CLuaArgument           Variable;
    ESyncType              syncType;
    eCustomDataClientTrust clientChangesMode;
};

class CCustomData
{
public:
    void Copy(CCustomData* pCustomData);

    SCustomData* Get(CStringName name) const;
    SCustomData* GetSynced(CStringName name);
    void         Set(CStringName name, const CLuaArgument& Variable, ESyncType syncType = ESyncType::BROADCAST);

    bool Delete(CStringName name);

    void SetClientChangesMode(CStringName name, eCustomDataClientTrust mode);

    unsigned short CountOnlySynchronized();

    CXMLNode* OutputToXML(CXMLNode* pNode);

    std::unordered_map<CStringName, SCustomData>::const_iterator IterBegin() { return m_Data.begin(); }
    std::unordered_map<CStringName, SCustomData>::const_iterator IterEnd() { return m_Data.end(); }

    std::unordered_map<CStringName, SCustomData>::const_iterator SyncedIterBegin() { return m_SyncedData.begin(); }
    std::unordered_map<CStringName, SCustomData>::const_iterator SyncedIterEnd() { return m_SyncedData.end(); }

private:
    bool DeleteSynced(CStringName name);
    void UpdateSynced(CStringName name, const CLuaArgument& Variable, ESyncType syncType);

    std::unordered_map<CStringName, SCustomData> m_Data;
    std::unordered_map<CStringName, SCustomData> m_SyncedData;
};
