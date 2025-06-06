/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.MtaVersion.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <string>
#include "SString.h"

namespace SharedUtil
{
    //
    // Container for MTA version string. e.g. "1.5.6-9.18123.0"
    //
    class CMtaVersion : public std::string
    {
    public:
        // Constructors like SString
        CMtaVersion() : std::string() {}
        CMtaVersion(const char* szText) : std::string(szText ? szText : "") {}
        CMtaVersion(const std::string& strString) : std::string(strString) {}

        // Assignment like SString
                    operator const char*() const { return c_str(); }            // Auto assign to const char* without using c_str()
        const char* operator*() const { return c_str(); }

        // Compare disregarding build type
        bool operator==(const char* other) const { return operator==(CMtaVersion(other)); }
        bool operator==(const CMtaVersion& other) const { return GetVersionStringUsingBuildType() == other.GetVersionStringUsingBuildType(); }

        bool operator>(const char* other) const { return operator>(CMtaVersion(other)); }
        bool operator>(const CMtaVersion& other) const { return GetVersionStringUsingBuildType() > other.GetVersionStringUsingBuildType(); }

        bool operator<(const char* other) const { return operator<(CMtaVersion(other)); }
        bool operator<(const CMtaVersion& other) const { return GetVersionStringUsingBuildType() < other.GetVersionStringUsingBuildType(); }

        // Return version as a string with MTASA_VERSION_TYPE replaced
        SString GetVersionStringUsingBuildType(int buildType = 9) const
        {
            SString result = *this;
            if (result.length() > 6)
                result.at(6) = '0' + buildType;
            return result;
        }

        // Extract MTASA_VERSION_BUILD (0 to 99999)
        int GetBuildNumber() const { return atoi(SStringX(*this).SubStr(8)); }

        // Extract MTASA_VERSION_MAJOR.MTASA_VERSION_MINOR e.g. '1.5'
        SString GetGeneration() const { return SStringX(*this).SubStr(0, 3); }
    };

}            // namespace SharedUtil
