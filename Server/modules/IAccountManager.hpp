#pragma once

#include "IAccount.hpp"

#include "../../Shared/sdk/SharedUtil.DynamicArray.h"

using SharedUtil::DynamicArray;

struct IAccountManager
{
    virtual IAccount* Get(const char* szName, const char* szPassword = nullptr, bool caseSensitive = true) const noexcept = 0;

    virtual DynamicArray<IAccount*>             GetAllAccounts() const noexcept = 0;
    virtual DynamicArray<IAccount*>             GetAccountsBySerial(const SString& strSerial) const noexcept = 0;
    virtual DynamicArray<IAccount*>             GetAccountsByIP(const SString& strIP) const noexcept = 0;
    virtual IAccount*                           GetAccountByID(const int ID) const noexcept = 0;
    virtual DynamicArray<IAccount*>             GetAccountsByData(const SString& dataName, const SString& value) const noexcept = 0;
};
