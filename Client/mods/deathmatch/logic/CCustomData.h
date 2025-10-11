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

#include "lua/CLuaArgument.h"
#include "CStringName.h"

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

    SCustomData* Get(const CStringName& name);
    void         Set(const CStringName& name, const CLuaArgument& Variable, bool bSynchronized = true);

    bool Delete(const CStringName& name);

    std::unordered_map<CStringName, SCustomData>::const_iterator IterBegin() { return m_Data.begin(); }
    std::unordered_map<CStringName, SCustomData>::const_iterator IterEnd() { return m_Data.end(); }

private:
    std::unordered_map<CStringName, SCustomData> m_Data;
};
