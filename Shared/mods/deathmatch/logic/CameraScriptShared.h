/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/CameraScriptShared.h
 *  PURPOSE:     Shared helpers for camera scripting sanitization
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <cmath>

namespace CameraScriptShared
{
    constexpr float kDefaultFOV = 70.0f;
    constexpr float kMaxFOV = 179.0f;
    constexpr float kFullRotationDegrees = 360.0f;

    inline bool IsFiniteVector(const CVector& vec) noexcept
    {
        return std::isfinite(vec.fX) && std::isfinite(vec.fY) && std::isfinite(vec.fZ);
    }

    inline float SanitizeFOV(float fov) noexcept
    {
        if (!std::isfinite(fov) || fov <= 0.0f)
            return kDefaultFOV;
        if (fov >= 180.0f)
            return kMaxFOV;
        return fov;
    }

    inline float NormalizeRoll(float rollDegrees) noexcept
    {
        if (!std::isfinite(rollDegrees))
            return 0.0f;

        float normalized = std::remainder(rollDegrees, kFullRotationDegrees);
        if (normalized < 0.0f)
            normalized += kFullRotationDegrees;
        return normalized;
    }
}
