/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CCVarsInterface.h
 *  PURPOSE:     Core client variables interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

class CCVarsInterface
{
public:
    // Existance check
    virtual bool Exists(const std::string& strVariable) = 0;

    // Get queries
    virtual bool Get(const std::string& strVariable, bool& val) = 0;
    virtual bool Get(const std::string& strVariable, std::string& val) = 0;
    virtual bool Get(const std::string& strVariable, int& val) = 0;
    virtual bool Get(const std::string& strVariable, unsigned int& val) = 0;
    virtual bool Get(const std::string& strVariable, float& val) = 0;

    // Set queries
    virtual void Set(const std::string& strVariable, bool val) = 0;
    virtual void Set(const std::string& strVariable, const std::string& val) = 0;
    virtual void Set(const std::string& strVariable, int val) = 0;
    virtual void Set(const std::string& strVariable, unsigned int val) = 0;
    virtual void Set(const std::string& strVariable, float val) = 0;

    // Get without needing caller temp variable
    template <class T>
    T GetValue(const std::string& strVariable, const T& defaultValue = 0)
    {
        T value = defaultValue;
        Get(strVariable, value);
        return value;
    }
};
