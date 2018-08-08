/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTimeCycleSA.cpp
 *  PURPOSE:     Time cycle logic
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

template <typename T>
class CTimeCycleProperty : public CTimeCyclePropertyInterface
{
public:
    CTimeCycleProperty(T* tAddress, bool bDecimal = false)
    {
        tMemoryAddress  = tAddress;
        bIsDecimal      = bDecimal;
        bOriginalSaved  = false;
    }
    ~CTimeCycleProperty()
    {
        if (bOriginalSaved)
        {
            // deallocate memory block of original values
            free(tOriginalValues);
        }
    }

    bool ResetValue(int iWeatherID = -1, int iCycleNum = -1)
    {
        if (!bOriginalSaved) return false; // there is nothing to reset
        if (iWeatherID == -1 && iCycleNum == -1)
        {
            // restore whole memory block
            MemCpyFast((void*)tMemoryAddress, tOriginalValues, sizeof(T) * 184);
        }
        else
        {
            int iOffset = iWeatherID + iCycleNum * 23;
            MemPutFast<T>((void*)(tMemoryAddress + iOffset), tOriginalValues[iOffset]);
        }
        return true;
    }
    bool SetValue(float fValue, int iWeatherID = -1, int iCycleNum = -1)
    {
        if (!bOriginalSaved)
        {
            bOriginalSaved = true;
            size_t blockSize = sizeof(T) * 184;
            // allocate memory block for original values
            tOriginalValues = static_cast<T*>(malloc(blockSize));
            // save original values
            MemCpyFast(tOriginalValues, (void*)tMemoryAddress, blockSize);
        }

        // prepare the value
        if (bIsDecimal) fValue *= 10;
        fValue = std::max((int)fValue, 0);                                  // >= 0
        fValue = std::min((int)fValue, (int)std::numeric_limits<T>::max()); // <= maximum possible number

        if (iWeatherID == -1 && iCycleNum == -1)
        {
            // set value to a whole memory block
            T tValues[184];
            std::fill_n(tValues, 184, fValue);
            MemCpyFast((void*)tMemoryAddress, &tValues, sizeof(tValues));
        }
        else
        {
            MemPutFast<T>((void*)(tMemoryAddress + iWeatherID + iCycleNum * 23), fValue);
        }
        return true;
    }
    float GetValue(int iWeatherID, int iCycleNum)
    {
        float fValue = *(tMemoryAddress + iWeatherID + iCycleNum * 23);
        if (bIsDecimal) fValue *= 0.1;
        return fValue;
    }
    float GetOriginalValue(int iWeatherID, int iCycleNum)
    {
        if (bOriginalSaved)
        {
            float fValue = tOriginalValues[iWeatherID + iCycleNum * 23];
            if (bIsDecimal) fValue *= 0.1;
            return fValue;
        }
        return GetValue(iWeatherID, iCycleNum);
    }
private:
    T * tMemoryAddress;
    T*   tOriginalValues;
    bool bOriginalSaved;
    bool bIsDecimal;
};

