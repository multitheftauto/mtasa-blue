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

    std::map<std::string, SCustomData>::const_iterator IterBegin() { return m_Data.begin(); }
    std::map<std::string, SCustomData>::const_iterator IterEnd() { return m_Data.end(); }

    std::map<std::string, SCustomData>::const_iterator SyncedIterBegin() { return m_SyncedData.begin(); }
    std::map<std::string, SCustomData>::const_iterator SyncedIterEnd() { return m_SyncedData.end(); }

private:
    bool DeleteSynced(const char* szName);
    void UpdateSynced(const char* szName, const CLuaArgument& Variable, bool bSynchronized);

    std::map<std::string, SCustomData> m_Data;
    std::map<std::string, SCustomData> m_SyncedData;
};
