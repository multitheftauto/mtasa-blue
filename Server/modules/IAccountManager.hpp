#pragma once

#include "IAccount.hpp"

#include <vector>

class IAccountManager
{
public:
    virtual IAccount* Get(const char* szName, const char* szPassword = nullptr, bool caseSensitive = true) const noexcept = 0;

    virtual std::vector<IAccount*> GetAllAccounts() const noexcept = 0;
    virtual std::vector<IAccount*> GetAccountsBySerial(const SString& strSerial) const noexcept = 0;
    virtual std::vector<IAccount*> GetAccountsByIP(const SString& strIP) const noexcept = 0;
    virtual IAccount*              GetAccountByID(const int ID) const noexcept = 0;
    virtual std::vector<IAccount*> GetAccountsByData(const SString& dataName, const SString& value) const noexcept = 0;
};
