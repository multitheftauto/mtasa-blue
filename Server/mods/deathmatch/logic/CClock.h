/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClock.h
 *  PURPOSE:     Game clock class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CClock
{
public:
    CClock();

    void Get(std::uint8_t& ucHour, std::uint8_t& ucMinute);
    void Set(std::uint8_t ucHour, std::uint8_t ucMinute);

    void          SetMinuteDuration(std::uint32_t ulDuration);
    std::uint32_t GetMinuteDuration();

private:
    std::uint64_t m_ullMidnightTime;
    std::uint32_t      m_ulMinuteDuration;
};