CTimeCycleSA::CTimeCycleSA()
{
    // create time cycle properties
    m_TimeCycleProps[TIMECYCLE_AMBIENT_RED]             = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_AMBRedPool);
    m_TimeCycleProps[TIMECYCLE_AMBIENT_GREEN]           = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_AMBGreenPool);
    m_TimeCycleProps[TIMECYCLE_AMBIENT_BLUE]            = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_AMBBluePool);
    m_TimeCycleProps[TIMECYCLE_AMBIENT_OBJ_RED]         = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_AMB_OBJRedPool);
    m_TimeCycleProps[TIMECYCLE_AMBIENT_OBJ_GREEN]       = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_AMB_OBJGreenPool);
    m_TimeCycleProps[TIMECYCLE_AMBIENT_OBJ_BLUE]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_AMB_OBJBluePool);
    m_TimeCycleProps[TIMECYCLE_SKY_TOP_RED]             = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SkyTopRedPool);
    m_TimeCycleProps[TIMECYCLE_SKY_TOP_GREEN]           = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SkyTopGreenPool);
    m_TimeCycleProps[TIMECYCLE_SKY_TOP_BLUE]            = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SkyTopBluePool);
    m_TimeCycleProps[TIMECYCLE_SKY_BOTTOM_RED]          = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SkyBotRedPool);
    m_TimeCycleProps[TIMECYCLE_SKY_BOTTOM_GREEN]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SkyBotGreenPool);
    m_TimeCycleProps[TIMECYCLE_SKY_BOTTOM_BLUE]         = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SkyBotBluePool);
    m_TimeCycleProps[TIMECYCLE_SUN_CORE_RED]            = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SunCoreRedPool);
    m_TimeCycleProps[TIMECYCLE_SUN_CORE_GREEN]          = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SunCoreGreenPool);
    m_TimeCycleProps[TIMECYCLE_SUN_CORE_BLUE]           = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SunCoreBluePool);
    m_TimeCycleProps[TIMECYCLE_SUN_CORONA_RED]          = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SunCoronaRedPool);
    m_TimeCycleProps[TIMECYCLE_SUN_CORONA_GREEN]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SunCoronaGreenPool);
    m_TimeCycleProps[TIMECYCLE_SUN_CORONA_BLUE]         = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SunCoronaBluePool);
    m_TimeCycleProps[TIMECYCLE_SUN_SIZE]                = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SunszPool, true);
    m_TimeCycleProps[TIMECYCLE_SPRITE_SIZE]             = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SprszPool, true);
    m_TimeCycleProps[TIMECYCLE_SPRITE_BRIGHTNESS]       = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_SprzbghtPool, true);
    m_TimeCycleProps[TIMECYCLE_SHADOW_STRENGTH]         = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ShdwPool);
    m_TimeCycleProps[TIMECYCLE_LIGHT_SHADOW_STRENGTH]   = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_LightShdPool);
    m_TimeCycleProps[TIMECYCLE_POLE_SHADOW_STRENGTH]    = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_PoleShdPool);
    m_TimeCycleProps[TIMECYCLE_FAR_CLIP]                = new CTimeCycleProperty<WORD>((WORD*)VAR_CTimeCyc_FarClpPool);
    m_TimeCycleProps[TIMECYCLE_FOG_START]               = new CTimeCycleProperty<WORD>((WORD*)VAR_CTimeCyc_FogStPool);
    m_TimeCycleProps[TIMECYCLE_LIGHTS_ON_GROUND]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_LightOnGroundPool, true);
    m_TimeCycleProps[TIMECYCLE_LOW_CLOUDS_RED]          = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_LowCloudsRedPool);
    m_TimeCycleProps[TIMECYCLE_LOW_CLOUDS_GREEN]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_LowCloudsGreenPool);
    m_TimeCycleProps[TIMECYCLE_LOW_CLOUDS_BLUE]         = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_LowCloudsBluePool);
    m_TimeCycleProps[TIMECYCLE_BOTTOM_CLOUDS_RED]       = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_BottomCloudsRedPool);
    m_TimeCycleProps[TIMECYCLE_BOTTOM_CLOUDS_GREEN]     = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_BottomCloudsGreenPool);
    m_TimeCycleProps[TIMECYCLE_BOTTOM_CLOUDS_BLUE]      = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_BottomCloudsBluePool);
    m_TimeCycleProps[TIMECYCLE_WATER_RED]               = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_WaterRedPool);
    m_TimeCycleProps[TIMECYCLE_WATER_GREEN]             = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_WaterGreenPool);
    m_TimeCycleProps[TIMECYCLE_WATER_BLUE]              = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_WaterBluePool);
    m_TimeCycleProps[TIMECYCLE_WATER_ALPHA]             = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_WaterAlphaPool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_CORRECT_RED]      = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourCorrectRedPool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_CORRECT_GREEN]    = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourCorrectGreenPool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_CORRECT_BLUE]     = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourCorrectBluePool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_CORRECT_ALPHA]    = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourCorrectAlphaPool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_ADD_RED]          = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourAddRedPool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_ADD_GREEN]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourAddGreenPool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_ADD_BLUE]         = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourAddBluePool);
    m_TimeCycleProps[TIMECYCLE_COLOUR_ADD_ALPHA]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_ColourAddAlphaPool);
    m_TimeCycleProps[TIMECYCLE_LOW_CLOUDS_ALPHA]        = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_LowCloudsAlphaPool);
    m_TimeCycleProps[TIMECYCLE_UPPER_CLOUDS_ALPHA]      = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_UpperCloudsAlphaPool);
    m_TimeCycleProps[TIMECYCLE_ILLUMINATION]            = new CTimeCycleProperty<BYTE>((BYTE*)VAR_CTimeCyc_IlluminationPool);
}

CTimeCycleSA::~CTimeCycleSA(void)
{
    // remove created time cycle properties
    std::map<std::string, CTimeCyclePropertyInterface*>::iterator it;
    for (it = m_TimeCycleProps.begin(); it != m_TimeCycleProps.end(); it++)
    {
        delete it->second;
    }
}

bool CTimeCycleSA::ResetTimeCycle()
{
    std::map<std::string, CTimeCyclePropertyInterface*>::iterator it;
    for (it = m_TimeCycleProps.begin(); it != m_TimeCycleProps.end(); it++)
    {
        it->second->ResetValue();
    }
    return true;
}

