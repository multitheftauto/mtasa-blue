/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.String.h
 *  PURPOSE:     String utilities
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <string>

namespace SharedUtil
{
    inline bool StartsWith(const std::string& what, const std::string& with) noexcept
    {
        return !what.rfind(with, 0);
    }

    inline bool EndsWith(const std::string& what, const std::string& with) noexcept
    {
        return !what.compare(what.size() - with.size(), with.size(), with);
    }
}
