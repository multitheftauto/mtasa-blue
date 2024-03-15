/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStatManager.h
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include "CPerfStatModule.h"

//
// CPerfStatManager
//
class CPerfStatManager
{
public:
    virtual ~CPerfStatManager() {}

    virtual void DoPulse() = 0;
    virtual void GetStats(CPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter) = 0;
    virtual void Stop() = 0;

    // Utility
    static SString   GetScaledByteString(std::int64_t Amount);
    static SString   GetScaledBitString(std::int64_t Amount);
    static SString   GetScaledFloatString(float fValue);
    static std::int64_t GetPerSecond(std::int64_t llValue, std::int64_t llDeltaTickCount);
    static void      ToPerSecond(std::int64_t& llValue, std::int64_t llDeltaTickCount);
    static SString   GetPerSecondString(std::int64_t llValue, double dDeltaTickCount);
    static SString   GetPercentString(std::int64_t llValue, std::int64_t llTotal);

    static CPerfStatManager* GetSingleton();
};
