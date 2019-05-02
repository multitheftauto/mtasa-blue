/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CCustomData.h
 *  PURPOSE:     Custom data storage class header
 *
 *****************************************************************************/

#pragma once

#include "SharedUtil.FastHashMap.h"
#include "lua/CLuaArgument.h"

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
    bool         Set(const char* szName, const CLuaArgument& Variable, bool bSynchronized = true, CLuaArgument* pOldVariable = nullptr);

    bool Delete(const char* szName);

    CFastHashMap<SString, SCustomData>::const_iterator IterBegin() { return m_Data.begin(); }
    CFastHashMap<SString, SCustomData>::const_iterator IterEnd() { return m_Data.end(); }

private:
    CFastHashMap<SString, SCustomData> m_Data;
};
