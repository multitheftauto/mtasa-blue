/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CStringMap.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <map>
#include <string>
#include "SString.h"
//
// String map with parsing helpers like CScriptArgReader
//
template <class V>
class CStringMapReader : public std::map<SString, V>
{
public:
    void ReadString(const SString& strKeyName, SString& strOutValue, const char* szDefaultValue)
    {
        V* pstrValue = MapFind(*this, strKeyName);

        if (pstrValue)
            strOutValue = *pstrValue;
        else
            strOutValue = szDefaultValue;
    }

    template <typename T, typename U>
    void ReadNumber(const SString& strKeyName, T& outValue, const U& defaultValue)
    {
        V* pstrValue = MapFind(*this, strKeyName);
        if (pstrValue)
        {
            std::stringstream ss(*pstrValue);
            if (ss >> outValue)
                return;
        }
        outValue = defaultValue;
    }

    void ReadBool(const SString& strKeyName, bool& bOutValue, const bool bDefaultValue) { ReadNumber(strKeyName, bOutValue, bDefaultValue); }

    template <typename T>
    void ReadStringMap(const SString& strKeyName, T& outMap)
    {
        V* pstrValue = MapFind(*this, strKeyName);
        if (pstrValue)
            outMap.insert(pstrValue->subMap.begin(), pstrValue->subMap.end());
    }
};

//
// String map value which can also contain a submap
//
class SStringMapValue : public SString
{
public:
    SStringMapValue(const char* szText = nullptr) : SString(std::string(szText ? szText : "")) {}
    CStringMapReader<SStringMapValue> subMap;
};

using CStringMap = CStringMapReader<SStringMapValue>;
