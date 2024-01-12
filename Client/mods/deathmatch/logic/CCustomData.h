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

    const SCustomData* Get(const SString& strName);
    SCustomDataResult Set(SString&& strName, CLuaArgument&& Variable, bool bSynchronized = true, SCustomData* oldValue = {});

    bool Delete(const SString& strName, SCustomData* pOldData = {});

    const std::unordered_map<SString, SCustomData>& GetData() const { return m_Data; }

private:
    std::unordered_map<SString, SCustomData> m_Data;
};
