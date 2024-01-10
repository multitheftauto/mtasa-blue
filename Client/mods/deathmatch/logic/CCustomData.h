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

#define MAX_CUSTOMDATA_NAME_LENGTH 128

struct SCustomData
{
    CLuaArgument Variable;
    bool         bSynchronized{true};
};

class CCustomData
{
public:
    void Copy(CCustomData* pCustomData);

    SCustomData* Get(const SString& strName, bool bCreate = false);
    bool         Set(const SString& strName, const CLuaArgument& Variable, bool bSynchronized = true, SCustomData* pOldData = {});

    bool Delete(const SString& strName, SCustomData* pOldData = {});

    const std::unordered_map<SString, SCustomData>& GetData() const { return m_Data; }

private:
    std::unordered_map<SString, SCustomData> m_Data;
};