bool CTimeCycleSA::SetTimeCycle(const char* szPropertyName, float fValue, int iWeatherID, int iCycleNum)
{
    if (m_TimeCycleProps.count(szPropertyName) != 1) return false; // property is not found
    bool bResetValue = fValue == -1.0; // -1 is reset

    if (iWeatherID == -1 && iCycleNum == -1) // set for all weathers and cycles
    {
        if (bResetValue)
            m_TimeCycleProps[szPropertyName]->ResetValue();
        else
            m_TimeCycleProps[szPropertyName]->SetValue(fValue);
    }
    else if (iWeatherID == -1) // set for all weathers
    {
        assert(iCycleNum >= 0 && iCycleNum <= 7);
        for (iWeatherID = 0; iWeatherID < 24; iWeatherID++)
        {
            if (bResetValue)
                m_TimeCycleProps[szPropertyName]->ResetValue(iWeatherID, iCycleNum);
            else
                m_TimeCycleProps[szPropertyName]->SetValue(fValue, iWeatherID, iCycleNum);
        }
    }
    else if (iCycleNum == -1) // set for all cycles of the weather
    {
        assert(iWeatherID >= 0 && iWeatherID <= 23);
        for (iCycleNum = 0; iCycleNum < 8; iCycleNum++)
        {
            if (bResetValue)
                m_TimeCycleProps[szPropertyName]->ResetValue(iWeatherID, iCycleNum);
            else
                m_TimeCycleProps[szPropertyName]->SetValue(fValue, iWeatherID, iCycleNum);
        }
    }
    else // set for the cycle of the weather
    {
        assert(iWeatherID >= 0 && iWeatherID <= 23);
        assert(iCycleNum >= 0 && iCycleNum <= 7);
        if (bResetValue)
            m_TimeCycleProps[szPropertyName]->ResetValue(iWeatherID, iCycleNum);
        else
            m_TimeCycleProps[szPropertyName]->SetValue(fValue, iWeatherID, iCycleNum);
    }
    return true;
}

float CTimeCycleSA::GetTimeCycle(int iWeatherID, int iCycleNum, const char* szPropertyName)
{
    if (m_TimeCycleProps.count(szPropertyName) != 1) return -1; // property is not found
    assert(iWeatherID >= 0 && iWeatherID <= 23);
    assert(iCycleNum >= 0 && iCycleNum <= 7);
    return m_TimeCycleProps[szPropertyName]->GetValue(iWeatherID, iCycleNum);
}

void CTimeCycleSA::GetTimeCycle(int iWeatherID, int iCycleNum, std::map<const char*, float>& m_Properties)
{
    assert(iWeatherID >= 0 && iWeatherID <= 23);
    assert(iCycleNum >= 0 && iCycleNum <= 7);

    std::map<std::string, CTimeCyclePropertyInterface*>::iterator it;
    for (it = m_TimeCycleProps.begin(); it != m_TimeCycleProps.end(); it++)
    {
        m_Properties[it->first.c_str()] = it->second->GetValue(iWeatherID, iCycleNum);
    }
}

void CTimeCycleSA::GetTimeCycle(int iWeatherID, std::map<int, std::map<const char*, float>>& m_Properties)
{
    assert(iWeatherID >= 0 && iWeatherID <= 23);

    std::map<std::string, CTimeCyclePropertyInterface*>::iterator it;
    for (it = m_TimeCycleProps.begin(); it != m_TimeCycleProps.end(); it++)
    {
        for (int iCycleNum = 0; iCycleNum < 8; iCycleNum++)
        {
            m_Properties[iCycleNum][it->first.c_str()] = it->second->GetValue(iWeatherID, iCycleNum);
        }
    }
}

float CTimeCycleSA::GetOriginalTimeCycle(int iWeatherID, int iCycleNum, const char* szPropertyName)
{
    if (m_TimeCycleProps.count(szPropertyName) != 1) return -1; // property is not found
    assert(iWeatherID >= 0 && iWeatherID <= 23);
    assert(iCycleNum >= 0 && iCycleNum <= 7);
    return m_TimeCycleProps[szPropertyName]->GetValue(iWeatherID, iCycleNum);
}

void CTimeCycleSA::GetOriginalTimeCycle(int iWeatherID, int iCycleNum, std::map<const char*, float>& m_Properties)
{
    assert(iWeatherID >= 0 && iWeatherID <= 23);
    assert(iCycleNum >= 0 && iCycleNum <= 7);

    std::map<std::string, CTimeCyclePropertyInterface*>::iterator it;
    for (it = m_TimeCycleProps.begin(); it != m_TimeCycleProps.end(); it++)
    {
        m_Properties[it->first.c_str()] = it->second->GetOriginalValue(iWeatherID, iCycleNum);
    }
}

void CTimeCycleSA::GetOriginalTimeCycle(int iWeatherID, std::map<int, std::map<const char*, float>>& m_Properties)
{
    assert(iWeatherID >= 0 && iWeatherID <= 23);

    std::map<std::string, CTimeCyclePropertyInterface*>::iterator it;
    for (it = m_TimeCycleProps.begin(); it != m_TimeCycleProps.end(); it++)
    {
        for (int iCycleNum = 0; iCycleNum < 8; iCycleNum++)
        {
            m_Properties[iCycleNum][it->first.c_str()] = it->second->GetOriginalValue(iWeatherID, iCycleNum);
        }
    }
}
