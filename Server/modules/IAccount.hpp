#pragma once

#include "../../Shared/sdk/SString.h"

struct IAccount
{
    virtual const SString& GetName() const noexcept = 0;
    virtual void           SetName(const std::string& strName) noexcept = 0;

    virtual void    SetPassword(const SString& strPassword) noexcept = 0;
    virtual bool    IsPassword(const SString& strPassword, bool* pbUsedHttpPassAppend = nullptr) const noexcept = 0;
    virtual SString GetPasswordHash() const noexcept = 0;

    virtual const std::string& GetIP() const noexcept = 0;
    virtual const std::string& GetSerial() const noexcept = 0;
    virtual const int          GetID() const noexcept = 0;
};
