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
#include "SharedUtil.FastHashMap.h"
#include <string>

#define MAX_CUSTOMDATA_NAME_LENGTH 128

enum class ESyncType : unsigned char
{
    LOCAL,
    BROADCAST,
    SUBSCRIBE,
};

struct SCustomData
{
    CLuaArgument Variable;
    ESyncType    syncType;
};

class CCustomData
{
public:
    SCustomData* Get(const std::string& name) { return MapFind(m_Data, name); }

    bool Delete(const std::string& name);

    CXMLNode* OutputToXML(CXMLNode* pNode);

    /*
    std::map<std::string, SCustomData>::const_iterator IterBegin() { return m_Data.begin(); }
    std::map<std::string, SCustomData>::const_iterator IterEnd() { return m_Data.end(); }
    */
private:
    SharedUtil::CFastHashMap<std::string, SCustomData> m_Data;
};
