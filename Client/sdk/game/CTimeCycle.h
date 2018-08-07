/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CTimeCycle.h
 *  PURPOSE:     Time cycle interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CGAME_TIMECYCLE
#define __CGAME_TIMECYCLE

#include "Common.h"

class CTimeCyclePropertyInterface
{
public:
    virtual bool  ResetValue(int iWeatherID = -1, int iCycleNum = -1) = 0;
    virtual bool  SetValue(float fValue, int iWeatherID = -1, int iCycleNum = -1) = 0;
    virtual float GetValue(int iWeatherID, int iCycleNum) = 0;
    virtual float GetOriginalValue(int iWeatherID, int iCycleNum) = 0;
};

class CTimeCycle
{
public:
    virtual bool    ResetTimeCycle() = 0;
    virtual bool    SetTimeCycle(const char* szPropertyName, float fValue, int iWeatherID, int iCycleNum) = 0;
    virtual float   GetTimeCycle(int iWeatherID, int iCycleNum, const char* szPropertyName) = 0;
    virtual void    GetTimeCycle(int iWeatherID, int iCycleNum, std::map<const char*, float>& m_Properties) = 0;
    virtual void    GetTimeCycle(int iWeatherID, std::map<int, std::map<const char*, float>>& m_Properties) = 0;
    virtual float   GetOriginalTimeCycle(int iWeatherID, int iCycleNum, const char* szPropertyName) = 0;
    virtual void    GetOriginalTimeCycle(int iWeatherID, int iCycleNum, std::map<const char*, float>& m_Properties) = 0;
    virtual void    GetOriginalTimeCycle(int iWeatherID, std::map<int, std::map<const char*, float>>& m_Properties) = 0;
};

#endif
